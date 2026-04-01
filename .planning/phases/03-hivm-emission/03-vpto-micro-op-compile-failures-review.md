# VPTO Micro-Op 编译失败核对单

## Purpose

本文件记录 `test/vpto/cases/micro-op` 在 `DEVICE=SIM`、未配置 `SIM_LIB_DIR` 条件下的批量编译扫描结果。

说明：

- 本文件只记录停在 step `1/6` 到 step `4/6` 的失败项
- 已能走到 step `5/6`、仅因未配置 `SIM_LIB_DIR` 停下的 case 不纳入本表
- 每条先记录当前观察到的失败归因
- `结论：` 由人工逐条填写；若已填写，本文件后续整理不覆盖既有结论
- 原始扫描结果位于 `output/vpto-sim-compile-batch2/results/compile_failures.tsv`

## Self-Check Summary

本轮对失败项做了抽样自检，当前可以先确认下面几类问题不能直接记为“实现缺失”：

- 测例书写错误：
  - `vaddc/vsubc/vaddcs/vsubcs` 被按单结果 op 使用，但当前 surface 是双结果形式，需同时接结果向量和 carry mask。
  - `vsel` 测例把比较模式字符串和 `!pto.mask` 结果类型误塞给了 `vsel`；比较模式属于 `vcmp/vcmps`，`vsel` 本身返回向量。
  - `vlds/vldas/vldus` 一批测例把内存类 op 当作向量计算 op 使用，例如把 `vldas` 写成 `vreg -> vreg`。
  - `vstar` 这类无结果 op 被写成了有结果赋值形式。
- 测例跟随旧 surface：
  - `vcvt` 当前 surface 采用 attribute 形式承载 `round_mode/sat/part`，不是旧的字符串位置参数形式。
  - 部分 load/store、predicate-load-store 的用例写法更接近旧文档/旧草稿，不符合当前 `VPTOOps.td` 的 assembly format。
- 测例目标和语义不一致：
  - 例如 `vands` 目标是按整型 vec-scalar 位运算覆盖，但测例实际使用了 `f32` 输入和 `f32` 标量。
- 仍可能是真实实现缺口：
  - 某些 op 在文档/规划中存在，但当前 `VPTOOps.td` 未定义，或者已定义但 VPTO LLVM emitter 尚未支持。这类需要在逐条核对完 surface 后再下结论，不能先用 skeleton 失败直接定性。

因此，本文件中的“观察”只代表首轮扫描现象；填写 `结论` 时应优先区分：

- 测例书写错误
- 测例语义与当前 surface 不符
- 文档 / scope / surface 漂移
- 真实实现缺口

## Next-Phase Fix List

下列 case 已经可以明确归入“下一阶段优先修正测例本身”的列表，暂不应作为实现缺口统计：

- 双结果 op 被写成单结果：
  - `micro-op/binary-vector/vaddc`
  - `micro-op/binary-vector/vaddc-carry-boundary`
  - `micro-op/binary-vector/vsubc`
  - `micro-op/binary-vector/vsubc-borrow-boundary`
  - `micro-op/vec-scalar/vaddcs`
  - `micro-op/vec-scalar/vaddcs-carry-boundary`
  - `micro-op/vec-scalar/vsubcs`
  - `micro-op/vec-scalar/vsubcs-borrow-boundary`
- `vsel` / `vcmp` 语义混用：
  - `micro-op/compare-select/vsel`
  - `micro-op/compare-select/vsel-i16`
  - `micro-op/compare-select/vsel-predicate-edge`
  - `micro-op/compare-select/vsel-tail`
- `vcvt` 沿用旧 surface 写法，需改为 attribute 形式：
  - `micro-op/conversion/vcvt-f16-special`
  - `micro-op/conversion/vcvt-f16-to-f32`
  - `micro-op/conversion/vcvt-f32-special`
  - `micro-op/conversion/vcvt-f32-to-f16`
  - `micro-op/conversion/vcvt-tail`
  - `micro-op/conversion/vcvt-tail-special`
- 向量访存类 op 被当成计算 op 使用：
  - `micro-op/vector-load-store/vlds`
  - `micro-op/vector-load-store/vldas-vldus`
- 无结果 op 被写成有结果赋值形式：
  - `micro-op/vector-load-store/vstar`
- 测例目标与数据类型/语义不一致：
  - `micro-op/vec-scalar/vands`

下列 case 很可能也属于“优先修测例或修 surface 对齐”的范围，但在本轮只做了模式级确认，下一阶段应按同 family 批量复查：

- `compare-select` 中其余 `vcmps` / `vselr` 相关 case
- `vector-load-store` 中其余 `vld*` / `vsld*` / `vst*` 相关 case
- `predicate-load-store` 中 `pst*` / `pld*` / `pstu*` 相关 case
- `materialization-predicate` 中 `p*` 变换类 case

## binary-vector

- `micro-op/binary-vector/vaddc`
  - 观察：当前 case 已按双结果 `pto.vaddc %lhs, %rhs, %mask -> %result, %carry` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vaddc`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/binary-vector/vaddc-carry-boundary`
  - 观察：当前 case 已按双结果 `pto.vaddc %lhs, %rhs, %mask -> %result, %carry` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vaddc`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/binary-vector/vshl`
  - 观察：ODS 类型约束不匹配：pto.vshl 仅接受整型向量元素
  - 结论：用例有误，pto.vshl仅支持整形向量，参考docs/isa
- `micro-op/binary-vector/vshl-shift-boundary`
  - 观察：ODS 类型约束不匹配：pto.vshl 仅接受整型向量元素
  - 结论：用例有误，pto.vshl仅支持整形向量，参考docs/isa
- `micro-op/binary-vector/vshr`
  - 观察：ODS 类型约束不匹配：pto.vshr 仅接受整型向量元素
  - 结论：用例有误，pto.vshl仅支持整形向量，参考docs/isa
- `micro-op/binary-vector/vshr-shift-boundary`
  - 观察：ODS 类型约束不匹配：pto.vshr 仅接受整型向量元素
  - 结论：用例有误，pto.vshl仅支持整形向量，参考docs/isa
- `micro-op/binary-vector/vsubc`
  - 观察：当前 case 已按双结果 `pto.vsubc %lhs, %rhs, %mask -> %result, %carry` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsubc`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/binary-vector/vsubc-borrow-boundary`
  - 观察：当前 case 已按双结果 `pto.vsubc %lhs, %rhs, %mask -> %result, %carry` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsubc`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口

## compare-select

- `micro-op/compare-select/vcmps-i16-signed`
  - 观察：ODS 类型约束不匹配：pto.vcmps 的标量类型需与源向量元素类型一致
  - 结论：当前仍属于用例编写问题；要保持目标不变，需要按 docs/isa 将标量操作数改成与向量元素一致的类型后再继续收敛
- `micro-op/compare-select/vcmps-i16-unsigned`
  - 观察：当前 case 已按 `ui16` 标量与 `!pto.vreg<128xui16>` 匹配的 surface 书写，但 `ptoas --vpto-emit-hivm-llvm` 仍在 official lowering pipeline 中停在 `llvm.load` 不接受 `ui16` 结果类型
  - 结论：当前已不是用例编写或 parse 问题；要保持 unsigned-scalar compare 目标不变，会直接暴露出当前 LLVM lowering 对 `ui16` scalar path 的实现缺口
- `micro-op/compare-select/vcmps-unordered-f32`
  - 观察：已按当前 `pto.vcmps` surface 重写为向量+标量+mask 形式；`ptoas --vpto-emit-hivm-llvm` step 1 可通过
  - 结论：当前 surface 下可忠实写出；用例编写和 parse 已收敛
- `micro-op/compare-select/vsel`
  - 观察：当前 case 已按 `pto.vsel %src0, %src1, %mask` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsel`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/compare-select/vsel-i16`
  - 观察：当前 case 已按 `pto.vsel %src0, %src1, %mask` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsel`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/compare-select/vsel-predicate-edge`
  - 观察：当前 case 已按 `pto.vsel %src0, %src1, %mask` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsel`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/compare-select/vsel-tail`
  - 观察：当前 case 已按 tail-mask `pto.vsel %src0, %src1, %mask` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsel`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 tail-mask surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/compare-select/vselr`
  - 观察：现有用例仍是旧的 `mask + cmp_mode` 伪接口，但当前无法根据 `docs/isa/11-compare-select.md`、`docs/isa/12-data-rearrangement.md` 与 `VPTOOps.td` 唯一确定 `vselr` 的真实语义
  - 结论：blocked。现有用例仍是旧的 `mask + cmp_mode` 伪接口，但当前无法根据 `docs/isa/11-compare-select.md`、`docs/isa/12-data-rearrangement.md` 与 `VPTOOps.td` 唯一确定 `vselr` 的真实语义，暂不继续收敛

## conversion

- `micro-op/conversion/vcvt-f16-special`
  - 观察：当前 case 已按 `pto.vcvt %input {round_mode = ...}` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vcvt`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/conversion/vcvt-f16-to-f32`
  - 观察：当前 case 已按 `pto.vcvt %input {round_mode = ...}` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vcvt`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/conversion/vcvt-f32-special`
  - 观察：当前 case 已按 `pto.vcvt %input {round_mode = ...}` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vcvt`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/conversion/vcvt-f32-to-f16`
  - 观察：当前 case 已按 `pto.vcvt %input {round_mode = ...}` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vcvt`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/conversion/vcvt-tail`
  - 观察：当前 case 已按 tail-mask `pto.vcvt %input {round_mode = ...}` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vcvt`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 tail-mask surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/conversion/vcvt-tail-special`
  - 观察：当前 case 已按 tail-mask `pto.vcvt %input {round_mode = ...}` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vcvt`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 tail-mask surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口

## dsa-sfu

- `micro-op/dsa-sfu/vaddrelu-f32`
  - 观察：当前 case 已按 `pto.vaddrelu %lhs, %rhs` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vaddrelu`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/dsa-sfu/vaddreluconv`
  - 观察：当前 skeleton 仍沿用旧的 `vector + scalar` 写法，但 `VPTOOps.td` 与 `docs/isa/13-dsa-sfu-ops.md` 都要求 `pto.vaddreluconv` 采用 `vector + vector` 输入；同时当前 verifier 还要求 source/result 保持总存储位宽，与文档中“conversion-result”示例之间仍有漂移
  - 结论：当前失败首先暴露的是测例 skeleton 失真；但即使修正到 `vector + vector`，`conversion-result` 目标仍会撞到 docs/ODS/verify 尚未完全对齐的问题，不能为了过 parser 把目标弱化成普通非-conv 向量算子
- `micro-op/dsa-sfu/vaxpy-f32`
  - 观察：当前 case 已按 `pto.vaxpy %src0, %src1, %alpha` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vaxpy`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/dsa-sfu/vci`
  - 观察：当前 case 已按 `pto.vci %index {order = ...}` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vci`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/dsa-sfu/vexpdiff-boundary`
  - 观察：当前 case 已按 `pto.vexpdiff %input, %max` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vexpdiff`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/dsa-sfu/vexpdiff-f32`
  - 观察：当前 case 已按 `pto.vexpdiff %input, %max` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vexpdiff`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/dsa-sfu/vmula`
  - 观察：已按当前 `pto.vmula %acc, %lhs, %rhs, %mask` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vmula`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/dsa-sfu/vmula-accumulator-boundary`
  - 观察：已按当前 `pto.vmula %acc, %lhs, %rhs, %mask` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vmula`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/dsa-sfu/vmulconv`
  - 观察：当前 skeleton 仍沿用旧的 `vector + scalar` 写法，但 `VPTOOps.td` 与 `docs/isa/13-dsa-sfu-ops.md` 都要求 `pto.vmulconv` 采用 `vector + vector` 输入；同时当前 verifier 还要求 source/result 保持总存储位宽，与文档中“conversion-result”示例之间仍有漂移
  - 结论：当前失败首先暴露的是测例 skeleton 失真；但即使修正到 `vector + vector`，`conversion-result` 目标仍会撞到 docs/ODS/verify 尚未完全对齐的问题，不能为了过 parser 把目标弱化成普通非-conv 向量算子
- `micro-op/dsa-sfu/vmull`
  - 观察：已按当前 `pto.vmull %lhs, %rhs, %mask -> %low, %high` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vmull`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/dsa-sfu/vprelu-f32`
  - 观察：当前 case 已按 `pto.vprelu %input, %alpha` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vprelu`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/dsa-sfu/vprelu-tail`
  - 观察：当前 case 已按 `pto.vprelu %input, %alpha` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vprelu`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/dsa-sfu/vsubrelu-f32`
  - 观察：当前 case 已按 `pto.vsubrelu %lhs, %rhs` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsubrelu`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/dsa-sfu/vtranspose`
  - 观察：当前 case 已按 `pto.vtranspose %dest, %src, %config` surface 书写；失败停在 VPTO LLVM emitter 路径构造 `scf.for` 时缺少 dialect 注册，而不是 parser/ODS
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 emitter pipeline / dialect 注册缺口
- `micro-op/dsa-sfu/vtranspose-multi-config`
  - 观察：当前 case 已按 `pto.vtranspose %dest, %src, %config` surface 书写；失败停在 VPTO LLVM emitter 路径构造 `scf.for` 时缺少 dialect 注册，而不是 parser/ODS
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 multi-config surface 忠实写出，阻塞点是 emitter pipeline / dialect 注册缺口

## gather-scatter

- `micro-op/gather-scatter/vgather2`
  - 观察：当前 case 已按 `pto.vgather2 %source, %offsets, %active_lanes` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vgather2`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/gather-scatter/vgather2-duplicate-index`
  - 观察：当前 case 已按 `pto.vgather2 %source, %offsets, %active_lanes` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vgather2`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/gather-scatter/vgather2_bc`
  - 观察：当前 case 已按 `pto.vgather2_bc %source, %offsets, %mask` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vgather2_bc`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/gather-scatter/vgather2_bc-sparse-mask`
  - 观察：当前 case 已按 `pto.vgather2_bc %source, %offsets, %mask` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vgather2_bc`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/gather-scatter/vgatherb`
  - 观察：当前 case 已按 `pto.vgatherb %source, %offsets, %active_lanes` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vgatherb`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/gather-scatter/vgatherb-block-boundary`
  - 观察：当前 case 已按 `pto.vgatherb %source, %offsets, %active_lanes` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vgatherb`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/gather-scatter/vscatter`
  - 观察：当前 case 已按 `pto.vscatter %value, %dest, %offsets, %active_lanes` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vscatter`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/gather-scatter/vscatter-out-of-order-index`
  - 观察：当前 case 已按 `pto.vscatter %value, %dest, %offsets, %active_lanes` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vscatter`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口

## materialization-predicate

- `micro-op/materialization-predicate/pand`
  - 观察：当前 case 已按 `pto.pand %lhs, %rhs` surface 书写；失败停在 VPTO LLVM emitter 路径构造 `scf.for` 时缺少 dialect 注册，而不是 parser/ODS
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 emitter pipeline / dialect 注册缺口
- `micro-op/materialization-predicate/pdintlv_b8`
  - 观察：已按当前双输入双结果 surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.pset_b8`，导致该 case 无法继续验证目标
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，但继续推进时被相关 emitter 实现缺口阻塞
- `micro-op/materialization-predicate/pdintlv_b8-nontrivial`
  - 观察：已按当前双输入双结果 surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.pset_b8`，导致该 case 无法继续验证目标
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，但继续推进时被相关 emitter 实现缺口阻塞
- `micro-op/materialization-predicate/pge-tail-mask`
  - 观察：已按当前 pattern attribute surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.pge_b8`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，但继续推进时被相关 emitter 实现缺口阻塞
- `micro-op/materialization-predicate/pge-tail-mask-boundary`
  - 观察：已按当前 pattern attribute surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.pge_b8`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，但继续推进时被相关 emitter 实现缺口阻塞
- `micro-op/materialization-predicate/pintlv_b16`
  - 观察：已按当前双输入双结果 surface 重写；失败已收敛为 VPTO LLVM emitter 不接受该 case 需要的 `pset_b16` pattern 组合
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，但继续推进时被当前 verifier/emitter 组合限制阻塞
- `micro-op/materialization-predicate/pintlv_b16-nontrivial`
  - 观察：已按当前双输入双结果 surface 重写；失败已收敛为 VPTO LLVM emitter 不接受该 case 需要的 `pset_b16` pattern 组合
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，但继续推进时被当前 verifier/emitter 组合限制阻塞
- `micro-op/materialization-predicate/plt-tail-mask`
  - 观察：已按当前 `%mask, %scalar_out = pto.plt_b* %scalar` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.plt_b8`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，但继续推进时被相关 emitter 实现缺口阻塞
- `micro-op/materialization-predicate/plt-tail-mask-boundary`
  - 观察：已按当前 `%mask, %scalar_out = pto.plt_b* %scalar` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.plt_b8`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，但继续推进时被相关 emitter 实现缺口阻塞
- `micro-op/materialization-predicate/pnot`
  - 观察：已按当前 `pto.pnot %input, %mask` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.pnot`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/materialization-predicate/por`
  - 观察：当前 case 已按 `pto.por %lhs, %rhs` surface 书写；失败停在 VPTO LLVM emitter 路径构造 `scf.for` 时缺少 dialect 注册，而不是 parser/ODS
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 emitter pipeline / dialect 注册缺口
- `micro-op/materialization-predicate/ppack-punpack`
  - 观察：已按当前 `pto.ppack/punpack %input, \"PART\"` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.ppack`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/materialization-predicate/ppack-punpack-nontrivial`
  - 观察：按当前 surface 可写，但 `pto.ppack` verifier 目前只接受 `LOWER`；case 继续推进后又暴露出 emitter 对相关 pattern / op 支持不足
  - 结论：当前不应通过弱化目标来规避；该 case 的目标组合在现 surface 下受 verifier 限制，属于接口/实现共同缺口
- `micro-op/materialization-predicate/psel`
  - 观察：已按当前 `pto.psel %src0, %src1, %mask` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.psel`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/materialization-predicate/psel-tail-predicate`
  - 观察：已按当前 `pto.psel %src0, %src1, %mask` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.psel`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 tail-predicate surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/materialization-predicate/pset-pattern`
  - 观察：已按当前 pattern attribute surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.pset_b8`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 pattern surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/materialization-predicate/pset-pattern-fragment`
  - 观察：已按当前 pattern attribute surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.pset_b8`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 pattern surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/materialization-predicate/pxor`
  - 观察：当前 case 已按 `pto.pxor %lhs, %rhs` surface 书写；失败停在 VPTO LLVM emitter 路径构造 `scf.for` 时缺少 dialect 注册，而不是 parser/ODS
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 emitter pipeline / dialect 注册缺口

## predicate-load-store

- `micro-op/predicate-load-store/pst-pld`
  - 观察：当前 case 目标要求 `packed-predicate-roundtrip`，但 `docs/isa/04-predicate-load-store.md` 中 `pto.pst` 支持 `NORM/PK`，`pto.pld` 只定义了 load dist `NORM/US/DS`；目标 op 组合自身没有文档化的对称 packed roundtrip surface
  - 结论：按当前 docs/isa 无法在不改变目标的前提下忠实写出；这是文档/语义缺口，不应靠猜测 load 语义补 case
- `micro-op/predicate-load-store/psti-pldi`
  - 观察：当前 case 目标要求 `packed-predicate-roundtrip`，但 `docs/isa/04-predicate-load-store.md` 中 `pto.psti` 支持 `NORM/PK`，`pto.pldi` 只定义了 load dist `NORM/US/DS`；目标 op 组合自身没有文档化的对称 packed roundtrip surface
  - 结论：按当前 docs/isa 无法在不改变目标的前提下忠实写出；这是文档/语义缺口，不应靠猜测 load 语义补 case
- `micro-op/predicate-load-store/psts-plds`
  - 观察：当前 case 目标要求 `packed-predicate-roundtrip`，但 `docs/isa/04-predicate-load-store.md` 中 `pto.psts` / `pto.plds` 只有普通标量偏移 UB store/load surface，并没有 packed 变体；不能在不偷换测试目标的前提下改写为合法 packed case
  - 结论：按当前 docs/isa 无法在不改变目标的前提下忠实写出；这是文档/语义缺口，不应通过降级为普通 roundtrip 来规避
- `micro-op/predicate-load-store/psts-plds-packed-prefix-boundary`
  - 观察：当前目标要求 `packed-predicate-roundtrip`，但 `docs/isa/04-predicate-load-store.md` 中 `pto.psts` / `pto.plds` 只有标量偏移的普通 UB store/load surface；`PK` 只出现在 `pto.pst` / `pto.psti`，并不属于本 case 的 target op
  - 结论：按当前 docs/isa 无法在不改变目标的前提下忠实写出；这是文档/语义缺口，不应通过替换 target op 来规避
- `micro-op/predicate-load-store/pstu`
  - 观察：已按当前 `pto.pstu %align_in, %value, %base -> %align_out, %base_out` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.pstu`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/predicate-load-store/pstu-state-advance-boundary`
  - 观察：已按当前 `pto.pstu %align_in, %value, %base -> %align_out, %base_out` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.pstu`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 state-update surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口

## rearrangement

- `micro-op/rearrangement/vintlv-vdintlv`
  - 观察：已按当前双输入双结果 surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vintlv`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/rearrangement/vintlv-vdintlv-lane-boundary`
  - 观察：已按当前双输入双结果 surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vintlv`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/rearrangement/vpack`
  - 观察：已按当前 `pto.vpack %src0, %src1, %part` surface 重写为整数 pack 形式；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vpack`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/rearrangement/vperm`
  - 观察：已按当前 `pto.vperm %src, %index` surface 重写为显式索引向量形式；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vperm`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/rearrangement/vshift`
  - 观察：已按当前 `pto.vshift %src, %amt` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vshift`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/rearrangement/vshift-tail-zero-fill`
  - 观察：已按当前 `pto.vshift %src, %amt` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vshift`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/rearrangement/vslide`
  - 观察：已按当前 `pto.vslide %src0, %src1, %amt` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vslide`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/rearrangement/vslide-tail-window`
  - 观察：已按当前 `pto.vslide %src0, %src1, %amt` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vslide`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/rearrangement/vsqz`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vsqz
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/rearrangement/vsqz-nontrivial-mask`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vsqz
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/rearrangement/vsunpack`
  - 观察：已按当前 `pto.vsunpack %src, %part` surface 重写为整数 unpack 形式；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsunpack`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/rearrangement/vusqz`
  - 观察：当前 `pto.vusqz` surface 只有 `%mask -> %result`，文档语义却依赖一个“source-front stream”隐式输入；若保持现有测试目标“predicate-driven-rearrangement, placement”，当前无法仅依据 docs 写出有稳定 oracle 的 case
  - 结论：当前问题不是 parser 本身，而是 surface / 文档不足以支撑稳定测试目标；不能为了 parse 成功伪造额外输入或弱化目标
- `micro-op/rearrangement/vusqz-nontrivial-mask`
  - 观察：同 `micro-op/rearrangement/vusqz`，当前文档和 surface 都不足以为“非平凡 placement”给出稳定 oracle；继续编写只会得到失真样例
  - 结论：当前问题不是 parser 本身，而是 surface / 文档不足以支撑稳定测试目标；不能为了 parse 成功伪造额外输入或弱化目标
- `micro-op/rearrangement/vzunpack`
  - 观察：已按当前 `pto.vzunpack %src, %part` surface 重写为整数 unpack 形式；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vzunpack`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口

## reduction

- `micro-op/reduction/vcgadd`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vcgadd
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/reduction/vcgadd-tail`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vcgadd
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/reduction/vcgmax`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vcgmax
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/reduction/vcgmax-tie`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vcgmax
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/reduction/vcgmin`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vcgmin
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/reduction/vcgmin-tie`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vcgmin
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/reduction/vcpadd`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vcpadd
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/reduction/vcpadd-tail`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vcpadd
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过

## unary-vector

- `micro-op/unary-vector/vbcnt`
  - 观察：ODS 类型约束不匹配：pto.vbcnt 仅接受整型向量元素
  - 结论：当前仍属于用例编写问题；要保持目标不变，需要按 docs/isa 将 case 改成整型向量输入后再继续收敛
- `micro-op/unary-vector/vcls`
  - 观察：ODS 类型约束不匹配：pto.vcls 仅接受整型向量元素
  - 结论：当前仍属于用例编写问题；要保持目标不变，需要按 docs/isa 将 case 改成整型向量输入后再继续收敛
- `micro-op/unary-vector/vln`
  - 观察：VPTO LLVM emitter 未支持该 op：pto.vln
  - 结论：当前已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/unary-vector/vln-domain-boundary`
  - 观察：VPTO LLVM emitter 未支持该 op：pto.vln
  - 结论：当前已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/unary-vector/vmov`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vmov
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/unary-vector/vmov-tail`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vmov
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/unary-vector/vneg`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vneg
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/unary-vector/vneg-f32-exceptional`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vneg
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/unary-vector/vnot`
  - 观察：VPTO LLVM emitter 未支持该 op：pto.vnot
  - 结论：当前已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/unary-vector/vrec`
  - 观察：VPTO LLVM emitter 未支持该 op：pto.vrec
  - 结论：当前已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/unary-vector/vrec-zero-inf`
  - 观察：VPTO LLVM emitter 未支持该 op：pto.vrec
  - 结论：当前已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/unary-vector/vrelu`
  - 观察：VPTO LLVM emitter 未支持该 op：pto.vrelu
  - 结论：当前已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/unary-vector/vrsqrt`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vrsqrt
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/unary-vector/vrsqrt-zero-inf`
  - 观察：PTO dialect 缺少 op 定义/注册：pto.vrsqrt
  - 结论：当前无法编写合法用例进入 parse；这是 PTO dialect surface 缺口，不应通过修改测试目标绕过
- `micro-op/unary-vector/vsqrt`
  - 观察：VPTO LLVM emitter 未支持该 op：pto.vsqrt
  - 结论：当前已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/unary-vector/vsqrt-domain-boundary`
  - 观察：VPTO LLVM emitter 未支持该 op：pto.vsqrt
  - 结论：当前已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口

## vec-scalar

- `micro-op/vec-scalar/vaddcs`
  - 观察：当前 case 已按 `pto.vaddcs %lhs, %rhs, %carry_in, %mask -> %result, %carry` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vaddcs`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/vec-scalar/vaddcs-carry-boundary`
  - 观察：当前 case 已按 `pto.vaddcs %lhs, %rhs, %carry_in, %mask -> %result, %carry` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vaddcs`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/vec-scalar/vands`
  - 观察：已按 docs 改成 `ui16` 向量 + `ui16` 标量 + `!pto.mask` 形式；失败已前移为 VPTO LLVM emitter 未支持 `pto.plt_b16`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与写法一致，阻塞点是 `plt_b16` 相关 emitter 实现缺口
- `micro-op/vec-scalar/vands-mask-edge`
  - 观察：已按 docs 改成 `ui16` 向量 + `ui16` 标量 + tail-mask 形式；失败已前移为 VPTO LLVM emitter 未支持 `pto.plt_b16`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与写法一致，阻塞点是 `plt_b16` 相关 emitter 实现缺口
- `micro-op/vec-scalar/vors`
  - 观察：已按 docs 改成 `ui16` 向量 + `ui16` 标量 + `!pto.mask` 形式；失败已前移为 VPTO LLVM emitter 未支持 `pto.plt_b16`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与写法一致，阻塞点是 `plt_b16` 相关 emitter 实现缺口
- `micro-op/vec-scalar/vors-mask-edge`
  - 观察：已按 docs 改成 `ui16` 向量 + `ui16` 标量 + tail-mask 形式；失败已前移为 VPTO LLVM emitter 未支持 `pto.plt_b16`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与写法一致，阻塞点是 `plt_b16` 相关 emitter 实现缺口
- `micro-op/vec-scalar/vshls`
  - 观察：已按 docs 改成 `ui16` 向量 + `ui16` 标量 + `!pto.mask` 形式；失败已前移为 VPTO LLVM emitter 未支持 `pto.plt_b16`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与写法一致，阻塞点是 `plt_b16` 相关 emitter 实现缺口
- `micro-op/vec-scalar/vshls-shift-boundary`
  - 观察：已按 docs 改成 `ui16` 向量 + `ui16` 标量 + tail-mask 形式；失败已前移为 VPTO LLVM emitter 未支持 `pto.plt_b16`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与写法一致，阻塞点是 `plt_b16` 相关 emitter 实现缺口
- `micro-op/vec-scalar/vshrs`
  - 观察：已按 docs 改成 `ui16` 向量 + `ui16` 标量 + `!pto.mask` 形式；失败已前移为 VPTO LLVM emitter 未支持 `pto.plt_b16`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与写法一致，阻塞点是 `plt_b16` 相关 emitter 实现缺口
- `micro-op/vec-scalar/vshrs-shift-boundary`
  - 观察：已按 docs 改成 `ui16` 向量 + `ui16` 标量 + tail-mask 形式；失败已前移为 VPTO LLVM emitter 未支持 `pto.plt_b16`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与写法一致，阻塞点是 `plt_b16` 相关 emitter 实现缺口
- `micro-op/vec-scalar/vsubcs`
  - 观察：当前 case 已按 `pto.vsubcs %lhs, %rhs, %carry_in, %mask -> %result, %carry` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsubcs`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/vec-scalar/vsubcs-borrow-boundary`
  - 观察：当前 case 已按 `pto.vsubcs %lhs, %rhs, %carry_in, %mask -> %result, %carry` surface 书写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsubcs`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/vec-scalar/vsubs`
  - 观察：已按当前 `pto.vsubs %input, %scalar, %mask` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsubs`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/vec-scalar/vsubs-tail`
  - 观察：已按当前 `pto.vsubs %input, %scalar, %mask` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsubs`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 tail-mask surface 忠实写出，阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/vec-scalar/vxors`
  - 观察：已按 docs 改成 `ui16` 向量 + `ui16` 标量 + `!pto.mask` 形式；失败已前移为 VPTO LLVM emitter 未支持 `pto.plt_b16`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与写法一致，阻塞点是 `plt_b16` 相关 emitter 实现缺口
- `micro-op/vec-scalar/vxors-mask-edge`
  - 观察：已按 docs 改成 `ui16` 向量 + `ui16` 标量 + tail-mask 形式；失败已前移为 VPTO LLVM emitter 未支持 `pto.plt_b16`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与写法一致，阻塞点是 `plt_b16` 相关 emitter 实现缺口

## vector-load-store

- `micro-op/vector-load-store/vldas-vldus`
  - 观察：已按当前 `vldas/vldus` surface 重写为 `pto.vecscope` 内的 unaligned stream 最小路径；`ptoas --vpto-emit-hivm-llvm` 可通过
  - 结论：已按当前 `vldas/vldus` surface 重写为 `pto.vecscope` 内的 unaligned stream 最小路径；`ptoas --vpto-emit-hivm-llvm` 可通过
- `micro-op/vector-load-store/vldas-vldus-state-chain`
  - 观察：已按当前 `vldas/vldus` surface 重写为 `pto.vecscope` 内的双步 state-chain；`ptoas --vpto-emit-hivm-llvm` 可通过
  - 结论：已按当前 `vldas/vldus` surface 重写为 `pto.vecscope` 内的双步 state-chain；`ptoas --vpto-emit-hivm-llvm` 可通过
- `micro-op/vector-load-store/vlds`
  - 观察：已改为真正的 `vlds + vsts` contiguous case，并显式使用 `{dist = "NORM"}`；`ptoas --vpto-emit-hivm-llvm` 可通过
  - 结论：已改为真正的 `vlds + vsts` contiguous case，并显式使用 `{dist = "NORM"}`；`ptoas --vpto-emit-hivm-llvm` 可通过
- `micro-op/vector-load-store/vlds-brc-b32`
  - 观察：改为目标语义后，当前 verifier 明确拒绝 `BRC_B32`；这是 surface / 文档 / verifier 不一致，不应弱化成别的 `dist`
  - 结论：改为目标语义后，当前 verifier 明确拒绝 `BRC_B32`；这是 surface / 文档 / verifier 不一致，不应弱化成别的 `dist`
- `micro-op/vector-load-store/vlds-tail`
  - 观察：已改为真正的 tail-mask `vlds + vsts` case；`ptoas --vpto-emit-hivm-llvm` 可通过
  - 结论：已改为真正的 tail-mask `vlds + vsts` case；`ptoas --vpto-emit-hivm-llvm` 可通过
- `micro-op/vector-load-store/vldx2-layout-check`
  - 观察：已按当前 `pto.vldx2 %src[%off], "DINTLV_*"` surface 重写，并用 `pto.vstx2` 保留 layout 可观测路径；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vldx2`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/vector-load-store/vldx2-vstx2`
  - 观察：已按当前 `pto.vldx2` / `pto.vstx2` surface 重写为最小 deinterleave/interleave roundtrip；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vldx2`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/vector-load-store/vsld`
  - 观察：已按当前 `pto.vsld %source[%offset], \"STRIDE\"` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsld`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/vector-load-store/vsld-vsst-stride-boundary`
  - 观察：已按当前 `pto.vsld/pto.vsst` strided memory surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsld`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是相关 VPTO LLVM emitter 实现缺口
- `micro-op/vector-load-store/vsldb`
  - 观察：`docs/isa/03-vector-load-store.md` 只说明 `%offset` 是“packed stride/control word”，但没有给出可据此构造 testcase 的编码规则；在不臆造 control word 的前提下，block-strided-load 目标无法被忠实写出
  - 结论：当前 skeleton case 本身不符合目标，也不能按现有文档把目标改写成合法 surface；应记录为文档/接口缺口，而不是继续修补这份错误 skeleton
- `micro-op/vector-load-store/vsst`
  - 观察：已按当前 `pto.vsst %value, %dest[%offset], \"STRIDE\"` surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsst`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
- `micro-op/vector-load-store/vsstb`
  - 观察：`docs/isa/03-vector-load-store.md` 只说明 `%offset` 是“packed stride/control word”，但没有给出可据此构造 testcase 的编码规则；在不臆造 control word 的前提下，block-strided-store 目标无法被忠实写出
  - 结论：当前 skeleton case 本身不符合目标，也不能按现有文档把目标改写成合法 surface；应记录为文档/接口缺口，而不是继续修补这份错误 skeleton
- `micro-op/vector-load-store/vsta`
  - 观察：`pto.vsta` 本身 surface 明确，但文档要求它消费“preceding unaligned-store stream”的 pending store-alignment state；当前能稳定写出的 unaligned store producer 仍受 `vstu/vstus/vstur` 文档/ODS 漂移影响，因此无法在不偷换测试目标的前提下构造有意义的 case
  - 结论：当前 skeleton case 明显没有测到目标语义；按现有 docs/ODS 也无法在不改变目标的前提下补出合法 producer/consumer 链，故记录为接口漂移
- `micro-op/vector-load-store/vsta-state-advance`
  - 观察：与 `vsta` 相同，`state-advance` 目标依赖真实 unaligned store state producer；在 `vstu/vstus/vstur` surface 未稳定前，不能忠实写出
  - 结论：当前 skeleton case 明显没有测到目标语义；按现有 docs/ODS 也无法在不改变目标的前提下补出合法 state-advance 路径，故记录为接口漂移
- `micro-op/vector-load-store/vstar`
  - 观察：用例文本写法不合法：无结果 op 被写成了带结果赋值形式
  - 结论：已修成 `pto.vecscope` 内的无结果写法；当前失败已收敛为 VPTO LLVM emitter 尚未支持 `pto.vstar`
- `micro-op/vector-load-store/vstas`
  - 观察：`pto.vstas` 的 scalar-offset flush surface 明确，但其输入仍应来自 preceding unaligned-store stream；当前缺少稳定、已敲定的 `vstu/vstus/vstur` case-writing surface，不能只为过 parser 伪造来源
  - 结论：当前 skeleton case 明显没有测到目标语义；按现有 docs/ODS 也无法在不改变目标的前提下补出合法 upstream state，故记录为接口漂移
- `micro-op/vector-load-store/vstas-vstus-offset-update`
  - 观察：该 case 同时要求 `vstas` 与 `vstus`；其中 `vstus` 仍存在文档/ODS 关于参数列表与 `mode` 的漂移，因此不能忠实落成当前 surface，也无法支撑 `offset-update` 目标
  - 结论：当前 skeleton case 明显没有测到目标语义；并且目标依赖的 `vstus` surface 自身未稳定，故只能记录为文档/接口漂移
- `micro-op/vector-load-store/vsts`
  - 观察：已改为当前 `pto.vsts %value, %dst[%off], %mask` surface；`ptoas --vpto-emit-hivm-llvm` 可通过
  - 结论：case 目标与当前写法一致；用例编写和 parse 已收敛
- `micro-op/vector-load-store/vsts-tail`
  - 观察：已改为真正的 tail-mask `vlds + vsts` case；`ptoas --vpto-emit-hivm-llvm` 可通过
  - 结论：case 目标与当前写法一致；用例编写和 parse 已收敛
- `micro-op/vector-load-store/vstu`
  - 观察：当前 `docs/isa/03-vector-load-store.md` 未给出与 `VPTOOps.td` 一致的 `mode` 参数表面；在不擅自发明 `mode` 的前提下，case 目标无法继续收敛到合法 surface
  - 结论：当前 skeleton case 本身不符合目标；同时文档/ODS 未提供可忠实落地的合法 surface，因此只能记录为接口漂移
- `micro-op/vector-load-store/vstu-state-advance`
  - 观察：当前 `docs/isa/03-vector-load-store.md` 未给出与 `VPTOOps.td` 一致的 `mode` 参数表面；在不擅自发明 `mode` 的前提下，case 目标无法继续收敛到合法 surface
  - 结论：当前 skeleton case 本身不符合目标；同时文档/ODS 未提供可忠实落地的合法 state-advance surface，因此只能记录为接口漂移
- `micro-op/vector-load-store/vstur`
  - 观察：当前 `docs/isa/03-vector-load-store.md` 未给出与 `VPTOOps.td` 一致的 `mode` 参数表面；在不擅自发明 `mode` 的前提下，case 目标无法继续收敛到合法 surface
  - 结论：当前 skeleton case 本身不符合目标；同时文档/ODS 未提供可忠实落地的合法 surface，因此只能记录为接口漂移
- `micro-op/vector-load-store/vstus`
  - 观察：当前 `docs/isa/03-vector-load-store.md` 未给出与 `VPTOOps.td` 一致的 `mode` 参数表面；在不擅自发明 `mode` 的前提下，case 目标无法继续收敛到合法 surface
  - 结论：当前 skeleton case 本身不符合目标；同时文档/ODS 未提供可忠实落地的合法 surface，因此只能记录为接口漂移
- `micro-op/vector-load-store/vstx2-layout-check`
  - 观察：已按当前 `pto.vstx2 %low, %high, %dst[%off], "INTLV_*", %mask` surface 重写，并保留双向量 layout 可观测路径；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vstx2`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是 VPTO LLVM emitter 实现缺口
