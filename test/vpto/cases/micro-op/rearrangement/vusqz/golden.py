#!/usr/bin/env python3
# case: micro-op/rearrangement/vusqz
# family: rearrangement
# target_ops: pto.vusqz
# scenarios: predicate-driven-rearrangement, placement

import argparse
from pathlib import Path

import numpy as np


ROWS = 32
COLS = 32
LANES = 64
BLOCKS = ROWS * COLS // LANES
ACTIVE_PER_BLOCK = 16
SEED = 19


def build_case() -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    src = np.zeros((BLOCKS, LANES), dtype=np.int32)
    mask_seed = np.full((BLOCKS, LANES), -1.0, dtype=np.float32)
    out = np.zeros((BLOCKS, LANES), dtype=np.int32)

    for block in range(BLOCKS):
      values = np.arange(block * 100 + 1, block * 100 + ACTIVE_PER_BLOCK + 1,
                         dtype=np.int32)
      src[block, :ACTIVE_PER_BLOCK] = values
      mask_seed[block, :ACTIVE_PER_BLOCK] = 1.0
      out[block, :ACTIVE_PER_BLOCK] = values

    return src.reshape(ROWS, COLS), mask_seed.reshape(ROWS, COLS), out.reshape(ROWS, COLS)


def generate(output_dir: Path) -> None:
    src, mask_seed, out = build_case()
    output_dir.mkdir(parents=True, exist_ok=True)
    src.reshape(-1).tofile(output_dir / "v1.bin")
    mask_seed.reshape(-1).tofile(output_dir / "v2.bin")
    out.reshape(-1).tofile(output_dir / "golden_v3.bin")


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate vusqz placement inputs/golden.")
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()
    del args.seed
    generate(args.output_dir)


if __name__ == "__main__":
    main()
