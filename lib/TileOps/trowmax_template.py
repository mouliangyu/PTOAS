# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.trowmax"""

import sys
from pathlib import Path
import tilelang_dsl as pto

@pto.vkernel(
    target="a5",
    op="pto.trowmax",
    advanced=True,
)
def template_trowmax(src: pto.Tile, tmp: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    lanes = pto.get_lanes(dtype)
    valid_rows, valid_cols = src.valid_shape

    # Initialize with dtype-specific minimum value (aligned with pto-isa Padding<T>::Min)
    init_val = pto.PadValue.MIN.eval(dtype)

    for row in range(0, valid_rows, 1):
        remained = valid_cols

        mask_1, _ = pto.make_mask(dtype, 1)

        # Initialize the accumulator for ROWMAX
        v_acc = pto.vbr(init_val)

        # Process column chunks
        for col in range(0, valid_cols, lanes):
            mask, remained = pto.make_mask(dtype, remained)
            v_src = pto.vlds(src[row, col:])

            # vcmax reduces src_dtype to acc_dtype
            v_reduced = pto.vcmax(v_src, mask)

            # Clear masked lanes to init_val for float types so vmax doesn't see NaN
            if pto.constexpr(dtype == pto.f32):
                v_reduced = pto.vsel(v_reduced, v_acc, mask)
            if pto.constexpr(dtype == pto.f16):
                v_reduced = pto.vsel(v_reduced, v_acc, mask)

            v_acc = pto.vmax(v_acc, v_reduced, mask_1)

        # Write final reduction to dest buffer once
        pto.vsts(v_acc, dst[row, 0:], mask_1)
    return
