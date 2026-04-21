"""TileLang DSL template for pto.tfillpad and pto.tfillpad_inplace

Semantic:
  - TFILLPAD copies src.valid data to dst
  - TFILLPAD fills cols from src.valid_cols to dst.cols with FillPadVal
  - TFILLPAD fills rows from src.rows to dst.rows with FillPadVal

Strategy:
  - Phase 1: Copy aligned valid blocks (cols 0 to aligned_col-1)
  - Phase 2: Fill cols aligned_col to dst_cols-1 with FillPadVal
  - Phase 3: Copy tail valid lanes (cols aligned_col to src_valid_cols-1)
  - Phase 4: Fill row expansion

Address alignment:
  - vlds/vsts require 32-byte aligned addresses
"""

import tilelang_dsl as pto

_NEG1_F32 = -1.0


@pto.vkernel(
    target="a5",
    ops=["pto.tfillpad", "pto.tfillpad_inplace"],
)
def template_tfillpad(src: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col

    has_valid_expansion = (src_valid_cols < dst_cols) or (src_valid_rows < dst_rows)

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

        # Phase 2: Fill cols from aligned_col to dst_cols-1
        if pto.constexpr(aligned_col < dst_cols):
            for row in range(0, src_valid_rows, 1):
                remained = dst_cols - aligned_col
                for col in range(aligned_col, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

        # Phase 3: Copy tail valid lanes (overwrite fill for valid region)
        if pto.constexpr(has_tail):
            for row in range(0, src_valid_rows, 1):
                remained = src_valid_cols - aligned_col
                mask_copy, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, aligned_col:])
                pto.vsts(data, dst[row, aligned_col:], mask_copy)

        # Phase 4: Fill row expansion
        if pto.constexpr(src_rows < dst_rows):
            for row in range(src_rows, dst_rows, 1):
                remained = dst_cols
                for col in range(0, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

    # i16 handling
    elif pto.constexpr(dtype == pto.i16):
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

        if pto.constexpr(aligned_col < dst_cols):
            for row in range(0, src_valid_rows, 1):
                remained = dst_cols - aligned_col
                for col in range(aligned_col, dst_cols, lanes):
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
                remained = dst_cols
                for col in range(0, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

    # si16 handling
    elif pto.constexpr(dtype == pto.si16):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()  # Returns i16, need vcvt to si16
        else:
            fill_scalar = pto.i16(0)  # Use signless i16(0), need vcvt to si16

        for row in range(0, src_valid_rows, 1):
            remained = aligned_col
            for col in range(0, aligned_col, lanes):
                mask, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, col:])
                pto.vsts(data, dst[row, col:], mask)

        if pto.constexpr(aligned_col < dst_cols):
            for row in range(0, src_valid_rows, 1):
                remained = dst_cols - aligned_col
                for col in range(aligned_col, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_si16 = pto.vcvt(vec, pto.si16, mask)
                    pto.vsts(vec_si16, dst[row, col:], mask)

        if pto.constexpr(has_tail):
            for row in range(0, src_valid_rows, 1):
                remained = src_valid_cols - aligned_col
                mask_copy, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, aligned_col:])
                pto.vsts(data, dst[row, aligned_col:], mask_copy)

        if pto.constexpr(src_rows < dst_rows):
            for row in range(src_rows, dst_rows, 1):
                remained = dst_cols
                for col in range(0, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_si16 = pto.vcvt(vec, pto.si16, mask)
                    pto.vsts(vec_si16, dst[row, col:], mask)

    # ui16 handling
    elif pto.constexpr(dtype == pto.ui16):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()  # Returns i16, need vcvt to ui16
        else:
            fill_scalar = pto.i16(0)  # Use signless i16(0), need vcvt to ui16

        for row in range(0, src_valid_rows, 1):
            remained = aligned_col
            for col in range(0, aligned_col, lanes):
                mask, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, col:])
                pto.vsts(data, dst[row, col:], mask)

        if pto.constexpr(aligned_col < dst_cols):
            for row in range(0, src_valid_rows, 1):
                remained = dst_cols - aligned_col
                for col in range(aligned_col, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_ui16 = pto.vcvt(vec, pto.ui16, mask)
                    pto.vsts(vec_ui16, dst[row, col:], mask)

        if pto.constexpr(has_tail):
            for row in range(0, src_valid_rows, 1):
                remained = src_valid_cols - aligned_col
                mask_copy, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, aligned_col:])
                pto.vsts(data, dst[row, aligned_col:], mask_copy)

        if pto.constexpr(src_rows < dst_rows):
            for row in range(src_rows, dst_rows, 1):
                remained = dst_cols
                for col in range(0, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_ui16 = pto.vcvt(vec, pto.ui16, mask)
                    pto.vsts(vec_ui16, dst[row, col:], mask)

    # i32 handling
    elif pto.constexpr(dtype == pto.i32):
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

        if pto.constexpr(aligned_col < dst_cols):
            for row in range(0, src_valid_rows, 1):
                remained = dst_cols - aligned_col
                for col in range(aligned_col, dst_cols, lanes):
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
                remained = dst_cols
                for col in range(0, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

    # si32 handling
    elif pto.constexpr(dtype == pto.si32):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()  # Returns i32, need vcvt to si32
        else:
            fill_scalar = pto.i32(0)  # Use signless i32(0), need vcvt to si32

        for row in range(0, src_valid_rows, 1):
            remained = aligned_col
            for col in range(0, aligned_col, lanes):
                mask, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, col:])
                pto.vsts(data, dst[row, col:], mask)

        if pto.constexpr(aligned_col < dst_cols):
            for row in range(0, src_valid_rows, 1):
                remained = dst_cols - aligned_col
                for col in range(aligned_col, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_si32 = pto.vcvt(vec, pto.si32, mask)
                    pto.vsts(vec_si32, dst[row, col:], mask)

        if pto.constexpr(has_tail):
            for row in range(0, src_valid_rows, 1):
                remained = src_valid_cols - aligned_col
                mask_copy, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, aligned_col:])
                pto.vsts(data, dst[row, aligned_col:], mask_copy)

        if pto.constexpr(src_rows < dst_rows):
            for row in range(src_rows, dst_rows, 1):
                remained = dst_cols
                for col in range(0, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_si32 = pto.vcvt(vec, pto.si32, mask)
                    pto.vsts(vec_si32, dst[row, col:], mask)

    # ui32 handling
    elif pto.constexpr(dtype == pto.ui32):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()  # Returns i32, need vcvt to ui32
        else:
            fill_scalar = pto.i32(0)  # Use signless i32(0), need vcvt to ui32

        for row in range(0, src_valid_rows, 1):
            remained = aligned_col
            for col in range(0, aligned_col, lanes):
                mask, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, col:])
                pto.vsts(data, dst[row, col:], mask)

        if pto.constexpr(aligned_col < dst_cols):
            for row in range(0, src_valid_rows, 1):
                remained = dst_cols - aligned_col
                for col in range(aligned_col, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_ui32 = pto.vcvt(vec, pto.ui32, mask)
                    pto.vsts(vec_ui32, dst[row, col:], mask)

        if pto.constexpr(has_tail):
            for row in range(0, src_valid_rows, 1):
                remained = src_valid_cols - aligned_col
                mask_copy, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, aligned_col:])
                pto.vsts(data, dst[row, aligned_col:], mask_copy)

        if pto.constexpr(src_rows < dst_rows):
            for row in range(src_rows, dst_rows, 1):
                remained = dst_cols
                for col in range(0, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_ui32 = pto.vcvt(vec, pto.ui32, mask)
                    pto.vsts(vec_ui32, dst[row, col:], mask)

    # i8 handling
    elif pto.constexpr(dtype == pto.i8):
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

        if pto.constexpr(aligned_col < dst_cols):
            for row in range(0, src_valid_rows, 1):
                remained = dst_cols - aligned_col
                for col in range(aligned_col, dst_cols, lanes):
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
                remained = dst_cols
                for col in range(0, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    pto.vsts(vec, dst[row, col:], mask)

    # si8 handling
    elif pto.constexpr(dtype == pto.si8):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()  # Returns i8, need vcvt to si8
        else:
            fill_scalar = pto.i8(0)  # Use signless i8(0), need vcvt to si8

        for row in range(0, src_valid_rows, 1):
            remained = aligned_col
            for col in range(0, aligned_col, lanes):
                mask, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, col:])
                pto.vsts(data, dst[row, col:], mask)

        if pto.constexpr(aligned_col < dst_cols):
            for row in range(0, src_valid_rows, 1):
                remained = dst_cols - aligned_col
                for col in range(aligned_col, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_si8 = pto.vcvt(vec, pto.si8, mask)
                    pto.vsts(vec_si8, dst[row, col:], mask)

        if pto.constexpr(has_tail):
            for row in range(0, src_valid_rows, 1):
                remained = src_valid_cols - aligned_col
                mask_copy, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, aligned_col:])
                pto.vsts(data, dst[row, aligned_col:], mask_copy)

        if pto.constexpr(src_rows < dst_rows):
            for row in range(src_rows, dst_rows, 1):
                remained = dst_cols
                for col in range(0, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_si8 = pto.vcvt(vec, pto.si8, mask)
                    pto.vsts(vec_si8, dst[row, col:], mask)

    # ui8 handling
    elif pto.constexpr(dtype == pto.ui8):
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            fill_scalar = dst.pad_value.eval()  # Returns i8, need vcvt to ui8
        else:
            fill_scalar = pto.i8(0)  # Use signless i8(0), need vcvt to ui8

        for row in range(0, src_valid_rows, 1):
            remained = aligned_col
            for col in range(0, aligned_col, lanes):
                mask, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, col:])
                pto.vsts(data, dst[row, col:], mask)

        if pto.constexpr(aligned_col < dst_cols):
            for row in range(0, src_valid_rows, 1):
                remained = dst_cols - aligned_col
                for col in range(aligned_col, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_ui8 = pto.vcvt(vec, pto.ui8, mask)
                    pto.vsts(vec_ui8, dst[row, col:], mask)

        if pto.constexpr(has_tail):
            for row in range(0, src_valid_rows, 1):
                remained = src_valid_cols - aligned_col
                mask_copy, remained = pto.make_mask(dtype, remained)
                data = pto.vlds(src[row, aligned_col:])
                pto.vsts(data, dst[row, aligned_col:], mask_copy)

        if pto.constexpr(src_rows < dst_rows):
            for row in range(src_rows, dst_rows, 1):
                remained = dst_cols
                for col in range(0, dst_cols, lanes):
                    mask, remained = pto.make_mask(dtype, remained)
                    vec = pto.vdup(fill_scalar, mask)
                    vec_ui8 = pto.vcvt(vec, pto.ui8, mask)
                    pto.vsts(vec_ui8, dst[row, col:], mask)

    return