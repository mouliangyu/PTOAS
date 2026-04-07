#!/usr/bin/env python3
# case: micro-op/rearrangement/vslide-tail-window
# family: rearrangement
# target_ops: pto.vslide
# scenarios: lane-order, slide-window, tail-mask
# NOTE: per-64-lane slide window with src1 == src0, amt == 3, logical elems == 1000.
# coding=utf-8

import argparse
from pathlib import Path

import numpy as np


ROWS = 32
COLS = 32
SEED = 19


def generate(output_dir: Path, seed: int) -> None:
    rng = np.random.default_rng(seed)
    flat = rng.uniform(-8.0, 8.0, size=ROWS * COLS).astype(np.float32)
    golden = np.zeros_like(flat)
    logical_elems = 1000
    for base in range(0, flat.size, 64):
        active = max(0, min(64, logical_elems - base))
        if active <= 0:
            continue
        chunk = flat[base : base + 64]
        golden[base : base + active] = np.roll(chunk, 3)[:active]
    v1 = flat.reshape(ROWS, COLS)
    v2 = np.zeros((ROWS, COLS), dtype=np.float32)
    golden_v2 = golden.reshape(ROWS, COLS)

    output_dir.mkdir(parents=True, exist_ok=True)
    v1.reshape(-1).tofile(output_dir / "v1.bin")
    v2.reshape(-1).tofile(output_dir / "v2.bin")
    golden_v2.reshape(-1).tofile(output_dir / "golden_v2.bin")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate numpy-based inputs/golden for VPTO micro-op vslide tail validation."
    )
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()
    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
