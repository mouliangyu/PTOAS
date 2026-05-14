# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

"""Unit tests for stable_key.py"""

import pytest
from tilelang_dsl.stable_key import (
    StableKey,
    OperandStableKey,
    compute_stable_key,
    normalize_dtype,
    normalize_blayout,
    normalize_slayout,
    normalize_memory_space,
)


class TestNormalizeDtype:
    """Tests for normalize_dtype function."""
    
    def test_string_dtype(self):
        assert normalize_dtype("f32") == "f32"
        assert normalize_dtype("F32") == "f32"
        assert normalize_dtype("f16") == "f16"
        assert normalize_dtype("bf16") == "bf16"
        assert normalize_dtype("i8") == "i8"
    
    def test_scalar_type_object(self):
        # Mock ScalarType
        class MockScalarType:
            name = "f32"
        
        assert normalize_dtype(MockScalarType()) == "f32"


class TestNormalizeBlayout:
    """Tests for normalize_blayout function."""
    
    def test_string_blayout(self):
        assert normalize_blayout("row_major") == "row_major"
        assert normalize_blayout("ROW_MAJOR") == "row_major"
        assert normalize_blayout("col_major") == "col_major"
    
    def test_int_blayout(self):
        assert normalize_blayout(0) == "row_major"
        assert normalize_blayout(1) == "col_major"
    
    def test_none_blayout(self):
        assert normalize_blayout(None) == "row_major"


class TestNormalizeSlayout:
    """Tests for normalize_slayout function."""
    
    def test_string_slayout(self):
        assert normalize_slayout("none_box") == "none_box"
        assert normalize_slayout("row_major") == "row_major"
        assert normalize_slayout("col_major") == "col_major"
    
    def test_int_slayout(self):
        assert normalize_slayout(0) == "none_box"
        assert normalize_slayout(1) == "row_major"
        assert normalize_slayout(2) == "col_major"
    
    def test_none_slayout(self):
        assert normalize_slayout(None) == "none_box"


class TestNormalizeMemorySpace:
    """Tests for normalize_memory_space function."""
    
    def test_string_memory_space(self):
        assert normalize_memory_space("ub") == "ub"
        assert normalize_memory_space("UB") == "ub"
        assert normalize_memory_space("gm") == "gm"
    
    def test_none_memory_space(self):
        assert normalize_memory_space(None) == "ub"


class TestOperandStableKey:
    """Tests for OperandStableKey class."""
    
    def test_tile_operand_to_hash_input(self):
        key = OperandStableKey(
            kind="tile",
            dtype="f32",
            shape=(16, 64),
            valid_shape=(16, 64),
            memory_space="ub",
            blayout="row_major",
            slayout="none_box",
            fractal=512,
            pad=0,
        )
        
        hash_input = key.to_hash_input()
        assert hash_input.startswith("tile:f32:")
        assert "16,64" in hash_input
        assert "ub" in hash_input
        assert "row_major" in hash_input
        assert "none_box" in hash_input
        assert "512" in hash_input
    
    def test_view_operand_to_hash_input(self):
        # ENHANCED: shape/strides/ms all enter key
        key = OperandStableKey(
            kind="view",
            dtype="f32",
            view_shape=(16, 64),
            view_strides=(64, 1),
            view_memory_space="gm",
        )
        
        hash_input = key.to_hash_input()
        assert hash_input.startswith("view:f32:")
        assert "16,64" in hash_input
        assert "64,1" in hash_input
        assert "gm" in hash_input
    
    def test_scalar_operand_to_hash_input(self):
        key = OperandStableKey(
            kind="scalar",
            dtype="f32",
        )
        
        hash_input = key.to_hash_input()
        assert hash_input == "scalar:f32"
    
    def test_tile_with_dynamic_valid_shape(self):
        key = OperandStableKey(
            kind="tile",
            dtype="f32",
            shape=(16, 64),
            valid_shape=(None, 64),  # dynamic first dim
            memory_space="ub",
            blayout="row_major",
            slayout="none_box",
            fractal=512,
            pad=0,
        )
        
        hash_input = key.to_hash_input()
        assert "?" in hash_input  # dynamic dim represented as ?


class TestStableKey:
    """Tests for StableKey class."""
    
    def test_stable_key_to_hash(self):
        key1 = StableKey(
            target="a5",
            op="tadd",
            operand_keys=(
                OperandStableKey(kind="tile", dtype="f32", shape=(16, 64)),
            ),
            context_key=frozenset(),
        )
        
        hash1 = key1.to_hash()
        assert len(hash1) == 16  # 16 hex chars
        assert hash1.isalnum()  # hex string
    
    def test_same_parameters_same_hash(self):
        """Test that same parameters produce same hash."""
        key1 = StableKey(
            target="a5",
            op="tadd",
            operand_keys=(
                OperandStableKey(
                    kind="tile",
                    dtype="f32",
                    shape=(16, 64),
                    valid_shape=(16, 64),
                    memory_space="ub",
                    blayout="row_major",
                    slayout="none_box",
                    fractal=512,
                    pad=0,
                ),
            ),
            context_key=frozenset(),
        )
        
        key2 = StableKey(
            target="a5",
            op="tadd",
            operand_keys=(
                OperandStableKey(
                    kind="tile",
                    dtype="f32",
                    shape=(16, 64),
                    valid_shape=(16, 64),
                    memory_space="ub",
                    blayout="row_major",
                    slayout="none_box",
                    fractal=512,
                    pad=0,
                ),
            ),
            context_key=frozenset(),
        )
        
        assert key1.to_hash() == key2.to_hash()
    
    def test_different_parameters_different_hash(self):
        """Test that different parameters produce different hash."""
        key1 = StableKey(
            target="a5",
            op="tadd",
            operand_keys=(
                OperandStableKey(kind="tile", dtype="f32", shape=(16, 64)),
            ),
            context_key=frozenset(),
        )
        
        key2 = StableKey(
            target="a5",
            op="tadd",
            operand_keys=(
                OperandStableKey(kind="tile", dtype="f16", shape=(16, 64)),
            ),
            context_key=frozenset(),
        )
        
        assert key1.to_hash() != key2.to_hash()
    
    def test_context_attrs_in_hash(self):
        """Test that context_attrs affect hash."""
        key1 = StableKey(
            target="a5",
            op="tadd",
            operand_keys=(
                OperandStableKey(kind="scalar", dtype="f32"),
            ),
            context_key=frozenset(("round_mode", "RINT"),),
        )
        
        key2 = StableKey(
            target="a5",
            op="tadd",
            operand_keys=(
                OperandStableKey(kind="scalar", dtype="f32"),
            ),
            context_key=frozenset(("round_mode", "FLOOR"),),
        )
        
        assert key1.to_hash() != key2.to_hash()


class TestComputeStableKey:
    """Tests for compute_stable_key function."""
    
    def test_tile_operand_specs(self):
        """Test with tile operand specs (all fields enter key)."""
        operand_specs = [
            {
                "kind": "tile",
                "dtype": "f32",
                "shape": [16, 64],
                "valid_shape": [16, 64],
                "memory_space": "ub",
                "config": {
                    "b_layout": "row_major",
                    "s_layout": "none_box",
                    "s_fractal_size": 512,
                    "pad_value": 0,
                },
            },
            {
                "kind": "tile",
                "dtype": "f32",
                "shape": [16, 64],
                "valid_shape": [16, 64],
                "memory_space": "ub",
                "config": {
                    "b_layout": "row_major",
                    "s_layout": "none_box",
                    "s_fractal_size": 512,
                    "pad_value": 0,
                },
            },
            {
                "kind": "tile",
                "dtype": "f32",
                "shape": [16, 64],
                "valid_shape": [16, 64],
                "memory_space": "ub",
                "config": {
                    "b_layout": "row_major",
                    "s_layout": "none_box",
                    "s_fractal_size": 512,
                    "pad_value": 0,
                },
            },
        ]
        
        key = compute_stable_key(
            target="a5",
            op="tadd",
            operand_specs=operand_specs,
        )
        
        assert key.target == "a5"
        assert key.op == "tadd"
        assert len(key.operand_keys) == 3
        assert all(ok.kind == "tile" for ok in key.operand_keys)
    
    def test_view_operand_specs_enhanced(self):
        """Test with view operand specs (ENHANCED: shape/strides/ms enter key)."""
        operand_specs = [
            {
                "kind": "view",
                "dtype": "f32",
                "shape": [16, 64],
                "strides": [64, 1],
                "memory_space": "gm",
            },
        ]
        
        key = compute_stable_key(
            target="a5",
            op="tload",
            operand_specs=operand_specs,
        )
        
        assert len(key.operand_keys) == 1
        ok = key.operand_keys[0]
        assert ok.kind == "view"
        assert ok.dtype == "f32"
        assert ok.view_shape == (16, 64)
        assert ok.view_strides == (64, 1)
        assert ok.view_memory_space == "gm"
    
    def test_view_different_strides_different_key(self):
        """Test that different strides produce different key (CRITICAL)."""
        specs1 = [
            {
                "kind": "view",
                "dtype": "f32",
                "shape": [16, 64],
                "strides": [64, 1],  # continuous
                "memory_space": "gm",
            },
        ]
        
        specs2 = [
            {
                "kind": "view",
                "dtype": "f32",
                "shape": [16, 64],
                "strides": [128, 2],  # strided
                "memory_space": "gm",
            },
        ]
        
        key1 = compute_stable_key("a5", "tload", specs1)
        key2 = compute_stable_key("a5", "tload", specs2)
        
        # CRITICAL: different strides must produce different key
        assert key1.to_hash() != key2.to_hash()
    
    def test_view_different_shape_different_key(self):
        """Test that different shape produce different key (CRITICAL)."""
        specs1 = [
            {
                "kind": "view",
                "dtype": "f32",
                "shape": [16, 64],
                "strides": [64, 1],
                "memory_space": "gm",
            },
        ]
        
        specs2 = [
            {
                "kind": "view",
                "dtype": "f32",
                "shape": [32, 128],
                "strides": [64, 1],
                "memory_space": "gm",
            },
        ]
        
        key1 = compute_stable_key("a5", "tload", specs1)
        key2 = compute_stable_key("a5", "tload", specs2)
        
        # CRITICAL: different shape must produce different key
        assert key1.to_hash() != key2.to_hash()
    
    def test_view_different_memory_space_different_key(self):
        """Test that different memory_space produce different key (CRITICAL)."""
        specs1 = [
            {
                "kind": "view",
                "dtype": "f32",
                "shape": [16, 64],
                "strides": [64, 1],
                "memory_space": "gm",
            },
        ]
        
        specs2 = [
            {
                "kind": "view",
                "dtype": "f32",
                "shape": [16, 64],
                "strides": [64, 1],
                "memory_space": "ub",
            },
        ]
        
        key1 = compute_stable_key("a5", "tload", specs1)
        key2 = compute_stable_key("a5", "tload", specs2)
        
        # CRITICAL: different memory_space must produce different key
        assert key1.to_hash() != key2.to_hash()
    
    def test_scalar_operand_specs(self):
        """Test with scalar operand specs."""
        operand_specs = [
            {
                "kind": "scalar",
                "dtype": "f32",
            },
        ]
        
        key = compute_stable_key(
            target="a5",
            op="tadds",
            operand_specs=operand_specs,
        )
        
        assert len(key.operand_keys) == 1
        ok = key.operand_keys[0]
        assert ok.kind == "scalar"
        assert ok.dtype == "f32"
    
    def test_context_attrs(self):
        """Test with context_attrs."""
        operand_specs = [
            {
                "kind": "tile",
                "dtype": "f32",
                "shape": [16, 64],
                "memory_space": "ub",
            },
        ]
        
        context_attrs = {
            "round_mode": "RINT",
        }
        
        key = compute_stable_key(
            target="a5",
            op="tcvt",
            operand_specs=operand_specs,
            context_attrs=context_attrs,
        )
        
        assert key.context_key == frozenset(("round_mode", "RINT"),)
    
    def test_mixed_operands(self):
        """Test with mixed tile/view/scalar operands."""
        operand_specs = [
            {
                "kind": "tile",
                "dtype": "f32",
                "shape": [16, 64],
                "memory_space": "ub",
            },
            {
                "kind": "view",
                "dtype": "f32",
                "shape": [16, 64],
                "strides": [64, 1],
                "memory_space": "gm",
            },
            {
                "kind": "scalar",
                "dtype": "f32",
            },
        ]
        
        key = compute_stable_key(
            target="a5",
            op="tadd",
            operand_specs=operand_specs,
        )
        
        assert len(key.operand_keys) == 3
        assert key.operand_keys[0].kind == "tile"
        assert key.operand_keys[1].kind == "view"
        assert key.operand_keys[2].kind == "scalar"
    
    def test_immutable_key(self):
        """Test that StableKey is immutable (frozen=True)."""
        key = compute_stable_key(
            target="a5",
            op="tadd",
            operand_specs=[{"kind": "tile", "dtype": "f32", "shape": [16, 64]}],
        )
        
        # Should raise error if we try to modify
        with pytest.raises(Exception):  # FrozenInstanceError
            key.target = "a3"


class TestStableKeyIntegration:
    """Integration tests for stable_key."""
    
    def test_real_tadd_scenario(self):
        """Test real scenario from ExpandTileOp.cpp."""
        # Simulate tadd with 3 tile operands (f32, 16x64)
        operand_specs = [
            {
                "kind": "tile",
                "dtype": "f32",
                "shape": [16, 64],
                "valid_shape": [16, 64],
                "memory_space": "ub",
                "config": {
                    "b_layout": "row_major",
                    "s_layout": "none_box",
                    "s_fractal_size": 512,
                    "pad_value": "0x0",
                },
            },
            {
                "kind": "tile",
                "dtype": "f32",
                "shape": [16, 64],
                "valid_shape": [16, 64],
                "memory_space": "ub",
                "config": {
                    "b_layout": "row_major",
                    "s_layout": "none_box",
                    "s_fractal_size": 512,
                    "pad_value": "0x0",
                },
            },
            {
                "kind": "tile",
                "dtype": "f32",
                "shape": [16, 64],
                "valid_shape": [16, 64],
                "memory_space": "ub",
                "config": {
                    "b_layout": "row_major",
                    "s_layout": "none_box",
                    "s_fractal_size": 512,
                    "pad_value": "0x0",
                },
            },
        ]
        
        key = compute_stable_key(
            target="a5",
            op="tadd",
            operand_specs=operand_specs,
        )
        
        # Verify key structure
        assert key.target == "a5"
        assert key.op == "tadd"
        assert len(key.operand_keys) == 3
        
        # Verify hash is stable
        hash1 = key.to_hash()
        hash2 = key.to_hash()
        assert hash1 == hash2
    
    def test_different_dtype_different_key(self):
        """Test that different dtype produces different key."""
        specs_f32 = [
            {"kind": "tile", "dtype": "f32", "shape": [16, 64]},
        ]
        specs_f16 = [
            {"kind": "tile", "dtype": "f16", "shape": [16, 64]},
        ]
        
        key_f32 = compute_stable_key("a5", "tadd", specs_f32)
        key_f16 = compute_stable_key("a5", "tadd", specs_f16)
        
        assert key_f32.to_hash() != key_f16.to_hash()
    
    def test_different_shape_different_key(self):
        """Test that different shape produces different key."""
        specs_16x64 = [
            {"kind": "tile", "dtype": "f32", "shape": [16, 64]},
        ]
        specs_32x128 = [
            {"kind": "tile", "dtype": "f32", "shape": [32, 128]},
        ]
        
        key1 = compute_stable_key("a5", "tadd", specs_16x64)
        key2 = compute_stable_key("a5", "tadd", specs_32x128)
        
        assert key1.to_hash() != key2.to_hash()
    
    def test_different_target_different_key(self):
        """Test that different target produces different key."""
        specs = [
            {"kind": "tile", "dtype": "f32", "shape": [16, 64]},
        ]
        
        key_a5 = compute_stable_key("a5", "tadd", specs)
        key_a3 = compute_stable_key("a3", "tadd", specs)
        
        assert key_a5.to_hash() != key_a3.to_hash()
    
    def test_different_op_different_key(self):
        """Test that different op produces different key."""
        specs = [
            {"kind": "tile", "dtype": "f32", "shape": [16, 64]},
        ]
        
        key_tadd = compute_stable_key("a5", "tadd", specs)
        key_tmul = compute_stable_key("a5", "tmul", specs)
        
        assert key_tadd.to_hash() != key_tmul.to_hash()