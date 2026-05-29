# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You can not use this file in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tmov.fp - Acc to Mat with quantization.

This template implements the TMOV_FP operation for cube kernels:
  - Source: L0C Accumulator buffer
  - Scaling: FB buffer with quantization parameters
  - Destination: L1 Mat buffer (quantized output)
  - Uses tmov.fp intrinsic operation

This is part of the fixpipe quantization path where:
  1. Matmul results are accumulated in L0C (int32/float)
  2. Scale parameters are loaded into FB buffer
  3. TMOV_FP performs quantization: Acc * scale -> quantized output

Supported scenarios:
  - int32 accumulator -> int8 output with scale
  - float32 accumulator -> float16/int8 output with scale
"""

import tilelang_dsl as pto


@pto.ckernel(
    target="a5",
    op="pto.tmov.fp",
    dtypes=[
        (pto.i32, pto.i64, pto.i8),
        (pto.f32, pto.i64, pto.f16),
        (pto.f32, pto.i64, pto.bf16),
    ],
)
def template_tmov_fp(src: pto.Tile, scale: pto.Tile, dst: pto.Tile):
    """Move and quantize data from Acc to Mat with scaling parameters.

    Args:
        src: Source tile in Acc location (accumulator)
        scale: Scaling tile in FB location (quantization params)
        dst: Destination tile in Mat location (quantized output)

    The tmov.fp operation performs fixpipe quantization.
    """
    # The tmov.fp op takes src (Acc), fp (scaling), and dst (Mat)
    pto.tmov_fp(src.as_ptr(), scale.as_ptr(), dst.as_ptr())
    return