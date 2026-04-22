"""TileLang DSL template for pto.tshls"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tshls",
)
def template_tshls(src: pto.Tile, scalar: pto.i16, dst: pto.Tile):
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            vec = pto.vlds(src[row, col:])
            result = pto.vshls(vec, scalar, mask)
            pto.vsts(result, dst[row, col:], mask)
    return
