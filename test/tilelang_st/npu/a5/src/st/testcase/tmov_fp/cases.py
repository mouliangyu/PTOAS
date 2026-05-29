# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You can not use this file in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

# coding=utf-8

"""Single source of truth for tmov_fp ST test cases.

Tests the TMOV_FP-related operations through a simplified matmul flow:
  - TLOAD: GM -> L1 Mat
  - TMOV2LEFT/TMOV2RIGHT: L1 Mat -> L0A/L0B
  - TMOV2SCALE: L1 Mat -> FB (scaling buffer) - dummy before matmul
  - TMATMUL: compute Acc = Left x Right
  - TSTORE: Acc -> GM

Simplified version - tests mte_l1_fb operation similar to tmov2scale.
Note: Full tmov.fp (Acc->Mat with FB) requires int8 matmul template which is not yet available.
"""

import numpy as np


CASES = [
    {
        "name": "f16_16x16x16",
        "dtype_a": np.float16,
        "dtype_b": np.float16,
        "dtype_c": np.float32,
        "shape_a": (16, 16),
        "shape_b": (16, 16),
        "shape_c": (16, 16),
        "eps": 1e-3,
    },
]