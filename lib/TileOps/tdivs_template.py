# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tdivs with IEEE 754 high-precision support

Supports two operand orders:
  1. tdivs(src_tile, scalar, dst) -> src / scalar
  2. tdivs(scalar, src_tile, dst) -> scalar / src

High-precision mode uses IEEE 754 compliant division algorithms from div_hp module
for improved accuracy with precision-sensitive, subnormal, and overflow boundary cases.
"""

import sys
from pathlib import Path
import tilelang_dsl as pto
from merge_axis import emit_binary_merge_axis, emit_unary_merge_axis, full_axis_constraint

# Import shared high-precision division algorithms
from div_hp import _div_ieee754_f32_impl, _div_ieee754_f16_impl


@pto.vkernel(
    target="a5",
    op="pto.tdivs",
    constraints=[full_axis_constraint],
    priority=100,
    advanced=True,
)
def template_tdivs_merge_axis_tile_scalar(src: pto.Tile, scalar: pto.AnyType, dst: pto.Tile):
    dtype = src.element_type
    valid_rows, valid_cols = dst.valid_shape
    total_elems = valid_rows * valid_cols
    lanes = pto.get_lanes(dtype)
    with pto.strict_vecscope(dst, src, scalar, total_elems, 0, total_elems, lanes) as (
        out_tile,
        in_tile,
        scalar_value,
        area,
        lb,
        ub,
        step,
    ):
        scalar_vec = pto.vbr(scalar_value)
        precision_mode = pto.get_op_attr("precision_mode", "DEFAULT")
        remained = area
        for lane in range(lb, ub, step):
            mask, remained = pto.make_mask(out_tile.element_type, remained)
            vec = pto.vlds(in_tile, lane)
            if pto.constexpr(precision_mode == "HIGH_PRECISION"):
                if pto.constexpr(out_tile.element_type == pto.f32):
                    result = _div_ieee754_f32_impl(vec, scalar_vec, mask)
                else:
                    result = _div_ieee754_f16_impl(vec, scalar_vec, mask)
            else:
                result = pto.vdiv(vec, scalar_vec, mask)
            pto.vsts(result, out_tile, lane, mask)
    return


@pto.vkernel(
    target="a5",
    op="pto.tdivs",
    constraints=[full_axis_constraint],
    priority=100,
    advanced=True,
)
def template_tdivs_merge_axis_scalar_tile(scalar: pto.AnyType, src: pto.Tile, dst: pto.Tile):
    dtype = src.element_type
    valid_rows, valid_cols = dst.valid_shape
    total_elems = valid_rows * valid_cols
    lanes = pto.get_lanes(dtype)
    with pto.strict_vecscope(dst, src, scalar, total_elems, 0, total_elems, lanes) as (
        out_tile,
        in_tile,
        scalar_value,
        area,
        lb,
        ub,
        step,
    ):
        scalar_vec = pto.vbr(scalar_value)
        precision_mode = pto.get_op_attr("precision_mode", "DEFAULT")
        remained = area
        for lane in range(lb, ub, step):
            mask, remained = pto.make_mask(out_tile.element_type, remained)
            vec = pto.vlds(in_tile, lane)
            if pto.constexpr(precision_mode == "HIGH_PRECISION"):
                if pto.constexpr(out_tile.element_type == pto.f32):
                    result = _div_ieee754_f32_impl(scalar_vec, vec, mask)
                else:
                    result = _div_ieee754_f16_impl(scalar_vec, vec, mask)
            else:
                result = pto.vdiv(scalar_vec, vec, mask)
            pto.vsts(result, out_tile, lane, mask)
    return


@pto.vkernel(
    target="a5",
    op="pto.tdivs",
)
def template_tdivs_tile_scalar(src: pto.Tile, scalar: pto.AnyType, dst: pto.Tile):
    """src / scalar with optional high-precision mode"""
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    precision_mode = pto.get_op_attr("precision_mode", "DEFAULT")
    if pto.constexpr(precision_mode == "HIGH_PRECISION"):
        for row in range(0, valid_rows, 1):
            remained = valid_cols
            for col in range(0, valid_cols, pto.get_lanes(dtype)):
                mask, remained = pto.make_mask(dtype, remained)
                vec = pto.vlds(src[row, col:])
                scalar_vec = pto.vbr(scalar)
                if pto.constexpr(dtype == pto.f32):
                    result = _div_ieee754_f32_impl(vec, scalar_vec, mask)
                else:  # dtype == pto.f16 (guaranteed by MLIR validation)
                    result = _div_ieee754_f16_impl(vec, scalar_vec, mask)
                pto.vsts(result, dst[row, col:], mask)
    else:
        for row in range(0, valid_rows, 1):
            remained = valid_cols
            for col in range(0, valid_cols, pto.get_lanes(dtype)):
                mask, remained = pto.make_mask(dtype, remained)
                vec = pto.vlds(src[row, col:])
                scalar_vec = pto.vbr(scalar)
                result = pto.vdiv(vec, scalar_vec, mask)
                pto.vsts(result, dst[row, col:], mask)
    return


@pto.vkernel(
    target="a5",
    op="pto.tdivs",
)
def template_tdivs_scalar_tile(scalar: pto.AnyType, src: pto.Tile, dst: pto.Tile):
    """scalar / src with optional high-precision mode"""
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    precision_mode = pto.get_op_attr("precision_mode", "DEFAULT")
    if pto.constexpr(precision_mode == "HIGH_PRECISION"):
        for row in range(0, valid_rows, 1):
            remained = valid_cols
            for col in range(0, valid_cols, pto.get_lanes(dtype)):
                mask, remained = pto.make_mask(dtype, remained)
                vec = pto.vlds(src[row, col:])
                scalar_vec = pto.vbr(scalar)
                if pto.constexpr(dtype == pto.f32):
                    result = _div_ieee754_f32_impl(scalar_vec, vec, mask)
                else:  # dtype == pto.f16 (guaranteed by MLIR validation)
                    result = _div_ieee754_f16_impl(scalar_vec, vec, mask)
                pto.vsts(result, dst[row, col:], mask)
    else:
        for row in range(0, valid_rows, 1):
            remained = valid_cols
            for col in range(0, valid_cols, pto.get_lanes(dtype)):
                mask, remained = pto.make_mask(dtype, remained)
                vec = pto.vlds(src[row, col:])
                scalar_vec = pto.vbr(scalar)
                result = pto.vdiv(scalar_vec, vec, mask)
                pto.vsts(result, dst[row, col:], mask)
    return
