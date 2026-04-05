# VPTO LLVM Blockers Shortlist

基线：
- 时间：`2026-04-03`
- 运行方式：`source scripts/ptoas_env.sh && WORK_SPACE=/tmp/vpto-microop-compile-only-20260403 ASCEND_HOME_PATH=/usr/local/Ascend/cann-9.0.0 DEVICE=SIM COMPILE_ONLY=1 JOBS=64 CASE_PREFIX=micro-op bash test/vpto/scripts/run_host_vpto_validation_parallel.sh`
- 范围：`test/vpto/cases/micro-op`
- 结果：`207` case，`186 PASS / 21 FAIL`
- 汇总文件：`/tmp/vpto-microop-compile-only-20260403/parallel-summary.tsv`

本文件只保留本轮 compile-only 仍失败的 blocker。旧条目已清空，已转 `compiled` 的 case 不再保留。

## 1. LLVM 已导出，但 Bisheng handoff / ABI 不接受

- 当前无新增条目
  - 说明：`vldx2/vstsx2` 已按与 `vsts` 同构的 LLVM ABI 收口，`vstsx2` 仅在 `vsts` 基础上扩成双 `vreg src`

## 2. 当前仍需逐条处理的 blocked 清单

- 当前无保留条目
  - 说明：`vbitsort` 已按 installed frontend trace 收口到 `llvm.hivm.VBS32.V300.{f16|f32}`，并完成定向 `COMPILE_ONLY`
