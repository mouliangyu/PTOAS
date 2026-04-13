"""TileLang DSL template for pto.tfillpad"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tfillpad"
)
def template_tfillpad(dst: pto.Tile, src: pto.Tile):
    dtype = dst.element_type
    src_valid_rows, src_valid_cols = src.valid_shape
    dst_rows, dst_cols = dst.shape
    pad_value = dst.pad_value
    
    lanes = pto.get_lanes(dtype)
    
    # 步骤1: 复制有效数据区域
    for row in range(0, src_valid_rows, 1):
        remained = src_valid_cols
        for col in range(0, src_valid_cols, lanes):
            mask, remained = pto.make_mask(dtype, remained)
            data = pto.vlds(src[row, col:])
            pto.vsts(data, dst[row, col:], mask)
    
    # 步骤2: 填充行尾 padding（每行有效列之后的区域）
    pad_cols = dst_cols - src_valid_cols
    if pad_cols > 0:
        for row in range(0, src_valid_rows, 1):
            for col in range(src_valid_cols, dst_cols, lanes):
                remained_pad = dst_cols - col
                mask, _ = pto.make_mask(dtype, remained_pad)
                vec_pad = pto.vdup(pad_value, mask)
                pto.vstus(vec_pad, dst[row, col:], mask)
    
    # 步骤3: 填充行尾 padding（有效行之后的所有区域）
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
