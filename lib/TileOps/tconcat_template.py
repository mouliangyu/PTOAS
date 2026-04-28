# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

import tilelang_dsl as pto

def _supports_tconcat(src0, src1, dst) -> bool:
    if src0.rank != 2 or src1.rank != 2 or dst.rank != 2:
        return False
    if src0.config.b_layout != pto.BLayout.ROW_MAJOR:
        return False
    if src1.config.b_layout != pto.BLayout.ROW_MAJOR:
        return False
    if dst.config.b_layout != pto.BLayout.ROW_MAJOR:
        return False
    if src0.dtype != dst.dtype or src1.dtype != dst.dtype:
        return False
    if src0.valid_shape[0] != dst.valid_shape[0]:
        return False
    if src1.valid_shape[0] != dst.valid_shape[0]:
        return False
    if src0.valid_shape[1] + src1.valid_shape[1] != dst.valid_shape[1]:
        return False
    return True

def _supports_tconcat_idx(src0, src1, src0Idx, src1Idx, dst) -> bool:
    if src0.rank != 2 or src1.rank != 2 or src0Idx.rank != 2 or src1Idx.rank != 2 or dst.rank != 2:
        return False
    if src0.config.b_layout != pto.BLayout.ROW_MAJOR:
        return False
    if src1.config.b_layout != pto.BLayout.ROW_MAJOR:
        return False
    if dst.config.b_layout != pto.BLayout.ROW_MAJOR:
        return False
    if src0Idx.config.b_layout != pto.BLayout.ROW_MAJOR:
        return False
    if src1Idx.config.b_layout != pto.BLayout.ROW_MAJOR:
        return False
    if src0.valid_shape[0] != dst.valid_shape[0] or src1.valid_shape[0] != dst.valid_shape[0]:
        return False
    if src0Idx.valid_shape[0] != dst.valid_shape[0] or src1Idx.valid_shape[0] != dst.valid_shape[0]:
        return False
    if src0Idx.shape[1] < 1 or src1Idx.shape[1] < 1:
        return False
    return True

@pto.vkernel(
    target="a5",
    op="pto.tconcat",
    advanced=True,
    constraints=[_supports_tconcat],
)
def template_tconcat(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    # concate with two tiles
    dtype = dst.element_type
    elem_bytes = pto.bytewidth(dtype)
    lanes = pto.get_lanes(dtype)
    valid_rows = dst.valid_shape[0]
    valid_cols0 = src0.valid_shape[1]
    valid_cols1 = src1.valid_shape[1]
    remained0_init = pto.i32(valid_cols0)
    remained1_init = pto.i32(valid_cols1)
    dst_row_stride = dst.shape[1]
    dst_ptr = dst.as_ptr()

    for row in range(0, valid_rows, 1):
        remained0 = remained0_init
        for col in range(0, valid_cols0, lanes):
            mask0, remained0 = pto.make_mask(dtype, remained0)
            vec0 = pto.vlds(src0[row, col:])
            pto.vsts(vec0, dst[row, col:], mask0)

        pto.mem_bar(pto.BarrierType.VST_VLD)

        remained1 = remained1_init
        if pto.constexpr(elem_bytes < 4):
            pack_factor = 4 // elem_bytes
            packed_dst_ptr = pto.castptr(dst_ptr, pto.ptr(pto.i32, pto.MemorySpace.UB))
            packed_lanes_i32 = pto.i32(pto.get_lanes(pto.i32))
            pack_factor_i32 = pto.i32(pack_factor)
            for col in range(0, valid_cols1, lanes):
                vec1 = pto.vlds(src1[row, col:])
                packed_vec1 = pto.vbitcast(vec1, pto.i32)
                active_words = remained1 // pack_factor_i32
                if active_words > packed_lanes_i32:
                    active_words = packed_lanes_i32

                base = pto.i32((row * dst_row_stride + valid_cols0 + col) // pack_factor)
                offsets = pto.vci(base, pto.OrderMode.ASC)
                packed_mask, _ = pto.make_mask(pto.i32, active_words)
                pto.vscatter(packed_vec1, packed_dst_ptr, offsets, packed_mask)
                remained1 = remained1 - active_words * pack_factor_i32
        else:
            lanes_i32 = pto.i32(lanes)
            for col in range(0, valid_cols1, lanes):
                active_lanes = remained1
                if active_lanes > lanes_i32:
                    active_lanes = lanes_i32

                base = pto.i32(row * dst_row_stride + valid_cols0 + col)
                vec1 = pto.vlds(src1[row, col:])
                offsets = pto.vci(base, pto.OrderMode.ASC)
                mask1, _ = pto.make_mask(dtype, active_lanes)
                pto.vscatter(vec1, dst_ptr, offsets, mask1)
                remained1 = remained1 - active_lanes
    return

@pto.vkernel(
    target="a5",
    op="pto.tconcatidx",
    advanced=True,
    constraints=[_supports_tconcat_idx],
)
def template_tconcat_idx(src0: pto.Tile, src1: pto.Tile, src0Idx: pto.Tile, src1Idx: pto.Tile, dst: pto.Tile):
    # concate with two tiles specified by two index tiles
    dtype = dst.element_type
    lanes = pto.get_lanes(dtype)
    idx_dtype = src0Idx.element_type
    idx_elem_bytes = pto.bytewidth(idx_dtype)
    idx_shift_bits = 0
    if pto.constexpr(idx_elem_bytes == 2):
        idx_shift_bits = 1
    elif pto.constexpr(idx_elem_bytes == 4):
        idx_shift_bits = 2
    elif pto.constexpr(idx_elem_bytes == 8):
        idx_shift_bits = 3
    dst_ptr = dst.as_ptr()

    valid_rows, dst_valid_cols = dst.valid_shape
    dst_row_stride = dst.shape[1]
    full_mask_b32 = pto.pset_b32(pto.PAT.ALL)
    lane_ids = pto.vci(pto.i32(0), pto.OrderMode.ASC)
    dst_valid_cols_vec = pto.vbr(pto.i32(dst_valid_cols))

    for row in range(0, valid_rows, 1):
        idx0_bytes = pto.vlds(src0Idx[row, 0:], dist=pto.VLoadDist.BRC_B32)
        idx1_bytes = pto.vlds(src1Idx[row, 0:], dist=pto.VLoadDist.BRC_B32)
        if pto.constexpr(idx_shift_bits == 0):
            idx0_num = idx0_bytes
            idx1_num = idx1_bytes
        else:
            idx0_num = pto.vshrs(idx0_bytes, pto.i16(idx_shift_bits), full_mask_b32)
            idx1_num = pto.vshrs(idx1_bytes, pto.i16(idx_shift_bits), full_mask_b32)

        src0_overflow = pto.vcmp(idx0_num, dst_valid_cols_vec, full_mask_b32, pto.CmpMode.GT)
        src0_cols = pto.vsel(dst_valid_cols_vec, idx0_num, src0_overflow)
        src1_capacity = pto.vsub(dst_valid_cols_vec, src0_cols, full_mask_b32)
        src1_overflow = pto.vcmp(idx1_num, src1_capacity, full_mask_b32, pto.CmpMode.GT)
        src1_cols = pto.vsel(src1_capacity, idx1_num, src1_overflow)

        for col in range(0, dst_valid_cols, lanes):
            col_vec = pto.vbr(pto.i32(col))
            remained0 = pto.vsub(src0_cols, col_vec, full_mask_b32)
            mask0 = pto.vcmp(lane_ids, remained0, full_mask_b32, pto.CmpMode.LT)
            vec0 = pto.vlds(src0[row, col:])
            pto.vsts(vec0, dst[row, col:], mask0)

        pto.mem_bar(pto.BarrierType.VST_VLD)

        for col in range(0, dst_valid_cols, lanes):
            col_vec = pto.vbr(pto.i32(col))
            active_lanes = pto.vsub(src1_cols, col_vec, full_mask_b32)
            mask1 = pto.vcmp(lane_ids, active_lanes, full_mask_b32, pto.CmpMode.LT)
            base = pto.vci(pto.i32(row * dst_row_stride + col), pto.OrderMode.ASC)
            offsets = pto.vadd(base, src0_cols, full_mask_b32)
            vec1 = pto.vlds(src1[row, col:])
            pto.vscatter(vec1, dst_ptr, offsets, mask1)
    return
