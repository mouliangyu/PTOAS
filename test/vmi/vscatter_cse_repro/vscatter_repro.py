#!/usr/bin/env python3
"""Minimal reproducer for vscatter CSE bug.

Loads UB[0..63] (all 1.0), scatters 42.0 to UB[3], then reloads UB[0..63].
Both loads are stored to separate UB regions (both are live) to trigger CSE.

Correct:  post[3] = 42.0  (vscatter visible after reload)
Bug:      post[3] = 0.0   (bisheng CSE'd the second vload into the first)

Run:
    CMAKE_PREFIX_PATH="" \\
    ASCEND_HOME_PATH=... \\
    PTOAS_BIN=.../ptoas \\
    PYTHONPATH=... bash .../sim_dsl.sh --soc-version Ascend950PR_9599 \\
    --output /tmp/vscatter_repro_out \\
    /path/to/vmi-demo/moe/topk_gate/vscatter_repro/vscatter_repro.py
"""
import ctypes

import numpy as np

from ptodsl import pto

# ── ACL runtime helpers ──────────────────────────────────────────────────────

ACL_MEM_MALLOC_HUGE_FIRST = 0
ACL_MEMCPY_HOST_TO_DEVICE = 1
ACL_MEMCPY_DEVICE_TO_HOST = 2


def load_acl():
    lib = ctypes.CDLL("libascendcl.so")
    lib.aclInit.argtypes = [ctypes.c_char_p]
    lib.aclInit.restype = ctypes.c_int
    lib.aclFinalize.argtypes = []
    lib.aclFinalize.restype = ctypes.c_int
    lib.aclrtSetDevice.argtypes = [ctypes.c_int]
    lib.aclrtSetDevice.restype = ctypes.c_int
    lib.aclrtCreateStream.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
    lib.aclrtCreateStream.restype = ctypes.c_int
    lib.aclrtDestroyStream.argtypes = [ctypes.c_void_p]
    lib.aclrtDestroyStream.restype = ctypes.c_int
    lib.aclrtMalloc.argtypes = [ctypes.POINTER(ctypes.c_void_p), ctypes.c_size_t, ctypes.c_int]
    lib.aclrtMalloc.restype = ctypes.c_int
    lib.aclrtFree.argtypes = [ctypes.c_void_p]
    lib.aclrtFree.restype = ctypes.c_int
    lib.aclrtMemcpy.argtypes = [
        ctypes.c_void_p, ctypes.c_size_t, ctypes.c_void_p, ctypes.c_size_t, ctypes.c_int,
    ]
    lib.aclrtMemcpy.restype = ctypes.c_int
    lib.aclrtSynchronizeStream.argtypes = [ctypes.c_void_p]
    lib.aclrtSynchronizeStream.restype = ctypes.c_int
    return lib


# ── Kernel ───────────────────────────────────────────────────────────────────

VLANES = 64


@pto.jit(
    name="vscatter_cse_repro",
    target="a5",
    backend="vpto",
    mode="explicit",
    kernel_kind="vector",
    insert_sync=False,
    ast_rewrite=False,
)
def vscatter_cse_repro(
    data_gm: pto.ptr(pto.f32, "gm"),
    result_gm: pto.ptr(pto.f32, "gm"),
):
    """Scatter 42.0 to UB[3], reload, output. Tests vscatter + mem_bar + vload.

    Both loads are live (stored to separate UB regions) to trigger bisheng CSE.
    """
    data_ub = pto.castptr(pto.const(0, dtype=pto.ui64), pto.ptr(pto.f32, "ub"))
    out_ub = pto.castptr(pto.const(256, dtype=pto.ui64), pto.ptr(pto.f32, "ub"))

    pto.mte_gm_ub(data_gm, data_ub, 0, VLANES * 4,
                  nburst=(1, VLANES * 4, VLANES * 4))
    pto.set_flag(pto.Pipe.MTE2, pto.Pipe.V, event_id=0)
    pto.wait_flag(pto.Pipe.MTE2, pto.Pipe.V, event_id=0)

    mask64 = pto.vmi.create_mask(VLANES, size=VLANES)

    # Load original data (all 1.0) — store to output[0..63] BEFORE scatter
    load1 = pto.vmi.vload(data_ub, 0, size=VLANES)
    pto.vmi.vstore(load1, out_ub, 0, mask64)

    # Scatter 42.0 to UB[3]
    offset_val = pto.vmi.vbrc(pto.i32(3), size=VLANES)
    scatter_val = pto.vmi.vbrc(pto.f32(42.0), size=VLANES)
    pto.vmi.vscatter(scatter_val, data_ub, offset_val, mask64)

    # mem_bar should enforce VST→VLD ordering
    pto.mem_bar(pto.BarrierType.VST_VLD)

    # Reload: should see 42.0 at position 3
    load2 = pto.vmi.vload(data_ub, 0, size=VLANES)

    # Output reload result to a DIFFERENT UB region (offset 128 bytes)
    out_ub2 = pto.castptr(pto.const(128, dtype=pto.ui64), pto.ptr(pto.f32, "ub"))
    pto.vmi.vstore(load2, out_ub2, 0, mask64)

    pto.set_flag(pto.Pipe.V, pto.Pipe.MTE3, event_id=1)
    pto.wait_flag(pto.Pipe.V, pto.Pipe.MTE3, event_id=1)
    # Copy pre-scatter data to result_gm[0..63]
    pto.mte_ub_gm(out_ub, result_gm, VLANES * 4,
                  nburst=(1, VLANES * 4, VLANES * 4))
    # Copy post-scatter reload to result_gm[64..127]
    result_gm2 = pto.castptr(pto.const(256, dtype=pto.ui64), pto.ptr(pto.f32, "gm"))
    pto.mte_ub_gm(out_ub2, result_gm2, VLANES * 4,
                  nburst=(1, VLANES * 4, VLANES * 4))


# ── Main ─────────────────────────────────────────────────────────────────────

def main():
    acl = load_acl()
    acl.aclInit(None)
    assert acl.aclrtSetDevice(0) == 0

    data_size = VLANES * 4          # 64 floats = 256 bytes
    result_size = VLANES * 4 * 2    # 128 floats = 512 bytes (pre + post)
    data_gm = ctypes.c_void_p()
    result_gm = ctypes.c_void_p()
    acl.aclrtMalloc(ctypes.byref(data_gm), data_size, ACL_MEM_MALLOC_HUGE_FIRST)
    acl.aclrtMalloc(ctypes.byref(result_gm), result_size, ACL_MEM_MALLOC_HUGE_FIRST)

    data = np.ones(VLANES, dtype=np.float32)
    result = np.zeros(VLANES * 2, dtype=np.float32)
    acl.aclrtMemcpy(data_gm, data_size, data.ctypes.data, data_size, ACL_MEMCPY_HOST_TO_DEVICE)

    stream = ctypes.c_void_p()
    acl.aclrtCreateStream(ctypes.byref(stream))

    kernel = vscatter_cse_repro.compile()
    kernel[1, stream](data_gm, result_gm)
    acl.aclrtSynchronizeStream(stream)

    acl.aclrtMemcpy(result.ctypes.data, result_size, result_gm, result_size, ACL_MEMCPY_DEVICE_TO_HOST)

    pre  = result[:VLANES]    # data loaded BEFORE vscatter
    post = result[VLANES:]    # data loaded AFTER vscatter

    print(f"Pre-scatter[3]  = {pre[3]}   (expected 1.0)")
    print(f"Post-scatter[3] = {post[3]}   (expected 42.0)")
    print(f"Pre-scatter[0:8]  = {pre[0:8]}")
    print(f"Post-scatter[0:8] = {post[0:8]}")
    if post[3] == 42.0:
        print("PASS: vscatter correctly visible after reload")
    else:
        print(f"FAIL: vscatter write lost — bisheng CSE'd the reload")
        print(f"  Expected post[3] = 42.0, got {post[3]}")

    acl.aclrtFree(data_gm)
    acl.aclrtFree(result_gm)
    acl.aclrtDestroyStream(stream)
    acl.aclFinalize()


if __name__ == "__main__":
    main()
