"""TileLang DSL template for pto.tors

Note: A5 hardware implements tors as:
  TEXPANDS_IMPL(dst, scalar);  // broadcast scalar to dst
  TOR_IMPL(dst, src, dst);     // dst = src | dst

This template uses vbr + vor to achieve element-wise bitwise OR.
Only supports tile, scalar order (matching TOrS.hpp).
"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tors",
)
def template_tors(src: pto.Tile, scalar: pto.AnyType, dst: pto.Tile):
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            vec = pto.vlds(src[row, col:])
            scalar_vec = pto.vbr(scalar)
            result = pto.vor(vec, scalar_vec, mask)
            pto.vsts(result, dst[row, col:], mask)
    return
