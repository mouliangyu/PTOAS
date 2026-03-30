# VPTO 文档漂移核对单

## Purpose

本文件单独记录当前已确认的 VPTO 文档漂移项，供后续评审时逐项敲定。

适用范围：

- `docs/vpto-spec.md`
- `docs/isa/`
- 与之对应的 VPTO 测试范围文档

说明：

- 本文件只记录“已观察到的漂移”和“待确认问题”
- 不在这里直接替代正式 spec 结论
- 每条都预留 `结论` 与 `落地动作`，待评审后填写

## Drift Items

### 1. `vmrgsort` vs `vmrgsort4` 命名不一致

现状：

- `docs/isa/13-dsa-sfu-ops.md` 在排序章节使用短名 `pto.vmrgsort`
- 同页 syntax / implementation summary 仍写的是 `pto.vmrgsort4`
- `docs/vpto-spec.md` 当前也记录为 `pto.vmrgsort4`
- 当前 scope / matrix 暂不纳入该条，命名争议由本 drift 文档单独跟踪

影响：

- scope / matrix / 用例命名无法稳定对齐
- 评审时难以判断“文档别名”还是“真实 surface 重命名”

待确认问题：

- 对外正式 surface 名称应为 `pto.vmrgsort` 还是 `pto.vmrgsort4`
- 若保留一个、兼容一个，哪个是主名，哪个是别名

结论：

- 待填写

落地动作：

- 待填写

### 2. Vec-Scalar 汇总表未同步正文 surface

现状：

- `docs/isa/08-vec-scalar-ops.md` 正文已包含：
  - `pto.vadds`
  - `pto.vsubs`
  - `pto.vmuls`
  - `pto.vmaxs`
  - `pto.vmins`
  - `pto.vands`
  - `pto.vors`
  - `pto.vxors`
  - `pto.vshls`
  - `pto.vshrs`
  - `pto.vlrelu`
  - `pto.vaddcs`
  - `pto.vsubcs`
- 但 `docs/vpto-spec.md` 的 group summary 仍是旧计数和旧列表，没有反映 `vsubs/vands/vors/vxors`

影响：

- 汇总表不能作为 scope 统计依据
- matrix 记账时会出现“正文有、summary 无”的歧义

待确认问题：

- group summary 是否应完整反映 `docs/isa/08` 当前正文 surface
- 计数应如何更新

结论：

- 待填写

落地动作：

- 待填写

### 3. Reduction 汇总表未同步正文 surface

现状：

- `docs/isa/10-reduction-ops.md` 正文已包含：
  - `pto.vcadd`
  - `pto.vcmax`
  - `pto.vcmin`
  - `pto.vcgadd`
  - `pto.vcgmax`
  - `pto.vcgmin`
  - `pto.vcpadd`
- 但 `docs/vpto-spec.md` 的 group summary 仍只列 `pto.vcadd`、`pto.vcmax`、`pto.vcmin`

影响：

- reduction family 的汇总口径与测试范围不一致
- `vcg*` / `vcpadd` 容易被误判为“未正式进入 surface”

待确认问题：

- `docs/vpto-spec.md` 的 reduction summary 是否应扩成完整 current surface
- 若只保留“代表项”，是否需要额外注明 summary 不是完整 surface

结论：

- 待填写

落地动作：

- 待填写

### 4. DSA/SFU 汇总表未同步 `vsort32`

现状：

- `docs/isa/13-dsa-sfu-ops.md` 正文已有 `pto.vsort32`
- 但 `docs/vpto-spec.md` 的 DSA/SFU summary 仍未纳入 `pto.vsort32`
- 当前 scope / matrix 暂不纳入 `vsort32`，相关结论由本 drift 文档单独跟踪

影响：

- 会出现“ISA 文档有该 op，但总表没有”的口径不一致
- 影响是否将其记为正式 in-scope surface

待确认问题：

- `pto.vsort32` 是否已经是正式 A5 surface
- 若是，summary 是否应纳入
- 若不是，`docs/isa/13-dsa-sfu-ops.md` 是否应降级表述

结论：

- 待填写

落地动作：

- 待填写

### 5. `VPTOOps.td` 中存在文档未完整收录的 VPTO op

现状：

- `include/PTO/IR/VPTOOps.td` 当前定义了以下在文档标题层未完整收录的 op：
  - `pto.vecscope`
  - `pto.strict_vecscope`
  - `pto.uvld`
  - `pto.vlds_post`
  - `pto.vsts_post`
- 其中：
  - `pto.vecscope` / `pto.strict_vecscope` 已在 `docs/vpto-spec.md` 正文 prose 和示例中出现，但没有独立 `###` op 条目
  - `pto.uvld` / `pto.vlds_post` / `pto.vsts_post` 当前在 `docs/vpto-spec.md` 与 `docs/isa/` 中都未见独立收录
- 当前 scope / matrix 暂不纳入这几条，待文档与 surface 结论敲定后再决定是否进入测试台账

影响：

- 仅靠文档标题或 summary 无法得出完整 surface 集合
- 评审时难以判断这些 op 是正式 surface、内部过渡 op，还是仅保留给特定 lowering 路径
- 后续补测试时，容易在“TD 已有定义”与“文档未声明”之间出现误判

待确认问题：

- `pto.vecscope` / `pto.strict_vecscope` 是否应在 `docs/vpto-spec.md` 中补齐为正式独立 op 条目
- `pto.uvld` / `pto.vlds_post` / `pto.vsts_post` 是否属于对外正式 VPTO surface
- 若属于内部 lowering / 过渡 op，是否应在 TD、文档或命名上显式标注其定位，避免被当作常规测试对象

结论：

- 待填写

落地动作：

- 待填写

## Review Notes

补充说明：

- 本轮测试范围文档已经按“正文 surface + 当前实现交集”收敛，不再直接信任旧 summary
- 后续如果结论要求改 summary、改正文、或调整 scope / matrix，应以本文件结论区为准

评审记录：

- 参与人：
  - 待填写
- 日期：
  - 待填写
- 最终决定摘要：
  - 待填写
