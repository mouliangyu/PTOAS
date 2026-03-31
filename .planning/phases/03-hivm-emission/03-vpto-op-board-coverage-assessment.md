# VPTO 微指令测试集覆盖评估

更新时间：2026-03-31

目的：

- 固化当前 `matrix` 的 family 级覆盖统计，避免后续讨论被上下文稀释。
- 用统一口径识别测试集在 family 粒度上的覆盖缺口，并为后续补充 case 提供数量基线。
- 指导 `03-vpto-op-board-test-scope.md` 继续细化测试范围，而不是让统计结论停留在独立汇总层。

## Document Contract

本文件负责：

- 基于 `matrix` 的当前状态，给出 family 级覆盖统计
- 判断当前测试集在 family 粒度上是否存在覆盖缺口
- 识别 `scope` 中仍需细化、补齐或拆分的 family
- 为后续补 case 提供数量缺口和细化方向

本文件不直接替代 `scope`。职责分工如下：

- `coverage assessment`
  - 回答“当前哪些 family 仍有缺口、这些缺口是什么、scope 还需要细化什么”
- `scope`
  - 回答“该 family 具体应该补哪些 case、每个 case 的测试目标是什么”
- `matrix`
  - 回答“这些 case 目前做到哪一步了”

迭代顺序固定为：

1. 先更新本文件，识别 family 级缺口与细化方向
2. 再把需要新增或细化的内容回写到 `03-vpto-op-board-test-scope.md`
3. 最后才在 `03-vpto-op-board-unit-tests-matrix.md` 中登记和推进状态

若本文件指出某个 family 仍有覆盖缺口，但 `scope` 中尚未把该 family 的 case 清单或 case 目标细化到可执行程度，
则应优先修改 `scope`，而不是直接在 `matrix` 中补状态。

## Scope Refinement Rules

本文件中的评估结论应这样作用到 `scope`：

- 若某个 family 的 `当前计划覆盖op数 < 总op数`
  - `scope` 需要先补齐该 family 的 case inventory，确保所有 in-scope op 都有承接 case
- 若某个 family 的 `建议case数 > 当前case数`
  - `scope` 需要补齐缺失的代表性 case，并同步推动 `matrix` 补齐对应条目
- 若某个 family 的 `已测op数` 很低，且 `Case Inventory` 只有粗粒度描述
  - `scope` 需要把该 family 的 case 目标写细，避免 `matrix` 中只存在条目而没有可执行目标
- 若某个 family 的 `建议case数` 明显高于 `总op数`
  - `scope` 需要解释额外 case 对应的是哪些场景维度，不允许只给数量不给理由
- 若某个 family 尚不能同时回答以下三个问题
  - 每条 in-scope op 由哪个 case 承接
  - 每个 case 的测试目标是什么
  - 每个 case 覆盖哪些维度
  - 则 `scope` 中该 family 仍需继续细化，不能视为稳定

## Completion Gates For Scope

assessment 不使用“明显偏少”“接近合理”“基本成形”这类软判断来允许停止细化。
对每个 family，只要满足以下任一条件，就必须继续回写 `scope`：

- `当前计划覆盖op数 < 总op数`
- `建议case数 > 当前case数`
- `Family Coverage Minimums` 的任一 `mandatory` 维度尚未被明确映射到具体 case
- `Case Inventory` 中仍存在“只有 case 名、没有明确测试目标或覆盖维度”的条目
- 存在成组 case，但尚未说明该组 case 如何覆盖组内每条 op 的核心语义

只有当以上条件全部不成立时，才可把该 family 视为“scope 已细化到可直接执行”。

补充说明：

- 当 `scope-matrix缺口 = 0` 时，本文件后续主要反映的是“执行覆盖率”问题，而不是 `scope` 细化不足。
- 在这种状态下，大量 `planned` 表示 case 已进入台账，但尚未推进到“已补充测例”状态；它不再自动意味着 `scope` 不完整。

统计口径：

- `总op数`：来自 `03-vpto-op-board-unit-tests-matrix.md` 的 `Op Summary`
- `当前计划覆盖op数`：在 `Case Matrix` 中至少被一条 case 行登记到的 op 数，不要求该 case 已执行
- `已测op数`：在 `Case Matrix` 中已有非 `planned` 结论的 op 数；`implemented` / `blocked` / `board-passed` 均计入
- `当前case数`：当前 `Case Matrix` 中登记的 case 行数
- `scope细化case数`：`03-vpto-op-board-test-scope.md` 中当前已经细化到 `Case Inventory` 的 case 数
- `scope-matrix缺口`：`scope细化case数 - 当前case数`
- `建议case数`：首轮“合理覆盖”目标，不追求全类型 x 全场景笛卡尔展开，但要求各 family 不只是骨架占位；当前以 `scope细化case数` 为最新基线

## Family Assessment

- `vector-load-store`
  - `总op数`：16
  - `当前计划覆盖op数`：16
  - `已测op数`：0
  - `op覆盖率`：0.0%
  - `当前case数`：24
  - `scope细化case数`：24
  - `scope-matrix缺口`：0
  - `建议case数`：24
  - `评估`：`scope` 与 `matrix` 已对齐；当前问题不是 case 台账缺失，而是已测进度仍为 0，主路径与扩展项均未进入非 `planned` 状态

- `predicate-load-store`
  - `总op数`：7
  - `当前计划覆盖op数`：7
  - `已测op数`：0
  - `op覆盖率`：0.0%
  - `当前case数`：6
  - `scope细化case数`：6
  - `scope-matrix缺口`：0
  - `建议case数`：6
  - `评估`：`scope` 与 `matrix` 已对齐；当前问题是已测进度仍为 0，round-trip 与边界项都还未推进

- `materialization-predicate`
  - `总op数`：20
  - `当前计划覆盖op数`：20
  - `已测op数`：2
  - `op覆盖率`：10.0%
  - `当前case数`：22
  - `scope细化case数`：22
  - `scope-matrix缺口`：0
  - `建议case数`：22
  - `评估`：`scope` 与 `matrix` 已对齐；当前问题是 22 条里仅 3 条进入非 `planned`，大部分 pattern / tail-mask / transform 扩展项仍未推进

- `unary-vector`
  - `总op数`：12
  - `当前计划覆盖op数`：12
  - `已测op数`：7
  - `op覆盖率`：58.3%
  - `当前case数`：30
  - `scope细化case数`：30
  - `scope-matrix缺口`：0
  - `建议case数`：30
  - `评估`：`scope` 与 `matrix` 已对齐；当前已测进度中等，但定义域边界、特殊值和 `vmov-tail` 仍停留在 `planned`

- `binary-vector`
  - `总op数`：13
  - `当前计划覆盖op数`：13
  - `已测op数`：6
  - `op覆盖率`：46.2%
  - `当前case数`：41
  - `scope细化case数`：41
  - `scope-matrix缺口`：0
  - `建议case数`：41
  - `评估`：`scope` 与 `matrix` 已对齐；当前已测进度不到一半，新增的 tail、bitwise 边界和 carry/borrow 边界项仍未推进

- `vec-scalar`
  - `总op数`：12
  - `当前计划覆盖op数`：12
  - `已测op数`：5
  - `op覆盖率`：41.7%
  - `当前case数`：31
  - `scope细化case数`：31
  - `scope-matrix缺口`：0
  - `建议case数`：31
  - `评估`：`scope` 与 `matrix` 已对齐；当前已测进度不到一半，新增的 scalar tail、bitwise 边界和 carry/borrow 边界项仍未推进

- `compare-select`
  - `总op数`：4
  - `当前计划覆盖op数`：4
  - `已测op数`：3
  - `op覆盖率`：75.0%
  - `当前case数`：18
  - `scope细化case数`：18
  - `scope-matrix缺口`：0
  - `建议case数`：18
  - `评估`：`scope` 与 `matrix` 已对齐；当前主线较完整，但 unordered compare 和 predicate-edge 选择语义仍未推进

- `conversion`
  - `总op数`：2
  - `当前计划覆盖op数`：2
  - `已测op数`：2
  - `op覆盖率`：100.0%
  - `当前case数`：10
  - `scope细化case数`：10
  - `scope-matrix缺口`：0
  - `建议case数`：10
  - `评估`：`scope` 与 `matrix` 已对齐；当前 case 台账完整，但 widening special、rounding boundary 和 tail+special 组合场景仍未推进

- `reduction`
  - `总op数`：7
  - `当前计划覆盖op数`：7
  - `已测op数`：3
  - `op覆盖率`：42.9%
  - `当前case数`：12
  - `scope细化case数`：12
  - `scope-matrix缺口`：0
  - `建议case数`：12
  - `评估`：`scope` 与 `matrix` 已对齐；当前已测进度仍不足一半，group reduction 的 tail/tie 和 prefix tail 场景尚未推进

- `rearrangement`
  - `总op数`：10
  - `当前计划覆盖op数`：10
  - `已测op数`：0
  - `op覆盖率`：0.0%
  - `当前case数`：14
  - `scope细化case数`：14
  - `scope-matrix缺口`：0
  - `建议case数`：14
  - `评估`：`scope` 与 `matrix` 已对齐；当前问题是已测进度仍为 0，所有顺序语义与边界项都还未推进

- `gather-scatter`
  - `总op数`：4
  - `当前计划覆盖op数`：4
  - `已测op数`：0
  - `op覆盖率`：0.0%
  - `当前case数`：8
  - `scope细化case数`：8
  - `scope-matrix缺口`：0
  - `建议case数`：8
  - `评估`：`scope` 与 `matrix` 已对齐；当前问题是已测进度仍为 0，所有索引形态扩展项都还未推进

- `dsa-sfu`
  - `总op数`：13
  - `当前计划覆盖op数`：13
  - `已测op数`：2
  - `op覆盖率`：15.4%
  - `当前case数`：20
  - `scope细化case数`：20
  - `scope-matrix缺口`：0
  - `建议case数`：20
  - `评估`：`scope` 与 `matrix` 已对齐；当前问题是 20 条里仅 3 条进入非 `planned`，特殊值、tail、边界累加和多配置 transpose 场景仍未推进

- `TOTAL`
  - `总op数`：120
  - `当前计划覆盖op数`：120
  - `已测op数`：30
  - `op覆盖率`：25.0%
  - `当前case数`：236
  - `scope细化case数`：236
  - `scope-matrix缺口`：0
  - `建议case数`：236
  - `评估`：`scope` 与 `matrix` 已完全对齐；当前 coverage 的主要问题已收敛为“236 条 case 中仅 60 条进入非 planned”，即执行和验证进度明显落后于台账规模

结论：

- 当前 `scope` 已细化到 `236` 条 case，已达到并超过之前用于校验合理性的 `230+` 量级。
- 当前 `scope` 与 `matrix` 已对齐，`scope-matrix缺口` 已清零。
- 因此，当前结论应表述为：
  - `scope` 的细化覆盖率已经基本合理
  - `matrix` 的登记覆盖率已与 `scope` 对齐，但执行覆盖率仍明显不足
  - 下一轮工作重点应是推动 `planned -> implemented/blocked/board-passed`，而不是继续扩写新的 family 目标

后续使用方式：

- 讨论“测试集总量是否合理”时，以本表为基线。
- 讨论“某类应该补多少 case”时，以 `建议case数 - 当前case数` 作为粗粒度缺口。
- 具体新增哪些 case，仍以 `03-vpto-op-board-test-scope.md` 和 `03-vpto-op-board-unit-tests-matrix.md` 的 case inventory 为准。
