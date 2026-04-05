#!/usr/bin/env python3
# case: micro-op/vec-scalar/vsadds
# family: vec-scalar
# target_ops: pto.vsadds
# scenarios: core-i16-signed, full-mask, scalar-operand

import argparse
from pathlib import Path

import numpy as np


ELEMS = 1024
SEED = 19
SCALAR = np.int16(37)
I16_MIN = np.iinfo(np.int16).min
I16_MAX = np.iinfo(np.int16).max


def sat_add(vec: np.ndarray, scalar: np.int16, min_value: int, max_value: int) -> np.ndarray:
    wide = vec.astype(np.int32) + int(scalar)
    return np.clip(wide, min_value, max_value).astype(np.int16)


def generate(output_dir: Path, seed: int) -> None:
    rng = np.random.default_rng(seed)
    v1 = rng.integers(-12000, 12000, size=ELEMS, dtype=np.int16)
    v2 = np.zeros(ELEMS, dtype=np.int16)
    golden_v2 = sat_add(v1, SCALAR, I16_MIN, I16_MAX)

    output_dir.mkdir(parents=True, exist_ok=True)
    v1.tofile(output_dir / "v1.bin")
    v2.tofile(output_dir / "v2.bin")
    golden_v2.tofile(output_dir / "golden_v2.bin")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()
    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
