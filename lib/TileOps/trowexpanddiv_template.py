"""TileLang DSL template for pto.trowexpanddiv"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.trowexpanddiv",
    dtypes=[(pto.AnyFloat, pto.AnyFloat, pto.AnyFloat), (pto.AnyInt, pto.AnyInt, pto.AnyInt)],
)
def template_trowexpanddiv(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    """Template for pto.trowexpanddiv.

    Divide each row of src0 by a per-row scalar from src1[row, 0].
    Semantics: dst[row, col] = src0[row, col] / src1[row, 0]
    """
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            # Load the scalar vector from src1[row, :]
            # For row-major src1, valid_shape[1] is 32/sizeof(dtype) (e.g., 8 for f32)
            # vdup broadcasts the first element to the full vector width
            scalar_vec = pto.vlds(src1[row, :])
            broadcasted = pto.vdup(scalar_vec, mask)
            lhs = pto.vlds(src0[row, col:])
            result = pto.vdiv(lhs, broadcasted, mask)
            pto.vsts(result, dst[row, col:], mask)
    return