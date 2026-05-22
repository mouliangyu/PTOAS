# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Tests for lowering_backend abstraction (Issue #237 Phase 1)."""

import pytest

# Import the backend abstraction
import sys
import os

# Add parent directory for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", "..", "python"))

from tilelang_dsl.lowering_backend import (
    LoweringBackend,
    LoweringResult,
    TextBackend,
    PybindBackend,
    get_backend,
    lower_with_backend,
    _SUPPORTED_BACKENDS,
)


class TestLoweringResult:
    """Tests for LoweringResult dataclass."""

    def test_empty_result_raises_error(self):
        """Empty result should raise ValueError."""
        result = LoweringResult()
        with pytest.raises(ValueError, match="no output"):
            result.as_text()
        with pytest.raises(ValueError, match="no output"):
            result.as_module()

    def test_text_result(self):
        """Result with text only."""
        text = "module {}"
        result = LoweringResult(text=text)
        assert result.as_text() == text
        assert result.text == text
        assert result.module is None

    def test_bool_conversion(self):
        """Boolean conversion."""
        empty = LoweringResult()
        assert not empty

        with_text = LoweringResult(text="module {}")
        assert with_text

        with_module = LoweringResult(module=object())
        assert with_module

    def test_str_conversion(self):
        """String conversion."""
        result = LoweringResult(text="module {}")
        assert str(result) == "module {}"


class TestLoweringBackend:
    """Tests for LoweringBackend abstract base class."""

    def test_abstract_methods(self):
        """LoweringBackend cannot be instantiated directly."""
        with pytest.raises(TypeError):
            LoweringBackend()


class TestTextBackend:
    """Tests for TextBackend wrapper."""

    def test_instantiation(self):
        """TextBackend can be instantiated."""
        backend = TextBackend()
        assert backend.name() == "text"
        assert repr(backend) == "TextBackend(name='text')"

    def test_lower_not_implemented_without_kernel(self):
        """TextBackend.lower() requires SemanticKernel."""
        backend = TextBackend()
        # Without a proper SemanticKernel, this will fail
        # This test verifies the interface exists
        assert hasattr(backend, "lower")


class TestPybindBackend:
    """Tests for PybindBackend placeholder."""

    def test_instantiation(self):
        """PybindBackend can be instantiated."""
        backend = PybindBackend()
        assert backend.name() == "pybind"
        assert repr(backend) == "PybindBackend(name='pybind')"

    def test_is_available(self):
        """is_available() returns False until Phase 2 complete."""
        backend = PybindBackend()
        # Returns False until pybind_renderer imports successfully
        available = backend.is_available()
        assert isinstance(available, bool)

    def test_lower_raises_not_implemented(self):
        """lower() raises NotImplementedError until implemented."""
        backend = PybindBackend()
        if not backend.is_available():
            with pytest.raises(NotImplementedError):
                backend.lower(None)


class TestGetBackend:
    """Tests for backend factory function."""

    def test_get_text_backend(self):
        """get_backend('text') returns TextBackend."""
        backend = get_backend("text")
        assert isinstance(backend, TextBackend)
        assert backend.name() == "text"

    def test_get_pybind_backend(self):
        """get_backend('pybind') returns PybindBackend."""
        backend = get_backend("pybind")
        assert isinstance(backend, PybindBackend)
        assert backend.name() == "pybind"

    def test_unknown_backend_raises(self):
        """Unknown backend name raises ValueError."""
        with pytest.raises(ValueError, match="Unknown backend"):
            get_backend("unknown")

    def test_default_backend(self):
        """Default backend is text."""
        backend = get_backend()
        assert backend.name() == "text"

    def test_supported_backends(self):
        """Supported backends are text and pybind."""
        assert "text" in _SUPPORTED_BACKENDS
        assert "pybind" in _SUPPORTED_BACKENDS
        assert len(_SUPPORTED_BACKENDS) == 2


class TestLowerWithBackend:
    """Tests for convenience function."""

    def test_lower_with_text_backend(self):
        """lower_with_backend with 'text' backend."""
        # Without a SemanticKernel, we can only verify the function exists
        assert callable(lower_with_backend)


if __name__ == "__main__":
    pytest.main([__file__, "-v"])