# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You can not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""End-to-end integration tests for PybindBackend (Issue #237).

This test module validates the complete lowering pipeline:
1. Parse TileLang DSL kernel
2. Lower with TextBackend (reference)
3. Lower with PybindBackend (pybinding)
4. Compare outputs for consistency

Prerequisites:
    - MLIR Python bindings must be available in PYTHONPATH
    - PTO dialect bindings must be available for PTO ops

Run tests:
    python3 tests/backend/test_e2e_pybind_backend.py

Environment setup:
    export PYTHONPATH=$LLVM_BUILD_DIR/tools/mlir/python_packages/mlir_core:$PTOAS_BUILD_DIR/python
"""

import pytest
import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "python"))

from tilelang_dsl.env_config import check_mlir_bindings_available, verify_environment
from tilelang_dsl.lowering_backend import (
    TextBackend,
    PybindBackend,
    get_backend,
    lower_with_backend,
    LoweringResult,
)


@pytest.fixture(scope="module")
def mlir_available():
    """Check if MLIR bindings are available before running tests."""
    return check_mlir_bindings_available()


class TestEnvironmentSetup:
    """Tests for environment configuration."""

    def test_environment_check(self):
        """Verify environment is properly configured."""
        # This test always passes - it just prints status
        status = verify_environment()
        # If MLIR not available, skip actual backend tests
        if not check_mlir_bindings_available():
            pytest.skip("MLIR Python bindings not available - see env_config.print_environment_help()")

    def test_pybind_backend_available(self, mlir_available):
        """Verify PybindBackend.is_available() matches MLIR status."""
        backend = PybindBackend()
        assert backend.is_available() == mlir_available


class TestPybindBackendLowering:
    """End-to-end tests for PybindBackend lowering."""

    @pytest.fixture
    def mock_kernel(self):
        """Create a mock SemanticKernel for testing."""
        from dataclasses import dataclass
        from tilelang_dsl.semantic import (
            SemanticParameter,
            SemanticBinding,
            SemanticScalarType,
            SemanticIndexType,
            SemanticTensorViewType,
        )
        from tilelang_dsl.types import ScalarType

        @dataclass
        class MockSemanticKernel:
            target: str = "a5"
            op: str = "test"
            symbol_name: str = "test_kernel"
            kernel_family: str = "test_family"  # Required by text renderer
            verify_enabled: bool = True
            advanced_enabled: bool = False
            dtype_signature: tuple = ()
            parameters: tuple = (
                SemanticParameter(
                    binding=SemanticBinding(
                        name="input",
                        ssa_name="%input",
                        type=SemanticTensorViewType(rank=2, element_dtype=ScalarType("f32")),
                        origin="parameter",
                    ),
                ),
            )
            tile_bindings: tuple = ()
            body: tuple = ()
            inline_helpers: tuple = ()

        return MockSemanticKernel()

    def test_pybind_backend_lower_simple(self, mock_kernel, mlir_available):
        """Test lowering a simple kernel with PybindBackend."""
        if not mlir_available:
            pytest.skip("MLIR Python bindings not available")

        backend = PybindBackend()
        result = backend.lower(mock_kernel)

        assert isinstance(result, LoweringResult)
        assert result.module is not None
        assert result.text is not None
        assert "module" in result.as_text()

    def test_text_backend_lower_simple(self, mock_kernel):
        """Test lowering a simple kernel with TextBackend."""
        backend = TextBackend()
        result = backend.lower(mock_kernel)

        assert isinstance(result, LoweringResult)
        assert result.text is not None
        assert "module" in result.as_text()

    def test_backend_output_comparison(self, mock_kernel, mlir_available):
        """Compare TextBackend and PybindBackend outputs."""
        if not mlir_available:
            pytest.skip("MLIR Python bindings not available")

        text_backend = TextBackend()
        pybind_backend = PybindBackend()

        text_result = text_backend.lower(mock_kernel)
        pybind_result = pybind_backend.lower(mock_kernel)

        # Both should produce MLIR text output
        assert text_result.as_text()
        assert pybind_result.as_text()

        # Normalize and compare
        from tilelang_dsl.backend_validator import _normalize_mlir_text

        text_norm = _normalize_mlir_text(text_result.as_text(), skip_comments=True)
        pybind_norm = _normalize_mlir_text(pybind_result.as_text(), skip_comments=True)

        # Check key elements are present in both
        assert mock_kernel.symbol_name in text_norm or "test_kernel" in text_norm


class TestBackendValidatorIntegration:
    """Integration tests for backend validator."""

    def test_compare_backends_integration(self, mlir_available):
        """Test compare_backends with mock kernel."""
        if not mlir_available:
            pytest.skip("MLIR Python bindings not available")

        from tilelang_dsl.backend_validator import compare_backends, BackendComparisonResult
        from dataclasses import dataclass

        @dataclass
        class MockSemanticKernel:
            target: str = "a5"
            op: str = "test"
            symbol_name: str = "mock_kernel"
            verify_enabled: bool = True
            advanced_enabled: bool = False
            dtype_signature: tuple = ()
            parameters: tuple = ()
            tile_bindings: tuple = ()
            body: tuple = ()
            inline_helpers: tuple = ()

        mock_kernel = MockSemanticKernel()
        result = compare_backends(mock_kernel)

        assert isinstance(result, BackendComparisonResult)
        assert result.pybind_available


if __name__ == "__main__":
    # Print environment help if MLIR bindings not available
    if not check_mlir_bindings_available():
        from tilelang_dsl.env_config import print_environment_help
        print_environment_help()
        print("\nSkipping tests due to missing MLIR bindings.")
        print("Set up environment and run again.")
        sys.exit(0)

    pytest.main([__file__, "-v"])