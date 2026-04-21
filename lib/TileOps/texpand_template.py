"""TileLang DSL template for pto.texpands"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.texpands"
)
def template_texpands(scalar: pto.AnyType, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            # Use vbr for scalar broadcast (vdup requires position for vector input)
            vec = pto.vdup(scalar, mask)
            pto.vsts(vec, dst[row, col:], mask)

    return
