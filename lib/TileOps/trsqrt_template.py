# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.trsqrt"""

import tilelang_dsl as pto
from merge_axis import emit_binary_merge_axis, emit_unary_merge_axis, full_axis_constraint

# TODO: Add implementation for HIGH_PRECISION type
@pto.vkernel(
    target="a5",
    op="pto.trsqrt",
    dtypes=[(pto.f16, pto.f16), (pto.f32, pto.f32)],
    constraints=[full_axis_constraint],
    priority=100,
    advanced=True,
)
def template_trsqrt_merge_axis(src: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape
    total_elems = valid_rows * valid_cols
    lanes = pto.get_lanes(dtype)
    if pto.constexpr(dtype == pto.f32):
        with pto.strict_vecscope(dst, src, pto.f32(1.0), total_elems, 0, total_elems, lanes) as (
            out_tile,
            in_tile,
            one_value,
            area,
            lb,
            ub,
            step,
        ):
            remained = area
            for lane in range(lb, ub, step):
                mask, remained = pto.make_mask(out_tile.element_type, remained)
                vec = pto.vlds(in_tile, lane)
                result = pto.vdiv(pto.vbr(one_value), pto.vsqrt(vec, mask), mask)
                pto.vsts(result, out_tile, lane, mask)
    else:
        with pto.strict_vecscope(dst, src, pto.f16(1.0), total_elems, 0, total_elems, lanes) as (
            out_tile,
            in_tile,
            one_value,
            area,
            lb,
            ub,
            step,
        ):
            remained = area
            for lane in range(lb, ub, step):
                mask, remained = pto.make_mask(out_tile.element_type, remained)
                vec = pto.vlds(in_tile, lane)
                result = pto.vdiv(pto.vbr(one_value), pto.vsqrt(vec, mask), mask)
                pto.vsts(result, out_tile, lane, mask)
    return


@pto.vkernel(
    target="a5",
    op="pto.trsqrt",
    dtypes=[(pto.f16, pto.f16), (pto.f32, pto.f32)]
)
def template_trsqrt(src: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            vinput = pto.vlds(src[row, col:])
            if pto.constexpr(dtype == pto.f16):
                one_scalar = pto.f16(1.0)
            else:
                one_scalar = pto.f32(1.0)
            one = pto.vbr(one_scalar)
            sqrt_result = pto.vsqrt(vinput, mask)
            result = pto.vdiv(one, sqrt_result, mask)
            pto.vsts(result, dst[row, col:], mask)
    return
