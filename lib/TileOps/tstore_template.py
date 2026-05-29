# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""`pto.tstore` 的 TileLang DSL 模板"""

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


def _match_store_tile_layout(src, *, row_major: bool, s_layout) -> bool:
    b_layout_ok = (
        src.config.b_layout == pto.BLayout.ROW_MAJOR
        if row_major
        else src.config.b_layout != pto.BLayout.ROW_MAJOR
    )
    return b_layout_ok and src.config.s_layout == s_layout


def _check_store_bounds(src, dst, *, logical_rows, logical_cols, stride_axis=None) -> bool:
    if dst.rank != 5:
        return False
    if stride_axis is not None and not _known_eq(dst.strides[stride_axis], 1):
        return False
    if not _known_eq(src.valid_shape[0], logical_rows):
        return False
    if not _known_eq(src.valid_shape[1], logical_cols):
        return False
    if not _known_le(src.valid_shape[0], src.shape[0]):
        return False
    if not _known_le(src.valid_shape[1], src.shape[1]):
        return False
    return True


def _tstore_preconditions_nd(src, dst) -> bool:
    logical_rows = dst.shape[0] * dst.shape[1] * dst.shape[2] * dst.shape[3]
    logical_cols = dst.shape[4]
    return _match_store_tile_layout(
        src, row_major=True, s_layout=pto.SLayout.NONE_BOX
    ) and _check_store_bounds(
        src, dst, logical_rows=logical_rows, logical_cols=logical_cols, stride_axis=4
    )
    
def _tstore_preconditions_dn(src, dst) -> bool:
    logical_rows = dst.shape[3]
    logical_cols = dst.shape[0] * dst.shape[1] * dst.shape[2] * dst.shape[4]
    return _match_store_tile_layout(
        src, row_major=False, s_layout=pto.SLayout.NONE_BOX
    ) and _check_store_bounds(
        src, dst, logical_rows=logical_rows, logical_cols=logical_cols, stride_axis=3
    )

def _tstore_preconditions_nz(src, dst) -> bool:
    logical_rows = dst.shape[2] * dst.shape[3]
    logical_cols = dst.shape[0] * dst.shape[1] * dst.shape[4]
    return _match_store_tile_layout(
        src, row_major=False, s_layout=pto.SLayout.ROW_MAJOR
    ) and _check_store_bounds(
        src, dst, logical_rows=logical_rows, logical_cols=logical_cols
    )

@pto.vkernel(
    target="a5",
    op="pto.tstore",
    advanced=True,
    constraints=[_tstore_preconditions_nd],
)
def template_tstore_nd(src: pto.Tile, dst: pto.PartitionTensorView):
    dtype = src.element_type
    elem_bytes = pto.bytewidth(dtype)

    g0, g1, g2, g3, g4 = dst.shape
    s0, s1, s2, s3, s4 = dst.strides

    valid_rows, valid_cols = src.valid_shape
    ub_rows, ub_cols = src.shape

    # These preconditions are expressed through the descriptor-level constraint
    # callable above, using direct `src.*` / `dst.*` metadata syntax.

    n_burst = g3
    len_burst = valid_cols * elem_bytes
    ub_stride = ub_cols * elem_bytes
    gm_stride = s3 * elem_bytes

    src_stride2 = g3 * ub_cols
    src_stride1 = g2 * src_stride2
    src_stride0 = g1 * src_stride1

    loop1 = g2
    loop2 = g1
    loop1_src_stride = src_stride2 * elem_bytes
    loop1_dst_stride = s2 * elem_bytes
    loop2_src_stride = src_stride1 * elem_bytes
    loop2_dst_stride = s1 * elem_bytes

    ub_ptr = src.as_ptr()
    gm_ptr = dst.as_ptr()

    if loop1 != 1 or loop2 != 1:
        pto.set_loop2_stride_ubtoout(
            src_stride=loop2_src_stride, dst_stride=loop2_dst_stride
        )
        pto.set_loop1_stride_ubtoout(
            src_stride=loop1_src_stride, dst_stride=loop1_dst_stride
        )
        pto.set_loop_size_ubtoout(loop1=loop1, loop2=loop2)

    for i in range(0, g0, 1):
        src_i = pto.addptr(ub_ptr, i * src_stride0)
        dst_i = pto.addptr(gm_ptr, i * s0)
        pto.copy_ubuf_to_gm(
            dst=dst_i,
            src=src_i,
            n_burst=n_burst,
            len_burst=len_burst,
            gm_stride=gm_stride,
            ub_stride=ub_stride,
        )

    if loop1 != 1 or loop2 != 1:
        pto.set_loop_size_ubtoout(loop1=1, loop2=1)
    return

@pto.vkernel(
    target="a5",
    op="pto.tstore",
    advanced=True,
    constraints=[_tstore_preconditions_dn],
)
def template_tstore_dn(src: pto.Tile, dst: pto.PartitionTensorView):
    dtype = src.element_type
    elem_bytes = pto.bytewidth(dtype)

    g0, g1, g2, g3, g4 = dst.shape
    s0, s1, s2, s3, s4 = dst.strides

    valid_rows, valid_cols = src.valid_shape
    ub_rows, ub_cols = src.shape

    n_burst = g4
    len_burst = valid_rows * elem_bytes
    gm_stride = s4 * elem_bytes
    ub_stride = ub_rows * elem_bytes

    # UB 源 tile 是列高 `ub_rows` 的紧凑 col-major 布局，
    # 与 `TStoreVecDN` 一样由 `g4` / `g2` / `g1` 递推出三级 stride。
    src_stride2 = ub_rows * g4
    src_stride1 = g2 * src_stride2
    src_stride0 = g1 * src_stride1

    loop1 = g2
    loop2 = g1
    loop1_src_stride = src_stride2 * elem_bytes
    loop1_dst_stride = s2 * elem_bytes
    loop2_src_stride = src_stride1 * elem_bytes
    loop2_dst_stride = s1 * elem_bytes

    ub_ptr = src.as_ptr()
    gm_ptr = dst.as_ptr()

    if loop1 != 1 or loop2 != 1:
        pto.set_loop2_stride_ubtoout(
            src_stride=loop2_src_stride, dst_stride=loop2_dst_stride
        )
        pto.set_loop1_stride_ubtoout(
            src_stride=loop1_src_stride, dst_stride=loop1_dst_stride
        )
        pto.set_loop_size_ubtoout(loop1=loop1, loop2=loop2)

    for i in range(0, g0, 1):
        src_i = pto.addptr(ub_ptr, i * src_stride0)
        dst_i = pto.addptr(gm_ptr, i * s0)
        pto.copy_ubuf_to_gm(
            dst=dst_i,
            src=src_i,
            n_burst=n_burst,
            len_burst=len_burst,
            gm_stride=gm_stride,
            ub_stride=ub_stride,        
        )

    if loop1 != 1 or loop2 != 1:
        pto.set_loop_size_ubtoout(loop1=1, loop2=1)
    return

@pto.vkernel(
    target="a5",
    op="pto.tstore",
    advanced=True,
    constraints=[_tstore_preconditions_nz],
)
def template_tstore_nz(src: pto.Tile, dst: pto.PartitionTensorView):
    dtype = src.element_type
    elem_bytes = pto.bytewidth(dtype)

    g0, g1, g2, g3, g4 = dst.shape
    s0, s1, s2, s3, s4 = dst.strides

    valid_rows, valid_cols = src.valid_shape
    ub_rows, ub_cols = src.shape

    # 对应 C++ `C0_SIZE_BYTE`。NZ 每个 burst 始终写一个完整 C0 block。
    c0_size_bytes = 32
    n_burst = g1
    len_burst = valid_rows * c0_size_bytes
    gm_stride = s1 * elem_bytes
    ub_stride = ub_rows * c0_size_bytes

    # 每个 g0 block 在 UB 中由 `g1` 个 NZ block 串接组成。
    tile_stride = g1 * ub_rows * g4

    ub_ptr = src.as_ptr()
    gm_ptr = dst.as_ptr()

    # NZ path 本身不使用 loop1/loop2，主动切回 normal mode 避免继承旧状态。
    pto.set_loop_size_ubtoout(loop1=1, loop2=1)
    for i in range(0, g0, 1):
        src_i = pto.addptr(ub_ptr, i * tile_stride)
        dst_i = pto.addptr(gm_ptr, i * s0)
        pto.copy_ubuf_to_gm(
            dst=dst_i,
            src=src_i,
            n_burst=n_burst,
            len_burst=len_burst,
            gm_stride=gm_stride,
            ub_stride=ub_stride,
        )
    return


# ============================================================================
# Cube Templates: TSTORE.ACC (ACC → GM)
# ============================================================================

def _constraint_tstore_acc_base(src, dst) -> bool:
    """TSTORE.ACC 基础约束检查"""
    # src 必须是 MemorySpace.ACC
    src_space = src.memory_space
    if src_space is None:
        return False
    src_space_value = src_space.value if hasattr(src_space, "value") else src_space
    if src_space_value not in {"acc", "ACC"}:
        return False
    # dst 必须是 GM (通过 PartitionTensorView)
    dst_space = dst.memory_space
    if dst_space is None:
        dst_space_value = "gm"  # PartitionTensorView 默认是 GM
    else:
        dst_space_value = dst_space.value if hasattr(dst_space, "value") else dst_space
    if dst_space_value not in {"gm", "GM"}:
        return False
    # ACC 的 dtype 必须是 f32 或 i32
    src_dtype = src.dtype
    if src_dtype is None:
        return False
    dtype_name = src_dtype.name if hasattr(src_dtype, "name") else str(src_dtype)
    if dtype_name not in {"f32", "i32"}:
        return False
    # dst dtype 可以是 f32, f16, bf16, i32
    dst_dtype = dst.dtype
    if dst_dtype is None:
        return True  # 允许 dst dtype 未指定
    dst_dtype_name = dst_dtype.name if hasattr(dst_dtype, "name") else str(dst_dtype)
    supported_dst_dtypes = {"f32", "f16", "bf16", "i32"}
    if dst_dtype_name not in supported_dst_dtypes:
        return False
    return True


def _constraint_tstore_acc_nz2nd(src, dst) -> bool:
    """TSTORE.ACC NZ2ND 约束"""
    if not _constraint_tstore_acc_base(src, dst):
        return False
    # dst 必须是 row-major layout (ND 格式)
    config = dst.config
    if config is None:
        return True  # 默认是 row-major
    b_layout = config.b_layout
    if b_layout is None:
        return True
    b_layout_value = b_layout.value if hasattr(b_layout, "value") else b_layout
    # ROW_MAJOR 对应 ND 格式
    if b_layout_value not in {"row_major", "ROW_MAJOR"}:
        return False
    return True


def _constraint_tstore_acc_nz2dn(src, dst) -> bool:
    """TSTORE.ACC NZ2DN 约束"""
    if not _constraint_tstore_acc_base(src, dst):
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


def _constraint_tstore_acc_nz2nz(src, dst) -> bool:
    """TSTORE.ACC NZ2NZ 约束"""
    if not _constraint_tstore_acc_base(src, dst):
        return False
    # dst 必须是 NZ layout (fractal)
    config = dst.config
    if config is None:
        return False
    # 检查是否有 fractal 或特殊的 NZ layout 标记
    s_layout = config.s_layout
    if s_layout is None:
        return False
    s_layout_value = s_layout.value if hasattr(s_layout, "value") else s_layout
    if s_layout_value not in {"row_major", "ROW_MAJOR"}:
        return False
    return True


@pto.ckernel(
    target="a5",
    op="pto.tstore",
    dtypes=[
        (pto.f32, pto.f32),
        (pto.f32, pto.f16),
        (pto.f32, pto.bf16),
        (pto.i32, pto.i32),
    ],
    constraints=[_constraint_tstore_acc_nz2nd],
    name="tstore_acc_to_gm_nz2nd",
)
def template_tstore_acc_to_gm_nz2nd(src: pto.Tile, dst: pto.Tile):
    """ACC → GM (NZ2ND 模式)

    将 L0C Accumulator Buffer 的 NZ 格式数据写回到 GM 的 Row-Major (ND) 格式。

    Args:
        src: Tile with ACC memory_space, shape=(M, N), dtype=f32/i32
        dst: Tile with GM memory_space, row-major (ND) 格式

    Uses:
        pto.mte_l0c_gm with layout="nz2nd"
    """
    m, n = src.valid_shape
    dtype = src.element_type

    acc_ptr = src.as_ptr()
    gm_ptr = dst.as_ptr()

    # src_stride: ACC buffer 的 stride (NZ 格式下为 N)
    # dst_stride: GM 的 stride (ND 格式下为 N)
    src_stride = n
    dst_stride = n

    pto.mte_l0c_gm(
        acc_ptr, gm_ptr,
        m, n, src_stride, dst_stride,
        0, 0,
        layout="nz2nd"
    )


@pto.ckernel(
    target="a5",
    op="pto.tstore",
    dtypes=[
        (pto.f32, pto.f32),
        (pto.f32, pto.f16),
        (pto.f32, pto.bf16),
        (pto.i32, pto.i32),
    ],
    constraints=[_constraint_tstore_acc_nz2dn],
    name="tstore_acc_to_gm_nz2dn",
)
def template_tstore_acc_to_gm_nz2dn(src: pto.Tile, dst: pto.Tile):
    """ACC → GM (NZ2DN 模式)

    将 L0C Accumulator Buffer 的 NZ 格式数据写回到 GM 的 Col-Major (DN) 格式。

    Args:
        src: Tile with ACC memory_space, shape=(M, N)
        dst: Tile with GM memory_space, col-major (DN) 格式

    Uses:
        pto.mte_l0c_gm with layout="nz2dn"
    """
    m, n = src.valid_shape

    acc_ptr = src.as_ptr()
    gm_ptr = dst.as_ptr()

    # NZ2DN 需要额外的 loop0_src_stride 参数
    src_stride = n
    dst_stride = m  # DN 格式下 stride 是 M

    pto.mte_l0c_gm(
        acc_ptr, gm_ptr,
        m, n, src_stride, dst_stride,
        0, 0,
        layout="nz2dn"
    )


@pto.ckernel(
    target="a5",
    op="pto.tstore",
    dtypes=[
        (pto.f32, pto.f32),
        (pto.f32, pto.f16),
        (pto.f32, pto.bf16),
        (pto.i32, pto.i32),
    ],
    constraints=[_constraint_tstore_acc_nz2nz],
    name="tstore_acc_to_gm_nz2nz",
)
def template_tstore_acc_to_gm_nz2nz(src: pto.Tile, dst: pto.Tile):
    """ACC → GM (NZ2NZ 模式)

    将 L0C Accumulator Buffer 的 NZ 格式数据写回到 GM 的 NZ 格式 (无转换)。

    Args:
        src: Tile with ACC memory_space, shape=(M, N)
        dst: Tile with GM memory_space, NZ (fractal) 格式

    Uses:
        pto.mte_l0c_gm with layout="nz2nz"
    """
    m, n = src.valid_shape

    acc_ptr = src.as_ptr()
    gm_ptr = dst.as_ptr()

    src_stride = n
    dst_stride = n

    pto.mte_l0c_gm(
        acc_ptr, gm_ptr,
        m, n, src_stride, dst_stride,
        0, 0,
        layout="nz2nz"
    )


# ============================================================================
# Cube Templates: TSTORE.MAT (MAT → GM)
# ============================================================================

def _constraint_tstore_mat(src, dst) -> bool:
    """TSTORE.MAT 约束检查"""
    # src 必须是 MemorySpace.MAT
    src_space = src.memory_space
    if src_space is None:
        return False
    src_space_value = src_space.value if hasattr(src_space, "value") else src_space
    if src_space_value not in {"mat", "MAT"}:
        return False
    # dst 必须是 GM
    dst_space = dst.memory_space
    if dst_space is None:
        dst_space_value = "gm"
    else:
        dst_space_value = dst_space.value if hasattr(dst_space, "value") else dst_space
    if dst_space_value not in {"gm", "GM"}:
        return False
    # dtype 检查
    src_dtype = src.dtype
    if src_dtype is None:
        return False
    dtype_name = src_dtype.name if hasattr(src_dtype, "name") else str(src_dtype)
    supported_dtypes = {"f16", "bf16", "f32", "i8", "si8", "ui8", "i16", "si16", "ui16", "i32"}
    if dtype_name not in supported_dtypes:
        return False
    return True


@pto.ckernel(
    target="a5",
    op="pto.tstore",
    dtypes=[
        (pto.f16,),
        (pto.bf16,),
        (pto.f32,),
    ],
    constraints=[_constraint_tstore_mat],
    name="tstore_mat_to_gm",
)
def template_tstore_mat_to_gm(src: pto.Tile, dst: pto.Tile):
    """MAT → GM 模板

    将 L1 MAT Buffer 数据写回到 GM。

    Args:
        src: Tile with MAT memory_space, shape=(M, K)
        dst: Tile with GM memory_space

    Note:
        当前 mte_l1_ub 只支持 MAT → UB，MAT → GM 的直接路径需要提单支持 mte_l1_gm。
        暂时使用 mte_l1_ub 写到 UB 中转。
    """
    m, k = src.valid_shape
    dtype = src.element_type
    elem_bytes = pto.bytewidth(dtype)

    mat_ptr = src.as_ptr()
    gm_ptr = dst.as_ptr()

    # TODO: 等待 mte_l1_gm DSL surface 支持后，替换为直接 MAT → GM
    # 当前临时使用 mte_l1_ub 写到 UB，再 copy_ubuf_to_gm 中转到 GM
    len_burst = k * elem_bytes

    pto.mte_l1_ub(mat_ptr, gm_ptr, len_burst, nburst=(m, 0, 0))


# ============================================================================
# Cube Templates: TSTORE_FP (ACC + FP → GM)
# ============================================================================

def _constraint_tstore_fp(src, fp, dst) -> bool:
    """TSTORE_FP 约束检查"""
    # src 必须是 MemorySpace.ACC
    src_space = src.memory_space
    if src_space is None:
        return False
    src_space_value = src_space.value if hasattr(src_space, "value") else src_space
    if src_space_value not in {"acc", "ACC"}:
        return False
    # fp 必须是 SCALING memory space 或特定 buffer
    fp_space = fp.memory_space
    if fp_space is None:
        return False
    fp_space_value = fp_space.value if hasattr(fp_space, "value") else fp_space
    if fp_space_value not in {"scaling", "SCALING", "ub", "UB"}:
        return False
    # dst 必须是 GM
    dst_space = dst.memory_space
    if dst_space is None:
        dst_space_value = "gm"
    else:
        dst_space_value = dst_space.value if hasattr(dst_space, "value") else dst_space
    if dst_space_value not in {"gm", "GM"}:
        return False
    # src dtype 必须是 f32
    src_dtype = src.dtype
    if src_dtype is None:
        return False
    dtype_name = src_dtype.name if hasattr(src_dtype, "name") else str(src_dtype)
    if dtype_name != "f32":
        return False
    return True


@pto.ckernel(
    target="a5",
    op="pto.tstore_fp",
    dtypes=[
        (pto.f32, pto.f16, pto.f16),
        (pto.f32, pto.bf16, pto.bf16),
    ],
    constraints=[_constraint_tstore_fp],
    name="tstore_fp_acc_to_gm",
)
def template_tstore_fp_acc_to_gm(src: pto.Tile, fp: pto.Tile, dst: pto.Tile):
    """ACC + FP → GM 带浮点转换 (TSTORE_FP)

    将 L0C Accumulator Buffer 的 f32 数据，配合 FP (scaling) 参数，
    写回到 GM 的 f16/bf16 格式。

    Args:
        src: Tile with ACC memory_space, dtype=f32
        fp: Tile with SCALING/UB memory_space, dtype=f16/bf16
        dst: Tile with GM memory_space, dtype=f16/bf16

    Note:
        TSTORE_FP 的底层实现使用 IR 层的 pto.tstore_fp op。
        该 op 对应硬件的 FIXPIPE 写回带量化参数。
        TODO: 等待 pto.tstore_fp DSL surface 支持后，替换为直接调用。
    """
    m, n = src.valid_shape

    acc_ptr = src.as_ptr()
    fp_ptr = fp.as_ptr()
    gm_ptr = dst.as_ptr()

    # TODO: 等待 tstore_fp DSL surface 支持后替换
    # 当前临时使用 mte_l0c_gm + pre_quant 实现类似功能
    src_stride = n
    dst_stride = n

    pto.mte_l0c_gm(
        acc_ptr, gm_ptr,
        m, n, src_stride, dst_stride,
        0, 0,
        layout="nz2nd",
        pre_quant=(fp_ptr, "f32_f16")
    )
