#!/usr/bin/env python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

import os
import sys

import numpy as np


def main() -> None:
  strict = os.getenv("COMPARE_STRICT", "1") != "0"
  if not os.path.exists("golden_v2.bin") or not os.path.exists("v2.bin"):
    print("[ERROR] Missing golden_v2.bin or v2.bin")
    sys.exit(2 if strict else 0)

  golden = np.fromfile("golden_v2.bin", dtype=np.uint8)
  output = np.fromfile("v2.bin", dtype=np.uint8)

  if golden.shape != output.shape:
    print(f"[ERROR] Shape mismatch: golden {golden.shape} vs output {output.shape}")
    sys.exit(2 if strict else 0)

  if not np.array_equal(golden, output):
    diff = np.nonzero(golden != output)[0]
    idx = int(diff[0]) if diff.size else 0
    print(
        f"[ERROR] Byte mismatch at idx={idx}: golden={int(golden[idx])} output={int(output[idx])}"
    )
    sys.exit(2 if strict else 0)

  print("[INFO] compare passed")


if __name__ == "__main__":
  main()
