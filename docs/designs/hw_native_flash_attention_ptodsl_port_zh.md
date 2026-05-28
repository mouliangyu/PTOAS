# hw-native Python Flash Attention 到 PTOAS PTODSL 的迁移说明

## 来源

实际迁移来源是：

```text
https://github.com/hw-native-sys/pto-isa/tree/main/kernels/python/flash_atten
```

核心文件是：

```text
kernels/python/flash_atten/kernels/fa_builder.py
```

该实现使用的是旧的外部 `huawei-csl/pto-dsl` 包，不是 PTOAS 仓内当前的 `ptodsl/` API。本迁移的目标是把它的 Python DSL kernel 表达改写成当前 PTOAS PTODSL 表面。

## 当前文件

```text
ptodsl/examples/hw_native_flash_attention.py
```

这个文件按 `fa_builder.py` 的结构组织，作为独立迁移示例存在；原有 PTOAS `flash_attention_sketch.py` 保持不动，避免把教学 sketch 和 hw-native port 混在一起。

保留的源实现结构：

```text
compute_qk  : Cube 侧 Q/K matmul，写 QK GM slot
compute_p   : SIMD 侧 streaming softmax，写 P GM slot
compute_pv  : Cube 侧 P/V matmul，写 PV GM slot
compute_gu  : SIMD 侧使用未归一化 running O 语义更新 O
finalize    : 最后用 running_sum 归一化 O
```

当前已补齐的 PTO-DSL/frontend 表达：

```text
QK_PRELOAD prologue / steady / epilogue shadow schedule
Vec_S0 row-slice 状态数组
exp_max_ring 按 preload slot 和 row-slice 保存
P slot 先按 [Vec_S0, S1_TILE] 生产，再按 [S0, CUBE_S1] 消费
block_idx/block_num 的 Q block 分发
```

## API 映射

```text
旧 to_ir_module_with_meta(...)       -> @pto.jit(...).compile(...).mlir_text()
旧 TensorType/SubTensorType 全局类型 -> pto.ptr + pto.make_tensor_view + pto.partition_view
旧 declare_global + FIFO entry       -> 显式 GM slot tensor view
旧 pto.load/pto.store                -> pto.tile.load / pto.tile.store
旧 tile.matmul / matmul_acc          -> @pto.cube + pto.mad / pto.mad_acc
旧 vector row-reduce softmax/GU      -> @pto.simd + tile row/reduce/expand ops
```

## 接口差距清单

下面这张表按“`fa_builder.py` 里已有、但当前 PTOAS PTODSL 前端还没有直接等价接口”的方式列差距。

| 旧实现接口 | 当前 PTODSL 状态 | 库内是否已有接近实现 | 影响 |
| --- | --- | --- | --- |
| `initialize_l2g2l_pipe(...)` | 无同名公开 API | 有，IR/EmitC/transform 已有 `pto.initialize_l2g2l_pipe` | 只能把 FIFO 语义压成 shadow schedule，不能直出真实 pipe |
| `declare_global(...)` | 无同名公开 API | 有 `pto.declare_global` / `pto.declare_tile` 的 IR 侧设计 | 不能直接表达 global entry descriptor |
| `talloc/tpush/tpop/tfree` | 无同名公开 API | 有 `pto.talloc* / pto.tpush* / pto.tpop* / pto.tfree*` 及 lowerer | Python 前端还不能写出这条流水协议 |
| `TensorType/SubTensorType` | 仅有 `ptr + make_tensor_view + partition_view` | 有更底层的 `tile_buf` / `tensor_view` 类型体系 | 类型层能描述，但前端不够贴近原始写法 |
| `pto.func / pto.call` | 当前主要靠 `@pto.jit` 和 Python trace | 库内有 module/kernel kind / section 体系 | 可组织 kernel，但不等于原 builder 的显式函数调度 |
| `section.cube/vector` | PTODSL 有 `@pto.cube/@pto.simd/@pto.simt` 辅助 | 有 `pto.section.cube/vector` 和 `pto.kernel_kind` | 能分流，但不是 `fa_builder.py` 的完整执行流建模 |
| `ffts_type / set_ffts` | 无公开对应 | 内部存在更底层的 pipe / sync / target 相关实现 | 目前只能做部分同步面表达 |

结论是：**库里不是没有接近实现，而是接近实现主要存在于 PTO dialect、verify、lowering 和 EmitC 层，当前 PTODSL 公共 Python 面还没把这些能力完整抬上来。**

## 库内已有的接近实现

这部分是前面容易漏看的地方，也是这次分析里要单独说明的结论。

1. `include/PTO/IR/PTOOps.td` 和 `lib/PTO/Transforms/PTOLowerFrontendPipeOpsPass.cpp` 已经有 `initialize_l2g2l_pipe`、`talloc`、`tpush`、`tpop`、`tfree` 这一整条前端 pipe 协议，以及对应的 lowerer。

2. `include/PTO/IR/PTOOps.td` 和 `lib/PTO/Transforms/PTOWrapFunctionsInSectionsPass.cpp`、`lib/PTO/Transforms/VPTOSplitCVModule.cpp` 已经有 `pto.section.cube/vector`、`pto.kernel_kind` 以及 cube/vector 模块拆分能力。

3. `include/PTO/IR/PTOAttrs.td`、`lib/PTO/Transforms/PTOToEmitC.cpp`、`lib/PTO/Transforms/PTOPlanMemory.cpp`、`lib/PTO/Transforms/PTOResolveReservedBuffersPass.cpp` 也已经有对应的属性、同步、pipe、PlanMemory 和 reserved buffer 管线。

4. 但这些能力当前不是 `ptodsl/ptodsl/pto.py` 暴露出来的 `fa_builder.py` 风格 API。当前公开面更像是：
   - `@pto.jit(kernel_kind=...)`
   - `@pto.cube/@pto.simd/@pto.simt`
   - `pto.pipe_barrier / pto.get_buf / pto.rls_buf`
   - `pto.set_cross_flag / pto.wait_cross_flag`

   它们能覆盖一部分语义，但还不能直接复刻 `fa_builder.py` 的 FIFO allocate/free/record/wait 代码形态。

## 形状和约束

当前迁移保持源实现的主要约束：

```text
HEAD = 128
S0 = 128
CUBE_S1 = 128
S1_TILE = 256 或 512
QK_PRELOAD = 3 或 4
SLOT_NUM = 8
仅非 causal
```

## 使用

生成 MLIR：

```bash
python3 ptodsl/examples/hw_native_flash_attention.py \
  --head-dim 128 \
  --s1-tile 256 \
  --qk-preload 3 \
  -o /tmp/hw_native_flash_attention_ptodsl.mlir
```

走 PTOAS frontend：

```bash
build/tools/ptoas/ptoas /tmp/hw_native_flash_attention_ptodsl.mlir \
  --emit-pto-ir \
  -o /tmp/hw_native_flash_attention_ptodsl.pto
```

运行测试：

```bash
python3 ptodsl/tests/test_flash_attention_demo_compile.py
python3 ptodsl/tests/test_flash_attention_frontend_verify.py
```

`test_flash_attention_frontend_verify.py` 会先验证 PTODSL 生成的 MLIR 本身可 parse/verify；随后调用 `ptoas --emit-pto-ir`。当前后端在低层 `pto.tile_buf_addr` 上会触发已知 `PTOPlanMemory` 缺口，测试会明确标记为 `KNOWN_BACKEND_GAP`，避免把 frontend 迁移问题和后端 PlanMemory 支撑问题混在一起。

## 当前边界

这是 PTO-DSL/frontend 级迁移，不包含 A3/A5 runtime launch。

补充确认一下：PTOAS 库里已经有接近 `fa_builder.py` 的底层实现，主要落在 PTO dialect、verify 和 lowering 上；只是当前 `ptodsl/ptodsl/` 的 Python 公共面还没有把这套能力完整暴露出来。

旧实现里的 `initialize_l2g2l_pipe`、`talloc`、`tpush`、`tpop_into`、`tfree` 是真实 FIFO/runtime 调度语义。当前 PTOAS PTODSL 前端没有同名稳定 API，因此本迁移先把 QK/P/PV 的 GM slot 地址、shape、row-slice 状态、`QK_PRELOAD` 三段 shadow schedule 和阶段数据流显式写进 MLIR；真实 FIFO allocate/free/record/wait、消费反压和生命周期闭环仍需要后端/runtime 配合。

因此，这个版本适合做：

- PTODSL API review
- MLIR/frontend compile 验证
- 后续后端 FIFO lowering 的输入样例

不适合直接作为性能 benchmark 或 runtime 正确性验证版本。

## 距离性能 benchmark / 正确性验证版本的剩余工作

剩余主要不是 Python frontend 结构问题，而是缺少 runtime/API 支撑：

- 需要 PTOAS 后端/runtime 提供真实 GM staged FIFO 的 allocate/free/record/wait 语义。
- 需要真实 cube/vector core 并行执行流，而不是当前单 JIT 内的 shadow schedule。
- 需要 PTOAS 后端 PlanMemory 支持当前 PTODSL 为 cube `mad` 生成的 `pto.tile_buf_addr` 形态。
- 需要接入 host launch、workspace 分配、输入输出布局约定和 torch/torch_npu 对拍。
- 需要在 A3/A5 环境分别做 correctness 和性能 benchmark。
