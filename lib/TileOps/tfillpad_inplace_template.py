# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tfillpad_inplace

Semantic (based on C++ TFillPad.hpp reference):
  - TFILLPAD_INPLACE: same physical buffer (src == dst), skips copy phase, only fills expansion
  - Inplace mode: src and dst share the same physical UB address

Strategy (inplace mode):
  - Skip Phase 1+3: Copy phases (data already in buffer)
  - Phase 2: Fill cols from src_valid_cols to dst_valid_cols-1 with FillPadVal

Note: Row expansion is not handled in inplace mode since src.shape == dst.shape.
"""

import tilelang_dsl as pto

_NEG1_F32 = -1.0

# All supported dtype pairs
_DTYPE_SIGNATURES = [
    (pto.f32, pto.f32),
    (pto.i16, pto.i16),
    (pto.si16, pto.si16),
    (pto.ui16, pto.ui16),
    (pto.i32, pto.i32),
    (pto.si32, pto.si32),
    (pto.ui32, pto.ui32),
    (pto.i8, pto.i8),
    (pto.si8, pto.si8),
    (pto.ui8, pto.ui8),
]


@pto.vkernel(
    target="a5",
    op="pto.tfillpad_inplace",
    dtypes=_DTYPE_SIGNATURES,
    advanced=True,  # Required for as_ptr()
)
def template_tfillpad_inplace(src: pto.Tile, dst: pto.Tile):
    """tfillpad_inplace: skip copy phase, only fill expansion regions.

Uses vstus+vstas for unaligned column fill, matching C++ TFillPad.hpp.
"""
    dtype = dst.element_type
    _, _ = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    _, _ = dst.shape
    dst_valid_rows, dst_valid_cols = dst.valid_shape
    lanes = pto.get_lanes(dtype)
    has_valid_expansion = (src_valid_cols < dst_valid_cols) or (src_valid_rows < dst_valid_rows)

    # PadValue handling - same as tfillpad_template.py
    # Note: dtype and pad_value are compile-time known, so constexpr is valid for those.
    # has_valid_expansion is a runtime value derived from dynamic shapes, so split the condition.
    if pto.constexpr(dtype == pto.f32):
        if pto.constexpr(dst.pad_value == pto.PadValue.ZERO):
            # For ZERO pad_value, use -1.0 encoding only when there's valid expansion
            if has_valid_expansion:
                fill_scalar = pto.f32(_NEG1_F32)
            else:
                fill_scalar = pto.f32(0.0)
        elif pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.f32(0.0)
    elif pto.constexpr(dtype == pto.ui16):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.ui16(0)
    elif pto.constexpr(dtype == pto.si16):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.si16(0)
    elif pto.constexpr(dtype == pto.i16):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.i16(0)
    elif pto.constexpr(dtype == pto.ui32):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.ui32(0)
    elif pto.constexpr(dtype == pto.si32):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.si32(0)
    elif pto.constexpr(dtype == pto.i32):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.i32(0)
    elif pto.constexpr(dtype == pto.ui8):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.ui8(0)
    elif pto.constexpr(dtype == pto.si8):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.si8(0)
    elif pto.constexpr(dtype == pto.i8):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.i8(0)

    # Phase 2: Fill cols from src_valid_cols to dst_valid_cols-1
    # Use vstus+vstas for unaligned starting column, matching C++ TFillPad.hpp
    # Runtime condition: valid_shape values may be dynamic at kernel specialization time.
    if src_valid_cols < dst_valid_cols:
        pad_cols = dst_valid_cols - src_valid_cols
        # Create fill vector once (reused across all rows)
        fill_vec = pto.vdup(fill_scalar, pto.make_mask(dtype, pto.PAT.ALL))
        for _ in range(0, dst_valid_rows, 1):
            # Initialize align register for this row
            ureg = pto.init_align()
            # Get pointer to UB buffer
            base_ptr = dst.as_ptr()
            # Simple loop: always iterate pad_cols times, each iteration uses min(lanes, remaining)
            # This keeps vstus structure without complex nested branching
            # ureg is loop-carried, updated in every iteration
            remaining = pad_cols
            for _ in range(0, pad_cols, lanes):
                # Compute current iteration count (always >= 1 since loop runs)
                # Use min to handle last partial iteration
                if remaining >= lanes:
                    ureg = pto.vstus(ureg, lanes, fill_vec, base_ptr)
                    remaining = remaining - lanes
                else:
                    ureg = pto.vstus(ureg, remaining, fill_vec, base_ptr)
                    remaining = 0
            # vstas: flush buffered bytes with offset pointing to src_valid_cols
            pto.vstas(ureg, base_ptr, src_valid_cols)
    return