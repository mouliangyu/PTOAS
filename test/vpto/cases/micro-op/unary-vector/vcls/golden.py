#!/usr/bin/env python3
# case: micro-op/unary-vector/vcls
# family: unary-vector
# target_ops: pto.vcls
# scenarios: core-i16-signed, full-mask
# NOTE: bulk-generated coverage skeleton.
# coding=utf-8

import argparse
from pathlib import Path

import numpy as np


ROWS = 32
COLS = 32
SEED = 19

def _clrsb32(x: int) -> int:
    sx = int(np.int32(x))
    if sx >= 0:
        return 31 if sx == 0 else 31 - sx.bit_length()
    inv = (~sx) & 0xFFFFFFFF
    return 31 if inv == 0 else 31 - inv.bit_length()


def generate(output_dir: Path, seed: int) -> None:
    rng = np.random.default_rng(seed)
    v1 = rng.integers(np.iinfo(np.int32).min, np.iinfo(np.int32).max, size=(ROWS, COLS), dtype=np.int32)
    v2 = np.zeros((ROWS, COLS), dtype=np.int32)
    golden_v2 = np.vectorize(_clrsb32, otypes=[np.int32])(v1)

    output_dir.mkdir(parents=True, exist_ok=True)
    v1.reshape(-1).tofile(output_dir / "v1.bin")
    v2.reshape(-1).tofile(output_dir / "v2.bin")
    golden_v2.reshape(-1).tofile(output_dir / "golden_v2.bin")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate numpy-based inputs/golden for VPTO micro-op vcls validation."
    )
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()
    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
