#!/usr/bin/python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

import numpy as np

CASES = [
    {
        "name": "f32_16x64_plain_concat",
        "kind": "plain",
        "dtype": np.float32,
        "shape": (16, 64),
        "valid_shape": (16, 64),
        "src0_valid_shape": (16, 32),
        "src1_valid_shape": (16, 32),
        "idx_shape": (16, 1),
        "idx_dtype": np.int32,
        "idx0_values": [32] * 16,
        "idx1_values": [32] * 16,
        "eps": 1e-6,
    },
    {
        "name": "f32_16x128_plain_concat",
        "kind": "plain",
        "dtype": np.float32,
        "shape": (16, 128),
        "valid_shape": (16, 128),
        "src0_valid_shape": (16, 64),
        "src1_valid_shape": (16, 64),
        "idx_shape": (16, 1),
        "idx_dtype": np.int32,
        "idx0_values": [64] * 16,
        "idx1_values": [64] * 16,
        "eps": 1e-6,
    },
    {
        "name": "f32_128x16_plain_concat",
        "kind": "plain",
        "dtype": np.float32,
        "shape": (128, 16),
        "valid_shape": (128, 16),
        "src0_valid_shape": (128, 8),
        "src1_valid_shape": (128, 8),
        "idx_shape": (128, 1),
        "idx_dtype": np.int32,
        "idx0_values": [8] * 128,
        "idx1_values": [8] * 128,
        "eps": 1e-6,
    },
    {
        "name": "f16_16x128_plain_concat",
        "kind": "plain",
        "dtype": np.float16,
        "shape": (16, 128),
        "valid_shape": (16, 128),
        "src0_valid_shape": (16, 64),
        "src1_valid_shape": (16, 64),
        "idx_shape": (16, 1),
        "idx_dtype": np.int32,
        "idx0_values": [64] * 16,
        "idx1_values": [64] * 16,
        "eps": 1e-3,
    },
    {
        "name": "f16_128x16_plain_concat",
        "kind": "plain",
        "dtype": np.float16,
        "shape": (128, 16),
        "valid_shape": (128, 16),
        "src0_valid_shape": (128, 8),
        "src1_valid_shape": (128, 8),
        "idx_shape": (128, 1),
        "idx_dtype": np.int32,
        "idx0_values": [8] * 128,
        "idx1_values": [8] * 128,
        "eps": 1e-3,
    },
    {
        "name": "f32_8x64_even_split",
        "kind": "indexed",
        "dtype": np.float32,
        "shape": (8, 64),
        "valid_shape": (8, 64),
        "idx_shape": (8, 1),
        "idx_dtype": np.int32,
        "idx0_values": [32 * 4] * 8,
        "idx1_values": [32 * 4] * 8,
        "eps": 1e-6,
    },
    {
        "name": "f32_8x64_clamped_split",
        "kind": "indexed",
        "dtype": np.float32,
        "shape": (8, 64),
        "valid_shape": (8, 64),
        "idx_shape": (8, 1),
        "idx_dtype": np.int32,
        "idx0_values": [1*4, 2*4, 16*4, 32*4, 48*4, 58*4, 62*4, 63*4],
        "idx1_values": [63*4, 62*4, 48*4, 32*4, 16*4, 6*4, 2*4, 1*4],
        "eps": 1e-6,
    },
    {
        "name": "f32_8x64_edge_split",
        "kind": "indexed",
        "dtype": np.float32,
        "shape": (8, 64),
        "valid_shape": (8, 64),
        "idx_shape": (8, 1),
        "idx_dtype": np.int32,
        "idx0_values": [0 * 4,  4 * 4,  31 * 4, 32 * 4, 33 * 4, 34 * 4, 45 * 4, 59 * 4],
        "idx1_values": [64 * 4, 63 * 4, 33 * 4, 32 * 4, 31 * 4, 30 * 4, 19 * 4, 5 * 4],
        "eps": 1e-6,
    },
]


def build_expected_output(case, src0, src1, idx0, idx1):
    rows, cols = case["shape"]
    golden = np.zeros((rows, cols), dtype=case["dtype"])

    if case.get("kind") == "plain":
        src0_cols = case["src0_valid_shape"][1]
        src1_cols = case["src1_valid_shape"][1]
        golden[:, :src0_cols] = src0[:, :src0_cols]
        golden[:, src0_cols : src0_cols + src1_cols] = src1[:, :src1_cols]
        return golden

    idx_elem_bytes = np.dtype(case["idx_dtype"]).itemsize
    idx0_flat = np.asarray(idx0, dtype=case["idx_dtype"]).reshape(rows)
    idx1_flat = np.asarray(idx1, dtype=case["idx_dtype"]).reshape(rows)

    for row in range(rows):
        split0 = min(int(idx0_flat[row]) // idx_elem_bytes, cols)
        split1 = min(int(idx1_flat[row]) // idx_elem_bytes, cols - split0)
        if split0 > 0:
            golden[row, :split0] = src0[row, :split0]
        if split1 > 0:
            golden[row, split0 : split0 + split1] = src1[row, :split1]

    return golden
