# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tlog"""

import tilelang_dsl as pto
from merge_axis import emit_binary_merge_axis, emit_unary_merge_axis, full_axis_constraint


@pto.inline_proc
def _tlog_high_precision(src: pto.Tile, dst: pto.Tile, dtype, valid_rows, valid_cols):
    if pto.constexpr(dtype == pto.f16):
        subnormal_threshold = pto.f16("0x03FF")
        mul_factor = pto.f16("0x6400")
        compensation = pto.f16(-6.931471805599453094172)
    elif pto.constexpr(dtype == pto.f32):
        subnormal_threshold = pto.f32("0x007FFFFF")
        mul_factor = pto.f32("0x4B000000")
        compensation = pto.f32(-15.9423851528787421)

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            vinput = pto.vlds(src[row, col:])
            cmp_mask = pto.vcmps(vinput, subnormal_threshold, mask, pto.CmpMode.LT)
            scaled = pto.vmuls(vinput, mul_factor, mask)
            selected_input = pto.vsel(scaled, vinput, cmp_mask)
            log_result = pto.vln(selected_input, mask)
            compensated = pto.vadds(log_result, compensation, mask)
            result = pto.vsel(compensated, log_result, cmp_mask)
            pto.vsts(result, dst[row, col:], mask)
    return None


@pto.inline_proc
def _tlog_default(src: pto.Tile, dst: pto.Tile, dtype, valid_rows, valid_cols):
    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            mask, remained = pto.make_mask(dtype, remained)
            vinput = pto.vlds(src[row, col:])
            result = pto.vln(vinput, mask)
            pto.vsts(result, dst[row, col:], mask)
    return None


@pto.vkernel(
    target="a5",
    op="pto.tlog",
    constraints=[full_axis_constraint],
    priority=100,
    advanced=True,
)
def template_tlog_merge_axis(src: pto.Tile, dst: pto.Tile):
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
        precision_mode = pto.get_op_attr("precision_mode", "DEFAULT")
        if pto.constexpr(precision_mode == "HIGH_PRECISION"):
            if pto.constexpr(out_tile.element_type == pto.f16):
                subnormal_threshold = pto.f16("0x03FF")
                mul_factor = pto.f16("0x6400")
                compensation = pto.f16(-6.931471805599453094172)
            elif pto.constexpr(out_tile.element_type == pto.f32):
                subnormal_threshold = pto.f32("0x007FFFFF")
                mul_factor = pto.f32("0x4B000000")
                compensation = pto.f32(-15.9423851528787421)
        remained = area
        for lane in range(lb, ub, step):
            mask, remained = pto.make_mask(out_tile.element_type, remained)
            vec = pto.vlds(in_tile, lane)
            if pto.constexpr(precision_mode == "HIGH_PRECISION"):
                cmp_mask = pto.vcmps(vec, subnormal_threshold, mask, pto.CmpMode.LT)
                scaled = pto.vmuls(vec, mul_factor, mask)
                selected_input = pto.vsel(scaled, vec, cmp_mask)
                log_result = pto.vln(selected_input, mask)
                compensated = pto.vadds(log_result, compensation, mask)
                result = pto.vsel(compensated, log_result, cmp_mask)
            else:
                result = pto.vln(vec, mask)
            pto.vsts(result, out_tile, lane, mask)
    return


@pto.vkernel(
    target="a5",
    op="pto.tlog",
    advanced=True
)
def template_tlog(src: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = dst.valid_shape
    precision_mode = pto.get_op_attr("precision_mode", "DEFAULT")

    if pto.constexpr(precision_mode == "HIGH_PRECISION"):
        _tlog_high_precision(src, dst, dtype, valid_rows, valid_cols)
    else:
        _tlog_default(src, dst, dtype, valid_rows, valid_cols)
    return
