# 2. DMA Copy Programming

> **Category:** DMA transfer configuration and execution
> **Pipelines:** MTE2 (GM→UB), MTE3 (UB→GM)

DMA transfers move data between Global Memory (GM) and Unified Buffer (UB). The MTE engines operate asynchronously from the Vector core, requiring explicit sync (see [Pipeline Sync](01-pipeline-sync.md)).

This document describes the public grouped MTE interfaces. Public op names use
the canonical form `pto.mte_<src>_<dst><_mx>`, where `<src>` and `<dst>` are PTO
address-space tokens (`gm`, `ub`, `mat`, `left`, `right`, `acc`, `bias`).
`mat` denotes the L1/CBUF matrix buffer address space.

- `pto.mte_gm_ub`
- `pto.mte_ub_gm`
- `pto.mte_ub_ub`
- `pto.mte_ub_mat`
- `pto.mte_gm_mat`
- `pto.mte_mat_ub`
- `pto.mte_mat_bias`
- `pto.mte_mat_left`
- `pto.mte_mat_right`
- `pto.mte_mat_left_mx`
- `pto.mte_mat_right_mx`
- `pto.mte_acc_mat`
- `pto.mte_acc_gm`
- `pto.mte_acc_ub`

This chapter covers the public grouped MTE interfaces and cube-side load/store
bridge wrappers. The legacy raw copy family remains documented separately; in
particular, `pto.copy_ubuf_to_ubuf` shares the same UB→UB copy contract as
`pto.mte_ub_ub` but remains a legacy surface op.

---

## MTE Transfer Execution

### `pto.mte_gm_ub`

- **syntax:**
```mlir
pto.mte_gm_ub %gm_src, %ub_dst, %l2_cache_ctl, %len_burst
  nburst(%n_burst, %src_stride, %dst_stride)
  [loop(%loop_count, %loop_src_stride, %loop_dst_stride)]*
  [pad(%pad_value[, %left_padding_count, %right_padding_count])]
  : !pto.ptr<T, gm>, !pto.ptr<T, ub>, i64, i64, i64,
    [loop i64, i64, i64,]*
    [pad T[, i64, i64]]
```
- **semantics:** Grouped GM→UB DMA transfer. `nburst(...)` defines the innermost repeated burst transfer, optional `loop(...)` groups add outer repetition levels, and `pad(...)` controls UB row padding.

**Parameter Table:**

| Parameter | Width | Description |
|-----------|-------|-------------|
| `%gm_src` | ptr | GM source pointer (`!pto.ptr<T, gm>`) |
| `%ub_dst` | ptr | UB destination pointer (`!pto.ptr<T, ub>`, 32B-aligned) |
| `%l2_cache_ctl` | 2 bits | L2 cache allocate control |
| `%len_burst` | 16 bits | Contiguous bytes transferred per burst row |
| `nburst(%n_burst, %src_stride, %dst_stride)` | 16 bits / 40 bits / 21 bits | Required innermost burst group: count, GM source stride, UB destination stride |
| `loop(%loop_count, %loop_src_stride, %loop_dst_stride)` | 21 bits / 40 bits / 21 bits | Optional outer repetition group: count, GM source stride, UB destination stride |
| `pad(%pad_value[, %left_padding_count, %right_padding_count])` | scalar / 8 bits / 8 bits | Optional padding: fill value, optional left padding count, optional right padding count |

**Constraints:**

- `nburst(...)` is always required.
- Each `loop(...)` group must be provided as a complete triple when present.
- `nburst(...)` is the innermost group.
- `loop(...)` groups are ordered from inner to outer.
- The first `loop(...)` group wraps `nburst(...)`.
- Each additional `loop(...)` group wraps all earlier groups.
- `pad(...)` may contain only `%pad_value`; omitted left and right padding counts default to 0.
- If either left or right padding count is provided, both counts must be provided.
- `pad(...)` is independent of the optional `loop(...)` groups.
- A DMA load may use `nburst(...) pad(...)` without any `loop(...)` group.

**Example:**

```mlir
pto.mte_gm_ub %gm_in, %ub_out, %cache, %len_burst
  nburst(%rows, %gm_row_stride, %ub_row_stride)
  loop(%tiles, %gm_tile_stride, %ub_tile_stride)
  pad(%pad)
  : !pto.ptr<f16, gm>, !pto.ptr<f16, ub>, i64, i64,
    loop i64, i64, i64, pad f16
```

---

### `pto.mte_ub_gm`

- **syntax:**
```mlir
pto.mte_ub_gm %ub_src, %gm_dst, %len_burst
  nburst(%n_burst, %src_stride, %dst_stride)
  [loop(%loop_count, %loop_src_stride, %loop_dst_stride)]*
  : !pto.ptr<T, ub>, !pto.ptr<T, gm>, i64, i64, i64, i64,
    [loop i64, i64, i64,]*
```
- **semantics:** Grouped UB→GM DMA transfer. `nburst(...)` defines the innermost repeated burst transfer, and optional `loop(...)` groups add outer repetition levels.

**Parameter Table:**

| Parameter | Width | Description |
|-----------|-------|-------------|
| `%ub_src` | ptr | UB source pointer (`!pto.ptr<T, ub>`, 32B-aligned) |
| `%gm_dst` | ptr | GM destination pointer (`!pto.ptr<T, gm>`) |
| `%len_burst` | 16 bits | Contiguous bytes transferred per burst row |
| `nburst(%n_burst, %src_stride, %dst_stride)` | 16 bits / 21 bits / 40 bits | Required innermost burst group: count, UB source stride, GM destination stride |
| `loop(%loop_count, %loop_src_stride, %loop_dst_stride)` | 21 bits / 21 bits / 40 bits | Optional outer repetition group: count, UB source stride, GM destination stride |

**Constraints:**

- `nburst(...)` is always required.
- Each `loop(...)` group must be provided as a complete triple when present.
- `nburst(...)` is the innermost group.
- `loop(...)` groups are ordered from inner to outer.
- The first `loop(...)` group wraps `nburst(...)`.
- Each additional `loop(...)` group wraps all earlier groups.

**Example:**

```mlir
pto.mte_ub_gm %ub_in, %gm_out, %len_burst
  nburst(%rows, %ub_row_stride, %gm_row_stride)
  loop(%tiles, %ub_tile_stride, %gm_tile_stride)
  loop(%batches, %ub_batch_stride, %gm_batch_stride)
  : !pto.ptr<f16, ub>, !pto.ptr<f16, gm>, i64, i64, i64, i64,
    loop i64, i64, i64, loop i64, i64, i64
```

---

### `pto.mte_ub_ub`

- **syntax:**
```mlir
pto.mte_ub_ub %ub_src, %ub_dst, %len_burst
  nburst(%n_burst, %src_gap, %dst_gap)
  : !pto.ptr<T, ub>, !pto.ptr<T, ub>, i64, i64, i64, i64
```
- **semantics:** Grouped UB→UB raw copy.

**Parameter Table:**

| Parameter | Width | Description |
|-----------|-------|-------------|
| `%ub_src` | ptr | UB source pointer (`!pto.ptr<T, ub>`, 32B-aligned) |
| `%ub_dst` | ptr | UB destination pointer (`!pto.ptr<T, ub>`, 32B-aligned) |
| `%len_burst` | 16 bits | Burst length in units of 32 bytes |
| `nburst(%n_burst, %src_gap, %dst_gap)` | 16 bits / 16 bits / 16 bits | Required copy burst group: count, source gap, destination gap |

**Constraints:**

- UB source and destination addresses must be 32B-aligned.
- `%len_burst`, `%src_gap`, and `%dst_gap` are encoded in units of 32 bytes.

**Example:**

```mlir
pto.mte_ub_ub %ub_src, %ub_dst, %len32b
  nburst(%rows, %src_gap, %dst_gap)
  : !pto.ptr<i16, ub>, !pto.ptr<i16, ub>, i64, i64, i64, i64
```

---

### `pto.mte_ub_mat`

- **syntax:**
```mlir
pto.mte_ub_mat %ub_src, %mat_dst, %len_burst
  nburst(%n_burst, %src_gap, %dst_gap)
  : !pto.ptr<T, ub>, !pto.ptr<T, mat>, i64, i64, i64, i64
```
- **semantics:** Grouped UB→L1 (`cbuf` / `mat`) raw copy.

**Parameter Table:**

| Parameter | Width | Description |
|-----------|-------|-------------|
| `%ub_src` | ptr | UB source pointer (`!pto.ptr<T, ub>`, 32B-aligned) |
| `%mat_dst` | ptr | L1 destination pointer (`!pto.ptr<T, mat>`, 32B-aligned) |
| `%len_burst` | 16 bits | Burst length in units of 32 bytes |
| `nburst(%n_burst, %src_gap, %dst_gap)` | 16 bits / 16 bits / 16 bits | Required copy burst group: count, source gap, destination gap |

**Constraints:**

- UB source and MAT destination addresses must be 32B-aligned.
- `%len_burst`, `%src_gap`, and `%dst_gap` are encoded in units of 32 bytes.

**Example:**

```mlir
pto.mte_ub_mat %ub_src, %mat_dst, %len32b
  nburst(%rows, %src_gap, %dst_gap)
  : !pto.ptr<i16, ub>, !pto.ptr<i16, mat>, i64, i64, i64, i64
```

---

## Cube Bridge Wrapper Ops

These wrappers move data around the cube matmul memory hierarchy: GM, L1
(`cbuf` / `mat`), L0A (`left`), L0B (`right`), L0C (`acc`), UB, and bias-table
buffers. Cube matrix multiply compute ops remain documented in
[Cube Matrix Multiply (MAT)](16-cube-matmul.md).

### `pto.mte_gm_mat` Burst Form

- **syntax:**
```mlir
pto.mte_gm_mat %src, %dst, %len_burst
  nburst(%count, %src_stride, %dst_stride)
  [loop(%count_i, %src_stride_i, %dst_stride_i)]*
  : !pto.ptr<T, gm>, !pto.ptr<T, mat>, i64, i64, i64, i64
```
- **semantics:** Structured GM-to-L1 (`cbuf`) wrapper.

**Parameter Table:** `%src`, `%dst`, `%len_burst`, `nburst(...)`, optional `loop(...)`.

**Constraints:**

- Wrapper lowers to loop/stride setup plus `pto.copy_gm_to_cbuf`.

**Example:**

```mlir
pto.mte_gm_mat %a_gm, %l1_a, %c16_i64
  nburst(%c1_i64, %c0_i64, %c0_i64)
  : !pto.ptr<f16, gm>, !pto.ptr<f16, mat>, i64, i64, i64, i64
```

---

### `pto.mte_mat_ub`

- **syntax:**
```mlir
pto.mte_mat_ub %src, %dst, %len_burst
  nburst(%count, %src_stride, %dst_stride)
  [loop(%count_i, %src_stride_i, %dst_stride_i)]*
  : !pto.ptr<T, mat>, !pto.ptr<T, ub>, i64, i64, i64, i64
```
- **semantics:** Structured L1 (`cbuf`) to UB wrapper.

**Parameter Table:** `%src`, `%dst`, `%len_burst`, `nburst(...)`, optional `loop(...)`.

**Constraints:**

- Wrapper lowers to `pto.copy_cbuf_to_ubuf` and optional outer loops.

**Example:**

```mlir
pto.mte_mat_ub %l1_src, %ub_dst, %c16_i64
  nburst(%c1_i64, %c0_i64, %c0_i64)
  : !pto.ptr<f16, mat>, !pto.ptr<f16, ub>, i64, i64, i64, i64
```

---

### `pto.mte_gm_mat` Fractal Form

- **syntax:**
```mlir
pto.mte_gm_mat %src, %dst, nd2nz|dn2nz, shape(%n_value, %d_value), src_layout(%src_inner_stride[, %src_outer_stride]), dst_group(%group_count, %dst_loop2_stride, %dst_loop3_stride, %dst_loop4_stride), ctrl(%l2_cache_ctrl, %smallc0_en)
  : !pto.ptr<T, gm>, !pto.ptr<T, mat>, ...
```
- **semantics:** Structured fractal-load wrapper for `nd2nz` / `dn2nz`.

**Parameter Table:** source/destination pointers, shape fields, source layout fields, destination group fields, control fields.

**Constraints:**

- Lowers to `set_mte2_nz_para` plus `copy_gm_to_cbuf_multi_*`.

**Example:**

```mlir
pto.mte_gm_mat %src, %dst, nd2nz,
  shape(%n, %d),
  src_layout(%sis),
  dst_group(%g, %l2s, %l3s, %l4s),
  ctrl(%l2, %small)
  : !pto.ptr<f16, gm>, !pto.ptr<f16, mat>, nd2nz, shape i64, i64, src_layout(i64), dst_group i64, i64, i64, i64, ctrl i64, i1
```

---

### `pto.mte_mat_bias`

- **syntax:**
```mlir
pto.mte_mat_bias %src, %dst, %len_burst
  nburst(%count, %src_gap, %dst_gap)
  : !pto.ptr<T, mat>, !pto.ptr<U, bias>, i64, i64, i64, i64
```
- **semantics:** Structured helper for L1 (`cbuf`) to bias-table load.

**Parameter Table:**

| Parameter | Width | Description |
|-----------|-------|-------------|
| `%src` | ptr | L1 source pointer (`mat`) |
| `%dst` | ptr | Bias destination pointer (`bias`) |
| `%len_burst` | i64 | Burst length |
| `%count` | i64 | Burst count |
| `%src_gap` | i64 | Source gap |
| `%dst_gap` | i64 | Destination gap |

**Constraints:**

- Supported type pairs: `f32->f32`, `i32->i32`, `f16->f32`, `bf16->f32`.

**Example:**

```mlir
pto.mte_mat_bias %l1_bias, %bt, %c16_i64 nburst(%c1_i64, %c0_i64, %c0_i64)
  : !pto.ptr<f16, mat>, !pto.ptr<f32, bias>, i64, i64, i64, i64
```

---

### `pto.mte_mat_left`

- **syntax:**
```mlir
pto.mte_mat_left %src, %dst, %m, %k
  : !pto.ptr<T, mat>, !pto.ptr<T, left>, i64, i64
```
- **semantics:** Structured L1-to-L0A wrapper.

**Parameter Table:** `%src`, `%dst`, `%m`, `%k`.

**Constraints:**

- Lowers to `pto.load_cbuf_to_ca`.

**Example:**

```mlir
pto.mte_mat_left %l1_a, %l0a, %c16_i64, %c16_i64
  : !pto.ptr<f16, mat>, !pto.ptr<f16, left>, i64, i64
```

---

### `pto.mte_mat_right`

- **syntax:**
```mlir
pto.mte_mat_right %src, %dst, %k, %n
  : !pto.ptr<T, mat>, !pto.ptr<T, right>, i64, i64
```
- **semantics:** Structured L1-to-L0B wrapper.

**Parameter Table:** `%src`, `%dst`, `%k`, `%n`.

**Constraints:**

- Lowers to `pto.load_cbuf_to_cb`.

**Example:**

```mlir
pto.mte_mat_right %l1_b, %l0b, %c16_i64, %c16_i64
  : !pto.ptr<f16, mat>, !pto.ptr<f16, right>, i64, i64
```

---

### `pto.mte_mat_left_mx`

- **syntax:**
```mlir
pto.mte_mat_left_mx %src, %dst, %m, %k
  : !pto.ptr<T, mat>, !pto.ptr<T, left>, i64, i64
```
- **semantics:** MX-mode L1-to-L0A wrapper.

**Parameter Table:** `%src`, `%dst`, `%m`, `%k`.

**Constraints:**

- Lowers to `pto.load_cbuf_to_ca_mx`.

**Example:**

```mlir
pto.mte_mat_left_mx %l1_a, %l0a, %c16_i64, %c64_i64
  : !pto.ptr<f8E4M3FN, mat>, !pto.ptr<f8E4M3FN, left>, i64, i64
```

---

### `pto.mte_mat_right_mx`

- **syntax:**
```mlir
pto.mte_mat_right_mx %src, %dst, %k, %n
  : !pto.ptr<T, mat>, !pto.ptr<T, right>, i64, i64
```
- **semantics:** MX-mode L1-to-L0B wrapper.

**Parameter Table:** `%src`, `%dst`, `%k`, `%n`.

**Constraints:**

- Lowers to `pto.load_cbuf_to_cb_mx`.

**Example:**

```mlir
pto.mte_mat_right_mx %l1_b, %l0b, %c64_i64, %c16_i64
  : !pto.ptr<f8E4M3FN, mat>, !pto.ptr<f8E4M3FN, right>, i64, i64
```

---

### `pto.mte_acc_mat`

- **syntax:**
```mlir
pto.mte_acc_mat %src, %dst, %m, %n, %src_stride, %dst_stride, %unit_flag_ctrl, %quant_pre, %relu_pre_mode, nz2nd|nz2dn(%loop0_src_stride)?|nz2nz(%split)? [loop3(%count, %src_stride3, %dst_stride3)]?
  : !pto.ptr<T, acc>, !pto.ptr<T, mat>, ...
```
- **semantics:** Structured L0C (`acc`) to L1 (`cbuf`) wrapper.

**Parameter Table:** `%src`, `%dst`, shape/stride fields, pre/post fields, layout mode (`nz2nd` / `nz2dn` / `nz2nz`), optional `loop3`.

**Constraints:**

- `nz2nz` mode does not accept `loop3(...)`.

**Example:**

```mlir
pto.mte_acc_mat %l0c, %l1_out, %c16_i64, %c16_i64, %c16_i64, %c16_i64, %c0_i64, %c0_i64, %c0_i64, nz2nd
  : !pto.ptr<f32, acc>, !pto.ptr<f32, mat>, i64, i64, i64, i64, i64, i64, i64, nz2nd
```

---

### `pto.mte_acc_gm`

- **syntax:**
```mlir
pto.mte_acc_gm %src, %dst, %m, %n, %src_stride, %dst_stride, %unit_flag_ctrl, %quant_pre, %relu_pre_mode, %sid, %l2_cache_ctrl, nz2nd|nz2dn(%loop0_src_stride)?|nz2nz(%split)? [loop3(%count, %src_stride3, %dst_stride3)]?
  : !pto.ptr<T, acc>, !pto.ptr<T, gm>, ...
```
- **semantics:** Structured L0C (`acc`) to GM wrapper.

**Parameter Table:** same fields as `pto.mte_acc_mat` plus `%sid` and `%l2_cache_ctrl`.

**Constraints:**

- GM output path controls (`sid`, `l2_cache_ctrl`) must be provided.

**Example:**

```mlir
pto.mte_acc_gm %l0c, %c_gm, %c16_i64, %c16_i64, %c16_i64, %c16_i64, %c0_i64, %c0_i64, %c0_i64, %c0_i64, %c0_i64, nz2nd
  : !pto.ptr<f32, acc>, !pto.ptr<f32, gm>, i64, i64, i64, i64, i64, i64, i64, i64, i64, nz2nd
```

---

### `pto.mte_acc_ub`

- **syntax:**
```mlir
pto.mte_acc_ub %src, %dst, %m, %n, %src_stride, %dst_stride, %unit_flag_ctrl, %quant_pre, %relu_pre_mode, %dual_dst_mode, %sub_blockid, nz2nd|nz2dn(%loop0_src_stride)?|nz2nz(%channel_split_en)? [loop3(%count, %src_stride3, %dst_stride3)]?
  : !pto.ptr<T, acc>, !pto.ptr<T, ub>, ...
```
- **semantics:** Structured L0C (`acc`) to UB wrapper.

**Parameter Table:** same fields as `pto.mte_acc_mat` plus `%dual_dst_mode`, `%sub_blockid`.

**Constraints:**

- `nz2nz` mode does not accept `loop3(...)`.

**Example:**

```mlir
pto.mte_acc_ub %l0c, %ub_out, %c16_i64, %c16_i64, %c16_i64, %c16_i64, %c0_i64, %c0_i64, %c0_i64, %c0_i64, %c0_i64, nz2nd
  : !pto.ptr<f32, acc>, !pto.ptr<f32, ub>, i64, i64, i64, i64, i64, i64, i64, i64, i64, nz2nd
```

---

## Grouped MTE Burst / Stride / Pad Model

This section describes the grouped MTE interfaces in this document:
`pto.mte_gm_ub` and `pto.mte_ub_gm`.

For these grouped MTE ops, the innermost `nburst(...)` group is
**stride-based**: the source and destination stride operands are the
start-to-start byte distance from one burst row to the next row.

### Key Terms

```
burst    = lenBurst contiguous bytes transferred per row
stride   = distance (bytes) from start of row[r] to start of row[r+1]
pad      = ub_stride - lenBurst, padded to the 32B alignment boundary
```

### Alignment Constraints

- **UB addresses** (both source and destination) must be **32-byte aligned**.
- **GM→UB padding**: When `pad(...)` is present on `pto.mte_gm_ub`, each UB row is padded from `lenBurst` up to the **32B-aligned boundary** of `ub_stride` with `pad_val`. This ensures every UB row starts at a 32B-aligned offset.
- **UB→GM de-padding**: MTE3 reads `lenBurst` bytes from each 32B-aligned UB row (skipping any padding that was added during load), writing only valid data to GM. This effectively strips padding on store.

---

## UB Copy Burst / Step Model

This section describes the grouped UB-copy interface in this document:
`pto.mte_ub_ub` and `pto.mte_ub_mat`.

For these grouped UB-copy ops, each burst copies `len_burst * 32` bytes.

The next burst starts at:

```text
src_next = src_curr + (len_burst + src_gap) * 32 bytes
dst_next = dst_curr + (len_burst + dst_gap) * 32 bytes
```

So `src_gap` and `dst_gap` are gap fields that advance to the next burst
after the copied 32B blocks.

### 2D Diagram: GM→UB (`pto.mte_gm_ub`)

```
GM (source, `!pto.ptr<T, gm>`):

          |<--- src_stride (start-to-start) --->|
          |<- len_burst ->|                     |
Row 0:    [##DATA########]......................|
Row 1:    [##DATA########]......................|
Row 2:    [##DATA########]......................|
          ...
Row N-1:  [##DATA########]

UB (destination, `!pto.ptr<T, ub>`, 32B-aligned):

          |<---------- dst_stride (32B-aligned) ---------->|
          |<- len_burst ->|<- pad (to 32B boundary) ->|    |
Row 0:    [##DATA########][000000 PAD 000000000000000]
Row 1:    [##DATA########][000000 PAD 000000000000000]
Row 2:    [##DATA########][000000 PAD 000000000000000]
          ...
Row N-1:  [##DATA########][000000 PAD 000000000000000]

N = n_burst
stride = start of row[r] to start of row[r+1]
pad    = filled with pad_val to 32B boundary (`pad(...)` present)
[DATA] = valid data transferred by DMA
[PAD]  = pad_val fill (from `pad(...)`)
```

### 2D Diagram: UB→GM (`pto.mte_ub_gm` with GM destination)

```
UB (source, `!pto.ptr<T, ub>`, 32B-aligned start addr):

          |<---------- src_stride (32B-aligned) --------->|
          |<- len_burst ->|<-- pad (ignored on read) -->| |
Row 0:    [##DATA########][000 pad 000000000000000000]
Row 1:    [##DATA########][000 pad 000000000000000000]
Row 2:    [##DATA########][000 pad 000000000000000000]
          ...
Row N-1:  [##DATA########][000 pad 000000000000000000]

GM (destination, `!pto.ptr<T, gm>`):

          |<--- dst_stride (start-to-start) --->|
          |<- len_burst ->|                     |
Row 0:    [##DATA########]......................|
Row 1:    [##DATA########]......................|
Row 2:    [##DATA########]......................|
          ...
Row N-1:  [##DATA########]

N = n_burst
MTE3 reads only len_burst bytes from each UB row (de-padding).
Only len_burst bytes are written to each GM row.
```

---

## Multi-Level Loop Semantics

The full DMA transfer is a nested loop. `nburst(...)` is the innermost group.
If one or more `loop(...)` groups are present, they wrap `nburst(...)` in the
same order they appear in the op: the first `loop(...)` is the innermost outer
group, the second `loop(...)` wraps the first one, and so on.

### GM→UB Full Loop

For a form

```mlir
pto.mte_gm_ub %gm_src, %ub_dst, %l2_cache_ctl, %len_burst
  nburst(%n_burst, %src_stride, %dst_stride)
  loop(%c0, %s0, %d0)
  loop(%c1, %s1, %d1)
  ...
  loop(%cN, %sN, %dN)
  [pad(%pad_value[, %left_padding_count, %right_padding_count])]
```

the transfer is equivalent to:

```c
for (int lN = 0; lN < cN; ++lN) {
  ...
  for (int l1 = 0; l1 < c1; ++l1) {
    for (int l0 = 0; l0 < c0; ++l0) {
      uint8_t *gm_base = gm_src + l0 * s0 + l1 * s1 + ... + lN * sN;
      uint8_t *ub_base = ub_dst + l0 * d0 + l1 * d1 + ... + lN * dN;
      for (int r = 0; r < n_burst; ++r) {
        memcpy(ub_base + r * dst_stride,
               gm_base + r * src_stride,
               len_burst);
        if (pad_enabled)
          memset(ub_base + r * dst_stride + len_burst,
                 pad_val,
                 dst_stride - len_burst);
      }
    }
  }
}
```

If no `loop(...)` group is present, only the innermost `nburst(...)` loop
remains.

### UB→Destination Full Loop

For a form

```mlir
pto.mte_ub_gm %ub_src, %dst, %len_burst
  nburst(%n_burst, %src_stride, %dst_stride)
  loop(%c0, %s0, %d0)
  loop(%c1, %s1, %d1)
  ...
  loop(%cN, %sN, %dN)
```

the transfer is equivalent to:

```c
for (int lN = 0; lN < cN; ++lN) {
  ...
  for (int l1 = 0; l1 < c1; ++l1) {
    for (int l0 = 0; l0 < c0; ++l0) {
      uint8_t *ub_base = ub_src + l0 * s0 + l1 * s1 + ... + lN * sN;
      uint8_t *dst_base = dst + l0 * d0 + l1 * d1 + ... + lN * dN;
      for (int r = 0; r < n_burst; ++r) {
        memcpy(dst_base + r * dst_stride,
               ub_base + r * src_stride,
               len_burst);
      }
    }
  }
}
```

If no `loop(...)` group is present, only the innermost `nburst(...)` loop
remains.

---

## Example 1: GM→UB — Load a 32×32 f32 Tile (Simple Case)

Load a 32×32 f32 tile from GM into UB. This matches the `abs_kernel_2d` test case.

```
GM layout (32 × 32 f32, contiguous):

    |<- len_burst = 128B (32 × 4) ->|
    |<- src_stride = 128B --------->|
    +--[#######TILE#######]--+  row 0
    +--[#######TILE#######]--+  row 1
    ...
    +--[#######TILE#######]--+  row 31

UB layout (32 × 32 f32, 32B-aligned, contiguous):

    |<- dst_stride = 128B (32B-aligned) ->|
    +--[#######TILE#######]--+  row 0
    +--[#######TILE#######]--+  row 1
    ...
    +--[#######TILE#######]--+  row 31

    len_burst   = 32 × 4 = 128 bytes
    src_stride  = 128 bytes (contiguous rows)
    dst_stride  = 128 bytes (already 32B-aligned, no padding)
```

```mlir
// Simple 2D load — only nburst(...) is needed
pto.mte_gm_ub %arg0, %ub_in, %c0_i64, %c128_i64
  nburst(%c32_i64, %c128_i64, %c128_i64)
  : !pto.ptr<f32, gm>, !pto.ptr<f32, ub>, i64, i64, i64
```

---

## Example 2: GM→UB — Load a 2D Tile from a Larger Matrix

Load a 64×128 tile (f16) from a 1024×512 matrix in GM into UB.

```
GM layout (1024 × 512 f16):

    col 0          col 128               col 512
    |              |                     |
    +--[###TILE###]+.....................+  row R
    +--[###TILE###]+.....................+  row R+1
    ...
    +--[###TILE###]+.....................+  row R+63

    |<--------- src_stride = 1024B ----------->|
    |<-len_burst=256B->|

    len_burst   = 128 × 2 = 256 bytes (128 f16 elements)
    src_stride  = 512 × 2 = 1024 bytes (start-to-start, full GM row)

UB layout (64 × 128 f16, 32B-aligned, contiguous):

    +--[###TILE###]--+  row 0  (256 bytes, 32B-aligned, no pad)
    +--[###TILE###]--+  row 1
    ...
    +--[###TILE###]--+  row 63

    dst_stride = 256 bytes (= len_burst, already 32B-aligned, no padding)
```

```mlir
pto.mte_gm_ub %gm_ptr, %ub_ptr, %c0_i64, %c256_i64
  nburst(%c64_i64, %c1024_i64, %c256_i64)
  : !pto.ptr<f16, gm>, !pto.ptr<f16, ub>, i64, i64, i64
```

---

## Example 3: GM→UB — Load with Padding

Load 100 valid columns from GM into a 128-wide UB tile (f16). The remaining 28 columns are zero-padded.

```
GM (100 cols valid, contiguous):

    |<-len_burst=200B->|
    |<- src_stride=200B (start-to-start) ->|
    +--[####DATA####]-+  row 0
    +--[####DATA####]-+  row 1
    ...
    +--[####DATA####]-+  row 63

UB (128 cols wide, 32B-aligned, padded):

    |<--------- dst_stride = 256B (32B-aligned) --------->|
    |<-len_burst=200B->|<---- pad = 56B to 32B boundary ->|
    +--[####DATA####]-+[0000000 PAD 0000000000000000000000]+  row 0
    +--[####DATA####]-+[0000000 PAD 0000000000000000000000]+  row 1
    ...
    +--[####DATA####]-+[0000000 PAD 0000000000000000000000]+  row 63

    len_burst   = 100 × 2 = 200 bytes
    src_stride  = 200 bytes (start-to-start, contiguous in GM)
    dst_stride  = 128 × 2 = 256 bytes (32B-aligned tile width in UB)
    pad         = 256 - 200 = 56 bytes (padded to 32B boundary with pad_val)
```

```mlir
%pad = arith.constant 0 : i16
pto.mte_gm_ub %gm_ptr, %ub_ptr, %c0_i64, %c200_i64
  nburst(%c64_i64, %c200_i64, %c256_i64)
  pad(%pad, %c0_i64, %c0_i64)
  : !pto.ptr<f16, gm>, !pto.ptr<f16, ub>, i64, i64, i64, pad i16, i64, i64
```

---

## Example 4: UB→GM — Store a 32×32 f32 Tile (Simple Case)

Store a 32×32 f32 tile from UB back to GM. This matches the `abs_kernel_2d` test case.

```
UB (source, 32B-aligned, 32 × 32 f32):

    |<- src_stride = 128B (32B-aligned) ->|
    |<- len_burst = 128B ->|
    +--[#######TILE#######]---+  row 0
    +--[#######TILE#######]---+  row 1
    ...
    +--[#######TILE#######]---+  row 31

    (no padding here — len_burst == src_stride)

GM (dest, 32 × 32 f32):

    |<- dst_stride = 128B ->|
    |<- len_burst = 128B -->|
    +--[#######TILE#######]---+  row 0
    +--[#######TILE#######]---+  row 1
    ...
    +--[#######TILE#######]---+  row 31
```

```mlir
pto.mte_ub_gm %ub_out, %arg1, %c128_i64
  nburst(%c32_i64, %c128_i64, %c128_i64)
  : !pto.ptr<f32, ub>, !pto.ptr<f32, gm>, i64, i64, i64, i64
```

---

## Example 5: UB→GM — Store a 2D Tile Back to a Larger Matrix

Store a 64×128 tile (f16) from UB back to a 1024×512 GM matrix at an offset.

```
UB (source, 32B-aligned, 64 × 128 f16):

    |<- src_stride = 256B (32B-aligned) ->|
    |<- len_burst = 256B ->|
    +--[#####TILE#####]---+  row 0
    +--[#####TILE#####]---+  row 1
    ...
    +--[#####TILE#####]---+  row 63

    (no padding here — len_burst == src_stride)

GM (dest, into 1024 × 512 matrix):

    |<----------- dst_stride = 1024B (start-to-start) --------->|
    |<- len_burst = 256B ->|                                    |
    col 0          col 128                              col 512
    +--[#####TILE#####]---+.............................+  row R
    +--[#####TILE#####]---+.............................+  row R+1
    ...
    +--[#####TILE#####]---+.............................+  row R+63

    MTE3 reads len_burst bytes from each 32B-aligned UB row,
    writes only len_burst bytes per GM row (stride controls row spacing).
```

```mlir
pto.mte_ub_gm %ub_ptr, %gm_ptr, %c256_i64
  nburst(%c64_i64, %c256_i64, %c1024_i64)
  : !pto.ptr<f16, ub>, !pto.ptr<f16, gm>, i64, i64, i64, i64
```

---

## Example 6: GM→UB with Multi-Level Loop (Batch of Tiles)

Load 4 batches of 8×128 tiles from a [4, 8, 128] f16 tensor using one outer
`loop(...)` group.

```
GM [4, 8, 128] f16 (contiguous):        UB (4 tiles laid out sequentially):

    batch 0: 8 rows × 256 bytes          [batch 0: 8×128][batch 1: 8×128]
    batch 1: 8 rows × 256 bytes          [batch 2: 8×128][batch 3: 8×128]
    batch 2: 8 rows × 256 bytes
    batch 3: 8 rows × 256 bytes          outer loop src_stride = 2048 bytes (8 × 256)
                                          outer loop dst_stride = 2048 bytes (8 × 256)
    Each batch = 8 × 256 = 2048 bytes     outer loop count = 4 (iterate over batches)
```

```mlir
// One outer loop group over 4 batches
pto.mte_gm_ub %gm_ptr, %ub_ptr, %c0_i64, %c256_i64
  nburst(%c8_i64, %c256_i64, %c256_i64)
  loop(%c4_i64, %c2048_i64, %c2048_i64)
  : !pto.ptr<f16, gm>, !pto.ptr<f16, ub>, i64, i64, i64, loop i64, i64, i64
```

Execution trace:

```
loop iter 0: gm_ptr + 0×2048 → ub_ptr + 0×2048, DMA 8 rows × 256B
loop iter 1: gm_ptr + 1×2048 → ub_ptr + 1×2048, DMA 8 rows × 256B
loop iter 2: gm_ptr + 2×2048 → ub_ptr + 2×2048, DMA 8 rows × 256B
loop iter 3: gm_ptr + 3×2048 → ub_ptr + 3×2048, DMA 8 rows × 256B
```
