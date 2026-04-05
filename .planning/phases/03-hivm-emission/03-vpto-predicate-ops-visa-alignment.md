# VPTO Predicate Ops VISA Alignment

基线：
- 来源：`visa.txt`
- 目标：只整理 `visa.txt` 已出现的 predicate 相关指令线索，与当前 PTO/VPTO surface、docs、实现现状做对齐
- 范围：本方案只讨论 predicate ops；不扩展到 vector load/store 或其他非 predicate 家族
- 约束：本文件不为 `visa.txt` 未出现的 predicate op 猜语义；这类 op 需要继续从 installed impl / Bisheng tracing 补证
- 支持边界：凡是依赖隐式寻址或隐式状态推进的形式，不纳入当前 VPTO/PTO 支持范围

准则：
- 本方案仅覆盖 predicate ops；非 predicate 指令不应混入本文件
- `visa.txt` 是 predicate family 能力范围的基线
- 对于 `visa.txt` 已定义的 predicate op / type family，只要 installed Bisheng / Clang wrapper / `strings bisheng` 证明确实支持，就应进入 PTO/VPTO 支持范围，而不应长期停留在“当前 surface 只暴露了子集”的状态
- “仓库当前尚未建模某个 type family” 不能作为能力裁剪依据
- 一旦 `visa.txt` 与 installed Bisheng 证实该 family 受支持，就应将其登记为待实现项，并按同一 family 的现有实现路径补齐 TD、verifier、emitter、tests、docs
- 只有在开发者明确确认存在额外 blocker 时，才允许暂缓；暂缓原因必须记录为具体 blocker，不能写成笼统的“当前未建模”或“当前 surface 只暴露子集”
- 若无明确 blocker，不允许在 plan 或公开 docs 中继续把“当前 surface 只暴露子集”当作结论
- 依赖隐式寻址/状态推进的 predicate 指令需要在本方案中明确标注为“不支持”
- 这些不支持的 predicate 指令，以及 `Am` 这一底层术语，都不应出现在 `docs/isa/` 等用户文档中；用户文档只描述当前保留的 PTO surface
- post 访存除非 LLVM 仅支持 post 形式，否则不纳入本阶段支持计划；已安装实现中的 post overload 只能作为旁证，不应倒推本阶段 surface

## 0. 显式寻址 Surface 边界

当前边界不是“所有 load/store 都不支持”，而是：

- 不支持：依赖隐式寻址、隐式状态推进、或专用地址状态语义的形式
- 支持：显式 `base[offset]` 与显式 `DIST` 的 predicate load/store 形式

明确排除的 op / 形式：

- `pto.pld`
  - 指令属性：`Am` 类 predicate load 指令
  - 原因：对应隐式寻址/状态推进的 predicate load 形式，不进入当前 PTO surface
- `pto.pst`
  - 指令属性：`Am` 类 predicate store 指令
  - 原因：对应隐式寻址/状态推进的 predicate store 形式，不进入当前 PTO surface

明确保留在支持范围内的相邻 op：

- `pto.pldi`
- `pto.plds`
- `pto.psti`
- `pto.psts`
- `pto.pstu`

这些 op 之所以保留，是因为当前 surface 都采用显式 pointer / offset / state 形式，不属于 `Am` 指令族，也不要求额外的隐式地址状态参与。

## 1. 直接证据

- `pset`
  - `visa.txt` 证据：
    - `p0 = pset(#all_true); //PSET`
  - 当前 PTO surface：
    - `pto.pset_b8`
    - `pto.pset_b16`
    - `pto.pset_b32`
  - 当前 docs：
    - [05-materialization-predicate.md](/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/docs/isa/05-materialization-predicate.md) 已收紧为明确 token 集合
  - 当前实现：
    - `include/PTO/IR/VPTOOps.td` 已定义
    - `lib/PTO/IR/VPTO.cpp` 已有 verifier
    - `lib/PTO/Transforms/VPTOLLVMEmitter.cpp` 已有 emitter
  - 当前补证：
    - installed Clang header `__clang_cce_vector_intrinsics.h` 中存在 `PAT_ALL`、`PAT_ALLF`、`PAT_H`、`PAT_Q`、`PAT_VL1/2/3/4/8/16/32/64/128`、`PAT_M3`、`PAT_M4` 的具体常量定义
    - `strings bisheng` 确认存在 `llvm.hivm.pset.b8/.b16/.b32`
    - repo-generated `.ll` 旁证：
      - `micro-op/materialization-predicate/pset-pattern` 当前发射到
        `llvm.hivm.pset.b8` / `llvm.hivm.pset.b16` / `llvm.hivm.pset.b32`
  - 收敛结论：
    - `PAT_ALL <-> #all_true` 映射已可在公开 docs 中明确
    - 公开 docs 不应继续把 `pset` 写成泛化 `PAT_*`；应明确支持 token 集合

- `plt`
  - `visa.txt` 证据：
    - `p0 = plt(N % VL); //PLT`
  - 当前 PTO surface：
    - `pto.plt_b8`
    - `pto.plt_b16`
    - `pto.plt_b32`
    - 当前签名是 `%mask, %scalar_out = pto.plt_b* %scalar : i32 -> !pto.mask<b*>, i32`
  - 当前 docs：
    - 已明确为 post-update wrapper 语义下的 `%mask + %scalar_out` 双结果 surface
  - 当前实现：
    - `include/PTO/IR/VPTOOps.td` 已定义双结果 surface
    - `lib/PTO/IR/VPTO.cpp` 已有 verifier
    - `lib/PTO/Transforms/VPTOLLVMEmitter.cpp` 已接线
  - 当前补证：
    - installed Clang header `__VF_PLT_V300` wrapper 采用 `plt_##TYPE(SCALAR &scalar, POST_UPDATE)` 形式
    - wrapper 内部通过 `__builtin_cce_plt_##TYPE##_v300(&ret, scalar)` 同时返回 `mask` 与 `scalar_out`
    - `strings bisheng` 确认存在 `llvm.hivm.plt.b8/.b16/.b32` 及 `.v300` 变体
    - repo-generated `.ll` 旁证：
      - `micro-op/materialization-predicate/plt-tail-mask` 当前发射到
        `llvm.hivm.plt.b8.v300` / `llvm.hivm.plt.b16.v300` / `llvm.hivm.plt.b32.v300`
      - LLVM IR 中 `plt` 调用结果类型为 `{ <256 x i1>, i32 }`，第二结果会被 `extractvalue`
  - 收敛结论：
    - `%scalar_out` 不是 repo-local 臆造结果，而是 source-level 对 installed V300 post-update wrapper 的直接建模
    - `visa.txt` 已明确给出 post-update 规则：`Sn = (Sn < VL_t) ? 0 : (Sn - VL_t)`
    - 因此公开 docs 可以把 `%scalar_out` 明确写成同一次 `plt` 调用返回的 post-update scalar 值，而不是仅保守描述为“可链式传递”

- `pge`
  - installed Clang header：
    - `pge_b8/pge_b16/pge_b32` 直接以 `clang_builtin_alias(__builtin_cce_pge_*)` 暴露
    - helper wrapper 走 `__builtin_cce_pge_b*(pattern, 0)` 形式
  - `strings bisheng`：
    - `llvm.hivm.pge.b8/.b16/.b32`
    - `llvm.hivm.pge.b8/.b16/.b32.v210`
  - 当前 PTO surface：
    - `pto.pge_b8`
    - `pto.pge_b16`
    - `pto.pge_b32`
  - repo-generated `.ll` 旁证：
    - `micro-op/materialization-predicate/pge-tail-mask` 当前发射到
      `llvm.hivm.pge.b8` / `llvm.hivm.pge.b16` / `llvm.hivm.pge.b32`
  - 收敛结论：
    - `pge` 不应再放在“仓库已有但缺直接证据”的集合里
    - 公开 docs 可以把它作为已确认 family 记录，只需继续避免过度推断 pattern 的底层编码

- `pand`
  - `visa.txt` 证据：
    - `tp = pand(tsupm, tp, sup_en);`
  - 当前 PTO surface：
    - `pto.pand %src0, %src1, %mask`
  - 当前 docs：
    - 已收紧为 governing predicate 口径
  - 当前实现：
    - `include/PTO/IR/VPTOOps.td` 已定义
    - `lib/PTO/IR/VPTO.cpp` 走 `verifyBinaryMaskOp`
    - `lib/PTO/Transforms/VPTOLLVMEmitter.cpp` 已接线
  - 当前补证：
    - installed Clang header `__VF_PREDICATE_Z(pand)` wrapper 为
      `pand(dst, src0, src1, mask) -> __builtin_cce_pand_z(src0, src1, mask)`
    - `strings bisheng` 确认存在 `llvm.hivm.pand.z`
  - 收敛结论：
    - 第三个参数应统一表述为 governing / enable predicate，而不是笼统 predication mask
    - inactive lanes 采用 zeroing 形式的叙述应与 `_z` builtin 保持一致

- `por`
  - `visa.txt` 证据：
    - `supm = por(supm, tp, sup_en);`
  - 当前 PTO surface：
    - `pto.por %src0, %src1, %mask`
  - 当前 docs：
    - 已收紧为 governing predicate 口径
  - 当前实现：
    - `include/PTO/IR/VPTOOps.td` 已定义
    - `lib/PTO/IR/VPTO.cpp` 走 `verifyBinaryMaskOp`
    - `lib/PTO/Transforms/VPTOLLVMEmitter.cpp` 已接线
  - 当前补证：
    - installed Clang header `__VF_PREDICATE_Z(por)` wrapper 为
      `por(dst, src0, src1, mask) -> __builtin_cce_por_z(src0, src1, mask)`
    - `strings bisheng` 确认存在 `llvm.hivm.por.z`
  - 收敛结论：
    - `por` / `pand` / `pxor` 应统一为同一 governing-predicate + zeroing 叙述

- `pnot` / `pxor` / `psel`
  - `visa.txt` 证据：
    - `PSEL Pd, Pn, Pm, Pg`
    - `PXOR Pd, Pn, Pm, Pg`
    - `PNOT Pd, Pn, Pg`
  - installed Clang wrapper：
    - `pnot(dst, src, mask) -> __builtin_cce_pnot_z(src, mask)`
    - `psel(dst, src0, src1, mask) -> __builtin_cce_psel(src0, src1, mask)`
    - `pxor` 走 `__VF_PREDICATE_Z(pxor)` 宏族，属于带 governing predicate 的 zeroing 形式
  - `strings bisheng`：
    - `llvm.hivm.pnot.z`
    - `llvm.hivm.pxor.z`
    - `llvm.hivm.psel`
  - 当前 PTO surface：
    - `pto.pnot %src, %mask`
    - `pto.pxor %src0, %src1, %mask`
    - `pto.psel %src0, %src1, %sel`
  - repo-generated `.ll` 旁证：
    - `micro-op/materialization-predicate/pnot` 当前发射到 `llvm.hivm.pnot.z`
    - `micro-op/materialization-predicate/pxor` 当前发射到 `llvm.hivm.pxor.z`
    - `micro-op/materialization-predicate/psel` 当前发射到 `llvm.hivm.psel`
  - 结论：
    - 当前 docs 可收紧为 governing/enable predicate 语义
    - `pnot` / `pxor` 的 inactive lanes 采用 zeroing 叙述

- `ppack` / `punpack`
  - `visa.txt` 证据：
    - `PPACK Pd, Pn, #part`
    - `PUNPACK Pd, Pn, #part`
    - `#part = #l/#h`
  - installed Clang wrapper：
    - `ppack(dst, src, part) -> __builtin_cce_ppack_z(src, part)`
    - `punpack(dst, src, part) -> __builtin_cce_punpack(src, part)`
  - `strings bisheng`：
    - `llvm.hivm.ppack.z`
    - `llvm.hivm.punpack`
  - 当前 PTO surface：
    - `pto.ppack %input, "PART"`
    - `pto.punpack %input, "PART"`
  - repo-generated `.ll` 旁证：
    - `micro-op/materialization-predicate/ppack-punpack` 当前发射到
      `llvm.hivm.ppack.z` / `llvm.hivm.punpack`
  - 结论：
    - `LOWER/HIGHER` 语义已具备直接依据，可同步到公开 docs

- `pintlv` / `pdintlv`
  - `visa.txt` 证据：
    - `PINTLV.type Pd, Pn, Pm`
    - `PDINTLV.type Pd, Pn, Pm`
    - hardware type family: `b8`, `b16`, `b32`
  - installed Clang wrapper：
    - 存在 `__builtin_cce_pintlv_*` / `__builtin_cce_pdintlv_*`
  - `strings bisheng`：
    - `llvm.hivm.pintlv.b8/b16/b32`
    - `llvm.hivm.pdintlv.b8/b16/b32`
  - 当前 PTO surface：
    - `pto.pdintlv_b8`
    - `pto.pdintlv_b16`
    - `pto.pdintlv_b32`
    - `pto.pintlv_b8`
    - `pto.pintlv_b16`
    - `pto.pintlv_b32`
  - repo-generated `.ll` 旁证：
    - `micro-op/materialization-predicate/pdintlv_b8` 当前发射到 `llvm.hivm.pdintlv.b8`
    - `micro-op/materialization-predicate/pdintlv_b16` 当前发射到 `llvm.hivm.pdintlv.b16`
    - `micro-op/materialization-predicate/pdintlv_b32` 当前发射到 `llvm.hivm.pdintlv.b32`
    - `micro-op/materialization-predicate/pintlv_b8` 当前发射到 `llvm.hivm.pintlv.b8`
    - `micro-op/materialization-predicate/pintlv_b16` 当前发射到 `llvm.hivm.pintlv.b16`
    - `micro-op/materialization-predicate/pintlv_b32` 当前发射到 `llvm.hivm.pintlv.b32`
  - 结论：
    - 当前 PTO surface 已与已确认的 hardware family 对齐到 `b8/b16/b32`
    - 公开 docs 应按完整 family 记录 `pdintlv_*` / `pintlv_*`

- `pldi` / `plds` / `psti` / `psts` / `pstu`
  - `visa.txt` 证据：
    - `PLDS`
    - `PLDI`
    - `PSTS`
    - `PSTI`
    - `PSTU`
  - 当前 PTO surface：
    - immediate-load：`pto.pldi %source[%offset], "DIST"`
    - dynamic-load：`pto.plds %source[%offset], "DIST"`
    - immediate-offset：`pto.psti %value, %dest[%offset], "DIST"`
    - dynamic-offset：`pto.psts %value, %dest[%offset], "DIST"`
    - unaligned-state-update：`pto.pstu %align_in, %value, %base`
  - 当前 docs：
    - `pldi/plds` 的 `DIST` 已写为 `NORM` / `US` / `DS`
    - `psti/psts` 的 `DIST` 已写为 `NORM` / `PK`
    - `%offset` 形式已统一成 `base[offset]`
  - 当前实现：
    - `include/PTO/IR/VPTOOps.td` 已定义
    - `lib/PTO/IR/VPTO.cpp` 已有 verifier
    - `lib/PTO/Transforms/VPTOLLVMEmitter.cpp` 已接线
  - 当前补证：
    - installed Clang header 中 `pldi/plds` 的合法 `DIST` 被约束为 `NORM/US/DS`
    - installed Clang header 中 `psti/psts` 的合法 `DIST` 被约束为 `NORM/PK`
    - installed Clang header 中 `pstu` 以 `pstu(vector_align&, vector_bool, __ubuf__ uint16_t*&|uint32_t*&)` wrapper 暴露
    - installed wrapper 仍同时存在 post-update overload，但本阶段不将 post 访存纳入支持计划；这些 overload 仅作为已安装实现旁证保留
    - `strings bisheng` 确认存在：
      - `llvm.hivm.pldi.b8`
      - `llvm.hivm.pldi.post.b8`
      - `llvm.hivm.plds.b8`
      - `llvm.hivm.plds.post.b8`
      - `llvm.hivm.psti.b8`
      - `llvm.hivm.psti.post.b8`
      - `llvm.hivm.psts.b8`
      - `llvm.hivm.psts.post.b8`
      - `llvm.hivm.pstu.b16`
      - `llvm.hivm.pstu.b32`
    - repo-generated `.ll` 旁证：
      - `micro-op/predicate-load-store/psti-pk-pldi-us` 当前发射到 `llvm.hivm.psti.b8` / `llvm.hivm.pldi.b8`
      - `micro-op/predicate-load-store/psts-pk-plds-us` 当前发射到 `llvm.hivm.psts.b8` / `llvm.hivm.plds.b8`
      - `micro-op/predicate-load-store/pstu` 当前发射到 `llvm.hivm.pstu.b32`
      - 当前 repo 路径未误落到 `.post.*` intrinsic
  - 收敛结论：
    - `psti/psts` 对应 packed predicate payload，而不是 logical predicate image
    - `psti` 与 `psts`、`pldi` 与 `plds` 的 surface 差异只应保留在 offset 来源
    - `pstu` 的当前 surface 已可明确为 `b16<->ui16` 与 `b32<->ui32` 的 align/base state-update 形式
    - `pst/pld` 这类依赖隐式寻址或状态推进的形式，已明确排除出当前支持范围，不进入 PTO surface
    - `pldi/plds/psti/psts` 的 post overload 在本阶段不进入支持面；只有在 LLVM 侧仅保留 post 形式时，才允许作为实现性让步单独评估

## 2. 当前仍需继续补证的相关 op

当前本文件内已登记的 predicate family 都已具备 installed impl / `strings bisheng` 旁证；其中 `pset`、`plt`、`pge`、`pand/por/pxor/pnot/psel`、`ppack/punpack`、`pintlv/pdintlv`、`pldi/plds/psti/psts/pstu` 已进一步拿到 repo-generated `.ll` 旁证。

后续若继续补证，重点不再是“有没有 builtin / intrinsic 名称”，而是：
- 某些 op 的数值或状态递推语义是否还需要更细粒度的 installed impl 旁证
- 是否需要把 repo-generated `.ll` 与 installed wrapper 做更细致的一一核对

## 3. 当前已确认的仓库侧观察

- predicate 相关 PTO op 大多已在 `include/PTO/IR/VPTOOps.td` 中定义
- 多数 op 在 `lib/PTO/IR/VPTO.cpp` 中已有 verifier
- 多数 op 在 `lib/PTO/Transforms/VPTOLLVMEmitter.cpp` 中已有 emitter
- `docs/isa/05-materialization-predicate.md` 当前仍需要继续和 `visa`/installed impl 对齐个别 token 与示例，但 `plt` 的 `%scalar_out` 递推公式已由 `visa.txt` 明确给出
- `docs/isa/04-predicate-load-store.md` 当前方向基本对，但还需要继续和 `visa`/installed impl 对齐 `DIST` 集合与 payload 语义
  - 其中 `pldi/plds/psti/psts/pstu` 的 surface 与 builtin / intrinsic 旁证已收敛
  - 后续若继续深挖，重点是更细粒度的状态递推或 runtime 语义，而不是 surface 缺口

## 4. 建议的执行顺序

- 第一步：收紧 `pset_b*`
  - 已收敛到已确认 token 集合
  - 已建立 `PAT_ALL <-> #all_true` 的映射说明

- 第二步：收紧 `plt_b*`
  - 已确认 `%scalar_out` 属于 installed V300 post-update wrapper 对应的正式 surface
  - 后续仅剩“是否需要进一步固化数值递推公式”的问题

- 第三步：收紧 `pand` / `por`
  - 已统一为 governing / enable predicate 叙述
  - `pxor` / `pnot` / `psel` 已跟随同一语义口径

- 第四步：收紧 `psti/psts/pldi/plds`
  - `DIST` 已按 installed impl 收敛到 `NORM/US/DS` 与 `NORM/PK`
  - packed predicate payload 语义已补齐到公开 docs
  - 与 `pld/pst` 的边界已明确：当前只支持显式 `base[offset]` 版本，不支持隐式寻址/状态推进形式

- 第五步：再看其他 predicate op
  - `pge`
  - `pnot`
  - `psel`
  - `ppack/punpack`
  - `pdintlv_*`
  - `pintlv_*`

## 5. 当前不建议做的事

- 不建议仅凭 repo 现有定义继续扩 predicate op 文档
- 不建议把 `visa.txt` 未出现的 op 直接标记为“语义已确定”
- 不建议在没有 installed impl / Bisheng 佐证前重写 `ppack/punpack/pdintlv/pintlv` 的语义
- 不建议把依赖隐式寻址或隐式状态推进的 predicate/load-store 形式纳入当前支持面
