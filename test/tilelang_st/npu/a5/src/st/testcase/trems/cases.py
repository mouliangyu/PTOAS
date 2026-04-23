#!/usr/bin/python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

# coding=utf-8

"""Single source of truth for trems ST test cases.

trems: integer remainder via vdiv, dst = src - trunc(src/scalar) * scalar.
All types: f32, f16, i32, i16.
"""

import numpy as np

CASES = [
    {
        "name": "i32_31x128",
        "dtype": np.int32,
        "shape": (31, 128),
        "valid_shape": (31, 128),
        "eps": 0,
    },
    {
        "name": "i16_15x192",
        "dtype": np.int16,
        "shape": (15, 192),
        "valid_shape": (15, 192),
        "eps": 0,
    },
]
