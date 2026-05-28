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
import sys
import tempfile

REPO_ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO_ROOT / "ptodsl"))

from ptodsl._bootstrap import make_context
from mlir.ir import Module


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def expect_parse_roundtrip_and_verify(text: str, label: str) -> None:
    with make_context() as ctx:
        parsed = Module.parse(text, ctx)
        parsed.operation.verify()
        roundtrip_text = str(parsed)
    expect(
        roundtrip_text == text,
        f"{label} should survive Module.parse(...) round-trip without textual drift",
    )


def load_flash_attention_demo():
    demo_candidates = [
        REPO_ROOT / "ptodsl" / "examples" / "hw_native_flash_attention.py",
        REPO_ROOT / "ptodsl" / "demos" / "hw_native_flash_attention.py",
    ]
    for demo_path in demo_candidates:
        if demo_path.is_file():
            break
    else:
        raise AssertionError(
            "canonical hw-native flash attention demo is missing: "
            + ", ".join(str(path) for path in demo_candidates)
        )

    spec = spec_from_file_location("ptodsl_flash_attention_demo", demo_path)
    expect(spec is not None and spec.loader is not None, f"unable to create import spec for {demo_path}")
    module = module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> None:
    demo = load_flash_attention_demo()

    expect(hasattr(demo, "emit_flash_attention_mlir"), "flash attention demo should export emit_flash_attention_mlir(...)")
    expect(hasattr(demo, "flash_attention_kernel"), "flash attention demo should export flash_attention_kernel")
    expect(hasattr(demo, "build_arg_parser"), "flash attention demo should expose a CLI argument parser")
    expect(
        hasattr(demo, "describe_hw_native_flash_attention_port"),
        "flash attention demo should describe the hw-native source port",
    )
    port_info = demo.describe_hw_native_flash_attention_port()
    expect(
        "hw-native-sys/pto-isa" in port_info["source"],
        f"unexpected source mapping: {port_info!r}",
    )
    expect(
        port_info["dataflow"] == ["compute_qk", "compute_p", "compute_pv", "compute_gu"],
        f"unexpected FA dataflow mapping: {port_info!r}",
    )
    expect(
        port_info["ported_file"] == "kernels/python/flash_atten/kernels/fa_builder.py",
        f"unexpected source file mapping: {port_info!r}",
    )
    for feature in [
        "QK_PRELOAD prologue/steady/epilogue shadow schedule",
        "Vec_S0 row-slice state arrays",
        "exp_max_ring per preload slot and row-slice",
        "wide P slot producer/consumer",
        "block_idx/block_num Q block distribution",
    ]:
        expect(feature in port_info["frontend_features"], f"missing frontend feature marker: {feature}")

    wrapper_text = demo.emit_flash_attention_mlir(
        head_dim=128,
        causal=False,
        s1_tile=256,
        qk_preload=3,
    )
    expect_parse_roundtrip_and_verify(wrapper_text, "flash attention wrapper-emitted MLIR")
    expect("func.func @flash_attention_kernel" in wrapper_text, "wrapper compile should emit the flash_attention_kernel entry")
    expect('pto.mode = "explicit"' in wrapper_text, "flash attention wrapper compile should carry explicit mode metadata")
    expect("pto.barrier <PIPE_ALL>" in wrapper_text, "demo phase boundaries should lower to pipe_barrier(Pipe.ALL)")
    expect(wrapper_text.count("pto.mad") >= 2, "wrapper compile should keep the QK and PV cube matmul stages")
    expect("pto.mad_acc" in wrapper_text, "wrapper compile should keep the PV accumulation stage")
    expect("pto.trowmax" in wrapper_text, "wrapper compile should keep tile row-max softmax")
    expect("pto.texp" in wrapper_text, "wrapper compile should keep tile exp softmax")
    expect("pto.trowexpandmul" in wrapper_text, "wrapper compile should keep GU rescale")
    expect("pto.trowexpanddiv" in wrapper_text, "wrapper compile should keep final normalization")
    expect("pto.make_tensor_view" in wrapper_text, "QK/P/PV GM slots should be explicit tensor views")
    expect("%c256" in wrapper_text, "S1_TILE=256 should appear in slot tensor view shape operands")
    expect("32x256xf32" in wrapper_text, "S1_TILE=256 should use Vec_S0=32 row-slice tiles")

    with tempfile.TemporaryDirectory() as tmpdir:
        cli_output = Path(tmpdir) / "flash_attention_hw_native_port.mlir"
        demo.main([
            "--head-dim", "128",
            "--s1-tile", "512",
            "--qk-preload", "4",
            "-o", str(cli_output),
        ])
        cli_text = cli_output.read_text(encoding="utf-8")
        expect_parse_roundtrip_and_verify(cli_text, "flash attention CLI-emitted MLIR")
        expect("func.func @flash_attention_kernel" in cli_text, "CLI output should contain the kernel entry")
        expect("%c512" in cli_text, "CLI S1_TILE=512 should specialize QK/P slot shape operands")
        expect("16x512xf32" in cli_text, "CLI S1_TILE=512 should use Vec_S0=16 row-slice tiles")

    compiled = demo.flash_attention_kernel.compile(
        HEAD_DIM=128,
        S1_TILE=256,
        QK_PRELOAD=3,
        CAUSAL=False,
        Q_ROWS=128,
    )
    compiled.verify()

    expect(
        compiled.constexpr_bindings == {
            "HEAD_DIM": 128,
            "S1_TILE": 256,
            "QK_PRELOAD": 3,
            "CAUSAL": False,
            "Q_ROWS": 128,
        },
        f"unexpected constexpr bindings: {compiled.constexpr_bindings!r}",
    )

    specialized_text = compiled.mlir_text()
    expect_parse_roundtrip_and_verify(specialized_text, "flash attention specialized MLIR")
    expect("func.func @flash_attention_kernel" in specialized_text, "direct compile should emit the flash_attention_kernel entry")
    expect('pto.mode = "explicit"' in specialized_text, "direct compile should carry explicit mode metadata")
    expect("!pto.tile_buf<mat, 128x128xf16" in specialized_text, "direct compile should keep source Q MAT tile shape")
    expect("pto.trowmax" in specialized_text, "direct compile should keep the softmax stage")

    cached = demo.flash_attention_kernel.cached_specializations()
    expect(len(cached) >= 2, "wrapper compile plus explicit compile should populate at least two cached specializations")
    print("ptodsl_flash_attention_demo_compile: PASS")


if __name__ == "__main__":
    main()
