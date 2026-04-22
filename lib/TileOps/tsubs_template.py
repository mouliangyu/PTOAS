"""TileLang DSL template for pto.tsubs

Note: A5 hardware implements tsubs as vadds with negated scalar:
  dst = src - scalar = src + (-scalar)
This template uses vbr + vsub to achieve element-wise subtraction.
"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tsubs",
)
def template_tsubs(src: pto.Tile, scalar: pto.AnyType, dst: pto.Tile):
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            vec = pto.vlds(src[row, col:])
            scalar_vec = pto.vbr(scalar)
            result = pto.vsub(vec, scalar_vec, mask)
            pto.vsts(result, dst[row, col:], mask)
    return
