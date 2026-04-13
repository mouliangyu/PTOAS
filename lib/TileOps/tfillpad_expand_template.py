"""TileLang DSL template for pto.tfillpad_expand"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tfillpad_expand"
)
def template_tfillpad_expand(dst: pto.Tile, src: pto.Tile):
    dtype = dst.element_type
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape
    pad_value = dst.pad_value
    
    lanes = pto.get_lanes(dtype)
    
    # 步骤1: 复制源 Tile 的有效数据到目标 Tile
    for row in range(0, src_valid_rows, 1):
        remained = src_valid_cols
        for col in range(0, src_valid_cols, lanes):
            mask, remained = pto.make_mask(dtype, remained)
            data = pto.vlds(src[row, col:])
            pto.vsts(data, dst[row, col:], mask)
    
    # 步骤2: 填充源数据区域的行尾 padding
    pad_cols_in_src = dst_cols - src_valid_cols
    if pad_cols_in_src > 0:
        for row in range(0, src_valid_rows, 1):
            for col in range(src_valid_cols, dst_cols, lanes):
                remained_pad = dst_cols - col
                mask, _ = pto.make_mask(dtype, remained_pad)
                vec_pad = pto.vdup(pad_value, mask)
                pto.vstus(vec_pad, dst[row, col:], mask)
    
    # 步骤3: 填充扩展的行（src_valid_rows 之后的所有行）
    pad_rows = dst_rows - src_valid_rows
    if pad_rows > 0:
        total_pad_elements = pad_rows * dst_cols
        for idx in range(0, total_pad_elements, lanes):
            remained_pad = total_pad_elements - idx
            mask, _ = pto.make_mask(dtype, remained_pad)
            vec_pad = pto.vdup(pad_value, mask)
            start_row = src_valid_rows + idx // dst_cols
            start_col = idx % dst_cols
            pto.vsts(vec_pad, dst[start_row, start_col:], mask)
    
    return
