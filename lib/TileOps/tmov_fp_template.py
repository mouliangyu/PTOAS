# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tmov.fp - Acc to Mat with quantization.

This template implements the TMOV_FP scenario for cube kernels:
  - Source: L0C Accumulator buffer (memory_space="acc")
  - Scaling: FB buffer with quantization parameters (memory_space="scaling")
  - Destination: L1 Mat buffer (memory_space="mat", quantized output)
  - Uses fixpipe intrinsic operation

This is part of the fixpipe quantization path where:
  1. Matmul results are accumulated in L0C (int32/float)
  2. Scale parameters are loaded into FB buffer
  3. TMOV_FP performs quantization: Acc * scale -> quantized output

Constraint: This template is selected when src.memory_space == ACC,
fp.memory_space == SCALING, and dst.memory_space == MAT.

Supported scenarios:
  - int32 accumulator -> int8 output with scale
  - float32 accumulator -> float16/int8 output with scale
"""

import tilelang_dsl as pto


def _tmov_fp_constraint(src: pto.Tile, fp: pto.Tile, dst: pto.Tile) -> bool:
    """Constraint: Fixpipe quantization scenario.

    Supported scenario:
      - src.memory_space == ACC
      - fp.memory_space == SCALING
      - dst.memory_space == MAT
    """
    src_ms = src.memory_space
    fp_ms = fp.memory_space
    dst_ms = dst.memory_space

    # Check src is ACC
    if isinstance(src_ms, str):
        src_is_acc = src_ms == "acc"
    elif isinstance(src_ms, pto.MemorySpace):
        src_is_acc = src_ms == pto.MemorySpace.ACC
    else:
        src_is_acc = hasattr(src_ms, "value") and src_ms.value == "acc"

    # Check fp is SCALING
    if isinstance(fp_ms, str):
        fp_is_scaling = fp_ms == "scaling"
    elif isinstance(fp_ms, pto.MemorySpace):
        fp_is_scaling = fp_ms == pto.MemorySpace.SCALING
    else:
        fp_is_scaling = hasattr(fp_ms, "value") and fp_ms.value == "scaling"

    # Check dst is MAT
    if isinstance(dst_ms, str):
        dst_is_mat = dst_ms == "mat"
    elif isinstance(dst_ms, pto.MemorySpace):
        dst_is_mat = dst_ms == pto.MemorySpace.MAT
    else:
        dst_is_mat = hasattr(dst_ms, "value") and dst_ms.value == "mat"

    return src_is_acc and fp_is_scaling and dst_is_mat


@pto.ckernel(
    target="a5",
    op="pto.tmov.fp",
    constraints=[_tmov_fp_constraint],
    advanced=True,
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