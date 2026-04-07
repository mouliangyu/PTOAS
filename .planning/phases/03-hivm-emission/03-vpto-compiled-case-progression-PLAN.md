# VPTO Compiled Runtime Progression Plan

## Summary

本专项用于把当前 `03-vpto-op-board-unit-tests-matrix.md` 中状态为 `compiled` 的 case，逐条从“仅 compile-only 可通过”推进到“运行校验无误 / 阻塞已明确”的结论闭环。

本专项的直接目标是把每条原始 `compiled` case 推进到以下三类结论之一：

- `board-passed`
- `board-blocked`
- `blocked`

其中：

- `board-passed` 表示已跑到 `compare passed`
- `board-blocked` 表示 case 已完成专项核对与运行尝试，但剩余问题确认属于 runtime / backend / toolchain
- `blocked` 只表示经开发者确认后，当前无法仅依据 `docs/vpto-spec.md` 与 `docs/isa/` 维持原测试目标并继续推进

本专项默认执行方式是“做完为止”，不是“清一批停一次”。

## Current Execution Mode

当前专项默认先使用模型路径推进，也就是优先使用 `DEVICE=SIM` 做运行校验与问题收敛；直接板测/NPU 不再作为默认首选路径。

这样做的目的不是降低标准，而是避免在已知直板路径不稳定时，把环境/板端现象误记成 case 结论，从而中断专项推进。

当前阶段的执行约定如下：

- 默认先走模型路径，形成 `sim-passed` 或继续留在 `checking/ready`
- 只有在模型路径已经通过，或已经足以排清 case 自身问题后，才允许把直板路径剩余问题记为 `board-blocked`
- 若直接板测先暴露异常，而模型路径尚未完成，则该异常只记录到 issue log / checklist 证据中，不直接作为 case 终态

## Document Contract

本文件只负责：

- 规定本轮 `compiled` case 从 `compiled` 推进到 `board-passed / board-blocked / blocked` 的执行顺序
- 规定每条 case 的固定检查项
- 规定何时允许把问题认定为真实 runtime 问题
- 规定何时允许把问题认定为正式 `blocked`
- 规定何时允许停止

本文件不负责：

- 替代 `matrix` 记录正式状态
- 替代 `scope` 定义 case 目标
- 替代 `runtime issues log` 归档已收敛问题

逐条执行进度由 `03-vpto-compiled-case-progression-checklist.md` 记录。

## Stop Condition

本轮允许停止的唯一条件是：

- `03-vpto-compiled-case-progression-checklist.md` 中不再存在 `todo`
- `03-vpto-compiled-case-progression-checklist.md` 中不再存在 `checking`
- `03-vpto-compiled-case-progression-checklist.md` 中不再存在 `ready`
- 每条原始 `compiled` case 都已落到 `sim-passed`、`board-blocked` 或正式 `blocked`

补充说明：

- 只要 checklist 中仍存在 `todo`、`checking` 或 `ready`，就必须继续执行下一条 case
- 不允许因为“已经连续推进了多条”“已经清完一个 family”“已经解决了一批共性问题”而结束当次执行
- 不允许在仍有未清空条目时输出收尾式总结、阶段性完结结论或“本轮到这里”的表述
- 若只是某条 case 落到了 `sim-passed` / `board-blocked` / `blocked`，这只意味着应立即进入下一条 case，而不是允许停下

只有以下真正的外部硬阻塞才允许中断当次执行：

- 当前机器或工具直接失效，后续 case 无法继续执行：
  - 构建工具不可用
  - runtime / simulator 环境整体不可用
  - 文件系统或临时目录不可写
  - 当前 shell/session 已损坏
- 必须由开发者拍板的正式语义分叉，且该分叉阻塞了下一步实现或 case 构造

以下都不属于允许中断的理由：

- 需要同步当前进展
- 需要先把已完成条目总结一下
- 已经拿到一批通过/阻塞结论
- 已经更新了 matrix / checklist / issue log
- 想先暂停，下一轮再继续

以下都不是允许停下的理由：

- 某个 family 清掉了一部分
- 已经整理出一批共性问题
- 某条 case runtime 失败
- 需要阶段性同步
- 已经把若干条 case 从 `compiled` 推进到了 `board-passed`
- 已同步 checklist / matrix / issue log
- 已经清完一小组或一个子家族

额外执行约束：

- 每完成一条 case 的状态落定后，必须立即进入 checklist 中下一条仍为 `todo`、`checking` 或 `ready` 的 case
- 不允许把“同步文档”或“整理进度”当作暂停点
- 本专项执行过程中，不做阶段性汇报；除非遇到必须由开发者确认的语义分叉、测试目标分叉或正式 `blocked` 判定，否则必须继续推进
- 当次执行结束前，必须再次检查 checklist 中是否仍有 `todo`、`checking` 或 `ready`；若有，则继续推进，不允许结束
- 在 checklist 仍存在 `todo`、`checking` 或 `ready` 时，除非出现真正的外部硬阻塞或必须由开发者拍板的语义分叉，否则不向用户输出任何结束当前执行链的消息；必须继续执行

## Execution Order

执行顺序固定如下，只用于组织处理顺序，不构成阶段边界：

1. `binary-vector` / `vec-scalar` / `unary-vector`
2. `compare-select`
3. `conversion`
4. `materialization-predicate` / `predicate-load-store`
5. `vector-load-store` / `reduction` / `rearrangement`
6. `gather-scatter` / `dsa-sfu`

要求：

- 清完一个 family 后直接进入下一个 family
- 不能因为一个 family 已有进展而暂停
- 同 family 内若发现共性问题，可以批量修，但修完必须继续扫完该 family 的剩余 `compiled` 条目

## Per-Case Checklist

每条 case 必须按以下顺序检查，不能跳步：

1. `kernel.pto`
   - 是否仍忠实对应 `scope` / `matrix` 的测试目标
   - 是否形成可观测输出并导回 GM
   - 是否满足 `pto.vecscope`、UB feed、同步等既定约束
2. `golden.py`
   - 输入与 oracle 是否和 `kernel.pto` 语义一致
3. `compare.py`
   - 文件名、dtype、shape、count、prefix、packed compare 逻辑是否正确
4. `main.cpp` / `launch.cpp` / `stub.cpp`
   - kernel symbol、buffer 数量、buffer 类型、host 接线是否一致
   - 特别检查 `kernel.pto` 导出的 kernel 名、`stub.cpp` 的声明名、`launch.cpp` 实际 launch 的 symbol 是否完全一致
   - 若出现“像是根本没执行到目标 kernel”“vec thread 为空”“veccore 指令日志近乎空白”，先把 host symbol mismatch 排除，再允许把现象解释为 backend/runtime 问题
5. runner contract
   - 是否能被 `test/vpto/scripts/run_host_vpto_validation.sh` 直接消费

处理规则：

- 若 1-5 任一项存在问题，先修 case 自身问题
- 修完后立刻继续跑该 case 的 runtime 验证
- 不允许把“已完成静态修正但还没跑”的 case 留到下一轮
- 本专项默认不主动展开实现修复；优先目标是把当前 case 的失败归因收敛清楚
- 只有在 1-5 全部确认无误并实际完成运行尝试后，失败才能记为真实 runtime / backend / toolchain 问题
- 若还未完成 1-5 的逐项核对，或尚未实际运行，则不得把 case 记为 `board-blocked`
- 若失败归因仍不确定，只能继续保留在 `checking` 或 `ready`，不得提前记为 `board-blocked`
- 只有当失败已被证明是稳定共性的实现缺陷，且修实现不会改变 case 目标时，才允许进入实现修复；修复后必须立刻回到当前 case 继续验证，不得借机切换到其他 case
- 若进入实现修复，语义依据必须首先来自 `docs/vpto-spec.md` 与 `docs/isa/`；不得直接按 `visa.txt` 或底层 intrinsic/wrapper 形式改实现
- `visa.txt` 只能作为底层对齐证据与 golden 参考；若其信息与 `docs/` 不一致或 `docs/` 缺失用户可见语义，必须先把结论沉淀到 `.planning/`，再把用户可见语义补充/修正到 `docs/`，之后才允许据此修改实现
- 若 `docs/` 当前不足以支撑实现修改，应先记录文档问题，而不是直接拿 `visa.txt` 作为实现契约
- 若在保持原测试目标不变的前提下，已经无法仅依据 `docs/vpto-spec.md` 与 `docs/isa/` 继续构造可验证闭环，必须先提交给开发者确认；只有开发者明确确认后，才允许把该 case 从专项 `board-blocked` 转成正式 `blocked`

## Status Transition

本专项执行状态只通过 `03-vpto-compiled-case-progression-checklist.md` 驱动，固定为：

- `todo`
- `checking`
- `ready`
- `sim-passed`
- `board-blocked`
- `blocked`

含义：

- `todo`
  - 还没开始清扫
- `checking`
  - 正在清扫，尚未形成最终运行结论
- `ready`
  - 已完成 case 自身问题修正与静态核对，下一步应立即进入运行尝试
- `sim-passed`
  - 已完成当前默认模型路径的专项核对，并已跑到 `compare passed`
- `board-blocked`
  - 在模型路径已通过或已足以排清 case 自身问题后，直板路径剩余问题确认属于 board/runtime/toolchain
- `blocked`
  - 经开发者确认后，当前无法仅依据 `docs/vpto-spec.md` 与 `docs/isa/` 保持原测试目标继续推进

额外约束：

- `ready` 只是中间态，不是允许长期停留的结论态
- 一条 case 进入 `checking` 后，必须继续推进到 `ready`、`sim-passed`、`board-blocked` 或正式 `blocked`
- 一条 case 进入 `ready` 后，必须继续推进到 `sim-passed`、`board-blocked` 或正式 `blocked`
- `board-blocked` 不是兜底状态，不能用“怀疑是 backend/runtime”替代证据
- `board-blocked` 也不是“直接板测先出问题”的快捷出口；若模型路径尚未跑完，不能先把 case 记成 `board-blocked`
- 只有在 case 自身问题已排清、并已有实际运行失败证据时，才允许落 `board-blocked`
- 不能因为阶段性想继续推进其他 case，就把未完成归因的条目提前记成 `board-blocked`

## Sync Rules

执行过程中同步规则固定如下：

- 若 case 到达 `sim-passed`
  - checklist 可视为当前阶段终态
  - 若当前只完成模型路径，则 `matrix` 暂保持原状态，并在 `notes` 中记录模型运行已通过
  - 若后续补做直板运行且通过，再把 `matrix` 中对应 case 改为 `board-passed`
- 若 case 到达 `board-blocked`
  - 立即把 `matrix` 中对应 case 从 `compiled` 改为 `board-blocked`
  - `notes` 必须明确写出：已完成专项核对与运行尝试，剩余问题属于 runtime / backend / toolchain
  - `notes` 与 checklist 必须同时记录失败命令、失败现象、归因结论和证据路径
- 若 case 经开发者确认到达正式 `blocked`
  - 立即把 `matrix` 中对应 case 从 `compiled` 改为 `blocked`
  - `notes` 必须明确写出文档缺口、语义冲突或 oracle 缺失的具体原因
- 若发现稳定复现的共性运行问题
  - 追加到 `03-vpto-runtime-issues-log.md`
- 若发现当前观测路径无法支撑测试目标
  - 才允许更新 `scope`

## Repeated Pitfalls

为避免同一类弯路被重复踩，本专项执行时必须遵守以下约定：

- 开始处理每条 case 前，先检索 `03-vpto-runtime-issues-log.md`，确认是否已有相同现象、已知问题或正确路径
- 若本轮推进中识别出新的稳定问题、错误路径或已验证可行的正确路径，必须追加到 `03-vpto-runtime-issues-log.md`
- 问题记录应优先沉淀“问题现象 + 错误路径 + 正确路径 + 证据”，而不是只写结论
- 后续遇到同类现象时，默认先复用问题记录中的正确路径，不重复走已确认错误的弯路
- 若现象是“空 vec thread / 空 instr_log / 像没执行到 kernel”，默认第一步不是怀疑 lowering，而是先核对：
  - `kernel.pto` 的 kernel 名
  - `stub.cpp` 的声明名
  - `launch.cpp` 的 launch symbol
  - `validation.log` 是否在修正后出现真实 `block_start/block_end`

## Principle

本专项负责把 `compiled` case 真正推进到“运行校验无误 / 阻塞已明确”，不允许通过以下方式伪造进展：

- 弱化测试目标
- 修改 case 语义以绕过真实校验
- 用“程序运行成功”代替语义 compare
- 把 case 自身问题误记成 runtime 问题
- 在 checklist 仍存在 `todo`、`checking` 或 `ready` 时，不允许以任何形式结束当次执行；必须继续推进下一条 case
