"""TileLang DSL template for pto.tcolexpandexpdif"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tcolexpandexpdif"
)
def template_tcolexpandexpdif(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            lhs = pto.vlds(src0[row, col:])
            rhs = pto.vlds(src1[0, col:])
            diff = pto.vsub(lhs, rhs, mask)
            result = pto.vexp(diff, mask)
            pto.vsts(result, dst[row, col:], mask)
    return