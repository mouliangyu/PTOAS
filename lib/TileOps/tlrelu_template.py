"""TileLang DSL template for pto.tlrelu (Leaky ReLU with scalar slope)"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tlrelu",
    advanced=True
)
def template_tlrelu(src: pto.Tile, slope: pto.f32, dst: pto.Tile):
    """Leaky ReLU: dst = src if src > 0 else src * slope.
    
    Semantics:
    For each element (i, j):
        dst[i, j] = src[i, j] > 0 ? src[i, j] : slope * src[i, j]
    
    Supported data types: f16, f32
    """
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape
    
    lanes = pto.get_lanes(dtype)
    if pto.constexpr(dtype == pto.f16):
        zero_scalar = pto.f16(0.0)
        slope_scalar = pto.f16(slope)
    else:
        zero_scalar = pto.f32(0.0)
        slope_scalar = slope
    
    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, lanes):
            mask, remained = pto.make_mask(dtype, remained)
            src_vec = pto.vlds(src[row, col:])
            positive_mask = pto.vcmps(src_vec, zero_scalar, mask, "gt")
            scaled = pto.vmuls(src_vec, slope_scalar, mask)
            result = pto.vsel(src_vec, scaled, positive_mask)
            pto.vsts(result, dst[row, col:], mask)
    return