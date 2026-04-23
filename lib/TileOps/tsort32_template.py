"""TileLang DSL template for pto.tsort32"""

import tilelang_dsl as pto

BLOCK_SIZE = 32
FLOAT_DST_STRIDE_COEF = 2
HALF_DST_STRIDE_COEF = 4
MAX_UB_TMP = 32 * 255  # 8160 bytes
REPEAT_MAX = 255


@pto.vkernel(
    target="a5",
    advanced=True,
    op="pto.tsort32"
)
def template_tsort32(src: pto.Tile, idx: pto.Tile, tmp: pto.Tile, dst: pto.Tile):
    """
    TSort32 Format2: Bitonic sort with tmp buffer for padding.

    Semantics (matching pto-isa TSort32.hpp):
    - Sorts src values into dst, generating indices in idx
    - Uses tmp buffer when src.valid_cols % 32 != 0 (padding needed)
    - When src.valid_cols % 32 == 0, directly uses Format1 logic (no tmp needed)
    - Pads unaligned tail with NaN to ensure correct sorting
    """
    dtype = dst.element_type
    valid_rows = dst.valid_shape[0]
    valid_cols = src.valid_shape[1]

    # Get pointers
    dst_ptr = dst.as_ptr()
    src_ptr = src.as_ptr()
    idx_ptr = idx.as_ptr()
    tmp_ptr = tmp.as_ptr()

    # Calculate strides (in elements)
    elem_bytes = pto.bytewidth(dtype)
    dst_stride = ((dst.shape[1] * elem_bytes + BLOCK_SIZE - 1) // BLOCK_SIZE * BLOCK_SIZE) // elem_bytes
    src_stride = ((src.shape[1] * elem_bytes + BLOCK_SIZE - 1) // BLOCK_SIZE * BLOCK_SIZE) // elem_bytes
    idx_stride = ((idx.shape[1] * 4 + BLOCK_SIZE - 1) // BLOCK_SIZE * BLOCK_SIZE) // 4
    if idx.valid_shape[0] == 1:
        idx_stride = 0

    # Type coefficient for destination stride
    type_coef = HALF_DST_STRIDE_COEF
    if pto.constexpr(dtype == pto.f32):
        type_coef = FLOAT_DST_STRIDE_COEF

    # Calculate repeat parameters
    repeat_num_per_row = (valid_cols + BLOCK_SIZE - 1) // BLOCK_SIZE
    src_tail_per_row = valid_cols % BLOCK_SIZE  # remaining elements in last block
    src_tail_repeat_num = ((valid_cols + BLOCK_SIZE - 1) // BLOCK_SIZE) % REPEAT_MAX

    # NaN value for padding (negative NaN) - use dtype-specific type
    if pto.constexpr(dtype == pto.f16):
        min_val = pto.f16(0xFC00) # Using -inf as a safe minimum for f16
    elif pto.constexpr(dtype == pto.bf16):
        min_val = pto.bf16(0xFF80)
    else:
        min_val = pto.f32(0xFF800000) # Using -inf as a safe minimum for f32

    # Optimization: if valid_cols % 32 == 0, use Format1 directly (no tmp needed)
    # Matching C++ TSORT32_IMPL line 208-210
    if valid_cols % BLOCK_SIZE == 0:
        if repeat_num_per_row <= REPEAT_MAX:
            for i in range(0, valid_rows, 1):
                pto.vbitsort(
                    pto.addptr(dst_ptr, i * dst_stride),
                    pto.addptr(src_ptr, i * src_stride),
                    pto.addptr(idx_ptr, i * idx_stride),
                    repeat_num_per_row
                )
        else:
            loop_num = (repeat_num_per_row + REPEAT_MAX - 1) // REPEAT_MAX
            tail_repeat_num = repeat_num_per_row % REPEAT_MAX
            for i in range(0, valid_rows, 1):
                for j in range(0, loop_num, 1):
                    repeat_num = REPEAT_MAX
                    if j == loop_num - 1:
                        repeat_num = tail_repeat_num
                        
                    pto.vbitsort(
                        pto.addptr(dst_ptr, i * dst_stride + j * REPEAT_MAX * BLOCK_SIZE * type_coef),
                        pto.addptr(src_ptr, i * src_stride + j * REPEAT_MAX * BLOCK_SIZE),
                        pto.addptr(idx_ptr, i * idx_stride + j * REPEAT_MAX * BLOCK_SIZE),
                        repeat_num
                    )
    else:
        # Check if entire row fits in tmp buffer
        src_shape_bytes_per_row = valid_cols * elem_bytes

        if src_shape_bytes_per_row <= MAX_UB_TMP:
            # Copy entire row to tmp, pad, then sort
            len_burst = (src_shape_bytes_per_row + BLOCK_SIZE - 1) // BLOCK_SIZE

            for i in range(0, valid_rows, 1):
                # Copy src row to tmp
                pto.copy_ubuf_to_ubuf(
                    pto.addptr(src_ptr, i * src_stride),
                    tmp_ptr,
                    0, 1, len_burst, 0, 0
                )

                # Pad the last unaligned 32 elements with NaN
                tmp_last_offset = ((valid_cols + BLOCK_SIZE - 1) // BLOCK_SIZE * BLOCK_SIZE) - BLOCK_SIZE

                # Load last block, pad tail with NaN
                vec = pto.vlds(tmp[0, tmp_last_offset:])
                # Create mask for elements to pad (elements >= src_tail_per_row in this block)
                # Elements 0 to src_tail_per_row-1 are valid, src_tail_per_row to 31 need padding
                # make_mask with BLOCK_SIZE - src_tail_per_row remaining gives mask for padding region
                pad_mask, _ = pto.make_mask(dtype, BLOCK_SIZE - src_tail_per_row)
                vec = pto.vdup(min_val, pad_mask)
                pto.vsts(vec, tmp[0, tmp_last_offset:], pad_mask)

                # Sort from tmp to dst
                pto.vbitsort(
                    pto.addptr(dst_ptr, i * dst_stride),
                    tmp_ptr,
                    pto.addptr(idx_ptr, i * idx_stride),
                    repeat_num_per_row
                )
        else:
            # Large buffer case: process each chunk separately (LargeTmpBufferImpl)
            loop_num = (repeat_num_per_row + REPEAT_MAX - 1) // REPEAT_MAX

            for i in range(0, valid_rows, 1):
                for j in range(0, loop_num, 1):
                    if j < loop_num - 1:
                        # Normal block: sort directly from src
                        pto.vbitsort(
                            pto.addptr(dst_ptr, i * dst_stride + j * REPEAT_MAX * BLOCK_SIZE * type_coef),
                            pto.addptr(src_ptr, i * src_stride + j * REPEAT_MAX * BLOCK_SIZE),
                            pto.addptr(idx_ptr, i * idx_stride + j * REPEAT_MAX * BLOCK_SIZE),
                            REPEAT_MAX
                        )
                    else:
                        # Last block: need padding via tmp
                        # Matching C++ LargeTmpBufferImpl lines 75-107
                        # Sort complete 32-blocks before tail (srcTailRepeatNum - 1 blocks)
                        # Note: when src_tail_repeat_num == 1, repeat=0 is valid (no pre-sort)
                        if src_tail_repeat_num > 0:
                            sort_repeat_num = 0
                            if src_tail_repeat_num > 1:
                                sort_repeat_num = src_tail_repeat_num - 1
                                
                            pto.vbitsort(
                                pto.addptr(dst_ptr, i * dst_stride + j * REPEAT_MAX * BLOCK_SIZE * type_coef),
                                pto.addptr(src_ptr, i * src_stride + j * REPEAT_MAX * BLOCK_SIZE),
                                pto.addptr(idx_ptr, i * idx_stride + j * REPEAT_MAX * BLOCK_SIZE),
                                sort_repeat_num
                            )

                        # Copy tail src to tmp, pad, then sort
                        tail_src_offset = (j * REPEAT_MAX + (src_tail_repeat_num - 1)) * BLOCK_SIZE
                        tail_dst_offset = (j * REPEAT_MAX + (src_tail_repeat_num - 1)) * BLOCK_SIZE * type_coef
                        len_burst = (src_tail_per_row * elem_bytes + BLOCK_SIZE - 1) // BLOCK_SIZE

                        pto.copy_ubuf_to_ubuf(
                            pto.addptr(src_ptr, i * src_stride + tail_src_offset),
                            tmp_ptr,
                            0, 1, len_burst, 0, 0
                        )

                        # Pad the last 32 elements in tmp
                        tmp_last_offset = ((src_tail_per_row + BLOCK_SIZE - 1) // BLOCK_SIZE * BLOCK_SIZE) - BLOCK_SIZE

                        # Load last block, pad tail with NaN
                        vec = pto.vlds(tmp[0, tmp_last_offset:])
                        pad_mask, _ = pto.make_mask(dtype, BLOCK_SIZE - src_tail_per_row)
                        vec = pto.vdup(min_val, pad_mask)
                        pto.vsts(vec, tmp[0, tmp_last_offset:], pad_mask)

                        # Sort from tmp to dst tail
                        pto.vbitsort(
                            pto.addptr(dst_ptr, i * dst_stride + tail_dst_offset),
                            tmp_ptr,
                            pto.addptr(idx_ptr, i * idx_stride + tail_src_offset),
                            1
                        )

    return