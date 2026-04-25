"""TileLang DSL template for pto.trowexpand"""

import sys
from pathlib import Path
import tilelang_dsl as pto


def _constraint_trowexpand_row_major(src: pto.Tile, dst: pto.Tile) -> bool:
    """Constraint for RowMajor layout trowexpand template."""
    # Both src and dst must be RowMajor layout
    src_row_major = src.config.b_layout == pto.BLayout.ROW_MAJOR
    dst_row_major = dst.config.b_layout == pto.BLayout.ROW_MAJOR
    return src_row_major and dst_row_major


@pto.vkernel(
    target="a5",
    op="pto.trowexpand",
    dtypes=[(pto.AnyFloat, pto.AnyFloat), (pto.AnyInt, pto.AnyInt)],
    constraints=[_constraint_trowexpand_row_major],
)
def template_trowexpand(src: pto.Tile, dst: pto.Tile):
    """Template for pto.trowexpand.

    Broadcast src[row, 0] to entire dst[row, :] for each row.
    Semantics: dst[row, col] = src[row, 0] for all col.
    """
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            # Load the first element of each row (src has cols=1, so entire row is the scalar)
            # vdup broadcasts the first element to the full vector width
            scalar_vec = pto.vlds(src[row, :])
            broadcasted = pto.vdup(scalar_vec, mask)
            pto.vsts(broadcasted, dst[row, col:], mask)
    return