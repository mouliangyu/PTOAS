# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You can not use this file in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tmov2right - Mat to Right buffer movement.

This template implements the TMOV2RIGHT operation for cube kernels:
  - Source: L1 Mat buffer
  - Destination: L0B Right buffer
  - Uses mte_l1_l0b intrinsic operation

The operation is part of the cube matmul data flow where:
  1. Data is loaded from GM to L1 Mat via TLOAD
  2. TMOV2RIGHT moves data from L1 Mat to L0B Right
  3. The Right buffer is then used as input for TMATMUL

Supported scenarios:
  - Mat -> Right for matmul B matrix input
  - RowMajor layout with ColMajor fractal storage (transposed)
"""

import tilelang_dsl as pto


@pto.ckernel(
    target="a5",
    op="pto.tmov2right",
    dtypes=[
        (pto.f16, pto.f16),
        (pto.bf16, pto.bf16),
        (pto.f32, pto.f32),
        (pto.i8, pto.i8),
    ],
)
def template_tmov2right(src: pto.Tile, dst: pto.Tile):
    """Move data from Mat buffer to Right buffer.

    Args:
        src: Source tile in L1 Mat location
        dst: Destination tile in L0B Right location

    The k, n dimensions are derived from the tile shapes.
    Transpose is typically enabled for Right buffer layout.
    """
    k, n = dst.valid_shape
    pto.right_load(src.as_ptr(), dst.as_ptr(), k, n, transpose=True)
    return