# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
"""
PTOAS PTODSL port of the hw-native Python Flash Attention example.

Source implementation:

    https://github.com/hw-native-sys/pto-isa/tree/main/kernels/python/flash_atten

The source is authored against the older external ``huawei-csl/pto-dsl``
package.  This file ports the authored kernel shape and dataflow to the current
PTOAS in-tree PTODSL surface.  It is intentionally frontend/compile focused:
it emits PTO MLIR that exposes the same high-level stages, slot layout, and
Cube/Vector pipe transaction boundaries, but does not implement the old runtime
FIFO protocol in Python.

Ported source structure:

    old fa_builder.py module/to_ir_module    -> @pto.jit(...).compile(...)
    old TensorType/SubTensorType globals     -> pto.ptr + make_tensor_view
    old declare_global + l2g2l FIFO entries  -> GM slot tensor views + local pipe surface
    old pto.load/store                       -> pto.tile.load/store
    old tile.matmul/matmul_acc               -> @pto.cube + pto.mad/mad_acc
    old vector row-reduce softmax/GU         -> @pto.simd tile ops

The source four-stage pipeline is preserved in the authored surface:

    compute_qk  (Cube): Q/K -> QK slot
    compute_p   (SIMD): QK slot -> streaming softmax -> P slot
    compute_pv  (Cube): P/V -> PV slot
    compute_gu  (SIMD): PV slot -> unnormalized running O

Current boundary:

* True FIFO scheduling, TSyncCVID lifetime management, and runtime backpressure
  are backend/runtime concerns.  This port represents the slot addresses and
  stage dataflow in frontend MLIR using explicit GM slot tensor views plus
  PTODSL local pipe init/push/pop/free transactions.
* The source example targets non-causal HEAD=128 FA.  This port keeps that
  contract and rejects unsupported specialization knobs early.
* Runtime launch, torch_npu validation, and A3/A5 performance checks are not
  part of this compile/frontend artifact.
"""

import argparse
from pathlib import Path
import sys

if __package__ in {None, ""}:
    here = Path(__file__).resolve()
    for candidate in here.parents:
        if (candidate / "ptodsl" / "__init__.py").exists():
            sys.path.insert(0, str(candidate))
            break
    else:
        raise RuntimeError("Unable to locate the PTODSL Python package root")

from ptodsl import pto, scalar


HW_NATIVE_FLASH_ATTEN_SOURCE = (
    "https://github.com/hw-native-sys/pto-isa/tree/main/"
    "kernels/python/flash_atten"
)

S0 = 128
HEAD = 128
CUBE_S1 = 128
DEFAULT_S1_TILE = 256
DEFAULT_QK_PRELOAD = 3
SLOT_NUM = 8
QK_C2V_PIPE_ID = 0
P_V2C_PIPE_ID = 1
PV_C2V_PIPE_ID = 2

SPLIT_UP_DOWN = 1
VEC_CORES = 2


def _validate_specialization(*, head_dim: int, s1_tile: int, qk_preload: int, causal: bool) -> None:
    if head_dim != HEAD:
        raise ValueError(f"hw-native flash_atten port currently requires head_dim={HEAD}, got {head_dim}")
    if s1_tile not in (256, 512):
        raise ValueError(f"s1_tile must be 256 or 512, got {s1_tile}")
    if s1_tile % CUBE_S1 != 0:
        raise ValueError(f"s1_tile={s1_tile} must be a multiple of CUBE_S1={CUBE_S1}")
    if qk_preload not in (3, 4):
        raise ValueError(f"qk_preload must be 3 or 4, got {qk_preload}")
    if causal:
        raise ValueError("hw-native flash_atten source port is non-causal; causal=True is not supported yet")


def describe_hw_native_flash_attention_port():
    s1_tile = DEFAULT_S1_TILE
    tile_factor = s1_tile // CUBE_S1
    vec_s0 = S0 // VEC_CORES // tile_factor
    return {
        "source": HW_NATIVE_FLASH_ATTEN_SOURCE,
        "ported_file": "kernels/python/flash_atten/kernels/fa_builder.py",
        "ported_surface": {
            "entry": "@pto.jit(target='a5', mode='explicit')",
            "old_module_builder": "to_ir_module_with_meta(...)",
            "new_module_builder": "flash_attention_kernel.compile(...).mlir_text()",
            "old_tensor_types": "TensorType/SubTensorType module globals",
            "new_tensor_views": "pto.ptr + pto.make_tensor_view + pto.partition_view",
            "old_fifo": "initialize_l2g2l_pipe + talloc/tpush/tpop/tfree",
            "new_frontend_slots": "explicit GM slot tensor views + A5 local pipe surface; runtime FIFO remains backend work",
        },
        "source_constants": {
            "S0": S0,
            "HEAD": HEAD,
            "CUBE_S1": CUBE_S1,
            "S1_TILE": s1_tile,
            "TILE_FACTOR": tile_factor,
            "Vec_S0": vec_s0,
            "QK_PRELOAD": DEFAULT_QK_PRELOAD,
            "SLOT_NUM": SLOT_NUM,
        },
        "dataflow": ["compute_qk", "compute_p", "compute_pv", "compute_gu"],
        "frontend_features": [
            "QK_PRELOAD prologue/steady/epilogue schedule",
            "A5 local pipe surface for QK/P/PV stage boundaries",
            "Vec_S0 row-slice state arrays",
            "exp_max_ring per preload slot and row-slice",
            "wide P slot producer/consumer",
            "block_idx/block_num Q block distribution",
        ],
    }


def emit_flash_attention_mlir(
    *,
    head_dim: int = HEAD,
    s1_tile: int = DEFAULT_S1_TILE,
    qk_preload: int = DEFAULT_QK_PRELOAD,
    causal: bool = False,
    q_rows: int = S0,
):
    _validate_specialization(
        head_dim=head_dim,
        s1_tile=s1_tile,
        qk_preload=qk_preload,
        causal=causal,
    )
    compiled = flash_attention_kernel.compile(
        HEAD_DIM=head_dim,
        S1_TILE=s1_tile,
        QK_PRELOAD=qk_preload,
        CAUSAL=causal,
        Q_ROWS=q_rows,
    )
    return compiled.mlir_text()


@pto.jit(target="a5", mode="explicit")
def flash_attention_kernel(
    gm_slot_buffer: pto.ptr(pto.f32, "gm"),
    gm_slot_buffer_fp16: pto.ptr(pto.f16, "gm"),
    gm_q: pto.ptr(pto.f16, "gm"),
    gm_k: pto.ptr(pto.f16, "gm"),
    gm_v: pto.ptr(pto.f16, "gm"),
    gm_o: pto.ptr(pto.f32, "gm"),
    s0: pto.i32,
    s1: pto.i32,
    *,
    HEAD_DIM: pto.constexpr = HEAD,
    S1_TILE: pto.constexpr = DEFAULT_S1_TILE,
    QK_PRELOAD: pto.constexpr = DEFAULT_QK_PRELOAD,
    CAUSAL: pto.constexpr = False,
    Q_ROWS: pto.constexpr = S0,
):
    """
    Launchable PTODSL entry for the hw-native FA Python example.

    The signature mirrors the source ``call_both`` shape after replacing the
    old FFTS/runtime handle with a compile-only JIT entry.  ``gm_slot_buffer``
    and ``gm_slot_buffer_fp16`` model the source GM-staged QK/P/PV slots.
    """
    if HEAD_DIM != HEAD:
        raise ValueError("flash_attention_kernel requires HEAD_DIM=128")
    if CAUSAL:
        raise ValueError("causal masking is not part of the hw-native source port yet")

    c0 = 0
    c1 = 1
    cS0 = S0
    cHEAD = HEAD
    cCUBE_S1 = CUBE_S1
    cS1_TILE = S1_TILE
    cQK_PRELOAD = QK_PRELOAD
    cSLOT_NUM = SLOT_NUM

    tile_factor = S1_TILE // CUBE_S1
    vec_s0 = S0 // VEC_CORES // tile_factor
    row_slice_count = VEC_CORES * tile_factor

    slot_size_qk_f32 = S0 * S1_TILE
    slot_size_pv_f32 = S0 * HEAD
    slot_size_p_f16 = S0 * S1_TILE
    qk_pipe_slot_bytes = S0 * CUBE_S1 * 4
    p_pipe_slot_bytes = vec_s0 * S1_TILE * 2
    pv_pipe_slot_bytes = S0 * HEAD * 4
    gm_pv_off_f32 = slot_size_qk_f32 * SLOT_NUM
    gm_p_off_f32 = gm_pv_off_f32 + slot_size_pv_f32 * SLOT_NUM

    s1_index = scalar.index_cast(s1)
    q_view = pto.make_tensor_view(gm_q, shape=[s0, cHEAD], strides=[cHEAD, c1])
    k_view = pto.make_tensor_view(gm_k, shape=[cHEAD, s1_index], strides=[c1, cHEAD])
    v_view = pto.make_tensor_view(gm_v, shape=[s1_index, cHEAD], strides=[cHEAD, c1])
    o_view = pto.make_tensor_view(gm_o, shape=[s0, cHEAD], strides=[cHEAD, c1])

    qk_slots = pto.make_tensor_view(
        gm_slot_buffer,
        shape=[cSLOT_NUM * cS0, cS1_TILE],
        strides=[cS1_TILE, c1],
    )
    pv_slot_row_offset = gm_pv_off_f32 // HEAD
    pv_workspace = pto.make_tensor_view(
        gm_slot_buffer,
        shape=[pv_slot_row_offset + cSLOT_NUM * cS0, cHEAD],
        strides=[cHEAD, c1],
    )
    pv_slots = pto.partition_view(
        pv_workspace,
        offsets=[pv_slot_row_offset, c0],
        sizes=[cSLOT_NUM * cS0, cHEAD],
    )
    p_slot_row_offset = (gm_p_off_f32 * 2) // S1_TILE
    p_workspace = pto.make_tensor_view(
        gm_slot_buffer_fp16,
        shape=[p_slot_row_offset + cSLOT_NUM * cS0, cS1_TILE],
        strides=[cS1_TILE, c1],
    )
    p_slots = pto.partition_view(
        p_workspace,
        offsets=[p_slot_row_offset, c0],
        sizes=[cSLOT_NUM * cS0, cS1_TILE],
    )
    qk_c2v_buf = pto.reserve_buffer(
        "fa_qk_c2v_fifo",
        size=qk_pipe_slot_bytes * SLOT_NUM,
        location="vec",
    )
    p_v2c_buf = pto.reserve_buffer(
        "fa_p_v2c_fifo",
        size=p_pipe_slot_bytes * SLOT_NUM,
        location="mat",
    )
    pv_c2v_buf = pto.reserve_buffer(
        "fa_pv_c2v_fifo",
        size=pv_pipe_slot_bytes * SLOT_NUM,
        location="vec",
    )
    qk_c2v_pipe = pto.pipe.c2v_local(
        slot_size=qk_pipe_slot_bytes,
        consumer_buf=qk_c2v_buf,
        id=QK_C2V_PIPE_ID,
        local_slot_num=SLOT_NUM,
        nosplit=True,
    )
    p_v2c_pipe = pto.pipe.v2c_local(
        slot_size=p_pipe_slot_bytes,
        consumer_buf=p_v2c_buf,
        id=P_V2C_PIPE_ID,
        local_slot_num=SLOT_NUM,
        nosplit=True,
    )
    pv_c2v_pipe = pto.pipe.c2v_local(
        slot_size=pv_pipe_slot_bytes,
        consumer_buf=pv_c2v_buf,
        id=PV_C2V_PIPE_ID,
        local_slot_num=SLOT_NUM,
        nosplit=True,
    )

    q_mat = pto.alloc_tile(shape=[S0, HEAD], dtype=pto.f16, memory_space="mat")
    q_left = pto.alloc_tile(shape=[S0, HEAD], dtype=pto.f16, memory_space="left")

    k_mat = pto.alloc_tile(
        shape=[HEAD, CUBE_S1],
        dtype=pto.f16,
        memory_space="mat",
        blayout="RowMajor",
        slayout="ColMajor",
    )
    k_right = pto.alloc_tile(shape=[HEAD, CUBE_S1], dtype=pto.f16, memory_space="right")
    qk_acc = pto.alloc_tile(shape=[S0, CUBE_S1], dtype=pto.f32, memory_space="acc")
    qk_tile = pto.alloc_tile(shape=[S0, CUBE_S1], dtype=pto.f32)

    p_recv = pto.alloc_tile(shape=[S0, CUBE_S1], dtype=pto.f16, memory_space="mat")
    p_left = pto.alloc_tile(shape=[S0, CUBE_S1], dtype=pto.f16, memory_space="left")
    v_mat = pto.alloc_tile(shape=[CUBE_S1, HEAD], dtype=pto.f16, memory_space="mat")
    v_right = pto.alloc_tile(shape=[CUBE_S1, HEAD], dtype=pto.f16, memory_space="right")
    pv_acc = pto.alloc_tile(shape=[S0, HEAD], dtype=pto.f32, memory_space="acc")
    pv_tile = pto.alloc_tile(shape=[S0, HEAD], dtype=pto.f32)

    qk_vec = pto.alloc_tile(shape=[vec_s0, S1_TILE], dtype=pto.f32)
    p_fp32 = pto.alloc_tile(shape=[vec_s0, S1_TILE], dtype=pto.f32)
    p_fp16 = pto.alloc_tile(shape=[vec_s0, S1_TILE], dtype=pto.f16)
    softmax_tmp = pto.alloc_tile(shape=[vec_s0, S1_TILE], dtype=pto.f32)
    pv_vec = pto.alloc_tile(shape=[vec_s0, HEAD], dtype=pto.f32)

    running_max = [
        pto.alloc_tile(shape=[vec_s0, 8], dtype=pto.f32, valid_shape=[vec_s0, 1])
        for _ in range(row_slice_count)
    ]
    running_sum = [
        pto.alloc_tile(shape=[vec_s0, 8], dtype=pto.f32, valid_shape=[vec_s0, 1])
        for _ in range(row_slice_count)
    ]
    local_max = [
        pto.alloc_tile(shape=[vec_s0, 8], dtype=pto.f32, valid_shape=[vec_s0, 1])
        for _ in range(row_slice_count)
    ]
    local_sum = [
        pto.alloc_tile(shape=[vec_s0, 8], dtype=pto.f32, valid_shape=[vec_s0, 1])
        for _ in range(row_slice_count)
    ]
    exp_max_ring = [
        [
            pto.alloc_tile(shape=[vec_s0, 8], dtype=pto.f32, valid_shape=[vec_s0, 1])
            for _ in range(row_slice_count)
        ]
        for _ in range(QK_PRELOAD)
    ]
    o_tile = [
        pto.alloc_tile(shape=[vec_s0, HEAD], dtype=pto.f32)
        for _ in range(row_slice_count)
    ]

    total_q_blocks = Q_ROWS // S0
    num_tiles_s1 = s1_index // cS1_TILE
    steady_tiles = num_tiles_s1 - cQK_PRELOAD

    block_num = scalar.index_cast(pto.get_block_num())
    block_idx = scalar.index_cast(pto.get_block_idx())
    floor_div = total_q_blocks // block_num
    extra = total_q_blocks % block_num
    fat_start = block_idx * (floor_div + c1)
    thin_start = extra * (floor_div + c1) + (block_idx - extra) * floor_div
    qb_start = scalar.select(block_idx < extra, fat_start, thin_start)
    per_core = scalar.select(block_idx < extra, floor_div + c1, floor_div)
    qb_end = qb_start + per_core

    def compute_qk_stage(tile_id):
        slot_id = tile_id % cSLOT_NUM
        tile_base = tile_id * cS1_TILE
        with pto.for_(0, tile_factor, step=1) as sub:
            s1_sub = tile_base + sub * cCUBE_S1
            qk_slot_part = pto.partition_view(
                qk_slots,
                offsets=[slot_id * cS0, sub * cCUBE_S1],
                sizes=[cS0, cCUBE_S1],
            )
            compute_qk(
                q_left,
                k_view,
                qk_slot_part,
                qk_c2v_pipe,
                k_mat,
                k_right,
                qk_acc,
                qk_tile,
                s1_sub,
            )

    def compute_softmax_stage(tile_id, ring_id, is_init):
        slot_id = tile_id % cSLOT_NUM
        with pto.for_(0, row_slice_count, step=1) as row_slice:
            row_off = row_slice * vec_s0
            qk_slot_part = pto.partition_view(
                qk_slots,
                offsets=[slot_id * cS0 + row_off, c0],
                sizes=[vec_s0, cS1_TILE],
            )
            p_slot_part = pto.partition_view(
                p_slots,
                offsets=[slot_id * cS0 + row_off, c0],
                sizes=[vec_s0, cS1_TILE],
            )

            # row_slice_count and QK_PRELOAD are constexpr.  The source keeps
            # one state tuple per row-slice and ring slot; this dispatch keeps
            # the same authored ownership even though the current frontend does
            # not have dynamic Python-list indexing by SSA values.
            for static_row_slice in range(row_slice_count):
                with pto.if_(row_slice == static_row_slice) as br:
                    with br.then_:
                        for static_ring in range(QK_PRELOAD):
                            if isinstance(ring_id, int):
                                if ring_id != static_ring:
                                    continue
                                compute_p(
                                    qk_slot_part,
                                    p_slot_part,
                                    qk_c2v_pipe,
                                    p_v2c_pipe,
                                    qk_vec,
                                    p_fp32,
                                    p_fp16,
                                    softmax_tmp,
                                    running_max[static_row_slice],
                                    running_sum[static_row_slice],
                                    local_max[static_row_slice],
                                    local_sum[static_row_slice],
                                    exp_max_ring[static_ring][static_row_slice],
                                    is_init=is_init,
                                )
                            else:
                                with pto.if_(ring_id == static_ring) as ring_br:
                                    with ring_br.then_:
                                        compute_p(
                                            qk_slot_part,
                                            p_slot_part,
                                            qk_c2v_pipe,
                                            p_v2c_pipe,
                                            qk_vec,
                                            p_fp32,
                                            p_fp16,
                                            softmax_tmp,
                                            running_max[static_row_slice],
                                            running_sum[static_row_slice],
                                            local_max[static_row_slice],
                                            local_sum[static_row_slice],
                                            exp_max_ring[static_ring][static_row_slice],
                                            is_init=is_init,
                                        )

    def compute_pv_stage(tile_id):
        slot_id = tile_id % cSLOT_NUM
        with pto.for_(0, tile_factor, step=1) as sub:
            s1_sub = tile_id * cS1_TILE + sub * cCUBE_S1
            p_slot_part = pto.partition_view(
                p_slots,
                offsets=[slot_id * cS0, sub * cCUBE_S1],
                sizes=[cS0, cCUBE_S1],
            )
            pv_slot_part = pto.partition_view(
                pv_slots,
                offsets=[slot_id * cS0, c0],
                sizes=[cS0, cHEAD],
            )
            compute_pv(
                p_slot_part,
                v_view,
                pv_slot_part,
                p_v2c_pipe,
                pv_c2v_pipe,
                p_recv,
                p_left,
                v_mat,
                v_right,
                pv_acc,
                pv_tile,
                s1_sub,
                is_first_sub=(sub == c0),
            )

    def compute_gu_stage(tile_id, ring_id, is_init):
        slot_id = tile_id % cSLOT_NUM
        with pto.for_(0, row_slice_count, step=1) as row_slice:
            row_off = row_slice * vec_s0
            pv_slot_part = pto.partition_view(
                pv_slots,
                offsets=[slot_id * cS0 + row_off, c0],
                sizes=[vec_s0, cHEAD],
            )
            for static_row_slice in range(row_slice_count):
                with pto.if_(row_slice == static_row_slice) as br:
                    with br.then_:
                        for static_ring in range(QK_PRELOAD):
                            if isinstance(ring_id, int):
                                if ring_id != static_ring:
                                    continue
                                compute_gu(
                                    pv_slot_part,
                                    pv_c2v_pipe,
                                    pv_vec,
                                    o_tile[static_row_slice],
                                    exp_max_ring[static_ring][static_row_slice],
                                    is_init=is_init,
                                )
                            else:
                                with pto.if_(ring_id == static_ring) as ring_br:
                                    with ring_br.then_:
                                        compute_gu(
                                            pv_slot_part,
                                            pv_c2v_pipe,
                                            pv_vec,
                                            o_tile[static_row_slice],
                                            exp_max_ring[static_ring][static_row_slice],
                                            is_init=is_init,
                                        )

    with pto.for_(qb_start, qb_end, step=1) as qb:
        init_fa_cube_pipes(qk_c2v_pipe, p_v2c_pipe, pv_c2v_pipe)
        init_fa_vector_pipes(qk_c2v_pipe, p_v2c_pipe, pv_c2v_pipe)

        q_part = pto.partition_view(q_view, offsets=[qb * cS0, c0], sizes=[cS0, cHEAD])
        pto.tile.load(q_part, q_mat)
        pto.tile.mov(q_mat, q_left)

        for row_slice in range(row_slice_count):
            running_max[row_slice].fill(float("-inf"))
            running_sum[row_slice].fill(0.0)
            o_tile[row_slice].fill(0.0)

        for kp in range(QK_PRELOAD):
            compute_qk_stage(kp)
        pto.pipe_barrier(pto.Pipe.ALL)

        for kp in range(QK_PRELOAD):
            compute_softmax_stage(
                kp,
                kp,
                is_init=(kp == 0),
            )
        pto.pipe_barrier(pto.Pipe.ALL)

        with pto.for_(0, steady_tiles, step=1) as tile_id:
            compute_pv_stage(tile_id)
            compute_gu_stage(tile_id, tile_id % cQK_PRELOAD, is_init=(tile_id == c0))
            compute_qk_stage(tile_id + cQK_PRELOAD)
            compute_softmax_stage(
                tile_id + cQK_PRELOAD,
                (tile_id + cQK_PRELOAD) % cQK_PRELOAD,
                is_init=False,
            )
            pto.pipe_barrier(pto.Pipe.ALL)

        for k in range(QK_PRELOAD):
            drain_tile = steady_tiles + k
            compute_pv_stage(drain_tile)
            compute_gu_stage(drain_tile, drain_tile % cQK_PRELOAD, is_init=(drain_tile == c0))
            pto.pipe_barrier(pto.Pipe.ALL)

        for row_slice in range(row_slice_count):
            row_off = row_slice * vec_s0
            pto.tile.rowexpanddiv(o_tile[row_slice], running_sum[row_slice], o_tile[row_slice])
            o_part = pto.partition_view(
                o_view,
                offsets=[qb * cS0 + row_off, c0],
                sizes=[vec_s0, cHEAD],
            )
            pto.tile.store(o_tile[row_slice], o_part)


@pto.cube
def init_fa_cube_pipes(qk_c2v_pipe, p_v2c_pipe, pv_c2v_pipe):
    qk_c2v_pipe.init_cube()
    p_v2c_pipe.init_cube()
    pv_c2v_pipe.init_cube()


@pto.simd
def init_fa_vector_pipes(qk_c2v_pipe, p_v2c_pipe, pv_c2v_pipe):
    qk_c2v_pipe.init_simd()
    p_v2c_pipe.init_simd()
    pv_c2v_pipe.init_simd()


@pto.cube
def compute_qk(
    q_left: pto.Tile,
    k_view: pto.TensorView,
    qk_slot_part: pto.PartitionTensorView,
    qk_pipe,
    k_mat: pto.Tile,
    k_right: pto.Tile,
    qk_acc: pto.Tile,
    qk_tile: pto.Tile,
    s1_sub: pto.index,
):
    k_part = pto.partition_view(k_view, offsets=[0, s1_sub], sizes=[HEAD, CUBE_S1])
    pto.tile.load(k_part, k_mat)
    pto.tile.mov(k_mat, k_right)
    pto.mad(q_left.as_ptr(), k_right.as_ptr(), qk_acc.as_ptr(), S0, CUBE_S1, HEAD)
    pto.mte_l0c_ub(qk_acc.as_ptr(), qk_tile.as_ptr(), S0, CUBE_S1, CUBE_S1, CUBE_S1, 0)
    pto.tile.store(qk_tile, qk_slot_part)
    qk_pipe.push(qk_tile, split=0)


@pto.simd
def compute_p(
    qk_slot_part: pto.PartitionTensorView,
    p_slot_part: pto.PartitionTensorView,
    qk_pipe,
    p_pipe,
    qk_tile: pto.Tile,
    p_fp32: pto.Tile,
    p_fp16: pto.Tile,
    tmp: pto.Tile,
    running_max: pto.Tile,
    running_sum: pto.Tile,
    local_max: pto.Tile,
    local_sum: pto.Tile,
    exp_max: pto.Tile,
    is_init: pto.i1,
):
    _ = qk_pipe.pop(result_type=qk_tile, split=0)
    qk_pipe.free(split=0)
    pto.tile.load(qk_slot_part, qk_tile)
    pto.tile.rowmax(qk_tile, local_max, tmp=tmp)

    def init_softmax_state():
        pto.tile.mov(local_max, running_max)
        pto.tile.rowexpandsub(qk_tile, running_max, p_fp32)
        pto.tile.muls(p_fp32, 0.08838834764831845, p_fp32)
        pto.tile.exp(p_fp32, p_fp32)
        pto.tile.rowsum(p_fp32, running_sum, tmp=tmp)
        exp_max.fill(1.0)

    def update_softmax_state():
        pto.tile.max(local_max, running_max, local_max)
        pto.tile.sub(running_max, local_max, exp_max)
        pto.tile.mov(local_max, running_max)
        pto.tile.muls(exp_max, 0.08838834764831845, exp_max)
        pto.tile.exp(exp_max, exp_max)
        pto.tile.rowexpandsub(qk_tile, running_max, p_fp32)
        pto.tile.muls(p_fp32, 0.08838834764831845, p_fp32)
        pto.tile.exp(p_fp32, p_fp32)
        pto.tile.mul(running_sum, exp_max, running_sum)
        pto.tile.rowsum(p_fp32, local_sum, tmp=tmp)
        pto.tile.add(running_sum, local_sum, running_sum)

    if isinstance(is_init, bool):
        if is_init:
            init_softmax_state()
        else:
            update_softmax_state()
    else:
        with pto.if_(is_init) as branch:
            with branch.then_:
                init_softmax_state()
            with branch.else_:
                update_softmax_state()

    pto.tile.cvt(p_fp32, p_fp16)
    pto.tile.store(p_fp16, p_slot_part)
    p_pipe.push(p_fp16, split=0)


@pto.cube
def compute_pv(
    p_slot_part: pto.PartitionTensorView,
    v_view: pto.TensorView,
    pv_slot_part: pto.PartitionTensorView,
    p_pipe,
    pv_pipe,
    p_recv: pto.Tile,
    p_left: pto.Tile,
    v_mat: pto.Tile,
    v_right: pto.Tile,
    pv_acc: pto.Tile,
    pv_tile: pto.Tile,
    s1_sub: pto.index,
    is_first_sub: pto.i1,
):
    _ = p_pipe.pop(result_type=p_recv, split=0)
    p_pipe.free(split=0)
    pto.tile.load(p_slot_part, p_recv)
    pto.tile.mov(p_recv, p_left)

    v_part = pto.partition_view(v_view, offsets=[s1_sub, 0], sizes=[CUBE_S1, HEAD])
    pto.tile.load(v_part, v_mat)
    pto.tile.mov(v_mat, v_right)

    with pto.if_(is_first_sub) as branch:
        with branch.then_:
            pto.mad(p_left.as_ptr(), v_right.as_ptr(), pv_acc.as_ptr(), S0, HEAD, CUBE_S1)
        with branch.else_:
            pto.mad_acc(p_left.as_ptr(), v_right.as_ptr(), pv_acc.as_ptr(), S0, HEAD, CUBE_S1)

    pto.mte_l0c_ub(pv_acc.as_ptr(), pv_tile.as_ptr(), S0, HEAD, HEAD, HEAD, 0)
    pto.tile.store(pv_tile, pv_slot_part)
    pv_pipe.push(pv_tile, split=0)


@pto.simd
def compute_gu(
    pv_slot_part: pto.PartitionTensorView,
    pv_pipe,
    pv_tile: pto.Tile,
    o_tile: pto.Tile,
    exp_max: pto.Tile,
    is_init: pto.i1,
):
    _ = pv_pipe.pop(result_type=pv_tile, split=0)
    pv_pipe.free(split=0)
    pto.tile.load(pv_slot_part, pv_tile)
    if isinstance(is_init, bool):
        if is_init:
            pto.tile.mov(pv_tile, o_tile)
        else:
            pto.tile.rowexpandmul(o_tile, exp_max, o_tile)
            pto.tile.add(o_tile, pv_tile, o_tile)
    else:
        with pto.if_(is_init) as branch:
            with branch.then_:
                pto.tile.mov(pv_tile, o_tile)
            with branch.else_:
                pto.tile.rowexpandmul(o_tile, exp_max, o_tile)
                pto.tile.add(o_tile, pv_tile, o_tile)

def build_arg_parser():
    parser = argparse.ArgumentParser(
        description="Emit PTOAS PTODSL MLIR for the hw-native Python flash_atten port."
    )
    parser.add_argument("--head-dim", type=int, default=HEAD)
    parser.add_argument("--s1-tile", type=int, default=DEFAULT_S1_TILE)
    parser.add_argument("--qk-preload", type=int, default=DEFAULT_QK_PRELOAD)
    parser.add_argument("--q-rows", type=int, default=S0)
    parser.add_argument("--causal", action="store_true")
    parser.add_argument("-o", "--output", default="-", help="output MLIR path, or '-' for stdout")
    return parser


def main(argv=None):
    parser = build_arg_parser()
    args = parser.parse_args(argv)
    mlir_text = emit_flash_attention_mlir(
        head_dim=args.head_dim,
        s1_tile=args.s1_tile,
        qk_preload=args.qk_preload,
        causal=args.causal,
        q_rows=args.q_rows,
    )
    if args.output == "-":
        print(mlir_text)
        return
    Path(args.output).write_text(mlir_text, encoding="utf-8")


if __name__ == "__main__":
    main()
