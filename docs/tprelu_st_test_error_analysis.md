# tprelu ST 测试错误分析报告

## 1. 问题概述

运行 `tprelu` ST 测试时遇到编译失败问题，本文档详细分析错误原因、tprelu 操作逻辑以及解决方案。

测试命令：
```bash
python3 test/tilelang_st/script/run_st.py -r sim -v a5 -t tprelu &> testST_tsel11.log
```

---

## 2. 错误分析

### 2.1 第一个错误：`unknown loc: ub`

**错误信息：**
```
loc("tprelu.pto":49:14):
error: unknown loc: ub

Error: Failed to parse MLIR.
```

**原因分析：**

在 `PTOTypeDefs.cpp:394-403` 中，`tile_buf` 类型的 `loc` 属性支持以下值：
- `mat` → AddressSpace::MAT
- `left` → AddressSpace::LEFT
- `right` → AddressSpace::RIGHT
- `acc` → AddressSpace::ACC
- `vec` → AddressSpace::VEC
- `bias` → AddressSpace::BIAS
- `scaling` → AddressSpace::SCALING

**不支持 `ub`！** 虽然 `pto.ptr<f16, ub>` 中可以使用 `ub`，但 `tile_buf` 的 `loc` 属性不支持。

**解决方案：**

将 `tprelu.pto` 文件中所有 `loc=ub` 替换为 `loc=vec`（两者都映射到 `AddressSpace::VEC`）。

```bash
# 替换命令
sed -i 's/loc=ub/loc=vec/g' test/tilelang_st/npu/a5/src/st/testcase/tprelu/tprelu.pto
```

---

### 2.2 第二个错误：VEC 内存溢出

**错误信息：**
```
loc("tprelu.pto":220:3):
error: vec overflow, requires 2097152 bits while 2031616 bits avaliable!

Error: Pass execution failed.
```

**原因分析：**

VEC（Vector Buffer）是 NPU 芯上的高速缓存，有固定的硬件容量限制：
- **可用容量：2031616 bits ≈ 248KB**

以 `TPRELU_f32_2048x8` 函数为例，分配了 4 个 tile buffer：

| Buffer | 尺寸 | 计算 |
|--------|------|------|
| src0_tile | 2048×8×f32 | 2048 × 8 × 32 = 524288 bits |
| src1_tile | 2048×8×f32 | 524288 bits |
| tmp_tile | 2048×8×f32 | 524288 bits |
| dst_tile | 2048×8×f32 | 524288 bits |
| **总计** | | **2097152 bits = 256KB** |

256KB > 248KB，导致内存溢出。

**所有测试 Case 的内存需求分析：**

| Case | 数据类型 | Tile尺寸 | 单buffer大小 | 4个buffer总需求 | 状态 |
|------|----------|----------|-------------|-----------------|------|
| f16_64x64 | f16 | 64×64 | 64×64×16 = 65536 bits | 262144 bits = 32KB | ✅ |
| f16_64x64_partial | f16 | 64×64 | 65536 bits | 262144 bits | ✅ |
| f16_1x16384 | f16 | 1×16384 | 262144 bits | 1048576 bits = 128KB | ✅ |
| f16_2048x16 | f16 | 2048×16 | 524288 bits | **2097152 bits = 256KB** | ❌ |
| f32_64x64 | f32 | 64×64 | 131072 bits | 524288 bits = 64KB | ✅ |
| f32_64x64_partial | f32 | 64×64 | 131072 bits | 524288 bits | ✅ |
| f32_1x16384 | f32 | 1×16384 | 524288 bits | **2097152 bits = 256KB** | ❌ |
| f32_2048x8 | f32 | 2048×8 | 524288 bits | **2097152 bits = 256KB** | ❌ |

**溢出的 Case：**
- `f16_2048x16`
- `f32_1x16384`
- `f32_2048x8`

---

## 3. tprelu 操作详解

### 3.1 PReLU 算法定义

**PReLU (Parametric ReLU)** 是带参数的 ReLU 激活函数：

```
output[i] = input[i] >= 0 ? input[i] : slope[i] * input[i]
```

即：
- 当元素 ≥ 0 时，保持原值
- 当元素 < 0 时，乘以对应的斜率系数 `slope[i]`

### 3.2 测试参数

| 参数 | 含义 |
|------|------|
| src0 | 输入数据（input） |
| src1 | 斜率参数（slope），每个元素对应一个斜率值 |
| dst | 输出结果 |

### 3.3 处理流程

以 Case 0 (`TPRELU_f16_64x64`) 为例：

```
┌─────────────────────────────────────────────────────────────────┐
│                    GM (Global Memory - 外部存储)                 │
│  src0_ptr ──► [1x1x1x64x64 f16]  (输入数据)                     │
│  src1_ptr ──► [1x1x1x64x64 f16]  (斜率参数)                     │
│  dst_ptr  ──► [1x1x1x64x64 f16]  (输出结果)                     │
└─────────────────────────────────────────────────────────────────┘
          │ tload (从GM加载到VEC)           │ tload
          ▼                                  ▼
┌─────────────────────────────────────────────────────────────────┐
│                    VEC (Vector Buffer - 片上缓存)               │
│  src0_tile ──► [64x64 f16]                                      │
│  src1_tile ──► [64x64 f16]                                      │
│  tmp_tile   ──► [64x64 f16] (临时缓冲)                          │
│  dst_tile   ──► [64x64 f16]                                     │
│                                                                 │
│  ┌─────────────────────────────────────────┐                   │
│  │         tprelu 计算                      │                   │
│  │  dst = src0 >= 0 ? src0 : src1 * src0   │                   │
│  │  (tmp 用于中间计算)                      │                   │
│  └─────────────────────────────────────────┘                   │
└─────────────────────────────────────────────────────────────────┘
          │ tstore (从VEC写回GM)
          ▼
┌─────────────────────────────────────────────────────────────────┐
│                    GM (Global Memory)                           │
│  dst_ptr ──► 结果数据                                           │
└─────────────────────────────────────────────────────────────────┘
```

### 3.4 MLIR 代码结构

```mlir
// 1. 创建 Tensor View（5维张量视图）
%src0_view = pto.make_tensor_view %src0_ptr,
  shape = [%c1, %c1, %c1, %c64, %c64],
  strides = [%c4096, %c4096, %c4096, %c64, %c1]
  : !pto.tensor_view<1x1x1x64x64xf16>

// 2. 创建 Partition（分区，取64×64的区域）
%src0_part = pto.partition_view %src0_view,
  offsets = [%c0, %c0, %c0, %c0, %c0],
  sizes = [%c1, %c1, %c1, %c64, %c64]

// 3. 分配 Tile Buffer（片上缓存）
%src0_tile = pto.alloc_tile
  : !pto.tile_buf<loc=vec, dtype=f16, rows=64, cols=64, v_row=64, v_col=64,
                  blayout=row_major, slayout=none_box, fractal=512, pad=0>
// ... 其他 tile buffer

// 4. 执行操作
pto.tload ins(%src0_part) outs(%src0_tile)      // 从GM加载到VEC
pto.tload ins(%src1_part) outs(%src1_tile)
pto.tprelu ins(%src0_tile, %src1_tile, %tmp_tile) outs(%dst_tile)  // 计算
pto.tstore ins(%dst_tile) outs(%dst_part)       // 从VEC写回GM
```

### 3.5 Tile Buffer 参数说明

```mlir
!pto.tile_buf<loc=vec, dtype=f16, rows=64, cols=64, v_row=64, v_col=64,
              blayout=row_major, slayout=none_box, fractal=512, pad=0>
```

| 参数 | 含义 |
|------|------|
| `loc=vec` | 存储位置：VEC缓冲区 |
| `dtype=f16` | 数据类型：16位浮点 |
| `rows=64, cols=64` | 物理尺寸：64行×64列 |
| `v_row=64, v_col=64` | 有效尺寸：全部有效（无padding） |
| `blayout=row_major` | 内存布局：行优先 |
| `slayout=none_box` | 存储布局：无分块 |
| `fractal=512` | 分块大小：512元素 |
| `pad=0` | 填充值：无填充 |

---

## 4. 为什么需要 4 个 Tile Buffer

`tprelu` 操作需要 4 个 buffer：

| Buffer | 用途 |
|--------|------|
| **src0_tile** | 存放输入数据（input） |
| **src1_tile** | 存放斜率参数（slope） |
| **tmp_tile** | 临时计算缓冲（中间结果） |
| **dst_tile** | 存放输出结果 |

所有 buffer 同时存在于 VEC 中，因为 `tprelu` 是一个 tile-level 操作，需要同时访问所有输入进行计算。

---

## 5. pto-isa 与 PTOAS 的内存管理差异

### 5.1 pto-isa 的解决方案：手动内存地址分配 + 内存复用

查看 `/home/wujiajun/llvm-workspace/pto-isa/tests/npu/a5/src/st/testcase/tprelu/tprelu_kernel.cpp`：

```cpp
template <typename T, int kTRows_, int kTCols_, int vRows, int vCols>
__global__ AICORE void runTPrelu(__gm__ T __out__ *out, __gm__ T __in__ *src0, __gm__ T __in__ *src1)
{
    using TileData = Tile<TileType::Vec, T, kTRows_, kTCols_, BLayout::RowMajor, -1, -1>;
    
    TileData src0Tile(vRows, vCols);
    TileData src1Tile(vRows, vCols);
    TileData dstTile(vRows, vCols);
    TileData tmpTile(vRows, vCols);
    
    // 关键：手动指定内存地址
    TASSIGN(src0Tile, 0x0);       // src0Tile @ 地址 0x0
    TASSIGN(src1Tile, 0x10000);   // src1Tile @ 地址 0x10000 (64KB偏移)
    TASSIGN(dstTile, 0x20000);    // dstTile  @ 地址 0x20000 (128KB偏移)
    TASSIGN(tmpTile, 0x0);        // tmpTile  @ 地址 0x0 ← 复用 src0Tile 的内存！
    
    // 执行操作
    TLOAD(src0Tile, src0Global);
    TLOAD(src1Tile, src1Global);
    TPRELU(dstTile, src0Tile, src1Tile, tmpTile);  // tmp 复用 src0 的空间
    TSTORE(dstGlobal, dstTile);
}
```

### 5.2 内存布局分析（以 f32_2048x8 为例）

每个 tile：`2048 × 8 × 4 bytes = 64KB`

| Tile | 地址偏移 | 实际占用 |
|------|---------|---------|
| src0Tile | 0x0 | 64KB |
| src1Tile | 0x10000 (64KB) | 64KB |
| dstTile | 0x20000 (128KB) | 64KB |
| tmpTile | 0x0 | **复用 src0Tile 的内存** |

**总共只需要 192KB**，而不是 256KB！

### 5.3 为什么可以复用？

`tprelu` 的执行顺序：

```
1. TLOAD(src0Tile, src0Global)  → 加载 src0 到 src0Tile
2. TLOAD(src1Tile, src1Global)  → 加载 src1 到 src1Tile
3. TPRELU(dstTile, src0Tile, src1Tile, tmpTile) → 计算
   - src0Tile 只作为输入读取
   - tmpTile 用于中间临时计算
   - 这两个 buffer 的生命周期不冲突，可以复用！
4. TSTORE(dstGlobal, dstTile)   → 写回结果
```

在 TPRELU 计算阶段：
- `src0Tile` 的值只需要**读取**，不再写入
- `tmpTile` 只在计算过程中**写入临时结果**
- 两者生命周期不重叠，可以安全地共用同一块内存

### 5.4 PTOAS 的情况：自动独立分配

PTOAS 使用 MLIR 的 `memref.alloc()` 来分配内存，每个 buffer 都是**独立分配**：

```mlir
%alloc = memref.alloc() : memref<2048x8xf32, ...>   // 独立分配 src0_tile
%alloc_8 = memref.alloc() : memref<2048x8xf32, ...>  // 独立分配 src1_tile
%alloc_11 = memref.alloc() : memref<2048x8xf32, ...> // 独立分配 tmp_tile
%alloc_14 = memref.alloc() : memref<2048x8xf32, ...> // 独立分配 dst_tile
```

**没有考虑 buffer 之间的生命周期复用**。

### 5.5 对比总结

| 特性 | pto-isa | PTOAS |
|------|---------|-------|
| 内存分配方式 | **手动指定地址**（TASSIGN） | **自动独立分配**（memref.alloc） |
| 内存复用 | **用户手动控制** | **无复用机制** |
| 大尺寸 case | 可复用内存，通过测试 | 独立分配导致溢出 |
| 灵活性 | 高（用户完全控制） | 低（依赖编译器优化） |
| 易用性 | 低（需要手动计算地址） | 高（自动化分配） |

---

## 6. 解决方案

### 6.1 方案一：添加内存复用 Pass（推荐）

在 PTOAS 编译器中添加一个 **Buffer Lifetime Analysis Pass**：

1. 分析每个 tile buffer 的生命周期
2. 找出生命周期不重叠的 buffer
3. 让它们共用同一块内存空间

**实现思路：**

```
┌─────────────────────────────────────────────────────────────┐
│                  Buffer Lifetime Analysis                   │
├─────────────────────────────────────────────────────────────┤
│  1. 构建操作依赖图                                           │
│     - 每个操作读取哪些 buffer                                │
│     - 每个操作写入哪些 buffer                                │
│                                                             │
│  2. 计算每个 buffer 的生命周期                               │
│     - 开始：第一次写入或最后一次需要读取                      │
│     - 结束：最后一次读取                                      │
│                                                             │
│  3. 找出生命周期不重叠的 buffer 对                           │
│     - buffer A 结束后，buffer B 才开始                       │
│     - 可以让 A 和 B 共用同一块内存                           │
│                                                             │
│  4. 合并内存分配                                             │
│     - 用同一个 memref.alloc 替换多个 alloc                   │
│     - 使用 view/subview 来切片使用                          │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 方案二：手动指定地址语法扩展

在 MLIR/PTO DSL 层面支持手动指定 tile buffer 地址：

```mlir
// 方式1：直接指定地址
%src0_tile = pto.alloc_tile at_address(0x0) 
  : !pto.tile_buf<loc=vec, dtype=f32, rows=2048, cols=8, ...>

%src1_tile = pto.alloc_tile at_address(0x10000)
  : !pto.tile_buf<loc=vec, dtype=f32, rows=2048, cols=8, ...>

// 方式2：复用现有 buffer
%tmp_tile = pto.alloc_tile reuse(%src0_tile)
  : !pto.tile_buf<loc=vec, dtype=f32, rows=2048, cols=8, ...>
```

### 6.3 方案三：修改测试用例（临时方案）

直接修改 `.pto` 文件，删除或减小溢出的 case：

**选项 A：删除溢出的 case**

删除以下函数：
- `TPRELU_f16_2048x16`
- `TPRELU_f32_1x16384`
- `TPRELU_f32_2048x8`

**选项 B：减小 tile 尺寸**

将大尺寸 tile 拆分成小块，循环处理：

```mlir
// 原来：一次性处理 2048x16
// 改为：分 32 次，每次处理 64x16
// 使用循环结构迭代处理
```

**选项 C：使用其他内存空间**

将某些 buffer 改用 `loc=mat`（MAT 缓冲区容量更大）：

```mlir
%tmp_tile = pto.alloc_tile
  : !pto.tile_buf<loc=mat, dtype=f32, rows=2048, cols=8, ...>  // 使用 MAT 而非 VEC
```

---

## 7. 验证步骤

修复后需要验证：

1. **编译验证**：确保 MLIR 能成功解析和编译
2. **内存验证**：确保所有 tile buffer 总大小不超过 VEC 容量
3. **运行验证**：确保生成的 kernel 能正确执行
4. **精度验证**：确保输出结果与 golden 数据一致

---

## 8. 相关文件

| 文件路径 | 说明 |
|----------|------|
| `test/tilelang_st/npu/a5/src/st/testcase/tprelu/tprelu.pto` | PTO MLIR 测试文件 |
| `test/tilelang_st/npu/a5/src/st/testcase/tprelu/main.cpp` | Host 主程序 |
| `test/tilelang_st/npu/a5/src/st/testcase/tprelu/launch.cpp` | Kernel 启动封装 |
| `lib/PTO/IR/PTOTypeDefs.cpp` | tile_buf 类型解析实现 |
| `lib/PTO/IR/PTO.cpp` | PTO 指令实现 |

---

## 9. 参考资料

- pto-isa tprelu 实现：`/home/wujiajun/llvm-workspace/pto-isa/tests/npu/a5/src/st/testcase/tprelu/`
- pto-isa Tile 类定义：`/home/wujiajun/llvm-workspace/pto-isa/include/pto/common/pto_tile.hpp`
- pto-isa TASSIGN 实现：`/home/wujiajun/llvm-workspace/pto-isa/include/pto/npu/a2a3/TAssign.hpp`