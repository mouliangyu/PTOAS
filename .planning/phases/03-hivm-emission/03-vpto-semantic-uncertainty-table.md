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

维护规则：

- 更新本文件前，先全文检索待记录问题是否已经存在于正文台账或 `Incremental Updates` 批次中
- 如果已有对应条目，只在原条目或原批次下补充新增观察，不重复新开同义条目
- 如果检索后确认不存在，才允许在 `Incremental Updates` 末尾追加新的 batch 或在当前 batch 下追加新条目
- 不要通过改写既有正文编号来表达本轮新增问题；新增问题一律按追加方式记录，便于 docs 贡献者分批消费

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

## Incremental Updates

### 2026-04-01 Batch 01

本批次只追加新观察，不改写上面的既有台账编号，供 docs 贡献者按批次处理。

#### Interface / Surface Alignment Additions

1. `vsld` 的 LLVM intrinsic ABI 未收口

当前新增观察：

- installed Clang headers 已确认 `vsld` wrapper surface 为 `vsld(vector_<T> &dst, __ubuf__ <elt>* base, vector_address offset, stride)`
- wrapper 内部调用 `__builtin_cce_vsld_*`
- `strings bisheng` 已确认 `llvm.hivm.vsld` intrinsic 名称存在
- 但当前 repo 生成的 LLVM IR 形式 `declare <64 x float> @llvm.hivm.vsld(ptr addrspace(6), i32, i32, i32)` 会被 bisheng 直接拒绝为 `Intrinsic has incorrect argument type`

增量结论：

- 这说明问题已经不是 case 写法或 docs surface，而是 LLVM 层正式 ABI 还没有被确认下来
- 在没有进一步 tracing 到真实 contract 前，不能继续猜参数类型或补 emitter

代表 case：

- `test/vpto/cases/micro-op/vector-load-store/vsld`

2. `vsst` 的 stride contract 未收口

当前新增观察：

- installed A5 wrapper 当前直接给出的 `vsst` surface 为 `vsst(vector_<T> src, __ubuf__ <elt>* base, vector_address offset, stride)`
- `npu_arch_3101/__clang_cce_vector_intrinsics.h` 对第 `4` 个参数做了 `static_assert(std::is_same<T, __cce_simd::S8_B16_Type>::value)`
- 也就是说，installed wrapper 当前只明确暴露 `S8_B16` 这一个 stride class
- 当前 docs / case 里已经在写 `STRIDE_S2_B64`

增量结论：

- 只要这层 contract 没有先收口，就不能继续把别的 stride immediate 当成已确认支持面去补 emitter

代表 case：

- `test/vpto/cases/micro-op/vector-load-store/vsst`

3. `vmov` 的 PTO surface 与 LLVM 形态未收口

当前新增观察：

- 当前能走通 compile-only 的形式是 `llvm.hivm.vmov.*.m`
- 观察到它需要 `2` 个 `vreg` 输入再加 `1` 个 `mask`
- 这和当前 `docs/isa/06-unary-vector-ops.md` 与 `VPTOOps.td` 中 `pto.vmov` 的 unary surface 还没有正式收口

增量结论：

- 需要补清第二个 `vreg` 是否是 merge-dst、是否必须与 source 同值、以及这是否仍然忠实表达文档里的 predicated copy 语义

代表 case：

- `test/vpto/cases/micro-op/unary-vector/vmov`
- `test/vpto/cases/micro-op/unary-vector/vmov-tail`

4. `vprelu` / `vexpdiff` / `vaddrelu` / `vsubrelu` 的 installed contract 观察补充

当前新增观察：

- `vprelu`：installed Clang wrapper 已确认它走通用 binary-op 形式，wrapper surface 是 `dst, src0, src1, mask, mode`；其中 merge 语义通过 `vmov` 注回 `dst`
- `vexpdiff`：installed A5 headers 与 Clang wrappers 里未观察到同名 surface，`strings bisheng` 也未观察到 `llvm.hivm.vexpdiff*`
- `vaddrelu` / `vsubrelu`：installed A5 headers、Clang wrappers、以及 `strings bisheng` 中都未观察到同名 surface / intrinsic

增量结论：

- `vprelu` 需要把 PTO surface、docs 和 installed wrapper / LLVM 参数列表关系写清楚
- `vexpdiff` / `vaddrelu` / `vsubrelu` 需要先决定 docs surface 是否真的已经落入 installed A5 toolchain contract

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vprelu-f32`
- `test/vpto/cases/micro-op/dsa-sfu/vexpdiff-f32`
- `test/vpto/cases/micro-op/dsa-sfu/vaddrelu-f32`
- `test/vpto/cases/micro-op/dsa-sfu/vsubrelu-f32`

#### Drift / Surface Additions

1. `vsubs` 的 docs surface 与 installed toolchain 支持面未收口

当前新增观察：

- `docs/isa/08-vec-scalar-ops.md` 已给出 `pto.vsubs` surface
- 但 installed Clang headers 中未观察到对应 `vsubs` wrapper
- `strings bisheng` 也未观察到 `llvm.hivm.vsubs.*`

增量结论：

- 这不只是 summary 漂移，而是 docs 是否真的在描述一个已落入 installed A5 toolchain contract 的 op 还没有收口

代表 case：

- `test/vpto/cases/micro-op/vec-scalar/vsubs`

### 2026-04-01 Batch 02

本批次继续只追加新观察，不改写上面的既有台账编号。

#### Interface / Surface Alignment Additions

1. `vgatherb` 的 docs surface 与 installed A5 v300 contract 未收口

当前新增观察：

- `docs/isa/03-vector-load-store.md` 与 `VPTOOps.td` 当前都把 `pto.vgatherb` 定义为
  `pto.vgatherb %source, %offsets, %active_lanes`
- 但 installed Clang wrapper 的 A5 v300 直接形式是
  `vgatherb(vector_<T> &dst, __ubuf__ <elt> *base, vector_u32 indexOffset)`
- 也就是 installed v300 surface 没有显式 `mask` / `active_lanes` 参数
- 先前按 `active_lanes -> llvm.hivm.plt.* -> llvm.hivm.vgatherb.*(base, offsets, mask)` 的 emission 虽可产出 `.ll`，
  但 bisheng verifier 直接以 `Intrinsic has incorrect argument type! ptr @llvm.hivm.vgatherb.v300.v64f32` 拒绝

增量结论：

- 这里已经不是普通 emitter 缺口，而是 PTO/docs surface 到 installed v300 contract 的关系没有收口
- 在确认 `active_lanes` 是否应被忽略、折叠、还是 docs surface 本身需要改写之前，不能继续猜 `vgatherb` 的 LLVM 形态

代表 case：

- `test/vpto/cases/micro-op/gather-scatter/vgatherb`
- `test/vpto/cases/micro-op/gather-scatter/vgatherb-block-boundary`

2. `vldx2` / `vstx2` 的 LLVM ABI 仍未收口

当前新增观察：

- installed Clang wrapper 已确认 surface 分别为
  `vld(dst0, dst1, base, offset, dist)` / `vst(src0, src1, base, offset, dist, mask)`，
  内部 builtin 都带 `0 /* #loop */`
- repo 当前生成的 `.ll` 已调整到
  `@llvm.hivm.vldx2(ptr addrspace(6), i64, i64, i64)` 与
  `@llvm.hivm.vstx2(<vec>, <vec>, ptr addrspace(6), i64, i64, i64, <256 x i1>)`
- 但完整 repo module 送 bisheng 时，仍会在 verifier 阶段报
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.vldx2`
  与 `Intrinsic has incorrect argument type! ptr @llvm.hivm.vstx2`
- 与之相对，先前单独构造的最小 probe 在同名 signature 下可以越过 verifier

增量结论：

- 当前剩余问题不是 case 写法，也不宜继续拍脑袋改 `i32/i64` 或返回形态
- 需要进一步 tracing 完整 repo module 与最小 probe 之间的真实 ABI 差异，再决定 emitter 应该如何收口

代表 case：

- `test/vpto/cases/micro-op/vector-load-store/vldx2-vstx2`
- `test/vpto/cases/micro-op/vector-load-store/vldx2-layout-check`
- `test/vpto/cases/micro-op/vector-load-store/vstx2-layout-check`

### 2026-04-01 Batch 03

本批次继续只追加新观察，不改写上面的既有台账编号。

#### Drift / Surface Additions

1. `vands` / `vors` / `vxors` 的 docs surface 与 installed toolchain 支持面未收口

当前新增观察：

- `docs/isa/08-vec-scalar-ops.md` 与当前 testcase 都把这三条 op 定义成
  `input + scalar + mask -> result`
- 但 installed Clang headers 中未直接观察到 `vands` / `vors` / `vxors` 同名 wrapper
- `strings bisheng` 里也未观察到 `llvm.hivm.vands.*` / `llvm.hivm.vors.*` / `llvm.hivm.vxors.*`
- 当前 emitter 继续往下推进只会落到“unsupported op”而不会产生可验证的 LLVM contract

增量结论：

- 这三条不能按普通 emitter 缺口处理；需要先确认 docs 里的 PTO surface 是否真对应 installed A5 contract，或者存在命名映射

代表 case：

- `test/vpto/cases/micro-op/vec-scalar/vands`
- `test/vpto/cases/micro-op/vec-scalar/vors`
- `test/vpto/cases/micro-op/vec-scalar/vxors`
- `test/vpto/cases/micro-op/vec-scalar/vands-mask-edge`
- `test/vpto/cases/micro-op/vec-scalar/vors-mask-edge`
- `test/vpto/cases/micro-op/vec-scalar/vxors-mask-edge`

2. `vshls` / `vshrs` 的 docs surface 与 `VPTOOps.td` 当前定义未收口

当前新增观察：

- `docs/isa/08-vec-scalar-ops.md` 将两条 op 都定义为
  `input + scalar + mask -> result`
- 但 `VPTOOps.td` 当前把 `pto.vshls` / `pto.vshrs` 都定义成 `PTO_VecScalarOp`，即只有
  `input + scalar -> result`，没有 `mask`
- 当前 testcase 也因此只能按无 mask 的 ODS surface 落地
- installed toolchain 侧虽然能观察到 `llvm.hivm.vshls.*` / `llvm.hivm.vshrs.*`，但在 docs / ODS / testcase 三者未先收口前，不能擅自选择其中一套语义继续补 emitter

增量结论：

- 这里的主问题不是 LLVM intrinsic 不存在，而是 PTO surface 还没有在 docs 与 ODS 间统一
- 在 surface 收口前，不能把 compile 失败简单归类成 emitter 缺口

代表 case：

- `test/vpto/cases/micro-op/vec-scalar/vshls`
- `test/vpto/cases/micro-op/vec-scalar/vshrs`
- `test/vpto/cases/micro-op/vec-scalar/vshls-shift-boundary`
- `test/vpto/cases/micro-op/vec-scalar/vshrs-shift-boundary`

### 2026-04-01 Batch 04

本批次继续只追加新观察，不改写上面的既有台账编号。

#### LLVM ABI Alignment Additions

1. `pset_b*` 的 installed wrapper 已知，但对应 `llvm.hivm.pset.*` 参数表仍未收口

当前新增观察：

- installed Clang wrapper 已确认：
  `pset_b8(T dist) -> __builtin_cce_pset_b8((ULL)dist.value)`
  `pset_b16(T dist) -> __builtin_cce_pset_b16((ULL)dist.value)`
  `pset_b32(T dist) -> __builtin_cce_pset_b32((ULL)dist.value)`
- `strings bisheng` 已确认 `llvm.hivm.pset.b8` / `llvm.hivm.pset.b16` / `llvm.hivm.pset.b32` 名字存在
- repo 当前按 `(<256 x i1> <- i64)` 生成 `.ll`，送 bisheng verifier 时会直接报
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.pset.b8`
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.pset.b16`
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.pset.b32`
- 进一步尝试用 ad hoc frontend probe 直接 `-emit-llvm` 追 intrinsic 形状时，又先撞上了该机器上独立 frontend 环境不完整的问题，未能拿到可用 LLVM IR

增量结论：

- 当前问题不是 docs / ODS surface 漂移，而是 installed frontend 对 `llvm.hivm.pset.*` 的真实 LLVM ABI 还没有被确认
- 在拿到真实 frontend 产物前，不能继续拍脑袋改返回类型或立即数类型

代表 case：

- `test/vpto/cases/micro-op/materialization-predicate/pset-pattern`
- `test/vpto/cases/micro-op/materialization-predicate/pset-pattern-fragment`

2. `pge_b*` 的 installed wrapper 已知，但对应 `llvm.hivm.pge.*` 参数表仍未收口

当前新增观察：

- installed Clang wrapper 已确认：
  `pge_b8(T dist) -> __builtin_cce_pge_b8((ULL)dist.value, 0)`
  `pge_b16(T dist) -> __builtin_cce_pge_b16((ULL)dist.value, 0)`
  `pge_b32(T dist) -> __builtin_cce_pge_b32((ULL)dist.value, 0)`
- `strings bisheng` 已确认 `llvm.hivm.pge.b8` / `llvm.hivm.pge.b16` / `llvm.hivm.pge.b32` 名字存在
- repo 当前按 `(<256 x i1> <- i64, i64)` 生成 `.ll`，送 bisheng verifier 时会直接报
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.pge.b8`
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.pge.b16`
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.pge.b32`

增量结论：

- 当前问题不是 docs / ODS surface 漂移，而是 installed frontend 对 `llvm.hivm.pge.*` 的真实 LLVM ABI 还没有被确认
- 在拿到真实 frontend 产物前，不能继续拍脑袋改第二个固定参数或返回类型

代表 case：

- `test/vpto/cases/micro-op/materialization-predicate/pge-tail-mask`
- `test/vpto/cases/micro-op/materialization-predicate/pge-tail-mask-boundary`

3. `ppack` / `punpack` 的 installed wrapper 已知，但对应 `llvm.hivm.ppack.z` / `llvm.hivm.punpack` 参数表仍未收口

当前新增观察：

- installed Clang wrapper 已确认：
  `ppack(vector_bool &dst, vector_bool src, T part) -> __builtin_cce_ppack_z(src, (ULL)part.value)`
  `punpack(vector_bool &dst, vector_bool src, T part) -> __builtin_cce_punpack(src, (ULL)part.value)`
- `strings bisheng` 已确认 `llvm.hivm.ppack.z` 与 `llvm.hivm.punpack` 名字存在
- repo 当前按 `llvm.hivm.ppack.z(<256 x i1>, i64)` / `llvm.hivm.punpack(<256 x i1>, i64)` 生成 `.ll`，送 bisheng verifier 时会直接报
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.ppack.z`
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.punpack`

增量结论：

- 当前问题不是 testcase 目标不合理，也不是 docs / ODS surface 漂移，而是 installed frontend 对这两条 intrinsic 的真实 LLVM ABI 还没有被确认
- 在拿到真实 frontend 产物前，不能继续拍脑袋改 `part` 的 lowering 形态

代表 case：

- `test/vpto/cases/micro-op/materialization-predicate/ppack-punpack`
- `test/vpto/cases/micro-op/materialization-predicate/ppack-punpack-nontrivial`

### 2026-04-01 Batch 05

本批次继续按追加方式记录新观察，不改写既有编号。

#### LLVM ABI Alignment Additions

1. `pst` / `pld` family 的 installed wrapper 已知，但对应 `llvm.hivm.pst.b8` / `llvm.hivm.pld.b8` 参数表仍未收口

当前新增观察：

- installed Clang wrapper 已确认：
  `pst(vector_bool src, __ubuf__ uint32_t *base, vector_address offset, T dist) -> __builtin_cce_pst_b8(src, base, offset, dist.value, 0)`
  `pld(vector_bool &dst, __ubuf__ uint32_t *base, vector_address offset, T dist) -> __builtin_cce_pld_b8(base, offset, dist.value, 0)`
- `strings bisheng` 已确认 `llvm.hivm.pst.b8` / `llvm.hivm.pld.b8` 名字存在
- repo 当前按 `llvm.hivm.pst.b8(<256 x i1>, ptr addrspace(6), i32, i32, i32)` / `llvm.hivm.pld.b8(ptr addrspace(6), i32, i32, i32)` 生成 `.ll`，送 bisheng verifier 时会直接报
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.pst.b8`
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.pld.b8`

增量结论：

- 当前问题不是 docs / ODS surface 漂移，而是 installed frontend 对 `pst/pld` family 的真实 LLVM ABI 还没有被确认
- 在拿到真实 frontend 产物前，不能继续拍脑袋改 `vector_address` 或 `dist` 的 lowering 形态

代表 case：

- `test/vpto/cases/micro-op/predicate-load-store/pst-pld`

2. `psti` / `pldi` family 的 installed wrapper 已知，但对应 `llvm.hivm.psti.b8` / `llvm.hivm.pldi.b8` 选择契约仍未收口

当前新增观察：

- installed Clang wrapper 已确认：
  `psti(vector_bool src, __ubuf__ uint32_t *base, int32_t offset, T dist) -> __builtin_cce_psti_b8(src, base, offset, dist.value, 0)`
  `pldi(vector_bool &dst, __ubuf__ uint32_t *base, int32_t offset, T dist) -> __builtin_cce_pldi_b8(base, offset, dist.value, 0)`
- `strings bisheng` 已确认 `llvm.hivm.psti.b8` / `llvm.hivm.pldi.b8` 名字存在
- repo 当前生成的 `llvm.hivm.psti.b8` / `llvm.hivm.pldi.b8` 已经不再停在 `unsupported op`
- 但送 bisheng 后，`pldi.b8` 在 instruction selection 阶段仍报 `Cannot select: intrinsic %llvm.hivm.pldi.b8`

增量结论：

- 当前问题已经从“是否有 intrinsic 名字”收敛到“installed frontend 对 `psti/pldi` family 的真实 LLVM contract 还没有被确认”
- 在拿到真实 frontend 产物前，不能继续拍脑袋改返回形态、立即数类型或固定尾参

代表 case：

- `test/vpto/cases/micro-op/predicate-load-store/psti-pldi`

3. `vaxpy` family 的 installed wrapper 已知，但对应 `llvm.hivm.vaxpy.*` 的 LLVM 参数顺序 / 选择契约仍未收口

当前新增观察：

- installed Clang wrapper 已确认 `vaxpy(vector_<T> &dst, vector_<T> src0, scalar, vector_bool mask, mode)` family 存在
- `docs/isa/13-dsa-sfu-ops.md` 把 VPTO surface 定义为纯 op：`%result = pto.vaxpy %src0, %src1, %alpha`
- `strings bisheng` 已确认 `llvm.hivm.vaxpy.v64f32.x` / `llvm.hivm.vaxpy.v128f16.x` 名字存在
- repo 当前按“result 作为更新后的 addend vector”生成 `llvm.hivm.vaxpy.v64f32.x` 调用后，bisheng 在 instruction selection 阶段仍报 `Cannot select`

增量结论：

- 当前问题不是 docs 是否存在该 op，而是 VPTO 纯 SSA surface 与 installed wrapper 的 `dst + src0 + scalar + mask` 关系尚未落到真实 LLVM contract
- 在拿到 installed frontend 的真实 LLVM 形状前，不能继续拍脑袋调整参数顺序或 merge 语义

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vaxpy-f32`

4. `vci` family 的 installed wrapper 已知，但对应 `llvm.hivm.vci.*` 参数表仍未收口

当前新增观察：

- installed Clang wrapper 已确认：
  `vci(vector_s8/s16/s32 dst, index, order)` 与
  `vci(vector_f16/f32 dst, index, order)` family 存在
- `strings bisheng` 已确认 `llvm.hivm.vci.v256s8` / `v128s16` / `v64s32` / `v128f16.f16` / `v64f32.f32` 名字存在
- repo 当前以 `micro-op/dsa-sfu/vci` 生成 `declare <64 x i32> @llvm.hivm.vci.v64s32(i32, i64)`
- 送 bisheng verifier 后直接报
  `Intrinsic has incorrect argument type! ptr @llvm.hivm.vci.v64s32`

增量结论：

- 当前问题不是 docs 是否存在 `pto.vci`，而是 installed frontend 对 `llvm.hivm.vci.*` 的真实 LLVM ABI 还没有被确认
- 在拿到真实 frontend 产物前，不能继续拍脑袋改 `order` 的整数位宽或返回形状

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vci`

5. `pstu` 的 surface 类型集合未收口

当前新增观察：

- `docs/isa/04-predicate-load-store.md` / `docs/vpto-spec.md` 当前把 `pto.pstu` 写成泛型 `!pto.ptr<T, ub>`
- installed Clang wrapper 只明确暴露 `__builtin_cce_pstu_b16` / `__builtin_cce_pstu_b32`
- 当前 repo testcase `test/vpto/cases/micro-op/predicate-load-store/pstu/kernel.pto` 仍以 `!pto.ptr<ui8, ub>` 书写

增量结论：

- 当前问题首先不是 emitter 没接，而是 docs surface、testcase 与 installed type contract 没有收口
- 在文档明确 `pstu` 是否真的接受 `ui8` 基址前，不应继续猜 LLVM lowering

代表 case：

- `test/vpto/cases/micro-op/predicate-load-store/pstu`
- `test/vpto/cases/micro-op/predicate-load-store/pstu-state-advance-boundary`

6. `vtranspose` 没有观察到同名 HIVM intrinsic，当前只看到 helper 级实现

当前新增观察：

- `docs/isa/13-dsa-sfu-ops.md` 把 `pto.vtranspose` 定义为 UB-to-UB helper，输入是 `%dest, %src, %config`
- `strings bisheng` 未观察到 `llvm.hivm.vtranspose` / `llvm.hivm.transpose` family
- installed A5 `TTrans.hpp` 当前通过 `vci + vmuls + vadds + vgather2 + vsts` 的 helper 序列实现转置
- 当前 repo testcase 只携带一个 `i64 %config`，但尚未看到这个 `%config` 如何与 installed helper 模板参数一一对应

增量结论：

- 当前问题不是简单“缺少一个 intrinsic 名字”，而是 `vtranspose` 到 helper 序列的 lowering 规则尚未收口
- 在 `config` 与 helper 参数关系明确前，不应直接猜单条 LLVM intrinsic 或随意展开成某个特定序列

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vtranspose`
- `test/vpto/cases/micro-op/dsa-sfu/vtranspose-multi-config`

7. `vpack` 的 PTO/docs surface 与 installed wrapper 参数表不一致

当前新增观察：

- `docs/isa/12-data-rearrangement.md` / `VPTOOps.td` 当前把 `pto.vpack` 定义为双输入：
  `%result = pto.vpack %src0, %src1, %part`
- installed Clang wrapper 当前只明确暴露单输入：
  `vpack(vector_<narrow> &dst, vector_<wide> src, part, mode)`
- `strings bisheng` 观察到的也是单源 family：
  `llvm.hivm.vpack.s322u16.*` / `llvm.hivm.vpack.u322u16.*`

增量结论：

- 当前不是简单 emitter 未接，而是 PTO/docs surface 与 installed contract 没有收口
- 在确认双输入 PTO surface 如何映射到真实 A5 contract 前，不应继续拍脑袋补 LLVM lowering

代表 case：

- `test/vpto/cases/micro-op/rearrangement/vpack`

8. `vperm` 的 docs 命名与 installed contract 没有建立直接映射

当前新增观察：

- `docs/isa/12-data-rearrangement.md` 把 `pto.vperm` 定义成 in-register `%src + %index` permute
- `strings bisheng` 未观察到 `llvm.hivm.vperm.*`
- installed trace 只明确观察到 memory-based `vgatherb` / `vgather2` family
- `docs/vpto-spec.md` 也保留了 “`pto.vperm` naming: a5_intrinsic `vgather` mapped to `pto.vperm`” 的待确认说明

增量结论：

- 当前不是简单 emitter 缺口，而是 docs 命名/语义与 installed contract 未收口
- 在确认 `pto.vperm` 是否真对应某个 gather family、以及是否仍保持 in-register 语义前，不应继续猜 LLVM lowering

代表 case：

- `test/vpto/cases/micro-op/rearrangement/vperm`

9. `vshift` 的 single-source zero-fill 语义尚未映射到明确的 installed LLVM contract

当前新增观察：

- `docs/isa/12-data-rearrangement.md` 把 `pto.vshift` 定义为 single-source zero-fill slide
- `docs/vpto-spec.md` 保留了 “a5_intrinsic `vsld` mapped to `pto.vshift`” 的待确认说明
- installed A5 当前只明确暴露 memory `vsld` family 与 in-register `vslide` family
- 当前尚未拿到能够证明 `pto.vshift == vslide(src, zero, amt)` 或其他等价式的 installed frontend 证据

增量结论：

- 当前不是简单 emitter 缺口，而是 op 命名/contract 未收口
- 在 installed frontend 明确真实映射前，不应继续拍脑袋把 `pto.vshift` 降成 `vslide` 或 `vsld`

代表 case：

- `test/vpto/cases/micro-op/rearrangement/vshift`
- `test/vpto/cases/micro-op/rearrangement/vshift-tail-zero-fill`
