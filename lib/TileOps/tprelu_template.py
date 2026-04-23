"""TileLang DSL template for pto.tprelu (Parametric ReLU with per-element slope)"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tprelu",
    dtypes=[(pto.AnyFloat, pto.AnyFloat, pto.AnyFloat, pto.AnyFloat)],
    advanced=True
)
def template_tprelu(src0: pto.Tile, src1: pto.Tile, tmp: pto.Tile, dst: pto.Tile):
    """Parametric ReLU: dst = src0 if src0 > 0 else src1 * src0.
    
    Semantics:
    For each element (i, j):
        dst[i, j] = src0[i, j] > 0 ? src0[i, j] : src1[i, j] * src0[i, j]
    
    Supported data types: f16, f32
    Note: tmp is a placeholder for A5 (not used in implementation)
    """
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape
    
    lanes = pto.get_lanes(dtype)
    if pto.constexpr(dtype == pto.f16):
        zero_scalar = pto.f16(0.0)
    else:
        zero_scalar = pto.f32(0.0)
    
    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, lanes):
            mask, remained = pto.make_mask(dtype, remained)
            src0_vec = pto.vlds(src0[row, col:])
            src1_vec = pto.vlds(src1[row, col:])
            positive_mask = pto.vcmps(src0_vec, zero_scalar, mask, "gt")
            scaled = pto.vmul(src0_vec, src1_vec, mask)
            result = pto.vsel(src0_vec, scaled, positive_mask)
            pto.vsts(result, dst[row, col:], mask)
    return