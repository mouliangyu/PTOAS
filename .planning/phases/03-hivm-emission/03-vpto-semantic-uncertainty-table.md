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

### 2. `vcmax` / `vcmin` 的 value/index 结果布局

当前文档只说明 reduction 结果里包含 `value/index`，但没有固定结果打包方式，特别是低位 packing 细节没有收口。只要这个布局没有定下来，相关 case 就无法有稳定 oracle，docs 也无法给出明确结果表示。

代表 case：

- `test/vpto/cases/micro-op/reduction/vcmax`

### 3. `vbitsort` 的可验证语义

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

### 5. `vprelu` / `vexpdiff` 的参数列表定义未收口

当前 PTO surface 和 LLVM 侧观察到的定义没有收口，至少存在下面这些待确认项：

- `vprelu`：LLVM 定义观察为 `3` 个 `vreg` 输入加 `1` 个 `mask` 输入
- `vexpdiff`：LLVM 定义观察为 `2` 个 `vreg` 输入加 `1` 个 `mask` 再加 `1` 个标量
这里需要先决定哪一层是权威定义，并把 PTO surface、docs 和 LLVM 参数列表关系写清楚；否则相关 case 虽然可以继续补 emitter，但接口本身仍处于未定状态。

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vprelu-f32`
- `test/vpto/cases/micro-op/dsa-sfu/vexpdiff-f32`

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

### 3. reduction summary 未同步正文

`docs/isa/10-reduction-ops.md` 正文已经出现 `vcg*`、`vcpadd`，但 `docs/vpto-spec.md` summary 还没有同步。这里需要先决定 summary 是否应反映完整 current surface；如果不是完整 summary，也应在文档里明确说明。

代表 case：

- `test/vpto/cases/micro-op/reduction/vcgadd`

## Recommended Update Order

1. 先处理纯语义待定项：`predicate-load-store` packed roundtrip、`vselr`、`vcmax/vcmin`、`vbitsort`
2. 再处理接口/表面未对齐项：unaligned/stateful store family、`vusqz`
3. 再处理文档漂移项：`vmrgsort` 命名、`vsort32`、reduction summary
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

4. `vprelu` / `vexpdiff` 的 installed contract 观察补充

当前新增观察：

- `vprelu`：installed Clang wrapper 已确认它走通用 binary-op 形式，wrapper surface 是 `dst, src0, src1, mask, mode`；其中 merge 语义通过 `vmov` 注回 `dst`
- `vexpdiff`：installed A5 headers 与 Clang wrappers 里未观察到同名 surface，`strings bisheng` 也未观察到 `llvm.hivm.vexpdiff*`
增量结论：

- `vprelu` 需要把 PTO surface、docs 和 installed wrapper / LLVM 参数列表关系写清楚
- `vexpdiff` 需要先决定 docs surface 是否真的已经落入 installed A5 toolchain contract

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vprelu-f32`
- `test/vpto/cases/micro-op/dsa-sfu/vexpdiff-f32`

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

2. `psti` / `pldi` family 的 installed wrapper 已知，repo 当前 emission 也已按该 contract 发射，但 bisheng 对 `llvm.hivm.pldi.b8` 仍无法完成指令选择

当前新增观察：

- installed Clang wrapper 已确认：
  `psti(vector_bool src, __ubuf__ uint32_t *base, int32_t offset, T dist) -> __builtin_cce_psti_b8(src, base, offset, dist.value, 0)`
  `pldi(vector_bool &dst, __ubuf__ uint32_t *base, int32_t offset, T dist) -> __builtin_cce_pldi_b8(base, offset, dist.value, 0)`
- repo 当前导出的 LLVM 形状为：
  `declare void @llvm.hivm.psti.b8(<256 x i1>, ptr addrspace(6), i32, i32, i32)`
  `declare <256 x i1> @llvm.hivm.pldi.b8(ptr addrspace(6), i32, i32, i32)`
- `strings bisheng` 已确认 `llvm.hivm.psti.b8` / `llvm.hivm.pldi.b8` 名字存在
- repo 当前生成的 `llvm.hivm.psti.b8` / `llvm.hivm.pldi.b8` 已经不再停在 `unsupported op`
- 但送 bisheng 后，`pldi.b8` 在 instruction selection 阶段仍报 `Cannot select: intrinsic %llvm.hivm.pldi.b8`

增量结论：

- 当前问题已经从“是否有 intrinsic 名字 / emission 是否缺失”收敛到“repo 当前按 wrapper 约定发射的 LLVM 形状，bisheng 后端仍无法选择”
- 在拿到 installed frontend 真实 LLVM 产物前，不能继续拍脑袋改返回形态、立即数类型、地址空间或固定尾参

代表 case：

- `test/vpto/cases/micro-op/predicate-load-store/psti-pldi`

更新：

- `2026-04-03` 已确认 `pldi/psti` 的 PTO surface 统一为 `base[offset], "DIST"`，其中 `%offset` 必须是常量 `index`；lowering 到 LLVM IR 时再转换为 intrinsic 所需的 `i32`
- 在该约束下，`test/vpto/cases/micro-op/predicate-load-store/psti-pldi` 的本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library

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
