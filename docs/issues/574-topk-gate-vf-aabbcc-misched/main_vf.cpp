#include <acl/acl.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

extern "C" void call_topk_gate_vf(uint32_t block_dim, void *stream,
                                   uint8_t *scores, uint8_t *topk_idx);

// These must match the kernel's compile-time K_E and K_K
#ifndef K_E
#define K_E 64
#endif
#ifndef K_K
#define K_K 6
#endif

namespace {

#ifndef K_N
#define K_N 4
#endif
constexpr int kN = K_N;
constexpr int kE = K_E;
constexpr int kK = K_K;
constexpr int kElemsPerVreg = 64;
constexpr int kMaxVregsPerGroup = 6;
constexpr int kNumVregsPerGroup =
    (kE < kMaxVregsPerGroup * kElemsPerVreg) ?
    ((kE + kElemsPerVreg - 1) / kElemsPerVreg) :
    kMaxVregsPerGroup;
constexpr int kExpertsPerGroup = kNumVregsPerGroup * kElemsPerVreg;
constexpr int kNumGroups = (kE + kExpertsPerGroup - 1) / kExpertsPerGroup;
constexpr int kEAligned = kNumGroups * kExpertsPerGroup;

void check_acl(aclError ret, const char *what)
{
    if (ret != ACL_SUCCESS) {
        std::cerr << what << " failed, aclError=" << ret << "\n";
        std::exit(1);
    }
}

void golden_topk_gate(const std::vector<float> &scores_orig,
                      std::vector<int32_t> &topk_golden)
{
    topk_golden.resize(kN * kK);
    std::vector<float> row(kE);
    for (int t = 0; t < kN; ++t) {
        for (int e = 0; e < kE; ++e)
            row[e] = scores_orig[t * kEAligned + e];
        for (int k = 0; k < kK; ++k) {
            float max_val = -1e30f;
            int32_t max_idx = 0;
            for (int e = 0; e < kE; ++e) {
                if (row[e] > max_val) {
                    max_val = row[e];
                    max_idx = e;
                }
            }
            topk_golden[t * kK + k] = max_idx;
            row[max_idx] = -1e30f;
        }
    }
}

} // namespace

int main()
{
    // Scores: kN=4 tokens, kE experts, padded to kEAligned
    // Padding entries set to -1e30 so they are never selected
    std::vector<float> scores_dev(kN * kEAligned, -1e30f);

    // Fill with deterministic test data covering edge cases:
    // Token 0: distinct values (clear ranking)
    // Token 1: some ties (tests tie-breaking)
    // Token 2: one dominant expert
    // Token 3: all equal (tests sequential tie-breaking)

    for (int t = 0; t < kN; ++t) {
        for (int e = 0; e < kE; ++e) {
            if (t == 0) {
                scores_dev[t * kEAligned + e] = (float)(kE - e);  // descending
            } else if (t == 1) {
                scores_dev[t * kEAligned + e] = (float)(e % 4);   // ties every 4
            } else if (t == 2) {
                scores_dev[t * kEAligned + e] = (e == 0) ? 100.0f : (float)e;
            } else {
                scores_dev[t * kEAligned + e] = 1.0f;  // all equal
            }
        }
    }

    std::vector<int32_t> topk_golden;
    golden_topk_gate(scores_dev, topk_golden);

    std::vector<int32_t> topk_dev(kN * kK, -1);

    constexpr size_t kScoresBytes = kN * kEAligned * sizeof(float);
    constexpr size_t kIdxBytes = kN * kK * sizeof(int32_t);

    check_acl(aclInit(nullptr), "aclInit");
    check_acl(aclrtSetDevice(0), "aclrtSetDevice");
    aclrtStream stream = nullptr;
    check_acl(aclrtCreateStream(&stream), "aclrtCreateStream");

    void *scores_dev_ptr = nullptr;
    void *idx_dev_ptr = nullptr;
    check_acl(aclrtMalloc(&scores_dev_ptr, kScoresBytes, ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc scores");
    check_acl(aclrtMalloc(&idx_dev_ptr, kIdxBytes, ACL_MEM_MALLOC_HUGE_FIRST), "aclrtMalloc idx");
    check_acl(aclrtMemcpy(scores_dev_ptr, kScoresBytes, scores_dev.data(), kScoresBytes,
                          ACL_MEMCPY_HOST_TO_DEVICE), "memcpy scores");
    check_acl(aclrtMemset(idx_dev_ptr, kIdxBytes, 0, kIdxBytes), "memset idx");

    call_topk_gate_vf(1, stream, static_cast<uint8_t *>(scores_dev_ptr),
                      static_cast<uint8_t *>(idx_dev_ptr));
    check_acl(aclrtSynchronizeStream(stream), "aclrtSynchronizeStream");
    check_acl(aclrtMemcpy(topk_dev.data(), kIdxBytes, idx_dev_ptr, kIdxBytes,
                          ACL_MEMCPY_DEVICE_TO_HOST), "memcpy idx back");

    aclrtFree(scores_dev_ptr);
    aclrtFree(idx_dev_ptr);
    aclrtDestroyStream(stream);
    aclrtResetDevice(0);
    aclFinalize();

    bool ok = true;
    for (int t = 0; t < kN; ++t) {
        for (int k = 0; k < kK; ++k) {
            int32_t got = topk_dev[t * kK + k];
            int32_t gold = topk_golden[t * kK + k];
            if (got != gold) {
                std::cerr << "FAIL t=" << t << " k=" << k
                          << ": got=" << got << " golden=" << gold << "\n";
                ok = false;
            }
        }
    }

    std::cout << "Config: kN=" << kN << " kE=" << kE << " kK=" << kK
              << " kEAligned=" << kEAligned << " kNumVregs=" << kNumVregsPerGroup
              << " kNumGroups=" << kNumGroups << "\n";
    std::cout << "Golden: ";
    for (int i = 0; i < kN * kK; ++i) std::cout << topk_golden[i] << " ";
    std::cout << "\nGot:    ";
    for (int i = 0; i < kN * kK; ++i) std::cout << topk_dev[i] << " ";
    std::cout << "\n";

    if (!ok) {
        std::cerr << "FAIL topk_gate_vf\n";
        return 1;
    }
    std::cout << "PASS topk_gate_vf (1:1 CCE intrinsics, kN=" << kN << " kE=" << kE
              << " kK=" << kK << " vregs=" << kNumVregsPerGroup
              << " groups=" << kNumGroups << ")\n";
    return 0;
}
