# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tcolmax"
)
def template_tcolmax(src: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    valid_rows, valid_cols = src.valid_shape

    lanes = pto.get_lanes(dtype)
    mask_all = pto.make_mask(dtype, pto.MaskPattern.PAT_ALL)

    for col_chunk in range(0, valid_cols, lanes):
        acc = pto.vlds(src[0, col_chunk:])
        for row in range(1, valid_rows, 1):
            row_vec = pto.vlds(src[row, col_chunk:])
            acc = pto.vmax(acc, row_vec, mask_all)
        pto.vsts(acc, dst[0, col_chunk:], mask_all)

    return
