# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""
Minimal cube matmul JIT demo that mirrors the manual-address IR shape used by
``TMATMUL_f16_16x16x16``.

The boundary movement uses explicit GM->L1 fractal loads and L0C->GM store,
while the core matmul stays on the TileOp path through ``pto.tmatmul``. The
same compiled kernel is launched with multiple runtime batch sizes.
"""

import argparse
import time

import numpy as np
from mlir.dialects import pto as _pto
from mlir.ir import Attribute

from ptodsl import pto, scalar as s
from ptodsl._ops import _coerce_i64
from ptodsl._surface_values import unwrap_surface_value

_DEVICE = "npu:0"


def _raw(value):
    return unwrap_surface_value(value)


def _mte_gm_l1_frac(source, destination, *, shape, src_layout, dst_group, ctrl):
    _pto.mte_gm_l1_frac(
        _raw(source),
        _raw(destination),
        _coerce_i64(shape[0], context="mte_gm_l1_frac shape[0]"),
        _coerce_i64(shape[1], context="mte_gm_l1_frac shape[1]"),
        _coerce_i64(src_layout[0], context="mte_gm_l1_frac src_layout[0]"),
        _coerce_i64(dst_group[0], context="mte_gm_l1_frac dst_group[0]"),
        _coerce_i64(dst_group[1], context="mte_gm_l1_frac dst_group[1]"),
        _coerce_i64(dst_group[2], context="mte_gm_l1_frac dst_group[2]"),
        _coerce_i64(dst_group[3], context="mte_gm_l1_frac dst_group[3]"),
        _coerce_i64(ctrl[0], context="mte_gm_l1_frac ctrl[0]"),
        _raw(ctrl[1]),
        Attribute.parse("#pto<cube_load_frac_mode nd2nz>"),
    )


def _tmatmul(lhs, rhs, dst):
    _pto.tmatmul(None, _raw(lhs), _raw(rhs), _raw(dst))


@pto.jit(
    name="TMATMUL_f16_16x16x16",
    target="a5",
    kernel_kind="cube",
    mode="explicit",
    insert_sync=False,
)
def TMATMUL_f16_16x16x16(
    A: pto.tensor_spec(rank=3, dtype=pto.f16),
    B: pto.tensor_spec(rank=3, dtype=pto.f16),
    C: pto.tensor_spec(rank=3, dtype=pto.f32),
    dim: pto.i32,
    batch: pto.i32,
):
    c0_idx = pto.const(0)
    c1_idx = pto.const(1)
    c0 = pto.const(0, dtype=pto.i64)
    c1 = pto.const(1, dtype=pto.i64)
    c2 = pto.const(2)
    c16 = s.index_cast(dim)
    c_batch = s.index_cast(batch)
    c16_static = pto.const(16)
    c32 = s.muli(c16_static, c2)
    c_tile_elems = s.muli(c16_static, c16_static)
    false = pto.const(0, dtype=pto.i1)

    l1_a_tile = pto.alloc_tile(
        shape=[16, 16],
        dtype=pto.f16,
        memory_space=pto.MemorySpace.MAT,
        blayout="ColMajor",
        slayout="RowMajor",
        addr=0,
    )
    l1_b_tile = pto.alloc_tile(
        shape=[16, 16],
        dtype=pto.f16,
        memory_space=pto.MemorySpace.MAT,
        blayout="ColMajor",
        slayout="RowMajor",
        addr=512,
    )
    l0a_tile = pto.alloc_tile(
        shape=[16, 16],
        dtype=pto.f16,
        memory_space=pto.MemorySpace.LEFT,
        blayout="ColMajor",
        slayout="RowMajor",
        addr=0,
    )
    l0b_tile = pto.alloc_tile(
        shape=[16, 16],
        dtype=pto.f16,
        memory_space=pto.MemorySpace.RIGHT,
        blayout="RowMajor",
        slayout="ColMajor",
        addr=0,
    )
    l0c_tile = pto.alloc_tile(
        shape=[16, 16],
        dtype=pto.f32,
        memory_space=pto.MemorySpace.ACC,
        blayout="ColMajor",
        slayout="RowMajor",
        fractal_size=1024,
        addr=0,
    )

    l1_a = l1_a_tile.as_ptr()
    l1_b = l1_b_tile.as_ptr()
    l0a = l0a_tile.as_ptr()
    l0b = l0b_tile.as_ptr()
    l0c = l0c_tile.as_ptr()
    a_view = pto.make_tensor_view(
        A,
        shape=[c_batch, c16_static, c16_static],
        strides=[c_tile_elems, c16_static, c1_idx],
    )
    b_view = pto.make_tensor_view(
        B,
        shape=[c_batch, c16_static, c16_static],
        strides=[c_tile_elems, c16_static, c1_idx],
    )
    c_view = pto.make_tensor_view(
        C,
        shape=[c_batch, c16_static, c16_static],
        strides=[c_tile_elems, c16_static, c1_idx],
    )

    with pto.for_(c0_idx, c_batch, step=c1_idx) as batch_idx:
        a_part = pto.partition_view(
            a_view,
            offsets=[batch_idx, c0_idx, c0_idx],
            sizes=[c1_idx, c16, c16],
        )
        b_part = pto.partition_view(
            b_view,
            offsets=[batch_idx, c0_idx, c0_idx],
            sizes=[c1_idx, c16, c16],
        )
        c_part = pto.partition_view(
            c_view,
            offsets=[batch_idx, c0_idx, c0_idx],
            sizes=[c1_idx, c16, c16],
        )
        a_ptr = a_part.as_ptr()
        b_ptr = b_part.as_ptr()
        c_ptr = c_part.as_ptr()

        _mte_gm_l1_frac(
            a_ptr,
            l1_a,
            shape=(c16, c16),
            src_layout=(c32,),
            dst_group=(c1, c1, c16, c0),
            ctrl=(c0, false),
        )
        pto.set_flag("MTE2", "MTE1", event_id=0)
        pto.wait_flag("MTE2", "MTE1", event_id=0)
        pto.mte_l1_l0a(l1_a, l0a, c16, c16)

        _mte_gm_l1_frac(
            b_ptr,
            l1_b,
            shape=(c16, c16),
            src_layout=(c32,),
            dst_group=(c1, c1, c16, c0),
            ctrl=(c0, false),
        )
        pto.set_flag("MTE2", "MTE1", event_id=1)
        pto.wait_flag("MTE2", "MTE1", event_id=1)
        pto.mte_l1_l0b(l1_b, l0b, c16, c16, transpose=True)

        pto.set_flag("MTE1", "M", event_id=0)
        pto.wait_flag("MTE1", "M", event_id=0)
        _tmatmul(l0a_tile, l0b_tile, l0c_tile)

        pto.set_flag("M", "FIX", event_id=1)
        pto.wait_flag("M", "FIX", event_id=1)
        pto.mte_l0c_gm(l0c, c_ptr, c16, c16, c16_static, c16_static, c0, c0)

    pto.pipe_barrier(pto.Pipe.ALL)


def emit_mlir():
    return pto.merge_jit_modules(TMATMUL_f16_16x16x16)


def init_runtime():
    import torch
    import torch_npu  # noqa: F401

    torch.npu.config.allow_internal_format = False
    torch_npu.npu.set_compile_mode(jit_compile=False)
    torch.npu.set_device(_DEVICE)
    return torch


def npu_stream(torch):
    return torch.npu.current_stream()._as_parameter_  # noqa: SLF001


def test_tmatmul() -> None:
    torch = init_runtime()
    rng = np.random.RandomState(0)
    stream = npu_stream(torch)
    cases = [
        (int(rng.randint(4, 16)), int(rng.randint(1, 5)))
        for _ in range(2)
    ] + [(16, 3)]

    t0 = time.perf_counter()
    compiled = TMATMUL_f16_16x16x16.compile()
    compile_s = time.perf_counter() - t0

    for dim, batch in cases:
        a_np = np.zeros((batch, 16, 16), dtype=np.float16)
        b_np = np.zeros((batch, 16, 16), dtype=np.float16)
        a_np[:, :dim, :dim] = rng.uniform(
            -1.0, 1.0, size=(batch, dim, dim)
        ).astype(np.float16)
        b_np[:, :dim, :dim] = rng.uniform(
            -1.0, 1.0, size=(batch, dim, dim)
        ).astype(np.float16)
        ref = np.matmul(
            a_np[:, :dim, :dim].astype(np.float32),
            b_np[:, :dim, :dim].astype(np.float32),
        )

        a = torch.from_numpy(a_np).to(_DEVICE)
        b = torch.from_numpy(b_np).to(_DEVICE)
        c = torch.empty((batch, 16, 16), dtype=torch.float32, device=_DEVICE)

        t0 = time.perf_counter()
        compiled[1, stream](a, b, c, dim, batch)
        torch.npu.synchronize()
        launch_s = time.perf_counter() - t0

        np.testing.assert_allclose(
            c.cpu().numpy()[:, :dim, :dim], ref, rtol=1e-6, atol=1e-6
        )
        print(
            f"PASS TMATMUL_f16_batch{batch}_{dim}x{dim}x{dim}  "
            f"compile={compile_s:.3f}s launch={launch_s:.3f}s"
        )


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--emit-mlir",
        action="store_true",
        help="print the generated MLIR module and exit",
    )
    args = parser.parse_args(argv)

    if args.emit_mlir:
        print(emit_mlir())
        return 0

    test_tmatmul()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
