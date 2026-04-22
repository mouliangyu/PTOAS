#!/usr/bin/python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

# coding=utf-8

import numpy as np
from cases import CASES
from st_common import validate_cases, setup_case_rng, save_case_data

# Scalar value for comparison (matches the scalar passed in launch.cpp)
SCALAR = 5.0

validate_cases(CASES)

for case in CASES:
    setup_case_rng(case)

    dtype = case["dtype"]
    out_dtype = case["out_dtype"]
    shape = case["shape"]
    valid_shape = case["valid_shape"]

    # Generate random input matching testcase/tcmps pattern
    if np.issubdtype(dtype, np.floating):
        input1 = np.random.randint(-5, 5, size=shape).astype(dtype)
    else:
        input1 = np.random.randint(1, 10, size=shape).astype(dtype)

    vr, vc = valid_shape
    if np.issubdtype(dtype, np.floating):
        scalar_val = dtype(SCALAR)
    else:
        scalar_val = dtype(int(SCALAR))

    # Compute element-wise comparison result (0 or 1 per element)
    # Using "lt" mode to match the template
    cmp_result = (input1[:vr, :vc] < scalar_val).astype(np.uint8, copy=False)

    # tcmps output uses psts with PK mode:
    # - PK mode stores VL/16 = 16 bytes per iteration for b32 mask
    # - "keeping one bit out of every two bits": actual data at even bit positions,
    #   zero bits at odd positions
    # - Each row is stored independently: row_offset = row * iters_per_row * 16 bytes
    # - Within each 16-byte block: element i (column index within row) -> bit position i*2
    lanes = 256 // np.dtype(dtype).itemsize  # 64 for f32/i32
    bytes_per_iter = 16  # VL/16 for PK mode
    iters_per_row = (vc + lanes - 1) // lanes

    # Output buffer size matches the tile buffer: rows*cols bytes
    # (main.cpp writes rows*cols*sizeof(uint8_t) bytes)
    total_bytes = shape[0] * shape[1]
    golden = np.zeros(total_bytes, dtype=np.uint8)

    for row in range(vr):
        for col in range(vc):
            if cmp_result[row, col]:
                # Column index within the current psts iteration
                col_in_iter = col % lanes
                # PK mode: element maps to bit position col_in_iter*2 (interleaved with zeros)
                bit_pos = col_in_iter * 2
                # Byte offset = row_offset + iteration_offset + byte_within_block
                byte_idx = (row * iters_per_row + col // lanes) * bytes_per_iter + (bit_pos // 8)
                bit_idx = bit_pos % 8
                if byte_idx < total_bytes:
                    golden[byte_idx] |= (1 << bit_idx)

    save_case_data(case["name"], {"input1": input1, "golden": golden})
    print(f"[INFO] gen_data: {case['name']} shape={shape} valid_shape={valid_shape} dtype={dtype.__name__} out_dtype={out_dtype.__name__} scalar={SCALAR}")
