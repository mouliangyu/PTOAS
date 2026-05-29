# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You can not use this file in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

# coding=utf-8

"""Single source of truth for tmov2scale ST test cases.

Tests the TMOV2SCALE (Mat->Scaling/FB) operation explicitly:
  - TLOAD: GM -> L1 Mat
  - TMOV2LEFT/TMOV2RIGHT: L1 Mat -> L0A/L0B
  - TMATMUL: compute Acc = Left x Right
  - TMOV2SCALE: L1 Mat -> Fixpipe Buffer (mte_l1_fb) - the operation being tested
  - TSTORE: Acc -> GM

Simplified version - only tests mte_l1_fb operation, not full quantization flow.
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