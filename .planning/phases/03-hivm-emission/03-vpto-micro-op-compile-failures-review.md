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
下列 case 很可能也属于“优先修测例或修 surface 对齐”的范围，但在本轮只做了模式级确认，下一阶段应按同 family 批量复查：

- `compare-select` 中其余 `vcmps` / `vselr` 相关 case
- `vector-load-store` 中其余 `vld*` / `vsld*` / `vst*` 相关 case
- `predicate-load-store` 中 `pst*` / `pld*` / `pstu*` 相关 case
- `materialization-predicate` 中 `p*` 变换类 case

## binary-vector

- `micro-op/binary-vector/vaddc`
  - 观察：当前 case 已按双结果 `pto.vaddc %lhs, %rhs, %mask -> %result, %carry` surface 书写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/binary-vector/vaddc-carry-boundary`
  - 观察：当前 case 已按双结果 `pto.vaddc %lhs, %rhs, %mask -> %result, %carry` surface 书写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/binary-vector/vshl`
  - 观察：case 已改成整型向量路径；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 step `4/6`
  - 结论：当前已越过 parse / emitter 阶段；compile-only 可通过
- `micro-op/binary-vector/vshl-shift-boundary`
  - 观察：case 已改成整型向量路径；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 step `4/6`
  - 结论：当前已越过 parse / emitter 阶段；compile-only 可通过
- `micro-op/binary-vector/vshr`
  - 观察：case 已改成整型向量路径；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 step `4/6`
  - 结论：当前已越过 parse / emitter 阶段；compile-only 可通过
- `micro-op/binary-vector/vshr-shift-boundary`
  - 观察：case 已改成整型向量路径；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 step `4/6`
  - 结论：当前已越过 parse / emitter 阶段；compile-only 可通过
- `micro-op/binary-vector/vsubc`
  - 观察：当前 case 已按双结果 `pto.vsubc %lhs, %rhs, %mask -> %result, %carry` surface 书写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/binary-vector/vsubc-borrow-boundary`
  - 观察：当前 case 已按双结果 `pto.vsubc %lhs, %rhs, %mask -> %result, %carry` surface 书写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径

## compare-select

- `micro-op/compare-select/vcmp-i16-signed`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 step `4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 parse / emitter / toolchain 早期阻塞；后续若继续推进，应关注 runtime / board 路径
- `micro-op/compare-select/vcmp-i16-unsigned`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 step `4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 parse / emitter / toolchain 早期阻塞；后续若继续推进，应关注 runtime / board 路径
- `micro-op/compare-select/vcmps-i16-signed`
  - 观察：ODS 类型约束不匹配：pto.vcmps 的标量类型需与源向量元素类型一致
  - 结论：当前仍属于用例编写问题；要保持目标不变，需要按 docs/isa 将标量操作数改成与向量元素一致的类型后再继续收敛
- `micro-op/compare-select/vcmps-i16-unsigned`
  - 观察：case 当前仍保持 `ui16` scalar-operand 目标，但已改成 `i16 constant + builtin.unrealized_conversion_cast -> ui16` 的最小桥接形式，不再依赖与目标无关的 scalar-load lowering；`2026-04-02` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
  - 结论：当前已不是用例编写或 parse 问题；在不改变目标语义的前提下，compile-only 可通过
- `micro-op/compare-select/vcmps-unordered-f32`
  - 观察：已按当前 `pto.vcmps` surface 重写为向量+标量+mask 形式；`ptoas --vpto-emit-hivm-llvm` step 1 可通过
  - 结论：当前 surface 下可忠实写出；用例编写和 parse 已收敛
- `micro-op/compare-select/vsel`
  - 观察：当前 case 已按 `pto.vsel %src0, %src1, %mask` surface 书写；installed wrapper `npu_arch_3101/__clang_cce_vector_intrinsics.h` 明确是 `vsel(dst, src0, src1, mask)`，与当前 PTO surface 一致；`strings bisheng` 也确认存在 `llvm.hivm.vsel.v64f32` family。进一步对 `f32/v64f32` 与 `.x/.z` 四种最小 `.ll` spelling 分别做直接 Bisheng 编译，全部在 step 2 的 instruction selection 同一路径崩溃
  - 结论：当前已不是用例编写、parse 或单纯 intrinsic spelling 问题；case 可按现有 surface 忠实写出，阻塞点已收敛为 Bisheng/toolchain 的 codegen 路径
- `micro-op/compare-select/vsel-i16`
  - 观察：当前 case 已按 `pto.vsel %src0, %src1, %mask` surface 书写；installed wrapper 明确存在 `vsel(vector_s16 &dst, vector_s16 src0, vector_s16 src1, vector_bool mask)`；repo 生成 `llvm.hivm.vsel.v128s16.z` 后已越过 step 1，但 bisheng 在 step 2 type legalization 阶段报 `Do not know how to expand the result of this operator!`
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，阻塞点已前移到 Bisheng/toolchain 的 type legalization / codegen 路径，而不是 VPTO emitter 缺口
- `micro-op/compare-select/vsel-predicate-edge`
  - 观察：当前 case 已按 `pto.vsel %src0, %src1, %mask` surface 书写；repo 已生成 `llvm.hivm.vsel.v64f32` 调用，installed headers 与 `strings bisheng` 也确认 `vsel` family 存在；对 `vsel` 最小 `.ll` probe 的多种 spelling 复核同样全部在 instruction selection 阶段崩溃
  - 结论：当前已不是用例编写或普通 emitter 缺口；case 可按现有 surface 忠实写出，阻塞点已前移到 Bisheng/toolchain 的 codegen 路径
- `micro-op/compare-select/vsel-tail`
  - 观察：当前 case 已按 tail-mask `pto.vsel %src0, %src1, %mask` surface 书写；installed wrapper 与 `strings bisheng` 均确认 `vsel` family 存在；repo 生成 `.ll` 已越过 step 1，但 bisheng 在 step 2 的 instruction selection 阶段对 `@vsel_tail_kernel_2d.vector.thread` 崩溃退出
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 tail-mask surface 忠实写出，阻塞点已前移到 Bisheng/toolchain 的 codegen 路径，而不是 VPTO emitter 缺口
- `micro-op/compare-select/vselr`
  - 观察：`2026-04-03` 已按 `%result = pto.vselr %src, %idx : !pto.vreg<NxT>, !pto.vreg<Nxi<width>> -> !pto.vreg<NxT>` 重写 case，并将 golden 更新为按 idx 向量从源向量取数的 lane 重排 oracle；`f32` 路径现按 `bitcast <64xf32> -> <64xi32> -> llvm.hivm.vselr.v64u32 -> bitcast <64xf32>` 方式导出 LLVM
  - 结论：compile-only 已通过，当前失败评审中不再保留为 blocker

## conversion

- `micro-op/conversion/vtrc-rounding-boundary`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 parse / emitter / toolchain 早期阻塞；compile-only 可通过
- `micro-op/conversion/vcvt-f16-special`
  - 观察：已按 `f16 -> f32` widening contract 重写为异常值输入 case；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与现有写法一致，compile-only 可通过
- `micro-op/conversion/vcvt-f16-to-f32`
  - 观察：已按 `!pto.vreg<128xf16> -> !pto.vreg<64xf32>` widening contract 重写，采用 `vlds {dist = "UNPK_B16"} + PART_EVEN/PART_ODD` 覆盖完整输出；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与现有写法一致，compile-only 可通过
- `micro-op/conversion/vcvt-f32-special`
  - 观察：已按 `f32 -> f16` narrowing contract 重写为异常值输入 case；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与现有写法一致，compile-only 可通过
- `micro-op/conversion/vcvt-f32-to-f16`
  - 观察：已按 `!pto.vreg<64xf32> -> !pto.vreg<128xf16>` narrowing contract 重写，采用 `PART_EVEN/PART_ODD + vor` 合并完整结果；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与现有写法一致，compile-only 可通过
- `micro-op/conversion/vcvt-tail`
  - 观察：已按 `LOGICAL_ELEMS=1000` 前缀场景重写为真实 `f32 -> f16` tail case；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与现有写法一致，compile-only 可通过
- `micro-op/conversion/vcvt-tail-special`
  - 观察：已按异常值前缀 + `LOGICAL_ELEMS=1000` 场景重写为真实 `f32 -> f16` tail case；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
  - 结论：当前已不是用例编写或 parse 问题；case 目标与现有写法一致，compile-only 可通过

## dsa-sfu

- `micro-op/dsa-sfu/vaxpy-f32`
  - 观察：当前 case 已按 `pto.vaxpy %src0, %src1, %alpha` surface 书写；本轮 emitter 已接到 `llvm.hivm.vaxpy.v64f32.x`，但 bisheng 在 instruction selection 阶段仍报 `Cannot select`
  - 结论：当前阻塞点已收敛为 LLVM intrinsic contract 未收口，而不是 parser / ODS 或普通 emitter 缺口；在拿到 installed frontend 的真实 LLVM 形状前不能继续猜参数表
- `micro-op/dsa-sfu/vci`
  - 观察：当前 case 已按 `pto.vci %index {order = "ASC"}` surface 书写；本轮 emitter 已接到 `llvm.hivm.vci.v64s32`，repo 当前生成 `declare <64 x i32> @llvm.hivm.vci.v64s32(i32, i64)`，但 bisheng verifier 仍报 `Intrinsic has incorrect argument type`
  - 结论：当前阻塞点已收敛为 LLVM intrinsic ABI 未收口，而不是 parser / ODS 或普通 emitter 缺口；在拿到 installed frontend 的真实 LLVM 形状前不能继续猜参数表
- `micro-op/dsa-sfu/vexpdiff-boundary`
  - 观察：已按 installed wrapper contract 收口为 `pto.vexpdiff %input, %max {part = "PART_*"} : ... -> !pto.vreg<...xf32>`；LLVM 已导出为 `llvm.hivm.vexpdif.v64f32f32(<64 x float>, <64 x float>, <256 x i1>, i32)`
  - 结论：当前已不是 parser / emitter 缺口；本轮 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
- `micro-op/dsa-sfu/vexpdiff-f32`
  - 观察：已按 installed wrapper contract 收口为 `pto.vexpdiff %input, %max {part = "PART_*"} : ... -> !pto.vreg<...xf32>`；LLVM 已导出为 `llvm.hivm.vexpdif.v64f32f32(<64 x float>, <64 x float>, <256 x i1>, i32)`
  - 结论：当前已不是 parser / emitter 缺口；本轮 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
- `micro-op/dsa-sfu/vmula`
  - 观察：已按当前 `pto.vmula %acc, %lhs, %rhs, %mask` surface 重写；本轮 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/dsa-sfu/vmula-accumulator-boundary`
  - 观察：已按当前 `pto.vmula %acc, %lhs, %rhs, %mask` surface 重写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/dsa-sfu/vmull`
  - 观察：已按当前 `pto.vmull %lhs, %rhs, %mask -> %low, %high` surface 重写；本轮 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/dsa-sfu/vprelu-f32`
  - 观察：当前 case 已按 `pto.vprelu %input, %alpha` surface 书写；installed wrapper 已确认 `vprelu(dst, src0, src1, mask, mode)` binary-op 形式，merge 路径还会显式经由 `vmov(dst, dstTmp, mask)`
  - 结论：blocked。当前 PTO surface 与 installed/LLVM 侧的 `merge-dst + 2 src + mask` 关系未收口；在不改变语义的前提下不能直接按“普通 binary op emitter 缺口”处理
- `micro-op/dsa-sfu/vprelu-tail`
  - 观察：当前 case 已按 `pto.vprelu %input, %alpha` surface 书写；installed wrapper 已确认 `vprelu(dst, src0, src1, mask, mode)` binary-op 形式，merge 路径还会显式经由 `vmov(dst, dstTmp, mask)`
  - 结论：blocked。当前 PTO surface 与 installed/LLVM 侧的 `merge-dst + 2 src + mask` 关系未收口；在不改变语义的前提下不能直接按“普通 binary op emitter 缺口”处理

## gather-scatter

- `micro-op/gather-scatter/vgather2`
  - 观察：当前 case 已按 `pto.vgather2 %source, %offsets, %active_lanes` surface 书写；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library
  - 结论：case 目标与当前写法一致；当前已越过 parse / emitter 缺口，compile-only 可通过
- `micro-op/gather-scatter/vgather2-duplicate-index`
  - 观察：当前 case 已按 `pto.vgather2 %source, %offsets, %active_lanes` surface 书写；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library
  - 结论：case 目标与当前写法一致；当前已越过 parse / emitter 缺口，compile-only 可通过
- `micro-op/gather-scatter/vgather2_bc`
  - 观察：当前 case 已按 `pto.vgather2_bc %source, %offsets, %mask` surface 书写；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library
  - 结论：case 目标与当前写法一致；当前已越过 parse / emitter 缺口，compile-only 可通过
- `micro-op/gather-scatter/vgather2_bc-sparse-mask`
  - 观察：当前 case 已按 `pto.vgather2_bc %source, %offsets, %mask` surface 书写；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library
  - 结论：case 目标与当前写法一致；当前已越过 parse / emitter 缺口，compile-only 可通过
- `micro-op/gather-scatter/vgatherb`
  - 观察：当前 case 已按 `pto.vgatherb %source, %offsets, %active_lanes` surface 书写；但 installed A5 v300 wrapper 只观察到 `vgatherb(dst, base, vector_u32 indexOffset)`，没有 docs/ODS 里的 `active_lanes`。本轮按 `active_lanes -> plt mask` 产出的 `.ll` 被 bisheng verifier 以 `Intrinsic has incorrect argument type! ptr @llvm.hivm.vgatherb.v300.v64f32` 拒绝；随后已把 emitter 回退为 step `1/6` 显式报 contract 未确认
  - 结论：blocked。当前不是普通 emitter 缺口，而是 PTO/docs surface 与 installed A5 v300 contract 未收口；在确认 `active_lanes` 与真实 LLVM 形态关系前不继续猜语义
- `micro-op/gather-scatter/vgatherb-block-boundary`
  - 观察：当前 case 已按 `pto.vgatherb %source, %offsets, %active_lanes` surface 书写；但 installed A5 v300 wrapper 只观察到 `base + vector_u32 indexOffset` 形式，没有 docs/ODS 中的 `active_lanes`。本轮已把错误的 mask-based lowering 回退为 step `1/6` 显式阻塞
  - 结论：blocked。当前不是普通 emitter 缺口，而是 PTO/docs surface 与 installed A5 v300 contract 未收口；边界 case 先随主 case 一并阻塞
- `micro-op/gather-scatter/vscatter`
  - 观察：当前 case 已按 `pto.vscatter %value, %dest, %offsets, %active_lanes` surface 书写；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library
  - 结论：case 目标与当前写法一致；当前已越过 parse / emitter 缺口，compile-only 可通过
- `micro-op/gather-scatter/vscatter-out-of-order-index`
  - 观察：当前 case 已按 `pto.vscatter %value, %dest, %offsets, %active_lanes` surface 书写；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library
  - 结论：case 目标与当前写法一致；当前已越过 parse / emitter 缺口，compile-only 可通过

## materialization-predicate

- `micro-op/materialization-predicate/pand`
  - 观察：当前 case 已按 `pto.pand %lhs, %rhs, %mask` surface 书写；本轮已越过旧的 `scf.for` dialect 注册缺口，并在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/materialization-predicate/pdintlv_b8`
  - 观察：`pdintlv_b8` 自身 emitter 已可落到 `llvm.hivm.pdintlv.b8`，但当前 case 输入构造仍依赖 `pset_b8`；repo 生成的 `llvm.hivm.pset.b8` 在 bisheng verifier 阶段报 `Intrinsic has incorrect argument type`
  - 结论：当前阻塞点已收敛为上游 `pset_b8` LLVM ABI 未收口，而不是 `pdintlv_b8` 自身 parser / emitter 缺口
- `micro-op/materialization-predicate/pdintlv_b8-nontrivial`
  - 观察：已按当前双输入双结果 surface 重写；`pdintlv_b8` 自身 emitter 已可落到 `llvm.hivm.pdintlv.b8`，但输入构造依赖 `pto.pset_b8`，repo 当前生成的 `llvm.hivm.pset.b8` 会被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，但继续推进时被上游 `pset_b8` 的 LLVM intrinsic ABI 未收口所阻塞
- `micro-op/materialization-predicate/pge-tail-mask`
  - 观察：已按当前 pattern attribute surface 重写；installed wrapper 与 `strings bisheng` 已确认 `pge_b*` family 存在，但 repo 当前生成的 `llvm.hivm.pge.b{8,16,32}(i64, i64)` 会被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝
  - 结论：当前阻塞点已收敛为 LLVM intrinsic ABI 未收口，而不是 parser/ODS 或普通 emitter 缺口；在拿到 installed frontend 的真实 LLVM 形状前不能继续猜参数表
- `micro-op/materialization-predicate/pge-tail-mask-boundary`
  - 观察：与 `micro-op/materialization-predicate/pge-tail-mask` 相同；repo 当前生成的 `llvm.hivm.pge.b{8,16,32}(i64, i64)` 会被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝
  - 结论：当前阻塞点已收敛为 LLVM intrinsic ABI 未收口，而不是 parser/ODS 或普通 emitter 缺口；在拿到 installed frontend 的真实 LLVM 形状前不能继续猜参数表
- `micro-op/materialization-predicate/pintlv_b16`
  - 观察：`pintlv_b16` emitter 已接到 `llvm.hivm.pintlv.b16`，但当前 case 输入构造仍依赖 `pset_b16`；repo 生成的 `llvm.hivm.pset.b16` 在 bisheng verifier 阶段报 `Intrinsic has incorrect argument type`
  - 结论：当前阻塞点已收敛为上游 `pset_b16` LLVM ABI 未收口，而不是 `pintlv_b16` 自身 parser / emitter 缺口
- `micro-op/materialization-predicate/pintlv_b16-nontrivial`
  - 观察：已按当前双输入双结果 surface 重写；`pintlv_b16` 自身 emitter 已可落到 `llvm.hivm.pintlv.b16`，但输入构造依赖 `pto.pset_b16`，repo 当前生成的 `llvm.hivm.pset.b16` 会被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝
  - 结论：当前已不是用例编写或 parse 问题；case 可按现有 surface 忠实写出，但继续推进时被上游 `pset_b16` 的 LLVM intrinsic ABI 未收口所阻塞
- `micro-op/materialization-predicate/plt-tail-mask`
  - 观察：已按当前 `%mask, %scalar_out = pto.plt_b* %scalar` surface 重写；本轮补齐 `plt_b8` emitter 后，`DEVICE=SIM COMPILE_ONLY=1` 已走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/materialization-predicate/plt-tail-mask-boundary`
  - 观察：已按当前 `%mask, %scalar_out = pto.plt_b* %scalar` surface 重写；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
  - 结论：当前已不是 parser / emitter 缺口；compile-only 可通过，后续若继续推进，应关注 runtime / board 路径
- `micro-op/materialization-predicate/pnot`
  - 观察：已按当前 `pto.pnot %input, %mask` surface 重写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/materialization-predicate/por`
  - 观察：当前 case 已按 `pto.por %lhs, %rhs, %mask` surface 书写；本轮已越过旧的 `scf.for` dialect 注册缺口，并在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/materialization-predicate/ppack-punpack`
  - 观察：已按当前 `pto.ppack/punpack %input, \"PART\"` surface 重写；installed wrapper 与 `strings bisheng` 已确认 `ppack/punpack` family 存在，但 repo 当前生成的 `llvm.hivm.ppack.z(<256 x i1>, i64)` / `llvm.hivm.punpack(<256 x i1>, i64)` 会被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝
  - 结论：当前阻塞点已收敛为 LLVM intrinsic ABI 未收口，而不是 parser/ODS 或普通 emitter 缺口；在拿到 installed frontend 的真实 LLVM 形状前不能继续猜参数表
- `micro-op/materialization-predicate/ppack-punpack-nontrivial`
  - 观察：当前 case 仍使用 `LOWER`，已排除先前 verifier 限制干扰；失败点与 `micro-op/materialization-predicate/ppack-punpack` 相同，repo 当前生成的 `llvm.hivm.ppack.z` / `llvm.hivm.punpack` 会被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝
  - 结论：当前阻塞点已收敛为 LLVM intrinsic ABI 未收口，而不是 case 目标本身；不应通过改弱目标来规避
- `micro-op/materialization-predicate/psel`
  - 观察：已按当前 `pto.psel %src0, %src1, %mask` surface 重写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/materialization-predicate/psel-tail-predicate`
  - 观察：已按当前 `pto.psel %src0, %src1, %mask` surface 重写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/materialization-predicate/pset-pattern`
  - 观察：已按当前 pattern attribute surface 重写；installed wrapper 与 `strings bisheng` 已确认 `pset_b*` family 存在，但 repo 当前生成的 `llvm.hivm.pset.b{8,16,32}(i64)` 会被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝
  - 结论：当前阻塞点已收敛为 LLVM intrinsic ABI 未收口，而不是 parser/ODS 或普通 emitter 缺口；在拿到 installed frontend 的真实 LLVM 形状前不能继续猜参数表
- `micro-op/materialization-predicate/pset-pattern-fragment`
  - 观察：与 `micro-op/materialization-predicate/pset-pattern` 相同；repo 当前生成的 `llvm.hivm.pset.b{8,16,32}(i64)` 会被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝
  - 结论：当前阻塞点已收敛为 LLVM intrinsic ABI 未收口，而不是 parser/ODS 或普通 emitter 缺口；在拿到 installed frontend 的真实 LLVM 形状前不能继续猜参数表
- `micro-op/materialization-predicate/pxor`
  - 观察：当前 case 已按 `pto.pxor %lhs, %rhs, %mask` surface 书写；本轮已越过旧的 `scf.for` dialect 注册缺口，并在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径

## predicate-load-store

- `micro-op/predicate-load-store/pst-pld`
  - 观察：`pst/pld` emitter 已接线，repo 当前生成的 `llvm.hivm.pst.b8(<256 x i1>, ptr, i32, i32, i32)` / `llvm.hivm.pld.b8(ptr, i32, i32, i32)` 在 bisheng verifier 阶段报 `Intrinsic has incorrect argument type`
  - 结论：当前阻塞点已收敛为 LLVM intrinsic ABI 未收口；在拿到 installed frontend 的真实 LLVM 形状前不能继续猜参数表
- `micro-op/predicate-load-store/psti-pldi`
  - 观察：已收紧为常量 `index` offset case，并统一为 `pto.psti %value, %base[%offset], "DIST"` / `%result = pto.pldi %base[%offset], "DIST"` surface；repo 当前 lowering 在发射 `llvm.hivm.psti.b8(<256 x i1>, ptr addrspace(6), i32, i32, i32)` / `llvm.hivm.pldi.b8(ptr addrspace(6), i32, i32, i32)` 前将该常量 `index` 转为 `i32`
  - 结论：当前已打通到 compile-only kernel shared library；后续若继续推进，应关注 runtime / board 路径
- `micro-op/predicate-load-store/psts-plds`
  - 观察：已按 `psti/pldi` 同语义的 dynamic-offset 版本收口，并统一为 `pto.psts %value, %base[%offset], "DIST"` / `%result = pto.plds %base[%offset], "DIST"` surface；`plds/psts` emitter 已接线，本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/predicate-load-store/psts-plds-packed-prefix-boundary`
  - 观察：已按 `logical_elems=1000` 的 dynamic-offset packed predicate roundtrip 重写，不再误用旧 compare skeleton
  - 结论：当前已不是 parser / emitter 缺口；本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library，后续若继续推进，应关注 runtime / board 路径
- `micro-op/predicate-load-store/pstu`
  - 观察：已按当前 `pto.pstu %align_in, %value, %base -> %align_out, %base_out` surface 重写；重新 tracing installed wrapper 后，只明确观察到 `__builtin_cce_pstu_b16/b32`，而当前 testcase 仍以 `!pto.ptr<ui8, ub>` 书写
  - 结论：当前阻塞点不是普通 emitter 缺口，而是 docs surface / testcase / installed type contract 未收口；在类型语义明确前不能继续猜 emitter
- `micro-op/predicate-load-store/pstu-state-advance-boundary`
  - 观察：与 `micro-op/predicate-load-store/pstu` 相同；installed wrapper 只明确到 `b16/b32`，当前 `ui8` state-update case 不能证明已与 toolchain 语义对齐
  - 结论：当前阻塞点不是普通 emitter 缺口，而是 docs surface / testcase / installed type contract 未收口；在类型语义明确前不能继续猜 emitter

## rearrangement

- `micro-op/rearrangement/vintlv-vdintlv`
  - 观察：installed wrapper 与 `strings bisheng` 均确认 `vintlv/vdintlv` family 存在；本轮补齐双结果 LLVM emission 后，`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`
  - 结论：当前已越过 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/rearrangement/vintlv-vdintlv-lane-boundary`
  - 观察：与主 case 共享同一双结果 emission 路径；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`
  - 结论：当前已越过 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/rearrangement/vpack`
  - 观察：installed wrapper 当前只明确暴露单输入 `vpack(vector_<narrow> &dst, vector_<wide> src, part, mode)`；而 `docs/isa/12-data-rearrangement.md` / `VPTOOps.td` 当前仍把 `pto.vpack` 定义成双输入 `%src0, %src1, %part`
  - 结论：blocked。当前不是普通 emitter 缺口，而是 PTO/docs surface 与 installed contract 未收口；在确认双输入 surface 如何映射前不继续猜 LLVM lowering
- `micro-op/rearrangement/vslide`
  - 观察：installed wrapper 与 `strings bisheng` 均确认 `vslide` family 存在；本轮补齐 LLVM emission 后，`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`
  - 结论：当前已越过 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/rearrangement/vslide-tail-window`
  - 观察：与主 case 共享同一 emission 路径；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`
  - 结论：当前已越过 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/rearrangement/vsqz`
  - 观察：`pto.vsqz` surface 已存在；installed stub 明确 `store` 形参是 `uint32_t`。本轮把 LLVM emission 中的 `store` 从 `i64` 收紧为 `i32` 后，`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`
  - 结论：当前已越过 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/rearrangement/vsqz-nontrivial-mask`
  - 观察：与主 case 共享同一 `vsqz` emission 路径；`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`
  - 结论：当前已越过 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/rearrangement/vsunpack`
  - 观察：installed stub 明确 `part` 形参可为 `int32_t`；本轮把 LLVM emission 中的 `part` 从 `i64` 收紧为 `i32` 后，`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`
  - 结论：当前已越过 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/rearrangement/vusqz`
  - 观察：当前 `pto.vusqz` surface 只有 `%mask -> %result`，文档语义却依赖一个“source-front stream”隐式输入；若保持现有测试目标“predicate-driven-rearrangement, placement”，当前无法仅依据 docs 写出有稳定 oracle 的 case
  - 结论：当前问题不是 parser 本身，而是 surface / 文档不足以支撑稳定测试目标；不能为了 parse 成功伪造额外输入或弱化目标
- `micro-op/rearrangement/vusqz-nontrivial-mask`
  - 观察：同 `micro-op/rearrangement/vusqz`，当前文档和 surface 都不足以为“非平凡 placement”给出稳定 oracle；继续编写只会得到失真样例
  - 结论：当前问题不是 parser 本身，而是 surface / 文档不足以支撑稳定测试目标；不能为了 parse 成功伪造额外输入或弱化目标
- `micro-op/rearrangement/vzunpack`
  - 观察：当前 zero-extend case 若继续使用 signless `i16 -> i32` 会生成 `s162s32` family；本轮把 case 收紧为 `ui16 -> ui32`，并把 LLVM emission 中的 `part` 收紧为 installed stub 明确的 `i32` 后，`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`
  - 结论：当前已越过 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径

## reduction

- `micro-op/reduction/vcgadd`
  - 观察：当前 case 已按 `pto.vcgadd %src, %mask` surface 落地；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/reduction/vcgadd-tail`
  - 观察：当前 case 已按 `pto.vcgadd %src, %mask` surface 落地；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/reduction/vcgmax`
  - 观察：当前 case 已按 `pto.vcgmax %src, %mask` surface 落地；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/reduction/vcgmax-tie`
  - 观察：当前 case 已按 `pto.vcgmax %src, %mask` surface 落地；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/reduction/vcgmin`
  - 观察：当前 case 已按 `pto.vcgmin %src, %mask` surface 落地；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/reduction/vcgmin-tie`
  - 观察：当前 case 已按 `pto.vcgmin %src, %mask` surface 落地；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/reduction/vcpadd`
  - 观察：当前 case 已按 `pto.vcpadd %src, %mask` surface 落地；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/reduction/vcpadd-tail`
  - 观察：当前 case 已按 `pto.vcpadd %src, %mask` surface 落地；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径

## unary-vector

- `micro-op/unary-vector/vbcnt`
  - 观察：ODS 类型约束不匹配：pto.vbcnt 仅接受整型向量元素
  - 结论：当前仍属于用例编写问题；要保持目标不变，需要按 docs/isa 将 case 改成整型向量输入后再继续收敛
- `micro-op/unary-vector/vcls`
  - 观察：ODS 类型约束不匹配：pto.vcls 仅接受整型向量元素
  - 结论：当前仍属于用例编写问题；要保持目标不变，需要按 docs/isa 将 case 改成整型向量输入后再继续收敛
- `micro-op/unary-vector/vln`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vln-domain-boundary`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vmov`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，当前 LLVM IR 使用 `llvm.hivm.vmov.*.m(src, src, mask)` 形式，可完成 device object 编译和 kernel shared library 链接
  - 结论：blocked。compile-only 虽可走通，但当前走通形式是 `vmov.*.m(src, src, mask)`；PTO unary surface 与 installed/LLVM 侧 merge 形态是否语义等价尚未收口，不能直接据此视为问题已解
- `micro-op/unary-vector/vmov-tail`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，当前 LLVM IR 使用 `llvm.hivm.vmov.*.m(src, src, mask)` 形式，可完成 device object 编译和 kernel shared library 链接
  - 结论：blocked。compile-only 虽可走通，但当前走通形式是 `vmov.*.m(src, src, mask)`；PTO unary surface 与 installed/LLVM 侧 merge 形态是否语义等价尚未收口，不能直接据此视为问题已解
- `micro-op/unary-vector/vneg`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 parse/emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/unary-vector/vneg-f32-exceptional`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 parse/emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/unary-vector/vnot`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 parse/emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/unary-vector/vrec`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vrec-zero-inf`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vrelu`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vrsqrt`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 parse/emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/unary-vector/vrsqrt-zero-inf`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 parse/emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/unary-vector/vsqrt`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vsqrt-domain-boundary`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vabs-f16`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vabs-i16-signed`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vabs-i16-unsigned`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vexp-f16`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vexp-f32-over-underflow`
  - 观察：`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`，可完成 LLVM IR 降低、device object 编译和 kernel shared library 链接
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vbcnt`
  - 观察：installed headers 与 `strings bisheng` 已确认 `vbcnt` contract 存在；本轮补齐 `VPTOLLVMEmitter` 后，`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理
- `micro-op/unary-vector/vcls`
  - 观察：installed headers 与 `strings bisheng` 已确认 `vcls` contract 存在；本轮补齐 `VPTOLLVMEmitter` 后，`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`
  - 结论：当前已越过 `unsupported op` 阶段；后续若继续推进，应关注 runtime / board 路径，而不是再按 emitter 缺口处理

## vec-scalar

- `micro-op/vec-scalar/vaddcs`
  - 观察：当前 case 已按 `pto.vaddcs %lhs, %rhs, %carry_in, %mask -> %result, %carry` surface 书写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/vec-scalar/vaddcs-carry-boundary`
  - 观察：当前 case 已按 `pto.vaddcs %lhs, %rhs, %carry_in, %mask -> %result, %carry` surface 书写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/vec-scalar/vshls`
  - 观察：`2026-04-02` 已按 docs 收口为 `input + i16 scalar + mask` surface，并通过 installed toolchain contract 补齐 `llvm.hivm.vshls.v128u16.x` emission；case 以常量 `i16` shift 值保留 scalar-operand 目标并在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step `4/6`
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/vec-scalar/vshls-shift-boundary`
  - 观察：`2026-04-02` 已按 docs 收口为 `input + i16 scalar + mask` surface，并把 boundary case 固化为 `31` 的 `i16` shift 标量；`DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/vec-scalar/vshrs`
  - 观察：`2026-04-02` 已按 docs 收口为 `input + i16 scalar + mask` surface，并通过 installed toolchain contract 补齐 `llvm.hivm.vshrs.v128u16.x` emission；case 以常量 `i16` shift 值保留 scalar-operand 目标并在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step `4/6`
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/vec-scalar/vshrs-shift-boundary`
  - 观察：`2026-04-02` 已按 docs 收口为 `input + i16 scalar + mask` surface，并把 boundary case 固化为 `31` 的 `i16` shift 标量；`DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/vec-scalar/vsubcs`
  - 观察：当前 case 已按 `pto.vsubcs %lhs, %rhs, %carry_in, %mask -> %result, %carry` surface 书写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/vec-scalar/vsubcs-borrow-boundary`
  - 观察：当前 case 已按 `pto.vsubcs %lhs, %rhs, %carry_in, %mask -> %result, %carry` surface 书写；本轮已在 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step 4/6
  - 结论：当前已不是 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
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
  - 观察：已按当前 `pto.vldx2 %src[%off], "DINTLV_*"` surface 重写，并用 `pto.vstx2` 保留 layout 可观测路径；repo 生成的 `.ll` 已进入 bisheng verifier，但仍报 `Intrinsic has incorrect argument type! ptr @llvm.hivm.vldx2` 与 `ptr @llvm.hivm.vstx2`
  - 结论：blocked。当前不是用例编写问题，也不宜继续猜 `vldx2/vstx2` 参数 ABI；需要先 tracing 完整 repo module 与最小 probe 的差异
- `micro-op/vector-load-store/vldx2-vstx2`
  - 观察：已按当前 `pto.vldx2` / `pto.vstx2` surface 重写为最小 deinterleave/interleave roundtrip；本轮复测里两条 op 都已越过 `unsupported op`，但完整 repo module 在 bisheng verifier 阶段仍报 `Intrinsic has incorrect argument type`
  - 结论：blocked。当前不是用例编写问题，也不再是单纯 emitter 缺口；需要先 tracing 完整 repo module 与最小 probe 的 ABI 差异
- `micro-op/vector-load-store/vsld`
  - 观察：已按当前 `pto.vsld %source[%offset], \"STRIDE\"` surface 重写；installed generic Clang header 已确认 wrapper 为 `vsld(dst, base, offset, stride)`，`strings bisheng` 也确认 `llvm.hivm.vsld` 存在；但 repo 生成的 `.ll` 形式 `declare <64 x float> @llvm.hivm.vsld(ptr addrspace(6), i32, i32, i32)` 会被 bisheng 在 step 2 直接报 `Intrinsic has incorrect argument type`
  - 结论：blocked。当前不是用例编写问题，也不宜继续猜 emitter 参数；首先需要把 `llvm.hivm.vsld` 的正式 LLVM ABI / 参数类型 tracing 清楚
- `micro-op/vector-load-store/vsld-vsst-stride-boundary`
  - 观察：已按当前 `pto.vsld/pto.vsst` strided memory surface 重写；失败已收敛为 VPTO LLVM emitter 未支持 `pto.vsld`
  - 结论：case 目标与当前写法一致，已不是用例编写或 parse 问题；阻塞点是相关 VPTO LLVM emitter 实现缺口
- `micro-op/vector-load-store/vsldb`
  - 观察：`docs/isa`、`docs/vpto-spec`、`VPTOOps.td` 现已统一为 `%block_stride: i16` + `%repeat_stride: i16` surface；case 已按该 surface 重写为 `pto.vecscope` 内合法 block-strided load，并在 `2026-04-03` 本地 `DEVICE=SIM COMPILE_ONLY=1` 走到 step `4/6`
  - 结论：当前已越过 parser / emitter / device compile 阶段；旧的 packed control-word 缺口已收敛
- `micro-op/vector-load-store/vsst`
  - 观察：installed A5 wrapper 当前只直接暴露 `vsst(..., S8_B16)`；而现有 case / docs 场景写的是 `STRIDE_S2_B64`。本轮复测已把失败点收敛为 `unsupported vsst stride immediate`
  - 结论：blocked。当前不是单纯 emitter 缺口，而是 current case/docs stride 与 installed toolchain wrapper 支持面未收口；在不改变测试目标的前提下，应先确认 `vsst` 在 A5 上的正式 stride contract
- `micro-op/vector-load-store/vsstb`
  - 观察：`docs/isa`、`docs/vpto-spec`、`VPTOOps.td` 现已统一为 `%block_stride: i16` + `%repeat_stride: i16` surface；case 已按该 surface 重写为 `pto.vecscope` 内合法 block-strided store，并在 `2026-04-03` 本地 `DEVICE=SIM COMPILE_ONLY=1` 走到 step `4/6`
  - 结论：当前已越过 parser / emitter / device compile 阶段；旧的 packed control-word 缺口已收敛
- `micro-op/vector-load-store/vstar`
  - 观察：无结果 case 已修正为 `pto.vecscope` 内的合法写法；本轮补齐 `vstar` LLVM emission 后，`2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过 `step 4/6`
  - 结论：当前已越过 parser / emitter 缺口；后续若继续推进，应关注 runtime / board 路径
- `micro-op/vector-load-store/vstas`
  - 观察：`pto.vstas` 的 scalar-offset flush surface 明确，但其输入仍应来自 preceding unaligned-store stream；当前缺少稳定、已敲定的 `vstus/vstur` case-writing surface，不能只为过 parser 伪造来源
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
- `micro-op/vector-load-store/vstur`
  - 观察：installed A5 wrapper 明确要求 `vstur(align, src, base, post)`，其中 `post` 为 `POST_UPDATE|NO_POST_UPDATE`；当前 surface 已与 docs/ODS 对齐。其硬件语义为有效地址 `base + AR`，`POST_UPDATE` 时固定 `SPR AR` 会按固定 `SPR SQZN` 更新；独立序列通常需从 `AR=0` 起步
  - 结论：当前不再是 surface 漂移；case 已按四参 `vstur` 形态重写，`--vpto-emit-hivm-llvm` 已可导出 `align = llvm.hivm.vstur(vreg, ptr6, align, post, 0)` 形态。后续若继续推进，应转向 compile-only / bisheng handoff 验证
  - 结论：当前 skeleton case 本身不符合目标；同时文档/ODS 未提供可忠实落地的合法 surface，因此只能记录为接口漂移
- `micro-op/vector-load-store/vstus`
  - 观察：当前 `docs/isa/03-vector-load-store.md` 未给出与 `VPTOOps.td` 一致的 `mode` 参数表面；在不擅自发明 `mode` 的前提下，case 目标无法继续收敛到合法 surface
  - 结论：当前 skeleton case 本身不符合目标；同时文档/ODS 未提供可忠实落地的合法 surface，因此只能记录为接口漂移
- `micro-op/vector-load-store/vstx2-layout-check`
  - 观察：已按当前 `pto.vstx2 %low, %high, %dst[%off], "INTLV_*", %mask` surface 重写，并保留双向量 layout 可观测路径；repo 生成的 `.ll` 已进入 bisheng verifier，但仍报 `Intrinsic has incorrect argument type! ptr @llvm.hivm.vstx2`
  - 结论：blocked。当前不是用例编写问题，也不宜继续猜 `vstx2` 的 LLVM ABI；需要先 tracing 完整 repo module 与最小 probe 的差异
