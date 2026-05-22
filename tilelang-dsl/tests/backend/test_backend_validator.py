# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You can not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Tests for backend_validator (Issue #237 Phase 4)."""

import pytest

import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "python"))

from tilelang_dsl.backend_validator import (
    VerifyResult,
    BackendComparisonResult,
    compare_backends,
    run_backend_validation_suite,
    print_comparison_summary,
    _normalize_mlir_text,
    _find_text_differences,
)


class TestVerifyResult:
    """Tests for VerifyResult dataclass."""

    def test_passed_result(self):
        """Passed result."""
        result = VerifyResult(passed=True)
        assert result.passed
        assert result.error is None

    def test_failed_result(self):
        """Failed result with error."""
        result = VerifyResult(passed=False, error="verification failed")
        assert not result.passed
        assert result.error == "verification failed"


class TestBackendComparisonResult:
    """Tests for BackendComparisonResult dataclass."""

    def test_ok_property(self):
        """ok property checks alignment status."""
        result = BackendComparisonResult(
            kernel_name="test_kernel",
            text_output="module {}",
            pybind_output="module {}",
            text_normalized="module {}",
            pybind_normalized="module {}",
            matches=True,
            normalized_matches=True,
            differences=(),
            text_verify_passed=True,
            pybind_verify_passed=True,
            text_verify_error=None,
            pybind_verify_error=None,
            pybind_available=True,
        )
        assert result.ok

    def test_has_differences_property(self):
        """has_differences property."""
        result_no_diff = BackendComparisonResult(
            kernel_name="test",
            text_output="",
            pybind_output="",
            text_normalized="",
            pybind_normalized="",
            matches=True,
            normalized_matches=True,
            differences=(),
            text_verify_passed=True,
            pybind_verify_passed=True,
            text_verify_error=None,
            pybind_verify_error=None,
            pybind_available=True,
        )
        assert not result_no_diff.has_differences

        result_with_diff = BackendComparisonResult(
            kernel_name="test",
            text_output="",
            pybind_output="",
            text_normalized="",
            pybind_normalized="",
            matches=False,
            normalized_matches=False,
            differences=("--- text_backend", "+++ pybind_backend"),
            text_verify_passed=True,
            pybind_verify_passed=True,
            text_verify_error=None,
            pybind_verify_error=None,
            pybind_available=True,
        )
        assert result_with_diff.has_differences


class TestNormalizeMlirText:
    """Tests for MLIR text normalization."""

    def test_empty_text(self):
        """Empty text returns empty."""
        assert _normalize_mlir_text("") == ""

    def test_skip_comments(self):
        """Comments are skipped."""
        text = "// comment\nmodule {}"
        normalized = _normalize_mlir_text(text, skip_comments=True)
        assert "comment" not in normalized
        assert "module {}" in normalized

    def test_skip_blank_lines(self):
        """Blank lines are skipped."""
        text = "module {\n\n\n}"
        normalized = _normalize_mlir_text(text, skip_blank_lines=True)
        assert "\n\n" not in normalized

    def test_normalize_ssa_names(self):
        """SSA names are normalized."""
        text = "%0 = arith.constant 0 : i32"
        normalized = _normalize_mlir_text(text, normalize_ssa_names=True)
        assert "%ssa" in normalized
        assert "%0" not in normalized

    def test_combined_normalization(self):
        """Combined normalization options."""
        text = """
// Kernel: test
module {
  %0 = arith.constant 0 : i32
  %1 = arith.constant 1 : i32
}
"""
        normalized = _normalize_mlir_text(
            text,
            skip_comments=True,
            skip_blank_lines=True,
            normalize_ssa_names=True,
        )
        assert "// Kernel" not in normalized
        assert "%ssa" in normalized


class TestFindTextDifferences:
    """Tests for difference finding."""

    def test_identical_texts(self):
        """Identical texts have no differences."""
        text = "module {}"
        diffs = _find_text_differences(text, text)
        assert len(diffs) == 0

    def test_different_texts(self):
        """Different texts have differences."""
        text_a = "module { %0 = arith.constant 0 : i32 }"
        text_b = "module { %0 = arith.constant 1 : i32 }"
        diffs = _find_text_differences(text_a, text_b)
        assert len(diffs) > 0

    def test_empty_texts(self):
        """Empty texts return appropriate messages."""
        diffs_a_empty = _find_text_differences("", "module {}")
        assert "empty" in diffs_a_empty[0].lower() or "Text backend" in diffs_a_empty[0]

        diffs_b_empty = _find_text_differences("module {}", "")
        assert "empty" in diffs_b_empty[0].lower() or "Pybind backend" in diffs_b_empty[0]


class TestCompareBackends:
    """Tests for compare_backends function."""

    def test_returns_comparison_result(self):
        """compare_backends returns BackendComparisonResult."""
        # Create a mock SemanticKernel for testing
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

        # compare_backends should work with this mock
        result = compare_backends(mock_kernel)

        assert isinstance(result, BackendComparisonResult)
        assert result.kernel_name == "mock_kernel"
        assert isinstance(result.text_output, str)
        assert isinstance(result.matches, bool)
        assert isinstance(result.pybind_available, bool)


class TestRunBackendValidationSuite:
    """Tests for validation suite runner."""

    def test_empty_suite(self):
        """Empty suite returns empty results."""
        results = run_backend_validation_suite([])
        assert len(results) == 0

    def test_print_summary(self):
        """print_comparison_summary works."""
        print_comparison_summary(())


if __name__ == "__main__":
    pytest.main([__file__, "-v"])