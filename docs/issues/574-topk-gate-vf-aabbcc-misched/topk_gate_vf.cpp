/**
 * topk_gate_vf — 1:1 CCE intrinsic translation of topk_gate_asc.py.
 *
 * This is a TRUE 1:1 translation using raw CCE vector intrinsics inside
 * __VEC_SCOPE__, NOT TROWARGMAX. It mirrors the asc.py algorithm exactly:
 *
 *   asc.py: S.vmax + S.vcmax     →  CCE: vmax + vcmax (element-wise + cross-lane reduce)
 *   asc.py: S.vcmp + S.vsel      →  CCE: vcmp_eq + vsel (mask + select)
 *   asc.py: S.vmin + S.vcmin     →  CCE: vmin + vcmin (element-wise + cross-lane reduce)
 *   asc.py: S.vdupv              →  CCE: vdup(POS_LOWEST) broadcasts lane 0 to all lanes
 *   asc.py: S.vld                →  CCE: vlds
 *   asc.py: S.vci                →  CCE: vci
 *   asc.py: S.vdup               →  CCE: vdup(scalar)
 *   asc.py: S.vsts(..., 'ONEPT_B32')  →  CCE: vsts(..., ONEPT_B32, ...)
 *   asc.py: S.mem_bar('VST_VLD') →  CCE: mem_bar(VST_VLD)
 *
 * Group logic (two paths, exactly like asc.py):
 *
 * **Single-group fast path (group=1, E ≤ 384):**
 *   1. Load scores from UB into vreg arrays ONCE:
 *        scores_vec[i] = vlds(scores_ub[i*64])
 *        index_vec[i]  = vci(i*64)
 *   2. Mask padding: scores_vec[i] = vsel(scores_vec[i], neg_inf, vcmp_lt(index_vec[i], num_exp))
 *   3. Per K iteration:
 *      a. acc_max = vmax(acc_max, scores_vec[i]) across all vregs
 *         mx = vdup(POS_LOWEST, vcmax(acc_max))  ← reduce + broadcast
 *      b. tmp_idx_vec[i] = vsel(index_vec[i], int_max, vcmp_eq(scores_vec[i], mx))
 *         acc_min_idx = vmin(acc_min_idx, tmp_idx_vec[i])
 *         idx = vdup(POS_LOWEST, vcmin(acc_min_idx))  ← reduce + broadcast
 *      c. vsts(out_ub[k], idx, ONEPT_B32)  ← store ONE element to UB
 *      d. scores_vec[i] = vsel(neg_inf, scores_vec[i], vcmp_eq(index_vec[i], idx))
 *         ← MASK IN VREG ARRAY, NOT UB! This avoids re-reading from UB and
 *           avoids memory barriers entirely. The entire K loop stays in PIPE_V.
 *
 * **Multi-group slow path (group>1, E > 384):**
 *   Per K iteration, per group:
 *     1. Re-load from UB: scores_vec[i] = vlds(scores_ub[offset])
 *     2. Compute max/argmax as above
 *     3. Mask winner BACK to UB: vsts(scores_ub[offset], scores_vec[i])
 *     4. mem_bar(VST_VLD)  ← needed because UB is written and re-read next K
 *
 * Key advantage over TROWARGMAX version:
 *   - Single-group path has NO V→S→V sync (no __tf__ scalar, no set_flag/wait_flag)
 *   - Entire K loop stays in PIPE_V → no DCCI issue
 *   - Winner masking is in vreg array, not UB → no memory barrier needed
 *
 * Template unrolling:
 *   CCE vector pipeline cannot handle dynamic vreg array indexing — all loops
 *   over vreg arrays must be fully unrolled at compile time. Template recursion
 *   (UnrollVmax<N>, UnrollVmin<N>, etc.) forces instantiation of each iteration
 *   as separate code, avoiding the "Unsupported Inst must be hoisted" backend error.
 *
 * Compile-time configurable via -DK_E / -DK_K / -DK_TOKEN_TILE / -DUSE_AABBCC.
 *
 * Outer schedule matches staged CCE (fair e2e compare; only VF compute differs):
 *   wave: TLOAD token_tile rows → wait MTE2→V → VF compute tile → wait V→MTE3
 *         → TSTORE token_tile outs → barrier
 *
 * Inputs:
 *   scores: __gm__ float[kN * kEAligned] — gate scores (padded to kEAligned)
 * Outputs:
 *   topk_idx: __gm__ int32_t[kN * kK] — selected expert indices per token
 */

#include "pto/pto-inst.hpp"

namespace topk_gate_vf {

// Configurable via compile flags for different benchmark configs
#ifndef K_E
#define K_E 64
#endif
#ifndef K_K
#define K_K 6
#endif

#ifndef K_N
#define K_N 4
#endif
#ifndef K_TOKEN_TILE
#define K_TOKEN_TILE 1
#endif
#ifndef USE_AABBCC
#define USE_AABBCC 0
#endif
constexpr int kN = K_N;           // tokens (sim-friendly; asc.py uses 512/8192)
constexpr int kE = K_E;          // experts (configurable)
constexpr int kK = K_K;          // top-K (configurable)
constexpr int kTokenTile = K_TOKEN_TILE;

// Derived constants — must match asc.py's logic exactly
constexpr int kElemsPerVreg = 64;     // 256B / 4B = 64 float32 per vreg
constexpr int kMaxVregsPerGroup = 6;  // asc.py: max_vregs_per_group = 6
constexpr int kNumVregsPerGroup =
    (kE < kMaxVregsPerGroup * kElemsPerVreg) ?
    ((kE + kElemsPerVreg - 1) / kElemsPerVreg) :
    kMaxVregsPerGroup;
constexpr int kExpertsPerGroup = kNumVregsPerGroup * kElemsPerVreg;
constexpr int kNumGroups = (kE + kExpertsPerGroup - 1) / kExpertsPerGroup;
constexpr int kEAligned = kNumGroups * kExpertsPerGroup;  // padded to group boundary
constexpr int kKAligned = ((kK + kElemsPerVreg - 1) / kElemsPerVreg) * kElemsPerVreg;
constexpr int kNumWaves = kN / kTokenTile;

static_assert(kTokenTile > 0 && kTokenTile <= kElemsPerVreg, "token_tile in 1..64");
static_assert(kN % kTokenTile == 0, "kN must be multiple of token_tile");
#if USE_AABBCC
static_assert(kNumGroups == 1, "VF AABBCC only for single-group E<=384");
static_assert(kTokenTile % 2 == 0, "VF AABBCC needs even token_tile");
#endif

// UB layout (same tiling as staged): scores[tile, E] then out[tile, K]
constexpr uint64_t kUbScores = 0x00000;
constexpr uint64_t kUbOut =
    kUbScores + (uint64_t)kTokenTile * kEAligned * sizeof(float);

#if defined(__DAV_VEC__)
using F32VecTile = pto::Tile<pto::TileType::Vec, float, 1, kEAligned, pto::BLayout::RowMajor, -1, -1>;
using I32VecTile = pto::Tile<pto::TileType::Vec, int32_t, 1, kKAligned, pto::BLayout::RowMajor, -1, -1>;

// ============================================================================
// Template helpers for compile-time loop unrolling.
// CCE vector pipeline cannot handle dynamic vreg array indexing — all loops
// over vreg arrays must be fully unrolled at compile time.
// ============================================================================

template <int N>
struct UnrollVmax {
    static PTO_INTERNAL void Run(pto::RegTensor<float> &acc, pto::RegTensor<float> *scores,
                                 pto::MaskReg &preg)
    {
        vmax(acc, acc, scores[N - 1], preg, MODE_ZEROING);
        UnrollVmax<N - 1>::Run(acc, scores, preg);
    }
};
template <> struct UnrollVmax<0> {
    static PTO_INTERNAL void Run(pto::RegTensor<float> &, pto::RegTensor<float> *,
                                 pto::MaskReg &) {}
};

template <int N>
struct UnrollVmin {
    static PTO_INTERNAL void Run(pto::RegTensor<int32_t> &acc, pto::RegTensor<int32_t> *tmp,
                                 pto::MaskReg &preg)
    {
        vmin(acc, acc, tmp[N - 1], preg, MODE_ZEROING);
        UnrollVmin<N - 1>::Run(acc, tmp, preg);
    }
};
template <> struct UnrollVmin<0> {
    static PTO_INTERNAL void Run(pto::RegTensor<int32_t> &, pto::RegTensor<int32_t> *,
                                 pto::MaskReg &) {}
};

template <int N>
struct UnrollLoad {
    static PTO_INTERNAL void Run(pto::RegTensor<float> *scores, pto::RegTensor<int32_t> *indices,
                                 __ubuf__ float *scoresPtr)
    {
        vlds(scores[N - 1], scoresPtr, (int32_t)((N - 1) * kElemsPerVreg), NORM);
        vci(indices[N - 1], (int32_t)((N - 1) * kElemsPerVreg));
        UnrollLoad<N - 1>::Run(scores, indices, scoresPtr);
    }
};
template <> struct UnrollLoad<0> {
    static PTO_INTERNAL void Run(pto::RegTensor<float> *, pto::RegTensor<int32_t> *,
                                 __ubuf__ float *) {}
};

template <int N>
struct UnrollMaskPad {
    static PTO_INTERNAL void Run(pto::RegTensor<float> *scores, pto::RegTensor<int32_t> *indices,
                                 pto::RegTensor<float> &neg_inf, pto::RegTensor<int32_t> &num_exp,
                                 pto::MaskReg &preg)
    {
        pto::MaskReg mask_pad;
        vcmp_lt(mask_pad, indices[N - 1], num_exp, preg);
        vsel(scores[N - 1], scores[N - 1], neg_inf, mask_pad);
        UnrollMaskPad<N - 1>::Run(scores, indices, neg_inf, num_exp, preg);
    }
};
template <> struct UnrollMaskPad<0> {
    static PTO_INTERNAL void Run(pto::RegTensor<float> *, pto::RegTensor<int32_t> *,
                                 pto::RegTensor<float> &, pto::RegTensor<int32_t> &,
                                 pto::MaskReg &) {}
};

template <int N>
struct UnrollMatchMax {
    static PTO_INTERNAL void Run(pto::RegTensor<int32_t> *tmp, pto::RegTensor<int32_t> *indices,
                                 pto::RegTensor<float> *scores, pto::RegTensor<float> &mx,
                                 pto::RegTensor<int32_t> &int_max, pto::MaskReg &preg)
    {
        pto::MaskReg mask_eq;
        vcmp_eq(mask_eq, scores[N - 1], mx, preg);
        vsel(tmp[N - 1], indices[N - 1], int_max, mask_eq);
        UnrollMatchMax<N - 1>::Run(tmp, indices, scores, mx, int_max, preg);
    }
};
template <> struct UnrollMatchMax<0> {
    static PTO_INTERNAL void Run(pto::RegTensor<int32_t> *, pto::RegTensor<int32_t> *,
                                 pto::RegTensor<float> *, pto::RegTensor<float> &,
                                 pto::RegTensor<int32_t> &, pto::MaskReg &) {}
};

template <int N>
struct UnrollMaskWinner {
    static PTO_INTERNAL void Run(pto::RegTensor<float> *scores, pto::RegTensor<int32_t> *indices,
                                 pto::RegTensor<float> &neg_inf, pto::RegTensor<int32_t> &idx,
                                 pto::MaskReg &preg)
    {
        pto::MaskReg mask_winner;
        vcmp_eq(mask_winner, indices[N - 1], idx, preg);
        vsel(scores[N - 1], neg_inf, scores[N - 1], mask_winner);
        UnrollMaskWinner<N - 1>::Run(scores, indices, neg_inf, idx, preg);
    }
};
template <> struct UnrollMaskWinner<0> {
    static PTO_INTERNAL void Run(pto::RegTensor<float> *, pto::RegTensor<int32_t> *,
                                 pto::RegTensor<float> &, pto::RegTensor<int32_t> &,
                                 pto::MaskReg &) {}
};

#if USE_AABBCC
// Interleave vmax across two tokens to hide latency (AA phase).
template <int N>
struct UnrollVmaxPair {
    static PTO_INTERNAL void Run(pto::RegTensor<float> &acc0, pto::RegTensor<float> &acc1,
                                 pto::RegTensor<float> *scores0, pto::RegTensor<float> *scores1,
                                 pto::MaskReg &preg)
    {
        vmax(acc0, acc0, scores0[N - 1], preg, MODE_ZEROING);
        vmax(acc1, acc1, scores1[N - 1], preg, MODE_ZEROING);
        UnrollVmaxPair<N - 1>::Run(acc0, acc1, scores0, scores1, preg);
    }
};
template <> struct UnrollVmaxPair<0> {
    static PTO_INTERNAL void Run(pto::RegTensor<float> &, pto::RegTensor<float> &,
                                 pto::RegTensor<float> *, pto::RegTensor<float> *,
                                 pto::MaskReg &) {}
};

template <int N>
struct UnrollVminPair {
    static PTO_INTERNAL void Run(pto::RegTensor<int32_t> &acc0, pto::RegTensor<int32_t> &acc1,
                                 pto::RegTensor<int32_t> *tmp0, pto::RegTensor<int32_t> *tmp1,
                                 pto::MaskReg &preg)
    {
        vmin(acc0, acc0, tmp0[N - 1], preg, MODE_ZEROING);
        vmin(acc1, acc1, tmp1[N - 1], preg, MODE_ZEROING);
        UnrollVminPair<N - 1>::Run(acc0, acc1, tmp0, tmp1, preg);
    }
};
template <> struct UnrollVminPair<0> {
    static PTO_INTERNAL void Run(pto::RegTensor<int32_t> &, pto::RegTensor<int32_t> &,
                                 pto::RegTensor<int32_t> *, pto::RegTensor<int32_t> *,
                                 pto::MaskReg &) {}
};

// Dual-token VF: shared index vregs; AABBCC-style K loop (vmax pair → vcmax×2 →
// match/vmin/vcmin×2 → store/mask×2).
__tf__ AICORE void topk_compute_vf_pair(
    F32VecTile::TileDType scores0Data,
    F32VecTile::TileDType scores1Data,
    I32VecTile::TileDType out0Data,
    I32VecTile::TileDType out1Data)
{
    __ubuf__ float *scores0Ptr = (__ubuf__ float *)__cce_get_tile_ptr(scores0Data);
    __ubuf__ float *scores1Ptr = (__ubuf__ float *)__cce_get_tile_ptr(scores1Data);
    __ubuf__ int32_t *out0Ptr = (__ubuf__ int32_t *)__cce_get_tile_ptr(out0Data);
    __ubuf__ int32_t *out1Ptr = (__ubuf__ int32_t *)__cce_get_tile_ptr(out1Data);

    __VEC_SCOPE__
    {
        pto::MaskReg preg_all = pset_b32(PAT_ALL);
        uint32_t sreg_one = 1;
        pto::MaskReg preg_one = pto::CreatePredicate<int32_t>(sreg_one);

        {
            pto::RegTensor<int32_t> vreg_zero;
            vbr(vreg_zero, 0);
            constexpr auto distNormClear =
                std::integral_constant<::DistVST, static_cast<::DistVST>(
                    pto::GetDistVst<int32_t, pto::DistVST::DIST_NORM>())>();
            for (uint16_t i = 0; i < (uint16_t)kKAligned; i += (uint16_t)kElemsPerVreg) {
                vsts(vreg_zero, out0Ptr, (int32_t)i, distNormClear, preg_all);
                vsts(vreg_zero, out1Ptr, (int32_t)i, distNormClear, preg_all);
            }
        }

        pto::RegTensor<float> vreg_neg_inf;
        pto::RegTensor<int32_t> vreg_int_max;
        pto::RegTensor<int32_t> vreg_num_exp;
        vdup(vreg_neg_inf, -1e30f, preg_all, MODE_ZEROING);
        vdup(vreg_int_max, (int32_t)0x7FFFFFFF, preg_all, MODE_ZEROING);
        vdup(vreg_num_exp, (int32_t)kE, preg_all, MODE_ZEROING);

        pto::RegTensor<float> scores0[kMaxVregsPerGroup];
        pto::RegTensor<float> scores1[kMaxVregsPerGroup];
        pto::RegTensor<int32_t> index_vec[kMaxVregsPerGroup];
        pto::RegTensor<int32_t> tmp0[kMaxVregsPerGroup];
        pto::RegTensor<int32_t> tmp1[kMaxVregsPerGroup];

        UnrollLoad<kNumVregsPerGroup>::Run(scores0, index_vec, scores0Ptr);
        UnrollLoad<kNumVregsPerGroup>::Run(scores1, index_vec, scores1Ptr);
        UnrollMaskPad<kNumVregsPerGroup>::Run(scores0, index_vec, vreg_neg_inf, vreg_num_exp, preg_all);
        UnrollMaskPad<kNumVregsPerGroup>::Run(scores1, index_vec, vreg_neg_inf, vreg_num_exp, preg_all);

        pto::RegTensor<float> acc0, acc1, mx0, mx1;
        pto::RegTensor<int32_t> amin0, amin1, idx0, idx1;

        for (uint16_t k = 0; k < (uint16_t)kK; ++k) {
            // AA: interleaved vmax trees
            vdup(acc0, -1e30f, preg_all, MODE_ZEROING);
            vdup(acc1, -1e30f, preg_all, MODE_ZEROING);
            UnrollVmaxPair<kNumVregsPerGroup>::Run(acc0, acc1, scores0, scores1, preg_all);

            // BB: vcmax + broadcast
            vcmax(mx0, acc0, preg_all, MODE_ZEROING);
            vcmax(mx1, acc1, preg_all, MODE_ZEROING);
            vdup(mx0, mx0, preg_all, POS_LOWEST, MODE_ZEROING);
            vdup(mx1, mx1, preg_all, POS_LOWEST, MODE_ZEROING);

            // CC: match + vmin + vcmin
            UnrollMatchMax<kNumVregsPerGroup>::Run(tmp0, index_vec, scores0, mx0, vreg_int_max, preg_all);
            UnrollMatchMax<kNumVregsPerGroup>::Run(tmp1, index_vec, scores1, mx1, vreg_int_max, preg_all);
            vdup(amin0, (int32_t)0x7FFFFFFF, preg_all, MODE_ZEROING);
            vdup(amin1, (int32_t)0x7FFFFFFF, preg_all, MODE_ZEROING);
            UnrollVminPair<kNumVregsPerGroup>::Run(amin0, amin1, tmp0, tmp1, preg_all);
            vcmin(idx0, amin0, preg_all, MODE_ZEROING);
            vcmin(idx1, amin1, preg_all, MODE_ZEROING);
            vdup(idx0, idx0, preg_all, POS_LOWEST, MODE_ZEROING);
            vdup(idx1, idx1, preg_all, POS_LOWEST, MODE_ZEROING);

            // DD: park + in-reg mask
            vsts(idx0, out0Ptr, (int32_t)k, ONEPT_B32, preg_one);
            vsts(idx1, out1Ptr, (int32_t)k, ONEPT_B32, preg_one);
            UnrollMaskWinner<kNumVregsPerGroup>::Run(scores0, index_vec, vreg_neg_inf, idx0, preg_all);
            UnrollMaskWinner<kNumVregsPerGroup>::Run(scores1, index_vec, vreg_neg_inf, idx1, preg_all);
        }
    }
}
#endif // USE_AABBCC

// ============================================================================
// __tf__ vector function: 1:1 translation of asc.py's SimdVF block
// ============================================================================
__tf__ AICORE void topk_compute_vf(
    F32VecTile::TileDType scoresTileData,
    I32VecTile::TileDType outTileData)
{
    __ubuf__ float *scoresPtr = (__ubuf__ float *)__cce_get_tile_ptr(scoresTileData);
    __ubuf__ int32_t *outPtr = (__ubuf__ int32_t *)__cce_get_tile_ptr(outTileData);

    __VEC_SCOPE__
    {
        pto::MaskReg preg_all = pset_b32(PAT_ALL);
        uint32_t sreg_one = 1;
        pto::MaskReg preg_one = pto::CreatePredicate<int32_t>(sreg_one);

        // Clear output UB (asc.py: T.clear(out_ub))
        {
            pto::RegTensor<int32_t> vreg_zero;
            vbr(vreg_zero, 0);
            constexpr auto distNormClear =
                std::integral_constant<::DistVST, static_cast<::DistVST>(pto::GetDistVst<int32_t, pto::DistVST::DIST_NORM>())>();
            for (uint16_t i = 0; i < (uint16_t)kKAligned; i += (uint16_t)kElemsPerVreg) {
                vsts(vreg_zero, outPtr, (int32_t)i, distNormClear, preg_all);
            }
        }

        // Constant vregs
        pto::RegTensor<float> vreg_neg_inf;
        pto::RegTensor<int32_t> vreg_int_max;
        pto::RegTensor<int32_t> vreg_num_exp;
        vdup(vreg_neg_inf, -1e30f, preg_all, MODE_ZEROING);
        vdup(vreg_int_max, (int32_t)0x7FFFFFFF, preg_all, MODE_ZEROING);
        vdup(vreg_num_exp, (int32_t)kE, preg_all, MODE_ZEROING);

        // Vreg arrays
        pto::RegTensor<float> scores_vec[kMaxVregsPerGroup];
        pto::RegTensor<int32_t> index_vec[kMaxVregsPerGroup];
        pto::RegTensor<int32_t> tmp_idx_vec[kMaxVregsPerGroup];

        // Accumulator vregs
        pto::RegTensor<float> acc_max;
        pto::RegTensor<int32_t> acc_min_idx;
        pto::RegTensor<float> mx;
        pto::RegTensor<int32_t> idx;

        constexpr auto distNorm =
            std::integral_constant<::DistVST, static_cast<::DistVST>(pto::GetDistVst<int32_t, pto::DistVST::DIST_NORM>())>();

        if constexpr (kNumGroups == 1) {
            // ================================================================
            // Fast path: single group (asc.py lines 66-96)
            // Load ONCE, mask in vreg, no UB write-back, no memory barrier
            // ================================================================

            // Step 1: Load scores + create index vectors
            UnrollLoad<kNumVregsPerGroup>::Run(scores_vec, index_vec, scoresPtr);

            // Step 2: Mask padding entries to -inf
            UnrollMaskPad<kNumVregsPerGroup>::Run(scores_vec, index_vec, vreg_neg_inf, vreg_num_exp, preg_all);

            // Step 3: Per-K iteration
            for (uint16_t k = 0; k < (uint16_t)kK; ++k) {
                // 3a: Get max value across all vregs
                vdup(acc_max, -1e30f, preg_all, MODE_ZEROING);
                UnrollVmax<kNumVregsPerGroup>::Run(acc_max, scores_vec, preg_all);
                // vcmax reduces to lane 0; vdup(POS_LOWEST) broadcasts to all lanes
                vcmax(mx, acc_max, preg_all, MODE_ZEROING);
                vdup(mx, mx, preg_all, POS_LOWEST, MODE_ZEROING);

                // 3b: Get minimal idx for all values equal to max
                UnrollMatchMax<kNumVregsPerGroup>::Run(tmp_idx_vec, index_vec, scores_vec, mx, vreg_int_max, preg_all);
                vdup(acc_min_idx, (int32_t)0x7FFFFFFF, preg_all, MODE_ZEROING);
                UnrollVmin<kNumVregsPerGroup>::Run(acc_min_idx, tmp_idx_vec, preg_all);
                vcmin(idx, acc_min_idx, preg_all, MODE_ZEROING);
                vdup(idx, idx, preg_all, POS_LOWEST, MODE_ZEROING);

                // 3c: Store winner index to out_ub (ONEPT_B32 = store lane 0 only)
                vsts(idx, outPtr, (int32_t)(k), ONEPT_B32, preg_one);

                // 3d: Mask winner in VREG ARRAY (NOT UB!) — no memory barrier needed
                UnrollMaskWinner<kNumVregsPerGroup>::Run(scores_vec, index_vec, vreg_neg_inf, idx, preg_all);
            }
        } else {
            // ================================================================
            // Slow path: multi-group (asc.py lines 97-170)
            // Re-read UB per group, mask back to UB, mem_bar between K iters
            // ================================================================

            for (uint16_t k = 0; k < (uint16_t)kK; ++k) {
                // Phase 1: per-group local max → global max
                vdup(acc_max, -1e30f, preg_all, MODE_ZEROING);
                for (uint16_t g = 0; g < (uint16_t)kNumGroups; ++g) {
                    int base = (int)g * kExpertsPerGroup;
                    #pragma unroll
                    for (uint16_t i = 0; i < (uint16_t)kNumVregsPerGroup; ++i) {
                        int offset = base + (int)i * kElemsPerVreg;
                        vci(index_vec[i], (int32_t)offset);
                        pto::MaskReg mask_pad;
                        vcmp_lt(mask_pad, index_vec[i], vreg_num_exp, preg_all);
                        vlds(scores_vec[i], scoresPtr, (int32_t)offset, NORM);
                        vsel(scores_vec[i], scores_vec[i], vreg_neg_inf, mask_pad);
                        vmax(acc_max, acc_max, scores_vec[i], preg_all, MODE_ZEROING);
                    }
                }
                vcmax(mx, acc_max, preg_all, MODE_ZEROING);
                vdup(mx, mx, preg_all, POS_LOWEST, MODE_ZEROING);

                // Phase 2: per-group winner index → global index
                vdup(acc_min_idx, (int32_t)0x7FFFFFFF, preg_all, MODE_ZEROING);
                for (uint16_t g = 0; g < (uint16_t)kNumGroups; ++g) {
                    int base = (int)g * kExpertsPerGroup;
                    #pragma unroll
                    for (uint16_t i = 0; i < (uint16_t)kNumVregsPerGroup; ++i) {
                        int offset = base + (int)i * kElemsPerVreg;
                        vci(index_vec[i], (int32_t)offset);
                        pto::MaskReg mask_pad;
                        vcmp_lt(mask_pad, index_vec[i], vreg_num_exp, preg_all);
                        vlds(scores_vec[i], scoresPtr, (int32_t)offset, NORM);
                        vsel(scores_vec[i], scores_vec[i], vreg_neg_inf, mask_pad);
                        pto::MaskReg mask_eq;
                        vcmp_eq(mask_eq, scores_vec[i], mx, preg_all);
                        vsel(tmp_idx_vec[i], index_vec[i], vreg_int_max, mask_eq);
                        vmin(acc_min_idx, acc_min_idx, tmp_idx_vec[i], preg_all, MODE_ZEROING);
                    }
                }
                vcmin(idx, acc_min_idx, preg_all, MODE_ZEROING);
                vdup(idx, idx, preg_all, POS_LOWEST, MODE_ZEROING);

                // Store winner index
                vsts(idx, outPtr, (int32_t)(k), ONEPT_B32, preg_one);

                // Phase 3: mask winner back to UB + mem_bar
                for (uint16_t g = 0; g < (uint16_t)kNumGroups; ++g) {
                    int base = (int)g * kExpertsPerGroup;
                    #pragma unroll
                    for (uint16_t i = 0; i < (uint16_t)kNumVregsPerGroup; ++i) {
                        int offset = base + (int)i * kElemsPerVreg;
                        vci(index_vec[i], (int32_t)offset);
                        vlds(scores_vec[i], scoresPtr, (int32_t)offset, NORM);
                        pto::MaskReg mask_winner;
                        vcmp_eq(mask_winner, index_vec[i], idx, preg_all);
                        vsel(scores_vec[i], vreg_neg_inf, scores_vec[i], mask_winner);
                        vsts(scores_vec[i], scoresPtr, (int32_t)offset, distNorm, preg_all);
                    }
                }
                mem_bar(VST_VLD);
            }
        }
    }
}
#endif

__global__ AICORE void TopkGateVfKernel(__gm__ float *scores, __gm__ int32_t *topk_idx)
{
#if defined(__DAV_VEC__)
    using namespace pto;

    using DynShape = pto::Shape<-1, -1, -1, -1, -1>;
    using DynStride = pto::Stride<-1, -1, -1, -1, -1>;

    // Same outer schedule as staged: one MTE2 → V → MTE3 wave per token_tile.
    for (int w = 0; w < kNumWaves; ++w) {
        int base = w * kTokenTile;

        for (int t = 0; t < kTokenTile; ++t) {
            GlobalTensor<float, DynShape, DynStride> srcGlobal(
                scores + (base + t) * kEAligned,
                pto::Shape(1, 1, 1, 1, kEAligned),
                pto::Stride(kEAligned, kEAligned, kEAligned, kEAligned, 1));
            F32VecTile rowTile(1, kEAligned);
            TASSIGN(rowTile, kUbScores + (uint64_t)t * kEAligned * sizeof(float));
            TLOAD(rowTile, srcGlobal);
        }

        set_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);
        wait_flag(PIPE_MTE2, PIPE_V, EVENT_ID0);

#if USE_AABBCC
        // Pairwise AABB within the already-loaded tile (no extra MTE).
        for (int t = 0; t < kTokenTile; t += 2) {
            F32VecTile scoresTile0(1, kEAligned);
            F32VecTile scoresTile1(1, kEAligned);
            I32VecTile outTile0(1, kKAligned);
            I32VecTile outTile1(1, kKAligned);
            TASSIGN(scoresTile0, kUbScores + (uint64_t)t * kEAligned * sizeof(float));
            TASSIGN(scoresTile1, kUbScores + (uint64_t)(t + 1) * kEAligned * sizeof(float));
            TASSIGN(outTile0, kUbOut + (uint64_t)t * kKAligned * sizeof(int32_t));
            TASSIGN(outTile1, kUbOut + (uint64_t)(t + 1) * kKAligned * sizeof(int32_t));
            topk_compute_vf_pair(scoresTile0.data(), scoresTile1.data(),
                                 outTile0.data(), outTile1.data());
        }
#else
        for (int t = 0; t < kTokenTile; ++t) {
            F32VecTile scoresTile(1, kEAligned);
            I32VecTile outTile(1, kKAligned);
            TASSIGN(scoresTile, kUbScores + (uint64_t)t * kEAligned * sizeof(float));
            TASSIGN(outTile, kUbOut + (uint64_t)t * kKAligned * sizeof(int32_t));
            topk_compute_vf(scoresTile.data(), outTile.data());
        }
#endif

        set_flag(PIPE_V, PIPE_MTE3, EVENT_ID1);
        wait_flag(PIPE_V, PIPE_MTE3, EVENT_ID1);

        for (int t = 0; t < kTokenTile; ++t) {
            I32VecTile outTile(1, kKAligned);
            TASSIGN(outTile, kUbOut + (uint64_t)t * kKAligned * sizeof(int32_t));
            GlobalTensor<int32_t, DynShape, DynStride> dstGlobal(
                topk_idx + (base + t) * kK,
                pto::Shape(1, 1, 1, 1, kK),
                pto::Stride(kK, kK, kK, kK, 1));
            TSTORE(dstGlobal, outTile);
        }

        pipe_barrier(PIPE_ALL);
    }

    pipe_barrier(PIPE_ALL);
#endif
}

} // namespace topk_gate_vf

extern "C" void call_topk_gate_vf(uint32_t block_dim, void *stream,
                                   uint8_t *scores, uint8_t *topk_idx)
{
    (void)block_dim;
    topk_gate_vf::TopkGateVfKernel<<<1, nullptr, stream>>>(
        (__gm__ float *)scores, (__gm__ int32_t *)topk_idx);
}
