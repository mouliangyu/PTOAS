# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""
Math: Cast [srcM=64, 128] f32 ND-layout to bf16 NZ-layout [8, 64, 16].
  Per row: vload 128 f32, vcvt to 128 bf16, vstore block-strided to NZ layout.
  Golden uses round-to-nearest-even bf16 conversion (byte-exact check).
"""

import argparse
import ctypes
import time

import numpy as np
import torch
import torch_npu 
from ptodsl import pto

torch.npu.config.allow_internal_format = False
torch_npu.npu.set_compile_mode(jit_compile=False)
torch.npu.set_device("npu:0")

SRC_N = 128
SRC_M = 64

_ACL_MEM_MALLOC_HUGE_FIRST = 0
_ACL_MEMCPY_HOST_TO_DEVICE = 1
_ACL_MEMCPY_DEVICE_TO_HOST = 2


@pto.jit(
    name="cast_nd2nz",
    target="a5",
    backend="vpto",
    mode="explicit",
    kernel_kind="vector",
    insert_sync=False,
)
def cast_nd2nz(
    out_ptr: pto.ptr(pto.bf16, "gm"),
    in_ptr: pto.ptr(pto.f32, "gm"),
):
    in_bytes = SRC_M * SRC_N * 4
    out_bytes = SRC_M * SRC_N * 2

    x_ub = pto.castptr(pto.const(0, dtype=pto.i64), pto.ptr(pto.f32, "ub"))
    y_ub = pto.castptr(pto.const(in_bytes, dtype=pto.i64), pto.ptr(pto.bf16, "ub"))

    pto.mte_load(in_ptr, x_ub, 0, in_bytes, nburst=(1, in_bytes, in_bytes))
    pto.set_flag("MTE2", "V", event_id=0)
    pto.wait_flag("MTE2", "V", event_id=0)

    mask = pto.vmi.create_mask(128, size=128)
    bs = pto.i16(SRC_M)
    rs = pto.i16(1)
    for m in range(SRC_M):
        x = pto.vmi.vload(x_ub, m * 128, size=128)
        h = pto.vmi.vcvt(x, to_dtype=pto.bf16)
        pto.vmi.vstore(h, y_ub, m * 16, stride=SRC_M*16, group=8)

    pto.set_flag("V", "MTE3", event_id=0)
    pto.wait_flag("V", "MTE3", event_id=0)
    pto.mte_store(y_ub, out_ptr, out_bytes, nburst=(1, out_bytes, out_bytes))


def emit_mlir() -> str:
    return cast_nd2nz.compile().mlir_text()


def to_bfloat16_bytes(f32_array):
    u32 = f32_array.astype(np.float32).view(np.uint32)
    rounding_bias = 0x7FFF + ((u32 >> 16) & 1)
    bf16 = ((u32 + rounding_bias) >> 16).astype(np.uint16)
    return bf16


def cast_nd2nz_bf16(x):
    srcM, srcN = x.shape
    N1 = srcN // 16
    x_cast = to_bfloat16_bytes(x.reshape(-1)).reshape(srcM, srcN)
    nz = x_cast.reshape(srcM, N1, 16).transpose(1, 0, 2).reshape(-1)
    return nz


def _gen_input():
    np.random.seed(2026)
    np.random.uniform(-5, 5, 64 * 128)  # skip case1
    np.random.uniform(-5, 5, 128 * 128)  # skip case2
    x = np.random.uniform(-5, 5, SRC_M * SRC_N).astype(np.float32).reshape(SRC_M, SRC_N)
    return x

def run_case_torch(torch):
    x = _gen_input()
    ref = cast_nd2nz_bf16(x)
    x_t = torch.from_numpy(x.copy()).to("npu:0")
    y_t = torch.zeros(SRC_M * SRC_N, dtype=torch.bfloat16, device="npu:0")
    stream = torch.npu.current_stream()._as_parameter_
    t0 = time.perf_counter()
    compiled = cast_nd2nz.compile()
    compile_s = time.perf_counter() - t0
    t0 = time.perf_counter()
    compiled[1, stream](y_t.data_ptr(), x_t.data_ptr())
    torch.npu.synchronize()
    launch_s = time.perf_counter() - t0
    out = y_t.cpu().numpy().view(np.uint16)
    if not np.array_equal(out, ref):
        mism = np.where(out != ref)[0]
        i = int(mism[0])
        raise AssertionError(f"bf16 mismatch at {i}: got={out[i]} ref={ref[i]} count={len(mism)}")
    print(f"PASS cast_nd2nz N={SRC_N}  compile={compile_s:.3f}s launch={launch_s:.3f}s")


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--emit-mlir", action="store_true", help="print MLIR module and exit (compile-only)")
    args = parser.parse_args(argv)
    if args.emit_mlir:
        print(emit_mlir())
        return 0
    run_case_torch(torch)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
