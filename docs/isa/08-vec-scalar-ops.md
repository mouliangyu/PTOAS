# 8. Vec-Scalar Ops

> **Category:** Vector-scalar operations
> **Pipeline:** PIPE_V (Vector Core)

Operations that combine a vector with a scalar value, applying the scalar to every lane.

## Common Operand Model

- `%input` is the source vector register value.
- `%scalar` is the scalar operand in SSA form.
- `%mask` is the predicate operand.
- `%result` is the destination vector register value.
- For 32-bit scalar forms, the scalar source MUST satisfy the backend's legal
  scalar-source constraints for this family.
- For elementwise vec-scalar families whose scalar conceptually matches the
  vector element type (`pto.vadds`, `pto.vsadds`, `pto.vmuls`, `pto.vmaxs`,
  `pto.vmins`, `pto.vlrelu`):
  - signed integer vectors accept signed integer scalars with the same width,
    and also accept signless `i<width>`
  - unsigned integer vectors accept unsigned integer scalars with the same
    width, and also accept signless `i<width>`
  - signless integer vectors accept signless `i<width>`
- `pto.vshls` and `pto.vshrs` are not part of that rule; their scalar operand
  is the shift amount and remains fixed to `i16`.

---

## Arithmetic

### `pto.vadds`

- **syntax:** `%result = pto.vadds %input, %scalar, %mask : !pto.vreg<NxT>, T, !pto.mask<G> -> !pto.vreg<NxT>`
- **A5 types:** `s8`, `s16`, `s32`, `u8`, `u16`, `u32`, `f16`, `bf16`, `f32`

```c
for (int i = 0; i < N; i++)
    dst[i] = src[i] + scalar;
```

- **inputs:** `%input` is the source vector, `%scalar` is broadcast logically to
  each lane, and `%mask` selects active lanes.
- **outputs:** `%result` is the lane-wise sum.
- **constraints and limitations:** Input vector element type, scalar type, and
  result vector element type MUST match. For integer vector forms, `%scalar`
  may also use matching-signedness integer or signless `i<width>` with the same
  bit width as the vector element type, so it can be fed directly from `arith`
  constants.

---

### `pto.vsadds`

- **syntax:** `%result = pto.vsadds %input, %scalar, %mask : !pto.vreg<NxT>, T, !pto.mask<G> -> !pto.vreg<NxT>`
- **A5 types:** s16

```c
for (int i = 0; i < N; i++)
    dst[i] = saturate(src[i] + scalar, T);
```

- **inputs:** `%input` is the source vector, `%scalar` is broadcast logically to
  each lane, and `%mask` selects active lanes.
- **outputs:** `%result` is the lane-wise saturated sum.
- **constraints and limitations:** Signed integer element types only. This op
  is the explicit saturating vector-scalar add family and is not interchangeable
  with `pto.vadds`. `%scalar` may use `si16` or signless `i16`.

---

### `pto.vmuls`

- **syntax:** `%result = pto.vmuls %input, %scalar, %mask : !pto.vreg<NxT>, T, !pto.mask<G> -> !pto.vreg<NxT>`

```c
for (int i = 0; i < N; i++)
    dst[i] = src[i] * scalar;
```

- **inputs:** `%input`, `%scalar`, and `%mask` as above.
- **outputs:** `%result` is the lane-wise product.
- **constraints and limitations:** Supported element types are hardware-family
  specific; the current PTO micro Instruction documentation covers the common
  numeric cases. For integer vector forms, `%scalar` may use matching-signedness
  integer or signless `i<width>` with the same bit width as the vector element
  type.

---

### `pto.vmaxs`

- **syntax:** `%result = pto.vmaxs %input, %scalar, %mask : !pto.vreg<NxT>, T, !pto.mask<G> -> !pto.vreg<NxT>`

```c
for (int i = 0; i < N; i++)
    dst[i] = (src[i] > scalar) ? src[i] : scalar;
```

- **inputs:** `%input`, `%scalar`, and `%mask` as above.
- **outputs:** `%result` is the lane-wise maximum.
- **constraints and limitations:** Input and result types MUST match. For
  integer vector forms, `%scalar` may use matching-signedness integer or
  signless `i<width>` with the same bit width as the vector element type.

---

### `pto.vmins`

- **syntax:** `%result = pto.vmins %input, %scalar, %mask : !pto.vreg<NxT>, T, !pto.mask<G> -> !pto.vreg<NxT>`

```c
for (int i = 0; i < N; i++)
    dst[i] = (src[i] < scalar) ? src[i] : scalar;
```

- **inputs:** `%input`, `%scalar`, and `%mask` as above.
- **outputs:** `%result` is the lane-wise minimum.
- **constraints and limitations:** Input and result types MUST match. For
  integer vector forms, `%scalar` may use matching-signedness integer or
  signless `i<width>` with the same bit width as the vector element type.

---

## Shift

### `pto.vshls`

- **syntax:** `%result = pto.vshls %input, %scalar, %mask : !pto.vreg<NxT>, i16, !pto.mask<G> -> !pto.vreg<NxT>`

```c
for (int i = 0; i < N; i++)
    dst[i] = src[i] << scalar;
```

- **inputs:** `%input` is the value vector, `%scalar` is the uniform `i16` shift
  amount, and `%mask` selects active lanes.
- **outputs:** `%result` is the shifted vector.
- **constraints and limitations:** Integer element types only. The shift amount
  SHOULD stay within the source element width.

---

### `pto.vshrs`

- **syntax:** `%result = pto.vshrs %input, %scalar, %mask : !pto.vreg<NxT>, i16, !pto.mask<G> -> !pto.vreg<NxT>`

```c
for (int i = 0; i < N; i++)
    dst[i] = src[i] >> scalar;
```

- **inputs:** `%input` is the value vector, `%scalar` is the uniform `i16` shift
  amount, and `%mask` selects active lanes.
- **outputs:** `%result` is the shifted vector.
- **constraints and limitations:** Integer element types only.

---

### `pto.vlrelu`

- **syntax:** `%result = pto.vlrelu %input, %scalar, %mask : !pto.vreg<NxT>, T, !pto.mask<G> -> !pto.vreg<NxT>`

```c
for (int i = 0; i < N; i++)
    dst[i] = (src[i] >= 0) ? src[i] : scalar * src[i];
```

- **inputs:** `%input` is the activation vector, `%scalar` is the leaky slope,
  and `%mask` selects active lanes.
- **outputs:** `%result` is the lane-wise leaky-ReLU result.
- **constraints and limitations:** Only `f16` and `f32` forms are currently
  documented for `pto.vlrelu`.

---

## Carry Operations

### `pto.vaddcs`

- **syntax:** `%result, %carry = pto.vaddcs %lhs, %rhs, %carry_in, %mask : !pto.vreg<NxT>, !pto.vreg<NxT>, !pto.mask<G>, !pto.mask<G> -> !pto.vreg<NxT>, !pto.mask<G>`
- **semantics:** Add with carry-in and carry-out.

```c
for (int i = 0; i < N; i++) {
    uint64_t r = (uint64_t)src0[i] + src1[i] + carry_in[i];
    dst[i] = (T)r;
    carry_out[i] = (r >> bitwidth);
}
```

- **inputs:** `%lhs` and `%rhs` are the value vectors, `%carry_in` is the
  incoming carry predicate, and `%mask` selects active lanes.
- **outputs:** `%result` is the arithmetic result and `%carry` is the carry-out
  predicate.
- **A5 types:** `i32`, `s32`, `u32`
- **constraints and limitations:** This is the scalar-extended carry-chain
  family. On the current A5 surface, only 32-bit integer element types are
  supported.

---

### `pto.vsubcs`

- **syntax:** `%result, %borrow = pto.vsubcs %lhs, %rhs, %borrow_in, %mask : !pto.vreg<NxT>, !pto.vreg<NxT>, !pto.mask<G>, !pto.mask<G> -> !pto.vreg<NxT>, !pto.mask<G>`
- **semantics:** Subtract with borrow-in and borrow-out.

```c
for (int i = 0; i < N; i++) {
    dst[i] = src0[i] - src1[i] - borrow_in[i];
    borrow_out[i] = (src0[i] < src1[i] + borrow_in[i]);
}
```

- **inputs:** `%lhs` and `%rhs` are the value vectors, `%borrow_in` is the
  incoming borrow predicate, and `%mask` selects active lanes.
- **outputs:** `%result` is the arithmetic result and `%borrow` is the
  borrow-out predicate.
- **A5 types:** `i32`, `s32`, `u32`
- **constraints and limitations:** This is the scalar-extended borrow-chain
  family and is currently restricted to 32-bit integer element types.

---

## Typical Usage

```mlir
// Add bias to all elements
%biased = pto.vadds %activation, %bias_scalar, %mask : !pto.vreg<64xf32>, f32, !pto.mask<G> -> !pto.vreg<64xf32>

// Scale by constant
%scaled = pto.vmuls %input, %scale, %mask : !pto.vreg<64xf32>, f32, !pto.mask<G> -> !pto.vreg<64xf32>

// Clamp to [0, 255] for uint8 quantization
%clamped_low = pto.vmaxs %input, %c0, %mask : !pto.vreg<64xf32>, f32, !pto.mask<G> -> !pto.vreg<64xf32>
%clamped = pto.vmins %clamped_low, %c255, %mask : !pto.vreg<64xf32>, f32, !pto.mask<G> -> !pto.vreg<64xf32>

// Shift right by fixed amount
%shifted = pto.vshrs %data, %c4, %mask : !pto.vreg<64xi32>, i16, !pto.mask<G> -> !pto.vreg<64xi32>
```
