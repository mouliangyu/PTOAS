#!/usr/bin/env python3
# case: micro-op/vec-scalar/vsubcs
# family: vec-scalar
# target_ops: pto.vsubcs
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
    lhs = rng.integers(0, 0xFFFFFFFF, size=LANES, dtype=np.uint32)
    rhs = rng.integers(0, 0xFFFFFFFF, size=LANES, dtype=np.uint32)
    subtrahend = rhs.astype(np.uint64) + np.uint64(1)
    lhs64 = lhs.astype(np.uint64)
    borrow = lhs64 < subtrahend
    result = ((lhs64 - subtrahend) & np.uint64(0xFFFFFFFF)).astype(np.uint32)

    output_dir.mkdir(parents=True, exist_ok=True)
    lhs.tofile(output_dir / "v1.bin")
    rhs.tofile(output_dir / "v2.bin")
    np.zeros(LANES, dtype=np.uint32).tofile(output_dir / "v3.bin")
    np.zeros(256, dtype=np.uint8).tofile(output_dir / "v4.bin")
    result.tofile(output_dir / "golden_v3.bin")
    pack_mask_bits(borrow).tofile(output_dir / "golden_v4.bin")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()
    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
