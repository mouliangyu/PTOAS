# VPTO LLVM Blockers Shortlist

基线：
- 时间：`2026-04-03`
- 运行方式：`source scripts/ptoas_env.sh && WORK_SPACE=/tmp/vpto-microop-compile-only-20260403 ASCEND_HOME_PATH=/usr/local/Ascend/cann-9.0.0 DEVICE=SIM COMPILE_ONLY=1 JOBS=64 CASE_PREFIX=micro-op bash test/vpto/scripts/run_host_vpto_validation_parallel.sh`
- 范围：`test/vpto/cases/micro-op`
- 结果：`207` case，`186 PASS / 21 FAIL`
- 汇总文件：`/tmp/vpto-microop-compile-only-20260403/parallel-summary.tsv`

本文件只保留本轮 compile-only 仍失败的 blocker。旧条目已清空，已转 `compiled` 的 case 不再保留。

## 1. Surface / parser / verifier 未收口
- `pto.vlds`
  - docs-signature: `%result = pto.vlds %base[%offset], "BRC_B32", %mask : !pto.ptr<T, ub>, !pto.mask<G> -> !pto.vreg<...>`
  - vpto-signature: `%result = pto.vlds %base[%offset], "BRC_B32", %mask : !pto.ptr<T, ub>, !pto.mask<G> -> !pto.vreg<...>`
  - llvm-signature: `not reached`
  - blocker: verifier 报 `supports only NORM, BLK, DINTLV_B32, and UNPK_B16 distributions`
  - cases: `micro-op/vector-load-store/vlds-brc-b32`

## 2. VPTO text / LLVM emission 仍失败

- `pto.vgatherb`
  - docs-signature: `%result = pto.vgatherb %base, %index, %mask : !pto.ptr<T, ub>, !pto.vreg<...>, !pto.mask<G> -> !pto.vreg<...>`
  - vpto-signature: `%result = pto.vgatherb %base, %index, %mask : !pto.ptr<T, ub>, !pto.vreg<...>, !pto.mask<G> -> !pto.vreg<...>`
  - llvm-signature: `not emitted`
  - blocker: emitter 报 `Failed to emit VPTO text`
  - cases: `micro-op/gather-scatter/vgatherb`, `micro-op/gather-scatter/vgatherb-block-boundary`

- `pto.vsst`
  - docs-signature: `pto.vsst %value, %base, %stride, %mask : !pto.vreg<...>, !pto.ptr<T, ub>, i32, !pto.mask<G>`
  - vpto-signature: `pto.vsst %value, %base, %stride, %mask : !pto.vreg<...>, !pto.ptr<T, ub>, i32, !pto.mask<G>`
  - llvm-signature: `not emitted`
  - blocker: emitter 报 `Failed to emit VPTO text`
  - cases: `micro-op/vector-load-store/vsst`

- `pto.vsld`, `pto.vsst`
  - docs-signature: `%result = pto.vsld %base, %stride, %mask : !pto.ptr<T, ub>, i32, !pto.mask<G> -> !pto.vreg<...>`; `pto.vsst %value, %base, %stride, %mask : !pto.vreg<...>, !pto.ptr<T, ub>, i32, !pto.mask<G>`
  - vpto-signature: `%result = pto.vsld %base, %stride, %mask : !pto.ptr<T, ub>, i32, !pto.mask<G> -> !pto.vreg<...>`; `pto.vsst %value, %base, %stride, %mask : !pto.vreg<...>, !pto.ptr<T, ub>, i32, !pto.mask<G>`
  - llvm-signature: `not emitted`
  - blocker: emitter 报 `Failed to emit VPTO text`
  - cases: `micro-op/vector-load-store/vsld-vsst-stride-boundary`

## 3. LLVM 已导出，但 Bisheng handoff / ABI 不接受

- `pto.pst`, `pto.pld`
  - docs-signature: `pto.pst %mask, %base[%off] : !pto.mask<G>, !pto.ptr<T, ub>`; `%result = pto.pld %base[%off], "DIST" : !pto.ptr<T, ub> -> !pto.mask<G>`
  - vpto-signature: `pto.pst %mask, %base[%off] : !pto.mask<G>, !pto.ptr<T, ub>`; `%result = pto.pld %base[%off], "DIST" : !pto.ptr<T, ub> -> !pto.mask<G>`
  - llvm-signature: `@llvm.hivm.pst.*(...), @llvm.hivm.pld.*(...)`
  - blocker: Bisheng 报 `Intrinsic has incorrect argument type` / `Broken module found, compilation aborted!`
  - cases: `micro-op/predicate-load-store/pst-pld`

- `pto.vldx2`, `pto.vstx2`
  - docs-signature: `%r0, %r1 = pto.vldx2 ...`; `pto.vstx2 %r0, %r1, ...`
  - vpto-signature: `%r0, %r1 = pto.vldx2 ...`; `pto.vstx2 %r0, %r1, ...`
  - llvm-signature: `@llvm.hivm.vldx2.*(...), @llvm.hivm.vstx2.*(...)`
  - blocker: Bisheng 报 `Intrinsic has incorrect argument type` / `Broken module found, compilation aborted!`
  - cases: `micro-op/vector-load-store/vldx2-vstx2`, `micro-op/vector-load-store/vldx2-layout-check`, `micro-op/vector-load-store/vstx2-layout-check`

- `pto.vsld`
  - docs-signature: `%result = pto.vsld %base, %stride, %mask : !pto.ptr<T, ub>, i32, !pto.mask<G> -> !pto.vreg<...>`
  - vpto-signature: `%result = pto.vsld %base, %stride, %mask : !pto.ptr<T, ub>, i32, !pto.mask<G> -> !pto.vreg<...>`
  - llvm-signature: `@llvm.hivm.vsld.*(...)`
  - blocker: Bisheng 报 `Intrinsic has incorrect argument type` / `Broken module found, compilation aborted!`
  - cases: `micro-op/vector-load-store/vsld`
