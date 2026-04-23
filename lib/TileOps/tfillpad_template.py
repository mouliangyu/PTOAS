"""TileLang DSL templates for pto.tfillpad and pto.tfillpad_inplace

Semantic:
  - TFILLPAD copies src.valid data to dst
  - TFILLPAD fills cols from src.valid_cols to dst.cols with FillPadVal
  - TFILLPAD fills rows from src.rows to dst.rows with FillPadVal

Strategy:
  - Phase 1: Copy aligned valid blocks (cols 0 to aligned_col-1)
  - Phase 2: Fill cols aligned_col to dst_cols-1 with FillPadVal
  - Phase 3: Copy tail valid lanes (cols aligned_col to src_valid_cols-1)
  - Phase 4: Fill row expansion

Templates organization:
  - One @pto.vkernel per dtype, explicitly bound via dtypes=[(dtype, dtype)]
  - ExpandTileOp selects matching template based on dtype signature
  - Each template is self-contained for easy review and maintenance

Address alignment:
  - vlds/vsts require 32-byte aligned addresses
"""

import tilelang_dsl as pto

_NEG1_F32 = -1.0


# ============================================================================
# f32 template
# ============================================================================

@pto.vkernel(
    target="a5",
    ops=["pto.tfillpad", "pto.tfillpad_inplace"],
    dtypes=[(pto.f32, pto.f32)],
)
def template_tfillpad_f32(src: pto.Tile, dst: pto.Tile):
    """f32 tfillpad template."""
    dtype = pto.f32
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col
    has_valid_expansion = (src_valid_cols < dst_cols) or (src_valid_rows < dst_rows)

    # PadValue handling for f32
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

    return


# ============================================================================
# i16 family templates
# ============================================================================

@pto.vkernel(
    target="a5",
    ops=["pto.tfillpad", "pto.tfillpad_inplace"],
    dtypes=[(pto.i16, pto.i16)],
)
def template_tfillpad_i16(src: pto.Tile, dst: pto.Tile):
    """i16 (signless 16-bit) tfillpad template."""
    dtype = pto.i16
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col

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

    return


@pto.vkernel(
    target="a5",
    ops=["pto.tfillpad", "pto.tfillpad_inplace"],
    dtypes=[(pto.si16, pto.si16)],
)
def template_tfillpad_si16(src: pto.Tile, dst: pto.Tile):
    """si16 (signed 16-bit) tfillpad template."""
    dtype = pto.si16
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col

    if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
        fill_scalar = dst.pad_value.eval()
    else:
        fill_scalar = pto.si16(0)

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

    return


@pto.vkernel(
    target="a5",
    ops=["pto.tfillpad", "pto.tfillpad_inplace"],
    dtypes=[(pto.ui16, pto.ui16)],
)
def template_tfillpad_ui16(src: pto.Tile, dst: pto.Tile):
    """ui16 (unsigned 16-bit) tfillpad template."""
    dtype = pto.ui16
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col

    if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
        fill_scalar = dst.pad_value.eval()
    else:
        fill_scalar = pto.ui16(0)

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

    return


# ============================================================================
# i32 family templates
# ============================================================================

@pto.vkernel(
    target="a5",
    ops=["pto.tfillpad", "pto.tfillpad_inplace"],
    dtypes=[(pto.i32, pto.i32)],
)
def template_tfillpad_i32(src: pto.Tile, dst: pto.Tile):
    """i32 (signless 32-bit) tfillpad template."""
    dtype = pto.i32
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col

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

    return


@pto.vkernel(
    target="a5",
    ops=["pto.tfillpad", "pto.tfillpad_inplace"],
    dtypes=[(pto.si32, pto.si32)],
)
def template_tfillpad_si32(src: pto.Tile, dst: pto.Tile):
    """si32 (signed 32-bit) tfillpad template."""
    dtype = pto.si32
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col

    if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
        fill_scalar = dst.pad_value.eval()
    else:
        fill_scalar = pto.si32(0)

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

    return


@pto.vkernel(
    target="a5",
    ops=["pto.tfillpad", "pto.tfillpad_inplace"],
    dtypes=[(pto.ui32, pto.ui32)],
)
def template_tfillpad_ui32(src: pto.Tile, dst: pto.Tile):
    """ui32 (unsigned 32-bit) tfillpad template."""
    dtype = pto.ui32
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col

    if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
        fill_scalar = dst.pad_value.eval()
    else:
        fill_scalar = pto.ui32(0)

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

    return


# ============================================================================
# i8 family templates
# ============================================================================

@pto.vkernel(
    target="a5",
    ops=["pto.tfillpad", "pto.tfillpad_inplace"],
    dtypes=[(pto.i8, pto.i8)],
)
def template_tfillpad_i8(src: pto.Tile, dst: pto.Tile):
    """i8 (signless 8-bit) tfillpad template."""
    dtype = pto.i8
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col

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

    return


@pto.vkernel(
    target="a5",
    ops=["pto.tfillpad", "pto.tfillpad_inplace"],
    dtypes=[(pto.si8, pto.si8)],
)
def template_tfillpad_si8(src: pto.Tile, dst: pto.Tile):
    """si8 (signed 8-bit) tfillpad template."""
    dtype = pto.si8
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col

    if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
        fill_scalar = dst.pad_value.eval()
    else:
        fill_scalar = pto.si8(0)

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

    return


@pto.vkernel(
    target="a5",
    ops=["pto.tfillpad", "pto.tfillpad_inplace"],
    dtypes=[(pto.ui8, pto.ui8)],
)
def template_tfillpad_ui8(src: pto.Tile, dst: pto.Tile):
    """ui8 (unsigned 8-bit) tfillpad template."""
    dtype = pto.ui8
    src_rows, src_cols = src.shape
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape

    lanes = pto.get_lanes(dtype)
    aligned_col = (src_valid_cols // lanes) * lanes
    has_tail = src_valid_cols > aligned_col

    if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
        fill_scalar = dst.pad_value.eval()
    else:
        fill_scalar = pto.ui8(0)

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

    return