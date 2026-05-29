# hw-native Python Flash Attention 到 PTOAS PTODSL 的迁移说明

## 来源

实际迁移来源：

```text
https://github.com/hw-native-sys/pto-isa/tree/main/kernels/python/flash_atten
```

核心参考文件：

```text
kernels/python/flash_atten/kernels/fa_builder.py
```

源实现使用的是旧的外部 `huawei-csl/pto-dsl` 包，不是 PTOAS 仓内当前的
`ptodsl/` API。本迁移的目标是把它的 Python DSL kernel 表达改写成当前
PTOAS PTODSL 表面。

## 当前文件

```text
ptodsl/examples/hw_native_flash_attention.py
```

这个文件按 `fa_builder.py` 的结构组织，作为独立迁移示例存在；原有
PTOAS `flash_attention_sketch.py` 保持不动，避免把教学 sketch 和
hw-native port 混在一起。

保留的源实现结构：

```text
compute_qk  : Cube 侧 Q/K matmul，产生 QK stage
compute_p   : SIMD 侧 streaming softmax，产生 P stage
compute_pv  : Cube 侧 P/V matmul，产生 PV stage
compute_gu  : SIMD 侧使用未归一化 running O 语义更新 O
finalize    : 最后用 running_sum 归一化 O
```

当前已经补齐的 PTO-DSL/frontend 表达：

```text
QK_PRELOAD prologue / steady / epilogue schedule
A5 local pipe surface for QK/P/PV stage boundaries
Vec_S0 row-slice 状态数组
exp_max_ring 按 preload slot 和 row-slice 保存
P slot 先按 [Vec_S0, S1_TILE] 生产，再按 [S0, CUBE_S1] 消费
block_idx/block_num 的 Q block 分发
```

## 当前 pipe 状态

之前文档里写的“PTODSL 前端不能写出 tpush/tpop/tfree”已经过期。

当前 FA 代码已经使用 PTOAS PTODSL 的 pipe surface：

```text
pto.reserve_buffer(...)
pto.pipe.c2v_local(...)
pto.pipe.v2c_local(...)
pipe.init_cube()
pipe.init_simd()
pipe.push(...)
pipe.pop(...)
pipe.free(...)
```

在生成的 MLIR 里，FA 已能看到这些 frontend pipe op：

```text
pto.aic_initialize_pipe
pto.aiv_initialize_pipe
pto.tpush_to_aiv
pto.tpop_from_aic
pto.tpush_to_aic
pto.tpop_from_aiv
pto.tfree_from_aic
pto.tfree_from_aiv
```

也就是说，`tpop` 不是当前 FA 前端的缺失项了。现在的限制是：FA 的 pipe
transaction 已经能在 PTODSL/frontend MLIR 里表达，但 runtime 正确性和性能
benchmark 仍需要后端/runtime 继续完成真实 FIFO 调度、生命周期和 A5/A3 运行验证。

## API 映射

| 旧实现接口 | 当前 PTODSL 表达 | 当前状态 |
| --- | --- | --- |
| `to_ir_module_with_meta(...)` | `@pto.jit(...).compile(...).mlir_text()` | 已迁移 |
| `TensorType/SubTensorType` | `pto.ptr + pto.make_tensor_view + pto.partition_view` | 已迁移 |
| `declare_global(...)` | 显式 GM slot tensor view | 已迁移为 frontend slot 表达 |
| `pto.load/pto.store` | `pto.tile.load / pto.tile.store` | 已迁移 |
| `tile.matmul / matmul_acc` | `@pto.cube + pto.mad / pto.mad_acc` | 已迁移 |
| vector row-reduce softmax/GU | `@pto.simd + tile row/reduce/expand ops` | 已迁移 |
| `initialize_l2g2l_pipe` | `pto.pipe.c2v_local / v2c_local + reserve_buffer` | A5 local pipe surface 已接入 |
| `talloc/tpush/tpop/tfree` | `pipe.push / pipe.pop / pipe.free` | frontend transaction 已接入 |
| `ffts_type / set_ffts` | 暂无同名 PTODSL API | 真实 runtime 调度仍属后端/runtime 工作 |

## 和 `fa_builder.py` 的剩余差距

剩余差距不再是“PTODSL 没有 tpop API”，而是下面这些更低层的问题：

- `fa_builder.py` 的旧 FIFO runtime 协议包含真实 allocate/free/record/wait、
  backpressure 和生命周期闭环；当前 PTODSL FA 只把 stage boundary 表达为
  frontend pipe init/push/pop/free transaction。
- 当前 FA 仍是 compile/frontend artifact，不是可直接作为性能 benchmark 的
  runtime 版本。
- 真实 cube/vector core 并行执行流、TSyncCVID 生命周期、host launch、
  workspace 分配和 torch/torch_npu 对拍，还需要后端/runtime 配合。
- `test_flash_attention_frontend_verify.py` 目前会把已知 `PTOPlanMemory`
  缺口标记为 `KNOWN_BACKEND_GAP`，避免把后端 PlanMemory 问题误判成
  frontend 迁移失败。

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

`test_flash_attention_demo_compile.py` 会检查 FA 生成的 MLIR 中包含 pipe
surface transaction，包括 `tpush_to_aiv`、`tpop_from_aic`、`tpush_to_aic`
和 `tpop_from_aiv`。

## 当前边界

这是 PTO-DSL/frontend 级迁移，不包含 A3/A5 runtime launch。

这个版本适合做：

- PTODSL API review
- MLIR/frontend compile 验证
- 后续后端 FIFO lowering 的输入样例

还不适合直接作为：

- runtime 正确性验证版本
- 性能 benchmark 版本

要进入 correctness / benchmark 阶段，还需要：

- 后端/runtime 完成真实 FIFO 调度和生命周期语义。
- 解决当前 FA frontend artifact 在后端 PlanMemory 中的已知缺口。
- 接入 host launch、workspace 分配和输入输出布局约定。
- 在 A5/A3 环境分别做 correctness 和性能验证。
