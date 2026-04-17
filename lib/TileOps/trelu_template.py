"""TileLang DSL template for pto.trelu (Elementwise ReLU)"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.trelu",
    dtypes=[(pto.f16, pto.f16), (pto.f32, pto.f32), (pto.i32, pto.i32)],
    advanced=True
)
def template_trelu(src: pto.Tile, dst: pto.Tile):
    """Elementwise ReLU: dst = max(0, src).
    
    Semantics:
    For each element (i, j):
        dst[i, j] = max(0, src[i, j])
    
    Supported data types: f16, f32, i32
    """
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape
    
    lanes = pto.get_lanes(dtype)
    
    if pto.constexpr(dtype == pto.f16):
        zero_scalar = pto.f16(0.0)
    elif pto.constexpr(dtype == pto.f32):
        zero_scalar = pto.f32(0.0)
    else:
        zero_scalar = pto.i32(0)
    
    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, lanes):
            mask, remained = pto.make_mask(dtype, remained)
            src_vec = pto.vlds(src[row, col:])
            if pto.constexpr(dtype == pto.f16 or dtype == pto.f32):
                result = pto.vrelu(src_vec, mask)
            else:
                result = pto.vmaxs(src_vec, zero_scalar, mask)
            pto.vsts(result, dst[row, col:], mask)
    return