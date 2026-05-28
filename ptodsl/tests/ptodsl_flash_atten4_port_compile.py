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


def load_demo():
    demo_path = REPO_ROOT / "ptodsl" / "examples" / "flash_atten4_port.py"
    expect(demo_path.is_file(), f"missing flash_atten4 port demo: {demo_path}")

    spec = spec_from_file_location("ptodsl_flash_atten4_port_demo", demo_path)
    expect(spec is not None and spec.loader is not None, f"unable to create import spec for {demo_path}")
    module = module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> None:
    demo = load_demo()

    expect(
        hasattr(demo, "emit_flash_atten4_port_mlir"),
        "flash_atten4 port demo should export emit_flash_atten4_port_mlir(...)",
    )
    expect(
        hasattr(demo, "flash_atten4_port_kernel"),
        "flash_atten4 port demo should export flash_atten4_port_kernel",
    )
    expect(
        hasattr(demo, "build_arg_parser"),
        "flash_atten4 port demo should expose a CLI argument parser",
    )

    wrapper_text = demo.emit_flash_atten4_port_mlir(
        s0=128,
        s1=1024,
        head_dim=128,
        cube_s0=128,
        cube_s1=128,
        tile_s1=256,
        qk_preload=4,
        cv_fifo_size=8,
        cv_fifo_cons_sync_period=4,
        fifo_mode=1,
        causal=False,
    )
    expect_parse_roundtrip_and_verify(wrapper_text, "flash_atten4 port wrapper-emitted MLIR")
    expect("func.func @flash_atten4_port_kernel" in wrapper_text, "wrapper compile should emit the port kernel entry")
    expect('pto.mode = "explicit"' in wrapper_text, "wrapper compile should carry explicit mode metadata")
    expect("pto.tcvt" in wrapper_text, "wrapper compile should keep the P fp32->f16 conversion stage")
    expect(wrapper_text.count("pto.mad") == 2, "wrapper compile should keep the two cube matmul stages")
    expect(wrapper_text.count("pto.sync.set <PIPE_FIX>") >= 8, "wrapper compile should expose A5 FFTS producer/backpressure markers")
    expect(wrapper_text.count("pto.sync.wait <PIPE_FIX>") >= 10, "wrapper compile should expose A5 FFTS consumer/backpressure/drain markers")
    expect("!pto.tensor_view<2048x128xf32>" in wrapper_text, "wrapper compile should size QK FIFO as CV_FIFO_SIZE*TILE_FACTOR*CUBE_S0")
    expect("!pto.tensor_view<2048x128xf16>" in wrapper_text, "wrapper compile should size P FIFO as CV_FIFO_SIZE*TILE_FACTOR*CUBE_S0")
    expect(wrapper_text.count("pto.tload") >= 7, "wrapper compile should reload QK/P/PV/PV_pend/exp_max/O_parts handoff data from scratch views")
    expect(wrapper_text.count("!pto.ptr<ui8, gm>, ui8") >= 4, "wrapper compile should write CV communication stage markers")
    expect("arith.constant 10 : i8" in wrapper_text, "wrapper compile should mark the QK_PRELOAD prologue phase")
    expect("arith.constant 20 : i8" in wrapper_text, "wrapper compile should mark the steady phase")
    expect("arith.constant 30 : i8" in wrapper_text, "wrapper compile should mark the epilogue phase")
    expect("arith.constant 50 : i8" in wrapper_text, "wrapper compile should mark qk2sm drain")
    expect("arith.constant 51 : i8" in wrapper_text, "wrapper compile should mark sm2pv drain")
    expect("arith.constant 52 : i8" in wrapper_text, "wrapper compile should mark pv2gu drain")
    expect("arith.constant 53 : i8" in wrapper_text, "wrapper compile should mark ubBuf drain")
    expect("arith.constant 54 : i8" in wrapper_text, "wrapper compile should mark pvUbBuf drain")
    expect("arith.constant 55 : i8" in wrapper_text, "wrapper compile should mark CV block end")
    expect("arith.constant 61 : i8" in wrapper_text, "wrapper compile should mark FIFO_MODE=1 ALL_UB")
    expect("arith.constant 70 : i8" in wrapper_text, "wrapper compile should mark launch-core logical-block striding")
    expect(wrapper_text.count("scf.for") >= 2, "wrapper compile should keep the tile loop and subtile loop structure")
    expect("!pto.tile_buf<mat, 32x128xf16" in wrapper_text, "wrapper compile should materialize the A5 row-sliced Q MAT tile")
    expect("!pto.tile_buf<vec, 32x128xf32" in wrapper_text, "wrapper compile should materialize the A5 row-sliced score/prob subtile")
    expect("!pto.tile_buf<vec, 32x128xf32" in wrapper_text, "wrapper compile should materialize the A5 row-sliced running O subtile")

    with tempfile.TemporaryDirectory() as tmpdir:
        cli_output = Path(tmpdir) / "fa4_cli.mlir"
        demo.main([
            "--s0", "128",
            "--s1", "1024",
            "--head-dim", "128",
            "--cube-s0", "128",
            "--cube-s1", "128",
            "--tile-s1", "256",
            "--qk-preload", "4",
            "--cv-fifo-size", "8",
            "--cv-fifo-cons-sync-period", "4",
            "--fifo-mode", "1",
            "-o", str(cli_output),
        ])
        cli_text = cli_output.read_text(encoding="utf-8")
        expect_parse_roundtrip_and_verify(cli_text, "flash_atten4 CLI-emitted MLIR")
        expect("func.func @flash_atten4_port_kernel" in cli_text, "CLI output should contain the kernel entry")

    compiled = demo.flash_atten4_port_kernel.compile(
        S0=64,
        S1=128,
        HEAD_DIM=128,
        CUBE_S0=32,
        CUBE_S1=64,
        TILE_S1=64,
        QK_PRELOAD=4,
        CV_FIFO_SIZE=8,
        CV_FIFO_CONS_SYNC_PERIOD=4,
        FIFO_MODE=1,
        CAUSAL=True,
    )
    compiled.verify()

    expect(
        compiled.constexpr_bindings == {
            "S0": 64,
            "S1": 128,
            "HEAD_DIM": 128,
            "CUBE_S0": 32,
            "CUBE_S1": 64,
            "TILE_S1": 64,
            "QK_PRELOAD": 4,
            "CV_FIFO_SIZE": 8,
            "CV_FIFO_CONS_SYNC_PERIOD": 4,
            "FIFO_MODE": 1,
            "CAUSAL": True,
        },
        f"unexpected constexpr bindings: {compiled.constexpr_bindings!r}",
    )

    specialized_text = compiled.mlir_text()
    expect_parse_roundtrip_and_verify(specialized_text, "flash_atten4 port specialized MLIR")
    expect(specialized_text.count("pto.mad") == 2, "specialized compile should still keep the two cube matmul stages")
    expect(specialized_text.count("pto.sync.set <PIPE_FIX>") >= 8, "specialized compile should still expose FFTS producer/backpressure markers")
    expect(specialized_text.count("pto.sync.wait <PIPE_FIX>") >= 10, "specialized compile should still expose FFTS consumer/backpressure/drain markers")
    expect("0xFF800000" in specialized_text, "CAUSAL=True specialization should materialize -inf score masking")
    expect("arith.cmpi sgt" in specialized_text, "CAUSAL=True specialization should compare kv column against q row")
    expect("pto.tcvt" in specialized_text, "specialized compile should still keep the P fp32->f16 conversion stage")
    expect("!pto.tile_buf<mat, 16x64xf16" in specialized_text, "TILE_S1=CUBE_S1 specialization should shrink the row-sliced subtile MAT shape")

    for fifo_mode, marker in ((0, "arith.constant 60 : i8"), (2, "arith.constant 62 : i8")):
        mode_text = demo.flash_atten4_port_kernel.compile(
            S0=64,
            S1=128,
            HEAD_DIM=128,
            CUBE_S0=32,
            CUBE_S1=64,
            TILE_S1=64,
            QK_PRELOAD=4,
            CV_FIFO_SIZE=8,
            CV_FIFO_CONS_SYNC_PERIOD=4,
            FIFO_MODE=fifo_mode,
            CAUSAL=False,
        ).mlir_text()
        expect_parse_roundtrip_and_verify(mode_text, f"flash_atten4 FIFO_MODE={fifo_mode} MLIR")
        expect(marker in mode_text, f"FIFO_MODE={fifo_mode} compile should materialize marker {marker}")

    launch_cap_text = demo.flash_atten4_port_kernel.compile(
        S0=4096,
        S1=1024,
        HEAD_DIM=128,
        CUBE_S0=128,
        CUBE_S1=128,
        TILE_S1=256,
        QK_PRELOAD=4,
        CV_FIFO_SIZE=8,
        CV_FIFO_CONS_SYNC_PERIOD=4,
        FIFO_MODE=1,
        CAUSAL=False,
    ).mlir_text()
    expect_parse_roundtrip_and_verify(launch_cap_text, "flash_atten4 launch cap MLIR")
    expect("arith.constant 28 : index" in launch_cap_text, "launch cap compile should materialize kFaLaunchCoreCount=28")
    expect("arith.constant 70 : i8" in launch_cap_text, "launch cap compile should keep logical-block stride marker")

    cached = demo.flash_atten4_port_kernel.cached_specializations()
    expect(len(cached) >= 2, "wrapper compile plus direct compile should populate cached specializations")
    print("ptodsl_flash_atten4_port_compile: PASS")


if __name__ == "__main__":
    main()
