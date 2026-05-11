#!/usr/bin/env python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

import argparse
import struct
from pathlib import Path

import numpy as np


M = 40
N = 64
K = 50
SEED = 97


def generate(output_dir: Path, seed: int) -> None:
    del seed
    a = np.ones((M, K), dtype=np.float16)
    b = np.ones((K, N), dtype=np.float16)
    elt = (np.arange(N, dtype=np.float16) % np.float16(8.0)).astype(np.float16)
    out = np.zeros((M, N), dtype=np.uint16)
    matmul = np.zeros((M, N), dtype=np.float32)
    a32 = a.astype(np.float32)
    b32 = b.astype(np.float32)
    for k_idx in range(K):
        matmul += a32[:, k_idx:k_idx + 1] * b32[k_idx:k_idx + 1, :]
    pre = matmul.astype(np.float16)
    eltwise = (pre + elt.reshape(1, N)).astype(np.float16)
    golden = np.rint(eltwise.astype(np.float32)).astype(np.int16)

    output_dir.mkdir(parents=True, exist_ok=True)
    a.reshape(-1).tofile(output_dir / "v1.bin")
    b.reshape(-1).tofile(output_dir / "v2.bin")
    elt.reshape(-1).tofile(output_dir / "v3.bin")
    out.reshape(-1).tofile(output_dir / "v4.bin")
    golden.reshape(-1).tofile(output_dir / "golden_v4.bin")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()
    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
