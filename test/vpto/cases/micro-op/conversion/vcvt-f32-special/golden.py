#!/usr/bin/env python3
# case: micro-op/conversion/vcvt-f32-special
# family: conversion
# target_ops: pto.vcvt
# scenarios: f32-to-f16, exceptional-values
# NOTE: bulk-generated coverage skeleton.
# coding=utf-8

import argparse
from pathlib import Path

import numpy as np


ROWS = 32
COLS = 32
SEED = 19


def generate(output_dir: Path, seed: int) -> None:
    del seed
    special = np.array(
        [
            0.0,
            -0.0,
            1.0,
            -1.0,
            np.inf,
            -np.inf,
            np.nan,
            65504.0,
            -65504.0,
            1.0e-8,
            -1.0e-8,
            1.0e-4,
            -1.0e-4,
            123.75,
            -123.75,
            0.33333334,
        ],
        dtype=np.float32,
    )
    v1 = np.resize(special, ROWS * COLS).reshape(ROWS, COLS)
    v2 = np.zeros((ROWS, COLS), dtype=np.float16)
    golden_v2 = v1.astype(np.float16)

    output_dir.mkdir(parents=True, exist_ok=True)
    v1.reshape(-1).tofile(output_dir / "v1.bin")
    v2.reshape(-1).tofile(output_dir / "v2.bin")
    golden_v2.reshape(-1).tofile(output_dir / "golden_v2.bin")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate numpy-based inputs/golden for VPTO micro-op vcvt-f32-special validation."
    )
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()
    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
