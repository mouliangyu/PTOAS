#!/usr/bin/env python3
# case: micro-op/predicate-load-store/psts-plds-packed-prefix-boundary
# family: predicate-load-store
# target_ops: pto.plds, pto.psts
# scenarios: packed-predicate-roundtrip, dynamic-offset, load-store-pair-preservation, representative-logical-elements

import argparse
from pathlib import Path

import numpy as np


ROWS = 32
COLS = 32
SEED = 19
LOGICAL_ELEMS = 1000
SRC_ELEM_BYTES = 4
REPEAT_BYTES = 256


def _ceil_div(x: int, y: int) -> int:
    return (x + y - 1) // y


def _packed_pred_storage_bytes(logical_elems: int, src_elem_bytes: int) -> int:
    repeat_elems = REPEAT_BYTES // src_elem_bytes
    if src_elem_bytes == 4:
        repeat_times = _ceil_div(logical_elems, repeat_elems) + 1
        return (repeat_times // 2) * 16
    return _ceil_div(logical_elems, repeat_elems) * (repeat_elems // 8)


def _prefix_mask_bytes(logical_elems: int, src_elem_bytes: int) -> np.ndarray:
    packed_bytes = _packed_pred_storage_bytes(logical_elems, src_elem_bytes)
    bits = np.zeros((ROWS * COLS,), dtype=np.uint8)
    bits[:logical_elems] = 1
    packed = np.packbits(bits, bitorder="little")
    out = np.zeros((ROWS * COLS,), dtype=np.uint8)
    out[:packed_bytes] = packed[:packed_bytes]
    return out


def generate(output_dir: Path, seed: int) -> None:
    del seed
    output_dir.mkdir(parents=True, exist_ok=True)
    np.zeros((ROWS * COLS,), dtype=np.float32).tofile(output_dir / "v1.bin")
    np.zeros((ROWS * COLS,), dtype=np.float32).tofile(output_dir / "v2.bin")
    np.zeros((ROWS * COLS,), dtype=np.uint8).tofile(output_dir / "v3.bin")
    _prefix_mask_bytes(LOGICAL_ELEMS, SRC_ELEM_BYTES).tofile(output_dir / "golden_v3.bin")


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate inputs/golden for psts-plds packed-prefix boundary.")
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    args = parser.parse_args()
    generate(args.output_dir, args.seed)


if __name__ == "__main__":
    main()
