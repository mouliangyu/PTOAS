"""TileLang DSL template for pto.tfillpad_expand

Expand mode semantics:
  - TFILLPAD_EXPAND: src rows may be less than dst rows
  - Copy src.valid data to dst
  - Fill cols from src.valid_cols to dst.valid_cols with FillPadVal
  - Fill rows from src.rows to dst.rows with FillPadVal

Strategy:
  - Phase 1: Copy aligned valid blocks
  - Phase 2: Fill cols aligned_col to dst_cols-1 with FillPadVal
  - Phase 3: Copy tail valid lanes
  - Phase 4: Fill row expansion
"""

import tilelang_dsl as pto

_NEG1_F32 = -1.0


@pto.vkernel(
    target="a5",
    op="pto.tfillpad_expand",
)
def template_tfillpad_expand(src: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape
    dst_valid_rows, dst_valid_cols = dst.valid_shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col

    has_valid_expansion = (src_valid_cols < dst_valid_cols) or (src_valid_rows < dst_valid_rows)

    # f32 handling
    if pto.constexpr(dtype == pto.f32):
        if pto.constexpr(dst.pad_value == pto.PadValue.ZERO and has_valid_expansion):
            fill_scalar = pto.f32(_NEG1_F32)
        elif pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.f32(0.0)

        # Phase 1: Copy aligned valid blocks
        for row in range(0, src_valid_rows, 1):
            remained = aligned_col
            for col in range(0, aligned_col, lanes):
                mask, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, col:])
                pto.vsts(data, dst[row, col:], mask)

        # Phase 2: Fill cols from aligned_col to dst_valid_cols-1
        if pto.constexpr(aligned_col < dst_valid_cols):
            for row in range(0, dst_valid_rows, 1):
                remained = dst_valid_cols - aligned_col
                for col in range(aligned_col, dst_valid_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

        # Phase 3: Copy tail valid lanes
        if pto.constexpr(has_tail):
            for row in range(0, src_valid_rows, 1):
                remained = src_valid_cols - aligned_col
                mask_copy, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, aligned_col:])
                pto.vsts(data, dst[row, aligned_col:], mask_copy)

        # Phase 4: Fill row expansion
        if pto.constexpr(src_rows < dst_rows):
            for row in range(src_rows, dst_rows, 1):
                remained = dst_valid_cols
                for col in range(0, dst_valid_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

    # i16 handling (includes i16, si16, ui16)
    elif pto.constexpr(dtype == pto.i16 or dtype == pto.si16 or dtype == pto.ui16):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.i16(0)

        for row in range(0, src_valid_rows, 1):
            remained = aligned_col
            for col in range(0, aligned_col, lanes):
                mask, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, col:])
                pto.vsts(data, dst[row, col:], mask)

        if pto.constexpr(aligned_col < dst_valid_cols):
            for row in range(0, dst_valid_rows, 1):
                remained = dst_valid_cols - aligned_col
                for col in range(aligned_col, dst_valid_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

        if pto.constexpr(has_tail):
            for row in range(0, src_valid_rows, 1):
                remained = src_valid_cols - aligned_col
                mask_copy, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, aligned_col:])
                pto.vsts(data, dst[row, aligned_col:], mask_copy)

        if pto.constexpr(src_rows < dst_rows):
            for row in range(src_rows, dst_rows, 1):
                remained = dst_valid_cols
                for col in range(0, dst_valid_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

    # i32 handling (includes i32, si32, ui32)
    elif pto.constexpr(dtype == pto.i32 or dtype == pto.si32 or dtype == pto.ui32):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.i32(0)

        for row in range(0, src_valid_rows, 1):
            remained = aligned_col
            for col in range(0, aligned_col, lanes):
                mask, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, col:])
                pto.vsts(data, dst[row, col:], mask)

        if pto.constexpr(aligned_col < dst_valid_cols):
            for row in range(0, dst_valid_rows, 1):
                remained = dst_valid_cols - aligned_col
                for col in range(aligned_col, dst_valid_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

        if pto.constexpr(has_tail):
            for row in range(0, src_valid_rows, 1):
                remained = src_valid_cols - aligned_col
                mask_copy, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, aligned_col:])
                pto.vsts(data, dst[row, aligned_col:], mask_copy)

        if pto.constexpr(src_rows < dst_rows):
            for row in range(src_rows, dst_rows, 1):
                remained = dst_valid_cols
                for col in range(0, dst_valid_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

    # i8 handling (includes i8, si8, ui8)
    elif pto.constexpr(dtype == pto.i8 or dtype == pto.si8 or dtype == pto.ui8):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()
        else:
            fill_scalar = pto.i8(0)

        for row in range(0, src_valid_rows, 1):
            remained = aligned_col
            for col in range(0, aligned_col, lanes):
                mask, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, col:])
                pto.vsts(data, dst[row, col:], mask)

        if pto.constexpr(aligned_col < dst_valid_cols):
            for row in range(0, dst_valid_rows, 1):
                remained = dst_valid_cols - aligned_col
                for col in range(aligned_col, dst_valid_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

        if pto.constexpr(has_tail):
            for row in range(0, src_valid_rows, 1):
                remained = src_valid_cols - aligned_col
                mask_copy, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, aligned_col:])
                pto.vsts(data, dst[row, aligned_col:], mask_copy)

        if pto.constexpr(src_rows < dst_rows):
            for row in range(src_rows, dst_rows, 1):
                remained = dst_valid_cols
                for col in range(0, dst_valid_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

    return