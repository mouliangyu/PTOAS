# VPTO Op Board Test Scope

## Purpose

本文件定义 `test/vpto` 中 VPTO 微指令板测的测试范围，同时也是本轮微指令板测的
case scope 文档。它负责三件事：

- 定义哪些 VPTO op / op family 在本轮范围内
- 列出这些 in-scope op 预期对应的 case 清单
- 说明每个 case 需要验证的测试目标和覆盖场景

具体来说，本文件用于约束：

- 哪些 op family 在本轮范围内
- 每类 op 至少需要覆盖哪些类型维度
- 每类 op 至少需要覆盖哪些运行时场景
- 每个 in-scope op 由哪些 case 承接
- 每个 case 的测试目标、覆盖维度和命名预期
- 哪些内容只作为基础设施复用，不单独立项
- matrix 中的 `scenarios` 字段应如何描述

本文件回答的是“测什么、每条 op 由哪些 case 来测、这些 case 的目标是什么”。

其中：

- `scope` 负责定义 in-scope op、case inventory 和每个 case 的目标
- `matrix` 负责记录这些 case 的当前状态、实现进度和验证结果
- `plan` 负责记录执行顺序、阶段安排和推进策略
- `coverage assessment` 负责记录 family 级统计、自评覆盖密度和 case 缺口

因此，若需要回答“某条 op 有没有被纳入范围、应该由什么 case 覆盖、case 目标是什么”，
应首先查看本文件；具体 case 目录状态流转和执行顺序仍以：

- `03-vpto-op-board-unit-tests-PLAN.md`
- `03-vpto-op-board-unit-tests-matrix.md`
- `03-vpto-op-board-coverage-assessment.md`

为准。

## Document Contract

本文件中的两个层次分别承担不同职责：

- `Family Coverage Minimums`
  - 回答某个 family 至少必须覆盖哪些维度
  - 是判断“该 family 是否还能继续收口”的最低标准
- `Case Inventory`
  - 回答某条 op 由哪些 case 承接，以及每个 case 的测试目标是什么
  - 是补充 `matrix` 和实现具体测例时的直接依据

使用约束：

- family 级覆盖合理性和 case 数缺口判断，先看 `03-vpto-op-board-coverage-assessment.md`
- 若 assessment 显示某个 family 仍存在任何 case 缺口或 case 目标缺失，则本文件必须继续细化该 family 的 `Case Inventory`，直到能直接指导补 case
- 新增或删除 case 前，先更新本文件的 `Case Inventory`
- 调整 family 级目标数量或覆盖合理性判断前，先更新 `03-vpto-op-board-coverage-assessment.md`
- `matrix` 只能记录本文件已经纳入范围的 case；不允许 `matrix` 先出现、`scope` 后补登记
- 若实现或文档变化导致某 case 目标失效，应先修改本文件中的目标描述，再更新 `matrix` 状态

与 assessment 的关系：

- assessment 负责指出“哪一类需要继续细化”
- 本文件负责把这种“需要细化”落成明确的 case inventory 和 case 目标
- 因此，本文件允许持续增长和细化；只要 assessment 仍显示 family 缺口或 case 目标缺失，就不应把对应 family 视为 scope 已稳定

## Exit Criteria

以下规则用于判断某个 family 是否达到“首轮可接受覆盖”：

- `Family Coverage Minimums` 中的 `mandatory` 维度已被 `Case Inventory` 中至少一个 case 明确承接
- 对允许成组验证的 family
  - 必须证明成组 case 确实覆盖了组内所有 op 的核心语义，而不是只借 round-trip 掩盖单边错误
- family 级当前进度、case 缺口和覆盖合理性判断，以 `03-vpto-op-board-coverage-assessment.md` 为准

以下情况不视为覆盖完成：

- family 级 `mandatory` 维度在 `Case Inventory` 中没有明确落到具体 case
- 只覆盖主路径，未覆盖该 family 明确要求的 tail、异常值、state-update、layout 或 packed round-trip 等关键维度
- 仅通过组合 case 间接“碰到”某条 op，但没有为该 op 的核心语义提供可解释的 oracle

## Source Of Truth

测试范围必须首先建立在文档化语义之上：

1. `docs/vpto-spec.md` 与 `docs/isa/*.md` 中文档化的 VPTO surface
2. case 是否可写、oracle 是否可定义，优先由上述文档判断

文档查询范围固定为：

- `docs/vpto-spec.md`
- `docs/isa/`

约束如下：

- 文档未声明、但 IR 暂时能打印出来的形式，不直接纳入正式覆盖范围
- 文档声明了且语义足够清楚时，应先把 case 写出来；即使当前 verifier 或 lowering 不接受，也不因此把条目标为 `blocked`
- 只有当文档本身不足以写出语义明确的 case 或无法定义稳定 oracle 时，才允许标记 `blocked`
- 执行阶段不得自行把“怀疑写不出来”的 case 记成 `blocked` 并跳过；只有开发者明确确认或提出后，才能把 blocker 正式登记到本文件与 `matrix`
- 已由开发者确认并登记为 `blocked` 的 case，在开发者主动取消前，不再进入错误分析与修复推进流程
- 类型、属性、参数列表、打印形式的“是否纳入 scope”以文档语义为准；实现链路失败属于后续 `implemented` 阶段的失败记录，而不是 scope 层 blocker

## Excluded Infrastructure

以下 op 作为其他 case 的准备动作或收尾动作复用，不单独立项建板测 case：

- pipeline sync
  - `set_flag`
  - `wait_flag`
  - `get_buf`
  - `rls_buf`
  - `pipe_barrier`
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

补充约束：

- 覆盖维度分为 `mandatory` 和 `applicable`
- `mandatory` 表示该 family 在本轮必须落地至少一个 case 触达该维度
- `applicable` 表示仅当文档语义和当前实现都支持时才需要展开；不允许为了“补维度”伪造不成立的 case
- matrix 中的 `scenarios` 字段只记录本 case 实际覆盖到的维度，不写“理论上可测”

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
- 向量 UB 访问和向量算子位于 `pto.vecscope` 内
- mask 计算位于对应 `pto.vecscope` 内
- 所有测例统一采用 `GM→UB→计算→UB→GM` 的可观测路径
- 对需要 operand feed 的向量 / mask / align 测例，输入只能通过访问 UB 的指令加载
- 对生成类 op，可在 `pto.vecscope` 内直接生成结果，但结果仍需先写回 UB，再导回 GM 做 host compare
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
- representative `dist`
- immediate vs scalar vs vector operand form
- exceptional-values
- floating overflow / underflow
- integer overflow / wraparound / saturation
- alias / no-alias

其中：

- `exceptional-values` 主要适用于 float family，例如 `0`、负数、NaN、Inf、近边界值
- `floating overflow / underflow` 仅对会产生放大、缩放、指数、倒数、转换等语义的 float family 纳入
- `integer overflow / wraparound / saturation` 仅对文档语义会区分环绕、饱和、截断或高低位保留的 integer/conversion family 纳入
- `alias / no-alias` 只在文档语义允许且实现可稳定验证时纳入
- `aligned / unaligned`、`contiguous / non-contiguous` 主要适用于 load/store、gather/scatter、rearrangement
- `dist` 只在该 op 或 family 语义中真实生效时纳入

对浮点 family，异常值测试属于正式范围，不是可选增强项。只要该 op 的文档语义允许相关输入进入计算，就应至少有一个 case 覆盖 `exceptional-values`。默认异常值集合包括：

- `+0`
- `-0`
- `+inf`
- `-inf`
- `nan`

如某个 family 还需要额外的浮点边界值，可在对应 family 条目或具体 case 中继续补充。

整数 family 如文档语义涉及溢出相关行为，至少需要覆盖以下其中一种明确可判定的行为：

- wraparound
- saturation
- truncation
- widening / high-low split

如果当前 docs/isa 尚未把某个 integer family 的溢出语义讲清楚，则该 family 相关 case 不预先承诺 overflow oracle，matrix 应标记为 `blocked` 或在 `notes` 中说明“当前只验证非溢出主路径”。

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

## Family Coverage Minimums

本节定义 family 级最小覆盖要求。每个 family 不要求一次性补齐所有 case，但新增 case 和 matrix 更新必须朝这些最小目标收敛。

### Vector Load/Store

mandatory:

- `core-f32`
- contiguous access
- representative `dist`
- full-mask
- tail-mask

applicable:

- aligned / unaligned
- non-contiguous access
- dual-lane / interleave layout

说明：

- `vlds` / `vsts` 作为最小连续访存主路径样板
- `vldas` / `vldus`、`vldx2` / `vstx2` 允许成组
- 若某变体只在 TD 中存在、文档未落定，不直接承诺进度

### Predicate Load/Store

mandatory:

- packed predicate round-trip
- load/store pair semantic preservation
- representative logical element count

applicable:

- immediate offset
- areg offset
- aligned / unaligned packed storage

说明：

- `psts+plds`、`pst+pld`、`psti+pldi` 允许成组
- oracle 必须按 packed predicate 的真实落盘前缀比较，不能按逻辑 mask 长度直接扩展比较

### Materialization Predicate

mandatory:

- one pattern-generation case
- one tail-mask generation case
- one predicate transform case

applicable:

- scalar carry out
- pack / unpack round-trip
- predicate select / invert

说明：

- `pset_*` family 允许共用 pattern 样板
- `pge_*` / `plt_*` family 至少各有一个代表 case 证明 tail-mask 路径

### Unary Vector

mandatory:

- `core-f32`
- full-mask
- tail-mask

applicable:

- `f16`
- `bf16`
- signed integer
- unsigned integer
- exceptional-values
- floating overflow / underflow

说明：

- `vabs`、`vexp` 作为 unary family 的首批样板
- 对 float unary，若语义允许 NaN/Inf 输入进入计算，则必须有 `exceptional-values`
- 对 `vexp`、`vrec`、`vsqrt` 等更敏感 family，应优先补充溢出、下溢、非法输入或定义域边界

### Binary Vector

mandatory:

- `core-f32`
- full-mask
- tail-mask

applicable:

- `f16`
- `bf16`
- signed integer
- unsigned integer
- exceptional-values
- integer overflow / wraparound / saturation

说明：

- `vadd` 作为首个详细展开 family 样板
- `vsub` / `vmul` / `vmax` 可沿用相同覆盖模板，但是否要求 exceptional-values 取决于文档语义

### Vec-Scalar

mandatory:

- `core-f32`
- full-mask
- tail-mask
- scalar operand semantic

applicable:

- immediate form
- scalar register/input form
- `f16`
- `bf16`
- signed integer
- unsigned integer
- exceptional-values
- integer overflow / wraparound / saturation

说明：

- `vadds` 作为首个详细展开 family 样板
- 后续 `vmuls`、`vmaxs`、`vmins` 优先验证“标量形态差异是否真正影响 lowering / 语义”
- `pto.vlrelu` 虽然在 `docs/isa/08-vec-scalar-ops.md` 也有 syntax 记录，但测试台账统一按 `docs/isa/13-dsa-sfu-ops.md` 归入 `dsa-sfu`

### Compare/Select

mandatory:

- representative relation set
- mask result or selection result validation
- full-mask
- tail-mask

applicable:

- signed / unsigned relation split
- float ordered / unordered distinction
- scalar compare form
- reversed select variant

说明：

- compare family 至少不能只验证一种 relation
- float compare 如文档区分 NaN 路径，必须显式加入 exceptional-values

### Conversion

mandatory:

- one widening or narrowing path
- one clearly defined rounding/truncation rule
- exact oracle for destination type

applicable:

- signed / unsigned source split
- saturation
- wraparound
- exceptional-values
- fp8-like conversion

说明：

- conversion family 必须把结果规则写死到 oracle；不能只做“看起来接近”的比较

### Reduction

mandatory:

- representative reduction op
- result placement validation
- full input domain over one vector tile

applicable:

- tail-mask
- max/min tie behavior
- float exceptional-values

说明：

- reduction case 需要同时验证“值正确”和“结果落位正确”

### Rearrangement

mandatory:

- one order-changing semantic
- explicit lane-order oracle

applicable:

- aligned / unaligned
- interleave / deinterleave pair round-trip
- byte-granularity variant

说明：

- rearrangement family 的 golden 不能只比集合相等，必须比顺序

### Gather/Scatter

mandatory:

- non-contiguous access
- explicit index pattern
- store/load effect validation

applicable:

- duplicate index
- out-of-order index
- aligned / unaligned base
- broadcast/carry variant

说明：

- gather/scatter family 默认需要 `no-alias`；如文档允许别名且实现可稳验，再单独补 alias case
- matrix 中与该 family 对应的 `scenarios` 应显式写出 `no-alias`，以及 `load-effect-validation` 或 `store-effect-validation`

### DSA/SFU

mandatory:

- one core semantic case
- float oracle

applicable:

- exceptional-values
- domain boundary
- overflow / underflow

说明：

- `vexpdiff`、`vlrelu`、`vprelu` 等 family 应优先补“输入边界 + 特殊值”，而不是只补普通随机数

## Case Inventory

本节只记录“当前计划用哪些 case 承接前述 minimum”。本节不再定义新的 family minimum；
若 case 清单与前述 minimum 不一致，应以前述 `Family Coverage Minimums` 为准，并同步修正 case 清单与 matrix。

### Binary Vector Cases

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
- `micro-op/binary-vector/vadd-i16-signed-overflow`
  - 目标：验证 signed integer 溢出语义
  - 覆盖：`core-i16-signed, full-mask, integer-overflow`
  - 状态：待补充
  - 备注：是否按 wraparound / saturation / 其他规则判定，需以 `docs/isa/07-binary-vector-ops.md` 与当前实现交集为准后固化 oracle
- `micro-op/binary-vector/vadd-i16-unsigned-overflow`
  - 目标：验证 unsigned integer 溢出语义
  - 覆盖：`core-i16-unsigned, full-mask, integer-overflow`
  - 状态：待补充
  - 备注：是否按 wraparound / saturation / 其他规则判定，需以 `docs/isa/07-binary-vector-ops.md` 与当前实现交集为准后固化 oracle
- `micro-op/binary-vector/vadd-f32-exceptional`
  - 目标：验证 `f32` 异常值输入
  - 覆盖：`core-f32, full-mask, exceptional-values`
  - 输入至少包含：`+0`、`-0`、`+inf`、`-inf`、`nan` 和普通有限值

当前不把 `vadd-f8*` 列为默认必测项，原因是 `docs/isa/07-binary-vector-ops.md` 还未把 `fp8-like` 明确列入 `pto.vadd` 的 A5 types。

### Vec-Scalar Cases

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
  - 覆盖：`core-f32, full-mask, scalar-operand`
- `micro-op/vec-scalar/vadds-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask, scalar-operand`
- `micro-op/vec-scalar/vadds-f16`
  - 目标：验证 `f16` 主语义
  - 覆盖：`core-f16, full-mask, scalar-operand`
  - 状态：blocked
  - 备注：`docs/isa/08-vec-scalar-ops.md` 仅给出通用 `T` 语法，尚未明确 `pto.vadds` 的 A5 type 集合
- `micro-op/vec-scalar/vadds-bf16`
  - 目标：验证 `bf16` 主语义
  - 覆盖：`core-bf16, full-mask, scalar-operand`
  - 状态：blocked
  - 备注：`docs/isa/08-vec-scalar-ops.md` 仅给出通用 `T` 语法，尚未明确 `pto.vadds` 的 A5 type 集合
- `micro-op/vec-scalar/vadds-i16-signed`
  - 目标：验证 signed integer 主语义
  - 覆盖：`core-i16-signed, full-mask, scalar-operand`
  - 状态：blocked
  - 备注：signed integer legality 仍需 `docs/vpto-spec.md` 与 `docs/isa/08-vec-scalar-ops.md` 的交集进一步固化
- `micro-op/vec-scalar/vadds-i16-unsigned`
  - 目标：验证 unsigned integer 主语义
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
  - 状态：blocked
  - 备注：unsigned integer legality 仍需 `docs/vpto-spec.md` 与 `docs/isa/08-vec-scalar-ops.md` 的交集进一步固化
- `micro-op/vec-scalar/vadds-f32-exceptional`
  - 目标：验证 `f32` 异常值输入
  - 覆盖：`core-f32, full-mask, scalar-operand, exceptional-values`

如文档后续对其他 `vec-scalar` op 给出更窄类型范围，应按 op 自身文档收缩，不直接复用 `vadds` 的全集。

其余已在 `docs/isa/08-vec-scalar-ops.md` 进入 surface 的 vec-scalar op，也至少需要一个最小 `core` case：

- `micro-op/vec-scalar/vsubs`
  - 目标：验证 scalar subtract 主语义
  - 覆盖：`core-f32, full-mask, scalar-operand`
- `micro-op/vec-scalar/vmuls`
  - 目标：验证 scalar multiply 主语义
  - 覆盖：`core-f32, full-mask, scalar-operand`
- `micro-op/vec-scalar/vmaxs`
  - 目标：验证 scalar max 主语义
  - 覆盖：`core-f32, full-mask, scalar-operand`
- `micro-op/vec-scalar/vmins`
  - 目标：验证 scalar min 主语义
  - 覆盖：`core-f32, full-mask, scalar-operand`
- `micro-op/vec-scalar/vands`
  - 目标：验证 integer scalar and 主语义
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
- `micro-op/vec-scalar/vors`
  - 目标：验证 integer scalar or 主语义
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
- `micro-op/vec-scalar/vxors`
  - 目标：验证 integer scalar xor 主语义
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
- `micro-op/vec-scalar/vshls`
  - 目标：验证 scalar left-shift 主语义
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
- `micro-op/vec-scalar/vshrs`
  - 目标：验证 scalar right-shift 主语义
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
- `micro-op/vec-scalar/vaddcs`
  - 目标：验证 add-with-carry 主语义
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand, carry-chain`
- `micro-op/vec-scalar/vsubcs`
  - 目标：验证 sub-with-borrow 主语义
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand, carry-chain`

补充的整数溢出项如下：

- `micro-op/vec-scalar/vadds-i16-signed-overflow`
  - 目标：验证 signed integer 溢出语义
  - 覆盖：`core-i16-signed, full-mask, scalar-operand, integer-overflow`
  - 状态：blocked
  - 备注：`docs/isa/08-vec-scalar-ops.md` 尚未给出明确 A5 types 与 overflow 规则，暂不固化 oracle
- `micro-op/vec-scalar/vadds-i16-unsigned-overflow`
  - 目标：验证 unsigned integer 溢出语义
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand, integer-overflow`
  - 状态：blocked
  - 备注：`docs/isa/08-vec-scalar-ops.md` 尚未给出明确 A5 types 与 overflow 规则，暂不固化 oracle

assessment 驱动的补充 case：

- `micro-op/vec-scalar/vsubs-tail`
  - 目标：验证 `vsubs` 在 `tail-mask` 下的边界行为
  - 覆盖：`core-f32, tail-mask, scalar-operand`
- `micro-op/vec-scalar/vmuls-tail`
  - 目标：验证 `vmuls` 在 `tail-mask` 下的边界行为
  - 覆盖：`core-f32, tail-mask, scalar-operand`
- `micro-op/vec-scalar/vmaxs-tail`
  - 目标：验证 `vmaxs` 在 `tail-mask` 下的选择语义
  - 覆盖：`core-f32, tail-mask, scalar-operand`
- `micro-op/vec-scalar/vmins-tail`
  - 目标：验证 `vmins` 在 `tail-mask` 下的选择语义
  - 覆盖：`core-f32, tail-mask, scalar-operand`
- `micro-op/vec-scalar/vands-mask-edge`
  - 目标：验证 `vands` 在交错 bit-pattern 下的位运算结果
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
- `micro-op/vec-scalar/vors-mask-edge`
  - 目标：验证 `vors` 在交错 bit-pattern 下的位运算结果
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
- `micro-op/vec-scalar/vxors-mask-edge`
  - 目标：验证 `vxors` 在交错 bit-pattern 下的位运算结果
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
- `micro-op/vec-scalar/vshls-shift-boundary`
  - 目标：验证 `vshls` 在临界 shift 量下的逐 lane 左移语义
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
- `micro-op/vec-scalar/vshrs-shift-boundary`
  - 目标：验证 `vshrs` 在临界 shift 量下的逐 lane 右移语义
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
- `micro-op/vec-scalar/vaddcs-carry-boundary`
  - 目标：验证 `vaddcs` 在 carry 传播边界上的结果与 carry-out
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand, carry-chain`
- `micro-op/vec-scalar/vsubcs-borrow-boundary`
  - 目标：验证 `vsubcs` 在 borrow 传播边界上的结果与 borrow-out
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand, carry-chain`

### Unary Vector Cases

主语义：

- `op(vec) -> out`

至少关注：

- `core-<type>`
- `full-mask`
- `tail-mask`
- float 特殊值
- family 特有定义域 / 边界

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
- `micro-op/unary-vector/vabs-bf16`
  - 目标：验证 `bf16` 主语义
  - 覆盖：`core-bf16, full-mask`
  - 状态：blocked
  - 备注：`docs/isa/06-unary-vector-ops.md` 当前未将 `bf16` 列入 `pto.vabs` 的 A5 types
- `micro-op/unary-vector/vabs-i16-signed`
  - 目标：验证 signed integer 主语义
  - 覆盖：`core-i16-signed, full-mask`
- `micro-op/unary-vector/vabs-i16-unsigned`
  - 目标：验证 unsigned integer 主语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/unary-vector/vabs-f32-exceptional`
  - 目标：验证 `f32` 特殊值输入
  - 覆盖：`core-f32, full-mask, exceptional-values`
  - 输入至少包含：`+0`、`-0`、`+inf`、`-inf`、`nan` 和普通有限值
- `micro-op/unary-vector/vabs-i16-signed-overflow-edge`
  - 目标：验证 signed integer 边界值行为
  - 覆盖：`core-i16-signed, full-mask, integer-overflow`
  - 状态：待补充
  - 备注：重点检查最小负值绝对值等边界；oracle 以文档与实现交集固化

`pto.vexp` 作为 `unary-vector` family 中对定义域边界更敏感的样板，当前预期补齐以下 case：

- `micro-op/unary-vector/vexp`
  - 目标：`f32` 主语义最小路径
  - 覆盖：`core-f32, full-mask`
- `micro-op/unary-vector/vexp-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask`
- `micro-op/unary-vector/vexp-f16`
  - 目标：验证 `f16` 主语义
  - 覆盖：`core-f16, full-mask`
- `micro-op/unary-vector/vexp-bf16`
  - 目标：验证 `bf16` 主语义
  - 覆盖：`core-bf16, full-mask`
  - 状态：blocked
  - 备注：`docs/isa/06-unary-vector-ops.md` 当前未将 `bf16` 列入 `pto.vexp` 的 A5 types
- `micro-op/unary-vector/vexp-f32-exceptional`
  - 目标：验证 `f32` 特殊值输入
  - 覆盖：`core-f32, full-mask, exceptional-values`
  - 输入至少包含：`+0`、`-0`、`+inf`、`-inf`、`nan`
- `micro-op/unary-vector/vexp-f32-over-underflow`
  - 目标：验证 `f32` 溢出 / 下溢边界
  - 覆盖：`core-f32, full-mask, floating-overflow-underflow`
  - 输入至少包含：大正值、大负值、接近 0 的有限值

其余已在 `docs/isa/06-unary-vector-ops.md` 进入 surface 的 unary op，也至少需要一个最小 `core` case：

- `micro-op/unary-vector/vneg`
  - 目标：验证 arithmetic negation 主语义
  - 覆盖：`core-f32, full-mask`
- `micro-op/unary-vector/vln`
  - 目标：验证 positive-domain natural log 主语义
  - 覆盖：`core-f32, full-mask, domain-positive`
- `micro-op/unary-vector/vsqrt`
  - 目标：验证 square-root 主语义
  - 覆盖：`core-f32, full-mask, domain-nonnegative`
- `micro-op/unary-vector/vrsqrt`
  - 目标：验证 reciprocal-square-root 主语义
  - 覆盖：`core-f32, full-mask, exceptional-values`
- `micro-op/unary-vector/vrec`
  - 目标：验证 reciprocal 主语义
  - 覆盖：`core-f32, full-mask, exceptional-values`
- `micro-op/unary-vector/vrelu`
  - 目标：验证 relu 主语义
  - 覆盖：`core-f32, full-mask`
- `micro-op/unary-vector/vnot`
  - 目标：验证 integer bitwise not 主语义
  - 覆盖：`core-i16-signed, full-mask`
- `micro-op/unary-vector/vbcnt`
  - 目标：验证 bit-count 主语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/unary-vector/vcls`
  - 目标：验证 count-leading-sign-bits 主语义
  - 覆盖：`core-i16-signed, full-mask`
- `micro-op/unary-vector/vmov`
  - 目标：验证 masked register copy 主语义
  - 覆盖：`core-f32, full-mask`

assessment 驱动的补充 case：

- `micro-op/unary-vector/vln-domain-boundary`
  - 目标：验证 `vln` 在接近 0 和大于 0 边界输入下的定义域行为
  - 覆盖：`core-f32, domain-positive, exceptional-values`
- `micro-op/unary-vector/vsqrt-domain-boundary`
  - 目标：验证 `vsqrt` 在 0、正数边界和非法负值附近的行为
  - 覆盖：`core-f32, domain-nonnegative, exceptional-values`
- `micro-op/unary-vector/vrsqrt-zero-inf`
  - 目标：验证 `vrsqrt` 对 0、Inf 和普通有限值的响应
  - 覆盖：`core-f32, exceptional-values`
- `micro-op/unary-vector/vrec-zero-inf`
  - 目标：验证 `vrec` 对 0、Inf 和普通有限值的响应
  - 覆盖：`core-f32, exceptional-values`
- `micro-op/unary-vector/vneg-f32-exceptional`
  - 目标：验证 `vneg` 对 `+0/-0/NaN/Inf` 的处理
  - 覆盖：`core-f32, exceptional-values`
- `micro-op/unary-vector/vmov-tail`
  - 目标：验证 `vmov` 在 `tail-mask` 下的选择性复制
  - 覆盖：`core-f32, tail-mask`

### Binary Vector Follow-up Cases

除 `vadd` 外，`binary-vector` family 还需要按相同粒度为已优先实现的样板补齐明确清单。

`pto.vdiv` 当前预期补齐以下 case：

- `micro-op/binary-vector/vdiv`
  - 目标：`f32` 主语义最小路径
  - 覆盖：`core-f32, full-mask`
- `micro-op/binary-vector/vdiv-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask`
- `micro-op/binary-vector/vdiv-f16`
  - 目标：验证 `f16` 主语义
  - 覆盖：`core-f16, full-mask`
- `micro-op/binary-vector/vdiv-bf16`
  - 目标：验证 `bf16` 主语义
  - 覆盖：`core-bf16, full-mask`
  - 状态：blocked
  - 备注：`docs/isa/07-binary-vector-ops.md` 当前未将 `bf16` 列入 `pto.vdiv` 的 A5 types
- `micro-op/binary-vector/vdiv-f32-exceptional`
  - 目标：验证除数 / 被除数特殊值
  - 覆盖：`core-f32, full-mask, exceptional-values`
  - 输入至少包含：`+0`、`-0`、`+inf`、`-inf`、`nan` 和普通有限值

`pto.vmin` 当前预期补齐以下 case：

- `micro-op/binary-vector/vmin`
  - 目标：`f32` 主语义最小路径
  - 覆盖：`core-f32, full-mask`
- `micro-op/binary-vector/vmin-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask`
- `micro-op/binary-vector/vmin-f16`
  - 目标：验证 `f16` 主语义
  - 覆盖：`core-f16, full-mask`
- `micro-op/binary-vector/vmin-bf16`
  - 目标：验证 `bf16` 主语义
  - 覆盖：`core-bf16, full-mask`
- `micro-op/binary-vector/vmin-i16-signed`
  - 目标：验证 signed integer 主语义
  - 覆盖：`core-i16-signed, full-mask`
- `micro-op/binary-vector/vmin-i16-unsigned`
  - 目标：验证 unsigned integer 主语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vmin-f32-exceptional`
  - 目标：验证 `f32` 特殊值与比较边界
  - 覆盖：`core-f32, full-mask, exceptional-values`

其余已在 `docs/isa/07-binary-vector-ops.md` 进入 surface 的 binary op，也至少需要一个最小 `core` case：

- `micro-op/binary-vector/vsub`
  - 目标：验证减法主语义
  - 覆盖：`core-f32, full-mask`
- `micro-op/binary-vector/vmul`
  - 目标：验证乘法主语义
  - 覆盖：`core-f32, full-mask`
- `micro-op/binary-vector/vmax`
  - 目标：验证最大值选择主语义
  - 覆盖：`core-f32, full-mask`
- `micro-op/binary-vector/vand`
  - 目标：验证按位与主语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vor`
  - 目标：验证按位或主语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vxor`
  - 目标：验证按位异或主语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vshl`
  - 目标：验证逐 lane 左移主语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vshr`
  - 目标：验证逐 lane 右移主语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vaddc`
  - 目标：验证 add-with-carry 的结果与 carry-out 主语义
  - 覆盖：`core-i16-unsigned, full-mask, carry-chain`
- `micro-op/binary-vector/vsubc`
  - 目标：验证 sub-with-borrow 的结果与 borrow-out 主语义
  - 覆盖：`core-i16-unsigned, full-mask, carry-chain`

assessment 驱动的补充 case：

- `micro-op/binary-vector/vsub-tail`
  - 目标：验证 `vsub` 在 `tail-mask` 下的边界行为
  - 覆盖：`core-f32, tail-mask`
- `micro-op/binary-vector/vmul-tail`
  - 目标：验证 `vmul` 在 `tail-mask` 下的边界行为
  - 覆盖：`core-f32, tail-mask`
- `micro-op/binary-vector/vmax-tail`
  - 目标：验证 `vmax` 在 `tail-mask` 下的选择语义
  - 覆盖：`core-f32, tail-mask`
- `micro-op/binary-vector/vand-mask-edge`
  - 目标：验证 `vand` 在交错 bit-pattern 下的位运算结果
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vor-mask-edge`
  - 目标：验证 `vor` 在交错 bit-pattern 下的位运算结果
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vxor-mask-edge`
  - 目标：验证 `vxor` 在交错 bit-pattern 下的位运算结果
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vshl-shift-boundary`
  - 目标：验证 `vshl` 在临界 shift 量下的逐 lane 左移语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vshr-shift-boundary`
  - 目标：验证 `vshr` 在临界 shift 量下的逐 lane 右移语义
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/binary-vector/vaddc-carry-boundary`
  - 目标：验证 `vaddc` 在 carry 传播边界上的结果与 carry-out
  - 覆盖：`core-i16-unsigned, full-mask, carry-chain`
- `micro-op/binary-vector/vsubc-borrow-boundary`
  - 目标：验证 `vsubc` 在 borrow 传播边界上的结果与 borrow-out
  - 覆盖：`core-i16-unsigned, full-mask, carry-chain`

### Compare/Select Cases

`pto.vcmp` 作为 `compare-select` family 的首个展开样板，当前预期补齐以下 case：

- `micro-op/compare-select/vcmp-eq`
  - 目标：验证等于关系
  - 覆盖：`core-f32, full-mask, relation-eq`
- `micro-op/compare-select/vcmp-lt`
  - 目标：验证小于关系
  - 覆盖：`core-f32, full-mask, relation-lt`
- `micro-op/compare-select/vcmp-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask`
- `micro-op/compare-select/vcmp-i16-signed`
  - 目标：验证 signed integer 比较
  - 覆盖：`core-i16-signed, full-mask`
- `micro-op/compare-select/vcmp-i16-unsigned`
  - 目标：验证 unsigned integer 比较
  - 覆盖：`core-i16-unsigned, full-mask`
- `micro-op/compare-select/vcmp-f32-exceptional`
  - 目标：验证 float 特殊值比较路径
  - 覆盖：`core-f32, full-mask, exceptional-values`

`pto.vsel` 作为 `compare-select` family 的选择样板，当前预期补齐以下 case：

- `micro-op/compare-select/vsel`
  - 目标：验证按 mask 选通
  - 覆盖：`core-f32, full-mask`
- `micro-op/compare-select/vsel-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask`
- `micro-op/compare-select/vsel-i16`
  - 目标：验证 integer 选择主语义
  - 覆盖：`core-i16-signed, full-mask`

`pto.vselr` 当前预期补齐以下 case：

- `micro-op/compare-select/vselr`
  - 状态：`blocked`
  - 原因：当前无法根据 `docs/isa/11-compare-select.md`、`docs/isa/12-data-rearrangement.md` 与 `VPTOOps.td` 唯一确定其语义；暂不继续收敛
  - 目标：待语义敲定后，再恢复“验证 reversed select 语义”
  - 覆盖：`core-f32, full-mask, reversed-select`

### Conversion Cases

`pto.vcvt` 作为 `conversion` family 的首个展开样板，当前预期补齐以下 case：

- `micro-op/conversion/vcvt-f32-to-f16`
  - 目标：验证窄化转换
  - 覆盖：`f32-to-f16, full-mask`
- `micro-op/conversion/vcvt-f16-to-f32`
  - 目标：验证扩宽转换
  - 覆盖：`f16-to-f32, full-mask`
- `micro-op/conversion/vcvt-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`f32-to-f16, tail-mask`
- `micro-op/conversion/vcvt-f32-special`
  - 目标：验证 float 特殊值转换
  - 覆盖：`f32-to-f16, exceptional-values`
- `micro-op/conversion/vcvt-i32-to-i16-overflow`
  - 目标：验证整数转换溢出语义
  - 覆盖：`i32-to-i16, integer-overflow`
  - 状态：blocked
  - 备注：`docs/isa/09-conversion-ops.md` 当前未明确列出 `i32 -> i16` 这一 A5 conversion pair

`pto.vtrc` 作为 `conversion` family 的 rounding 样板，当前预期补齐以下 case：

- `micro-op/conversion/vtrc-f32-rounding`
  - 目标：验证 `ROUND_R` / `ROUND_Z` / `ROUND_F` 的舍入语义
  - 覆盖：`core-f32, round-r, round-z, round-f`
- `micro-op/conversion/vtrc-f32-special`
  - 目标：验证特殊值输入在 rounding family 中的行为
  - 覆盖：`core-f32, exceptional-values`

assessment 驱动的补充 case：

- `micro-op/conversion/vcvt-f16-special`
  - 目标：验证 `f16 -> f32` 扩宽路径上的特殊值保持
  - 覆盖：`f16-to-f32, exceptional-values`
- `micro-op/conversion/vtrc-rounding-boundary`
  - 目标：验证临界舍入点附近的 `ROUND_R / ROUND_Z / ROUND_F` 行为
  - 覆盖：`core-f32, round-r, round-z, round-f`
- `micro-op/conversion/vcvt-tail-special`
  - 目标：验证 `vcvt` 在 `tail-mask` 与特殊值同时出现时的结果
  - 覆盖：`f32-to-f16, tail-mask, exceptional-values`

### Compare-Select Follow-up Cases

`pto.vcmps` 已在 `docs/vpto-spec.md` 与 `docs/isa/11-compare-select.md` 中进入正式 surface，当前预期补齐以下 case：

- `micro-op/compare-select/vcmps-f32`
  - 目标：验证 vector-vs-scalar compare 主语义
  - 覆盖：`core-f32, full-mask, scalar-operand`
- `micro-op/compare-select/vcmps-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask, scalar-operand`
- `micro-op/compare-select/vcmps-i16-signed`
  - 目标：验证 signed integer scalar compare
  - 覆盖：`core-i16-signed, full-mask, scalar-operand`
- `micro-op/compare-select/vcmps-i16-unsigned`
  - 目标：验证 unsigned integer scalar compare
  - 覆盖：`core-i16-unsigned, full-mask, scalar-operand`
- `micro-op/compare-select/vcmps-f32-exceptional`
  - 目标：验证 float 特殊值 compare 路径
  - 覆盖：`core-f32, full-mask, scalar-operand, exceptional-values`

assessment 驱动的补充 case：

- `micro-op/compare-select/vcmp-unordered-f32`
  - 目标：验证 `vcmp` 在 `NaN` 参与时的 unordered 比较路径
  - 覆盖：`core-f32, full-mask, exceptional-values`
- `micro-op/compare-select/vcmps-unordered-f32`
  - 目标：验证 `vcmps` 在 `NaN` 参与时的 unordered 比较路径
  - 覆盖：`core-f32, full-mask, scalar-operand, exceptional-values`
- `micro-op/compare-select/vsel-predicate-edge`
  - 目标：验证 `vsel` 在交错 predicate 下的元素选通行为
  - 覆盖：`core-f32, full-mask`

### Materialization Predicate Cases

`materialization-predicate` family 当前预期补齐以下 case：

- `pto.pset_b8` / `pto.pset_b16` / `pto.pset_b32`
  - 统一由 `pset-pattern` 样板覆盖
- `pto.pge_b8` / `pto.pge_b16` / `pto.pge_b32`
  - 统一由 `pge-tail-mask` 样板覆盖
- `pto.plt_b8` / `pto.plt_b16` / `pto.plt_b32`
  - 统一由 `plt-tail-mask` 样板覆盖
- `pto.pdintlv_b8`
  - 由 `pdintlv_b8` case 覆盖
- `pto.pintlv_b16`
  - 由 `pintlv_b16` case 覆盖

- `micro-op/materialization-predicate/vbr-f32`
  - 目标：验证 scalar broadcast 到 vector 的主语义
  - 覆盖：`core-f32, scalar-broadcast`
- `micro-op/materialization-predicate/vbr-i32`
  - 目标：验证 integer broadcast 主语义
  - 覆盖：`core-i32-signed, scalar-broadcast`
- `micro-op/materialization-predicate/vdup-scalar`
  - 目标：验证 scalar 输入的 duplicate 语义
  - 覆盖：`core-f32, scalar-operand`
- `micro-op/materialization-predicate/vdup-lane`
  - 目标：验证 vector lane duplicate 语义
  - 覆盖：`core-f32, lane-select`
- `micro-op/materialization-predicate/pset-pattern`
  - 目标：验证 pattern mask 生成
  - 覆盖：`pattern-mask, pat-all, pat-vl`
- `micro-op/materialization-predicate/pge-tail-mask`
  - 目标：验证 tail-mask 生成
  - 覆盖：`tail-mask`
- `micro-op/materialization-predicate/plt-tail-mask`
  - 目标：验证带 scalar_out 的 tail-mask 生成
  - 覆盖：`tail-mask, scalar-carry-out`
- `micro-op/materialization-predicate/ppack-punpack`
  - 目标：验证 predicate pack/unpack round-trip
  - 覆盖：`pack-unpack-roundtrip`
  - 承接：`pto.ppack` 负责逻辑 predicate 到 packed 形式的压缩；`pto.punpack` 负责 packed 结果恢复为逻辑 predicate
- `micro-op/materialization-predicate/pdintlv_b8`
  - 目标：验证 predicate deinterleave
  - 覆盖：`predicate-transform, lane-order`
- `micro-op/materialization-predicate/pintlv_b16`
  - 目标：验证 predicate interleave
  - 覆盖：`predicate-transform, lane-order`
- `micro-op/materialization-predicate/pand`
  - 目标：验证 predicate AND
  - 覆盖：`predicate-transform`
- `micro-op/materialization-predicate/por`
  - 目标：验证 predicate OR
  - 覆盖：`predicate-transform`
- `micro-op/materialization-predicate/pxor`
  - 目标：验证 predicate XOR
  - 覆盖：`predicate-transform`
- `micro-op/materialization-predicate/pnot`
  - 目标：验证 predicate NOT
  - 覆盖：`predicate-transform`
- `micro-op/materialization-predicate/psel`
  - 目标：验证 predicate select
  - 覆盖：`predicate-transform, predicate-select`

assessment 驱动的补充 case：

- `micro-op/materialization-predicate/pset-pattern-fragment`
  - 目标：验证非全量 pattern 在部分激活长度上的 mask 生成
  - 覆盖：`pattern-mask, pat-vl, representative-logical-elements`
- `micro-op/materialization-predicate/pge-tail-mask-boundary`
  - 目标：验证 tail-mask 在临界长度附近的边界行为
  - 覆盖：`tail-mask, representative-logical-elements`
- `micro-op/materialization-predicate/plt-tail-mask-boundary`
  - 目标：验证带 `scalar_out` 的 tail-mask 边界行为
  - 覆盖：`tail-mask, scalar-carry-out, representative-logical-elements`
- `micro-op/materialization-predicate/ppack-punpack-nontrivial`
  - 目标：验证非平凡 predicate 模式的 pack / unpack round-trip
  - 覆盖：`pack-unpack-roundtrip, representative-logical-elements`
  - 承接：`pto.ppack` 验证非平凡 predicate 模式的打包结果；`pto.punpack` 验证对应恢复后的逻辑位置
- `micro-op/materialization-predicate/pdintlv_b8-nontrivial`
  - 目标：验证 `pdintlv_b8` 在非平凡 lane 模式下的顺序语义
  - 覆盖：`predicate-transform, lane-order`
- `micro-op/materialization-predicate/pintlv_b16-nontrivial`
  - 目标：验证 `pintlv_b16` 在非平凡 lane 模式下的顺序语义
  - 覆盖：`predicate-transform, lane-order`
- `micro-op/materialization-predicate/psel-tail-predicate`
  - 目标：验证 predicate select 在部分激活 mask 下的选择语义
  - 覆盖：`predicate-transform, predicate-select, tail-mask`

### Predicate Load/Store Cases

`predicate-load-store` family 当前预期补齐以下 case：

- `micro-op/predicate-load-store/psts-plds`
  - 目标：验证 scalar-offset predicate round-trip
  - 覆盖：`packed-predicate-roundtrip, scalar-offset, load-store-pair-preservation, representative-logical-elements`
  - 承接：`pto.psts` 负责 scalar-offset packed predicate store；`pto.plds` 负责对应 load 后的 packed predicate 恢复
- `micro-op/predicate-load-store/pst-pld`
  - 目标：验证 areg-offset predicate round-trip
  - 覆盖：`packed-predicate-roundtrip, areg-offset, load-store-pair-preservation, representative-logical-elements`
  - 承接：`pto.pst` 负责 areg-offset packed predicate store；`pto.pld` 负责对应 load 后的 packed predicate 恢复
- `micro-op/predicate-load-store/psti-pldi`
  - 目标：验证 immediate-offset predicate round-trip
  - 覆盖：`packed-predicate-roundtrip, immediate-offset, load-store-pair-preservation, representative-logical-elements`
  - 承接：`pto.psti` 负责 immediate-offset packed predicate store；`pto.pldi` 负责对应 load 后的 packed predicate 恢复
- `micro-op/predicate-load-store/pstu`
  - 目标：验证 unaligned predicate store 的状态推进
  - 覆盖：`unaligned-packed-store, state-update, representative-logical-elements`

assessment 驱动的补充 case：

- `micro-op/predicate-load-store/psts-plds-packed-prefix-boundary`
  - 目标：验证 packed predicate 在非整字节前缀长度下的落盘前缀保持
  - 覆盖：`packed-predicate-roundtrip, scalar-offset, load-store-pair-preservation, representative-logical-elements`
  - 承接：`pto.psts` 验证非整字节前缀长度下的 packed store；`pto.plds` 验证对应 load 后的前缀保持
- `micro-op/predicate-load-store/pstu-state-advance-boundary`
  - 目标：验证 `pstu` 在非整包长度下的地址推进和落盘边界
  - 覆盖：`unaligned-packed-store, state-update, representative-logical-elements`

### Reduction Cases

`reduction` family 当前预期补齐以下 case：

- `micro-op/reduction/vcadd`
  - 目标：验证 full reduction sum 主语义
  - 覆盖：`core-f32, result-placement`
- `micro-op/reduction/vcadd-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask, result-placement`
- `micro-op/reduction/vcmax`
  - 目标：验证 max reduction 与结果落位
  - 覆盖：`core-f32, result-placement`
- `micro-op/reduction/vcmin`
  - 目标：验证 min reduction 与结果落位
  - 覆盖：`core-f32, result-placement`
- `micro-op/reduction/vcgadd`
  - 目标：验证 per-vlane group reduction
  - 覆盖：`group-reduction, result-placement`
- `micro-op/reduction/vcgmax`
  - 目标：验证 per-vlane group max
  - 覆盖：`group-reduction, result-placement`
- `micro-op/reduction/vcgmin`
  - 目标：验证 per-vlane group min
  - 覆盖：`group-reduction, result-placement`
- `micro-op/reduction/vcpadd`
  - 目标：验证 prefix sum 语义
  - 覆盖：`prefix-op, full-mask`

assessment 驱动的补充 case：

- `micro-op/reduction/vcgadd-tail`
  - 目标：验证 group reduction 在部分激活输入上的结果落位
  - 覆盖：`group-reduction, tail-mask, result-placement`
- `micro-op/reduction/vcgmax-tie`
  - 目标：验证 group max 在并列极值下的稳定落位
  - 覆盖：`group-reduction, result-placement`
- `micro-op/reduction/vcgmin-tie`
  - 目标：验证 group min 在并列极值下的稳定落位
  - 覆盖：`group-reduction, result-placement`
- `micro-op/reduction/vcpadd-tail`
  - 目标：验证 prefix sum 在 `tail-mask` 下的前缀边界
  - 覆盖：`prefix-op, tail-mask`

### Vector Load/Store Cases

主语义：

- 指针、mask、布局相关操作

至少关注：

- aligned / unaligned
- contiguous / non-contiguous
- representative `dist`
- paired op 的 round-trip 语义

`vector-load-store` family 以“单 op 主语义 + 成组 round-trip”混合组织：

- `micro-op/vector-load-store/vlds`
  - 目标：验证 `vlds` 的 contiguous 主路径
  - 覆盖：`core-f32, contiguous, full-mask, aligned, dist-norm`
- `micro-op/vector-load-store/vlds-tail`
  - 目标：验证 `vlds` 的 `tail-mask` 消费场景
  - 覆盖：`core-f32, contiguous, tail-mask, aligned, dist-norm`
- `micro-op/vector-load-store/vlds-brc-b32`
  - 目标：验证 broadcast distribution
  - 覆盖：`core-f32, full-mask, aligned, dist-brc-b32`
- `micro-op/vector-load-store/vsts`
  - 目标：验证 `vsts` 的 contiguous 主路径
  - 覆盖：`core-f32, contiguous, full-mask, aligned, dist-norm`
- `micro-op/vector-load-store/vldas-vldus`
  - 目标：验证 unaligned load stream
  - 覆盖：`core-f32, full-mask, unaligned, stream-state`
  - 承接：`pto.vldas` 验证 aligned stream state 基础路径；`pto.vldus` 验证 unaligned stream state 基础路径
- `micro-op/vector-load-store/vldx2-vstx2`
  - 目标：验证 deinterleave / interleave 成组 round-trip
  - 覆盖：`core-f32, full-mask, paired-roundtrip, dintlv`
  - 承接：`pto.vldx2` 验证双路读取后的 deinterleave 布局；`pto.vstx2` 验证对应 interleave 写回布局
- `micro-op/vector-load-store/vsld`
  - 目标：验证固定 stride load 主语义
  - 覆盖：`core-f32, full-mask, strided-load`
- `micro-op/vector-load-store/vsldb`
  - 目标：验证 block-strided load 与 block mask
  - 覆盖：`core-f32, full-mask, block-strided-load, block-mask`
- `micro-op/vector-load-store/vsst`
  - 目标：验证 strided store 主语义
  - 覆盖：`core-f32, full-mask, strided-store`
- `micro-op/vector-load-store/vsstb`
  - 目标：验证 block-strided store 与块掩码主语义
  - 覆盖：`core-f32, full-mask, block-strided-store, block-mask`
- `micro-op/vector-load-store/vsta`
  - 目标：验证 aligned state store 的状态推进
  - 覆盖：`core-f32, full-mask, aligned, state-update`
- `micro-op/vector-load-store/vstas`
  - 目标：验证 immediate-offset aligned state store 的状态推进
  - 覆盖：`core-f32, full-mask, aligned, immediate-offset, state-update`
- `micro-op/vector-load-store/vstar`
  - 目标：验证 reference-updated aligned state store 主语义
  - 覆盖：`core-f32, full-mask, aligned, state-update`
- `micro-op/vector-load-store/vstu`
  - 目标：验证 unaligned state store 的状态推进
  - 覆盖：`core-f32, full-mask, unaligned, state-update`
- `micro-op/vector-load-store/vstus`
  - 目标：验证 immediate-offset unaligned state store 的状态推进
  - 覆盖：`core-f32, full-mask, unaligned, immediate-offset, state-update`
- `micro-op/vector-load-store/vstur`
  - 目标：验证 reference-updated unaligned state store 主语义
  - 覆盖：`core-f32, full-mask, unaligned, state-update`

assessment 驱动的补充 case：

- `micro-op/vector-load-store/vsts-tail`
  - 目标：验证 `vsts` 在 `tail-mask` 下的写回边界
  - 覆盖：`core-f32, contiguous, tail-mask, aligned, dist-norm`
- `micro-op/vector-load-store/vldas-vldus-state-chain`
  - 目标：验证 stream load 连续调用时的状态推进
  - 覆盖：`core-f32, full-mask, unaligned, stream-state, state-update`
  - 承接：`pto.vldas` 验证连续调用下的 aligned stream state 推进；`pto.vldus` 验证连续调用下的 unaligned stream state 推进
- `micro-op/vector-load-store/vldx2-layout-check`
  - 目标：验证 `vldx2` 单边 deinterleave 后的 lane 布局
  - 覆盖：`core-f32, full-mask, paired-roundtrip, dintlv, lane-order`
- `micro-op/vector-load-store/vstx2-layout-check`
  - 目标：验证 `vstx2` 单边 interleave 写回后的 lane 布局
  - 覆盖：`core-f32, full-mask, paired-roundtrip, dintlv, lane-order`
- `micro-op/vector-load-store/vsta-state-advance`
  - 目标：验证 `vsta` 写回后的 aligned 地址推进
  - 覆盖：`core-f32, full-mask, aligned, state-update`
- `micro-op/vector-load-store/vstu-state-advance`
  - 目标：验证 `vstu` 写回后的 unaligned 地址推进
  - 覆盖：`core-f32, full-mask, unaligned, state-update`
- `micro-op/vector-load-store/vstas-vstus-offset-update`
  - 目标：验证 immediate offset 与 state-update 的组合效果
  - 覆盖：`core-f32, full-mask, immediate-offset, state-update`
  - 承接：`pto.vstas` 验证 aligned immediate-offset + state-update；`pto.vstus` 验证 unaligned immediate-offset + state-update
- `micro-op/vector-load-store/vsld-vsst-stride-boundary`
  - 目标：验证 stride load/store 在边界步长下的访存效果
  - 覆盖：`core-f32, strided-load, strided-store, block-mask`
  - 承接：`pto.vsld` 验证 stride 读取边界；`pto.vsst` 验证对应 stride 写回边界

### Gather/Scatter Cases

- `micro-op/gather-scatter/vscatter`
  - 目标：验证 indexed scatter 主语义
  - 覆盖：`core-f32, full-mask, non-contiguous, explicit-index-pattern, scatter-store, store-effect-validation, no-alias`
- `micro-op/gather-scatter/vgather2`
  - 目标：验证 indexed gather 主语义
  - 覆盖：`core-f32, full-mask, non-contiguous, explicit-index-pattern, load-effect-validation, no-alias`
- `micro-op/gather-scatter/vgatherb`
  - 目标：验证 block gather
  - 覆盖：`core-f32, full-mask, block-gather, aligned-base, load-effect-validation, no-alias`
- `micro-op/gather-scatter/vgather2_bc`
  - 目标：验证带 mask 的 gather 语义
  - 覆盖：`core-f32, full-mask, non-contiguous, masked-gather, load-effect-validation, no-alias`

assessment 驱动的补充 case：

- `micro-op/gather-scatter/vgather2-duplicate-index`
  - 目标：验证重复 index 下的 gather 读取效果
  - 覆盖：`core-f32, non-contiguous, explicit-index-pattern, load-effect-validation, no-alias`
- `micro-op/gather-scatter/vgather2_bc-sparse-mask`
  - 目标：验证稀疏 mask 下的 masked gather 行为
  - 覆盖：`core-f32, masked-gather, load-effect-validation, no-alias`
- `micro-op/gather-scatter/vgatherb-block-boundary`
  - 目标：验证 block gather 在块边界附近的读取行为
  - 覆盖：`core-f32, block-gather, aligned-base, load-effect-validation, no-alias`
- `micro-op/gather-scatter/vscatter-out-of-order-index`
  - 目标：验证乱序 index 下的 scatter 写回效果
  - 覆盖：`core-f32, explicit-index-pattern, scatter-store, store-effect-validation, no-alias`

### Rearrangement Cases

- `micro-op/rearrangement/vintlv-vdintlv`
  - 目标：验证 interleave / deinterleave round-trip
  - 覆盖：`paired-roundtrip, lane-order`
  - 承接：`pto.vintlv` 验证 interleave 顺序；`pto.vdintlv` 验证对应 deinterleave 恢复顺序
- `micro-op/rearrangement/vslide`
  - 目标：验证双源 slide 窗口语义
  - 覆盖：`lane-order, slide-window`
- `micro-op/rearrangement/vshift`
  - 目标：验证单源 shift 与 zero-fill
  - 覆盖：`lane-order, zero-fill`
- `micro-op/rearrangement/vsqz`
  - 目标：验证按 mask 压缩到前部
  - 覆盖：`predicate-driven-rearrangement, stable-order`
- `micro-op/rearrangement/vusqz`
  - 目标：验证按 mask 展开到激活位置
  - 覆盖：`predicate-driven-rearrangement, placement`
- `micro-op/rearrangement/vperm`
  - 目标：验证寄存器内 permutation
  - 覆盖：`lane-order, explicit-index-pattern`
- `micro-op/rearrangement/vpack`
  - 目标：验证 narrowing pack
  - 覆盖：`pack-unpack, narrowing`
- `micro-op/rearrangement/vsunpack`
  - 目标：验证 sign-extending unpack
  - 覆盖：`pack-unpack, sign-extend`
- `micro-op/rearrangement/vzunpack`
  - 目标：验证 zero-extending unpack
  - 覆盖：`pack-unpack, zero-extend`

assessment 驱动的补充 case：

- `micro-op/rearrangement/vintlv-vdintlv-lane-boundary`
  - 目标：验证 interleave / deinterleave 在临界 lane 布局下的顺序保持
  - 覆盖：`paired-roundtrip, lane-order`
  - 承接：`pto.vintlv` 验证临界 lane 布局下的 interleave 顺序；`pto.vdintlv` 验证对应 deinterleave 恢复顺序
- `micro-op/rearrangement/vslide-tail-window`
  - 目标：验证 slide 在 `tail-mask` 下的窗口拼接
  - 覆盖：`lane-order, slide-window, tail-mask`
- `micro-op/rearrangement/vshift-tail-zero-fill`
  - 目标：验证 shift 在 `tail-mask` 下的 zero-fill 行为
  - 覆盖：`lane-order, zero-fill, tail-mask`
- `micro-op/rearrangement/vsqz-nontrivial-mask`
  - 目标：验证非平凡 predicate 模式下的压缩顺序
  - 覆盖：`predicate-driven-rearrangement, stable-order`
- `micro-op/rearrangement/vusqz-nontrivial-mask`
  - 目标：验证非平凡 predicate 模式下的展开位置
  - 覆盖：`predicate-driven-rearrangement, placement`

### DSA/SFU Cases

`dsa-sfu` family 当前预期补齐以下 case：

- `micro-op/dsa-sfu/vlrelu-f32`
  - 目标：验证 scalar alpha 的 leaky-relu 主语义
  - 覆盖：`core-f32, scalar-operand, full-mask`
- `micro-op/dsa-sfu/vlrelu-tail`
  - 目标：验证 `tail-mask`
  - 覆盖：`core-f32, tail-mask, scalar-operand`
- `micro-op/dsa-sfu/vlrelu-f16`
  - 目标：验证 `f16` 主语义
  - 覆盖：`core-f16, full-mask, scalar-operand`
- `micro-op/dsa-sfu/vprelu-f32`
  - 目标：验证 per-element alpha vector 语义
  - 覆盖：`core-f32, vector-alpha`
- `micro-op/dsa-sfu/vexpdiff-f32`
  - 目标：验证 fused `exp(x - max)` 主语义
  - 覆盖：`core-f32, fused-expdiff`
- `micro-op/dsa-sfu/vaddrelu-f32`
  - 目标：验证 fused add + relu
  - 覆盖：`core-f32, fused-op`
- `micro-op/dsa-sfu/vsubrelu-f32`
  - 目标：验证 fused sub + relu
  - 覆盖：`core-f32, fused-op`
- `micro-op/dsa-sfu/vaxpy-f32`
  - 目标：验证 `alpha * x + y` 主语义
  - 覆盖：`core-f32, scalar-operand, fused-op`
- `micro-op/dsa-sfu/vaddreluconv`
  - 目标：验证 fused add/relu/convert 语义
  - 覆盖：`fused-op, conversion-result`
- `micro-op/dsa-sfu/vmulconv`
  - 目标：验证 fused mul/convert 语义
  - 覆盖：`fused-op, conversion-result`
- `micro-op/dsa-sfu/vmull`
  - 目标：验证 widening multiply 的 low/high 结果
  - 覆盖：`widening-op, hi-lo-split`
- `micro-op/dsa-sfu/vmula`
  - 目标：验证 multiply-accumulate 语义
  - 覆盖：`core-f32, fused-op, accumulator`
- `micro-op/dsa-sfu/vci`
  - 目标：验证 index generation 主语义
  - 覆盖：`index-generation`
- `micro-op/dsa-sfu/vbitsort`
  - 目标：验证 bit-sort surface 与基础数据重排语义
  - 覆盖：`index-generation, layout-transform`
  - 状态：blocked
  - 备注：`docs/vpto-spec.md` 与 `docs/isa/13-dsa-sfu-ops.md` 目前只给出 surface/接口层信息，尚未形成可稳定闭环的 oracle 语义
- `micro-op/dsa-sfu/vtranspose`
  - 目标：验证 UB-to-UB transpose
  - 覆盖：`ub-to-ub, layout-transform, representative-config`

assessment 驱动的补充 case：

- `micro-op/dsa-sfu/vlrelu-f32-exceptional`
  - 目标：验证 `vlrelu` 在特殊值输入下的行为
  - 覆盖：`core-f32, scalar-operand, exceptional-values`
- `micro-op/dsa-sfu/vprelu-tail`
  - 目标：验证 `vprelu` 在 `tail-mask` 下的逐元素 alpha 语义
  - 覆盖：`core-f32, vector-alpha, tail-mask`
- `micro-op/dsa-sfu/vexpdiff-boundary`
  - 目标：验证 `vexpdiff` 在极值和接近零差值下的行为
  - 覆盖：`core-f32, fused-expdiff, exceptional-values, floating-overflow-underflow`
- `micro-op/dsa-sfu/vmula-accumulator-boundary`
  - 目标：验证 `vmula` 在累加器边界值附近的累加语义
  - 覆盖：`core-f32, fused-op, accumulator`
- `micro-op/dsa-sfu/vtranspose-multi-config`
  - 目标：验证 `vtranspose` 在多个代表配置下的布局变换一致性
  - 覆盖：`ub-to-ub, layout-transform, representative-config`

说明：

- `pto.vlrelu` 在文档层同时出现在 vec-scalar 与 DSA/SFU 两处；测试台账统一按 DSA/SFU 归类，`docs/isa/08-vec-scalar-ops.md` 仅作为语义交叉参考
- `vtranspose` 不是 `vreg -> vreg` op，但已在 `docs/isa/13-dsa-sfu-ops.md` 进入正式 surface，因此仍纳入本轮范围
- 已转入 `[03-vpto-doc-drift-review.md](/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.planning/phases/03-hivm-emission/03-vpto-doc-drift-review.md)` 的文档漂移项，暂不继续留在本范围文档记账
- `vaddreluconv` / `vmulconv` 的具体类型对需要以文档与当前实现交集为准；若某一对不稳定，应在 matrix 中标记 `blocked`
- `vmull` 的 oracle 必须分别验证 low/high 两个输出

## Scenario Naming

matrix 中的 `scenarios` 字段不应只写泛化语义，例如：

- 不推荐：`elementwise add`
- 推荐：`core-f32, full-mask`

case 命名约定：

- 对单 op case，允许使用 `op` 或 `op-<scenario>` 形式，例如 `vadd`、`vcmp-eq`、`vbr-f32`
- 只要条目目标明确指向某个 op，本文件就视为该 op 已有明确测试项
- 只有被显式标注为“成组 round-trip / 成组验证”的条目，才按多 op 共享一个 case 处理

推荐使用以下组合方式：

- 以下列表是当前已在本文件 case 中实际使用或明确约定的标签集合；后续如新增标签，需先回写本节再进入 matrix

- `core-f32`
- `core-f16`
- `core-bf16`
- `core-f8e4m3`
- `core-i32-signed`
- `core-i16-signed`
- `core-i16-unsigned`
- `full-mask`
- `tail-mask`
- `exceptional-values`
- `floating-overflow-underflow`
- `integer-overflow`
- `aligned`
- `unaligned`
- `contiguous`
- `non-contiguous`
- `scalar-offset`
- `areg-offset`
- `immediate-offset`
- `scalar-operand`
- `scalar-broadcast`
- `lane-select`
- `vector-alpha`
- `pattern-mask`
- `pat-all`
- `pat-vl`
- `packed-predicate-roundtrip`
- `load-store-pair-preservation`
- `representative-logical-elements`
- `pack-unpack-roundtrip`
- `unaligned-packed-store`
- `state-update`
- `predicate-transform`
- `predicate-select`
- `scalar-carry-out`
- `reversed-select`
- `relation-eq`
- `relation-lt`
- `carry-chain`
- `domain-positive`
- `domain-nonnegative`
- `round-r`
- `round-z`
- `round-f`
- `f32-to-f16`
- `f16-to-f32`
- `i32-to-i16`
- `result-placement`
- `group-reduction`
- `prefix-op`
- `dist-norm`
- `dist-brc-b32`
- `stream-state`
- `strided-load`
- `strided-store`
- `block-strided-store`
- `paired-roundtrip`
- `dintlv`
- `block-gather`
- `block-strided-load`
- `block-mask`
- `masked-gather`
- `aligned-base`
- `explicit-index-pattern`
- `scatter-store`
- `load-effect-validation`
- `store-effect-validation`
- `no-alias`
- `lane-order`
- `slide-window`
- `zero-fill`
- `predicate-driven-rearrangement`
- `stable-order`
- `placement`
- `pack-unpack`
- `narrowing`
- `sign-extend`
- `zero-extend`
- `fused-op`
- `fused-expdiff`
- `conversion-result`
- `widening-op`
- `hi-lo-split`
- `accumulator`
- `index-generation`
- `ub-to-ub`
- `ordering`
- `layout-transform`
- `representative-config`
- `dist-x`
