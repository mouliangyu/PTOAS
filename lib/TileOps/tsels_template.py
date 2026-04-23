"""TileLang DSL template for pto.tsels

NOTE: This template uses pto.vdup for scalar broadcasting and pto.vsel
for conditional selection between source tile and scalar value.
"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tsels",
    dtypes=[(pto.AnyFloat, pto.AnyFloat, pto.AnyFloat, pto.AnyFloat, pto.AnyType)],
    advanced=True
)
def template_tsels(mask: pto.Tile, src: pto.Tile, tmp: pto.Tile, scalar: pto.AnyType, dst: pto.Tile):
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
            pred_mask, remained = pto.make_mask(dtype, remained)
            mask_vec = pto.vlds(mask[row, col:])
            select_mask = pto.vcmps(mask_vec, zero_scalar, pred_mask, "ne")
            src_vec = pto.vlds(src[row, col:])
            scalar_vec = pto.vdup(scalar, pred_mask)
            selected = pto.vsel(src_vec, scalar_vec, select_mask)
            pto.vsts(selected, dst[row, col:], pred_mask)
    return