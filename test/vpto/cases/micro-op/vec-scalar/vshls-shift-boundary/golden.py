#!/usr/bin/env python3
# case: micro-op/vec-scalar/vshls-shift-boundary
# family: vec-scalar
# target_ops: pto.vshls
# scenarios: core-i16-unsigned, full-mask, scalar-operand
# NOTE: bulk-generated coverage skeleton.
# coding=utf-8

import argparse
from pathlib import Path

import numpy as np


ROWS = 32
COLS = 32
SEED = 19
SHIFT = 31
LOGICAL_ELEMS = 1000


def generate(output_dir: Path, seed: int) -> None:
    rng = np.random.default_rng(seed)
    v1 = rng.integers(0, 1 << 10, size=(ROWS, COLS), dtype=np.uint32)
    v2 = np.zeros((ROWS, COLS), dtype=np.int32)
    golden_v2 = np.zeros((ROWS, COLS), dtype=np.int32)
    flat = (v1.reshape(-1)[:LOGICAL_ELEMS] << SHIFT).astype(np.uint32, copy=False)
    golden_v2.reshape(-1)[:LOGICAL_ELEMS] = flat.view(np.int32)

    output_dir.mkdir(parents=True, exist_ok=True)
    v1.view(np.int32).reshape(-1).tofile(output_dir / "v1.bin")
    v2.reshape(-1).tofile(output_dir / "v2.bin")
    golden_v2.reshape(-1).tofile(output_dir / "golden_v2.bin")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()
    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
