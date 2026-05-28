# flash_atten4 A5 C++ 到 PTODSL 迁移计划

## 目标和边界

实际移植源是 GitCode `csjlchen/pto-isa` 的 `fa4dsl` 分支：

```text
kernels/manual/a5/flash_atten4/
```

`hw-native-sys/pto-isa` 的 `kernels/python/flash_atten/README_zh.md` 只作为 PTODSL 表达方式、测试方式和调度经验参考，不能作为移植源覆盖 A5 C++ 的默认参数或行为。

当前 `ptodsl/examples/flash_atten4_port.py` 是 compile-only 的 PTODSL 迁移脚手架，不是高性能实现完成版。

## A5 C++ 源码要点

主要文件：

```text
fa_performance_kernel.h
fa_performance_dn_kernel.cpp
pto_macro_dn_matmul.hpp
pto_macro_fa_dn_softmax.hpp
pto_macro_fa_dn_gu.hpp
```

关键默认值来自 `fa_performance_kernel.h`：

```text
kFaCvFifoSize = 8
kFaCvFifoConsSyncPeriod = 4
kFaCubeS1 = 128
kFaTileS1 = 256
kFaQkPreload = 4
kFaLaunchCoreCount = 28
VEC_CORES = 2
```

主流水：

```text
compute_qk  -> qk_tile_fifo, fp32
compute_p   -> p_tile_fifo, fp16, plus exp_max_ififo
compute_pv  -> pv_tile_fifo, fp32
compute_gu  -> running O update / final O
```

重要调度和内存设计：

- 默认 `FIFO_MODE=1`，即 ALL_UB path；CMake 也支持 `0=ALL_GM`、`2=QK_PV_UB_ONLY`。
- `TILE_FACTOR = TILE_S1 / CUBE_S1`，默认是 `2`。
- `compute_qk` 和 `compute_pv` 在 cube core 上交错。
- `compute_p` 和 `compute_gu` 在 vector core 上交错。
- Vector 侧不是整块 `CUBE_S0 x CUBE_S1` 常驻处理，而是按 row-slice 降低 UB 压力：

```text
Vec_S0 = CUBE_S0 / VEC_CORES / TILE_FACTOR
VecGuRows = CUBE_S0 / VEC_CORES
```

在默认 `CUBE_S0=128, VEC_CORES=2, TILE_FACTOR=2` 下，`Vec_S0=32`，`VecGuRows=64`。

## 当前 PTODSL 状态

已经具备：

- PTODSL 文件 `ptodsl/examples/flash_atten4_port.py`。
- 四阶段函数命名已经对齐 A5 源码：`fa4_compute_qk`、`fa4_compute_p`、`fa4_compute_pv`、`fa4_compute_gu`。
- 默认形状已经按 A5 C++ 对齐：`S0=128`、`HEAD_DIM=128`、`CUBE_S1=128`、`TILE_S1=256`、`QK_PRELOAD=4`。
- `CV_FIFO_SIZE=8`、`CV_FIFO_CONS_SYNC_PERIOD=4` 和 `FIFO_MODE=1` 已经暴露为 PTODSL constexpr 参数，后续接真实 FIFO 调度时可以复用入口契约。
- Vector 侧物理 tile 已经开始按 A5 row-slice 思路收缩：默认 `Vec_S0=32`。
- Manual-parity 默认形状已经可以通过 `ptoas --emit-pto-ir` 的 vec memory 检查。
- Softmax/GU 状态更新已经改为 A5 风格：循环内维护未归一化 running O，最后用最终 `global_sum` 做归一化。
- Entry signature 已加入 A5 scratch/通信指针雏形：`p_tile_fifo`、`exp_max_ififo`、`qk_tile_fifo`、`pv_tile_fifo`、`pv_pend_tile_fifo`、`o_parts_out`、`cv_comm_buf`。
- QK/P/exp_max/PV/PV_pend/O_parts 阶段已经向对应 GM FIFO/scratch view 写入中间 tile，使 compile-only IR 中能看到 FIFO 数据边界。
- QK/P FIFO 的物理 view 已经按 A5 `CV_FIFO_SIZE * TILE_FACTOR * CUBE_S0` 建模，sub-tile 不再复用同一个 FIFO row window。
- P、PV、GU 阶段已经从 QK/P/PV/PV_pend/exp_max/O_parts FIFO/scratch view 显式 `tile.load` 回读 handoff 数据，compile-only IR 中可以同时看到 producer store 和 consumer load。
- `cv_comm_buf` 入口不再是空参数：当前用 ui8 stage marker 写入 QK/P/PV/GU 四个阶段，先让通信控制面在 PTODSL/PTOAS frontend IR 中可见。
- 已加入由 `QK_PRELOAD` 驱动的 prologue/steady/epilogue schedule marker：`10=prologue`、`20=steady`、`30=epilogue`。当前它是 shadow schedule，用来让流水结构先进入 IR，计算仍保留在顺序路径中。
- 已加入 pending consumption drain 的 compile-only marker：`50=qk2sm drain`、`51=sm2pv drain`、`52=pv2gu drain`、`53=ubBuf drain`、`54=pvUbBuf drain`、`55=CV_BLOCK_END`，对应 A5 C++ 主循环后的 TSync/CV 通信尾部清理。
- `FIFO_MODE` 不再是完全空参数：当前用 `60=ALL_GM`、`61=ALL_UB`、`62=QK_PV_UB_ONLY` marker 区分三种 specialization，并加入 compile/frontend 回归。
- 已加入 `kFaLaunchCoreCount=28` 对应的 launch-core cap，并用 `70` marker 表达 `logical_block_idx += launch_block_count` 的 logical-block stride。当前真实计算仍只走当前 block 的顺序路径，stride loop 还是 shadow marker。
- `CAUSAL=True` 不再只是占位参数：当前在 softmax 前用 SIMT scalar loop 对 `kv_col > q_row` 的 score 写 `-inf`，并已加入 compile/frontend 回归。
- 已加入第一层 compile-only 同步标记，对齐 A5 C++ 的 `BUF0_QK_READY=0/1`、`BUF1_SM_READY=2/3`、`UPDATE_READY=4/5`、`UB_BUF_READY=6/7` 四组 flag：QK->P、P->PV、PV->GU、FIFO slot reuse/backpressure 和输出复用边界在 IR 中已经可见。
- compile-only 测试和 PTOAS frontend 小形状验证测试已经加入。
- 为 PTODSL ptr/tile-buffer 过 PTOAS frontend 修了必要的 alias/liveness/type handoff。

compile/frontend 收敛判定：

- PTODSL entry signature 已覆盖 A5 C++ kernel 的 Q/K/V、QK/P/PV/PV_pend/exp_max/O/O_parts/CV comm 指针。
- A5 默认 constexpr 已覆盖：`S0=128`、`HEAD_DIM=128`、`CUBE_S0=128`、`CUBE_S1=128`、`TILE_S1=256`、`QK_PRELOAD=4`、`CV_FIFO_SIZE=8`、`CV_FIFO_CONS_SYNC_PERIOD=4`、`FIFO_MODE=1`。
- QK/P/PV/PV_pend/exp_max/O_parts 的 producer store 和 consumer load 都能在 MLIR/PTO IR 中看到。
- QK/P FIFO 的 `TILE_FACTOR` 维度、row-slice `Vec_S0`、launch cap、shadow schedule、drain、`FIFO_MODE`、causal mask 都有 compile/frontend 回归。
- dev-481211 上 `ptodsl_flash_atten4_port_compile.py`、`ptodsl_flash_atten4_port_frontend_verify.py` 和 manual 默认 artifact 的 `ptoas --emit-pto-ir` 已通过。

## 使用方式

以下命令只生成和验证 PTODSL/PTOAS frontend artifact，不执行 A5 runtime。

在仓库根目录生成 A5 默认形状 MLIR：

```bash
python3 ptodsl/examples/flash_atten4_port.py -o /tmp/fa4_default.mlir
```

用 PTOAS frontend 检查：

```bash
build/tools/ptoas/ptoas /tmp/fa4_default.mlir --emit-pto-ir -o /tmp/fa4_default.pto
```

生成 causal 版本：

```bash
python3 ptodsl/examples/flash_atten4_port.py --causal -o /tmp/fa4_causal.mlir
build/tools/ptoas/ptoas /tmp/fa4_causal.mlir --emit-pto-ir -o /tmp/fa4_causal.pto
```

切换 `FIFO_MODE`：

```bash
python3 ptodsl/examples/flash_atten4_port.py --fifo-mode 0 -o /tmp/fa4_fifo_gm.mlir
python3 ptodsl/examples/flash_atten4_port.py --fifo-mode 1 -o /tmp/fa4_fifo_ub.mlir
python3 ptodsl/examples/flash_atten4_port.py --fifo-mode 2 -o /tmp/fa4_fifo_qk_pv_ub.mlir
```

编译一个更大的 launch-cap 检查形状：

```bash
python3 ptodsl/examples/flash_atten4_port.py \
  --s0 4096 --s1 1024 --cube-s0 128 --cube-s1 128 --tile-s1 256 \
  -o /tmp/fa4_launch_cap.mlir
```

常用回归：

```bash
python3 ptodsl/tests/ptodsl_flash_atten4_port_compile.py
python3 ptodsl/tests/ptodsl_flash_atten4_port_frontend_verify.py
```

CLI 参数说明：

```text
--s0 / --s1 / --head-dim          logical problem shape
--cube-s0 / --cube-s1 / --tile-s1 A5 tiling shape
--qk-preload                      QK preload depth, currently 3 or 4
--cv-fifo-size                    CV FIFO depth, A5 default 8
--cv-fifo-cons-sync-period        consumption sync period, A5 default 4
--fifo-mode                       0=ALL_GM, 1=ALL_UB, 2=QK_PV_UB_ONLY
--causal                          enable compile/frontend causal mask
-o / --output                     output MLIR path, or '-' for stdout
```

`cv_comm_buf` marker 约定：

```text
1/2/3/4   QK/P/PV/GU stage marker
10/20/30  prologue/steady/epilogue shadow schedule
50..55    qk2sm/sm2pv/pv2gu/ubBuf/pvUbBuf/CV_BLOCK_END drain marker
60/61/62  FIFO_MODE=ALL_GM/ALL_UB/QK_PV_UB_ONLY
70        launch logical-block stride marker
```

当前差距：

- 当前同步只是 `pto.sync.set/wait` 级别的编译可见标记，还没有实现 A5 `TSync` 的 allocate/free/record/wait 完整语义，也没有按真实 cube/vector core 拆分执行流。
- `FIFO_MODE` 三种 specialization 已经在 IR 中可见，但还没有改变真实数据路径。
- 当前 FIFO 已经有显式 GM view、store/load 和静态环形 slot offset，但尚未实现真实 cube/vector 并行下的消费反压和生命周期闭环。
- 当前 `cv_comm_buf` 只是 stage marker，不是 A5 `TSyncCVID`/FFTS runtime 控制协议。
- 当前 prologue/steady/epilogue 还是 shadow schedule，尚未把 `compute_qk/compute_p/compute_pv/compute_gu` 真正拆入三段流水执行。
- 当前 launch logical-block stride 还是 shadow marker，尚未把计算主体搬进跨 logical block 的 outer loop。
- A5 C++ 里的 `PV_UB_BUF_READY=8`、`CV_BLOCK_END=10` 已先映射成 `cv_comm_buf` marker；当前 PTODSL 静态 sync facade 限制 event id 在 `[0, 7]`，所以还没有生成对应 sync op。
- `compute_p` 仍是简化的 row loop，不是 A5 `pto_macro_fa_dn_softmax.hpp` 的 DN vector instruction macro。
- `compute_gu` 已经使用 A5 的未归一化 running O 语义，但仍是简化的 SIMT scalar loop，不是 A5 `pto_macro_fa_dn_gu.hpp` 的 vector instruction macro。
- Cube 侧 full-CUBE_S0 matmul 目前先表达成 row-slice matmul，用于降低 compile-only artifact 的 vec 压力；后续要恢复到 A5 的 cube full tile + vector row-slice handoff。
- causal mask 已有 compile/frontend 级实现，但还不是 A5 vector macro 内的高性能 mask 路径。

## 后续开发顺序

1. 保持 compile-only 小形状 frontend 测试，作为 PTODSL/PTOAS handoff 回归。
2. 把 vector 侧 tile 形态改成 A5 row-slice：

```text
qk/prob working tile: [Vec_S0, TILE_S1]
running O / PV tile: [VecGuRows or Vec_S0, HEAD_DIM]
reduce tile: [Vec_S0, 1]
```

3. 将 `compute_p` 从当前简化在线 softmax 改为接近 `pto_macro_fa_dn_softmax.hpp` 的状态更新：

```text
local_max
new_global_max
exp_max = exp((old_global_max - new_global_max) * scale)
x_exp = exp((qk - new_global_max) * scale)
new_global_sum = old_global_sum * exp_max + local_sum
```

4. 将 `compute_gu` 改成 A5 语义：

```text
running_o = running_o * exp_max + pv
final tile: running_o / new_global_sum
```

5. 把当前同步标记推进成真实 `TSync` 语义：

```text
qk2smSync: QK produce / softmax consume, plus FIFO slot reuse backpressure
sm2pvSync: softmax produce / PV consume, plus P FIFO slot reuse backpressure
pv2guSync: PV produce / GU consume, plus PV FIFO slot reuse backpressure
ubBufSync / pvUbBufSync: UB tile reuse boundary
```

6. 把 shadow schedule 推进成真实的三段流水调度：

```text
prologue: preload QK[0..QK_PRELOAD-1]
steady:   compute next QK while draining current P/PV/GU
epilogue: drain remaining PV/GU
```

7. 最后再把 `FIFO_MODE` marker 推进成真实数据路径差异、把 causal mask 下沉到 A5 vector macro 路径、A5 端运行验证。

## 验证策略

dev-481211：

- `python3 ptodsl/tests/ptodsl_flash_atten4_port_compile.py`
- `python3 ptodsl/tests/ptodsl_flash_atten4_port_frontend_verify.py`
- `ptoas --emit-pto-ir` 检查资源是否下降到可接受范围。

A5 `192.168.1.52`：

- 只在 `/root/zjm/` 下创建独立实验目录。
- 先用 dev-481211 构建出的后端产物，避免 A5 内存不足。
- 先跑小 case 正确性，再跑 manual 默认 case。

A3 `101.245.68.6`：

- 只用于兼容性验证，不作为 A5 `flash_atten4` 移植源。
