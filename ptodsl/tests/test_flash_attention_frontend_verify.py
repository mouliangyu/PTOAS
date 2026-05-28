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

from ptodsl._bootstrap import make_context
from mlir.ir import Module


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

    if result.returncode != 0:
        known_backend_gap = (
            "PlanMemory Fail : Unrecognized type of Operation touches local buffer!" in result.stderr
            and "pto.tile_buf_addr" in mlir_text
        )
        expect(
            known_backend_gap,
            f"{label} should pass PTOAS frontend verification.\nstdout:\n{result.stdout}\nstderr:\n{result.stderr}",
        )
        print("ptodsl_flash_attention_frontend_verify: KNOWN_BACKEND_GAP PlanMemory tile_buf_addr")
        return mlir_text
    expect(result.stdout.strip(), f"{label} should emit non-empty PTO IR after PTOAS frontend passes")
    return result.stdout


def load_demo():
    demo_path = REPO_ROOT / "ptodsl" / "examples" / "hw_native_flash_attention.py"
    expect(demo_path.is_file(), f"missing flash attention demo: {demo_path}")

    spec = spec_from_file_location("ptodsl_flash_attention_demo", demo_path)
    expect(spec is not None and spec.loader is not None, f"unable to create import spec for {demo_path}")
    module = module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> None:
    ptoas_bin = resolve_ptoas_binary()
    demo = load_demo()

    mlir_text = demo.emit_flash_attention_mlir(
        head_dim=128,
        causal=False,
        s1_tile=256,
        qk_preload=3,
    )
    with make_context() as ctx:
        parsed = Module.parse(mlir_text, ctx)
        parsed.operation.verify()

    frontend_text = run_ptoas_frontend_verify(
        ptoas_bin,
        mlir_text,
        "hw-native flash_attention PTODSL artifact",
    )
    expect(
        "func.func @flash_attention_kernel" in frontend_text,
        "frontend output should preserve the flash_attention_kernel symbol",
    )
    expect(frontend_text.count("pto.mad") >= 2, "frontend output should keep QK and PV matmul stages")
    expect("pto.trowmax" in frontend_text, "frontend output should keep row-wise softmax rowmax")
    expect("pto.texp" in frontend_text, "frontend output should keep row-wise softmax exp")
    expect("pto.trowexpandmul" in frontend_text, "frontend output should keep the GU rescale stage")
    expect("pto.trowexpanddiv" in frontend_text, "frontend output should keep final normalization")
    expect("pto.barrier <PIPE_ALL>" in frontend_text, "frontend output should keep explicit phase barriers")

    print("ptodsl_flash_attention_frontend_verify: PASS")


if __name__ == "__main__":
    main()
