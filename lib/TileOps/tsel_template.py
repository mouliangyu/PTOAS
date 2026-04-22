"""TileLang DSL template for pto.tsel

NOTE: This template uses pto.plds for mask loading which directly
loads predicate mask from UB without vcmps comparison.
This approach matches the TSel.hpp implementation in pto-isa.

Mask tile format:
- For f32: mask uses ui8 (packed predicate, 1 byte per lane)
- For f16/i8: mask uses ui32 (packed predicate, 32-bit aligned)

REQUIRES: tilelang_dsl support for plds, pintlv_b16, castptr operations
"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tsel",
    dtypes=[
        (pto.f32, pto.f32, pto.f32, pto.f32, pto.f32),
        (pto.f16, pto.f16, pto.f16, pto.f16, pto.f16),
        (pto.i8, pto.i8, pto.i8, pto.i8, pto.i8),
    ],
    advanced=True
)
def template_tsel(mask: pto.Tile, src0: pto.Tile, src1: pto.Tile, tmp: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape

    lanes = pto.get_lanes(dtype)
    
    mask_base_ptr = mask.as_ptr()
    
    if pto.constexpr(dtype == pto.f32):
        mask_ptr = pto.castptr(mask_base_ptr, pto.ptr(pto.ui8, pto.MemorySpace.UB))
    else:
        mask_ptr = pto.castptr(mask_base_ptr, pto.ptr(pto.ui32, pto.MemorySpace.UB))

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, lanes):
            pred_mask, remained = pto.make_mask(dtype, remained)
            if pto.constexpr(dtype == pto.f32):
                select_mask = pto.plds(mask_ptr, row * valid_cols + col, pto.PredicateDist.US)
                select_mask0, select_mask1 = pto.pintlv_b16(select_mask, pto.pset_b16(pto.MaskPattern.ALL))
                lhs = pto.vlds(src0[row, col:])
                rhs = pto.vlds(src1[row, col:])
                selected = pto.vsel(lhs, rhs, select_mask0)
                pto.vsts(selected, dst[row, col:], pred_mask)
            elif pto.constexpr(dtype == pto.f16):
                select_mask = pto.plds(mask_ptr, row * valid_cols + col, pto.PredicateDist.US)
                lhs = pto.vlds(src0[row, col:])
                rhs = pto.vlds(src1[row, col:])
                selected = pto.vsel(lhs, rhs, select_mask)
                pto.vsts(selected, dst[row, col:], pred_mask)
            else:
                select_mask = pto.plds(mask_ptr, row * valid_cols + col, pto.PredicateDist.NORM)
                lhs = pto.vlds(src0[row, col:])
                rhs = pto.vlds(src1[row, col:])
                selected = pto.vsel(lhs, rhs, select_mask)
                pto.vsts(selected, dst[row, col:], pred_mask)
    return