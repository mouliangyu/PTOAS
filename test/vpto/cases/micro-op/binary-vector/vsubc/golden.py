#!/usr/bin/env python3
# case: micro-op/binary-vector/vsubc
# family: binary-vector
# target_ops: pto.vsubc
# scenarios: core-u32-unsigned, full-mask, carry-chain

import argparse
from pathlib import Path

import numpy as np


LANES = 64
SEED = 19


def pack_mask_bits(bits):
    out = np.zeros(256, dtype=np.uint8)
    for idx, bit in enumerate(bits):
        if bit:
            out[idx // 8] |= np.uint8(1 << (idx % 8))
    return out


def generate(output_dir: Path, seed: int) -> None:
    rng = np.random.default_rng(seed)
    v1 = rng.integers(0, 0xFFFFFFFF, size=LANES, dtype=np.uint32)
    v2 = rng.integers(0, 0xFFFFFFFF, size=LANES, dtype=np.uint32)
    diff = (v1 - v2).astype(np.uint32, copy=False)
    borrow = v1 < v2

    output_dir.mkdir(parents=True, exist_ok=True)
    v1.tofile(output_dir / "v1.bin")
    v2.tofile(output_dir / "v2.bin")
    np.zeros(LANES, dtype=np.uint32).tofile(output_dir / "v3.bin")
    np.zeros(256, dtype=np.uint8).tofile(output_dir / "v4.bin")
    diff.tofile(output_dir / "golden_v3.bin")
    pack_mask_bits(borrow).tofile(output_dir / "golden_v4.bin")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()
    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
