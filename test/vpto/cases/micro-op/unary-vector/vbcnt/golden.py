#!/usr/bin/env python3
# case: micro-op/unary-vector/vbcnt
# family: unary-vector
# target_ops: pto.vbcnt
# scenarios: core-i16-unsigned, full-mask
# NOTE: bulk-generated coverage skeleton.
# coding=utf-8

import argparse
from pathlib import Path

import numpy as np


ROWS = 32
COLS = 32
SEED = 19

LOGICAL_ELEMS = 1024


def _bitcount32(x: int) -> int:
    return (int(x) & 0xFFFFFFFF).bit_count()


def generate(output_dir: Path, seed: int) -> None:
    rng = np.random.default_rng(seed)
    v1 = rng.integers(0, np.iinfo(np.uint32).max, size=(ROWS, COLS), dtype=np.uint32)
    v2 = np.zeros((ROWS, COLS), dtype=np.int32)
    golden_v2 = np.vectorize(_bitcount32, otypes=[np.int32])(v1.view(np.int32))

    output_dir.mkdir(parents=True, exist_ok=True)
    v1.view(np.int32).reshape(-1).tofile(output_dir / "v1.bin")
    v2.reshape(-1).tofile(output_dir / "v2.bin")
    golden_v2.reshape(-1).tofile(output_dir / "golden_v2.bin")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate numpy-based inputs/golden for VPTO micro-op vbcnt validation."
    )
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()
    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
