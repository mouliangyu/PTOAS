#!/usr/bin/env python3
# case: micro-op/vec-scalar/vors-mask-edge
# family: vec-scalar
# target_ops: pto.vors
# scenarios: core-i16-unsigned, full-mask, scalar-operand
# NOTE: bulk-generated coverage skeleton.

import os
import sys
import numpy as np


def compare_bin_prefix(golden_path, output_path, dtype, eps, count):
    if not os.path.exists(golden_path) or not os.path.exists(output_path):
        return False
    golden = np.fromfile(golden_path, dtype=dtype, count=count)
    output = np.fromfile(output_path, dtype=dtype, count=count)
    return golden.shape == output.shape and np.allclose(
        golden, output, atol=eps, rtol=eps, equal_nan=True
    )


def main():
    strict = os.getenv("COMPARE_STRICT", "1") != "0"
    ok = compare_bin_prefix("golden_v2.bin", "v2.bin", np.float32, 1e-4, 1000)
    if not ok:
        if strict:
            print("[ERROR] compare failed")
            sys.exit(2)
        print("[WARN] compare failed (non-gating)")
        return
    print("[INFO] compare passed")


if __name__ == "__main__":
    main()
