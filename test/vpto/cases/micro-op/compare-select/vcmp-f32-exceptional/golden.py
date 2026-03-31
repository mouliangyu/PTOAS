#!/usr/bin/env python3
# coding=utf-8

import argparse
from pathlib import Path

import numpy as np


ROWS = 32
COLS = 32
SEED = 19
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


def _pack_predicate_mask(mask: np.ndarray, src_elem_bytes: int) -> np.ndarray:
    logical_elems = ROWS * COLS
    stored_bytes = _packed_pred_storage_bytes(logical_elems, src_elem_bytes)
    packed_bits = np.packbits(mask.reshape(-1).astype(np.uint8, copy=False), bitorder="little")
    out = np.zeros((logical_elems,), dtype=np.uint8)
    out[: packed_bits.size] = packed_bits
    out[stored_bytes:] = 0
    return out


def generate(output_dir: Path, seed: int, src_elem_bytes: int) -> None:
    del seed
    specials = np.array(
        [-np.inf, -3.0, -0.0, 0.0, 0.5, np.inf, np.nan, 1.0],
        dtype=np.float32,
    )
    v1 = np.resize(specials, ROWS * COLS).reshape(ROWS, COLS).astype(np.float32)
    v2 = np.resize(specials[::-1], ROWS * COLS).reshape(ROWS, COLS).astype(np.float32)
    mask = np.less(v1, v2)

    output_dir.mkdir(parents=True, exist_ok=True)
    v1.reshape(-1).tofile(output_dir / "v1.bin")
    v2.reshape(-1).tofile(output_dir / "v2.bin")
    np.zeros((ROWS * COLS,), dtype=np.uint8).tofile(output_dir / "v3.bin")
    _pack_predicate_mask(mask, src_elem_bytes).tofile(output_dir / "golden_v3.bin")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate numpy-based inputs/golden for VPTO micro-op vcmp-f32-exceptional validation."
    )
    parser.add_argument("--output-dir", type=Path, default=Path("."))
    parser.add_argument("--seed", type=int, default=SEED)
    parser.add_argument("--src-elem-bytes", type=int, default=SRC_ELEM_BYTES)
    args = parser.parse_args()
    generate(args.output_dir, args.seed, args.src_elem_bytes)


if __name__ == "__main__":
    main()
