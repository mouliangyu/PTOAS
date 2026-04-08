#!/usr/bin/env python3
# case: micro-op/compare-select/vcmps-unordered-f32
# family: compare-select
# target_ops: pto.vcmps
# scenarios: core-f32, full-mask, scalar-operand, exceptional-values
# NOTE: blocked placeholder case. The current PTO surface and docs only expose
# eq/ne/lt/le/gt/ge compare modes for pto.vcmps, so a true unordered compare
# case cannot be expressed yet. This script only materializes placeholder
# inputs for the host harness; it is not a semantic oracle.
# coding=utf-8

import argparse
from pathlib import Path

import numpy as np


ROWS = 32
COLS = 32
SEED = 19
SRC_ELEM_BYTES = 4
BLOCKED_REASON = (
    "blocked placeholder: unordered compare is not part of the current "
    "pto.vcmps surface; docs/isa/11-compare-select.md only defines "
    "eq/ne/lt/le/gt/ge"
)


def generate(output_dir: Path, seed: int, src_elem_bytes: int) -> None:
    rng = np.random.default_rng(seed)

    v1 = rng.uniform(-3.0, 3.0, size=(ROWS, COLS)).astype(np.float32)
    v2 = np.full((ROWS, COLS), np.float32(np.nan), dtype=np.float32)
    output_init = np.zeros((ROWS * COLS,), dtype=np.uint8)

    output_dir.mkdir(parents=True, exist_ok=True)
    v1.reshape(-1).tofile(output_dir / "v1.bin")
    v2.reshape(-1).tofile(output_dir / "v2.bin")
    output_init.tofile(output_dir / "v3.bin")
    output_init.tofile(output_dir / "golden_v3.bin")
    (output_dir / "BLOCKED.txt").write_text(BLOCKED_REASON + "\n", encoding="ascii")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate numpy-based inputs/golden for VPTO micro-op vcmp-eq validation."
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
    parser.add_argument(
        "--src-elem-bytes",
        type=int,
        default=SRC_ELEM_BYTES,
        help="Unused placeholder argument kept for harness compatibility.",
    )
    args = parser.parse_args()

    generate(args.output_dir, args.seed, args.src_elem_bytes)


if __name__ == "__main__":
    main()
