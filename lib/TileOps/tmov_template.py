# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tmov - tile data movement"""

import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.tmov",
    advanced=True,
)
def template_tmov_basic(src: pto.Tile, dst: pto.Tile):
    """Basic tile-to-tile data movement using vlds/vsts."""
    dtype = dst.element_type
    lanes = pto.get_lanes(dtype)

    # Use dst.valid_shape[1] as the copy count
    # The dst tile defines how many elements to write
    copy_elements = dst.valid_shape[1]

    for col in range(0, copy_elements, lanes):
        remained = copy_elements - col
        mask, remained = pto.make_mask(dtype, remained)
        data = pto.vlds(src[0, col:])
        pto.vsts(data, dst[0, col:], mask)

    return None