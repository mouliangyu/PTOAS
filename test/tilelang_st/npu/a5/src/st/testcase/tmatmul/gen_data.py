#!/usr/bin/python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

import numpy as np
from cases import CASES
from st_common import validate_cases, setup_case_rng, save_case_data

validate_cases(CASES)

for case in CASES:
    setup_case_rng(case)

    out_dtype = case["dtype"]
    shape = case["shape"]
    valid_shape = case["valid_shape"]
    vr, vc = valid_shape

    input1 = np.random.randint(1, 5, size=shape).astype(np.float16)
    input2 = np.random.randint(1, 5, size=shape).astype(np.float16)

    golden = np.zeros(shape, dtype=out_dtype)
    golden[:vr, :vc] = np.matmul(
        input1[:vr, :vc].astype(np.float32), input2[:vr, :vc].astype(np.float32)
    ).astype(out_dtype, copy=False)

    save_case_data(case["name"], {"input1": input1, "input2": input2, "golden": golden})
    print(
        f"[INFO] gen_data: {case['name']} shape={shape} valid_shape={valid_shape} "
        f"in_dtype=float16 out_dtype={out_dtype.__name__}"
    )
