# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Shared helpers for full-axis merge-axis TileOp implementations."""

import tilelang_dsl as pto


def full_axis_constraint(**attrs):
    """Match only full-axis tiles whose valid extents equal their allocation shape."""
    matched_any_tile = False
    for attr_name, shape in attrs.items():
        if not attr_name.endswith("_shape") or attr_name.endswith("_valid_shape"):
            continue
        valid_shape = attrs.get(attr_name.replace("_shape", "_valid_shape"))
        if valid_shape is None:
            continue
        matched_any_tile = True
        if tuple(shape) != tuple(valid_shape):
            return False
    return matched_any_tile


@pto.inline_proc
def emit_unary_merge_axis(src: pto.Tile, dst: pto.Tile, compute):
    """Lower a full-axis unary element-wise kernel through one linearized loop."""
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape
    total_elems = valid_rows * valid_cols
    lanes = pto.get_lanes(dtype)

    with pto.strict_vecscope(dst, src, total_elems, 0, total_elems, lanes) as (
        out_tile,
        in_tile,
        area,
        lb,
        ub,
        step,
    ):
        remained = area
        for lane in range(lb, ub, step):
            mask, remained = pto.make_mask(out_tile.element_type, remained)
            result = compute(pto.vlds(in_tile, lane), mask)
            pto.vsts(result, out_tile, lane, mask)


@pto.inline_proc
def emit_binary_merge_axis(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile, compute):
    """Lower a full-axis binary element-wise kernel through one linearized loop."""
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
            result = compute(pto.vlds(lhs_tile, lane), pto.vlds(rhs_tile, lane), mask)
            pto.vsts(result, out_tile, lane, mask)
