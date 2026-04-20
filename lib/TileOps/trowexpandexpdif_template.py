"""TileLang DSL template for pto.trowexpandexpdif"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.trowexpandexpdif",
    dtypes=[(pto.AnyFloat, pto.AnyFloat, pto.AnyFloat)],
)
def template_trowexpandexpdif(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    """Template for pto.trowexpandexpdif.

    Compute exp(src0 - scalar) for each row using per-row scalars from src1[row, 0].
    Semantics: dst[row, col] = exp(src0[row, col] - src1[row, 0])
    Used in numerically stable softmax computation.
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
            # Compute lhs - broadcasted scalar (vector-vector operation)
            diff = pto.vsub(lhs, broadcasted, mask)
            # Compute exp(diff)
            result = pto.vexp(diff, mask)
            pto.vsts(result, dst[row, col:], mask)
    return