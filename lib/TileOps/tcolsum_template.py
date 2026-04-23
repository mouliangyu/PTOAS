"""TileLang DSL template for pto.tcolsum"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tcolsum"
)
def template_tcolsum(src: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = src.valid_shape

    lanes = pto.get_lanes(dtype)
    mask_all = pto.make_mask(dtype, pto.MaskPattern.PAT_ALL)

    for col_chunk in range(0, valid_cols, lanes):
        acc = pto.vlds(src[0, col_chunk:])
        for row in range(1, valid_rows, 1):
            row_vec = pto.vlds(src[row, col_chunk:])
            acc = pto.vadd(acc, row_vec, mask_all)
        pto.vsts(acc, dst[0, col_chunk:], mask_all)

    return
