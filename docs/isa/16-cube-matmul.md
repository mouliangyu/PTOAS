# 16. Cube Matrix Multiply (MAT)

> **Category:** Cube unit — GM/L1 staging, L0A/L0B loads, L0C accumulate, GM writeback  
> **Pipelines:** MTE2 (GM→L1 / cbuf), Cube (L0A/L0B→L0C), MTE3 (L0C→GM)

This group documents **buffer-pointer** PTO ops used to express a minimal **cube matmul data path** on A5: data is moved from GM into L1-aligned buffers (`cbuf`), loaded into L0A/L0B, multiplied into L0C (`cc`), then written back to GM. These ops are distinct from the vector `!pto.vreg<…>` surface in groups 3–13.

Typical usage keeps the body inside `pto.vecscope { … }` (or another enclosing region required by the PTO verifier) so cube-side effects remain ordered with respect to other PTO work.

---

## `pto.copy_gm_to_cbuf_align_v2`

- **syntax:** `pto.copy_gm_to_cbuf_align_v2 %src, %dst, %n_burst, %len_burst, %src_stride, %dst_stride : !pto.ptr<…, gm>, !pto.ptr<…, ub>, i64, i64, i64, i64`
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

## `pto.vmatmul`

- **syntax:** `pto.vmatmul %lhs, %rhs, %dst, %m, %n, %k : !pto.ptr<…, ub>, !pto.ptr<…, ub>, !pto.ptr<…, ub>, i64, i64, i64`
- **semantics:** Cube **multiply** on L0A (`%lhs`) and L0B (`%rhs`) into L0C (`%dst`). All three pointers must be UB-backed **buffer** pointers (`!pto.ptr<…, ub>`) classified as left / right / accumulator roles in lowering.

Supported element-type combinations follow the HIVM intrinsic selection in the compiler (for example `f16` × `f16` → `f32` accumulation, and MX-dtyped paths where applicable).

---

## `pto.copy_matrix_cc_to_gm`

- **syntax:** `pto.copy_matrix_cc_to_gm %src, %dst, %m, %n : !pto.ptr<…, ub>, !pto.ptr<…, gm>, i64, i64`
- **semantics:** L0C (`cc`) → GM matrix writeback. `%src` is UB-backed `cc`; `%dst` is GM.

---

## Current PTOAS Coverage

- VPTO → LLVM (`--vpto-emit-hivm-llvm`) lowers the ops in this group to target-specific `llvm.hivm.*` intrinsics with explicit address spaces for GM / cbuf / L0A / L0B / L0C.
- FileCheck coverage lives under `test/basic/vpto_vmatmul_*.pto` and `test/basic/vpto_cube_dma_matmul_*.pto`.
- TileLang ST builds a cube-linked host testcase via `pto_tilelang_cube_st(tmatmul)` in `test/tilelang_st/npu/a5/src/st/testcase/tmatmul/`.
