#!/usr/bin/env python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

from importlib.util import module_from_spec, spec_from_file_location
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


REPO_ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO_ROOT / "ptodsl"))


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def resolve_ptoas_binary() -> Path:
    candidates = [
        REPO_ROOT / "build" / "tools" / "ptoas" / "ptoas",
        REPO_ROOT / "install" / "bin" / "ptoas",
    ]
    for candidate in candidates:
        if candidate.is_file():
            return candidate

    from_path = shutil.which("ptoas")
    if from_path:
        return Path(from_path)

    raise FileNotFoundError("unable to locate a ptoas binary under build/, install/, or PATH")


def run_ptoas_frontend_verify(ptoas_bin: Path, mlir_text: str, label: str) -> str:
    with tempfile.NamedTemporaryFile("w", suffix=".mlir", delete=False, encoding="utf-8") as handle:
        handle.write(mlir_text)
        input_path = Path(handle.name)

    try:
        result = subprocess.run(
            [str(ptoas_bin), str(input_path), "--emit-pto-ir", "-o", "-"],
            capture_output=True,
            text=True,
            check=False,
        )
    finally:
        input_path.unlink(missing_ok=True)

    expect(
        result.returncode == 0,
        f"{label} should pass PTOAS frontend verification.\nstdout:\n{result.stdout}\nstderr:\n{result.stderr}",
    )
    expect(result.stdout.strip(), f"{label} should emit non-empty PTO IR after PTOAS frontend passes")
    return result.stdout


def load_demo():
    demo_path = REPO_ROOT / "ptodsl" / "examples" / "flash_atten4_port.py"
    expect(demo_path.is_file(), f"missing flash_atten4 port demo: {demo_path}")

    spec = spec_from_file_location("ptodsl_flash_atten4_port_demo", demo_path)
    expect(spec is not None and spec.loader is not None, f"unable to create import spec for {demo_path}")
    module = module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> None:
    ptoas_bin = resolve_ptoas_binary()
    demo = load_demo()

    mlir_text = demo.flash_atten4_port_kernel.compile(
        S0=128,
        S1=256,
        HEAD_DIM=128,
        CUBE_S0=64,
        CUBE_S1=64,
        TILE_S1=128,
        QK_PRELOAD=4,
        CV_FIFO_SIZE=8,
        CV_FIFO_CONS_SYNC_PERIOD=4,
        FIFO_MODE=1,
        CAUSAL=False,
    ).mlir_text()

    frontend_text = run_ptoas_frontend_verify(
        ptoas_bin,
        mlir_text,
        "flash_atten4_port PTODSL artifact",
    )
    expect(
        "func.func @flash_atten4_port_kernel" in frontend_text,
        "frontend verification output should preserve the flash_atten4_port_kernel symbol",
    )
    expect(
        frontend_text.count("pto.mad") == 2,
        "frontend verification output should keep the two cube matmul stages visible",
    )
    expect(
        "pto.tcvt" in frontend_text,
        "frontend verification output should keep the P fp32->f16 conversion stage visible",
    )
    expect(
        frontend_text.count("pto.get_buf") >= 4,
        "frontend verification output should keep stage acquire lifecycle ops visible",
    )
    expect(
        frontend_text.count("pto.rls_buf") >= 4,
        "frontend verification output should keep stage release lifecycle ops visible",
    )
    expect(
        frontend_text.count("pto.tload") >= 3,
        "frontend verification output should keep required Q/O/scratch reloads visible in FIFO_MODE=1",
    )
    expect(
        frontend_text.count("memref<?xui8, #pto.address_space<gm>>, ui8") >= 4,
        "frontend verification output should keep CV communication stage marker stores visible",
    )
    expect(
        all(marker in frontend_text for marker in ("arith.constant 10 : i8", "arith.constant 20 : i8", "arith.constant 30 : i8")),
        "frontend verification output should keep prologue/steady/epilogue schedule markers visible",
    )
    expect(
        all(marker in frontend_text for marker in ("arith.constant 50 : i8", "arith.constant 51 : i8", "arith.constant 52 : i8", "arith.constant 53 : i8")),
        "frontend verification output should keep pending sync drain markers visible",
    )
    expect(
        all(marker in frontend_text for marker in ("arith.constant 54 : i8", "arith.constant 55 : i8")),
        "frontend verification output should keep pvUb drain and CV block end markers visible",
    )
    expect(
        "arith.constant 61 : i8" in frontend_text,
        "frontend verification output should keep FIFO_MODE=1 ALL_UB marker visible",
    )
    expect(
        all(marker in frontend_text for marker in ("arith.constant 81 : i8", "arith.constant 83 : i8", "arith.constant 85 : i8")),
        "frontend verification output should keep FIFO_MODE=1 UB handoff path markers visible",
    )
    expect(
        "arith.constant 70 : i8" in frontend_text,
        "frontend verification output should keep launch-core logical-block stride marker visible",
    )
    expect(
        frontend_text.count("pto.sync.set <PIPE_FIX>") >= 8,
        "frontend verification output should keep A5 FFTS producer/backpressure markers visible",
    )
    expect(
        frontend_text.count("pto.sync.wait <PIPE_FIX>") >= 10,
        "frontend verification output should keep A5 FFTS consumer/backpressure/drain markers visible",
    )

    causal_mlir_text = demo.flash_atten4_port_kernel.compile(
        S0=64,
        S1=128,
        HEAD_DIM=128,
        CUBE_S0=64,
        CUBE_S1=64,
        TILE_S1=64,
        QK_PRELOAD=4,
        CV_FIFO_SIZE=8,
        CV_FIFO_CONS_SYNC_PERIOD=4,
        FIFO_MODE=1,
        CAUSAL=True,
    ).mlir_text()
    causal_frontend_text = run_ptoas_frontend_verify(
        ptoas_bin,
        causal_mlir_text,
        "flash_atten4_port causal PTODSL artifact",
    )
    expect(
        "0xFF800000" in causal_frontend_text and "arith.cmpi sgt" in causal_frontend_text,
        "frontend verification output should preserve CAUSAL=True score masking",
    )

    for fifo_mode, marker, path_markers in (
        (0, "arith.constant 60 : i8", ("arith.constant 80 : i8", "arith.constant 82 : i8", "arith.constant 84 : i8")),
        (2, "arith.constant 62 : i8", ("arith.constant 81 : i8", "arith.constant 82 : i8", "arith.constant 85 : i8")),
    ):
        mode_mlir_text = demo.flash_atten4_port_kernel.compile(
            S0=64,
            S1=128,
            HEAD_DIM=128,
            CUBE_S0=64,
            CUBE_S1=64,
            TILE_S1=64,
            QK_PRELOAD=4,
            CV_FIFO_SIZE=8,
            CV_FIFO_CONS_SYNC_PERIOD=4,
            FIFO_MODE=fifo_mode,
            CAUSAL=False,
        ).mlir_text()
        mode_frontend_text = run_ptoas_frontend_verify(
            ptoas_bin,
            mode_mlir_text,
            f"flash_atten4_port FIFO_MODE={fifo_mode} PTODSL artifact",
        )
        expect(marker in mode_frontend_text, f"frontend output should preserve FIFO_MODE={fifo_mode} marker")
        expect(
            all(path_marker in mode_frontend_text for path_marker in path_markers),
            f"frontend output should preserve FIFO_MODE={fifo_mode} handoff path markers",
        )
        if fifo_mode == 0:
            expect(
                mode_frontend_text.count("pto.tload") >= 7,
                "FIFO_MODE=0 frontend output should keep all GM FIFO consumer reloads visible",
            )

    print("ptodsl_flash_atten4_port_frontend_verify: PASS")


if __name__ == "__main__":
    main()
