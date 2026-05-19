# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""TileLang DSL template for pto.tinsert - Insert sub-tile window

This template implements tile data insertion from a source tile to a destination tile
at specified row/col offset. Similar to TInsert in pto-isa.
"""

import tilelang_dsl as pto


def _constraint_scalar(value):
    return value.value if hasattr(value, "value") else value


def _known_le(lhs, rhs) -> bool:
    lhs_value = _constraint_scalar(lhs)
    rhs_value = _constraint_scalar(rhs)
    if lhs_value is None or rhs_value is None:
        return True
    return lhs_value <= rhs_value


def _tinsert_vec2vec_nd_constraint(src: pto.Tile, dst: pto.Tile) -> bool:
    """Constraint for Vec->Vec ND (RowMajor) path
    
    Supported scenario:
      - Both src and dst are Vec tiles (UB location)
      - Both have RowMajor layout (ND format)
    """
    if src.config.loc != pto.TileType.VEC:
        return False
    if dst.config.loc != pto.TileType.VEC:
        return False
    
    if src.config.b_layout != pto.BLayout.ROW_MAJOR:
        return False
    if dst.config.b_layout != pto.BLayout.ROW_MAJOR:
        return False
    
    if src.config.s_layout != pto.SLayout.NONE_BOX:
        return False
    if dst.config.s_layout != pto.SLayout.NONE_BOX:
        return False
    
    return True


@pto.vkernel(
    target="a5",
    op="pto.tinsert",
    constraints=[_tinsert_vec2vec_nd_constraint],
)
def template_tinsert_vec2vec_nd(
    src: pto.Tile,
    index_row: int,
    index_col: int,
    dst: pto.Tile
):
    """Vec->Vec ND (RowMajor) tile insertion using vlds/vsts
    
    Reference: TInsertVecToVecNDImpl in TInsert.hpp (L346-373)
    
    Args:
        src: Source Vec tile (ND format)
        index_row: Row offset in destination
        index_col: Col offset in destination  
        dst: Destination Vec tile (ND format)
    """
    dtype = src.element_type
    lanes = pto.get_lanes(dtype)
    
    valid_rows, valid_cols = src.valid_shape
    
    for row in range(valid_rows):
        remained = valid_cols
        for col in range(0, valid_cols, lanes):
            mask, remained = pto.make_mask(dtype, remained)
            data = pto.vlds(src[row, col:])
            pto.vsts(data, dst[index_row + row, index_col + col:], mask)
    
    return None