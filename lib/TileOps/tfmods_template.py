"""TileLang DSL template for pto.tfmods

Note: A5 hardware implements tfmods as floating-point modulo with scalar:
  dst = src - trunc(src / scalar) * scalar

This uses vdiv + vtrc + vmuls + vsub sequence.
"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tfmods",
    advanced=True,
)
def template_tfmods(src: pto.Tile, scalar: pto.AnyType, dst: pto.Tile):
    """dst = src - trunc(src / scalar) * scalar"""
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            vec = pto.vlds(src[row, col:])
            
            # 1. quotient = vec / scalar
            # Broadcast scalar to vector for division
            scalar_vec = pto.vbr(scalar)
            quotient = pto.vdiv(vec, scalar_vec, mask)
            
            # 2. truncated = trunc(quotient) towards zero
            # rnd="Z" is required for correct floating-point modulo semantics.
            # Default is "R" (round to nearest), which produces wrong results.
            truncated = pto.vtrc(quotient, mask, rnd="Z")
            
            # 3. product = truncated * scalar
            product = pto.vmuls(truncated, scalar, mask)
            
            # 4. result = vec - product
            result = pto.vsub(vec, product, mask)
            
            # 5. Store result
            pto.vsts(result, dst[row, col:], mask)
    return
