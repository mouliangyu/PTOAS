# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tmov2vec - Acc to Vec/UB buffer movement.

This template implements the TMOV2VEC operation for cube kernels:
  - Source: L0C Accumulator buffer
  - Destination: UB (Unified Buffer) Vec location
  - Uses mte_l0c_ub intrinsic operation

This is part of the fixpipe path where accumulator results are
moved from the cube unit to the vector unit for further processing.

Supported scenarios:
  - Acc -> Vec for post-matmul vector operations
  - NZ2ND layout conversion (fractal to row-major)
"""

import tilelang_dsl as pto


@pto.ckernel(
    target="a5",
    op="pto.tmov2vec",
    dtypes=[
        (pto.f32, pto.f32),
        (pto.i32, pto.i32),
    ],
)
def template_tmov2vec(src: pto.Tile, dst: pto.Tile):
    """Move data from Acc buffer to Vec/UB buffer.

    Args:
        src: Source tile in Acc location
        dst: Destination tile in Vec/UB location

    The m, n dimensions and strides are derived from the tile shapes.
    This performs NZ2ND layout conversion.
    """
    m, n = dst.valid_shape
    src_stride = (m + 15) // 16 * 16  # Align to 16 blocks
    dst_stride = n  # Row-major stride
    pto.acc_store_ub(src.as_ptr(), dst.as_ptr(), m, n, src_stride, dst_stride)
    return