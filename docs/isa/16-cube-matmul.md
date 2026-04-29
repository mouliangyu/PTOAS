# 16. Cube Matrix Multiply (MAT)

> **Category:** Cube unit — GM/L1 staging, L0A/L0B loads, L0C accumulate, and matrix-side side-buffer moves  
> **Pipelines:** MTE2 (GM→L1 / cbuf), Cube (L0A/L0B→L0C), MTE3/FIX (L0C→{GM,L1,UB}, L1→{BT,FB})

This group documents **buffer-pointer** PTO ops used to express a minimal **cube matmul data path** on A5: data is moved from GM into L1-aligned buffers (`cbuf`), loaded into L0A/L0B, multiplied into L0C (`cc`), then written back or redistributed to GM/L1/UB and related matrix-side buffers. These ops are distinct from the vector `!pto.vreg<…>` surface in groups 3–13.

Typical usage keeps the body inside `pto.vecscope { … }` (or another enclosing region required by the PTO verifier) so cube-side effects remain ordered with respect to other PTO work.

---

## `pto.copy_gm_to_cbuf`

- **syntax:** `pto.copy_gm_to_cbuf %src, %dst, %n_burst, %len_burst, %src_stride, %dst_stride : !pto.ptr<…, gm>, !pto.ptr<…, ub>, i64, i64, i64, i64`
- **semantics:** GM→L1 (`cbuf`) aligned copy. `%src` is GM; `%dst` is UB-backed L1 (`cbuf`) staging.

Operands `%n_burst`, `%len_burst`, `%src_stride`, and `%dst_stride` configure the transfer shape; they are lowered to packed `i64` configuration tokens for the target `llvm.hivm` MOV family intrinsic.

---

## `pto.load_cbuf_to_ca`

- **syntax:** `pto.load_cbuf_to_ca %src, %dst, %m, %k : !pto.ptr<…, ub>, !pto.ptr<…, ub>, i64, i64`
- **semantics:** L1 (`cbuf`) → L0A load. `%src` is `cbuf`; `%dst` is UB-backed L0A staging.

---

## `pto.load_cbuf_to_cb`

- **syntax:** `pto.load_cbuf_to_cb %src, %dst, %k, %n : !pto.ptr<…, ub>, !pto.ptr<…, ub>, i64, i64`
- **semantics:** L1 (`cbuf`) → L0B load. `%src` is `cbuf`; `%dst` is UB-backed L0B staging.

---

## `pto.mad`

- **syntax:** `pto.mad %lhs, %rhs, %dst, %m, %n, %k : !pto.ptr<…, left>, !pto.ptr<…, right>, !pto.ptr<…, acc>, i64, i64, i64`
- **semantics:** Cube matmul with zero-initialized `C`. Computes `dst = lhs * rhs`.
- **attributes:**
  - `unit_flag_ctrl` selects the accumulator phase control. Supported values are `0`, `2`, and `3`.
  - `disable_gemv` controls the GEMV-disable bit in the packed MMAD config.

## `pto.mad_acc`

- **syntax:** `pto.mad_acc %lhs, %rhs, %dst, %m, %n, %k : !pto.ptr<…, left>, !pto.ptr<…, right>, !pto.ptr<…, acc>, i64, i64, i64`
- **semantics:** Cube matmul with `dst` as the accumulator source. Computes `dst += lhs * rhs`.
- **attributes:**
  - `unit_flag_ctrl` selects the accumulator phase control. Supported values are `0`, `2`, and `3`.
  - `disable_gemv` controls the GEMV-disable bit in the packed MMAD config.

## `pto.mad_bias`

- **syntax:** `pto.mad_bias %lhs, %rhs, %dst, %bias, %m, %n, %k : !pto.ptr<…, left>, !pto.ptr<…, right>, !pto.ptr<…, acc>, !pto.ptr<…, bias>, i64, i64, i64`
- **semantics:** Cube matmul with bias-table initialization. Computes `dst = lhs * rhs + bias`, where `%bias` provides the BT address used as the initial C source.
- **attributes:**
  - `unit_flag_ctrl` selects the accumulator phase control. Supported values are `0`, `2`, and `3`.
  - `disable_gemv` controls the GEMV-disable bit in the packed MMAD config.

## `pto.mad_mx`

- **syntax:** `pto.mad_mx %lhs, %rhs, %dst, %m, %n, %k : !pto.ptr<…, left>, !pto.ptr<…, right>, !pto.ptr<…, acc>, i64, i64, i64`
- **semantics:** MX cube matmul with zero-initialized `C`. Computes `dst = lhs * rhs`.
- **attributes:**
  - `unit_flag_ctrl` selects the accumulator phase control. Supported values are `0`, `2`, and `3`.
  - `disable_gemv` controls the GEMV-disable bit in the packed MMAD config.

## `pto.mad_mx_acc`

- **syntax:** `pto.mad_mx_acc %lhs, %rhs, %dst, %m, %n, %k : !pto.ptr<…, left>, !pto.ptr<…, right>, !pto.ptr<…, acc>, i64, i64, i64`
- **semantics:** MX cube matmul with `dst` as the accumulator source. Computes `dst += lhs * rhs`.
- **attributes:**
  - `unit_flag_ctrl` selects the accumulator phase control. Supported values are `0`, `2`, and `3`.
  - `disable_gemv` controls the GEMV-disable bit in the packed MMAD config.

## `pto.mad_mx_bias`

- **syntax:** `pto.mad_mx_bias %lhs, %rhs, %dst, %bias, %m, %n, %k : !pto.ptr<…, left>, !pto.ptr<…, right>, !pto.ptr<…, acc>, !pto.ptr<…, bias>, i64, i64, i64`
- **semantics:** MX cube matmul with bias-table initialization. Computes `dst = lhs * rhs + bias`, where `%bias` provides the BT address used as the initial C source.
- **attributes:**
  - `unit_flag_ctrl` selects the accumulator phase control. Supported values are `0`, `2`, and `3`.
  - `disable_gemv` controls the GEMV-disable bit in the packed MMAD config.

Supported element-type combinations follow the HIVM intrinsic selection in the compiler (for example `f16` × `f16` → `f32` accumulation, and MX-dtyped paths where applicable).

---

## `pto.copy_matrix_cc_to_gm`

- **syntax:** `pto.copy_matrix_cc_to_gm %src, %dst, %xm, %xt : !pto.ptr<…, ub>, !pto.ptr<…, gm>, i64, i64`
- **semantics:** L0C (`cc`) → GM matrix writeback. `%src` is UB-backed `cc`; `%dst` is GM. `%xm` and `%xt` are the target writeback configuration registers passed through directly.

---

## `pto.copy_gm_to_cbuf_multi_nd2nz` / `pto.copy_gm_to_cbuf_multi_dn2nz`

- **syntax:** `pto.copy_gm_to_cbuf_multi_* %src, %dst, %sid, %loop1_src_stride, %l2_cache_ctrl, %n_value, %d_value, %loop4_src_stride, %smallc0_en : !pto.ptr<…, gm>, !pto.ptr<…, ub>, i64, i64, i64, i64, i64, i64, i1`
- **semantics:** GM→L1 (`cbuf`) multi-fractal staging paths for cube data layout conversion variants (`ND2NZ` / `DN2NZ`), lowered to `llvm.hivm.MOV.OUT.TO.L1.MULTI.*` families.
- `%loop1_src_stride`, `%n_value`, `%d_value`, and `%loop4_src_stride` describe the source traversal and packing shape.
- `%smallc0_en` controls small-C0 mode. It is only valid when `d_value <= 4`.
- The destination NZ layout is additionally configured through `pto.set_mte2_nz_para`, whose fields provide the group count and destination loop2/loop3/loop4 strides.

---

## `pto.copy_matrix_cc_to_cbuf` / `pto.copy_matrix_cc_to_ub`

- **syntax:** `pto.copy_matrix_cc_to_* %src, %dst, %config0, %config1 : !pto.ptr<…, ub>, !pto.ptr<…, ub>, i64, i64`
- **semantics:** L0C (`cc`) redistribution to L1 (`cbuf`) or UB destinations. These are post-matmul movement ops typically used before follow-up fusion or output formatting.

---

## `pto.copy_cbuf_to_bt` / `pto.copy_cbuf_to_fbuf`

- **syntax:** `pto.copy_cbuf_to_bt ...` and `pto.copy_cbuf_to_fbuf ...`
- **semantics:** L1 (`cbuf`) to bias/scaling-side buffers for matrix post-processing setup. Lowered to `llvm.hivm.MOV.L1.TO.BT.f16` and `llvm.hivm.MOV.L1.TO.FB.V2`.

## `pto.bias_load`

- **syntax:** `pto.bias_load %src, %dst, %len_burst nburst(%count, %src_gap, %dst_gap) : !pto.ptr<T, mat>, !pto.ptr<U, bias>, i64, i64, i64, i64`
- **semantics:** Structured L1 (`cbuf`) to bias-table (`BT`) load helper. Expands to `pto.copy_cbuf_to_bt`.
- **type rules:** supported source/destination element-type pairs are `f32 -> f32`, `i32 -> i32`, `f16 -> f32`, and `bf16 -> f32`.
- **conversion behavior:** `f16 -> f32` sets the underlying `CVT_EN` bit. For `bf16 -> f32`, conversion to `f32` is implied by the instruction semantics and the control bit is ignored.

---

## Cube Bridge Wrapper Ops (Structured Convenience Layer)

These ops are **wrapper interfaces** that fuse common cube register-configuration sequences plus a terminal cube movement op. They are expanded by `PTOVPTOExpandBridgeOps` into base PTO ops during lowering.

### `pto.cube_load`

- **syntax:** `pto.cube_load %src, %dst, %len_burst nburst(%count, %src_stride, %dst_stride) loop(%count, %src_stride, %dst_stride) ... : !pto.ptr<..., gm>, !pto.ptr<..., mat>, i64, i64, i64, i64[, i64, i64, i64 ...]`
- **semantics:** structured GM→L1 (`cbuf`) staging helper.
- **loop order:** repeated `loop(...)` groups are written from inner to outer. The first two groups lower to hardware loop config; any remaining outer groups expand to software `scf.for` loops around the copy.
- **expands to:** `pto.set_loop2_stride_outtol1` + `pto.set_loop1_stride_outtol1` + `pto.set_loop_size_outtol1` + `pto.copy_gm_to_cbuf`

### `pto.cube_store`

- **syntax:** `pto.cube_store %src, %dst, %len_burst nburst(%count, %src_stride, %dst_stride) loop(%count, %src_stride, %dst_stride) ... : !pto.ptr<..., mat>, !pto.ptr<..., ub>, i64, i64, i64, i64[, i64, i64, i64 ...]`
- **semantics:** structured L1 (`cbuf`) → UB staging helper.
- **loop order:** repeated `loop(...)` groups are written from inner to outer. All wrapper loop levels expand to software `scf.for` loops around the terminal copy.
- **expands to:** `pto.copy_cbuf_to_ubuf`

### `pto.cube_load_frac`

- **syntax:** `pto.cube_load_frac %src, %dst, nd2nz|dn2nz, shape(%n_value, %d_value), src_layout(%src_inner_stride[, %src_outer_stride]), dst_group(%group_count, %dst_loop2_stride, %dst_loop3_stride, %dst_loop4_stride), ctrl(%l2_cache_ctrl, %smallc0_en) : !pto.ptr<..., gm>, !pto.ptr<..., mat>, nd2nz|dn2nz, shape i64, i64, src_layout(i64[, i64]), dst_group i64, i64, i64, i64, ctrl i64, i1`
- **semantics:** structured GM→L1 fractal staging helper for `ND2NZ` / `DN2NZ`.
- `shape(...)` describes the logical `N x D` payload moved by one multi-fractal transfer.
- `src_layout(...)` describes source-side traversal. `%src_outer_stride` is optional and defaults to `0`.
- `dst_group(...)` maps directly to `pto.set_mte2_nz_para`, providing group count plus destination loop2/loop3/loop4 strides in units of `C0_size`.
- `ctrl(...)` exposes the intrinsic `l2_cache_ctrl` and `smallc0_en` fields. `smallc0_en` is only valid when `d_value <= 4`.
- **expands to:** `pto.set_mte2_nz_para` + `pto.copy_gm_to_cbuf_multi_nd2nz|pto.copy_gm_to_cbuf_multi_dn2nz`

### `pto.left_load`

- **syntax:** `pto.left_load %src, %dst, %m, %k, %loop3_count, %loop3_src_stride, %loop3_dst_stride, %loop0_src_stride : !pto.ptr<..., mat>, !pto.ptr<..., left>, i64, i64, i64, i64, i64, i64`
- **semantics:** structured L1→L0A helper for matmul left operand loading. `%loop3_count`, `%loop3_src_stride`, and `%loop3_dst_stride` describe the `LOOP3_PARA` register fields `LOOP3_PARA[15:0]`, `LOOP3_PARA[31:16]`, and `LOOP3_PARA[63:32]`. `%loop0_src_stride` describes `CHANNEL_PARA[63:48]`, the loop0 source stride in units of `C0_SIZE`.
- **expands to:** `pto.set_loop3_para` + `pto.set_channel_para` + `pto.load_cbuf_to_ca`

### `pto.right_load`

- **syntax:** `pto.right_load %src, %dst, %k, %n, %loop3_count, %loop3_src_stride, %loop3_dst_stride, %loop0_src_stride : !pto.ptr<..., mat>, !pto.ptr<..., right>, i64, i64, i64, i64, i64, i64`
- **semantics:** structured L1→L0B helper for matmul right operand loading. `%loop3_count`, `%loop3_src_stride`, and `%loop3_dst_stride` describe the `LOOP3_PARA` register fields `LOOP3_PARA[15:0]`, `LOOP3_PARA[31:16]`, and `LOOP3_PARA[63:32]`. `%loop0_src_stride` describes `CHANNEL_PARA[63:48]`, the loop0 source stride in units of `C0_SIZE`.
- **expands to:** `pto.set_loop3_para` + `pto.set_channel_para` + `pto.load_cbuf_to_cb`

### `pto.acc_store`

- **syntax:** `pto.acc_store %src, %dst, %m, %n, %src_stride, %dst_stride, %unit_flag_ctrl, %quant_pre, %relu_pre_mode, nz2nd|nz2dn(%loop0_src_stride)?|nz2nz(%split)? loop3(%count, %src_stride, %dst_stride)? : !pto.ptr<..., acc>, !pto.ptr<..., mat>, i64, i64, i64, i64, i64, i64, i64[, nz2dn(i64)|nz2nz(i64)][, loop3(i64, i64, i64)]`
- **semantics:** structured accumulator-store helper for `FIX_L0C_TO_L1`. `%m` and `%n` describe the matrix tile shape for each write-back step. `%src_stride` and `%dst_stride` describe the per-step source and destination strides. The mode selects the destination layout conversion: `nz2nd` writes NZ fragments to ND layout, `nz2dn` writes NZ fragments to DN layout, and `nz2nz` writes NZ fragments to NZ layout. `nz2dn(%loop0_src_stride)` additionally controls the loop0 source stride in units of `C0_SIZE`. `nz2nz(%split)` selects split NZ write-back when needed. `loop3(%count, %src_stride, %dst_stride)` is an optional special hardware loop descriptor for this op. `nz2nz` does not accept `loop3(...)`.
- **expands to:** `pto.set_loop3_para` + `pto.set_channel_para` + `pto.copy_matrix_cc_to_cbuf`

### `pto.acc_store_gm`

- **syntax:** `pto.acc_store_gm %src, %dst, %m, %n, %src_stride, %dst_stride, %unit_flag_ctrl, %quant_pre, %relu_pre_mode, %sid, %l2_cache_ctrl, nz2nd|nz2dn(%loop0_src_stride)?|nz2nz(%split)? loop3(%count, %src_stride, %dst_stride)? : !pto.ptr<..., acc>, !pto.ptr<..., gm>, i64, i64, i64, i64, i64, i64, i64, i64, i64[, nz2dn(i64)|nz2nz(i64)][, loop3(i64, i64, i64)]`
- **semantics:** structured accumulator-store helper for `FIX_L0C_TO_OUT`. Compared with `pto.acc_store`, this GM path additionally exposes `%sid` and `%l2_cache_ctrl`, which map to the GM-specific OUT-path control fields.
- **expands to:** `pto.set_loop3_para` + `pto.set_channel_para` + `pto.copy_matrix_cc_to_gm`

### `pto.acc_store_ub`

- **syntax:** `pto.acc_store_ub %src, %dst, %m, %n, %src_stride, %dst_stride, %unit_flag_ctrl, %quant_pre, %relu_pre_mode, %dual_dst_mode, %sub_blockid, nz2nd|nz2dn(%loop0_src_stride)?|nz2nz(%channel_split_en)? loop3(%count, %src_stride, %dst_stride)? : !pto.ptr<..., acc>, !pto.ptr<..., ub>, i64, i64, i64, i64, i64, i64, i64, i64, i64[, nz2dn(i64)|nz2nz(i64)][, loop3(i64, i64, i64)]`
- **semantics:** structured accumulator-to-UB helper for `FIX_L0C_TO_UB`. `%m` and `%n` describe the matrix tile shape for each write-back step. `%src_stride` and `%dst_stride` describe the per-step source and destination strides. `%dual_dst_mode` and `%sub_blockid` map to the UB dual-destination control fields. The mode selects the layout path: `nz2nd` writes NZ fragments to ND layout, `nz2dn(%loop0_src_stride)` writes NZ fragments to DN layout and additionally provides the loop0 source stride in units of `C0_SIZE`, and `nz2nz(%channel_split_en)` selects normal DMA behavior, with the optional operand controlling the F32 channel-split bit. `loop3(%count, %src_stride, %dst_stride)` is the optional `LOOP3_PARA` descriptor used by `nz2nd` / `nz2dn`. `nz2nz` does not accept `loop3(...)`.
- **expands to:** `pto.set_loop3_para` + `pto.set_channel_para` + `pto.copy_matrix_cc_to_ub`

---

## Verified A5 Op Set (Current Batch)

The following PTO ops have been verified in the current A5 VPTO→LLVM/HIVM and Bisheng cube-flow validation batch:

- `pto.copy_cbuf_to_bt`
- `pto.copy_cbuf_to_fbuf`
- `pto.copy_gm_to_cbuf_multi_dn2nz`
- `pto.copy_gm_to_cbuf_multi_nd2nz`
- `pto.copy_matrix_cc_to_cbuf`
- `pto.copy_matrix_cc_to_ub`
- `pto.load_cbuf_to_ca_mx`
- `pto.load_cbuf_to_ca_s4`
- `pto.load_cbuf_to_cb_mx`
- `pto.load_cbuf_to_cb_s4`
- `pto.set_atomic_s32`
- `pto.set_atomic_s8`
- `pto.set_channel_para`
- `pto.set_fpc`
- `pto.set_loop1_stride_outtol1`
- `pto.set_loop2_stride_outtol1`
- `pto.set_loop3_para`
- `pto.set_loop_size_outtol1`
- `pto.set_mte2_nz_para`
- `pto.set_pad_val_outtol1`
- `pto.set_quant_pre`

---

## Current PTOAS Coverage

- VPTO → LLVM (`--vpto-emit-hivm-llvm`) lowers the ops in this group to target-specific `llvm.hivm.*` intrinsics with explicit address spaces for GM / cbuf / L0A / L0B / L0C and matrix-side side buffers.
- FileCheck coverage lives under `test/basic/vpto_mad_*.pto` and `test/basic/vpto_cube_dma_matmul_*.pto`.
- TileLang ST builds a cube-linked host testcase via `pto_tilelang_cube_st(tmatmul)` in `test/tilelang_st/npu/a5/src/st/testcase/tmatmul/`.
