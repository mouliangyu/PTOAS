# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""
PTODSL port sketch of the hand-written A5 ``flash_atten4`` kernel.

This is a compile-only porting step. It intentionally preserves the original
high-performance kernel's stage decomposition:

    compute_qk (Cube)
      -> compute_p (SIMD/vector)
      -> compute_pv (Cube)
      -> compute_gu (SIMT/scalar update)

The PTODSL version keeps the original blocking vocabulary:

    S0 / S1 / HEAD_DIM
    CUBE_S0 / CUBE_S1 / TILE_S1
    TILE_FACTOR = TILE_S1 / CUBE_S1
    QK_PRELOAD as a schedule knob

The source of truth for this file is the GitCode fa4dsl A5 C++ implementation:

    kernels/manual/a5/flash_atten4/

The hw-native-sys Python FA example is only a secondary reference for PTODSL
expression and validation style.

What is still intentionally simplified at this stage:

- the original A5/A3-specific ping-pong / CV FIFO / FFTS runtime semantics
- the full overlap schedule across cube/vector subblocks
- backend-lowered high-performance softmax/GU TileOps

The goal here is to land the *port structure* in current PTODSL syntax while
keeping the manual kernel's tiled pipeline recognizable.
"""

import argparse
from math import sqrt
from pathlib import Path
import sys

if __package__ in {None, ""}:
    here = Path(__file__).resolve()
    for candidate in here.parents:
        if (candidate / "ptodsl" / "__init__.py").exists():
            sys.path.insert(0, str(candidate))
            break
    else:
        raise RuntimeError(
            "Unable to locate the PTODSL Python package root from flash_atten4_port.py"
        )

from ptodsl import pto, scalar


MANUAL_S0 = 128
MANUAL_HEAD_DIM = 128
MANUAL_CUBE_S0 = 128
MANUAL_CUBE_S1 = 128
MANUAL_TILE_S1 = 256
MANUAL_QK_PRELOAD = 4
MANUAL_CV_FIFO_SIZE = 8
MANUAL_CV_FIFO_CONS_SYNC_PERIOD = 4
MANUAL_FIFO_MODE = 1
MANUAL_VEC_CORES = 2
MANUAL_SRC_VEC_TN_BUFFERS = 2
MANUAL_LAUNCH_CORE_COUNT = 28
DEFAULT_S1 = 1024

# A5 fa_performance_dn_kernel.cpp::FftsBufferFlag.  PTODSL currently exposes
# static sync event ids in [0, 7], so this compile-only port models the first
# four two-flag TSync groups and leaves PV_UB_BUF_READY/CV_BLOCK_END for the
# later real pipeline split.
FA4_SYNC_QK2SM = 0
FA4_SYNC_SM2PV = 2
FA4_SYNC_PV2GU = 4
FA4_SYNC_UB_BUF = 6

FA4_COMM_STAGE_QK = 1
FA4_COMM_STAGE_P = 2
FA4_COMM_STAGE_PV = 3
FA4_COMM_STAGE_GU = 4
FA4_COMM_PHASE_PROLOGUE = 10
FA4_COMM_PHASE_STEADY = 20
FA4_COMM_PHASE_EPILOGUE = 30
FA4_COMM_DRAIN_QK2SM = 50
FA4_COMM_DRAIN_SM2PV = 51
FA4_COMM_DRAIN_PV2GU = 52
FA4_COMM_DRAIN_UB = 53
FA4_COMM_DRAIN_PV_UB = 54
FA4_COMM_BLOCK_END = 55
FA4_COMM_FIFO_MODE_GM = 60
FA4_COMM_FIFO_MODE_UB = 61
FA4_COMM_FIFO_MODE_QK_PV_UB = 62
FA4_COMM_LOGICAL_BLOCK_STRIDE = 70


def _min_i32(lhs, rhs):
    return scalar.select(lhs < rhs, lhs, rhs)


def _block_extent_i32(total, block_index, block_size):
    block_size_i32 = pto.const(block_size, dtype=pto.i32)
    remaining = total - block_index * block_size_i32
    return _min_i32(remaining, block_size_i32)


def _fa4_sync_record(event_id: int):
    pto.set_cross_flag(pto.Pipe.FIX, event_id)


def _fa4_sync_wait(event_id: int):
    pto.wait_cross_flag(pto.Pipe.FIX, event_id)


def _fa4_sync_record_if(condition, event_id: int):
    with pto.if_(condition) as sync_br:
        with sync_br.then_:
            _fa4_sync_record(event_id)


def _fa4_sync_wait_if(condition, event_id: int):
    with pto.if_(condition) as sync_br:
        with sync_br.then_:
            _fa4_sync_wait(event_id)


def _fa4_should_wait_consumption(sync_iter, fifo_size: int, sync_period: int):
    period = pto.const(sync_period)
    return (sync_iter >= pto.const(fifo_size)) & ((sync_iter % period) == pto.const(0))


def _fa4_should_notify_consumption(sync_iter, sync_period: int):
    period = pto.const(sync_period)
    return ((sync_iter + pto.const(1)) % period) == pto.const(0)


def _fa4_mark_cv_comm(cv_comm_ptr, offset, stage_code: int):
    scalar.store(pto.ui8(stage_code), cv_comm_ptr, offset)


def _fa4_pending_consumption_events(tiles_processed: int, fifo_size: int, sync_period: int) -> int:
    if tiles_processed <= 0 or fifo_size <= 0 or sync_period <= 0:
        return 0
    notify_count = tiles_processed // sync_period
    wait_count = 0
    if tiles_processed > fifo_size:
        last_iter = tiles_processed - 1
        wait_count = (last_iter // sync_period) - ((fifo_size - 1) // sync_period)
    max_pending = (fifo_size + sync_period - 1) // sync_period
    pending = notify_count - wait_count
    if pending < 0:
        return 0
    return pending if pending < max_pending else max_pending


def _fa4_drain_pending_syncs(
    cv_comm_ptr,
    drain_comm_base,
    tile_block_count: int,
    cv_fifo_size: int,
    cv_fifo_cons_sync_period: int,
):
    pending = _fa4_pending_consumption_events(
        tile_block_count,
        cv_fifo_size,
        cv_fifo_cons_sync_period,
    )
    ub_drain_count = (
        tile_block_count
        if tile_block_count < MANUAL_SRC_VEC_TN_BUFFERS
        else MANUAL_SRC_VEC_TN_BUFFERS
    )

    with pto.for_(0, pto.const(pending), step=1) as drain_idx:
        _fa4_sync_wait(FA4_SYNC_QK2SM + 1)
        _fa4_mark_cv_comm(cv_comm_ptr, drain_comm_base + drain_idx, FA4_COMM_DRAIN_QK2SM)

    with pto.for_(0, pto.const(pending), step=1) as drain_idx:
        _fa4_sync_wait(FA4_SYNC_SM2PV + 1)
        _fa4_mark_cv_comm(
            cv_comm_ptr,
            drain_comm_base + pto.const(pending) + drain_idx,
            FA4_COMM_DRAIN_SM2PV,
        )

    with pto.for_(0, pto.const(pending), step=1) as drain_idx:
        _fa4_sync_wait(FA4_SYNC_PV2GU + 1)
        _fa4_mark_cv_comm(
            cv_comm_ptr,
            drain_comm_base + pto.const(2 * pending) + drain_idx,
            FA4_COMM_DRAIN_PV2GU,
        )

    with pto.for_(0, pto.const(ub_drain_count), step=1) as drain_idx:
        _fa4_sync_wait(FA4_SYNC_UB_BUF + 1)
        _fa4_mark_cv_comm(
            cv_comm_ptr,
            drain_comm_base + pto.const(3 * pending) + drain_idx,
            FA4_COMM_DRAIN_UB,
        )

    pv_ub_drain_base = drain_comm_base + pto.const(3 * pending + ub_drain_count)
    with pto.for_(0, pto.const(ub_drain_count), step=1) as drain_idx:
        _fa4_mark_cv_comm(cv_comm_ptr, pv_ub_drain_base + drain_idx, FA4_COMM_DRAIN_PV_UB)

    cv_block_end_base = pv_ub_drain_base + pto.const(ub_drain_count)
    _fa4_mark_cv_comm(cv_comm_ptr, cv_block_end_base, FA4_COMM_BLOCK_END)


def _fa4_mark_fifo_mode(cv_comm_ptr, mode_comm_base, fifo_mode: int):
    if fifo_mode == 0:
        _fa4_mark_cv_comm(cv_comm_ptr, mode_comm_base, FA4_COMM_FIFO_MODE_GM)
    elif fifo_mode == 1:
        _fa4_mark_cv_comm(cv_comm_ptr, mode_comm_base, FA4_COMM_FIFO_MODE_UB)
    elif fifo_mode == 2:
        _fa4_mark_cv_comm(cv_comm_ptr, mode_comm_base, FA4_COMM_FIFO_MODE_QK_PV_UB)


def _fa4_mark_logical_block_stride(
    cv_comm_ptr,
    logical_comm_base,
    block_idx,
    logical_block_count: int,
    launch_block_count: int,
):
    with pto.for_(0, pto.const(logical_block_count), step=launch_block_count) as logical_stride:
        logical_block_idx = block_idx + logical_stride
        with pto.if_(logical_block_idx < pto.const(logical_block_count)) as logical_br:
            with logical_br.then_:
                _fa4_mark_cv_comm(
                    cv_comm_ptr,
                    logical_comm_base + logical_block_idx,
                    FA4_COMM_LOGICAL_BLOCK_STRIDE,
                )


def _fa4_mark_schedule_phases(
    cv_comm_ptr,
    schedule_base,
    tile_block_count: int,
    qk_preload: int,
):
    preload_tile_count = qk_preload if qk_preload < tile_block_count else tile_block_count
    steady_tile_count = tile_block_count - qk_preload if tile_block_count > qk_preload else 0
    epilogue_tile_count = tile_block_count - steady_tile_count

    with pto.for_(0, pto.const(preload_tile_count), step=1) as preload_tile:
        _fa4_mark_cv_comm(cv_comm_ptr, schedule_base + preload_tile, FA4_COMM_PHASE_PROLOGUE)

    with pto.for_(0, pto.const(steady_tile_count), step=1) as steady_tile:
        _fa4_mark_cv_comm(
            cv_comm_ptr,
            schedule_base + pto.const(tile_block_count) + steady_tile,
            FA4_COMM_PHASE_STEADY,
        )

    with pto.for_(0, pto.const(epilogue_tile_count), step=1) as epilogue_tile:
        _fa4_mark_cv_comm(
            cv_comm_ptr,
            schedule_base + pto.const(2 * tile_block_count) + epilogue_tile,
            FA4_COMM_PHASE_EPILOGUE,
        )


@pto.cube
def fa4_compute_qk(
    q_mat: pto.Tile,
    k_mat: pto.Tile,
    q_l0a: pto.Tile,
    k_l0b: pto.Tile,
    qk_acc: pto.Tile,
    scores_tile: pto.Tile,
):
    m = q_mat.valid_shape[0]
    k = q_mat.valid_shape[1]
    n = k_mat.valid_shape[0]

    pto.mte_l1_l0a(q_mat.as_ptr(), q_l0a.as_ptr(), m, k)
    pto.mte_l1_l0b(k_mat.as_ptr(), k_l0b.as_ptr(), k, n, transpose=True)
    pto.mad(q_l0a.as_ptr(), k_l0b.as_ptr(), qk_acc.as_ptr(), m, n, k)
    pto.mte_l0c_ub(qk_acc.as_ptr(), scores_tile.as_ptr(), m, n, n, n, 0)


@pto.cube
def fa4_compute_pv(
    p_mat: pto.Tile,
    v_mat: pto.Tile,
    p_l0a: pto.Tile,
    v_l0b: pto.Tile,
    pv_acc: pto.Tile,
    pv_tile: pto.Tile,
):
    m = p_mat.valid_shape[0]
    k = p_mat.valid_shape[1]
    n = v_mat.valid_shape[1]

    pto.mte_l1_l0a(p_mat.as_ptr(), p_l0a.as_ptr(), m, k)
    pto.mte_l1_l0b(v_mat.as_ptr(), v_l0b.as_ptr(), k, n)
    pto.mad(p_l0a.as_ptr(), v_l0b.as_ptr(), pv_acc.as_ptr(), m, n, k)
    pto.mte_l0c_ub(pv_acc.as_ptr(), pv_tile.as_ptr(), m, n, n, n, 0)


@pto.simd
def fa4_compute_p(
    scores_tile: pto.Tile,
    probs_tile: pto.Tile,
    m_prev_tile: pto.Tile,
    l_prev_tile: pto.Tile,
    m_next_tile: pto.Tile,
    l_next_tile: pto.Tile,
    exp_max_tile: pto.Tile,
    score_row_base: pto.i32,
    rows: pto.i32,
    cols: pto.i32,
    scale: pto.f32,
):
    with pto.for_(0, rows, step=1) as row:
        col_mask = pto.make_mask(pto.f32, cols)

        raw_scores = pto.vlds(scores_tile[score_row_base + row, 0:])
        scaled_scores = pto.vmuls(raw_scores, scale, col_mask)

        m_prev = scalar.load(m_prev_tile[row, 0])
        l_prev = scalar.load(l_prev_tile[row, 0])

        row_max = pto.vcgmax(scaled_scores, col_mask)
        m_next = scalar.max(m_prev, row_max)

        shifted_scores = pto.vsubs(scaled_scores, m_next, col_mask)
        probs = pto.vexp(shifted_scores, col_mask)
        row_sum = pto.vcgadd(probs, col_mask)

        exp_max = scalar.exp(m_prev - m_next)
        l_next = l_prev * exp_max + row_sum

        pto.vsts(probs, probs_tile[row, 0:], col_mask)
        scalar.store(m_next, m_next_tile[row, 0])
        scalar.store(l_next, l_next_tile[row, 0])
        scalar.store(exp_max, exp_max_tile[row, 0])


@pto.simt
def fa4_apply_causal_mask(
    scores_tile: pto.Tile,
    score_row_base: pto.i32,
    q_abs_row_base,
    kv_abs_col_base,
    rows: pto.i32,
    cols: pto.i32,
):
    with pto.for_(0, rows, step=1) as row:
        q_abs_row = q_abs_row_base + row

        with pto.for_(0, cols, step=1) as col:
            kv_abs_col = kv_abs_col_base + col

            with pto.if_(kv_abs_col > q_abs_row) as mask_br:
                with mask_br.then_:
                    scalar.store(float("-inf"), scores_tile[score_row_base + row, col])


@pto.simt
def fa4_compute_gu(
    o_prev_tile: pto.Tile,
    pv_tile: pto.Tile,
    exp_max_tile: pto.Tile,
    o_next_tile: pto.Tile,
    rows: pto.i32,
    dim: pto.i32,
):
    with pto.for_(0, rows, step=1) as row:
        exp_max = scalar.load(exp_max_tile[row, 0])

        with pto.for_(0, dim, step=1) as col:
            o_prev = scalar.load(o_prev_tile[row, col])
            pv_val = scalar.load(pv_tile[row, col])
            o_next = exp_max * o_prev + pv_val
            scalar.store(o_next, o_next_tile[row, col])


@pto.simt
def fa4_finalize_o(
    o_acc_tile: pto.Tile,
    l_final_tile: pto.Tile,
    o_final_tile: pto.Tile,
    rows: pto.i32,
    dim: pto.i32,
):
    with pto.for_(0, rows, step=1) as row:
        denom = scalar.load(l_final_tile[row, 0])

        with pto.for_(0, dim, step=1) as col:
            acc = scalar.load(o_acc_tile[row, col])
            scalar.store(acc / denom, o_final_tile[row, col])


def fa4_process_subtile(
    q_mat: pto.Tile,
    k_part: pto.PartitionTensorView,
    v_part: pto.PartitionTensorView,
    qk_fifo_view: pto.TensorView,
    p_fifo_view: pto.TensorView,
    exp_max_fifo_view: pto.TensorView,
    pv_fifo_view: pto.TensorView,
    pv_pend_fifo_view: pto.TensorView,
    o_parts_view: pto.TensorView,
    cv_comm_ptr,
    q_abs_row_base,
    kv_abs_col_base,
    k_mat: pto.Tile,
    v_mat: pto.Tile,
    o_prev_tile: pto.Tile,
    o_next_tile: pto.Tile,
    m_prev_tile: pto.Tile,
    l_prev_tile: pto.Tile,
    m_next_tile: pto.Tile,
    l_next_tile: pto.Tile,
    scores_tile: pto.Tile,
    probs_tile: pto.Tile,
    probs_f16_tile: pto.Tile,
    p_mat: pto.Tile,
    pv_tile: pto.Tile,
    pv_pend_tile: pto.Tile,
    exp_max_tile: pto.Tile,
    q_l0a: pto.Tile,
    p_l0a: pto.Tile,
    rhs_l0b: pto.Tile,
    qk_acc: pto.Tile,
    pv_acc: pto.Tile,
    tile_id,
    qkp_fifo_row_base,
    exp_max_fifo_row_base,
    pv_fifo_row_base,
    cv_comm_base,
    o_parts_row_base,
    score_row_base: pto.i32,
    vec_rows: pto.i32,
    cols: pto.i32,
    dim: pto.i32,
    scale: pto.f32,
    cv_fifo_size: int,
    cv_fifo_cons_sync_period: int,
    causal: bool,
):
    should_wait_consume = _fa4_should_wait_consumption(
        tile_id,
        cv_fifo_size,
        cv_fifo_cons_sync_period,
    )
    should_notify_consume = _fa4_should_notify_consumption(
        tile_id,
        cv_fifo_cons_sync_period,
    )
    should_wait_ub_reuse = tile_id >= pto.const(MANUAL_SRC_VEC_TN_BUFFERS)
    bytes_per_row = dim * pto.bytewidth(pto.f16)
    gm_row_stride = k_part.strides[0] * pto.bytewidth(pto.f16)
    mat_row_stride = k_mat.shape[1] * pto.bytewidth(pto.f16)

    _fa4_sync_wait_if(should_wait_consume, FA4_SYNC_QK2SM + 1)
    _fa4_sync_wait_if(should_wait_ub_reuse, FA4_SYNC_UB_BUF + 1)
    pto.mte_load(
        k_part.as_ptr(),
        k_mat.as_ptr(),
        0,
        bytes_per_row,
        nburst=(cols, gm_row_stride, mat_row_stride),
    )
    pto.mte_load(
        v_part.as_ptr(),
        v_mat.as_ptr(),
        0,
        bytes_per_row,
        nburst=(cols, gm_row_stride, mat_row_stride),
    )
    pto.pipe_barrier(pto.Pipe.ALL)

    fa4_compute_qk(q_mat, k_mat, q_l0a, rhs_l0b, qk_acc, scores_tile)
    qk_fifo_part = pto.partition_view(
        qk_fifo_view,
        offsets=[qkp_fifo_row_base, 0],
        sizes=[q_mat.shape[0], scores_tile.shape[1]],
    )
    pto.tile.store(scores_tile, qk_fifo_part)
    _fa4_mark_cv_comm(cv_comm_ptr, cv_comm_base, FA4_COMM_STAGE_QK)
    _fa4_sync_record(FA4_SYNC_QK2SM)
    pto.pipe_barrier(pto.Pipe.ALL)

    _fa4_sync_wait(FA4_SYNC_QK2SM)
    pto.tile.load(qk_fifo_part, scores_tile)
    if causal:
        fa4_apply_causal_mask(
            scores_tile,
            score_row_base,
            q_abs_row_base,
            kv_abs_col_base,
            vec_rows,
            cols,
        )
    fa4_compute_p(
        scores_tile,
        probs_tile,
        m_prev_tile,
        l_prev_tile,
        m_next_tile,
        l_next_tile,
        exp_max_tile,
        score_row_base,
        vec_rows,
        cols,
        scale,
    )
    pto.pipe_barrier(pto.Pipe.ALL)

    pto.tile.cvt(probs_tile, probs_f16_tile)
    p_fifo_part = pto.partition_view(
        p_fifo_view,
        offsets=[qkp_fifo_row_base, 0],
        sizes=[probs_f16_tile.shape[0], probs_f16_tile.shape[1]],
    )
    exp_max_fifo_part = pto.partition_view(
        exp_max_fifo_view,
        offsets=[exp_max_fifo_row_base, 0],
        sizes=[exp_max_tile.shape[0], exp_max_tile.shape[1]],
    )
    pto.tile.store(probs_f16_tile, p_fifo_part)
    pto.tile.store(exp_max_tile, exp_max_fifo_part)
    _fa4_mark_cv_comm(cv_comm_ptr, cv_comm_base + pto.const(1), FA4_COMM_STAGE_P)
    _fa4_sync_record_if(should_notify_consume, FA4_SYNC_QK2SM + 1)
    _fa4_sync_record(FA4_SYNC_UB_BUF + 1)
    _fa4_sync_record(FA4_SYNC_SM2PV)
    pto.pipe_barrier(pto.Pipe.ALL)

    _fa4_sync_wait(FA4_SYNC_SM2PV)
    _fa4_sync_wait_if(should_wait_consume, FA4_SYNC_PV2GU + 1)
    pto.tile.load(p_fifo_part, p_mat)
    fa4_compute_pv(p_mat, v_mat, p_l0a, rhs_l0b, pv_acc, pv_tile)
    pv_fifo_part = pto.partition_view(
        pv_fifo_view,
        offsets=[pv_fifo_row_base, 0],
        sizes=[pv_tile.shape[0], pv_tile.shape[1]],
    )
    pv_pend_fifo_part = pto.partition_view(
        pv_pend_fifo_view,
        offsets=[pv_fifo_row_base, 0],
        sizes=[pv_tile.shape[0], pv_tile.shape[1]],
    )
    pto.tile.store(pv_tile, pv_fifo_part)
    pto.tile.store(pv_tile, pv_pend_fifo_part)
    _fa4_mark_cv_comm(cv_comm_ptr, cv_comm_base + pto.const(2), FA4_COMM_STAGE_PV)
    _fa4_sync_record(FA4_SYNC_SM2PV + 1)
    _fa4_sync_record(FA4_SYNC_PV2GU)
    pto.pipe_barrier(pto.Pipe.ALL)

    _fa4_sync_wait(FA4_SYNC_PV2GU)
    pto.tile.load(pv_fifo_part, pv_tile)
    pto.tile.load(pv_pend_fifo_part, pv_pend_tile)
    pto.tile.load(exp_max_fifo_part, exp_max_tile)
    fa4_compute_gu(
        o_prev_tile,
        pv_tile,
        exp_max_tile,
        o_next_tile,
        vec_rows,
        dim,
    )
    o_parts_part = pto.partition_view(
        o_parts_view,
        offsets=[o_parts_row_base, 0],
        sizes=[o_next_tile.shape[0], o_next_tile.shape[1]],
    )
    pto.tile.store(o_next_tile, o_parts_part)
    _fa4_mark_cv_comm(cv_comm_ptr, cv_comm_base + pto.const(3), FA4_COMM_STAGE_GU)
    pto.tile.load(o_parts_part, o_next_tile)
    _fa4_sync_record_if(should_notify_consume, FA4_SYNC_PV2GU + 1)
    _fa4_sync_record(FA4_SYNC_UB_BUF)
    pto.pipe_barrier(pto.Pipe.ALL)


def emit_flash_atten4_port_mlir(
    *,
    s0=MANUAL_S0,
    s1=DEFAULT_S1,
    head_dim=MANUAL_HEAD_DIM,
    cube_s0=MANUAL_CUBE_S0,
    cube_s1=MANUAL_CUBE_S1,
    tile_s1=MANUAL_TILE_S1,
    qk_preload=MANUAL_QK_PRELOAD,
    cv_fifo_size=MANUAL_CV_FIFO_SIZE,
    cv_fifo_cons_sync_period=MANUAL_CV_FIFO_CONS_SYNC_PERIOD,
    fifo_mode=MANUAL_FIFO_MODE,
    causal=False,
):
    if s0 <= 0 or s1 <= 0 or head_dim <= 0:
        raise ValueError("s0, s1, and head_dim must be positive")
    if cube_s0 <= 0 or cube_s1 <= 0 or tile_s1 <= 0:
        raise ValueError("cube_s0, cube_s1, and tile_s1 must be positive")
    if s0 % cube_s0 != 0:
        raise ValueError("s0 must be divisible by cube_s0 for the staged flash_atten4 port")
    if s1 % tile_s1 != 0:
        raise ValueError("s1 must be divisible by tile_s1 for the staged flash_atten4 port")
    if tile_s1 % cube_s1 != 0:
        raise ValueError("tile_s1 must be divisible by cube_s1")
    if cube_s0 % (MANUAL_VEC_CORES * (tile_s1 // cube_s1)) != 0:
        raise ValueError("cube_s0 must be divisible by VEC_CORES * TILE_FACTOR")
    if cube_s0 // MANUAL_VEC_CORES // (tile_s1 // cube_s1) < 16:
        raise ValueError("row-sliced Vec_S0 must be at least 16 for PTODSL tile layout")
    if qk_preload not in (3, 4):
        raise ValueError("qk_preload must be 3 or 4 for the staged flash_atten4 port")
    if cv_fifo_size <= 0:
        raise ValueError("cv_fifo_size must be positive")
    if cv_fifo_cons_sync_period <= 0:
        raise ValueError("cv_fifo_cons_sync_period must be positive")
    if fifo_mode not in (0, 1, 2):
        raise ValueError("fifo_mode must be 0, 1, or 2")
    if s1 < tile_s1 * qk_preload:
        raise ValueError("s1 must cover at least qk_preload TILE_S1 tiles")

    compiled = flash_atten4_port_kernel.compile(
        S0=s0,
        S1=s1,
        HEAD_DIM=head_dim,
        CUBE_S0=cube_s0,
        CUBE_S1=cube_s1,
        TILE_S1=tile_s1,
        QK_PRELOAD=qk_preload,
        CV_FIFO_SIZE=cv_fifo_size,
        CV_FIFO_CONS_SYNC_PERIOD=cv_fifo_cons_sync_period,
        FIFO_MODE=fifo_mode,
        CAUSAL=causal,
    )
    return compiled.mlir_text()


@pto.jit(target="a5", mode="explicit", insert_sync=False)
def flash_atten4_port_kernel(
    Q_ptr: pto.ptr(pto.f16, "gm"),
    K_ptr: pto.ptr(pto.f16, "gm"),
    V_ptr: pto.ptr(pto.f16, "gm"),
    P_fifo_ptr: pto.ptr(pto.f16, "gm"),
    Exp_max_fifo_ptr: pto.ptr(pto.f32, "gm"),
    QK_fifo_ptr: pto.ptr(pto.f32, "gm"),
    PV_fifo_ptr: pto.ptr(pto.f32, "gm"),
    PV_pend_fifo_ptr: pto.ptr(pto.f32, "gm"),
    O_ptr: pto.ptr(pto.f32, "gm"),
    O_parts_ptr: pto.ptr(pto.f32, "gm"),
    CV_comm_ptr: pto.ptr(pto.ui8, "gm"),
    *,
    S0: pto.constexpr = MANUAL_S0,
    S1: pto.constexpr = DEFAULT_S1,
    HEAD_DIM: pto.constexpr = MANUAL_HEAD_DIM,
    CUBE_S0: pto.constexpr = MANUAL_CUBE_S0,
    CUBE_S1: pto.constexpr = MANUAL_CUBE_S1,
    TILE_S1: pto.constexpr = MANUAL_TILE_S1,
    QK_PRELOAD: pto.constexpr = MANUAL_QK_PRELOAD,
    CV_FIFO_SIZE: pto.constexpr = MANUAL_CV_FIFO_SIZE,
    CV_FIFO_CONS_SYNC_PERIOD: pto.constexpr = MANUAL_CV_FIFO_CONS_SYNC_PERIOD,
    FIFO_MODE: pto.constexpr = MANUAL_FIFO_MODE,
    CAUSAL: pto.constexpr = False,
):
    _ = QK_PRELOAD
    _ = CV_FIFO_SIZE
    _ = CV_FIFO_CONS_SYNC_PERIOD
    _ = PV_pend_fifo_ptr
    _ = O_parts_ptr

    logical_block_count = S0 // CUBE_S0
    launch_block_count = (
        logical_block_count
        if logical_block_count < MANUAL_LAUNCH_CORE_COUNT
        else MANUAL_LAUNCH_CORE_COUNT
    )
    tile_block_count = S1 // TILE_S1
    tile_factor = TILE_S1 // CUBE_S1
    vec_s0 = CUBE_S0 // MANUAL_VEC_CORES // tile_factor
    vec_gu_rows = CUBE_S0 // MANUAL_VEC_CORES
    scale = pto.const(1.0 / sqrt(float(HEAD_DIM)), dtype=pto.f32)

    q_view = pto.make_tensor_view(Q_ptr, shape=[S0, HEAD_DIM], strides=[HEAD_DIM, 1])
    k_view = pto.make_tensor_view(K_ptr, shape=[S1, HEAD_DIM], strides=[HEAD_DIM, 1])
    v_view = pto.make_tensor_view(V_ptr, shape=[S1, HEAD_DIM], strides=[HEAD_DIM, 1])
    o_view = pto.make_tensor_view(O_ptr, shape=[S0, HEAD_DIM], strides=[HEAD_DIM, 1])
    o_parts_view = pto.make_tensor_view(O_parts_ptr, shape=[S0, HEAD_DIM], strides=[HEAD_DIM, 1])
    qk_fifo_view = pto.make_tensor_view(
        QK_fifo_ptr,
        shape=[CV_FIFO_SIZE * tile_factor * CUBE_S0, CUBE_S1],
        strides=[CUBE_S1, 1],
    )
    p_fifo_view = pto.make_tensor_view(
        P_fifo_ptr,
        shape=[CV_FIFO_SIZE * tile_factor * CUBE_S0, CUBE_S1],
        strides=[CUBE_S1, 1],
    )
    exp_max_fifo_view = pto.make_tensor_view(
        Exp_max_fifo_ptr,
        shape=[CV_FIFO_SIZE * CUBE_S0, 1],
        strides=[1, 1],
    )
    pv_fifo_view = pto.make_tensor_view(
        PV_fifo_ptr,
        shape=[CV_FIFO_SIZE * CUBE_S0, HEAD_DIM],
        strides=[HEAD_DIM, 1],
    )
    pv_pend_fifo_view = pto.make_tensor_view(
        PV_pend_fifo_ptr,
        shape=[CV_FIFO_SIZE * CUBE_S0, HEAD_DIM],
        strides=[HEAD_DIM, 1],
    )

    block_idx = scalar.index_cast(pto.get_block_idx())
    active_block = block_idx < pto.const(launch_block_count)

    with pto.if_(active_block) as active_block_br:
        with active_block_br.then_:
            q_row_base = block_idx * pto.const(CUBE_S0)
            cube_rows_idx = pto.const(CUBE_S0)
            vec_rows_idx = pto.const(vec_s0)
            full_dim_idx = pto.const(HEAD_DIM)
            cube_rows = pto.const(CUBE_S0, dtype=pto.i32)
            vec_rows = pto.const(vec_s0, dtype=pto.i32)
            full_dim = pto.const(HEAD_DIM, dtype=pto.i32)
            one = pto.const(1, dtype=pto.i32)
            vec_subblock = scalar.index_cast(pto.get_subblock_idx())
            vec_subblock_rows = pto.const(vec_gu_rows)
            stage_comm_bytes_per_block = tile_block_count * tile_factor * 4
            schedule_comm_bytes_per_block = tile_block_count * 3
            pending_consumption_events = _fa4_pending_consumption_events(
                tile_block_count,
                CV_FIFO_SIZE,
                CV_FIFO_CONS_SYNC_PERIOD,
            )
            ub_drain_count = (
                tile_block_count
                if tile_block_count < MANUAL_SRC_VEC_TN_BUFFERS
                else MANUAL_SRC_VEC_TN_BUFFERS
            )
            drain_comm_bytes_per_block = pending_consumption_events * 3 + ub_drain_count * 2 + 1
            mode_comm_bytes_per_block = 1
            logical_comm_bytes_per_block = logical_block_count
            cv_comm_block_base = block_idx * pto.const(
                stage_comm_bytes_per_block
                + schedule_comm_bytes_per_block
                + drain_comm_bytes_per_block
                + mode_comm_bytes_per_block
                + logical_comm_bytes_per_block
            )
            schedule_comm_base = cv_comm_block_base + pto.const(stage_comm_bytes_per_block)
            drain_comm_base = schedule_comm_base + pto.const(schedule_comm_bytes_per_block)
            mode_comm_base = drain_comm_base + pto.const(drain_comm_bytes_per_block)
            logical_comm_base = mode_comm_base + pto.const(mode_comm_bytes_per_block)

            _fa4_mark_fifo_mode(CV_comm_ptr, mode_comm_base, FIFO_MODE)
            _fa4_mark_logical_block_stride(
                CV_comm_ptr,
                logical_comm_base,
                block_idx,
                logical_block_count,
                launch_block_count,
            )

            _fa4_mark_schedule_phases(
                CV_comm_ptr,
                schedule_comm_base,
                tile_block_count,
                QK_PRELOAD,
            )

            q_part = pto.partition_view(
                q_view,
                offsets=[q_row_base, 0],
                sizes=[cube_rows_idx, full_dim_idx],
            )

            q_mat = pto.alloc_tile(
                shape=[CUBE_S0, HEAD_DIM],
                dtype=pto.f16,
                memory_space=pto.MemorySpace.MAT,
                valid_shape=[cube_rows, full_dim],
                blayout="ColMajor",
                slayout="RowMajor",
            )
            q_l0a = pto.alloc_tile(
                shape=[CUBE_S0, HEAD_DIM],
                dtype=pto.f16,
                memory_space=pto.MemorySpace.LEFT,
                valid_shape=[cube_rows, full_dim],
            )

            k_mat = pto.alloc_tile(
                shape=[CUBE_S1, HEAD_DIM],
                dtype=pto.f16,
                memory_space=pto.MemorySpace.MAT,
                valid_shape=[pto.const(CUBE_S1, dtype=pto.i32), full_dim],
                blayout="ColMajor",
                slayout="RowMajor",
            )
            v_mat = pto.alloc_tile(
                shape=[CUBE_S1, HEAD_DIM],
                dtype=pto.f16,
                memory_space=pto.MemorySpace.MAT,
                valid_shape=[pto.const(CUBE_S1, dtype=pto.i32), full_dim],
                blayout="ColMajor",
                slayout="RowMajor",
            )
            rhs_l0b = pto.alloc_tile(
                shape=[CUBE_S1, HEAD_DIM],
                dtype=pto.f16,
                memory_space=pto.MemorySpace.RIGHT,
                valid_shape=[pto.const(CUBE_S1, dtype=pto.i32), full_dim],
            )

            scores_tile = pto.alloc_tile(shape=[CUBE_S0, CUBE_S1], dtype=pto.f32)
            probs_tile = pto.alloc_tile(shape=[vec_s0, CUBE_S1], dtype=pto.f32)
            probs_f16_tile = pto.alloc_tile(shape=[vec_s0, CUBE_S1], dtype=pto.f16)
            p_mat = pto.alloc_tile(
                shape=[vec_s0, CUBE_S1],
                dtype=pto.f16,
                memory_space=pto.MemorySpace.MAT,
            )
            p_l0a = pto.alloc_tile(
                shape=[vec_s0, CUBE_S1],
                dtype=pto.f16,
                memory_space=pto.MemorySpace.LEFT,
            )
            qk_acc = pto.alloc_tile(
                shape=[CUBE_S0, CUBE_S1],
                dtype=pto.f32,
                memory_space=pto.MemorySpace.ACC,
            )
            pv_acc = pto.alloc_tile(
                shape=[vec_s0, HEAD_DIM],
                dtype=pto.f32,
                memory_space=pto.MemorySpace.ACC,
                valid_shape=[vec_rows, full_dim],
            )
            pv_tile = pto.alloc_tile(
                shape=[vec_s0, HEAD_DIM],
                dtype=pto.f32,
                valid_shape=[vec_rows, full_dim],
            )
            pv_pend_tile = pto.alloc_tile(
                shape=[vec_s0, HEAD_DIM],
                dtype=pto.f32,
                valid_shape=[vec_rows, full_dim],
            )

            o_prev_tile = pto.alloc_tile(shape=[vec_s0, HEAD_DIM], dtype=pto.f32, valid_shape=[vec_rows, full_dim])
            o_next_tile = pto.alloc_tile(shape=[vec_s0, HEAD_DIM], dtype=pto.f32, valid_shape=[vec_rows, full_dim])
            m_prev_tile = pto.alloc_tile(shape=[vec_s0, 1], dtype=pto.f32, valid_shape=[vec_rows, one], blayout="ColMajor")
            m_next_tile = pto.alloc_tile(shape=[vec_s0, 1], dtype=pto.f32, valid_shape=[vec_rows, one], blayout="ColMajor")
            l_prev_tile = pto.alloc_tile(shape=[vec_s0, 1], dtype=pto.f32, valid_shape=[vec_rows, one], blayout="ColMajor")
            l_next_tile = pto.alloc_tile(shape=[vec_s0, 1], dtype=pto.f32, valid_shape=[vec_rows, one], blayout="ColMajor")
            exp_max_tile = pto.alloc_tile(shape=[vec_s0, 1], dtype=pto.f32, valid_shape=[vec_rows, one], blayout="ColMajor")
            o_final_tile = pto.alloc_tile(shape=[vec_s0, HEAD_DIM], dtype=pto.f32, valid_shape=[vec_rows, full_dim])

            pto.tile.load(q_part, q_mat)

            with pto.for_(0, pto.const(tile_factor), step=1) as row_slice:
                row_slice_base = (
                    q_row_base
                    + vec_subblock * vec_subblock_rows
                    + row_slice * pto.const(vec_s0)
                )
                score_row_index = (
                    vec_subblock * vec_subblock_rows
                    + row_slice * pto.const(vec_s0)
                )
                m_prev_tile.fill(float("-inf"))
                l_prev_tile.fill(0.0)
                o_prev_tile.fill(0.0)

                with pto.for_(0, pto.const(tile_block_count), step=1) as tile_id:
                    with pto.for_(0, pto.const(tile_factor), step=1) as sub_tile:
                        fifo_slot = tile_id % pto.const(CV_FIFO_SIZE)
                        qkp_fifo_row_base = (
                            fifo_slot * pto.const(tile_factor * CUBE_S0)
                            + sub_tile * pto.const(CUBE_S0)
                            + score_row_index
                        )
                        exp_max_fifo_row_base = (
                            fifo_slot * pto.const(CUBE_S0)
                            + score_row_index
                        )
                        pv_fifo_row_base = (
                            fifo_slot * pto.const(CUBE_S0)
                            + score_row_index
                        )
                        cv_comm_base = (
                            cv_comm_block_base
                            + (
                                tile_id * pto.const(tile_factor)
                                + sub_tile
                            )
                            * pto.const(4)
                        )
                        kv_row_base = (
                            tile_id * pto.const(TILE_S1)
                            + sub_tile * pto.const(CUBE_S1)
                        )
                        kv_rows_idx = pto.const(CUBE_S1)
                        kv_rows = pto.const(CUBE_S1, dtype=pto.i32)

                        k_part = pto.partition_view(
                            k_view,
                            offsets=[kv_row_base, 0],
                            sizes=[kv_rows_idx, full_dim_idx],
                        )
                        v_part = pto.partition_view(
                            v_view,
                            offsets=[kv_row_base, 0],
                            sizes=[kv_rows_idx, full_dim_idx],
                        )

                        fa4_process_subtile(
                            q_mat,
                            k_part,
                            v_part,
                            qk_fifo_view,
                            p_fifo_view,
                            exp_max_fifo_view,
                            pv_fifo_view,
                            pv_pend_fifo_view,
                            o_parts_view,
                            CV_comm_ptr,
                            row_slice_base,
                            kv_row_base,
                            k_mat,
                            v_mat,
                            o_prev_tile,
                            o_next_tile,
                            m_prev_tile,
                            l_prev_tile,
                            m_next_tile,
                            l_next_tile,
                            scores_tile,
                            probs_tile,
                            probs_f16_tile,
                            p_mat,
                            pv_tile,
                            pv_pend_tile,
                            exp_max_tile,
                            q_l0a,
                            p_l0a,
                            rhs_l0b,
                            qk_acc,
                            pv_acc,
                            tile_id,
                            qkp_fifo_row_base,
                            exp_max_fifo_row_base,
                            pv_fifo_row_base,
                            cv_comm_base,
                            row_slice_base,
                            scalar.index_cast(
                                pto.i32,
                                score_row_index,
                            ),
                            vec_rows,
                            kv_rows,
                            full_dim,
                            scale,
                            CV_FIFO_SIZE,
                            CV_FIFO_CONS_SYNC_PERIOD,
                            CAUSAL,
                        )

                        pto.tile.mov(m_next_tile, m_prev_tile)
                        pto.tile.mov(l_next_tile, l_prev_tile)
                        pto.tile.mov(o_next_tile, o_prev_tile)

                o_part = pto.partition_view(
                    o_view,
                    offsets=[row_slice_base, 0],
                    sizes=[vec_rows_idx, full_dim_idx],
                )
                fa4_finalize_o(o_prev_tile, l_prev_tile, o_final_tile, vec_rows, full_dim)
                pto.tile.store(o_final_tile, o_part)

                _fa4_drain_pending_syncs(
                    CV_comm_ptr,
                    drain_comm_base,
                    tile_block_count,
                    CV_FIFO_SIZE,
                    CV_FIFO_CONS_SYNC_PERIOD,
                )


def build_arg_parser():
    parser = argparse.ArgumentParser(
        description="Emit compile-only PTODSL MLIR for the A5 flash_atten4 port.",
    )
    parser.add_argument("--s0", type=int, default=MANUAL_S0)
    parser.add_argument("--s1", type=int, default=DEFAULT_S1)
    parser.add_argument("--head-dim", type=int, default=MANUAL_HEAD_DIM)
    parser.add_argument("--cube-s0", type=int, default=MANUAL_CUBE_S0)
    parser.add_argument("--cube-s1", type=int, default=MANUAL_CUBE_S1)
    parser.add_argument("--tile-s1", type=int, default=MANUAL_TILE_S1)
    parser.add_argument("--qk-preload", type=int, default=MANUAL_QK_PRELOAD, choices=(3, 4))
    parser.add_argument("--cv-fifo-size", type=int, default=MANUAL_CV_FIFO_SIZE)
    parser.add_argument(
        "--cv-fifo-cons-sync-period",
        type=int,
        default=MANUAL_CV_FIFO_CONS_SYNC_PERIOD,
    )
    parser.add_argument("--fifo-mode", type=int, default=MANUAL_FIFO_MODE, choices=(0, 1, 2))
    parser.add_argument("--causal", action="store_true")
    parser.add_argument("-o", "--output", default="-", help="output MLIR path, or '-' for stdout")
    return parser


def main(argv=None):
    args = build_arg_parser().parse_args(argv)
    mlir_text = emit_flash_atten4_port_mlir(
        s0=args.s0,
        s1=args.s1,
        head_dim=args.head_dim,
        cube_s0=args.cube_s0,
        cube_s1=args.cube_s1,
        tile_s1=args.tile_s1,
        qk_preload=args.qk_preload,
        cv_fifo_size=args.cv_fifo_size,
        cv_fifo_cons_sync_period=args.cv_fifo_cons_sync_period,
        fifo_mode=args.fifo_mode,
        causal=args.causal,
    )
    if args.output == "-":
        print(mlir_text)
        return
    Path(args.output).write_text(mlir_text, encoding="utf-8")


if __name__ == "__main__":
    main()
