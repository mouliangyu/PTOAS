# VPTO Op Board Unit Tests Plan

## Summary

本轮目标是在现有 `test/vpto` 框架上，为文档化的 VPTO surface 构建可上板验证的单元测例集合。

本轮不单独立项测试以下基础设施 op：

- `set_flag` / `wait_flag`
- `get_buf` / `rls_buf`
- `pipe_barrier` / `mem_bar`
- `set_loop*`
- `copy_gm_to_ubuf` / `copy_ubuf_to_gm` / `copy_ubuf_to_ubuf`

这些 op 仍然会出现在测试中，但只作为其他 VPTO op 的准备动作或收尾动作复用。

## Scope

被测对象以 `docs/isa` 中文档化的 VPTO 指令面为准，且满足以下规则：

- 文档查询范围固定为 `docs/vpto-spec.md` 与 `docs/isa/`
- 测试计划、matrix 和具体 case 的语义判断都以该文档范围为第一信息源
- 如 `docs/vpto-spec.md` 与 `docs/isa/` 存在表述差异，需要先在计划或 matrix 的 `notes` 中记明，再决定是否落 case

- 共享 `arith` / `scf` 不作为独立 VPTO 指令建 case
- 需要从 GM 喂数或把结果导回 GM 的 case，统一通过 DMA `copy*` 家族完成 GM↔UB 搬运

本轮优先覆盖这些家族：

- Vector load/store
- Predicate materialization + predicate load/store
- Unary vector
- Binary vector
- Vec-scalar
- Compare/select
- Conversion / special
- Rearrangement
- Gather/scatter / complex memory

## Non-goals

本轮不做以下工作：

- 不为 Pipeline sync 建独立板测 case
- 不为 DMA copy/config 建独立板测 case
- 不追求“每条 op 全类型全分支”展开
- 不把仅作为包裹层出现的 `arith` / `scf` 记为独立覆盖目标

## Static Tracking

测试覆盖状态必须静态记录到仓库文件，不依赖聊天上下文、提交说明或个人笔记。

配套维护文件：

- `03-vpto-op-board-test-scope.md`
- `03-vpto-op-board-unit-tests-PLAN.md`
- `03-vpto-op-board-unit-tests-matrix.md`

维护关系约定：

- `scope` 定义“测什么”与 case/scenario 命名口径
- `matrix` 严格按 `scope` 已显式列出的 case 与覆盖标签登记
- `PLAN` 只维护执行组织、目录约束和状态流转，不单独扩写 `scope` 之外的 case
- 新增或调整测试范围时，先更新 `scope`
- 进入测例构造、上板验证和回归阶段后，`scope` 视为只读；整体进度只在 `matrix` 中推进

其中 matrix 作为覆盖台账，至少记录：

- `op`
- `family`
- `doc_source`
- `in_scope`
- `case`
- `scenarios`
- `status`
- `notes`

状态约定：

- `infra`
  仅作为基础设施复用，不单独立项测试
- `planned`
  已纳入本轮范围，但尚未落地 case
- `implemented`
  case 已实现，但尚未完成板测闭环
- `board-passed`
  case 已完成上板验证
- `blocked`
  当前存在明确阻塞，待后续补齐

## Case Design Rules

所有新增 case 使用 `test/vpto/cases/` 下的 leaf case 目录，leaf case 目录必须包含：

- `kernel.pto`
- `stub.cpp`
- `launch.cpp`
- `main.cpp`
- `golden.py`
- `compare.py`

命名约束：

- 单 op 主语义 case 的 case 名默认与 op 名保持一致，例如 `pto.vadd` 对应 case `vadd`
- 强耦合成组 case 才允许使用组合名，例如 `vldas-vldus`、`vldx2-vstx2`
- 不再使用与 op 名无关的泛化名称，避免 matrix、case 目录和被测语义之间出现额外映射层

目录组织约束：

- runner 支持多级目录发现，物理目录按“测试层级 + family”组织
- VPTO 微指令单-op case 统一放在 `test/vpto/cases/micro-op/<family>/<case>/`
- `CASE_NAME` 使用相对 `test/vpto/cases/` 的路径，例如 `micro-op/binary-vector/vadd`
- matrix 中的 `case` 字段记录 runner 可见的真实 case 路径，不再区分逻辑名和物理名
- 无 `v` 前缀的现有历史 case 统一归入 `test/vpto/cases/tileop/`
- `tileop/` 下的 case 表示 tile 级 op 或派生组合验证，不能直接计作向量单 op 覆盖闭环

统一设计约束：

- 采用“前处理 + 主体 + 后处理”三段式模板
- 用例骨架参考 `test/vpto/cases/tileop/abs/`，尤其是 GM↔UB 搬运方式与同步方式
- 前处理负责按 `tileop/abs` 的方式完成 GM→UB DMA 与必要同步
- 主体在 `pto.vecscope` region 内执行被测 VPTO op
- 不再新写 dummy loop carrier；统一以 `pto.vecscope` 作为 vecscope 边界表达
- 向量 / mask / align 相关的任何被测语义都必须位于 `pto.vecscope` 内，不允许把相关计算放到 region 外
- 所有测例统一采用 GM→UB→计算→UB→GM 的可观测路径
- 对需要 operand feed 的向量 / mask / align 测例，输入只能通过访问 UB 的指令加载；不允许绕过 UB 直接消费 GM 数据
- 对生成类 op，可在 `pto.vecscope` 内直接生成结果，但结果仍需先写回 UB，再由后处理导回 GM 做 host compare
- 后处理负责按 `tileop/abs` 的方式完成必要同步、UB→GM DMA 与 host compare
- `golden.py` 与 `compare.py` 必须验证语义，而不是只验证程序成功返回

强耦合 op 允许成组测试，例如：

- `vldas + vldus`
- `psts + plds`
- `pst/psti + pld/pldi`
- `vldx2 + vstx2`
- `vintlv* + vdintlv*`

纯算子原则上一条 op 一个主 case。

## Coverage Strategy

覆盖规则如下：

- 每条被测 op 至少在一个 case 中作为主语义出现
- 同类 family 的关键场景在整个测试集合里至少出现一次
- 不要求每条 op 自己独立覆盖所有 applicable 维度，但不能出现整类场景从未被任何 case 触达

重点场景包括：

- full-mask / tail-mask
- aligned / unaligned
- contiguous / non-contiguous access
- 代表性的 `dist`
- compare family 的关系分支
- reduction 的结果落位
- rearrangement 的顺序变化

## Validation

执行顺序固定为：

1. 若本轮要新增或调整测试范围，先更新 `scope`，明确 case 与 `scenarios`
2. 按 `scope` 回填 `matrix`
3. 进入测例构造阶段后，`scope` 视为只读，新增或修改 case 目录与用例内容
4. 每个 case 单独上板验证
5. 仅在 `matrix` 中推进 `planned/implemented/board-passed/blocked` 状态
6. 运行 `test/vpto` 全量板测回归
7. 检查 `scope`、`matrix` 与 case 目录、板测结果保持一致

验收标准：

- 本轮 in-scope 的 VPTO op 都在 matrix 中有条目
- 已实现 case 都能在 matrix 中找到映射
- `matrix` 中不存在 `scope` 未显式列出的 case
- `test/vpto` 全量板测通过
- 无“既未覆盖、也未标注 blocker”的空白项

## Assumptions

- “每条 op”按文档化 VPTO surface 解释，但基础设施 sync/DMA 不单测
- `arith` / `scf` 不是独立被测对象
- 所有 GM↔UB 传输统一复用 DMA `copy*` 家族
- 文档不完整或实现不稳的 op 可以标记 `blocked`，但必须写明原因
