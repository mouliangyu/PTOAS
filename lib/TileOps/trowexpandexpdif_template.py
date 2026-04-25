# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You can not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.trowexpandexpdif"""

import sys
from pathlib import Path
import tilelang_dsl as pto


def _constraint_trowexpandexpdif_row_major(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile) -> bool:
    """Constraint for RowMajor layout trowexpandexpdif template."""
    # All tiles must be RowMajor layout
    src0_row_major = src0.config.b_layout == pto.BLayout.ROW_MAJOR
    src1_row_major = src1.config.b_layout == pto.BLayout.ROW_MAJOR
    dst_row_major = dst.config.b_layout == pto.BLayout.ROW_MAJOR
    return src0_row_major and src1_row_major and dst_row_major


@pto.vkernel(
    target="a5",
    op="pto.trowexpandexpdif",
    dtypes=[(pto.f32, pto.f32, pto.f32)],
    constraints=[_constraint_trowexpandexpdif_row_major],
)
def template_trowexpandexpdif_f32(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    """Template for pto.trowexpandexpdif with f32 dtype.

    Compute exp(src0 - scalar) for each row using per-row scalars from src1[row, 0].
    Semantics: dst[row, col] = exp(src0[row, col] - src1[row, 0])
    Used in numerically stable softmax computation.
    """
    valid_rows, valid_cols = dst.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(pto.f32)):
            mask, remained = pto.make_mask(pto.f32, remained)
            scalar_vec = pto.vlds(src1[row, :])
            broadcasted = pto.vdup(scalar_vec, mask)
            lhs = pto.vlds(src0[row, col:])
            result = pto.vexpdif(lhs, broadcasted, pto.VcvtPartMode.EVEN)
            pto.vsts(result, dst[row, col:], mask)
    return


@pto.vkernel(
    target="a5",
    op="pto.trowexpandexpdif",
    dtypes=[(pto.f16, pto.f16, pto.f16)],
    constraints=[_constraint_trowexpandexpdif_row_major],
)
def template_trowexpandexpdif_f16(src0: pto.Tile, src1: pto.Tile, dst: pto.Tile):
    """Template for pto.trowexpandexpdif with f16 dtype.

    Compute exp(src0 - scalar) for each row using per-row scalars from src1[row, 0].
    Semantics: dst[row, col] = exp(src0[row, col] - src1[row, 0])
    Used in numerically stable softmax computation.

    Data flow (processing 128 f16 columns per iteration):
    - vlds loads 128 f16 elements (with UNPK_B16 unpacking for consistency)
    - vdup broadcasts the scalar value to 128 lanes (b16 mask)
    - vexpdif: 128 f16 inputs -> 64 f32 outputs
    - vcvt: 64 f32 inputs -> 128 f16 outputs (b32 mask for input)
    - vsts: stores 128 f16 with b32 mask (PK_B32)
    """
    valid_rows, valid_cols = dst.valid_shape
    # Use f32 lanes (64) for iteration: each iteration stores 128 f16 columns
    f32_lanes = pto.get_lanes(pto.f32)
    # b16 full mask for vlds/vdup/vcvt input operations (128 positions)
    full_mask_b16 = pto.make_mask(pto.f16, pto.PAT.ALL)
    # b32 full mask for vcvt f32->f16 (64 positions controlling 128 f16)
    full_mask_b32 = pto.make_mask(pto.f32, pto.PAT.ALL)

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        # Iterate by f32 lanes because that's the intermediate result count
        for col in range(0, valid_cols, f32_lanes):
            # Create b32 mask for final storage (controls 64 positions -> 128 f16 elements)
            store_mask, remained = pto.make_mask(pto.f32, remained)
            # Load f16 inputs - each vlds returns f16 vector with 128 lanes
            scalar_vec = pto.vlds(src1[row, :], dist=pto.VLoadDist.UNPK_B16)
            lhs_vec = pto.vlds(src0[row, col:], dist=pto.VLoadDist.UNPK_B16)
            # Broadcast scalar with b16 full mask
            broadcasted = pto.vdup(scalar_vec, full_mask_b16)
            # vexpdif: f16 inputs -> f32 result (64 lanes)
            result_f32 = pto.vexpdif(lhs_vec, broadcasted, pto.VcvtPartMode.EVEN)
            # vcvt: f32 -> f16, mask must match input type (f32, so b32)
            result_f16 = pto.vcvt(
                result_f32,
                pto.f16,
                full_mask_b32,
                rnd=pto.VcvtRoundMode.R,
                sat=pto.VcvtSatMode.SAT,
                part=pto.VcvtPartMode.EVEN,
            )
            # Store with b32 mask using PK_B32 distribution
            pto.vsts(result_f16, dst[row, col:], store_mask, dist=pto.VStoreDist.PK_B32)
    return