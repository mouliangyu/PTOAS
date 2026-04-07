# VPTO Op Board Unit Tests Plan

## Summary

本轮目标是在现有 `test/vpto` 框架上，为文档化的 VPTO surface 构建可上板验证的单元测例集合。

本轮首先追求“覆盖率完整”，不是“先把少量 case 跑通”。执行原则如下：

- 先把 `matrix` 中的全部 case 推进到“已补充测例”的状态
- 再集中处理这些 case 的实现缺陷、lowering 缺陷、板测失败和 blocker
- 不因为某条 case 暂时跑不过，就中断同 family 或其他 family 的 case 补充工作
- parser 失败、verifier 失败、lowering 失败、代码生成失败、本地构建失败、板测失败，均视为有效暴露问题的测试结论
- 只要 case 已经被静态补充到仓库并能明确指向一个失败点或阻塞点，就不应把它当作“未完成”
- `blocked` 的含义需要严格收紧：它只表示“当前无法仅依据 `docs/vpto-spec.md` 与 `docs/isa/` 写出一个语义明确、输入输出可定义的 case”
- 因此，能依据文档把 case 写出来、但在 parser / verifier / lowering / codegen / runtime / board 阶段失败的条目，不应记为 `blocked`，而应记为 `implemented` 并在 `notes` 中记录失败点

因此，本轮的阶段目标应区分为两层：

- 第一层：覆盖率推进
  - 要求 `matrix` 中所有 case 都有对应的静态测例或明确 blocker 结论
- 第二层：通过率推进
  - 在覆盖率完整的基础上，再逐步把 `planned/blocked/implemented/compiled` 收敛到更多 `board-passed`

停止条件收紧如下：

- 覆盖率推进阶段的默认执行方式是“做完为止”，不是“做一批再停”
- 只要 `matrix` 中仍存在 `planned` 条目，实施就不应因为阶段性同步、局部进展汇报或单个 family 暂未跑通而停止
- 覆盖率推进阶段允许停止的唯一原因，是遇到必须和需求方确认的语义分歧、文档冲突或会改变测试范围定义的决策点
- parser / verifier / lowering / codegen / runtime / board failure 不属于允许停下的理由；若 case 依据文档可写，则这些失败应沉淀为 `implemented` 结论并继续推进剩余 case
- 因此，在覆盖率阶段内，进度同步只用于报告当前消灭了多少 `planned`，不构成暂停实施的节点

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
- `03-vpto-op-board-coverage-assessment.md`
- `03-vpto-op-board-unit-tests-PLAN.md`
- `03-vpto-op-board-unit-tests-matrix.md`
- `03-vpto-compiled-case-progression-PLAN.md`
- `03-vpto-compiled-case-progression-checklist.md`
- `03-vpto-runtime-issues-log.md`

维护关系约定：

- `scope` 定义“测什么”，并细化到 in-scope op、family minimum、case inventory、case 目标和 `scenarios` 命名口径
- `coverage assessment` 负责统计当前 coverage 缺口，并明确 `scope` 还需要继续细化哪些 family 与 case
- `matrix` 严格按 `scope` 已显式列出的 case 与覆盖标签登记当前状态，不额外发明 `scope` 外的 case
- `PLAN` 只维护执行组织、目录约束、文档协作顺序和状态流转，不单独扩写 `scope` 之外的 case
- 当工作重心从“补 case”切换为“把 `compiled` case 推进到 `board-passed`、`board-blocked` 或阻塞已明确”时，专项执行规则由 `03-vpto-compiled-case-progression-PLAN.md` 驱动，逐条推进台账由 `03-vpto-compiled-case-progression-checklist.md` 驱动
- 新增或调整测试范围时，先更新 `coverage assessment`，再回写 `scope`，最后才更新 `matrix`
- 进入测例构造、上板验证和回归阶段后，`scope` 不是绝对只读；只要 `coverage assessment` 仍显示 case 缺口或 case 目标缺失，就必须继续细化 `scope`
- 运行阶段若遇到已知环境、脚本接线、SIM/NPU链路或 oracle 漂移问题，先检索 `03-vpto-runtime-issues-log.md`，避免重复定位

文档职责边界：

- `03-vpto-op-board-test-scope.md`
  - 负责细化测试范围
  - 回答“哪条 op 由哪些 case 覆盖、每个 case 的目标是什么”
- `03-vpto-op-board-coverage-assessment.md`
  - 负责 family 级统计与 coverage 缺口识别
  - 回答“当前 coverage 还缺什么、这些缺口是否已经反映到 scope”
- `03-vpto-op-board-unit-tests-matrix.md`
  - 负责 case 状态台账
  - 回答“这些已定义 case 目前做到哪一步”
- `03-vpto-op-board-unit-tests-PLAN.md`
  - 负责文档协作顺序与执行流程
  - 回答“先更新哪份文档、再推进哪一步”
- `03-vpto-runtime-issues-log.md`
  - 负责沉淀已复现且已收敛的运行阶段问题
  - 回答“遇到相同运行现象时应先检查什么、已有结论是什么”
- `03-vpto-compiled-case-progression-PLAN.md`
  - 负责本轮 `compiled` case 从 `compiled` 推进到 `board-passed / board-blocked / blocked` 的专项执行规则
  - 回答“如何避免阶段性停下、每条 case 如何推进到运行校验无误或阻塞已明确”
- `03-vpto-compiled-case-progression-checklist.md`
  - 负责本轮全部 `compiled` case 的逐条执行台账
  - 回答“当前已经清扫到哪条、哪条还没看、哪条已经形成运行结论”

用户文档与内部台账的边界约束：

- `docs/vpto-spec.md` 与 `docs/isa/*.md` 是用户文档，只描述 PTO surface、用户可见语义、输入输出约束和使用方式
- 用户文档不暴露底层 instruction 名、installed wrapper 名、LLVM intrinsic 名、frontend trace、`visa.txt` 对齐过程等内部实现对齐信息
- `visa.txt`、installed PTO/Clang headers、Bisheng frontend trace、LLVM intrinsic spellings、wrapper ABI 这类证据，只记录在 `.planning/` 下的内部台账中
- 若为了收敛 docs 语义而做了底层对齐调研，结论应先沉淀到 `.planning/`，再把提炼后的用户可见语义回写到 `docs/`
- 发现用户文档中混入上述底层实现信息时，应优先清理回 `.planning/`，而不是继续在用户文档中追加更多底层细节

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
  case 已实现，包含“静态 case 已补齐但编译/运行链路仍失败”的情形
- `compiled`
  case 已实现，且 compile-only 路径已稳定走到编译产物；尚未完成 runtime / board 验证
- `board-passed`
  case 已完成上板验证
- `blocked`
  当前无法仅依据 `docs/vpto-spec.md` 与 `docs/isa/` 写出语义明确的 case；阻塞点来自文档缺口、文档冲突或文档未给出足够 oracle 信息，而不是实现链路失败

补充约定：

- 本轮应优先消灭“只有 matrix 条目、但还没有对应静态测例”的空白状态
- 对覆盖率推进而言，`implemented`、`compiled`、`blocked`、`board-passed` 都属于“已补充测例”
- 只有 `planned` 才表示该 case 仍未被真正补充到仓库，且也尚未形成“文档层面无法写 case”的明确 blocker 结论
- `blocked` 不能用于表达“当前实现编不过/跑不过/还没接 lowering”；这些应保留为 `implemented`，并在 `notes` 中记录失败点
- 执行阶段不得因为主观判断“这条看起来写不出来”而自行跳过 case
- 只有开发者明确确认或提出某条 case / 某类语义应按 blocker 管理后，才允许把该 blocker 正式落到 `scope` / `matrix`
- 在获得上述确认前，case 仍按 `planned` 或 `implemented` 推进；不能先斩后奏地改成 `blocked`
- 一旦某条 case 已被开发者确认并登记为 `blocked`，后续错误分析、问题收敛和修复阶段都应跳过该条目
- 已登记的 `blocked` 条目只有在开发者主动取消 `blocked` 标记后，才重新进入分析和修复队列

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
- 强耦合成组 case 才允许使用组合名，例如 `vldas-vldus`、`vldsx2-vstsx2`
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
- 测例必须保持 `scope` / `matrix` 中声明的测试目标；不允许为了通过 parser / verifier / lowering 而把 case 改成无法验证原目标语义的形式
- 如果当前测例实现与测试目标不符合，必须继续修正到符合目标；不能把不符合目标的实现当作“已修复”
- 若当前 surface / lowering / emitter 还无法同时满足“语法合法”和“测试目标成立”，应将其记为 surface / implementation 问题，而不是弱化测例目标
- 用例骨架参考 `test/vpto/cases/tileop/abs/`，尤其是 GM↔UB 搬运方式与同步方式
- 前处理负责按 `tileop/abs` 的方式完成 GM→UB DMA 与必要同步
- 主体默认在 `pto.vecscope` region 内执行被测 VPTO op；但若某条 op 已在 docs / verifier / runtime issue 中明确标注为非 vecscope UB helper，则必须保持在 `pto.vecscope` 外
- 不再新写 dummy loop carrier；统一以 `pto.vecscope` 作为 vecscope 边界表达
- 向量 / mask / align 相关的任何被测语义都必须位于 `pto.vecscope` 内，不允许把相关计算放到 region 外；`pto.vbitsort` 这类非 vecscope UB helper 不适用该条，且禁止放入 `pto.vecscope`
- 所有测例统一采用 GM→UB→计算→UB→GM 的可观测路径
- 对需要 operand feed 的向量 / mask / align 测例，输入只能通过访问 UB 的指令加载；不允许绕过 UB 直接消费 GM 数据
- 对生成类 op，可在 `pto.vecscope` 内直接生成结果，但结果仍需先写回 UB，再由后处理导回 GM 做 host compare
- 后处理负责按 `tileop/abs` 的方式完成必要同步、UB→GM DMA 与 host compare
- `golden.py` 与 `compare.py` 必须验证语义，而不是只验证程序成功返回

强耦合 op 允许成组测试，例如：

- `vldas + vldus`
- `psts + plds`
- `psti + pldi`
- `vldx2 + vstsx2`
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

## Error Analysis Rules

错误分析与问题收敛阶段，必须遵守以下约定：

1. 分析对象只来自已静态登记的 case
   - 只分析 `matrix` 中已存在、且 `scope` 已定义目标的 case
   - 不允许绕过 `scope` / `matrix`，直接对临时 case 或未登记 case 下结论

2. `blocked` 条目默认不进入分析队列
   - 已被开发者确认并登记为 `blocked` 的 case，在开发者主动取消前，一律跳过错误分析、问题收敛和修复
   - 错误分析阶段只处理 `implemented`、`compiled` 或 `board-passed` 回退后的失败条目

3. 先检查 case 是否符合 `scope` 目标
   - 若 case 实现与 `scope` / `matrix` 声明的测试目标不一致，优先判为测例实现问题
   - 不允许为了消除 parser / verifier / lowering / codegen 错误而弱化 case 目标
   - 若当前实现无法同时满足“语法合法”和“目标成立”，应记录为 surface / implementation 问题，而不是把 case 改成无意义样例

4. 语义判断只以文档为准
   - 错误分析中的语义判断首先依据 `docs/vpto-spec.md` 与 `docs/isa/`
   - 若文档之间存在冲突或不足以给出稳定 oracle，应整理成待开发者确认结论
   - 未经开发者确认，不得自行把该 case 转成 `blocked`

5. 先判定失败层级，再决定处理方式
   - parser / verifier 失败：先检查 case 写法是否符合当前 ODS / surface
   - lowering / emitter / codegen 失败：在 case 目标不变前提下，记录为实现链路问题
   - runtime / board 失败：在 case 与 oracle 均成立前提下，记录为运行链路或后端问题
   - 不允许把实现链路失败误记成文档 blocker

6. 向量 case 的结构合法性必须优先检查
   - 向量 / mask / align 相关语义必须位于 `pto.vecscope` 内
   - 需要 operand feed 的向量 / mask / align 输入只能通过 UB 访问 op 获得
   - 结果必须遵循 `GM→UB→计算→UB→GM` 的可观测路径
   - 若 case 违反这些结构约束，应先修 case，再继续分析下游报错

7. 错误结论必须静态回写
   - case 已存在且失败时，状态保持为 `implemented`；若 compile-only 已稳定走到产物，可升级为 `compiled`
   - 失败点、已确认原因和当前判断应写入 `matrix.notes`
   - 只有开发者确认的文档层 blocker，才允许改成 `blocked`

8. 错误分析不改变覆盖率推进原则
   - 单个 case 或单个 family 的失败，不影响继续收敛其他非-`blocked` 条目
   - 只要 `planned` 未清零或仍有非-`blocked` 失败条目待收敛，就不因为中间汇报而停止

## Validation

执行顺序固定为：

1. 先更新 `coverage assessment`，重新统计 `matrix` 当前 coverage，并识别 `scope` 仍需细化的 family / case
2. 按 `coverage assessment` 回写 `scope`，把缺口落实成明确的 case inventory、case 目标和覆盖维度
3. 再按最新 `scope` 回填或修正 `matrix`
4. 进入测例构造阶段后，新增或修改 case 目录与用例内容
5. 每个 case 单独上板验证
6. 仅在 `matrix` 中推进 `planned/implemented/compiled/board-passed/blocked` 状态
7. 运行 `test/vpto` 全量板测回归
8. 重新更新 `coverage assessment`，检查最新 `matrix` 是否仍存在 case 缺口或 scope/matrix 漂移
9. 检查 `scope`、`coverage assessment`、`matrix` 与 case 目录、板测结果保持一致

实施策略补充：

1. 覆盖率推进阶段，不以“这条 case 是否已经板测通过”作为是否继续补例的前提
2. 若在补例过程中遇到 parser / verifier / lowering / codegen / runtime 问题，应记录到对应 case 的 `notes`；未形成稳定编译产物前保持为 `implemented`，compile-only 已稳定走到产物后可记为 `compiled`，而不是转成 `blocked`
3. 单个 family 中已有 case 失败，不影响继续补齐该 family 的剩余 case
4. 只有当 `matrix` 中所有 case 都已进入 `implemented`、`compiled`、`blocked` 或 `board-passed` 后，才进入“集中提通过率”的主阶段
5. 覆盖率阶段中的中间汇报不改变执行状态；若 `planned` 未清零，则默认继续实施，而不是等待下一轮再推进
6. 只有当 case 无法依据 `docs/vpto-spec.md` 与 `docs/isa/` 写出时，才允许转为 `blocked`
7. 若怀疑某 case 应转为 `blocked`，默认动作不是跳过，而是先整理成待确认结论；只有开发者确认或提出后，才能更新 `scope` / `matrix`
8. 对已登记为 `blocked` 的条目，默认不再做错误分析与修复推进；除非开发者主动取消 `blocked`

验收标准分两层：

- 覆盖率阶段验收：
  - 本轮 in-scope 的 VPTO op 都在 matrix 中有条目
  - `coverage assessment` 中识别出的 scope 缺口，必须已经在 `scope` 中落实为明确 case 或明确 blocker
  - 已补充 case 都能在 matrix 中找到映射
  - `matrix` 中不存在 `scope` 未显式列出的 case
  - 重新统计 coverage 时，不能跳过 `coverage assessment` 直接对 `scope` / `matrix` 下结论
  - 在覆盖率阶段结束时，`matrix` 中不应再存在 `planned` 条目
  - 无“既未覆盖、也未标注 blocker”的空白项
  - `blocked` 条目必须对应文档层面的不可写原因，不能用实现链路失败替代

- 整体阶段验收：
  - 在覆盖率阶段完成后，再继续推进通过率
  - `test/vpto` 全量板测通过，或未通过项已被显式转为 `blocked` 并写明原因

## Assumptions

- “每条 op”按文档化 VPTO surface 解释，但基础设施 sync/DMA 不单测
- `arith` / `scf` 不是独立被测对象
- 所有 GM↔UB 传输统一复用 DMA `copy*` 家族
- 文档不完整或实现不稳的 op 可以标记 `blocked`，但必须写明原因
