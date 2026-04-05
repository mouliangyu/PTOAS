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

- 该漂移已收口
- `docs/isa/13-dsa-sfu-ops.md` 与 `docs/vpto-spec.md` 现统一使用 `pto.vmrgsort4`

影响：

- 命名漂移已消除

待确认问题：

- 无

结论：

- 正式 surface 收口为 `pto.vmrgsort4`

落地动作：

- 已完成：统一 `docs/isa/13-dsa-sfu-ops.md` 与 `docs/vpto-spec.md`

### 2. Vec-Scalar 汇总表未同步正文 surface

现状：

- `docs/vpto-spec.md` 的 vec-scalar summary 已与 `docs/isa/08-vec-scalar-ops.md` 对齐

影响：

- 此项漂移已消除

待确认问题：

- 无

结论：

- group summary 应反映当前正文 surface，当前已按该口径收口

落地动作：

- 已完成：同步 `docs/vpto-spec.md` 汇总表

### 3. Reduction 汇总表未同步正文 surface

现状：

- `docs/vpto-spec.md` 的 reduction summary 已同步 `vcg*` 与 `vcpadd`

影响：

- 此项漂移已消除

待确认问题：

- 无

结论：

- reduction summary 应反映当前正文 surface，当前已按该口径收口

落地动作：

- 已完成：同步 `docs/vpto-spec.md` 汇总表

### 4. DSA/SFU 汇总表未同步 `vsort32`

现状：

- 该漂移已收口
- `docs/isa/13-dsa-sfu-ops.md` 已移除 `pto.vsort32`
- `docs/vpto-spec.md` 不再需要为 `pto.vsort32` 保留汇总项

影响：

- 此项漂移已消除

待确认问题：

- 无

结论：

- `pto.vsort32` 不属于当前文档化 VPTO surface，因此不保留在用户文档中

落地动作：

- 已完成：从 `docs/isa/13-dsa-sfu-ops.md` 移除 `pto.vsort32`

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
