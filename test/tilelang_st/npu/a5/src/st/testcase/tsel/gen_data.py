#!/usr/bin/python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

# coding=utf-8

import numpy as np
from cases import CASES
from st_common import validate_cases, setup_case_rng, save_case_data

validate_cases(CASES)

for case in CASES:
    setup_case_rng(case)

    dtype = case["dtype"]
    shape = case["shape"]
    valid_shape = case["valid_shape"]

    src0 = np.random.randint(1, 10, size=shape).astype(dtype)
    src1 = np.random.randint(1, 10, size=shape).astype(dtype)
    mask = np.random.randint(0, 2, size=shape).astype(dtype)

    golden = np.zeros(shape, dtype=dtype)
    vr, vc = valid_shape
    mask_valid = mask[:vr, :vc]
    src0_valid = src0[:vr, :vc]
    src1_valid = src1[:vr, :vc]
    golden[:vr, :vc] = np.where(mask_valid != 0, src0_valid, src1_valid)

    save_case_data(case["name"], {"input1": src0, "input2": src1, "input3": mask, "golden": golden})
    print(f"[INFO] gen_data: {case['name']} shape={shape} valid_shape={valid_shape} dtype={dtype.__name__}")