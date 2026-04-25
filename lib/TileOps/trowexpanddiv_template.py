"""TileLang DSL template for pto.trowexpanddiv"""

import sys
from pathlib import Path
import tilelang_dsl as pto


def _constraint_trowexpanddiv_row_major(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile) -> bool:
    """Constraint for RowMajor layout trowexpanddiv template."""
    # All tiles must be RowMajor layout
    src0_row_major = src0.config.b_layout == pto.BLayout.ROW_MAJOR
    src1_row_major = src1.config.b_layout == pto.BLayout.ROW_MAJOR
    dst_row_major = dst.config.b_layout == pto.BLayout.ROW_MAJOR
    return src0_row_major and src1_row_major and dst_row_major


@pto.vkernel(
    target="a5",
    op="pto.trowexpanddiv",
    dtypes=[(pto.f32, pto.f32, pto.f32)],
    constraints=[_constraint_trowexpanddiv_row_major],
)
def template_trowexpanddiv_f32(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    """Template for pto.trowexpanddiv with f32 dtype.

    Divide each row of src0 by a per-row scalar from src1[row, 0].
    Semantics: dst[row, col] = src0[row, col] / src1[row, 0]
    """
    valid_rows, valid_cols = dst.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(pto.f32)):
            mask, remained = pto.make_mask(pto.f32, remained)
            scalar_vec = pto.vlds(src1[row, :])
            broadcasted = pto.vdup(scalar_vec, mask)
            lhs = pto.vlds(src0[row, col:])
            result = pto.vdiv(lhs, broadcasted, mask)
            pto.vsts(result, dst[row, col:], mask)
    return


@pto.vkernel(
    target="a5",
    op="pto.trowexpanddiv",
    dtypes=[(pto.f16, pto.f16, pto.f16)],
    constraints=[_constraint_trowexpanddiv_row_major],
)
def template_trowexpanddiv_f16(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    """Template for pto.trowexpanddiv with f16 dtype.

    Divide each row of src0 by a per-row scalar from src1[row, 0].
    Semantics: dst[row, col] = src0[row, col] / src1[row, 0]
    """
    valid_rows, valid_cols = dst.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(pto.f16)):
            mask, remained = pto.make_mask(pto.f16, remained)
            scalar_vec = pto.vlds(src1[row, :])
            broadcasted = pto.vdup(scalar_vec, mask)
            lhs = pto.vlds(src0[row, col:])
            result = pto.vdiv(lhs, broadcasted, mask)
            pto.vsts(result, dst[row, col:], mask)
    return