# VPTO Op Board Test Scope

## Purpose

本文件定义 `test/vpto` 中 VPTO 微指令板测的测试范围，用于约束：

- 哪些 op family 在本轮范围内
- 每类 op 至少需要覆盖哪些类型维度
- 每类 op 至少需要覆盖哪些运行时场景
- 哪些内容只作为基础设施复用，不单独立项
- matrix 中的 `scenarios` 字段应如何描述

本文件是“测什么”的范围文档；具体 case 目录、状态流转和执行顺序仍以：

- `03-vpto-op-board-unit-tests-PLAN.md`
- `03-vpto-op-board-unit-tests-matrix.md`

为准。

## Source Of Truth

测试范围必须建立在以下两类信息的交集上：

1. `docs/isa/*.md` 中文档化的 VPTO surface
2. 当前 VPTO IR 类型系统、op verifier、lowering 和板端链路实际接受的形式

约束如下：

- 文档未声明、但 IR 暂时能打印出来的形式，不直接纳入正式覆盖范围
- 文档声明了、但当前 verifier 或 lowering 不接受的形式，记录为 `blocked`，不伪造用例
- 类型、属性、参数列表、打印形式一律以“文档语义 + 当前实现可落地”的交集为准

## Excluded Infrastructure

以下 op 作为其他 case 的准备动作或收尾动作复用，不单独立项建板测 case：

- pipeline sync
  - `set_flag`
  - `wait_flag`
  - `get_buf`
  - `rls_buf`
  - `barrier`
  - `mem_bar`
- DMA copy / config
  - `copy_gm_to_ubuf`
  - `copy_ubuf_to_gm`
  - `copy_ubuf_to_ubuf`
  - `set_loop*`
- shared wrapper
  - `arith`
  - `scf`

这些 op 仍然必须在具体 case 中正确使用，但不计入“微指令单-op 覆盖完成态”。

## Test Layers

`test/vpto` 中的测试分两层：

- `tileop/`
  - tile 级或派生组合验证
  - 可作为 micro-op case 的骨架参考
  - 不直接计入微指令单-op 覆盖
- `micro-op/`
  - VPTO 微指令单-op 或强耦合小组 case
  - 是本轮覆盖台账的主体

## Coverage Axes

每条 in-scope op 的覆盖都按以下 4 个维度组织：

1. 类型覆盖
2. 主语义覆盖
3. 运行时场景覆盖
4. 结果验证方式

### 类型覆盖

类型覆盖必须和 `docs/isa` 当前显式声明的类型范围一致；只有在 `docs/isa` 已声明，或该 family 尚未在 `docs/isa` 细化但仓库规范文档已稳定约定时，才纳入正式覆盖范围。统一按“类型族”而不是单一 `dtype` 描述：

- `f16`
- `bf16`
- `f32`
- signed integer
- unsigned integer
- `fp8-like`
  - 仅在对应 family 的文档或稳定规范显式声明时纳入
  - 当前命名必须跟随仓库内现有文档口径，例如 `f8e4m3`、`f8e5m2`
  - 不在范围文档里预先扩写成某个 family 必测的通用要求

约束如下：

- 不是所有 op 都要求覆盖全部类型族
- 每个 family 只覆盖文档和实现共同支持的类型族
- signed / unsigned 必须分开记录，不能用“整数”合并描述
- `fp8-like` 只有在对应 family 的文档已经明确进入 surface 时才单独记账

### 主语义覆盖

每条 op 至少需要一个最小主语义 case，记为 `core-<type>`，例如：

- `core-f32`
- `core-f16`
- `core-i16-signed`
- `core-i16-unsigned`
- `core-f8e4m3`

主语义 case 的要求：

- 只保留被测 op 所需的最小数据流
- 前处理 / 主体 / 后处理三段式清晰
- 向量 UB 访问和向量算子位于 `llvm.loop.aivector_scope` 内
- mask 计算位于 `aivector_scope` 内
- 不混入与该 op 主语义无关的复杂控制流

### 运行时场景覆盖

除主语义外，每个 family 需要明确记录代表性场景。不是要求每条 op 全展开，但不能出现整类场景从未被任何 case 触达。

通用场景维度：

- `full-mask`
- `tail-mask`
- aligned access
- unaligned access
- contiguous access
- non-contiguous access
- representative `mode` / `dist`
- immediate vs scalar vs vector operand form
- special values
- alias / no-alias

其中：

- `special values` 主要适用于 float family，例如 `0`、负数、NaN、Inf、近边界值
- `alias / no-alias` 只在文档语义允许且实现可稳定验证时纳入
- `aligned / unaligned`、`contiguous / non-contiguous` 主要适用于 load/store、gather/scatter、rearrangement
- `mode` / `dist` 只在该 op 或 family 语义中真实生效时纳入

对浮点 family，异常值测试属于正式范围，不是可选增强项。只要该 op 的文档语义允许相关输入进入计算，就应至少有一个 case 覆盖 `exceptional-values`。默认异常值集合包括：

- `+0`
- `-0`
- `+inf`
- `-inf`
- `nan`

如某个 family 还需要额外的浮点边界值，可在对应 family 条目或具体 case 中继续补充。

### 结果验证方式

oracle 必须和类型族一致：

- float
  - 默认 `allclose`
  - `fp8-like` 如进入该 family 的正式范围，需明确格式转换和舍入规则
- integer
  - 默认精确比较
- predicate / mask
  - 需定义落盘形式后再比较
- conversion
  - 必须明确舍入、截断、饱和或溢出预期

## Family Scope

本轮优先覆盖以下 family：

- vector-load-store
- predicate-load-store
- materialization-predicate
- unary-vector
- binary-vector
- vec-scalar
- compare-select
- conversion
- reduction
- rearrangement
- gather-scatter
- dsa-sfu

### Binary Vector

主语义：

- `lhs op rhs -> out`

至少关注：

- `core-<type>`
- `full-mask`
- `tail-mask`
- float 特殊值
- signed / unsigned 分离

`pto.vadd` 作为 `binary-vector` family 的首个展开样板，当前预期补齐以下 case：

- `micro-op/binary-vector/vadd`
  - 目标：`f32` 主语义最小路径
  - 覆盖：`core-f32, full-mask`
- `micro-op/binary-vector/vadd-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask`
- `micro-op/binary-vector/vadd-f16`
  - 目标：验证 `f16` 主语义
  - 覆盖：`core-f16, full-mask`
- `micro-op/binary-vector/vadd-bf16`
  - 目标：验证 `bf16` 主语义
  - 覆盖：`core-bf16, full-mask`
- `micro-op/binary-vector/vadd-i16-signed`
  - 目标：验证 signed integer 主语义
  - 覆盖：`core-i16-signed, full-mask`
- `micro-op/binary-vector/vadd-i16-unsigned`
  - 目标：验证 unsigned integer 主语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vadd-f32-exceptional`
  - 目标：验证 `f32` 异常值输入
  - 覆盖：`core-f32, full-mask, exceptional-values`
  - 输入至少包含：`+0`、`-0`、`+inf`、`-inf`、`nan` 和普通有限值

当前不把 `vadd-f8*` 列为默认必测项，原因是 `docs/isa/07-binary-vector-ops.md` 还未把 `fp8-like` 明确列入 `pto.vadd` 的 A5 types。

### Vec-Scalar

主语义：

- `vec op scalar -> out`

至少关注：

- `core-<type>`
- immediate/scalar 参数语义
- `full-mask`
- `tail-mask`
- signed / unsigned 分离

`pto.vadds` 作为 `vec-scalar` family 的首个展开样板，当前预期补齐以下 case：

- `micro-op/vec-scalar/vadds`
  - 目标：`f32` 主语义最小路径
  - 覆盖：`core-f32, full-mask`
- `micro-op/vec-scalar/vadds-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask`
- `micro-op/vec-scalar/vadds-f16`
  - 目标：验证 `f16` 主语义
  - 覆盖：`core-f16, full-mask`
- `micro-op/vec-scalar/vadds-bf16`
  - 目标：验证 `bf16` 主语义
  - 覆盖：`core-bf16, full-mask`
- `micro-op/vec-scalar/vadds-i16-signed`
  - 目标：验证 signed integer 主语义
  - 覆盖：`core-i16-signed, full-mask`
- `micro-op/vec-scalar/vadds-i16-unsigned`
  - 目标：验证 unsigned integer 主语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/vec-scalar/vadds-f32-exceptional`
  - 目标：验证 `f32` 异常值输入
  - 覆盖：`core-f32, full-mask, exceptional-values`

如文档后续对其他 `vec-scalar` op 给出更窄类型范围，应按 op 自身文档收缩，不直接复用 `vadds` 的全集。

### Unary Vector

主语义：

- `vec -> out`

至少关注：

- `core-<type>`
- `full-mask`
- `tail-mask`
- `exceptional-values`

`pto.vabs` 作为 `unary-vector` family 的首个展开样板，当前预期补齐以下 case：

- `micro-op/unary-vector/vabs`
  - 目标：`f32` 主语义最小路径
  - 覆盖：`core-f32, full-mask`
- `micro-op/unary-vector/vabs-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask`
- `micro-op/unary-vector/vabs-f16`
  - 目标：验证 `f16` 主语义
  - 覆盖：`core-f16, full-mask`
- `micro-op/unary-vector/vabs-i16-signed`
  - 目标：验证 signed integer 主语义
  - 覆盖：`core-i16-signed, full-mask`
- `micro-op/unary-vector/vabs-f32-exceptional`
  - 目标：验证 `f32` 的 `+0/-0/nan/inf`
  - 覆盖：`core-f32, full-mask, exceptional-values`

对其他 unary op 的附加要求：

- `vexp` / `vln` / `vsqrt` / `vrec` / `vrsqrt`
  - 必须有 `exceptional-values`
  - 需要根据文档约束设计合法输入与异常输入
- `vnot` / `vbcnt`
  - 重点放在 signed / unsigned 与位模式验证
- `vcls`
  - 必须单独覆盖 signed 语义

### Compare / Select

主语义：

- compare 产生 predicate
- select 使用 predicate 选值

至少关注：

- float / signed-int / unsigned-int 分离
- 代表性关系分支
- predicate 结果落盘验证方式
- 对 float compare family，纳入 `exceptional-values`

`compare-select` family 需要拆成 compare 和 select 两个样板链路：

- `micro-op/compare-select/vcmp-f32`
  - 目标：验证 `vcmp` 的 `f32` 主语义
  - 覆盖：`core-f32, full-mask`
- `micro-op/compare-select/vcmp-i16-signed`
  - 目标：验证 signed integer compare
  - 覆盖：`core-i16-signed, full-mask`
- `micro-op/compare-select/vcmp-i16-unsigned`
  - 目标：验证 unsigned integer compare
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/compare-select/vcmp-f32-exceptional`
  - 目标：验证 float compare 在 `nan/inf/+0/-0` 下的谓词结果
  - 覆盖：`core-f32, full-mask, exceptional-values`
- `micro-op/compare-select/vcmp-modes`
  - 目标：验证代表性关系分支
  - 覆盖：`cmp-eq`, `cmp-lt`, `cmp-ge`
- `micro-op/compare-select/vsel-f32`
  - 目标：验证 `vsel` 的主语义
  - 覆盖：`core-f32, full-mask`
- `micro-op/compare-select/vsel-tail`
  - 目标：验证 `vsel` 的 `tail-mask`
  - 覆盖：`core-f32, tail-mask`

若后续实现 `vcmps`，应补充：

- `micro-op/compare-select/vcmps-f32`
  - 目标：验证 vector-vs-scalar compare
  - 覆盖：`core-f32, full-mask, scalar-operand`

### Conversion

主语义：

- `src-type -> dst-type`

至少关注：

- src/dst 类型对
- 舍入 / 截断 / 饱和
- signed / unsigned 方向
- `fp8-like` 类型是否已在该 family 文档中进入正式范围

`conversion` family 以 `vcvt` 为样板，测试项按“类型对 + 属性”组织，而不是按单一源类型组织：

- `micro-op/conversion/vcvt-f32-to-f16`
  - 目标：验证 float-float 窄化
  - 覆盖：`src-f32-dst-f16, round-r, part-even/odd`
- `micro-op/conversion/vcvt-f32-to-bf16`
  - 目标：验证 float-float 转换
  - 覆盖：`src-f32-dst-bf16, round-r`
- `micro-op/conversion/vcvt-f16-to-i16`
  - 目标：验证 float-int 转换
  - 覆盖：`src-f16-dst-i16, round-r`
- `micro-op/conversion/vcvt-f32-to-i32-sat`
  - 目标：验证带饱和的 float-int 转换
  - 覆盖：`src-f32-dst-i32, round-r, sat-enable`
- `micro-op/conversion/vcvt-i32-to-f32`
  - 目标：验证 int-float 转换
  - 覆盖：`src-i32-dst-f32`
- `micro-op/conversion/vtrc-f32-rounding`
  - 目标：验证 `vtrc` 的 rounding mode
  - 覆盖：`round-r`, `round-z`, `round-f`

如果后续有文档明确把 `fp8-like` 纳入 `vcvt` 的正式范围，再补充：

- `micro-op/conversion/vcvt-*-to-f8e4m3`
- `micro-op/conversion/vcvt-f8e4m3-to-*`

### Vector Load/Store / Gather/Scatter / Rearrangement

主语义：

- 指针、mask、布局相关操作

至少关注：

- aligned / unaligned
- contiguous / non-contiguous
- representative `dist` / `mode`
- paired op 的 round-trip 语义

`vector-load-store` family 以“单 op 主语义 + 成组 round-trip”混合组织：

- `micro-op/vector-load-store/vlds`
  - 目标：验证 `vlds` 的 contiguous 主路径
  - 覆盖：`core-f32, aligned, dist-norm`
- `micro-op/vector-load-store/vlds-tail`
  - 目标：验证 `vlds` 的 `tail-mask` 消费场景
  - 覆盖：`core-f32, tail-mask, dist-norm`
- `micro-op/vector-load-store/vlds-brc-b32`
  - 目标：验证 broadcast distribution
  - 覆盖：`dist-brc-b32`
- `micro-op/vector-load-store/vsts`
  - 目标：验证 `vsts` 的 contiguous 主路径
  - 覆盖：`core-f32, aligned, dist-norm`
- `micro-op/vector-load-store/vldas-vldus`
  - 目标：验证 unaligned load stream
  - 覆盖：`unaligned, stream-state`
- `micro-op/vector-load-store/vldx2-vstx2`
  - 目标：验证 deinterleave / interleave 成组 round-trip
  - 覆盖：`paired-roundtrip, dintlv`

对 gather/scatter/rearrangement 继续沿用相同原则：

- contiguous 主路径先有一个最小 case
- layout / dist / mode 的代表性变体各有独立 case
- 强耦合成组语义用 round-trip case 表达

## Scenario Naming

matrix 中的 `scenarios` 字段不应只写泛化语义，例如：

- 不推荐：`elementwise add`
- 推荐：`core-f32, full-mask`

推荐使用以下组合方式：

- `core-f32`
- `core-f16`
- `core-f8e4m3`
- `core-i16-signed`
- `core-i16-unsigned`
- `tail-mask`
- `float-special-values`
- `aligned`
- `unaligned`
- `mode-x`
- `dist-x`

## Current Interpretation

按本文件口径，当前已落地的 `micro-op` case 只应视为：

- `vadd`: `core-f32`
- `vdiv`: `core-f32`
- `vmin`: `core-f32`
- `vadds`: `core-f32`

它们不是对应 op 的完整覆盖闭环，后续仍需按类型族和场景维度继续补齐。
