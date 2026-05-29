# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tmov2bias - Mat to Bias buffer movement.

This template implements the TMOV2BIAS operation for cube kernels:
  - Source: L1 Mat buffer (1xN row-major layout)
  - Destination: L0 Bias Table buffer
  - Uses mte_l1_bt intrinsic operation

The Bias Table is a special 4KB buffer in L0 used for bias addition
in matmul operations. Requirements:
  - Row dimension must be 1
  - Column dimension * sizeof(dtype) must be aligned to 64 bits
  - Total size must not exceed 4KB (4096 bytes)

Supported dtypes:
  - float32, float16, bfloat16, int32 (with type conversion)
"""

import tilelang_dsl as pto


@pto.ckernel(
    target="a5",
    op="pto.tmov2bias",
    dtypes=[
        (pto.f32, pto.f32),
        (pto.f16, pto.f32),
        (pto.bf16, pto.f32),
        (pto.i32, pto.i32),
    ],
)
def template_tmov2bias(src: pto.Tile, dst: pto.Tile):
    """Move data from Mat buffer to Bias Table buffer.

    Args:
        src: Source tile in L1 Mat location (1xN row-major)
        dst: Destination tile in Bias Table location

    The bias data is moved using burst transfer to the Bias Table.
    """
    # Bias has shape 1xN, we derive N from the valid shape
    _, n = dst.valid_shape
    # Calculate burst parameters
    # For bias: len_burst = ceil(N * sizeof(dtype) / 32)
    # For simplicity, use single burst for aligned cases
    pto.bias_load(src.as_ptr(), dst.as_ptr(), n)
    return