# VPTO Runtime Issues Log

本文件记录 `test/vpto` 在 `DEVICE=SIM` / `DEVICE=NPU` 运行阶段已经遇到并收敛过的问题，目标是让后续板测或仿真遇到相同现象时可以快速定位。

使用规则：
- 只记录已经复现过、且已有明确结论或明确临时规避方式的问题
- 不记录纯 parser / verifier / emitter 问题；这些继续放在现有 compile-only / blocker 文档中
- 新条目按追加方式记录，不改写旧条目的编号
- 若旧问题已过期，可在对应条目下补充“当前状态”，不要直接删除历史结论

## 1. SIM 链接阶段找不到 `libruntime_camodel`

- 现象：
  - `DEVICE=SIM` 运行全量 case 时，host 链接阶段统一报：
  - `ld.lld: error: unable to find library -lruntime_camodel`
- 复现背景：
  - `SIM_LIB_DIR` 传入了 simulator 根目录，例如：
  - `~/.local/ascend/cann/aarch64-linux/simulator/Ascend950PR_9599`
- 结论：
  - `test/vpto/scripts/run_host_vpto_validation.sh` 期望 `SIM_LIB_DIR` 直接指向放置 `.so` 的目录，而不是 simulator 根目录
- 处理：
  - 将 `SIM_LIB_DIR` 指向实际库目录：
  - `~/.local/ascend/cann/aarch64-linux/simulator/Ascend950PR_9599/lib`

## 2. SIM host 链接阶段报 `rt*` 未定义

- 现象：
  - host 可执行链接阶段报：
  - `rtDevBinaryRegister`
  - `rtDevBinaryUnRegister`
  - `rtLaunch`
  - `rtKernelLaunch`
  - `rtKernelLaunchWithFlagV2`
  - `rtFunctionRegister`
- 复现背景：
  - `DEVICE=SIM`
  - case 的 `lib<case>_kernel.so` 已生成，但 host 可执行在 step 5 失败
- 结论：
  - 问题不在 case 本身，而在 `test/vpto/scripts/run_host_vpto_validation.sh` 的 SIM 链接接线
  - 需要在 kernel so 链接阶段显式带上 SIM runtime，并在 host 链接阶段允许 shared-library unresolved 由运行时装载解决
- 处理：
  - 已在 [test/vpto/scripts/run_host_vpto_validation.sh](/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/test/vpto/scripts/run_host_vpto_validation.sh) 修正
  - 若后续再次看到同类 `rt*` 未定义，先检查脚本是否回退或被覆盖

## 3. `~/.local/ascend/cann` 的 SIM 运行时库不自洽

- 现象：
  - step 6 启动可执行时立即失败，典型报错：
  - `libruntime_camodel.so: undefined symbol: _ZTIN3cce7runtime6StreamE`
  - 手工补入 `camodel` 目录后，进一步出现：
  - `libstars_wrapper.so: undefined symbol: _ZN9STARS_TOP27ext_write_ffts_plus_contextEjjPv`
- 复现背景：
  - `ASCEND_HOME_PATH=~/.local/ascend/cann`
  - `SIM_LIB_DIR=~/.local/ascend/cann/aarch64-linux/simulator/dav_3510/lib`
  - 或使用 `Ascend950PR_9599/lib` 这类指向 `dav_3510` 的别名路径
- 结论：
  - 当前本地这套 `~/.local/ascend/cann` simulator runtime 缺少能提供上述符号的库，整套运行时不自洽
  - 这不是 VPTO case 自身的问题
- 处理：
  - 当前不要用 `~/.local/ascend/cann` 这套 simulator 做结论性回归
  - 若必须修复，应先从 CANN 安装完整性或库版本匹配入手，而不是修改 VPTO case

## 4. `/usr/local/Ascend/cann-9.0.0` 自带 SIM 可用于 smoke

- 现象：
  - 使用 `/usr/local/Ascend/cann-9.0.0/aarch64-linux/simulator/dav_3510/lib` 时，`micro-op/binary-vector/vadd` 可以完整走通并 `compare passed`
- 结论：
  - 当前机器上可用的 SIM smoke 基线是 `/usr/local/Ascend/cann-9.0.0` 自带的 `dav_3510/lib`
- 处理：
  - 若需要做 SIM smoke，优先使用：
  - `ASCEND_HOME_PATH=/usr/local/Ascend/cann-9.0.0`
  - `SIM_LIB_DIR=/usr/local/Ascend/cann-9.0.0/aarch64-linux/simulator/dav_3510/lib`

## 5. SIM 高并发全量运行不稳定

- 现象：
  - `DEVICE=SIM JOBS=64` 全量并行时，大量 case 能进入真实运行，但中途出现：
  - `Aborted (core dumped)`
  - 最终 `parallel-summary.tsv` 只写出部分结果
- 结论：
  - 当前 simulator 在高并发下不稳定，不能把这类中断直接当作 case 回归结论
- 处理：
  - SIM 只用于 smoke 或低并发验证
  - 结论性运行测试优先走 `DEVICE=NPU`

## 6. NPU 运行优先使用 `ssh root@localhost`

- 现象：
  - 普通用户上下文可能在 `aclrtSetDevice` 阶段失败
  - 当前仓库脚本默认已经把 `HOST_RUNNER` 设为 `ssh root@localhost`
- 结论：
  - 本机的 NPU 运行优先使用脚本默认 root SSH 路径，而不是假设 `sudo` 可用
- 处理：
  - 直接使用：
  - `DEVICE=NPU`
  - 保持默认 `HOST_RUNNER=ssh root@localhost`
  - 若要先验环境，可手工确认：
  - `ssh -o BatchMode=yes -o StrictHostKeyChecking=no root@localhost 'id -u && hostname'`

## 7. `/tmp` 不是唯一风险点，工具链临时目录也可能打满

- 现象：
  - 即使工作区放在 `/home/...`，仍可能报：
  - `VPTO LLVM emission failed: cannot create bisheng query input: No space left on device`
  - `bisheng: error: unable to make temporary file: No space left on device`
- 结论：
  - 问题不一定是 `WORK_SPACE` 所在盘，而可能是工具链默认 `TMPDIR`
- 处理：
  - 运行测试时显式设置：
  - `TMPDIR=/home/<user>/tmp/<subdir>`

## 8. 首个 NPU 真实 case 失败要先检查 oracle 是否漂移

- 现象：
  - `micro-op/binary-vector/vadd-bf16` 首次 NPU 板测时 `compare failed`
- 定位：
  - [kernel.pto](/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/test/vpto/cases/micro-op/binary-vector/vadd-bf16/kernel.pto) 使用的是 `pto.vadd`
  - 但 [golden.py](/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/test/vpto/cases/micro-op/binary-vector/vadd-bf16/golden.py) 却按 `v1 - v2` 生成 oracle
- 结论：
  - 这是测例 oracle 漂移，不是后端或板端执行错误
- 处理：
  - 已将该 case 的 `golden.py` 修正为 `v1 + v2`
  - 后续板测遇到首个 `compare failed` 时，先核对：
  - `kernel.pto`
  - `golden.py`
  - `compare.py`
  - 三者是否和 case 目标一致

## 9. 当前已验证通过的 NPU smoke 基线

- 已通过：
  - `abs_strict_vecscope`
  - `micro-op/binary-vector/vadd`
  - `micro-op/binary-vector/vadd-bf16`（修正 oracle 后）
- 参考工作目录：
  - [/home/mouliangyu/tmp/vpto-npu-runtime-20260405-1](/home/mouliangyu/tmp/vpto-npu-runtime-20260405-1)
  - [/home/mouliangyu/tmp/vpto-npu-smoke-20260405-1](/home/mouliangyu/tmp/vpto-npu-smoke-20260405-1)
  - [/home/mouliangyu/tmp/vpto-npu-smoke-20260405-2](/home/mouliangyu/tmp/vpto-npu-smoke-20260405-2)

## 10. `vaddc` 已切到 `psti` 路径，但 runtime 问题尚未收敛

- 现象：
  - `micro-op/binary-vector/vaddc` 与 `micro-op/binary-vector/vaddc-carry-boundary` 已将 carry 输出从动态 `psts` 改为 16 个显式 `psti`
  - 两个 case 都能稳定通过 `DEVICE=NPU COMPILE_ONLY=1`
  - 两个 case 的真实 NPU 运行仍会 `compare failed`
- 已确认事实：
  - `visa.txt` 与 installed builtin header 都表明 `vaddc` / `vaddcs` 应按 32-bit carry op 对待
  - 这两个 case 的场景标签应为 `core-u32-unsigned`，不是旧的 `core-i16-unsigned`
  - 当前正式 case 已经是 `psti` 路径，不再是 `psts`
- 进一步探针结论：
  - 临时 result-only 探针去掉 carry-store 后，`COMPILE_ONLY` 仍可通过
  - 但真实 NPU 运行会卡在板端执行阶段，没有像原始 case 一样走到 compare
- 结论：
  - 当前不能把 `vaddc` 的 runtime 问题简单归因为 `psts` 残留
  - 后续需要继续排查 `vaddc` result path / intrinsic contract / execution 行为
- 参考工作目录：
  - 原始 case compile-only：
  - [/home/mouliangyu/tmp/vaddc-compile-only-5/micro-op_binary-vector_vaddc](/home/mouliangyu/tmp/vaddc-compile-only-5/micro-op_binary-vector_vaddc)
  - [/home/mouliangyu/tmp/vaddc-boundary-compile-only-5/micro-op_binary-vector_vaddc-carry-boundary](/home/mouliangyu/tmp/vaddc-boundary-compile-only-5/micro-op_binary-vector_vaddc-carry-boundary)
  - 原始 case runtime：
  - [/home/mouliangyu/tmp/vaddc-npu-run-serial/micro-op_binary-vector_vaddc](/home/mouliangyu/tmp/vaddc-npu-run-serial/micro-op_binary-vector_vaddc)
  - 临时 result-only 探针：
  - [/home/mouliangyu/tmp/vaddc-result-only-run/.debug_vaddc-result-only](/home/mouliangyu/tmp/vaddc-result-only-run/.debug_vaddc-result-only)

## 11. 若生成结果中的 vec thread 为空或明显缺少目标指令，优先把 op 序列收进 `vecscope { loop { ... } }`

- 现象：
  - 某些 case 虽然 compile-only 可通过，甚至能导出 LLVM/汇编产物，但最终生成结果里的 vec thread 为空，或缺少预期的向量/谓词指令
  - 这类现象在 `pldi/plds` 一类 case 的收敛过程中出现过
- 错误路径：
  - 只把 op 放进 `pto.vecscope`，但没有形成与后端期望一致的 `vecscope { loop { ... } }` 骨架
  - 直接从 LLVM 侧剪最小片段，导致 vecscope/loop 结构信息丢失
  - 看到“能编出来”就继续追运行，而没有先检查生成结果里 vec thread 是否真的包含目标指令
- 正确路径：
  - 若出现“vec thread 为空”或“vec thread 缺指令”，优先把该 case 的目标 op 序列收进一个完整的 `vecscope { loop { ... } }` 结构中，再重新导出和观察产物
  - 先确认生成结果里已经出现预期 vec 指令，再继续推进运行验证
  - 做最小化探针时，优先从现有可工作的 VPTO case 结构裁剪，保留 `vecscope + loop` 骨架，不从 LLVM 文本反推
- 适用范围：
  - 当前可泛化用于 `pldi/plds` 及其他依赖明确 vec thread materialization 的 VPTO case
- 证据：
  - `pldi/plds` 相关收敛记录分散见：
  - [03-vpto-predicate-ops-visa-alignment.md](/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.planning/phases/03-hivm-emission/03-vpto-predicate-ops-visa-alignment.md)
  - [03-vpto-micro-op-compile-failures-review.md](/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.planning/phases/03-hivm-emission/03-vpto-micro-op-compile-failures-review.md)
- 已通过的相关 case 包括：
  - `micro-op/predicate-load-store/psti-pk-pldi-us`
  - `micro-op/predicate-load-store/psti-norm-pldi-ds`
  - `micro-op/predicate-load-store/psts-pk-plds-us`
  - `micro-op/predicate-load-store/psts-norm-plds-ds`

## 12. typed binary-vector 批量生成 case 可能残留旧的 `f32/tail-mask` skeleton

- 现象：
  - 某些 `binary-vector` 类型变体表面上已有独立 case 目录，但 `kernel.pto`、`golden.py`、host 接线仍沿用旧的 `f32/tail-mask` 模板
  - 典型表现是 case 名称写的是 `f16` / `bf16` / `full-mask`，实际 kernel 与 oracle 仍在测试另一套目标
- 错误路径：
  - 看到 `COMPILE_ONLY` 能过就直接推进 runtime
  - 只看 case 目录名与 matrix 标签，不逐项核对 `kernel.pto` / `golden.py` / `compare.py` / host 接线
- 正确路径：
  - 对 typed binary-vector case，先按专项 plan 的 1-5 项核对 case 本体是否真的对齐测试目标
  - 若仍残留旧 skeleton，先把 case 收口到真实目标语义，再进入模型路径运行
- 已验证样例：
  - `micro-op/binary-vector/vadd-f16`
  - `micro-op/binary-vector/vadd-bf16`
- 证据：
  - [/home/mouliangyu/tmp/vpto-progress-vadd-f16-sim/micro-op_binary-vector_vadd-f16](/home/mouliangyu/tmp/vpto-progress-vadd-f16-sim/micro-op_binary-vector_vadd-f16)
  - [/home/mouliangyu/tmp/vpto-progress-vadd-bf16-sim/micro-op_binary-vector_vadd-bf16](/home/mouliangyu/tmp/vpto-progress-vadd-bf16-sim/micro-op_binary-vector_vadd-bf16)

## 13. exceptional-values 测例在 SIM 中可能打印 vec 异常日志，但仍与 oracle 一致

- 现象：
  - `f32` 异常值输入的向量算术在 `DEVICE=SIM` 运行时，模型日志可能反复打印：
  - `vec_err_idata_inf_nan_t0`
  - 指令日志示例：
  - `RV_VADD Dtype: F32`
- 错误路径：
  - 看到模型日志中的 vec 异常提示，就直接把 case 记成 runtime 失败或 blocker
- 正确路径：
  - 对 exceptional-values case，先以最终 `compare.py` 结果为准
  - 若日志有 vec 异常提示，但最终 `compare passed`，当前应记为模型路径通过，并把该日志现象作为证据备注下来
- 已验证样例：

## 14. 若看起来“没有任何 vec 指令执行”，先排查 host launch symbol 是否打到真实 kernel

- 现象：
  - case 能完整走完 lowering、编译、链接与 host 构建
  - 但运行后最初观察到的现象像是：
  - vec thread 为空
  - veccore 指令日志接近空白
  - 或看起来像 device 侧根本没有执行目标 kernel
- 已复现样例：
  - `micro-op/vector-load-store/vldsx2-layout-check`
  - `micro-op/vector-load-store/vldsx2-vstsx2`
  - `micro-op/vector-load-store/vstsx2-layout-check`
- 错误路径：
  - 直接根据“空 vec thread / 空 instr_log / 没看到目标指令”下结论，认为是 `vldsx2` lowering、LLVM emission 或指令选择失效
  - 没先核对 `kernel.pto` 导出的设备 kernel 名称，是否和 `stub.cpp` / `launch.cpp` 中声明与 launch 的 symbol 一致
- 根因：
  - case 的 host 侧仍残留旧 symbol：
  - `LaunchVabs_kernel_2d(...)`
  - 或 `stub.cpp` / `launch.cpp` 中引用了错误的 kernel ABI 名
  - 导致最初的运行观测不可信，前面对“没有 vec 指令”的判断被 host launch 错误污染
- 正确路径：
  - 一旦运行现象像“没执行到目标 kernel”，先核对四件事：
  - `kernel.pto` 中的 `func.func @...`
  - `stub.cpp` 中 `extern "C" __global__ AICORE void ...`
  - `launch.cpp` 中实际 `<<<...>>>` launch 的 symbol
  - 产物工作目录中的 `validation.log` / veccore log 是否在修正后出现真实 `block_start/block_end` 与非零 `instr_log`
  - 只有 host symbol 对齐后，才允许继续解读：
  - LLVM IR
  - 汇编
  - veccore 日志
  - compare 结果
- 当前结论：
  - `vldsx2-layout-check` 修正 host symbol 后，SIM `compare passed`
  - `vldsx2-vstsx2` 修正 host symbol 与 oracle 后，SIM `compare passed`
  - `vstsx2-layout-check` 修正 host symbol 后，SIM `compare passed`
- 证据：
  - layout-check：
  - [/home/mouliangyu/tmp/vpto-vldsx2-layout-rerun-20260407/micro-op_vector-load-store_vldsx2-layout-check](/home/mouliangyu/tmp/vpto-vldsx2-layout-rerun-20260407/micro-op_vector-load-store_vldsx2-layout-check)
  - roundtrip：
  - [/home/mouliangyu/tmp/vpto-vldsx2-vstsx2-rerun-20260407/micro-op_vector-load-store_vldsx2-vstsx2](/home/mouliangyu/tmp/vpto-vldsx2-vstsx2-rerun-20260407/micro-op_vector-load-store_vldsx2-vstsx2)
  - store-layout：
  - [/home/mouliangyu/tmp/vpto-vstsx2-layout-rerun-20260407/micro-op_vector-load-store_vstsx2-layout-check](/home/mouliangyu/tmp/vpto-vstsx2-layout-rerun-20260407/micro-op_vector-load-store_vstsx2-layout-check)

## 14. `pstu` 旧的 `RV_WMOV type.B8` 结论已过期

- 旧现象：
  - `micro-op/predicate-load-store/pstu`
  - `micro-op/predicate-load-store/pstu-state-advance-boundary`
  - 曾在旧环境/旧构造下报：
  - `Unsupported Instr/Type). instr.name=RV_WMOV, type.B8`
- 新结论：
  - 该结论已过期，不能再作为当前分支的阻塞依据
  - `2026-04-08` 使用 `/usr/local/Ascend/cann-9.0.0/aarch64-linux/simulator/dav_3510/lib` 重新复跑，这两条 case 均已进入真实 block 执行并 `compare passed`
- 当前参考工作目录：
  - `/tmp/vpto-unaligned-rerun-20260408/micro-op_predicate-load-store_pstu`
  - `/tmp/vpto-unaligned-rerun-20260408/micro-op_predicate-load-store_pstu-state-advance-boundary`

## 15. step 6 若 `Total tick` 很短且输出全 0，先核对 host launcher 是否与 LLVM IR kernel 名完全一致

- 现象：
  - `vslide`
  - `vslide-tail-window`
  - `vintlv-vdintlv`
  - `vintlv-vdintlv-lane-boundary`
  - `vsunpack`
  - `vzunpack`
  - 这两条 case 早期都出现过 `Total tick: 5/6/8`、无 `block_start`、`instr_log.dump` 全空、输出全 0
- 结论：
  - 这类“空跑”现象不能直接归到 runtime/backend
  - 第一优先级应核对 `stub.cpp` / `launch.cpp` / `main.cpp` 里的 kernel / launcher 符号是否与导出的 LLVM IR `define void @...` 完全同名
  - `vslide` 旧结论中的 empty-run 根因是 host 侧仍调用 `vabs_kernel_2d`
  - `vslide-tail-window` 旧结论中的 empty-run 根因是 host 侧调用名写成了 `vslide_tail_window_kernel_2d`，而 LLVM IR 实际是 `vslide_tail_kernel_2d`
  - `vintlv-vdintlv` / `vintlv-vdintlv-lane-boundary` / `vsunpack` / `vzunpack` 的旧 empty-run 根因同样是 host 侧仍调用 `vabs_kernel_2d`
- 处理：
  - 若看到 `Total tick` 很短 + `instr_log.dump` 为空，先做这三步：
  - 打开本次导出的 LLVM IR，确认真实 kernel 名
  - 检查 `stub.cpp` / `launch.cpp` / `main.cpp` 是否逐字对齐该 kernel / launcher 名
  - 只有在 host 符号完全对齐后，才允许继续把问题归到 runtime/backend
- 参考工作目录：
  - `/home/mouliangyu/tmp/vpto-vslide-rerun-20260407/micro-op_rearrangement_vslide`
  - `/home/mouliangyu/tmp/vpto-vslide-tail-rerun-20260407/micro-op_rearrangement_vslide-tail-window`
  - `/home/mouliangyu/tmp/vpto-vintlv-fix-20260407/micro-op_rearrangement_vintlv-vdintlv`
  - `/home/mouliangyu/tmp/vpto-vintlv-boundary-fix2-20260407/micro-op_rearrangement_vintlv-vdintlv-lane-boundary`
  - `/home/mouliangyu/tmp/vpto-vsunpack-fix-20260407/micro-op_rearrangement_vsunpack`
  - `/home/mouliangyu/tmp/vpto-vzunpack-fix-20260407/micro-op_rearrangement_vzunpack`

## 16. `vslide` 家族在已验证 SIM 基线上会落到 `RV_VSLIDE type.B32` 的 runtime 缺口

- 现象：
  - `micro-op/rearrangement/vslide`
  - `micro-op/rearrangement/vslide-tail-window`
  - 修正 host symbol，并使用已验证可用的 `/usr/local/Ascend/cann-9.0.0/aarch64-linux/simulator/dav_3510/lib` 后，step 6 都进入真实 block 执行，最终统一报：
  - `Unsupported Instr/Type). instr.name=RV_VSLIDE, type.B32`
- 结论：
  - 这两条 case 当前已排除 host skeleton / empty-run 误判
  - 当前证据指向 simulator/runtime backend 对 `RV_VSLIDE type.B32` 的实现缺口
- 处理：
  - 后续若再次看到同一签名，不要回头修改 case/oracle，优先按 runtime/backend 缺口处理
- 参考工作目录：
  - `/home/mouliangyu/tmp/vpto-vslide-fix-usr3510-20260407/micro-op_rearrangement_vslide`
  - `/home/mouliangyu/tmp/vpto-vslide-tail-fix2-usr3510-20260407/micro-op_rearrangement_vslide-tail-window`

## 17. `vstar` 的旧 `RV_WMOV type.B8` 记录仍需与 `vstur` 分开看

- 现象：
  - `micro-op/vector-load-store/vstar`
  - 在 `DEVICE=SIM` step 6 运行时报：
  - `Unsupported Instr/Type). instr.name=RV_WMOV, type.B8`
- 结论：
  - 这不是 compile-only 或 host skeleton 问题；case 已进入真实 runtime
  - `vstar` 早期 skeleton 曾误写成 `vldas -> vstar`；按 docs 修正为最小合法链 `vlds -> vldas(dest) -> vstur -> vstar` 后，现象不变
  - 当前该阻塞只针对 `vstar`
- 处理：
  - `vstar` 后续构造必须依赖前置 stateful store 链，不能再直接把 `vldas` 结果喂给 `vstar`
  - 后续若再碰到同样签名，不要先改 case/oracle，优先按 runtime 缺口处理
- 参考工作目录：
  - `/home/mouliangyu/tmp/vpto-progress-vls-batch1-sim-20260407/micro-op_vector-load-store_vstar`
  - `/home/mouliangyu/tmp/vpto-vstar-rerun2-20260407/micro-op_vector-load-store_vstar`

## 18. `vstur` 旧的 `RV_WMOV type.B8` 结论已过期

- 旧现象：
  - `micro-op/vector-load-store/vstur`
  - 曾在旧环境/旧构造下报：
  - `Unsupported Instr/Type). instr.name=RV_WMOV, type.B8`
- 新结论：
  - 该结论已过期，不能再作为当前分支的阻塞依据
  - `2026-04-08` 使用 `/usr/local/Ascend/cann-9.0.0/aarch64-linux/simulator/dav_3510/lib` 重新复跑，`micro-op/vector-load-store/vstur` 已进入真实 block 执行并 `compare passed`
- 当前参考工作目录：
  - `/tmp/vpto-unaligned-rerun-20260408/micro-op_vector-load-store_vstur`

## 17. `vsldb` / `vsstb` 即使补齐最小 inner-loop 后仍无 vec/cube 指令日志，按 empty-run 处理

- 现象：
  - `micro-op/vector-load-store/vsldb`
  - `micro-op/vector-load-store/vsstb`
  - 按 plan 将 vecscope 内目标 op 序列收口到最小 single-iteration inner-loop 后重新 `DEVICE=SIM` 复跑
  - 两条 case 都仅跑出 `Total tick: 6`
  - 输出保持全 0
- 已确认事实：
  - repo-generated LLVM IR 已分别发射 `llvm.hivm.vsldb` 与 `llvm.hivm.vsstb`
  - 但 `core0.veccore*.instr_log.dump` / `core0.cubecore0.instr_log.dump` 仍为空文件
- 结论：
  - inner-loop 结构修正未改变现象
  - 当前不能再把问题归因到 vecscope 结构不合规，应按 runtime/backend empty-run 处理
- 处理：
  - 后续若继续排查，优先检查 SIM 端是否对 `vsldb/vsstb` lowering 路径根本未 materialize 指令流
- 参考工作目录：
  - `/home/mouliangyu/tmp/vpto-progress-vsldb-rerun-sim-20260407/micro-op_vector-load-store_vsldb`
  - `/home/mouliangyu/tmp/vpto-progress-vsstb-rerun-sim-20260407/micro-op_vector-load-store_vsstb`

## 19. `vldas-vldus-state-chain` 旧的链式 state-update 结论已过期

- 旧现象：
  - `micro-op/vector-load-store/vldas-vldus-state-chain`
  - 曾按错误的“隐式 state-update”预期解释输出
- 新结论：
  - `2026-04-08` 已按 no-post 真实语义收口：两次 `pto.vldus` 各自显式消费自己的 `pto.vldas`
  - 使用 `/usr/local/Ascend/cann-9.0.0/aarch64-linux/simulator/dav_3510/lib` 重新复跑后 `compare passed`
- 当前参考工作目录：
  - `/tmp/vpto-unaligned-rerun-20260408/micro-op_vector-load-store_vldas-vldus-state-chain`

## 20. `vstas-vstus-offset-update` 旧的 `RV_WMOV type.B8` 结论已过期

- 旧现象：
  - `micro-op/vector-load-store/vstas-vstus-offset-update`
  - 曾在旧环境/旧构造下出现输出全 0 与 `RV_WMOV type.B8`
- 新结论：
  - `2026-04-08` 已将 case 收口为合法的 no-post stream：`pto.vstus` 只返回 `%align_out`，`pto.vstas` 使用显式 base+offset 的匹配 flush point
  - 使用 `/usr/local/Ascend/cann-9.0.0/aarch64-linux/simulator/dav_3510/lib` 重新复跑后 `compare passed`
- 当前参考工作目录：
  - `/tmp/vpto-unaligned-rerun-20260408/micro-op_vector-load-store_vstas-vstus-offset-update`

## 16. `vsqz` 的旧 `ISU stall` 结论已过期，根因是未重建旧 `ptoas` 二进制

- 旧现象：
  - `micro-op/rearrangement/vsqz`
  - `micro-op/rearrangement/vsqz-nontrivial-mask`
  - 历史记录曾观察到 `RV_VSQZ + RV_VSTS` 后出现 `ISU stall`
- 新结论：
  - `2026-04-09` 已确认当时使用的是未重建的旧 `ptoas` 二进制
  - 旧 `.ll` 将 standalone `vsqz -> vsts` 误发为 `llvm.hivm.vsqz...(..., i32 1)`
  - 重建 `ptoas` 后重新导出 `.ll`，当前已正确发射 `st=0`
  - `micro-op/rearrangement/vsqz` 与 `micro-op/rearrangement/vsqz-nontrivial-mask` 在 `DEVICE=SIM` 路径均 compare passed
- 当前参考工作目录：
  - `/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.work/vsqz-rerun-after-rebuild/micro-op_rearrangement_vsqz`
  - `/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.work/vsqz-nontrivial-rerun-after-rebuild/micro-op_rearrangement_vsqz-nontrivial-mask`

## 17. `vsts` 当前也会落到短 tick empty-run

- 现象：
  - `micro-op/vector-load-store/vsts`
  - case 本体已是纯 `vlds + vsts` identity-store 观测，不再有旧 skeleton 语义漂移
  - `DEVICE=SIM` step 6 仅 `Total tick: 6`，输出全 0
- 结论：
  - 当前可直接归类为 runtime/backend empty-run，不需要继续改 oracle
- 参考工作目录：
  - `/home/mouliangyu/tmp/vpto-progress-vls-batch1-sim-20260407/micro-op_vector-load-store_vsts`

## 18. `vstar` 当前在 SIM 上也会落到 `RV_WMOV type.B8`

- 现象：
  - `micro-op/vector-load-store/vstar`
  - 运行已进入真实 block 执行，但在：
  - `Unsupported Instr/Type). instr.name=RV_WMOV, type.B8`
  - 处触发 assert / core dump
- 结论：
  - 与 `pstu` 类似，当前证据指向 simulator/runtime backend 缺口
- 参考工作目录：
  - `/home/mouliangyu/tmp/vpto-progress-vls-batch1-sim-20260407/micro-op_vector-load-store_vstar`

## 14. 若 step 6 仅 `Total tick: 12` 且没有 `block_start`，优先怀疑 runtime/backend 未真正发射可执行 vec 线程

- 现象：
  - `DEVICE=SIM` step 1-5 全部通过，`.ll` 中也能看到目标 `llvm.hivm.*` intrinsic
  - 但 step 6 日志里没有：
  - `block_start`
  - `block_end`
  - 同工作目录下 `core*.veccore*.instr_log.dump` 为空
  - 最终只看到：
  - `Total tick: 12`
  - 输出缓冲区保持全零或初值
- 已确认样例：
  - `micro-op/unary-vector/vnot`
  - `micro-op/unary-vector/vexp-f16`
- 结论：
  - 这两条旧结论已失效，根因是 host skeleton 仍调用了错误 kernel symbol
  - 修正 host symbol 后，两条 case 都已进入真实 block 执行并 compare passed
- 处理：
  - 若再次出现同类 “`Total tick: 12` + 无 `block_start` + 输出全零”，先核对 `kernel.pto` / `stub.cpp` / `launch.cpp` / `main.cpp` 的 kernel symbol 是否完全一致
  - 不要在未核对 host symbol 前把结论记成 runtime/backend 缺口
- 证据：
  - [/home/mouliangyu/tmp/vpto-vnot-rerun-20260407/micro-op_unary-vector_vnot](/home/mouliangyu/tmp/vpto-vnot-rerun-20260407/micro-op_unary-vector_vnot)
  - [/home/mouliangyu/tmp/vpto-vexpf16-rerun-20260407/micro-op_unary-vector_vexp-f16](/home/mouliangyu/tmp/vpto-vexpf16-rerun-20260407/micro-op_unary-vector_vexp-f16)
  - `micro-op/binary-vector/vadd-f32-exceptional`
  - `micro-op/binary-vector/vdiv-f32-exceptional`
  - `micro-op/binary-vector/vmin-f32-exceptional`
  - `micro-op/dsa-sfu/vlrelu-f32-exceptional`

## 14. unary-vector 旧 skeleton 若残留 `vabs`/`vexp` kernel symbol，先修正 host 绑定再判断 runtime 结论

- 现象：
  - `micro-op/unary-vector/vrsqrt`
  - `micro-op/unary-vector/vrec`
  - `micro-op/unary-vector/vbcnt`
  - `micro-op/unary-vector/vcls`
  - `micro-op/unary-vector/vrsqrt-zero-inf`
  - `micro-op/unary-vector/vrec-zero-inf`
  - 这些 case 的 `kernel.pto` / `stub.cpp` / `launch.cpp` / `main.cpp` 曾残留旧 skeleton 的 `vabs_kernel_2d` 或 `vexp_kernel_2d`
- 已确认事实：
  - 修正 host symbol 前，旧结论不足以证明是目标 op 的真实 runtime 行为
  - 修正 host symbol 后，以上 case 都能稳定通过 step 1-5 并进入真实 `block_start`
  - 之后才分别在：
  - `RV_VRSQRT type.F32`
  - `RV_VREC type.F32`
  - `RV_VBCNT type.B32`
  - `RV_VCLS type.S32`
  - 处触发 `Unsupported Instr/Type`
- 结论：
  - 这组 case 先有 host skeleton 漂移，再有真实 runtime/backend 缺口
  - 不应再直接引用修正前的旧 evidence 作为最终 blocker 结论
- 处理：
  - 若 unary-vector case 看起来“直接 unsupported”，先核对是否仍残留 `vabs`/`vexp` 旧 symbol
  - 只有修正 host symbol 且确认出现真实 `block_start` 后，才允许把 unsupported 记为 runtime/backend 缺口
- 证据：
  - `/home/mouliangyu/tmp/vpto-vrsqrt-rerun-20260407/micro-op_unary-vector_vrsqrt`
  - `/home/mouliangyu/tmp/vpto-vrec-rerun-20260407/micro-op_unary-vector_vrec`
  - `/home/mouliangyu/tmp/vpto-vbcnt-rerun-20260407/micro-op_unary-vector_vbcnt`
  - `/home/mouliangyu/tmp/vpto-vcls-rerun-20260407/micro-op_unary-vector_vcls`
  - `/home/mouliangyu/tmp/vpto-vrsqrt-zero-inf-rerun-20260407/micro-op_unary-vector_vrsqrt-zero-inf`
  - `/home/mouliangyu/tmp/vpto-vrec-zero-inf-rerun-20260407/micro-op_unary-vector_vrec-zero-inf`

## 14. gather/scatter gather 家族需先排除 host symbol 漂移；修正后当前表现为真实执行但结果不对

- 现象：
  - `micro-op/gather-scatter/vgather2`
  - `micro-op/gather-scatter/vgatherb`
  - `micro-op/gather-scatter/vgather2_bc`
  - 以及对应变体在 `DEVICE=SIM` 运行时最终 `compare failed`
  - 早期曾被误记为输出全 0 / `ffts_verify_log0.log` 为空
- 已确认事实：
  - `vgather2` 主 case 的 host skeleton 也曾错误绑定到 `vabs_kernel_2d`
  - `vgatherb` / `vgather2_bc` / `vgather2-duplicate-index` / `vgather2_bc-sparse-mask` / `vgatherb-block-boundary` 的 host skeleton 曾错误绑定到 `vabs_kernel_2d`
  - 修正 host symbol 后，以上 case 都已进入真实 block 执行并出现 `block_start/block_end`
  - case 本体已核对，`kernel.pto` / `golden.py` / `compare.py` 与目标一致
  - repo-generated `.ll` 已分别发射：
  - `llvm.hivm.vgather2.v300.v64f32`
  - `llvm.hivm.vgatherb.v310.v64f32`
  - `llvm.hivm.vgather2.bc.v64f32`
  - 周边 `vldsx1` / `vstsx1` / `plt` 也都已发射
- 结论：
  - 当前问题不在 compile-only，也不在 host skeleton；修正 host 后仍稳定 compare failed
  - 当前证据更接近 runtime/backend 缺口
- 处理：
  - 暂记相关 case 为 `board-blocked`
  - 后续若继续追，应优先对照已安装实现/真实工具链产物确认 gather intrinsic contract

## 14. `vexpdiff-f32` 当前 SIM 路径会生成 `vexpdif` LLVM IR，但运行时输出全零且 vec 指令日志为空

- 现象：
  - `micro-op/dsa-sfu/vexpdiff-f32` 能稳定通过 `DEVICE=SIM COMPILE_ONLY=1`
  - 真实 `DEVICE=SIM` 运行后 `v2.bin` 全零，`compare failed`
  - 同一工作目录下 `core0.veccore0.instr_log.dump` / `core0.veccore1.instr_log.dump` 为空
- 已确认事实：
  - 当前 case body 是 `pto.vexpdiff %vec, %vec, "ODD"`，也就是 `input=max=input, part=ODD`
  - repo-generated `.ll` 已发射：
  - `call <64 x float> @llvm.hivm.vexpdif.v64f32f32(<64 x float> %17, <64 x float> %17, <256 x i1> %19, i32 1)`
  - `.ll` 仍保留了 `!llvm.loop.aivector_scope`
- 结论：
  - 当前问题不是 parser / verifier / compile-only 缺口
  - 在现有证据下，更像是 `vexpdif` 的 LLVM-path runtime contract 或 simulator 行为仍未收敛
- 处理：
  - 先将 `micro-op/dsa-sfu/vexpdiff-f32` 记为 `sim-blocked`
  - 后续继续推进同族 case 时，优先对照：
  - repo-generated `.ll`
  - 最终 `v*.bin`
  - `veccore*.instr_log.dump`
- 参考工作目录：
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_dsa-sfu_vexpdiff-f32-sim-rerun-1/micro-op_dsa-sfu_vexpdiff-f32](/home/mouliangyu/tmp/vpto-progress-micro-op_dsa-sfu_vexpdiff-f32-sim-rerun-1/micro-op_dsa-sfu_vexpdiff-f32)

## 15. `vexpdiff-f16-part` 当前 SIM 路径已发射并执行 vec 指令，但输出布局/语义与文档目标不对称

- 现象：
  - `micro-op/dsa-sfu/vexpdiff-f16-part` 能稳定通过 `DEVICE=SIM COMPILE_ONLY=1`
  - 真实 `DEVICE=SIM` 运行后 `compare failed`
  - `core0.veccore0.instr_log.dump` 非空，说明 vec 指令确实执行了
- 已确认事实：
  - repo-generated `.ll` 已发射两次：
  - `@llvm.hivm.vexpdif.v128f16f32(..., i32 0)`
  - `@llvm.hivm.vexpdif.v128f16f32(..., i32 1)`
  - 当前输出不是全零；首段结果可直接匹配逐 lane `exp(input - max)`
  - 但后续段出现非预期常数 1，与当前 `part-even-odd` 文档目标不对称
- 结论：
  - 当前问题不是“没有发射/没有执行”
  - 更像是 `vexpdif` 在 `f16 -> f32` + `part` 模式下的真实 contract 尚未与文档收敛
- 处理：
  - 先将 `micro-op/dsa-sfu/vexpdiff-f16-part` 记为 `sim-blocked`
  - 后续若继续推进，优先对齐 docs 目标与实际 part 语义，再决定是否修改 oracle
- 参考工作目录：
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_dsa-sfu_vexpdiff-f16-part-sim-rerun-1/micro-op_dsa-sfu_vexpdiff-f16-part](/home/mouliangyu/tmp/vpto-progress-micro-op_dsa-sfu_vexpdiff-f16-part-sim-rerun-1/micro-op_dsa-sfu_vexpdiff-f16-part)

## 16. `vmull` 当前 SIM 路径已发射 `llvm.hivm.vmull.v64s32`，但运行输出全零且 vec 指令日志为空

- 现象：
  - `micro-op/dsa-sfu/vmull` 能稳定通过 `DEVICE=SIM COMPILE_ONLY=1`
  - 真实 `DEVICE=SIM` 运行后 `v2.bin` 全零，`compare failed`
  - `core0.veccore0.instr_log.dump` / `core0.veccore1.instr_log.dump` 都为空
- 已确认事实：
  - 当前 case 已收口为真实 `i32 input -> low/high output` 语义，host/oracle 不再是旧的 `f32` skeleton
  - repo-generated `.ll` 已发射：
  - `declare { <64 x i32>, <64 x i32> } @llvm.hivm.vmull.v64s32(<64 x i32>, <64 x i32>, <256 x i1>)`
  - `.ll` 保留了 `!llvm.loop.aivector_scope`
- 结论：
  - 当前问题不是 case 本体未接好
  - 在现有证据下，更像是 `vmull` 的 LLVM-path runtime contract 或 simulator 行为仍未收敛
- 处理：
  - 先将 `micro-op/dsa-sfu/vmull` 记为 `sim-blocked`
  - 后续若继续推进，优先对照 `.ll`、`v2.bin` 和 `veccore*.instr_log.dump`
- 参考工作目录：
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_dsa-sfu_vmull-sim-rerun-1/micro-op_dsa-sfu_vmull](/home/mouliangyu/tmp/vpto-progress-micro-op_dsa-sfu_vmull-sim-rerun-1/micro-op_dsa-sfu_vmull)

## 17. `vbitsort` 不能放在 `pto.vecscope` 内；移到 scope 外后 SIM 已通过

- 现象：
  - `micro-op/dsa-sfu/vbitsort` 旧版本放在 `pto.vecscope` 内时，`DEVICE=SIM COMPILE_ONLY=1` 可过，但真实 `DEVICE=SIM` 运行后 `compare failed`
- 已确认事实：
  - 当前 case 已按文档收口为 `f32 score + u32 index -> packed proposal records`
  - repo-generated `.ll` 已发射 `llvm.hivm.VBS32.V300.f32`
  - 直接从 `.ll` 编出来的汇编中也能看到 `VBS32.f32`
  - 根因不是 emission 缺失，而是 `pto.vbitsort` 被错误放进了 `pto.vecscope`
- 结论：
  - `pto.vbitsort` 是 UB helper op，不属于 vecscope 内的向量执行语义
  - authoring 约束应明确为：`pto.vbitsort` 必须位于 `pto.vecscope` / `pto.strict_vecscope` 外
- 处理：
  - 已将 case 改为 scope 外写法并复跑
  - 已将 `micro-op/dsa-sfu/vbitsort` 更新为 `sim-passed`
  - 后续如再遇到 `vbitsort` 空跑或 compare failed，先检查是否被错误嵌入 `pto.vecscope`
- 参考工作目录：
  - [/home/mouliangyu/tmp/vpto-vbitsort-rerun3-20260407/micro-op_dsa-sfu_vbitsort](/home/mouliangyu/tmp/vpto-vbitsort-rerun3-20260407/micro-op_dsa-sfu_vbitsort)

## 18. `vlrelu-f32-exceptional` 在 SIM 中会打印 vec 异常日志，但最终 oracle 一致

- 现象：
  - `micro-op/dsa-sfu/vlrelu-f32-exceptional` 在 `DEVICE=SIM` 运行时反复打印：
  - `vec_err_idata_inf_nan_t0`
  - 指令日志点名：
  - `RV_VLRELU Dtype: F32`
- 结论：
  - 这类日志在异常值 case 中不直接等价于测试失败
  - 当前应先以 `compare.py` 结果为准；若最终 `compare passed`，则按计划记为模型路径通过
- 参考工作目录：
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_dsa-sfu_vlrelu-f32-exceptional-sim-rerun-1/micro-op_dsa-sfu_vlrelu-f32-exceptional](/home/mouliangyu/tmp/vpto-progress-micro-op_dsa-sfu_vlrelu-f32-exceptional-sim-rerun-1/micro-op_dsa-sfu_vlrelu-f32-exceptional)

## 14. `vcvt f16 -> f32` widening 当前在 LLVM-path SIM 上只能产出前半结果

- 现象：
  - `micro-op/conversion/vcvt-f16-to-f32` 与 `micro-op/conversion/vcvt-f16-special` 都能稳定通过 `DEVICE=SIM COMPILE_ONLY=1`
  - 真实 `DEVICE=SIM` 运行时，kernel 完整执行到 `block_end`，但输出仅每个 128-lane block 的前 64 个 `f32` 结果有效，后 64 个恒为 0
- 已确认事实：
  - repo-generated `.ll` 已稳定发射两次 `llvm.hivm.vcvtff.f162f32.x(<128 x half>, <256 x i1>, i32)`，最后一个 `i32` 分别为 `0` / `1`
  - 第二路 `part = 1` 的结果在最终输出中未体现，导致 widening case 的后半结果缺失
  - 这不是简单的 case store-mask 问题；将 case 改成 `PAT_ALL` full-mask 后，现象不变
- 已安装实现线索：
  - installed Clang header `__clang_cce_vector_intrinsics.h` 中，`vfcvt(f16 -> f32)` 的 surface 确实走 `__builtin_cce_vcvtff_f162f32_x(src, mask, part)`
  - 但 installed debug spec 还表明 `part_mode` 会影响底层 `pgIdx` / op1 选择，说明当前 repo LLVM-path contract 仍需继续核对
- 结论：
  - 当前应把 `f16 -> f32` widening 相关 case 记为真实 `sim-blocked`
  - 在确认 installed PTO / Bisheng 对 `part = ODD` 的真实 LLVM contract 之前，不应继续猜测性修改 emitter
- 证据：
  - `/home/mouliangyu/tmp/vpto-progress-micro-op_conversion_vcvt-f16-to-f32-sim-rerun-4/micro-op_conversion_vcvt-f16-to-f32`
  - `/home/mouliangyu/tmp/vpto-progress-micro-op_conversion_vcvt-f16-special-sim-rerun-2/micro-op_conversion_vcvt-f16-special`
  - `micro-op/binary-vector/vadd-f32-exceptional`
- 证据：
  - [/home/mouliangyu/tmp/vpto-progress-vadd-f32-exceptional-sim/micro-op_binary-vector_vadd-f32-exceptional](/home/mouliangyu/tmp/vpto-progress-vadd-f32-exceptional-sim/micro-op_binary-vector_vadd-f32-exceptional)

## 14. 某些 compile-only 已通过的饱和算术在 SIM 中仍可能缺少具体 `RV_* + type` 运行支持

- 现象：
  - case 能完成 compile-only，也能进入 step 6
  - 但模型运行阶段直接报：
  - `Unsupported Instr/Type`
  - 例如：
  - `instr.name=RV_VSADD, type.S16`
- 错误路径：
  - 因为 compile-only 已通过，就默认认为运行期也该支持
  - 看到 core dump 后先怀疑 case 构造，而不先核对模型日志中的具体 `RV_*` / `type`
- 正确路径：
  - 先确认 case 本体、golden、compare、host 接线都与测试目标一致
  - 若模型明确报出 `Unsupported Instr/Type`，应优先记为 runtime/backend 实现缺口，而不是修改 case 目标
- 已验证样例：
  - `micro-op/binary-vector/vsadd`

## 15. `psts NORM` 的 predicate 落盘布局要按 mask granularity 展开，不是统一 1-bit packed

- 现象：
  - `micro-op/compare-select/vcmp-eq` 在修正为最小 64-lane `vcmp + psts` case 后，模型可完整运行，但按 `np.packbits(equal(...), bitorder=\"little\")` 生成的 oracle 仍然 compare failed
  - `micro-op/compare-select/vcmp-i16-signed` 收口后，若把 `mask<b16>` 也按 1-bit packed 处理，同样会与真实输出语义不一致
- 错误路径：
  - 把 `mask<b32>` 当成“每个逻辑 lane 只占 1 bit”的 packed predicate image
  - 用 `np.packbits` 直接生成 `golden_v3.bin`
- 正确路径：
  - `vcmp` / `vcmps` 在 LLVM 侧产出的是统一的 `<256 x i1>`
  - `psts NORM` 的 host oracle 必须按 mask granularity 展开：
  - `mask<b32>`: 每个逻辑 lane 占 4 个 predicate bit，也就是每个 lane 一个 nibble
  - `mask<b16>`: 每个逻辑 lane 占 2 个 predicate bit，也就是每 4 个 lane 打成 1 byte
  - 不能用统一的 `np.packbits` 直接生成 `golden_v3.bin`
- 已验证样例：
  - `micro-op/compare-select/vcmp-eq`
  - `micro-op/compare-select/vcmp-i16-signed`
- 证据：
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_compare-select_vcmp-eq-sim/micro-op_compare-select_vcmp-eq](/home/mouliangyu/tmp/vpto-progress-micro-op_compare-select_vcmp-eq-sim/micro-op_compare-select_vcmp-eq)
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_compare-select_vcmp-i16-signed-sim/micro-op_compare-select_vcmp-i16-signed](/home/mouliangyu/tmp/vpto-progress-micro-op_compare-select_vcmp-i16-signed-sim/micro-op_compare-select_vcmp-i16-signed)

## 16. `vshls` / `vshrs` 若出现“输出全 0”，先排查 host symbol；两者都已证实不是 runtime 缺口

- 现象：
  - `micro-op/vec-scalar/vshls`
  - `micro-op/vec-scalar/vshrs`
  - 以及对应的 boundary case
  - 都能完成 `COMPILE_ONLY`
  - 也能进入 `DEVICE=SIM` step 6
  - 早期 `vshls` / `vshrs` 及其 boundary case 都曾被误记为 `v2.bin` 全 0，`compare failed`
- 已确认事实：
  - case 已从旧的 `f32/prefix` skeleton 收口为真实 `ui16 + PAT_ALL` 最小闭环
  - 生成的 `.ll` 已明确包含：
  - `llvm.hivm.vshls.v128u16.x`
  - `llvm.hivm.vshrs.v128u16.x`
  - 对应的 `vldsx1` / `vstsx1`
  - 与之对照，`vadds-i16-unsigned` 使用同一 host/kernel 骨架时可在 SIM 中正常 compare passed
  - `vshls` / `vshrs` 及其 boundary case 的 host skeleton 曾错误绑定到 `vmuls_tail_kernel_2d`
  - 修正 host symbol 后：
  - `micro-op/vec-scalar/vshls` compare passed
  - `micro-op/vec-scalar/vshrs` compare passed
  - `micro-op/vec-scalar/vshls-shift-boundary` compare passed
  - `micro-op/vec-scalar/vshrs-shift-boundary` compare passed
- 错误路径：
  - 看到 `.ll` 中已有目标 intrinsic，就把问题继续归因到 golden/compare
  - 未先检查 veccore trace 是否真的有向量线程执行
- 正确路径：
  - 对这两条指令，若 `DEVICE=SIM` 输出全 0，应先检查：
  - `core0.veccore0.instr_popped_log.dump`
  - `core0.veccore0.rvec.simd.idu.TRACE.dump`
  - 以及 `kernel.pto` / `stub.cpp` / `launch.cpp` 的 kernel symbol 是否一致
  - 只有 symbol 对齐后仍失败，才允许归因为 runtime/backend 缺口，而不是继续改 case oracle
- 证据：
  - [/home/mouliangyu/tmp/vpto-vshls-rerun-20260407/micro-op_vec-scalar_vshls](/home/mouliangyu/tmp/vpto-vshls-rerun-20260407/micro-op_vec-scalar_vshls)
  - [/home/mouliangyu/tmp/vpto-vshrs-rerun-20260407/micro-op_vec-scalar_vshrs](/home/mouliangyu/tmp/vpto-vshrs-rerun-20260407/micro-op_vec-scalar_vshrs)
  - [/home/mouliangyu/tmp/vpto-vshrs-boundary-rerun-20260407/micro-op_vec-scalar_vshrs-shift-boundary](/home/mouliangyu/tmp/vpto-vshrs-boundary-rerun-20260407/micro-op_vec-scalar_vshrs-shift-boundary)
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_vec-scalar_vadds-i16-unsigned-sim/micro-op_vec-scalar_vadds-i16-unsigned](/home/mouliangyu/tmp/vpto-progress-micro-op_vec-scalar_vadds-i16-unsigned-sim/micro-op_vec-scalar_vadds-i16-unsigned)

## 18. `vsqz` 若出现 `ISU stall`，先确认 `ptoas` 是否已重建；当前已证实不是 runtime 缺口

- 现象：
  - `micro-op/rearrangement/vsqz`
  - `micro-op/rearrangement/vsqz-nontrivial-mask`
  - 旧记录中都曾被归因为 `RV_VSQZ + RV_VSTS` 后的 veccore `ISU stall`
- 已确认事实：
  - 当前 repo 源码中的 `determineVsqzStoreHint()` 只在 `vsqz` 结果被 `pto.vstur` 消费时才发 `st=1`
  - `micro-op/rearrangement/vsqz/kernel.pto` 的 post-pass VPTO IR 中，`pto.vsqz` 的直接 consumer 是 `pto.vsts`
  - 使用旧 `build/tools/ptoas/ptoas` 导出的 `.ll` 曾错误生成 `llvm.hivm.vsqz...(..., i32 1)`
  - 重建 `ptoas` 后重新导出 `.ll`，同一 standalone `vsqz -> vsts` 路径已正确生成 `llvm.hivm.vsqz...(..., i32 0)`
  - 之后两条 case 在 `DEVICE=SIM` 复跑都 compare passed
- 错误路径：
  - 没有先重建 `ptoas`，直接拿旧 `.ll` / 旧 SIM 现象继续分析 `SQZN` 或 runtime
  - 看到 `ISU stall` 就继续归因到 `VSQZ/SQZN` 协议本身
- 正确路径：
  - 若 standalone `vsqz -> vsts` case 出现 `ISU stall`，先执行 `ninja -C build ptoas`
  - 再重新导出 `.ll`，确认 `llvm.hivm.vsqz...` 的第三个参数是否为 `i32 0`
  - 只有在重建后二进制仍发成 `st=1` 或仍稳定挂住时，才继续分析 runtime/backend
- 证据：
  - `/tmp/vsqz-recheck-after-rebuild.ll`
  - [/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.work/vsqz-rerun-after-rebuild/micro-op_rearrangement_vsqz](/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.work/vsqz-rerun-after-rebuild/micro-op_rearrangement_vsqz)
  - [/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.work/vsqz-nontrivial-rerun-after-rebuild/micro-op_rearrangement_vsqz-nontrivial-mask](/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.work/vsqz-nontrivial-rerun-after-rebuild/micro-op_rearrangement_vsqz-nontrivial-mask)

## 17. `vaddcs` / `vsubcs` 当前 SIM 路径会完整执行 block，但结果向量与 carry/borrow 输出都保持为 0

- 现象：
  - `micro-op/vec-scalar/vaddcs`
  - `micro-op/vec-scalar/vaddcs-carry-boundary`
  - `micro-op/vec-scalar/vsubcs`
  - `micro-op/vec-scalar/vsubcs-borrow-boundary`
  - 都能稳定通过 `DEVICE=SIM COMPILE_ONLY=1`
  - 真实 `DEVICE=SIM` 运行时也有 `block_start/block_end`
  - 但 `v3.bin` 与 `v4.bin` 最终都保持全 0
- 已确认事实：
  - case 已从旧的 float skeleton 收口为真实 `u32` 双输出 case
  - host/golden/compare 已同步到 result + carry/borrow 双观测
  - `.ll` 已明确发射：
  - `llvm.hivm.vaddcs.v64u32` / `llvm.hivm.vsubcs.v64u32`
  - `llvm.hivm.vstsx1.v64u32`
  - `llvm.hivm.psti.b8`
- 错误路径：
  - 看到 `block_start/block_end` 就把问题继续归因到 compare 或 packed predicate oracle
  - 忽略 `v3.bin` 与 `v4.bin` 同时全 0 这一事实
- 正确路径：
  - 对这两条指令，若 `COMPILE_ONLY` 已通过且 `.ll` 已有 `vaddcs/vsubcs + psti`，但运行后 result/carry(或 borrow) 同时全 0，应优先记为 runtime/backend 缺口
  - 不要再通过弱化 case 目标或只看单一输出去规避问题
- 证据：
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_vec-scalar_vaddcs-sim/micro-op_vec-scalar_vaddcs](/home/mouliangyu/tmp/vpto-progress-micro-op_vec-scalar_vaddcs-sim/micro-op_vec-scalar_vaddcs)
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_vec-scalar_vaddcs-carry-boundary-sim/micro-op_vec-scalar_vaddcs-carry-boundary](/home/mouliangyu/tmp/vpto-progress-micro-op_vec-scalar_vaddcs-carry-boundary-sim/micro-op_vec-scalar_vaddcs-carry-boundary)
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_vec-scalar_vsubcs-sim/micro-op_vec-scalar_vsubcs](/home/mouliangyu/tmp/vpto-progress-micro-op_vec-scalar_vsubcs-sim/micro-op_vec-scalar_vsubcs)
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_vec-scalar_vsubcs-borrow-boundary-sim/micro-op_vec-scalar_vsubcs-borrow-boundary](/home/mouliangyu/tmp/vpto-progress-micro-op_vec-scalar_vsubcs-borrow-boundary-sim/micro-op_vec-scalar_vsubcs-borrow-boundary)
- 同类样例：
  - `micro-op/binary-vector/vssub`
- 证据：
  - [/home/mouliangyu/tmp/vpto-progress-vsadd-sim/micro-op_binary-vector_vsadd](/home/mouliangyu/tmp/vpto-progress-vsadd-sim/micro-op_binary-vector_vsadd)
  - [/home/mouliangyu/tmp/vpto-progress-vssub-sim/micro-op_binary-vector_vssub](/home/mouliangyu/tmp/vpto-progress-vssub-sim/micro-op_binary-vector_vssub)

## 15. `vaddc` / `vsubc` 在 SIM 中可能完整执行但结果与 carry/borrow 输出同时全 0

- 现象：
  - `micro-op/binary-vector/vaddc` 与 `micro-op/binary-vector/vsubc` 都能完成 compile-only，也能完整进入 `step 6/6`
  - `validation.log` 显示模型正常 `block_start -> block_end -> compare failed`
  - 输入文件与 oracle 正常，但 `v3.bin` 与 `v4.bin` 都是全 0
- 已确认事实：
  - case 本体已经按 `u32 + full-mask + carry-chain` surface 收口
  - LLVM IR 中已完整发射：
  - `llvm.hivm.vaddc.v64u32` / `llvm.hivm.vsubc.v64u32`
  - `llvm.hivm.vstsx1.v64u32`
  - `llvm.hivm.psti.b8`
  - 模型没有报 `Unsupported Instr/Type`
- 错误路径：
  - 把这类现象先归咎为 `golden.py` 或 `compare.py`
  - 看到 `compare failed` 就继续改 case，而不先核对输入/oracle/LLVM IR 是否都已对齐测试目标
- 正确路径：
  - 先确认 `kernel.pto`、`golden.py`、`compare.py`、host 接线都已对齐
  - 再核对 `.ll` 中是否确实存在目标 carry op 与后续 store
  - 若以上均成立、模型也完整执行结束，但输出仍整体归零，则当前优先记为 runtime/backend 实现缺口
- 已验证样例：
  - `micro-op/binary-vector/vaddc`
  - `micro-op/binary-vector/vsubc`
- 证据：
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_binary-vector_vaddc-sim/micro-op_binary-vector_vaddc](/home/mouliangyu/tmp/vpto-progress-micro-op_binary-vector_vaddc-sim/micro-op_binary-vector_vaddc)
  - [/home/mouliangyu/tmp/vpto-progress-micro-op_binary-vector_vsubc-sim-rerun/micro-op_binary-vector_vsubc](/home/mouliangyu/tmp/vpto-progress-micro-op_binary-vector_vsubc-sim-rerun/micro-op_binary-vector_vsubc)

## 16. materialization-predicate 家族的 `psts` 结果必须按 32B 槽位观测，oracle 只比较语义前缀

- 现象：
  - `ppack/punpack`、`pdintlv/pintlv`、`pand/por/pxor/pnot/psel`、`pset-pattern-fragment` 一批 `materialization-predicate` case 在 `DEVICE=SIM` 下最初不是 compare 失败，就是命中 `psts` 地址对齐断言
  - 典型报错：
  - `RV_PSTS ... Address 0x00000008 is not aligned to 32 bytes`
- 错误路径：
  - 把多个 predicate 结果写到 `%ub_out[%c8]` / `%ub_out[%c16]` 这类非 32B 对齐偏移
  - 直接按整块输出 buffer 全量比较，误把未定义尾部或未写满区域当作语义输出
  - `psel-tail-predicate` 这类双槽位 case 只拷贝了首个 `32B` 槽位，漏掉第二个 predicate 结果
- 正确路径：
  - `psts` 落盘统一使用 `0/32/64...` 这类 32B 对齐槽位
  - oracle 只比较各 case 真正写出的 packed-predicate 语义前缀，不把未定义尾部纳入 compare
  - 若 case 写出多个 32B predicate 槽位，`copy_ubuf_to_gm` 长度必须覆盖全部已写槽位；例如 `psel-tail-predicate` 需要 `64B`
- 已验证样例：
  - `micro-op/materialization-predicate/pge-tail-mask`
  - `micro-op/materialization-predicate/plt-tail-mask`
  - `micro-op/materialization-predicate/ppack-punpack`
  - `micro-op/materialization-predicate/pdintlv_b8`
  - `micro-op/materialization-predicate/pintlv_b8`
  - `micro-op/materialization-predicate/pand`
  - `micro-op/materialization-predicate/psel-tail-predicate`

## 17. reduction 家族的 `vcg*` / `vcpadd` 不能按当前 docs 猜测结果布局，先记为 docs/runtime 不一致

- 现象：
  - `micro-op/reduction/vcgadd`
  - `micro-op/reduction/vcgmax`
  - `micro-op/reduction/vcgmin`
  - `micro-op/reduction/vcgadd-tail`
  - `micro-op/reduction/vcgmax-tie`
  - `micro-op/reduction/vcgmin-tie`
  - `micro-op/reduction/vcpadd`
  - `micro-op/reduction/vcpadd-tail`
  - 都能稳定通过 `DEVICE=SIM COMPILE_ONLY=1`
  - 真实 `DEVICE=SIM` 运行也会执行到 `block_end`
  - 但结果布局与 `docs/isa/10-reduction-ops.md` 不一致
- 已确认事实：
  - 这组 case 早期都还残留旧 skeleton 的 `vabs_kernel_2d` host symbol；2026-04-07 修正 host 绑定后，已确认不再是 empty-run 或错误 kernel 调用
  - `vcgadd/vcgmax/vcgmin` 当前 SIM 输出会把每个 64-lane chunk 的 8 个 group result 连续写到首 8 lane，然后其余 lane 清零
  - 这与当前 docs 中“结果写在 `0, 8, 16, ... 56` 槽位”的描述不一致
  - `vcgmax-tie/vcgmin-tie` 也表现为同样的 contiguous-first-8 布局
  - `vcpadd` 当前 SIM 非零输出只出现在 `[0..31], [64..95], ...` 这类子区间，不符合 docs 的整向量 prefix 语义
  - 这些 case 的旧 skeleton/golden 漂移已先修正，不是 case 本体错误
- 错误路径：
  - 根据当前 SIM 观测去反推并修改 docs 语义
  - 为了过 compare 直接把 oracle 改成贴合当前 runtime 行为
  - 把这类 mismatch 继续归因到 golden/compare，而不先承认 docs/runtime 已真实不一致
- 正确路径：
  - 先把 case 保持在 docs 目标下
  - 在 matrix/checklist 中记为 `sim-blocked`
  - 等 docs 语义或 runtime 行为由开发者确认后再收敛
- 证据：
  - [/home/mouliangyu/tmp/vpto-vcgadd-rerun-20260407/micro-op_reduction_vcgadd](/home/mouliangyu/tmp/vpto-vcgadd-rerun-20260407/micro-op_reduction_vcgadd)
  - [/home/mouliangyu/tmp/vpto-vcgmax-rerun-20260407/micro-op_reduction_vcgmax](/home/mouliangyu/tmp/vpto-vcgmax-rerun-20260407/micro-op_reduction_vcgmax)
  - [/home/mouliangyu/tmp/vpto-vcgmin-rerun-20260407/micro-op_reduction_vcgmin](/home/mouliangyu/tmp/vpto-vcgmin-rerun-20260407/micro-op_reduction_vcgmin)
  - [/home/mouliangyu/tmp/vpto-vcpadd-rerun-20260407/micro-op_reduction_vcpadd](/home/mouliangyu/tmp/vpto-vcpadd-rerun-20260407/micro-op_reduction_vcpadd)
  - [/home/mouliangyu/tmp/vpto-vcpadd-tail-rerun-20260407/micro-op_reduction_vcpadd-tail](/home/mouliangyu/tmp/vpto-vcpadd-tail-rerun-20260407/micro-op_reduction_vcpadd-tail)
  - [/home/mouliangyu/tmp/vpto-vcgadd-tail-rerun-20260407/micro-op_reduction_vcgadd-tail](/home/mouliangyu/tmp/vpto-vcgadd-tail-rerun-20260407/micro-op_reduction_vcgadd-tail)
  - [/home/mouliangyu/tmp/vpto-vcgmax-tie-rerun-20260407/micro-op_reduction_vcgmax-tie](/home/mouliangyu/tmp/vpto-vcgmax-tie-rerun-20260407/micro-op_reduction_vcgmax-tie)
  - [/home/mouliangyu/tmp/vpto-vcgmin-tie-rerun-20260407/micro-op_reduction_vcgmin-tie](/home/mouliangyu/tmp/vpto-vcgmin-tie-rerun-20260407/micro-op_reduction_vcgmin-tie)
