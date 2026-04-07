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

1. `pto.vusqz` 的用户文档语义与当前实跑行为不一致

- docs 签名：
  - `%result = pto.vusqz %src, %mask : !pto.vreg<NxT>, !pto.mask<G> -> !pto.vreg<NxT>`
- vpto 签名：
  - `%result = pto.vusqz %src, %mask : !pto.vreg<NxT>, !pto.mask<G> -> !pto.vreg<NxT>`
- llvm 签名：
  - `declare <vec> @llvm.hivm.vusqz.<suffix>(<vec>, <256 x i1>)`
- 当前 docs 语义：
  - “按 `%mask` 把 `%src` 的 active-prefix payload 展开到激活位置，其余位置补 0”
- 当前实跑观察：
  - `micro-op/rearrangement/vusqz`
  - `micro-op/rearrangement/vusqz-nontrivial-mask`
  - 都已进入真实 `DEVICE=SIM` 运行，并能看到 `RV_VUSQZ`
  - 但输出模式与当前 docs 语义明显不一致，不能继续按 docs 稳定定义 oracle
- 当前阻塞点：
  - 需要 docs 贡献者明确 `pto.vusqz` 的真实用户语义；在结论明确前，这两条 case 保持 `blocked`

## Interface / Surface Alignment Needed

- 当前无保留条目

## Drift / Surface Uncertainty

- 当前无保留条目

## Recommended Update Order

1. 本文件当前没有未收口条目；新增问题时按文首追加规则登记
2. 若后续发现用户文档与实现表面漂移，优先先在 `.planning/` 留存证据，再决定是否回写 `docs/`

完成以上结论后，建议优先回写：

1. `docs/vpto-spec.md`
2. `docs/isa/*.md`
3. `03-vpto-doc-drift-review.md`

## Incremental Updates

### 2026-04-07 Batch 05

本批次继续只追加新观察，不改写上面的既有台账编号。

#### Semantic Decisions Needed Additions

1. `pto.vsts` 的 `PK` / `MRG2CHN` / `MRG4CHN` 用户文档不足以唯一写出稳定 oracle

当前新增观察：

- 代表 case：
  - `micro-op/vector-load-store/vsts-pk-b16`
  - `micro-op/vector-load-store/vsts-mrg2chn-b16`
  - `micro-op/vector-load-store/vsts-mrg4chn-b8`
- 当前 `docs/isa/03-vector-load-store.md` 只给出：
  - `PK`: “Pack low half bits of each element before store”
  - `MRG2CHN`: “Merge 2 interleaved channels within each 32B block”
  - `MRG4CHN`: “Merge 4 interleaved b8 channels within each 32B block”
- 文档尚未明确：
  - packed/merged 后的目标 UB 布局
  - 未被覆盖的目标字节是否保持原值还是置零
  - `b16` / `b8` 场景下一个 vector chunk 对应多少目标字节、如何跨 32B block 映射

当前阻塞点：

- 在不猜底层布局的前提下，现有用户文档不足以唯一写出这三条 case 的稳定 `golden.py`
- 在 docs 补齐用户可见布局语义前，这三条 case 不应通过臆造 oracle 推进

### 2026-04-01 Batch 01

本批次只追加新观察，不改写上面的既有台账编号，供 docs 贡献者按批次处理。

#### Interface / Surface Alignment Additions

1. `vprelu` / `vexpdiff` 的 installed contract 观察补充

当前新增观察：

- `vprelu`：installed Clang wrapper 已确认它走通用 binary-op 形式，wrapper surface 是 `dst, src0, src1, mask, mode`
- `vexpdiff`：当前已确认 docs surface 对应硬件 `VEXPDIF`，installed Clang wrapper 是 `vexpdif(dst, src0, src1, pg, part)`；repo emitter 当前生成的 LLVM 名字也与这一族对齐为 `llvm.hivm.vexpdif.*`
增量结论：

- `vprelu` 需要把 PTO surface、docs 和 installed wrapper / LLVM 参数列表关系写清楚
- `vexpdiff` 已不再构成 contract uncertainty，应从后续 docs/接口待确认列表中移除

代表 case：

- `test/vpto/cases/micro-op/dsa-sfu/vprelu-f32`

后续结论（2026-04-05）：

- `pto.vprelu` 的用户可见 surface 保持为当前 PTO 形式：
  `%result = pto.vprelu %input, %alpha : !pto.vreg<NxT>, !pto.vreg<NxT> -> !pto.vreg<NxT>`
- lowering 侧若需要补齐 full-mask 或其他内部参数，属于内部实现细节，不再作为用户文档层 uncertainty 继续跟踪

### 2026-04-01 Batch 02

本批次继续只追加新观察，不改写上面的既有台账编号。

#### Interface / Surface Alignment Additions

1. `vgatherb` 的 docs surface 与 installed A5 v300 contract 未收口

当前新增观察：

- `visa.txt` 已确认硬件语义为 `VGATHERB Vd, [Sn], Vn, Pg`
- 当前仓库已据此把 `pto.vgatherb` 收口为
  `pto.vgatherb %source, %offsets, %mask`
- `offsets` 语义也已改成 block gather 的 `u32` byte-offset，且仅低 `VL/8` 字节有效
- installed headers 同时暴露：
  - v300: `vgatherb(dst, base, vector_u32 indexOffset)`
  - v310: `vgatherb(dst, base, vector_u32 indexOffset, vector_bool pg)`
- 当前仓库已按带 `Pg` 的语义走 `llvm.hivm.vgatherb.v310.*(base, offsets, mask)`，本地 `COMPILE_ONLY` 已可通过

增量结论：

- `active_lanes` 漂移问题已经解决，不再是 blocker
- 若后续需要更严格对齐 installed A5 wrapper 文本 surface，还需单独确认 A5 路径是否应统一采用 v310 intrinsic 还是在全真 mask 时折叠到 v300 wrapper

代表 case：

- `test/vpto/cases/micro-op/gather-scatter/vgatherb`
- `test/vpto/cases/micro-op/gather-scatter/vgatherb-block-boundary`

2. `vldx2` / `vstsx2` 的 LLVM ABI 已收口

当前新增观察：

- installed Clang wrapper 已确认 surface 分别为
  `vld(dst0, dst1, base, offset, dist)` / `vst(src0, src1, base, offset, dist, mask)`，
  内部 builtin 都带 `0 /* #loop */`
- repo 当前生成的 `.ll` 已收口为与 `vsts` 同构的 typed intrinsic：
  `@llvm.hivm.vldsx2.v<lanes><etype>(ptr addrspace(6), i32, i32, i32) -> {<vec>, <vec>}` 与
  `@llvm.hivm.vstsx2.v<lanes><etype>(<vec>, <vec>, ptr addrspace(6), i32, i32, i32, <256 x i1>)`
- 其中 `vstsx2` 与 `vsts` 的差异仅为输入 `src` 从一个 `vreg` 扩成两个 `vreg`
- 相关 case 已在 repo 内以 `DEVICE=SIM COMPILE_ONLY=1` 走到编译产物

增量结论：

- 当前这一组不再构成 LLVM ABI blocker
- 后续若出现新问题，应按具体 case 的 runtime / board 现象单独登记，不再回退到“`vldx2/vstsx2` ABI 未收口”

代表 case：

- `test/vpto/cases/micro-op/vector-load-store/vldsx2-vstsx2`
- `test/vpto/cases/micro-op/vector-load-store/vldsx2-layout-check`
- `test/vpto/cases/micro-op/vector-load-store/vstsx2-layout-check`

### 2026-04-01 Batch 03

本批次继续只追加新观察，不改写上面的既有台账编号。

#### Drift / Surface Additions

1. `vmrgsort4` / `vsort32` / reduction summary 漂移已收口

当前新增观察：

- `docs/isa/13-dsa-sfu-ops.md` 已统一使用 `pto.vmrgsort4`
- `docs/isa/13-dsa-sfu-ops.md` 已移除未进入当前 VPTO surface 的 `pto.vsort32`
- `docs/vpto-spec.md` 的 reduction summary 已同步 `vcg*` 与 `vcpadd`

增量结论：

- 这三项不再构成 surface uncertainty

### 2026-04-01 Batch 04

本批次继续只追加新观察，不改写上面的既有台账编号。

#### LLVM ABI Alignment Additions

1. `pset_b*` 的旧 ABI 观察已过期，当前不再构成 uncertainty

当前新增观察：

- installed Clang wrapper 已确认：
  `pset_b8(T dist) -> __builtin_cce_pset_b8((ULL)dist.value)`
  `pset_b16(T dist) -> __builtin_cce_pset_b16((ULL)dist.value)`
  `pset_b32(T dist) -> __builtin_cce_pset_b32((ULL)dist.value)`
- `strings bisheng` 已确认 `llvm.hivm.pset.b8` / `llvm.hivm.pset.b16` / `llvm.hivm.pset.b32` 名字存在
- 当前 repo-generated `.ll` 已确认发射为：
  `declare <256 x i1> @llvm.hivm.pset.b8(i32)`
  `declare <256 x i1> @llvm.hivm.pset.b16(i32)`
  `declare <256 x i1> @llvm.hivm.pset.b32(i32)`

增量结论：

- 旧的 `i64` 参数表观察已经过期
- 当前 `pset_b*` 的 repo ABI 证据与 compile-only 回归一致，不再作为语义/ABI uncertainty 继续跟踪

代表 case：

- `test/vpto/cases/micro-op/materialization-predicate/pset-pattern`
- `test/vpto/cases/micro-op/materialization-predicate/pset-pattern-fragment`

2. `pge_b*` 的旧 ABI 观察已过期，当前不再构成 uncertainty

当前新增观察：

- installed Clang wrapper 已确认：
  `pge_b8(T dist) -> __builtin_cce_pge_b8((ULL)dist.value, 0)`
  `pge_b16(T dist) -> __builtin_cce_pge_b16((ULL)dist.value, 0)`
  `pge_b32(T dist) -> __builtin_cce_pge_b32((ULL)dist.value, 0)`
- `strings bisheng` 已确认 `llvm.hivm.pge.b8` / `llvm.hivm.pge.b16` / `llvm.hivm.pge.b32` 名字存在
- 当前 repo-generated `.ll` 已确认发射为：
  `declare <256 x i1> @llvm.hivm.pge.b8(i32, i32)`
  `declare <256 x i1> @llvm.hivm.pge.b16(i32, i32)`
  `declare <256 x i1> @llvm.hivm.pge.b32(i32, i32)`

增量结论：

- 旧的 `i64, i64` 参数表观察已经过期
- 当前 `pge_b*` 的 repo ABI 证据与 compile-only 回归一致，不再作为语义/ABI uncertainty 继续跟踪

代表 case：

- `test/vpto/cases/micro-op/materialization-predicate/pge-tail-mask`
- `test/vpto/cases/micro-op/materialization-predicate/pge-tail-mask-boundary`

3. `ppack` / `punpack` 的旧 ABI 观察已过期，当前不再构成 uncertainty

当前新增观察：

- installed Clang wrapper 已确认：
  `ppack(vector_bool &dst, vector_bool src, T part) -> __builtin_cce_ppack_z(src, (ULL)part.value)`
  `punpack(vector_bool &dst, vector_bool src, T part) -> __builtin_cce_punpack(src, (ULL)part.value)`
- `strings bisheng` 已确认 `llvm.hivm.ppack.z` 与 `llvm.hivm.punpack` 名字存在
- 当前 repo-generated `.ll` 已确认发射为：
  `declare <256 x i1> @llvm.hivm.ppack.z(<256 x i1>, i32)`
  `declare <256 x i1> @llvm.hivm.punpack(<256 x i1>, i32)`

增量结论：

- 旧的 `i64` `part` 参数观察已经过期
- 当前 `ppack/punpack` 的 repo ABI 证据与 compile-only 回归一致，不再作为语义/ABI uncertainty 继续跟踪

代表 case：

- `test/vpto/cases/micro-op/materialization-predicate/ppack-punpack`
- `test/vpto/cases/micro-op/materialization-predicate/ppack-punpack-nontrivial`

### 2026-04-01 Batch 05

本批次继续按追加方式记录新观察，不改写既有编号。

#### LLVM ABI Alignment Additions

1. `psti` / `pldi` family 的旧 instruction-selection 观察已过期，当前不再构成 uncertainty

当前新增观察：

- installed Clang wrapper 已确认：
  `psti(vector_bool src, __ubuf__ uint32_t *base, int32_t offset, T dist) -> __builtin_cce_psti_b8(src, base, offset, dist.value, 0)`
  `pldi(vector_bool &dst, __ubuf__ uint32_t *base, int32_t offset, T dist) -> __builtin_cce_pldi_b8(base, offset, dist.value, 0)`
- repo 当前导出的 LLVM 形状为：
  `declare void @llvm.hivm.psti.b8(<256 x i1>, ptr addrspace(6), i32, i32, i32)`
  `declare <256 x i1> @llvm.hivm.pldi.b8(ptr addrspace(6), i32, i32, i32)`
- `strings bisheng` 已确认 `llvm.hivm.psti.b8` / `llvm.hivm.pldi.b8` 名字存在
- 当前 repo 里的 `psti/pldi` 组合 case 已可通过 `DEVICE=SIM COMPILE_ONLY=1`，说明旧的 `Cannot select` 观察已经不再代表当前状态

增量结论：

- 旧的 instruction-selection 失败观察已经过期
- 当前 `psti/pldi` surface、repo ABI 与 compile-only 回归已经一致，不再作为语义/ABI uncertainty 继续跟踪

代表 case：

- `test/vpto/cases/micro-op/predicate-load-store/psti-pk-pldi-us`

更新：

- `2026-04-03` 已确认 `pldi/psti` 的 PTO surface 统一为 `base[offset], "DIST"`，其中 `%offset` 必须是常量 `index`；lowering 到 LLVM IR 时再转换为 intrinsic 所需的 `i32`
- 在该约束下，`test/vpto/cases/micro-op/predicate-load-store/psti-pk-pldi-us` 的本地 `DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library

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
