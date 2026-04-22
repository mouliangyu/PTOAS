"""TileLang DSL template for pto.trems

Note: A5 hardware implements trems as:
  - float/half: dst = src - trunc(src/scalar) * scalar
  - integer: dst = src % scalar (using vmod) - NOT YET SUPPORTED

This template uses vbr + vdiv + vtrc + vmuls + vsub for the remainder computation.
Only supports tile, scalar, tmp order (matching TRemS.hpp).
"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.trems",
    dtypes=[(pto.f32, pto.f32, pto.f32, pto.f32), (pto.f16, pto.f16, pto.f16, pto.f16)],
    advanced=True,
)
def template_trems(src: pto.Tile, scalar: pto.AnyType, tmp: pto.Tile, dst: pto.Tile):
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            vec = pto.vlds(src[row, col:])
            scalar_vec = pto.vbr(scalar)
            # quotient = vec / scalar
            quotient = pto.vdiv(vec, scalar_vec, mask)
            # For floating-point: trunc(quotient) towards zero (rnd="Z")
            # For integer: vdiv already truncates towards zero, skip vtrc
            if pto.constexpr(dtype == pto.f32 or dtype == pto.f16):
                truncated = pto.vtrc(quotient, mask, rnd="Z")
            else:
                truncated = quotient
            # remainder = vec - truncated * scalar
            product = pto.vmuls(truncated, scalar, mask)
            result = pto.vsub(vec, product, mask)
            pto.vsts(result, dst[row, col:], mask)
    return
