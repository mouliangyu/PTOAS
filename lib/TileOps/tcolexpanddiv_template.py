"""TileLang DSL template for pto.tcolexpanddiv"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tcolexpanddiv"
)
def template_tcolexpanddiv(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            lhs = pto.vlds(src0[row, col:])
            rhs = pto.vlds(src1[0, col:])
            # TODO: 当前使用普通精度版本，后续需要添加高精度版本（vdivh）
            result = pto.vdiv(lhs, rhs, mask)
            pto.vsts(result, dst[row, col:], mask)
    return