"""TileLang DSL template for pto.tcmps

Note: A5 hardware implements tcmps as packed comparison with scalar:
  dst = packed_mask(src cmp scalar)

Uses vcmps + psts(PK) to produce packed predicate mask output.
psts PK mode stores VL/16 = 16 bytes per iteration, so byte_offset
increments by 16 to maintain 16-byte alignment.

Only supports 32-bit source types (f32, i32) with ui8 destination.
"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tcmps",
    dtypes=[(pto.f32, pto.f32, pto.ui8), (pto.i32, pto.i32, pto.ui8)],
    advanced=True,
)
def template_tcmps(src: pto.Tile, scalar: pto.AnyType, dst: pto.Tile):
    """src cmp scalar -> packed mask in dst (ui8)

    Follows TCmps pattern from TCmps.hpp:
    - Each iteration: vcmps -> psts(PK)
    - psts PK mode stores VL/16 = 16 bytes per iteration
    - byte_offset = col * 16 // lanes to maintain alignment
    """
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    lanes = pto.get_lanes(dtype)  # 64 for f32/i32
    bytes_per_iter = 16  # VL/16 = 256/16 = 16 bytes per psts PK iteration
    iters_per_row = (valid_cols + lanes - 1) // lanes

    dst_ptr = dst.as_ptr()

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, lanes):
            mask, remained = pto.make_mask(dtype, remained)

            vec = pto.vlds(src[row, col:])
            cmp = pto.vcmps(vec, scalar, mask, "lt")
            # Global byte offset accounts for both row and column position
            byte_offset = (row * iters_per_row + col // lanes) * bytes_per_iter
            pto.psts(cmp, dst_ptr, byte_offset, pto.PredicateDist.PK)

    return
