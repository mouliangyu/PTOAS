# VPTO 微指令测试集覆盖评估

更新时间：2026-03-31

目的：

- 固化当前 `matrix` 的 family 级覆盖统计，避免后续讨论被上下文稀释。
- 区分“coverage 台账是否补齐”和“这些 case 是否已经上板跑通”。
- 在 `planned = 0` 之后，把工作重心从“补 case”切换为“清理 blocker / 提升通过率”。

## Document Contract

本文件负责：

- 基于 `03-vpto-op-board-unit-tests-matrix.md` 给出 family 级覆盖统计
- 判断当前测试集在 family 粒度上是否仍有 case 台账缺口
- 区分覆盖率推进是否完成，以及通过率推进还剩哪些工作

本文件不替代 `scope`。职责分工如下：

- `coverage assessment`
  - 回答“当前 coverage 台账是否补齐、执行状态分布如何、下一步该做什么”
- `scope`
  - 回答“每条 op 具体应该补哪些 case、每个 case 的测试目标是什么”
- `matrix`
  - 回答“这些 case 目前做到哪一步了”

## Current Reading

当前状态应这样理解：

- `scope` 与 `matrix` 已对齐，且 `planned = 0`
- 覆盖率推进阶段已经完成
- 但这不等于“全部跑通”
- 当前仍有大量 `implemented` 与少量文档层 `blocked`
- 后续主任务已经转为：
  - 清理 `blocked`
  - 把更多 `implemented` 推进到 `board-passed`

状态口径：

- `board-passed`
  - 已完成上板验证
- `implemented`
  - case 已静态落地到仓库；即便 parser / verifier / lowering / codegen / runtime / board 失败，也仍算 coverage 已补齐
- `blocked`
  - 当前无法仅依据 `docs/vpto-spec.md` 与 `docs/isa/` 写出语义明确的 case

## Statistics

统计口径：

- `总op数`
  - 来自 `03-vpto-op-board-unit-tests-matrix.md` 的 `Op Summary`
- `已测op数`
  - 在 `Case Matrix` 中已有非 `planned` 结论的 op 数；当前由于 `planned = 0`，因此与 `总op数` 相同
- `当前case数`
  - `Case Matrix` 中登记的 case 行数
- `scope细化case数`
  - `03-vpto-op-board-test-scope.md` 中当前已细化的 case 数
- `scope-matrix缺口`
  - `scope细化case数 - 当前case数`

## Family Assessment

- `vector-load-store`
  - `总op数`：16
  - `已测op数`：16
  - `op覆盖率`：100.0%
  - `当前case数`：24
  - `scope细化case数`：24
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；下一步集中清理 load/store 相关 verifier / emitter / runtime 问题

- `gather-scatter`
  - `总op数`：4
  - `已测op数`：4
  - `op覆盖率`：100.0%
  - `当前case数`：8
  - `scope细化case数`：8
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；后续重点转为 gather/scatter lowering 与板测闭环

- `predicate-load-store`
  - `总op数`：7
  - `已测op数`：7
  - `op覆盖率`：100.0%
  - `当前case数`：6
  - `scope细化case数`：6
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；后续工作是把 round-trip / state-update case 从 `implemented` 推到 `board-passed`

- `materialization-predicate`
  - `总op数`：20
  - `已测op数`：20
  - `op覆盖率`：100.0%
  - `当前case数`：22
  - `scope细化case数`：22
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；部分 case 仍依赖 predicate emitter / verifier 收敛

- `unary-vector`
  - `总op数`：12
  - `已测op数`：12
  - `op覆盖率`：100.0%
  - `当前case数`：30
  - `scope细化case数`：30
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；后续重点是异常值、非 `f32` 类型以及若干 TD / emitter 缺口

- `binary-vector`
  - `总op数`：13
  - `已测op数`：13
  - `op覆盖率`：100.0%
  - `当前case数`：41
  - `scope细化case数`：41
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；后续重点是整数、carry/borrow、tail 与特殊值路径的真实闭环

- `vec-scalar`
  - `总op数`：12
  - `已测op数`：12
  - `op覆盖率`：100.0%
  - `当前case数`：31
  - `scope细化case数`：31
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；仍需继续清理标量立即数、整数形态和位运算变体的实现问题

- `compare-select`
  - `总op数`：4
  - `已测op数`：4
  - `op覆盖率`：100.0%
  - `当前case数`：18
  - `scope细化case数`：18
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；后续重点是 `vsel/vselr`、unordered compare 与 packed-mask 路径

- `conversion`
  - `总op数`：2
  - `已测op数`：2
  - `op覆盖率`：100.0%
  - `当前case数`：10
  - `scope细化case数`：10
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；后续重点是 `vcvt` 各类型对与特殊值路径的真实支持情况

- `reduction`
  - `总op数`：7
  - `已测op数`：7
  - `op覆盖率`：100.0%
  - `当前case数`：12
  - `scope细化case数`：12
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；`vcmax/vcmin` 仍有文档层 oracle 缺口，group/prefix reduction 仍待实现闭环

- `rearrangement`
  - `总op数`：10
  - `已测op数`：10
  - `op覆盖率`：100.0%
  - `当前case数`：14
  - `scope细化case数`：14
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；后续集中处理 TD surface 与 emitter 缺口

- `dsa-sfu`
  - `总op数`：12
  - `已测op数`：12
  - `op覆盖率`：100.0%
  - `当前case数`：19
  - `scope细化case数`：19
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已补齐；后续重点是 fused-op、widening、layout-transform 与异常值路径

- `dsa-sfu / conversion`
  - `总op数`：1
  - `已测op数`：1
  - `op覆盖率`：100.0%
  - `当前case数`：1
  - `scope细化case数`：1
  - `scope-matrix缺口`：0
  - `评估`：`vci` 已纳入 coverage 台账；后续再处理实现与板测闭环

- `TOTAL`
  - `总op数`：120
  - `已测op数`：120
  - `op覆盖率`：100.0%
  - `当前case数`：236
  - `scope细化case数`：236
  - `scope-matrix缺口`：0
  - `评估`：coverage 台账已完全补齐；当前阶段目标已从“清理 planned”切换为“清理 blocker / 提升通过率”

## Conclusion

- 当前 `scope` 已细化到 `236` 条 case，`matrix` 也保持同样规模。
- 当前 `scope` 与 `matrix` 已对齐，`scope-matrix缺口 = 0`。
- 当前 `matrix` 中状态分布为：
  - `board-passed`：4
  - `implemented`：216
  - `blocked`：16
  - `planned`：0
- 因此，当前结论应表述为：
  - 覆盖率推进阶段已经完成
  - 文档层 `blocked` 只剩 16 条
  - 后续主任务已经转为：
    - 补文档缺口
    - 清理 parser / verifier / lowering / codegen / runtime / board 问题
    - 把更多 `implemented/blocked` 收敛到 `board-passed`

## Usage

- 讨论“coverage 台账是否补齐”时，以本文件为准。
- 讨论“哪些 case 已经静态落地”时，以 `matrix` 的 `implemented/blocked/board-passed` 为准。
- 讨论“哪些 case 真正跑通了”时，以 `board-passed` 为准。
