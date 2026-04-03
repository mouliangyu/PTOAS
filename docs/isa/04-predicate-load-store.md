# 4. Predicate Load/Store

> **Category:** UB ↔ Predicate Register data movement
> **Pipeline:** PIPE_V (Vector Core)

Predicate registers (`!pto.mask<G>`) are 256-bit registers that enable per-lane conditional execution. These ops move predicate values between UB and predicate registers.

In concrete examples, `G` should be chosen to match the consumer family. The
examples below use `b32` when the loaded/stored mask is paired with `f32`
vector compares or selects.

---

## Predicate Loads

### `pto.plds`

- **syntax:** `%result = pto.plds %source[%offset], "DIST" : !pto.ptr<T, ub>, index -> !pto.mask<G>`
- **semantics:** Load predicate register with runtime offset. This is the
  dynamic-offset form of `pto.pldi`: the predicate payload interpretation is
  the same, but `%offset` is supplied as an SSA `index` instead of a constant
  `index` immediate.

**Distribution modes:** `NORM`, `US`, `DS`.

**Example:**
```mlir
%mask = pto.plds %ub[%c0], "NORM" : !pto.ptr<T, ub>, index -> !pto.mask<G>
```

---

### `pto.pld`

- **syntax:** `%result = pto.pld %source[%offset], "DIST" : !pto.ptr<T, ub>, index -> !pto.mask<G>`
- **semantics:** Load predicate register with areg offset.

---

### `pto.pldi`

- **syntax:** `%result = pto.pldi %source[%offset], "DIST" : !pto.ptr<T, ub>, index -> !pto.mask<G>`
- **offset:** must be a constant `index` immediate in PTO surface form.
- **semantics:** Load predicate register with immediate offset.
- **DIST:** mandatory string token, one of `NORM`, `US`, `DS`.
  - `NORM`: load the normal 256-byte predicate payload.
  - `US`: load 128 bytes, then duplicate the loaded bits once.
  - `DS`: load 512 bytes, then keep one bit out of every two bits.

---

## Predicate Stores

### `pto.psts`

- **syntax:** `pto.psts %value, %dest[%offset], "DIST" : !pto.mask<G>, !pto.ptr<T, ub>, index`
- **semantics:** Store predicate register with runtime offset. This is the
  dynamic-offset form of `pto.psti`: the predicate payload interpretation is
  the same, but `%offset` is supplied as an SSA `index` instead of a constant
  `index` immediate.

**Distribution modes:** `NORM`, `PK`

**Example:**
```mlir
pto.psts %mask, %ub[%c0], "NORM" : !pto.mask<G>, !pto.ptr<T, ub>, index
```

---

### `pto.pst`

- **syntax:** `pto.pst %value, %dest[%offset], "DIST" : !pto.mask<G>, !pto.ptr<T, ub>, index`
- **semantics:** Store predicate register with areg offset.

**Distribution modes:** `NORM`, `PK`

---

### `pto.psti`

- **syntax:** `pto.psti %value, %dest[%offset], "DIST" : !pto.mask<G>, !pto.ptr<T, ub>, index`
- **offset:** must be a constant `index` immediate in PTO surface form.
- **semantics:** Store predicate register with immediate offset.
- **DIST:** mandatory string token, one of `NORM`, `PK`.
  - `NORM`: store predicate payload into a normal 256-byte space.
  - `PK`: store into a 128-byte space, keeping one bit out of every two bits.

---

### `pto.pstu`

- **syntax:** `%align_out, %base_out = pto.pstu %align_in, %value, %base : !pto.align, !pto.mask<b16>, !pto.ptr<ui16, ub> -> !pto.align, !pto.ptr<ui16, ub>`
- **syntax:** `%align_out, %base_out = pto.pstu %align_in, %value, %base : !pto.align, !pto.mask<b32>, !pto.ptr<ui32, ub> -> !pto.align, !pto.ptr<ui32, ub>`
- **semantics:** Predicate unaligned store with align/base state update. The base type is fixed by mask granularity: `b16 <-> ui16`, `b32 <-> ui32`.

---

## Typical Usage Pattern

```mlir
// Generate comparison mask
%mask = pto.vcmp %v0, %v1, %seed, "lt" : !pto.vreg<64xf32>, !pto.vreg<64xf32>, !pto.mask<b32> -> !pto.mask<b32>

// Store mask to UB for later use
pto.psts %mask, %ub_mask[%c0], "NORM" : !pto.mask<b32>, !pto.ptr<T, ub>, index

// ... later in another kernel ...

// Load mask from UB
%saved_mask = pto.plds %ub_mask[%c0], "NORM" : !pto.ptr<T, ub>, index -> !pto.mask<b32>

// Use for predicated select
%result = pto.vsel %v_true, %v_false, %saved_mask : !pto.vreg<64xf32>, !pto.vreg<64xf32>, !pto.mask<b32> -> !pto.vreg<64xf32>
```
