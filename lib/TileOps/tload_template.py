# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""`pto.tload` 的 TileLang DSL 模板"""

import tilelang_dsl as pto


def _constraint_scalar(value):
    return value.value if hasattr(value, "value") else value


def _known_eq(lhs, rhs) -> bool:
    lhs_value = _constraint_scalar(lhs)
    rhs_value = _constraint_scalar(rhs)
    if lhs_value is None or rhs_value is None:
        return True
    return lhs_value == rhs_value


def _known_le(lhs, rhs) -> bool:
    lhs_value = _constraint_scalar(lhs)
    rhs_value = _constraint_scalar(rhs)
    if lhs_value is None or rhs_value is None:
        return True
    return lhs_value <= rhs_value


def _match_tile_layout(dst, *, row_major: bool, s_layout) -> bool:
    b_layout_ok = (
        dst.config.b_layout == pto.BLayout.ROW_MAJOR
        if row_major
        else dst.config.b_layout != pto.BLayout.ROW_MAJOR
    )
    return b_layout_ok and dst.config.s_layout == s_layout


def _check_load_bounds(src, dst, *, logical_rows, logical_cols=None, stride_axis=None) -> bool:
    if src.rank != 5:
        return False
    if stride_axis is not None and not _known_eq(src.strides[stride_axis], 1):
        return False
    if not _known_le(dst.valid_shape[0], logical_rows):
        return False
    if not _known_le(logical_rows, dst.shape[0]):
        return False
    if not _known_le(dst.valid_shape[0], dst.shape[0]):
        return False
    if logical_cols is not None:
        if not _known_le(dst.valid_shape[1], logical_cols):
            return False
        if not _known_le(logical_cols, dst.shape[1]):
            return False
    if not _known_le(dst.valid_shape[1], dst.shape[1]):
        return False
    return True


def _tload_preconditions_nd2nd(src, dst) -> bool:
    logical_rows = src.shape[0] * src.shape[1] * src.shape[2] * src.shape[3]
    logical_cols = src.shape[4]
    return _match_tile_layout(
        dst, row_major=True, s_layout=pto.SLayout.NONE_BOX
    ) and _check_load_bounds(
        src, dst, logical_rows=logical_rows, logical_cols=logical_cols, stride_axis=4
    )


def _tload_preconditions_dn2dn(src, dst) -> bool:
    logical_rows = src.shape[3]
    logical_cols = src.shape[0] * src.shape[1] * src.shape[2] * src.shape[4]
    return _match_tile_layout(
        dst, row_major=False, s_layout=pto.SLayout.NONE_BOX
    ) and _check_load_bounds(
        src, dst, logical_rows=logical_rows, logical_cols=logical_cols, stride_axis=3
    )

def _tload_preconditions_nz2nz(src, dst) -> bool:
    logical_rows = src.shape[2]
    return _match_tile_layout(
        dst, row_major=False, s_layout=pto.SLayout.ROW_MAJOR
    ) and _check_load_bounds(
        src, dst, logical_rows=logical_rows
    )


@pto.vkernel(
    target="a5",
    op="pto.tload",
    advanced=True,
    constraints=[_tload_preconditions_nd2nd],
)
def template_tload_nd2nd(src: pto.PartitionTensorView, dst: pto.Tile):
    dtype = dst.element_type
    elem_bytes = pto.bytewidth(dtype)
    if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
        pto.set_mov_pad_val(dst.pad_value.eval())

    g0, g1, g2, g3, g4 = src.shape
    s0, s1, s2, s3, s4 = src.strides

    valid_rows, valid_cols = dst.valid_shape
    ub_rows, ub_cols = dst.shape

    n_burst = g3
    len_burst = g4 * elem_bytes
    gm_stride = s3 * elem_bytes
    ub_stride = ub_cols * elem_bytes

    dst_stride2 = g3 * ub_cols
    dst_stride1 = g2 * dst_stride2
    dst_stride0 = g1 * dst_stride1

    loop1 = g2
    loop2 = g1
    loop1_src_stride = s2 * elem_bytes
    loop1_dst_stride = dst_stride2 * elem_bytes
    loop2_src_stride = s1 * elem_bytes
    loop2_dst_stride = dst_stride1 * elem_bytes

    gm_ptr = src.as_ptr()
    ub_ptr = dst.as_ptr()

    if loop1 != 1 or loop2 != 1:
        pto.set_loop2_stride_outtoub(
            src_stride=loop2_src_stride, dst_stride=loop2_dst_stride
        )
        pto.set_loop1_stride_outtoub(
            src_stride=loop1_src_stride, dst_stride=loop1_dst_stride
        )
        pto.set_loop_size_outtoub(loop1=loop1, loop2=loop2)

    for i in range(0, g0, 1):
        src_i = pto.addptr(gm_ptr, i * s0)
        dst_i = pto.addptr(ub_ptr, i * dst_stride0)
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            pto.copy_gm_to_ubuf(
                dst=dst_i,
                src=src_i,
                n_burst=n_burst,
                len_burst=len_burst,
                gm_stride=gm_stride,
                ub_stride=ub_stride,
                enable_ub_pad=True,
            )
        else:
            pto.copy_gm_to_ubuf(
                dst=dst_i,
                src=src_i,
                n_burst=n_burst,
                len_burst=len_burst,
                gm_stride=gm_stride,
                ub_stride=ub_stride,
                enable_ub_pad=False,
            )

    if loop1 != 1 or loop2 != 1:
        pto.set_loop_size_outtoub(loop1=1, loop2=1)
    return

@pto.vkernel(
    target="a5",
    op="pto.tload",
    advanced=True,
    constraints=[_tload_preconditions_dn2dn],
)
def template_tload_dn2dn(src: pto.PartitionTensorView, dst: pto.Tile):
    dtype = dst.element_type
    elem_bytes = pto.bytewidth(dtype)
    if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
        pto.set_mov_pad_val(dst.pad_value.eval())

    # rank-5 partition view 元信息。
    g0, g1, g2, g3, g4 = src.shape
    s0, s1, s2, s3, s4 = src.strides

    tile_rows, tile_cols = dst.shape
    valid_rows, valid_cols = dst.valid_shape

    n_burst = g4
    len_burst = valid_rows * elem_bytes
    gm_stride = s4 * elem_bytes
    ub_stride = tile_rows * elem_bytes

    # UB 目标 tile 是列高为 `tile_rows` 的紧凑 col-major 布局，
    # 从最内层 `g4 × tile_rows` 块递推出三层阶梯 stride。
    dst_stride2 = g4 * tile_rows
    dst_stride1 = g2 * dst_stride2
    dst_stride0 = g1 * dst_stride1

    # loop1 ↔ g2（内层），loop2 ↔ g1（外层），软件 for ↔ g0。
    loop1 = g2
    loop2 = g1
    loop1_src_stride = s2 * elem_bytes
    loop1_dst_stride = dst_stride2 * elem_bytes
    loop2_src_stride = s1 * elem_bytes
    loop2_dst_stride = dst_stride1 * elem_bytes

    gm_ptr = src.as_ptr()
    ub_ptr = dst.as_ptr()

    if loop1 != 1 or loop2 != 1:
        pto.set_loop2_stride_outtoub(
            src_stride=loop2_src_stride, dst_stride=loop2_dst_stride
        )
        pto.set_loop1_stride_outtoub(
            src_stride=loop1_src_stride, dst_stride=loop1_dst_stride
        )
        pto.set_loop_size_outtoub(loop1=loop1, loop2=loop2)

    for i in range(0, g0, 1):
        src_i = pto.addptr(gm_ptr, i * s0)
        dst_i = pto.addptr(ub_ptr, i * dst_stride0)
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            pto.copy_gm_to_ubuf(
                dst=dst_i,
                src=src_i,
                n_burst=n_burst,
                len_burst=len_burst,
                gm_stride=gm_stride,
                ub_stride=ub_stride,
                enable_ub_pad=True,
            )
        else:
            pto.copy_gm_to_ubuf(
                dst=dst_i,
                src=src_i,
                n_burst=n_burst,
                len_burst=len_burst,
                gm_stride=gm_stride,
                ub_stride=ub_stride,
                enable_ub_pad=False,
            )

    if loop1 != 1 or loop2 != 1:
        pto.set_loop_size_outtoub(loop1=1, loop2=1)
    return

@pto.vkernel(
    target="a5",
    op="pto.tload",
    advanced=True,
    constraints=[_tload_preconditions_nz2nz],
)
def template_tload_nz2nz(src: pto.PartitionTensorView, dst: pto.Tile):
    dtype = dst.element_type
    elem_bytes = pto.bytewidth(dtype)

    if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
        pto.set_mov_pad_val(dst.pad_value.eval())

    # rank-5 partition view 元信息。NZ 静态分块约束（g3/g4 与 dtype 的关系）
    # 由更高层 schema/static-check 保证，这里只保留运行时搬运公式。
    g0, g1, g2, g3, g4 = src.shape
    s0, s1, s2, s3, s4 = src.strides

    tile_rows, tile_cols = dst.shape
    valid_rows, valid_cols = dst.valid_shape

    c0_size_bytes = 32
    n_burst = g1
    len_burst = valid_rows * c0_size_bytes
    gm_stride = s1 * elem_bytes
    ub_stride = tile_rows * c0_size_bytes

    # 每个 g0 block 在 UB 中包含 `g1` 个 NZ 小块；每块的列宽是 `g4` elems。
    tile_stride = g1 * tile_rows * g4

    gm_ptr = src.as_ptr()
    ub_ptr = dst.as_ptr()

    # NZ2NZ 对应实现始终走 normal mode，不复用 loop1/loop2 寄存器。
    pto.set_loop_size_outtoub(loop1=1, loop2=1)
    for i in range(0, g0, 1):
        src_i = pto.addptr(gm_ptr, i * s0)
        dst_i = pto.addptr(ub_ptr, i * tile_stride)
        if pto.constexpr(dst.pad_value != pto.PadValue.NULL):
            pto.copy_gm_to_ubuf(
                dst=dst_i,
                src=src_i,
                n_burst=n_burst,
                len_burst=len_burst,
                gm_stride=gm_stride,
                ub_stride=ub_stride,
                enable_ub_pad=True,
            )
        else:
            pto.copy_gm_to_ubuf(
                dst=dst_i,
                src=src_i,
                n_burst=n_burst,
                len_burst=len_burst,
                gm_stride=gm_stride,
                ub_stride=ub_stride,
                enable_ub_pad=False,
            )
    return


# ============================================================================
# Cube Matrix Templates: TLOAD.MAT (GM → L1)
# ============================================================================

def _constraint_tload_mat_base(src, dst) -> bool:
    """TLOAD.MAT 基础约束检查"""
    # dst 必须是 MemorySpace.MAT
    dst_space = dst.memory_space
    if dst_space is None:
        return False
    dst_space_value = dst_space.value if hasattr(dst_space, "value") else dst_space
    if dst_space_value not in {"mat", "MAT"}:
        return False
    # dst 必须是 2D Tile
    if dst.rank != 2:
        return False
    # dtype 检查
    dst_dtype = dst.dtype
    if dst_dtype is None:
        return False
    dtype_name = dst_dtype.name if hasattr(dst_dtype, "name") else str(dst_dtype)
    supported_dtypes = {"f16", "bf16", "f32", "i8", "si8", "ui8", "i16", "si16", "ui16", "i32", "si32"}
    if dtype_name not in supported_dtypes:
        return False
    return True


def _constraint_tload_mat_nd2nz(src, dst) -> bool:
    """TLOAD.MAT ND2NZ 分形加载约束"""
    if not _constraint_tload_mat_base(src, dst):
        return False
    # dst layout 必须是 col_major (NZ 格式)
    config = dst.config
    if config is None:
        return False
    b_layout = config.b_layout
    if b_layout is None:
        return False
    b_layout_value = b_layout.value if hasattr(b_layout, "value") else b_layout
    # COL_MAJOR 对应 NZ 格式
    if b_layout_value not in {"col_major", "COL_MAJOR"}:
        return False
    return True


def _constraint_tload_mat_dn2nz(src, dst) -> bool:
    """TLOAD.MAT DN2NZ 分形加载约束"""
    if not _constraint_tload_mat_base(src, dst):
        return False
    config = dst.config
    if config is None:
        return False
    b_layout = config.b_layout
    if b_layout is None:
        return False
    b_layout_value = b_layout.value if hasattr(b_layout, "value") else b_layout
    if b_layout_value not in {"col_major", "COL_MAJOR"}:
        return False
    return True


@pto.ckernel(
    target="a5",
    op="pto.tload",
    dtypes=[
        (pto.f16,),
        (pto.bf16,),
        (pto.f32,),
    ],
    constraints=[_constraint_tload_mat_nd2nz],
    name="tload_gm_to_mat_nd2nz",
)
def template_tload_gm_to_mat_nd2nz(src: pto.Tile, dst: pto.Tile):
    """GM → MAT ND2NZ 分形加载模板

    将 GM 中的 Row-Major (ND) 格式数据加载到 L1 MAT Buffer 的 NZ 格式。

    Args:
        src: Tile with GM memory_space, PartitionTensorView
        dst: Tile with MAT memory_space, shape=(M, K), col_major layout

    Uses:
        pto.mte_gm_l1_frac with mode="nd2nz"
    """
    m, k = dst.valid_shape
    dtype = dst.element_type
    elem_bytes = pto.bytewidth(dtype)

    gm_ptr = src.as_ptr()
    mat_ptr = dst.as_ptr()

    # ND2NZ 参数计算
    # n_value = M (行数), d_value = K (列数)
    n_value = m
    d_value = k

    # src_layout: 内层 stride = K (一行有多少元素)
    src_inner_stride = k

    # dst_group: (group_count, loop2_stride, loop3_stride, loop4_stride)
    # 对于简单单块情况: (1, 1, m, 0)
    dst_group = (1, 1, m, 0)

    # ctrl: (l2_cache_ctrl, smallc0_en)
    ctrl = (0, False)

    pto.mte_gm_l1_frac(
        gm_ptr, mat_ptr, "nd2nz",
        shape=(n_value, d_value),
        src_layout=(src_inner_stride,),
        dst_group=dst_group,
        ctrl=ctrl
    )


@pto.ckernel(
    target="a5",
    op="pto.tload",
    dtypes=[
        (pto.f16,),
        (pto.bf16,),
        (pto.f32,),
    ],
    constraints=[_constraint_tload_mat_dn2nz],
    name="tload_gm_to_mat_dn2nz",
)
def template_tload_gm_to_mat_dn2nz(src: pto.Tile, dst: pto.Tile):
    """GM → MAT DN2NZ 分形加载模板

    将 GM 中的 Col-Major (DN) 格式数据加载到 L1 MAT Buffer 的 NZ 格式。

    Args:
        src: Tile with GM memory_space, col-major source layout
        dst: Tile with MAT memory_space, shape=(M, K), col_major layout

    Uses:
        pto.mte_gm_l1_frac with mode="dn2nz"
    """
    m, k = dst.valid_shape
    dtype = dst.element_type

    gm_ptr = src.as_ptr()
    mat_ptr = dst.as_ptr()

    # DN2NZ 参数计算
    # 对于 DN 格式，原始 shape 是 (K, M)，需要转换
    # n_value = K, d_value = M
    n_value = k
    d_value = m

    # src_layout: 内层 stride = M (一列有多少元素)
    src_inner_stride = m

    dst_group = (1, 1, k, 0)
    ctrl = (0, False)

    pto.mte_gm_l1_frac(
        gm_ptr, mat_ptr, "dn2nz",
        shape=(n_value, d_value),
        src_layout=(src_inner_stride,),
        dst_group=dst_group,
        ctrl=ctrl
    )
