# `acc_store_fix` 结构化接口设计草案

## 1. 目标

当前 `acc_store_fix` 的参数列表是平铺的：

```mlir
pto.acc_store_fix %src, %dst, %m, %n, %src_stride, %dst_stride,
                  %unit_flag_ctrl, %quant_pre, %relu_pre_mode,
                  nz2nd|nz2dn(%loop0_src_stride)?|nz2nz(%split)?
  loop3(%count, %src_stride, %dst_stride)?
```

这套接口的问题不是能力不够，而是可读性差：

- `shape` / `stride` / `mode ctrl` / `loop` 混在一起
- `nz2dn` 和 `nz2nz` 的模式附加参数没有单独成组
- 从语法上看，不容易快速区分“主语义参数”和“模式特有参数”

本文提出一版结构化接口草案，目标是把 `acc_store_fix` 设计得更像 `cube_load_frac`：

- 主参数按语义分组
- mode 自身仍然保留为 `nz2nd` / `nz2dn` / `nz2nz`
- 只在需要时暴露 mode-specific 子参数
- 不改变现有底层能力边界

本文只讨论 VPTO 接口设计，不讨论 release 文档写法，也不讨论 LLVM emitter 细节。

## 2. 设计原则

### 2.1 分组优先

接口按以下语义分组：

- 源/目的地址
- 基本写回形状
- 基本源/目的步长
- 写回控制字段
- 布局模式
- 外层循环

### 2.2 mode 参数只在对应模式下出现

- `nz2nd` 不需要附加参数
- `nz2dn` 只需要 `loop0_src_stride`
- `nz2nz` 只需要 `split`，并且不允许 `loop3(...)`

### 2.3 保持和现有 lowering 一致

当前 `acc_store_fix` expand 的核心语义已经比较清楚：

- `loop3(...)` 作为特殊硬件 loop，进入 `set_loop3_para`
- `nz2dn(loop0_src_stride)` 进入 `set_channel_para`
- `nz2nz(split)` 作为 `channel_split_en`
- 最终落到 `copy_matrix_cc_to_gm`

因此结构化接口只重组表达方式，不额外发明新的语义。

## 3. 建议语法

建议把接口改成下面这种形式：

```mlir
pto.acc_store_fix %src, %dst,
    shape(%m, %n),
    dst_layout(%src_stride, %dst_stride),
    ctrl(%unit_flag_ctrl, %quant_pre, %relu_pre_mode),
    nz2nd | nz2dn(%loop0_src_stride) | nz2nz(%split)
    loop3(%count, %src_stride, %dst_stride)?
```

对应类型区写成：

```mlir
: !pto.ptr<..., acc>, !pto.ptr<..., gm>,
  shape i64, i64,
  dst_layout(i64, i64),
  ctrl i64, i64, i64,
  nz2nd | nz2dn(i64) | nz2nz(i64)
  [, loop3 i64, i64, i64]
```

## 4. 各分组语义

### 4.1 地址

```mlir
%src, %dst
```

- `%src`
  - ACC/L0C 中待写回结果的起始地址
- `%dst`
  - GM 中写回目标的起始地址

### 4.2 `shape(%m, %n)`

```mlir
shape(%m, %n)
```

- `%m`
  - 当前一次基础写回所覆盖的结果行数
- `%n`
  - 当前一次基础写回所覆盖的结果列数

这里描述的是“这一块结果有多大”，不描述 layout 转换模式。

### 4.3 `dst_layout(%src_stride, %dst_stride)`

```mlir
dst_layout(%src_stride, %dst_stride)
```

- `%src_stride`
  - ACC/L0C 源端相邻写回步之间的源步长配置
- `%dst_stride`
  - GM 目标端相邻写回步之间的目标步长配置

之所以叫 `dst_layout`，是因为从用户角度看，这两个 stride 一起描述了“这块结果如何落到目标布局里”。  
虽然其中一个来自源端，但在接口使用时，这一组参数通常是围绕目标写回布局一起思考的。

如果你觉得这个命名不够准，也可以改成更中性的：

```mlir
strides(%src_stride, %dst_stride)
```

这两种中我倾向于：

- 对外接口先用 `strides(...)`
- 文档解释中再强调一个是 ACC 源步长，一个是 GM 目标步长

所以更稳妥的版本也可以是：

```mlir
pto.acc_store_fix %src, %dst,
    shape(%m, %n),
    strides(%src_stride, %dst_stride),
    ctrl(%unit_flag_ctrl, %quant_pre, %relu_pre_mode),
    ...
```

## 4.4 `ctrl(%unit_flag_ctrl, %quant_pre, %relu_pre_mode)`

```mlir
ctrl(%unit_flag_ctrl, %quant_pre, %relu_pre_mode)
```

- `%unit_flag_ctrl`
  - 底层写回控制字段中的 unit-flag 相关部分
- `%quant_pre`
  - 写回前的 quant 配置
- `%relu_pre_mode`
  - 写回前的 relu 配置

这三个值本质上都是“写回控制位”，放在一个组里最自然。

### 4.5 mode

#### `nz2nd`

```mlir
nz2nd
```

- 表示把 ACC/L0C 中的 NZ/fractal 结果按 ND 语义写回到 GM
- 无附加参数

#### `nz2dn(%loop0_src_stride)`

```mlir
nz2dn(%loop0_src_stride)
```

- 表示把 ACC/L0C 中的 NZ/fractal 结果按 DN 语义写回到 GM
- `%loop0_src_stride`
  - mode 专属参数
  - 对应当前 expand 里的 `channelLoop0Stride`
  - 语义上就是 `CHANNEL_PARA` 里的 loop0 source stride

这个参数不属于通用 stride，也不属于通用 ctrl，所以放在 mode 自己名下是合适的。

#### `nz2nz(%split)`

```mlir
nz2nz(%split)
```

- 表示把 ACC/L0C 中的 NZ/fractal 结果继续按 NZ 语义写回
- `%split`
  - mode 专属参数
  - 对应当前 expand 里的 `channelSplitEn`
  - 用于控制 split NZ 写回行为

## 4.6 `loop3(%count, %src_stride, %dst_stride)?`

```mlir
loop3(%count, %src_stride, %dst_stride)?
```

- `%count`
  - 当前外层循环次数
- `%src_stride`
  - 每次外层迭代时，ACC/L0C 源地址的增量
- `%dst_stride`
  - 每次外层迭代时，GM 目标地址的增量

- 这是 `acc_store_fix` 自己的特殊 loop 语义
- 最多只允许一组
- 这组参数直接进入硬件 `LOOP3_PARA`

## 5. 建议 Builder 结构

如果后面要真正改 IR builder，建议也和 `cube_load_frac` 保持同样风格，拆成几个小结构：

```cpp
struct AccStoreFixShapeConfig {
  Value m;
  Value n;
};

struct AccStoreFixStrideConfig {
  Value srcStride;
  Value dstStride;
};

struct AccStoreFixCtrlConfig {
  Value unitFlagCtrl;
  Value quantPre;
  Value reluPreMode;
};

struct AccStoreFixModeConfig {
  AccStoreFixMode mode;
  std::optional<Value> split;
  std::optional<Value> loop0SrcStride;
};
```

然后 builder 形式变成：

```cpp
build(builder, state,
      source, destination,
      shapeConfig,
      strideConfig,
      ctrlConfig,
      modeConfig,
      loop3);
```

这样和 `cube_load_frac` 的 builder 组织会明显更一致。

## 6. 例子

### 6.1 `nz2nd`

```mlir
pto.acc_store_fix %acc, %gm,
    shape(%m, %n),
    strides(%src_stride, %dst_stride),
    ctrl(%unit_flag_ctrl, %quant_pre, %relu_pre_mode),
    nz2nd
    loop3(%count, %src_loop_stride, %dst_loop_stride)
```

### 6.2 `nz2dn`

```mlir
pto.acc_store_fix %acc, %gm,
    shape(%m, %n),
    strides(%src_stride, %dst_stride),
    ctrl(%unit_flag_ctrl, %quant_pre, %relu_pre_mode),
    nz2dn(%loop0_src_stride)
    loop3(%count, %src_loop_stride, %dst_loop_stride)
```

### 6.3 `nz2nz`

```mlir
pto.acc_store_fix %acc, %gm,
    shape(%m, %n),
    strides(%src_stride, %dst_stride),
    ctrl(%unit_flag_ctrl, %quant_pre, %relu_pre_mode),
    nz2nz(%split)
```

`nz2nz` 不允许带 `loop3(...)`。

## 7. 和当前接口的关系

这版草案不要求底层能力变化，只要求：

- 自定义 parser / printer 改为分组语法
- builder 参数改成分组结构
- 现有 `split` / `loop0_src_stride` 继续保留，但只在各自 mode 下出现

也就是说，这更像一次“接口整理”，不是功能扩张。

## 8. 建议

我建议下一步按下面顺序推进：

1. 先只改 `acc_store_fix` 的 parser / printer / builder 形态
2. 保持 expand 和 emitter 逻辑不变
3. 文档同步改成结构化语法
4. 原有测试按新语法重写

这样风险最小，也最容易验证“只是接口重组，没有语义漂移”。
