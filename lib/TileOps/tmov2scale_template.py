# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tmov2scale - Mat to Scaling/FB buffer movement.

This template implements the TMOV2SCALE operation for cube kernels:
  - Source: L1 Mat buffer (1xN row-major layout)
  - Destination: L0 Fixpipe Buffer (FB)
  - Uses mte_l1_fb intrinsic operation

The Fixpipe Buffer (FB) is a 4KB buffer used for storing quantization
scale parameters in the fixpipe quantization flow. Requirements:
  - Row dimension must be 1
  - Column dimension * sizeof(dtype) must be aligned to 128 bits
  - Total size must not exceed 4KB (4096 bytes)

The FB buffer is used with TMOV_FP for quantized output storage.
"""

import tilelang_dsl as pto


@pto.ckernel(
    target="a5",
    op="pto.tmov2scale",
    dtypes=[
        (pto.i64, pto.i64),
    ],
)
def template_tmov2scale(src: pto.Tile, dst: pto.Tile):
    """Move data from Mat buffer to Fixpipe Buffer (Scaling).

    Args:
        src: Source tile in L1 Mat location (1xN row-major)
        dst: Destination tile in Scaling/FB location

    The scale parameters are stored as uint64 values containing
    quantization configuration (M1, offset, sign fields).
    """
    # Scale has shape 1xN
    _, n = dst.valid_shape
    # Burst transfer to FB buffer
    pto.fb_load(src.as_ptr(), dst.as_ptr(), n)
    return