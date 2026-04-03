#!/usr/bin/env python3
# case: micro-op/compare-select/vselr
# family: compare-select
# target_ops: pto.vselr
# scenarios: core-f32, full-mask, explicit-lane-index
# coding=utf-8

import argparse
from pathlib import Path

import numpy as np


ROWS = 32
COLS = 32
SEED = 19


def generate(output_dir: Path, seed: int) -> None:
    rng = np.random.default_rng(seed)
    v1 = rng.uniform(-3.0, 3.0, size=(ROWS, COLS)).astype(np.float32, copy=False)
    idx = np.arange(ROWS * COLS, dtype=np.int32).reshape(ROWS, COLS)
    lane_ids = np.arange(64, dtype=np.int32).reshape(16, 64)
    idx = (lane_ids[:, ::-1] + (lane_ids // 8) * 3) % 64
    idx = idx.reshape(ROWS, COLS).astype(np.int32, copy=False)
    golden_v3 = np.take_along_axis(v1, idx, axis=1).astype(np.float32, copy=False)
    v3 = np.zeros((ROWS, COLS), dtype=np.float32)

    output_dir.mkdir(parents=True, exist_ok=True)
    v1.reshape(-1).tofile(output_dir / "v1.bin")
    idx.reshape(-1).tofile(output_dir / "v2.bin")
    v3.reshape(-1).tofile(output_dir / "v3.bin")
    golden_v3.reshape(-1).tofile(output_dir / "golden_v3.bin")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate numpy-based inputs/golden for VPTO micro-op vselr validation."
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("."),
        help="Directory where v1.bin/v2.bin/v3.bin/golden_v3.bin are written.",
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=SEED,
        help="Numpy random seed.",
    )
    args = parser.parse_args()

    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
