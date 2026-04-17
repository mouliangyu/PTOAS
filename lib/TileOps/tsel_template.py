"""TileLang DSL template for pto.tsel

NOTE: This template uses pto.vlds for mask loading which requires
dtype matching between mask and data vectors. The mask tile should
use the same dtype as src0/src1/dst for proper vsel operation.
"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tsel",
    dtypes=[(pto.AnyFloat, pto.AnyFloat, pto.AnyFloat, pto.AnyFloat, pto.AnyFloat),(pto.i8, pto.i8, pto.i8, pto.i8, pto.i8)],  # 
    advanced=True
)
def template_tsel(mask: pto.Tile, src0: pto.Tile, src1: pto.Tile, tmp: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape

    lanes = pto.get_lanes(dtype)
    if pto.constexpr(dtype == pto.f16):
        zero_scalar = pto.f16(0.0)
    elif pto.constexpr(dtype == pto.i8):
        zero_scalar = pto.i8(0)
    else:
        zero_scalar = pto.f32(0.0)

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, lanes):
            pred_mask, remained = pto.make_mask(dtype, remained)
            mask_vec = pto.vlds(mask[row, col:])
            select_mask = pto.vcmps(mask_vec, zero_scalar, pred_mask, "ne")
            lhs = pto.vlds(src0[row, col:])
            rhs = pto.vlds(src1[row, col:])
            selected = pto.vsel(lhs, rhs, select_mask)
            pto.vsts(selected, dst[row, col:], pred_mask)
    return