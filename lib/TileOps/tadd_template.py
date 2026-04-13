"""TileLang DSL template for pto.tadd"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.inline_proc
def my_func(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
  dtype = dst.element_type
  valid_rows, valid_cols = dst.valid_shape

  for row in range(0, valid_rows, 1):
      remained = valid_cols
      for col in range(0, valid_cols, pto.get_lanes(dtype)):
          mask, remained = pto.make_mask(dtype, remained)
          lhs = pto.vlds(src0[row, col:])
          rhs = pto.vlds(src1[row, col:])
          summed = pto.vadd(lhs, rhs, mask)
          pto.vsts(summed, dst[row, col:], mask)
  return


@pto.inline_proc
def store_row(dst: pto.Tile, src: pto.Tile, row: pto.i32):
    vec = pto.vlds(src[row, 0:])
    mask = pto.make_mask(dst.element_type, pto.PAT.ALL)
    pto.vsts(vec, dst[row, 0:], mask)
    return None


@pto.vkernel(
    target="a5",
    op="pto.tadd"
)
def template_tadd(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    my_func(src0, src1, dst)

    # dtype = dst.element_type
    # valid_rows, valid_cols = dst.valid_shape

    # for row in range(0, valid_rows, 1):
    #     remained = valid_cols
    #     for col in range(0, valid_cols, pto.get_lanes(dtype)):
    #         mask, remained = pto.make_mask(dtype, remained)
    #         lhs = pto.vlds(src0[row, col:])
    #         rhs = pto.vlds(src1[row, col:])
    #         summed = pto.vadd(lhs, rhs, mask)
    #         pto.vsts(summed, dst[row, col:], mask)

    # store_row(dst, src0, 0)
    # vec = pto.vlds(src0[0, 0:])
    # mask = pto.make_mask(dst.element_type, pto.PAT.ALL)
    # pto.vsts(vec, dst[0, 0:], mask)
    return
