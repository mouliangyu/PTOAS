#!/usr/bin/python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

import os
import sys

import numpy as np

from cases import CASES


def main():
    case_filter = sys.argv[1] if len(sys.argv) > 1 else None
    all_passed = True

    for case in CASES:
        if case_filter is not None and case["name"] != case_filter:
            continue

        case_dir = case["name"]
        golden = np.fromfile(os.path.join(case_dir, "golden.bin"),
                             dtype=case["dtype"]).reshape(case["output_shape"])
        output = np.fromfile(os.path.join(case_dir, "output.bin"),
                             dtype=case["dtype"]).reshape(case["output_shape"])

        vr, vc = case["output_valid_shape"]
        if not np.allclose(golden[:vr, :vc], output[:vr, :vc],
                           atol=case["eps"], rtol=case["eps"], equal_nan=True):
            print(f"[ERROR] {case['name']}: compare failed")
            all_passed = False
        else:
            print(f"[INFO] {case['name']}: compare passed")

    if not all_passed:
        sys.exit(2)


if __name__ == "__main__":
    main()
