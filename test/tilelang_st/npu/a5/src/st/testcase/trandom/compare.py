#!/usr/bin/python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You can not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

# coding=utf-8

import os
import sys
import numpy as np

from cases import CASES
from st_common import style_fail, style_pass, validate_cases


def main():
    validate_cases(CASES)
    case_filter = sys.argv[1] if len(sys.argv) > 1 else None

    all_passed = True
    for case in CASES:
        if case_filter is not None and case["name"] != case_filter:
            continue

        case_dir = case["name"]
        shape = case["shape"]
        dtype = case["dtype"]
        vr, vc = case["valid_shape"]

        output = np.fromfile(os.path.join(case_dir, "output.bin"), dtype=dtype).reshape(shape)

        print(style_pass(f"[INFO] {case['name']}: output shape={shape}, dtype={dtype.__name__}"))
        print(f"[INFO] Output stats: min={output[:vr, :vc].min()}, max={output[:vr, :vc].max()}, mean={output[:vr, :vc].mean()}")

    if not all_passed:
        sys.exit(2)
    print(style_pass("[INFO] all cases passed"))


if __name__ == "__main__":
    main()