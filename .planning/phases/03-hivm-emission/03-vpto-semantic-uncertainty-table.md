# VPTO 语义待确认文档台账

## Purpose

本文件面向 VPTO 文档贡献者，整理当前仍需要补语义结论或补文档契约的问题。

输入参考包括：

- `03-vpto-micro-op-compile-failures-review.md`
- `03-vpto-op-board-test-scope.md`
- `03-vpto-op-board-unit-tests-matrix.md`
- `03-vpto-doc-drift-review.md`

筛选原则：

- 只保留“文档不足以写出语义明确 case”或“无法定义稳定 oracle”的问题
- 不保留“语义已清楚、只是 emitter / lowering / dialect 未实现”的问题
- 不保留“已可忠实写出 case、只是 skeleton 还没修完”的问题
- 不保留“仅表现为 TD 缺 op / verifier 未接线，但不需要额外语义拍板”的问题

说明：

- 本文件只关注 docs 侧需要补什么语义
- 已经有明确结论的条目不再保留在这里

## Semantic Decisions Needed

### 1. predicate-load-store 的 packed roundtrip 是否真实存在

当前文档只在部分 store 侧给出 `PK`，但对应的 load 侧没有对称 packed surface。现有目标想验证 packed roundtrip，但按照当前 `docs/isa/04-predicate-load-store.md`，无法在不改变目标的前提下忠实写出这类 case。这里需要先决定 packed roundtrip 是否真的是正式语义；如果不是，就应该从文档中去掉这类对称 roundtrip 期待。

代表 case：

- `test/vpto/cases/micro-op/predicate-load-store/pst-pld`

### 2. `vselr` 的正式语义

当前 `vselr` 仍停留在旧的 `mask + cmp_mode` 伪接口印象上，但从 `docs/isa/11-compare-select.md`、`docs/isa/12-data-rearrangement.md` 和 `VPTOOps.td` 之间，无法唯一确定它的正式 operands、results，以及它和 `vsel` 的实际差异。这个问题本质上不是实现缺口，而是文档还没有把 op 语义说清楚。

代表 case：

- `test/vpto/cases/micro-op/compare-select/vselr`

### 3. `vcmax` / `vcmin` 的 value/index 结果布局

当前文档只说明 reduction 结果里包含 `value/index`，但没有固定结果打包方式，特别是低位 packing 细节没有收口。只要这个布局没有定下来，相关 case 就无法有稳定 oracle，docs 也无法给出明确结果表示。

代表 case：

- `test/vpto/cases/micro-op/reduction/vcmax`

### 4. `vbitsort` 的可验证语义

当前关于 `vbitsort` 的资料只有 surface 或接口层信息，还不足以说明输出到底应该如何解释。排序键、索引含义、输出布局、以及最终应如何验证，目前都没有形成可闭环的文档语义，因此不能只靠实现来反推。

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vbitsort`

### 5. integer overflow 规则未固化

一批整数相关 case 的 blocker 本质相同，不是 case 本身不会写，而是文档没有明确溢出和边界行为。例如到底是 wrap、saturate、truncate，还是某种特定边界规则，目前没有统一说法。这会影响 `vadd`、`vadds`、`vabs` 这类 op 的整数边界 case。

代表 case：

- `test/vpto/cases/micro-op/binary-vector/vadd-i16-signed-overflow`

### 6. `vadds` 的类型集合与 legality 未固化

`docs/isa/08-vec-scalar-ops.md` 当前仍偏向通用 `T` 语法，没有把 `pto.vadds` 在 A5 侧到底支持哪些类型、signed/unsigned integer 是否都合法讲清楚。只要类型集合没有固化，docs 和测试都很容易继续漂移。

代表 case：

- `test/vpto/cases/micro-op/vec-scalar/vadds-f16`

### 7. `vabs` / `vexp` 的 `bf16` 支持范围未固化

当前 `docs/isa/06-unary-vector-ops.md` 没有把 `bf16` 列入 `pto.vabs`、`pto.vexp` 的 A5 types，但 scope 和 case 又在尝试覆盖这条路径。这里需要先由文档明确：`bf16` 到底是正式支持、明确不支持，还是暂未进入 surface。

代表 case：

- `test/vpto/cases/micro-op/unary-vector/vabs-bf16`

### 8. `vcvt i32 -> i16` 是否属于正式 conversion pair

当前 `docs/isa/09-conversion-ops.md` 没有明确 `i32 -> i16` 是否在正式 conversion pair 内；即使假设它存在，对应的 overflow 规则也还没有文档化。因此这不是“实现没做完”，而是文档还没有先把 pair 和结果规则定义清楚。

代表 case：

- `test/vpto/cases/micro-op/conversion/vcvt-i32-to-i16-overflow`

## Interface / Surface Alignment Needed

### 1. unaligned / stateful store family 的 interface 未稳定

`vstu`、`vstus`、`vstur` 这组 op 一方面涉及 `mode` 参数 surface，另一方面又是 `vsta`、`vstas` 这类 flush/state-update case 的上游 producer。当前 `docs/isa/03-vector-load-store.md` 与 `VPTOOps.td` 对参数表面和上游 state 契约没有完全对齐，导致整组 op 的文档接口还不够稳定。

代表 case：

- `test/vpto/cases/micro-op/vector-load-store/vstu`

### 2. `vsldb` / `vsstb` 的 packed control word 编码规则缺失

当前文档只说 `%offset` 是 packed stride/control word，但没有进一步给出字段含义、编码方式和如何写 testcase。只要编码规则不明确，docs 就没有真正把这两个 op 的可用接口定义完整。

代表 case：

- `test/vpto/cases/micro-op/vector-load-store/vsldb`

### 3. `vusqz` 的 surface 无法支撑当前目标

当前 surface 只有 `%mask -> %result`，但文档语义又依赖一个“source-front stream”式的隐式输入。也就是说，现有表面形式不足以承载文档想表达的 placement / rearrangement 语义，这需要优先在 docs 上明确真实输入模型，而不是继续沿着现有 case 补。

代表 case：

- `test/vpto/cases/micro-op/rearrangement/vusqz`

### 4. `vaddreluconv` / `vmulconv` 的 docs、ODS、verifier 未对齐

当前失败分析显示，这两个 op 的 skeleton 还停留在旧的 `vector + scalar` 写法，而 docs/ODS 已经要求 `vector + vector` 输入。即使修正到 `vector + vector`，`conversion-result` 的 result 形状和总位宽约束又会撞到 verifier 限制。这里需要先统一哪一层才是权威 surface，再去补文档和 case。

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vaddreluconv`

### 5. `vprelu` / `vexpdiff` / `vaddrelu` / `vsubrelu` 的参数列表定义未收口

当前 PTO surface 和 LLVM 侧观察到的定义没有收口，至少存在下面这些待确认项：

- `vprelu`：LLVM 定义观察为 `3` 个 `vreg` 输入加 `1` 个 `mask` 输入
- `vexpdiff`：LLVM 定义观察为 `2` 个 `vreg` 输入加 `1` 个 `mask` 再加 `1` 个标量
- `vaddrelu` / `vsubrelu`：也需要明确它们在 LLVM 层的正式参数列表，当前 docs/ODS/PTO surface 尚未把这层关系讲清楚

这里需要先决定哪一层是权威定义，并把 PTO surface、docs 和 LLVM 参数列表关系写清楚；否则相关 case 虽然可以继续补 emitter，但接口本身仍处于未定状态。

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vprelu-f32`
- `test/vpto/cases/micro-op/dsa-sfu/vexpdiff-f32`
- `test/vpto/cases/micro-op/dsa-sfu/vaddrelu-f32`
- `test/vpto/cases/micro-op/dsa-sfu/vsubrelu-f32`

### 6. `vlds BRC_B32` 与 verifier 结论不一致

`vlds-brc-b32` 在改写到目标语义后，当前 verifier 明确拒绝 `BRC_B32`。这说明 docs、surface、verifier 至少有一处不一致。这里需要先把 `BRC_B32` 是否属于正式合法 `dist` 讲清楚，否则 docs 无法稳定描述。

代表 case：

- `test/vpto/cases/micro-op/vector-load-store/vlds-brc-b32`

## Drift / Surface Uncertainty

### 1. `vmrgsort` vs `vmrgsort4` 命名不一致

当前文档里同时出现 `vmrgsort` 和 `vmrgsort4`，但还没有明确到底是别名关系，还是一次真实 surface 重命名。这个问题首先要在 docs 里收口，否则后续 spec、case 命名和说明都会继续漂移。

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vmrgsort`

### 2. `vsort32` 是否已经是正式 surface

`docs/isa/13-dsa-sfu-ops.md` 正文已有 `pto.vsort32`，但 `docs/vpto-spec.md` 的 summary 还没有收录它。这里需要先决定：它是否已经是正式对外 surface；如果是，summary 应同步；如果不是，正文表述应降级。

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vsort32`

### 3. vec-scalar summary 未同步正文

当前 `docs/vpto-spec.md` summary 没有反映 `vsubs`、`vands`、`vors`、`vxors` 等已在 `docs/isa/08` 正文出现的 surface，导致仅看总表时会误判支持范围。这个问题虽然不是单个 op 语义未定，但属于 docs 贡献者需要补齐的表述漂移。

代表 case：

- `test/vpto/cases/micro-op/vec-scalar/vsubs`

### 4. reduction summary 未同步正文

`docs/isa/10-reduction-ops.md` 正文已经出现 `vcg*`、`vcpadd`，但 `docs/vpto-spec.md` summary 还没有同步。这里需要先决定 summary 是否应反映完整 current surface；如果不是完整 summary，也应在文档里明确说明。

代表 case：

- `test/vpto/cases/micro-op/reduction/vcgadd`

## Recommended Update Order

1. 先处理纯语义待定项：`predicate-load-store` packed roundtrip、`vselr`、`vcmax/vcmin`、`vbitsort`
2. 再处理接口/表面未对齐项：unaligned/stateful store family、`vsldb/vsstb`、`vusqz`、`vaddreluconv/vmulconv`
3. 再处理文档漂移项：`vmrgsort` 命名、`vsort32`、vec-scalar summary、reduction summary
4. 最后处理 P1 范围的类型与 overflow 规则：`vadds`、`vabs/vexp`、`vadd`、`vcvt`

完成以上结论后，建议优先回写：

1. `docs/vpto-spec.md`
2. `docs/isa/*.md`
3. `03-vpto-doc-drift-review.md`
