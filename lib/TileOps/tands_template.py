"""TileLang DSL template for pto.tands

Note: A5 hardware implements tands as:
  TEXPANDS_IMPL(dst, scalar);  // broadcast scalar to dst
  TAND_IMPL(dst, src, dst);    // dst = src & dst

This template uses vbr + vand to achieve element-wise bitwise AND.
Only supports tile, scalar order (matching TAndS.hpp).
"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tands",
)
def template_tands(src: pto.Tile, scalar: pto.AnyType, dst: pto.Tile):
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            vec = pto.vlds(src[row, col:])
            scalar_vec = pto.vbr(scalar)
            result = pto.vand(vec, scalar_vec, mask)
            pto.vsts(result, dst[row, col:], mask)
    return
