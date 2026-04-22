"""TileLang DSL template for pto.tdivs

Supports two operand orders (matching TDivS.hpp):
  1. tdivs(src_tile, scalar, dst) -> src / scalar
  2. tdivs(scalar, src_tile, dst) -> scalar / src
"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tdivs",
)
def template_tdivs_tile_scalar(src: pto.Tile, scalar: pto.AnyType, dst: pto.Tile):
    """src / scalar"""
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            vec = pto.vlds(src[row, col:])
            scalar_vec = pto.vbr(scalar)
            result = pto.vdiv(vec, scalar_vec, mask)
            pto.vsts(result, dst[row, col:], mask)
    return


@pto.vkernel(
    target="a5",
    op="pto.tdivs",
)
def template_tdivs_scalar_tile(scalar: pto.AnyType, src: pto.Tile, dst: pto.Tile):
    """scalar / src"""
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            vec = pto.vlds(src[row, col:])
            scalar_vec = pto.vbr(scalar)
            result = pto.vdiv(scalar_vec, vec, mask)
            pto.vsts(result, dst[row, col:], mask)
    return
