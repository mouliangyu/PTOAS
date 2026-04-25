# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tmrgsort"""

import tilelang_dsl as pto

STRUCT_SIZE = 8  # bytes per structure (value + index)
STRUCT_SIZE_SHIFT = 3  # log2(8)
BLOCK_NUM = 4


def _get_tile_dtype(tile):
    """Get dtype from tile, compatible with both Tile and _ConstraintParamView."""
    # _ConstraintParamView uses 'dtype', Tile uses 'element_type'
    if hasattr(tile, 'dtype'):
        return tile.dtype
    return tile.element_type


def _tmrgsort_check_dtypes(dst: pto.Tile, tmp: pto.Tile,
                            src0: pto.Tile, src1: pto.Tile = None,
                            src2: pto.Tile = None, src3: pto.Tile = None) -> bool:
    """Check that all tiles have the same dtype (half or float)."""
    dtype = _get_tile_dtype(dst)
    # Check dtype is half or float
    if dtype not in (pto.f16, pto.f32):
        return False
    # Check all tiles have same dtype
    if _get_tile_dtype(tmp) != dtype:
        return False
    if _get_tile_dtype(src0) != dtype:
        return False
    if src1 is not None and _get_tile_dtype(src1) != dtype:
        return False
    if src2 is not None and _get_tile_dtype(src2) != dtype:
        return False
    if src3 is not None and _get_tile_dtype(src3) != dtype:
        return False
    return True


def _tmrgsort_preconditions_1list(src, block_len, dst) -> bool:
    """Check preconditions for 1-list internal block sorting."""
    # block_len is a scalar parameter, not a tile
    return (src.shape[0] == 1 and dst.shape[0] == 1 and
            src.shape[1] == dst.shape[1] and
            _tmrgsort_check_dtypes(dst, dst, src))


def _tmrgsort_preconditions_2list(src0, src1, dst, tmp, exhausted) -> bool:
    """Check preconditions for 2-list merge sort."""
    return (
        src0.shape[0] == 1 and src1.shape[0] == 1 and
        dst.shape[0] == 1 and tmp.shape[0] == 1 and
        tmp.shape[1] >= src0.shape[1] + src1.shape[1] and
        _tmrgsort_check_dtypes(dst, tmp, src0, src1)
    )


def _tmrgsort_preconditions_3list(src0, src1, src2, dst, tmp, exhausted) -> bool:
    """Check preconditions for 3-list merge sort."""
    return (
        src0.shape[0] == 1 and src1.shape[0] == 1 and src2.shape[0] == 1 and
        dst.shape[0] == 1 and tmp.shape[0] == 1 and
        tmp.shape[1] >= src0.shape[1] + src1.shape[1] + src2.shape[1] and
        _tmrgsort_check_dtypes(dst, tmp, src0, src1, src2)
    )


def _tmrgsort_preconditions_4list(src0, src1, src2, src3, dst, tmp, exhausted) -> bool:
    """Check preconditions for 4-list merge sort."""
    return (
        src0.shape[0] == 1 and src1.shape[0] == 1 and
        src2.shape[0] == 1 and src3.shape[0] == 1 and
        dst.shape[0] == 1 and tmp.shape[0] == 1 and
        tmp.shape[1] >= src0.shape[1] + src1.shape[1] + src2.shape[1] + src3.shape[1] and
        _tmrgsort_check_dtypes(dst, tmp, src0, src1, src2, src3)
    )




@pto.inline_proc
def init_config(exhausted: pto.i1):
    """Initialize config for vmrgsort4."""
    config = pto.i64(0)
    if exhausted:
        config = config | (pto.i64(1) << pto.i64(12))  # Xt[12]: enable exhausted suspension
    config = config | pto.i64(1)  # Xt[7:0]: repeat time = 1
    return config


@pto.inline_proc
def mov_ub2ub(dst: pto.Tile, tmp: pto.Tile, dst_col):
    """Copy data from tmp buffer to dst buffer."""
    dtype = dst.element_type
    block_byte_size = 32  # BLOCK_BYTE_SIZE
    bw = pto.bytewidth(dtype)  # index type

    # Compute len_burst using index arithmetic, then cast to i64
    # dst_col and bw are index type, so the arithmetic is index arithmetic
    # len_burst = ceil(dst_col * bw / block_byte_size)
    len_burst_idx = (dst_col * bw + (block_byte_size - 1)) // block_byte_size
    len_burst = pto.i64(len_burst_idx)

    # Get pointers
    dst_ptr = dst.as_ptr()
    tmp_ptr = tmp.as_ptr()

    # copy_ubuf_to_ubuf(src, dst, sid, n_burst, len_burst, src_stride, dst_stride)
    pto.copy_ubuf_to_ubuf(tmp_ptr, dst_ptr, pto.i64(0), pto.i64(1), len_burst, pto.i64(0), pto.i64(0))


@pto.inline_proc
def get_exhausted_data():
    """Get exhausted execution counts from VMS4_SR register.

    Note: This is a placeholder for hardware integration.
    The actual implementation would use pto.set_wait_flag and pto.get_vms4_sr,
    but these may not be available in current DSL.
    """
    # Placeholder: return 0 values (actual hardware integration pending)
    return pto.i32(0)


@pto.inline_proc
def tmrgsort_2list_instr(tmp: pto.Tile, src0: pto.Tile, src1: pto.Tile,
                         src0_col, src1_col, exhausted: pto.i1):
    """Execute vmrgsort4 for 2 lists."""
    # Cast to i64 for bitwise operations (avoid i32 truncation for 48-bit data)
    src0_col_i64 = pto.i64(src0_col)
    src1_col_i64 = pto.i64(src1_col)

    count = src0_col_i64 | (src1_col_i64 << pto.i64(16))
    config = init_config(exhausted)
    config = config | (pto.i64(0b0011) << pto.i64(8))  # mask for 2 lists (src0, src1 active)

    # Get pointers for vmrgsort4
    tmp_ptr = tmp.as_ptr()
    src0_ptr = src0.as_ptr()
    src1_ptr = src1.as_ptr()

    # Pass src0 as placeholder for unused src2, src3
    pto.vmrgsort4(tmp_ptr, src0_ptr, src1_ptr, src0_ptr, src0_ptr, count, config)


@pto.inline_proc
def tmrgsort_3list_instr(tmp: pto.Tile, src0: pto.Tile, src1: pto.Tile, src2: pto.Tile,
                         src0_col, src1_col, src2_col, exhausted: pto.i1):
    """Execute vmrgsort4 for 3 lists."""
    # Cast to i64 for bitwise operations (avoid i32 truncation for 48-bit data)
    src0_col_i64 = pto.i64(src0_col)
    src1_col_i64 = pto.i64(src1_col)
    src2_col_i64 = pto.i64(src2_col)

    count = src0_col_i64 | (src1_col_i64 << pto.i64(16)) | (src2_col_i64 << pto.i64(32))
    config = init_config(exhausted)
    config = config | (pto.i64(0b0111) << pto.i64(8))  # mask for 3 lists (src0, src1, src2 active)

    # Get pointers for vmrgsort4
    tmp_ptr = tmp.as_ptr()
    src0_ptr = src0.as_ptr()
    src1_ptr = src1.as_ptr()
    src2_ptr = src2.as_ptr()

    # Pass src0 as placeholder for unused src3
    pto.vmrgsort4(tmp_ptr, src0_ptr, src1_ptr, src2_ptr, src0_ptr, count, config)


@pto.inline_proc
def tmrgsort_4list_instr(tmp: pto.Tile, src0: pto.Tile, src1: pto.Tile, src2: pto.Tile, src3: pto.Tile,
                         src0_col, src1_col, src2_col, src3_col, exhausted: pto.i1):
    """Execute vmrgsort4 for 4 lists."""
    # Cast to i64 for bitwise operations (avoid i32 truncation for 48-bit data)
    src0_col_i64 = pto.i64(src0_col)
    src1_col_i64 = pto.i64(src1_col)
    src2_col_i64 = pto.i64(src2_col)
    src3_col_i64 = pto.i64(src3_col)

    count = src0_col_i64 | (src1_col_i64 << pto.i64(16)) | (src2_col_i64 << pto.i64(32)) | (src3_col_i64 << pto.i64(48))
    config = init_config(exhausted)
    config = config | (pto.i64(0b1111) << pto.i64(8))  # mask for 4 lists

    # Get pointers for vmrgsort4
    tmp_ptr = tmp.as_ptr()
    src0_ptr = src0.as_ptr()
    src1_ptr = src1.as_ptr()
    src2_ptr = src2.as_ptr()
    src3_ptr = src3.as_ptr()

    pto.vmrgsort4(tmp_ptr, src0_ptr, src1_ptr, src2_ptr, src3_ptr, count, config)


@pto.inline_proc
def tmrgsort_1list_instr(dst: pto.Tile, src: pto.Tile,
                         num_structures, repeat_times):
    """Execute vmrgsort4 for single list (4 internal blocks)."""
    dtype = dst.element_type
    bw = pto.bytewidth(dtype)  # index type

    # num_structures and repeat_times are passed as index type from caller
    # Compute offset in elements for pointer arithmetic (index arithmetic)
    offset = num_structures * STRUCT_SIZE // bw

    # Cast to i64 for bitwise operations
    ns_i64 = pto.i64(num_structures)
    rt_i64 = pto.i64(repeat_times)

    count = ns_i64 | (ns_i64 << pto.i64(16)) | (ns_i64 << pto.i64(32)) | (ns_i64 << pto.i64(48))
    config = rt_i64 | (pto.i64(0b1111) << pto.i64(8))  # Xt[7:0] + mask for 4 lists

    # Get pointers from tiles
    dst_ptr = dst.as_ptr()
    src_ptr = src.as_ptr()

    # Compute offset pointers for the 4 source blocks (offset is index type)
    src0 = src_ptr
    src1 = pto.addptr(src_ptr, offset)
    src2 = pto.addptr(src_ptr, offset * 2)
    src3 = pto.addptr(src_ptr, offset * 3)

    # Execute vmrgsort4 with pointers
    pto.vmrgsort4(dst_ptr, src0, src1, src2, src3, count, config)
    return


@pto.vkernel(
    target="a5",
    op="pto.tmrgsort",
    advanced=True,
    constraints=[_tmrgsort_preconditions_1list],
)
def template_tmrgsort_1list(src: pto.Tile, block_len: pto.i32, dst: pto.Tile):
    """Template for tmrgsort with single input list (internal block sorting).

    Args:
        src: Source tile containing data to be sorted
        block_len: Block length in elements (passed as i32, used as hint)
                   In HPP: blockLen includes values and indices, e.g., 32 values + 32 indices -> blockLen = 64
        dst: Destination tile for sorted output

    Note: Due to DSL type constraints (i32 vs index arithmetic), block_len is derived from tile shape.
    """
    dtype = dst.element_type
    src_valid_col = src.valid_shape[1]  # index type

    bw = pto.bytewidth(dtype)  # bytes per element (index type)

    # Derive block_len from tile shape (tile columns divided by 4 blocks)
    # This matches the expected block partition for 4-way merge sort
    tile_cols = dst.shape[1]  # index type (static tile column count)
    block_len_idx = tile_cols // BLOCK_NUM  # elements per block

    # Compute num_structures: how many 8-byte structures fit in a block
    # HPP: numStrcutures = blockLen * sizeof(dtype) >> STRUCT_SIZE_SHIFT
    num_structures_idx = (block_len_idx * bw) // STRUCT_SIZE  # index type

    # Compute repeat_times: how many times to repeat the 4-block sorting
    # HPP: repeatTimes = srcCol / (blockLen * BLOCK_NUM)
    repeat_times_idx = src_valid_col // (block_len_idx * BLOCK_NUM)  # index type

    tmrgsort_1list_instr(dst, src, num_structures_idx, repeat_times_idx)

    return None

@pto.vkernel(
    target="a5",
    op="pto.tmrgsort",
    advanced=True,
    constraints=[_tmrgsort_preconditions_2list],
)
def template_tmrgsort_2list(src0: pto.Tile, src1: pto.Tile,
                            dst: pto.Tile, tmp: pto.Tile,
                            exhausted: pto.i1):
    """Template for tmrgsort with 2 input lists."""
    dtype = dst.element_type

    # ELE_NUM_SHIFT: float -> 1 (divide by 2), half -> 2 (divide by 4)
    # A structure is 8 bytes: float (4 bytes) -> 2 elems per struct, half (2 bytes) -> 4 elems per struct
    elem_divisor = 4  # default for f16: 8 bytes / 2 bytes = 4 elems per struct
    if pto.constexpr(dtype == pto.f32):
        elem_divisor = 2  # for f32: 8 bytes / 4 bytes = 2 elems per struct

    src0_valid_col = src0.valid_shape[1]  # index type
    src1_valid_col = src1.valid_shape[1]  # index type
    dst_valid_col = dst.valid_shape[1]  # index type

    # Convert valid_col (index) to scalar, applying elem_divisor
    # This converts element count to structure count
    src0_col = pto.i32(src0_valid_col // elem_divisor)
    src1_col = pto.i32(src1_valid_col // elem_divisor)

    tmrgsort_2list_instr(tmp, src0, src1, src0_col, src1_col, exhausted)

    if exhausted:
        # Note: get_exhausted_data() is a placeholder for hardware integration
        _ = get_exhausted_data()
        return None

    mov_ub2ub(dst, tmp, dst_valid_col)
    return None


@pto.vkernel(
    target="a5",
    op="pto.tmrgsort",
    advanced=True,
    constraints=[_tmrgsort_preconditions_3list],
)
def template_tmrgsort_3list(src0: pto.Tile, src1: pto.Tile, src2: pto.Tile,
                            dst: pto.Tile, tmp: pto.Tile,
                            exhausted: pto.i1):
    """Template for tmrgsort with 3 input lists."""
    dtype = dst.element_type

    # ELE_NUM_SHIFT: float -> 1 (divide by 2), half -> 2 (divide by 4)
    elem_divisor = 4  # default for f16
    if pto.constexpr(dtype == pto.f32):
        elem_divisor = 2  # for f32

    src0_valid_col = src0.valid_shape[1]  # index type
    src1_valid_col = src1.valid_shape[1]  # index type
    src2_valid_col = src2.valid_shape[1]  # index type
    dst_valid_col = dst.valid_shape[1]  # index type

    # Convert valid_col (index) to scalar, applying elem_divisor
    src0_col = pto.i32(src0_valid_col // elem_divisor)
    src1_col = pto.i32(src1_valid_col // elem_divisor)
    src2_col = pto.i32(src2_valid_col // elem_divisor)

    tmrgsort_3list_instr(tmp, src0, src1, src2, src0_col, src1_col, src2_col, exhausted)

    if exhausted:
        # Note: get_exhausted_data() is a placeholder for hardware integration
        _ = get_exhausted_data()
        return None

    mov_ub2ub(dst, tmp, dst_valid_col)
    return None


@pto.vkernel(
    target="a5",
    op="pto.tmrgsort",
    advanced=True,
    constraints=[_tmrgsort_preconditions_4list],
)
def template_tmrgsort_4list(src0: pto.Tile, src1: pto.Tile, src2: pto.Tile, src3: pto.Tile,
                            dst: pto.Tile, tmp: pto.Tile,
                            exhausted: pto.i1):
    """Template for tmrgsort with 4 input lists."""
    dtype = dst.element_type

    # ELE_NUM_SHIFT: float -> 1 (divide by 2), half -> 2 (divide by 4)
    elem_divisor = 4  # default for f16
    if pto.constexpr(dtype == pto.f32):
        elem_divisor = 2  # for f32

    src0_valid_col = src0.valid_shape[1]  # index type
    src1_valid_col = src1.valid_shape[1]  # index type
    src2_valid_col = src2.valid_shape[1]  # index type
    src3_valid_col = src3.valid_shape[1]  # index type
    dst_valid_col = dst.valid_shape[1]  # index type

    # Convert valid_col (index) to scalar, applying elem_divisor
    src0_col = pto.i32(src0_valid_col // elem_divisor)
    src1_col = pto.i32(src1_valid_col // elem_divisor)
    src2_col = pto.i32(src2_valid_col // elem_divisor)
    src3_col = pto.i32(src3_valid_col // elem_divisor)

    tmrgsort_4list_instr(tmp, src0, src1, src2, src3, src0_col, src1_col, src2_col, src3_col, exhausted)

    if exhausted:
        # Note: get_exhausted_data() is a placeholder for hardware integration
        _ = get_exhausted_data()
        return None

    mov_ub2ub(dst, tmp, dst_valid_col)
    return None