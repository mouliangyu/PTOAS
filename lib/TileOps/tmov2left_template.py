# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You can not use this file in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tmov2left - Mat to Left buffer movement.

This template implements the TMOV2LEFT operation for cube kernels:
  - Source: L1 Mat buffer
  - Destination: L0A Left buffer
  - Uses mte_l1_l0a intrinsic operation

The operation is part of the cube matmul data flow where:
  1. Data is loaded from GM to L1 Mat via TLOAD
  2. TMOV2LEFT moves data from L1 Mat to L0A Left
  3. The Left buffer is then used as input for TMATMUL

Supported scenarios:
  - Mat -> Left for matmul A matrix input
  - ColMajor layout with RowMajor fractal storage
"""

import tilelang_dsl as pto


@pto.ckernel(
    target="a5",
    op="pto.tmov2left",
    dtypes=[
        (pto.f16, pto.f16),
        (pto.bf16, pto.bf16),
        (pto.f32, pto.f32),
        (pto.i8, pto.i8),
    ],
)
def template_tmov2left(src: pto.Tile, dst: pto.Tile):
    """Move data from Mat buffer to Left buffer.

    Args:
        src: Source tile in L1 Mat location
        dst: Destination tile in L0A Left location

    The m, k dimensions are derived from the tile shapes.
    """
    m, k = dst.valid_shape
    pto.left_load(src.as_ptr(), dst.as_ptr(), m, k)
    return