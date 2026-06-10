# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tsub"""

import sys
from pathlib import Path
import tilelang_dsl as pto
from merge_axis import emit_binary_merge_axis, emit_unary_merge_axis, full_axis_constraint


@pto.vkernel(
    target="a5",
    op="pto.tsub",
    constraints=[full_axis_constraint],
    priority=100,
    advanced=True,
)
def template_tsub_merge_axis(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape
    total_elems = valid_rows * valid_cols
    lanes = pto.get_lanes(dtype)

    with pto.strict_vecscope(dst, src0, src1, total_elems, 0, total_elems, lanes) as (
        out_tile,
        lhs_tile,
        rhs_tile,
        area,
        lb,
        ub,
        step,
    ):
        remained = area
        for lane in range(lb, ub, step):
            mask, remained = pto.make_mask(out_tile.element_type, remained)
            lhs = pto.vlds(lhs_tile, lane)
            rhs = pto.vlds(rhs_tile, lane)
            result = pto.vsub(lhs, rhs, mask)
            pto.vsts(result, out_tile, lane, mask)
    return


@pto.vkernel(
    target="a5",
    op="pto.tsub"
)
def template_tsub(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            lhs = pto.vlds(src0[row, col:])
            rhs = pto.vlds(src1[row, col:])
            subtracted = pto.vsub(lhs, rhs, mask)
            pto.vsts(subtracted, dst[row, col:], mask)
    return