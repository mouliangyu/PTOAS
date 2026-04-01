#!/usr/bin/env python3
# coding=utf-8

import argparse
from pathlib import Path

import numpy as np


ROWS = 32
COLS = 32
SEED = 19


def generate(output_dir: Path, seed: int) -> None:
    rng = np.random.default_rng(seed)
    lhs = rng.uniform(-8.0, 8.0, size=(ROWS, COLS)).astype(np.float32)
    rhs = lhs.copy()

    lane_ids = np.arange(ROWS * COLS, dtype=np.int32).reshape(ROWS, COLS)
    edge_mask = ((lane_ids % 64) < 4) | ((lane_ids % 64) >= 60) | ((lane_ids % 17) == 0)
    rhs[edge_mask] = (rhs[edge_mask] + np.float32(3.5)).astype(np.float32)
    rhs[~edge_mask] = (rhs[~edge_mask] - np.float32(2.0)).astype(np.float32)

    golden_v3 = np.where(lhs > rhs, lhs, rhs).astype(np.float32, copy=False)
    v3 = np.zeros((ROWS, COLS), dtype=np.float32)

    output_dir.mkdir(parents=True, exist_ok=True)
    lhs.reshape(-1).tofile(output_dir / "v1.bin")
    rhs.reshape(-1).tofile(output_dir / "v2.bin")
    v3.reshape(-1).tofile(output_dir / "v3.bin")
    golden_v3.reshape(-1).tofile(output_dir / "golden_v3.bin")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate numpy-based inputs/golden for VPTO micro-op vsel predicate-edge validation."
    )
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()

    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
