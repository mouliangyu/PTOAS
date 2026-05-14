#!/usr/bin/env python3
"""Simple test runner for stable_key module (without pytest)."""

import sys
sys.path.insert(0, '/home/zhoushaofan/code/PTOAS/tilelang-dsl/python')

from tilelang_dsl.stable_key import (
    StableKey,
    OperandStableKey,
    compute_stable_key,
    normalize_dtype,
    normalize_blayout,
    normalize_slayout,
    normalize_memory_space,
)


def test_normalize_dtype():
    """Test normalize_dtype."""
    assert normalize_dtype("f32") == "f32"
    assert normalize_dtype("F32") == "f32"
    assert normalize_dtype("f16") == "f16"
    print("✓ test_normalize_dtype passed")


def test_normalize_blayout():
    """Test normalize_blayout."""
    assert normalize_blayout("row_major") == "row_major"
    assert normalize_blayout(0) == "row_major"
    assert normalize_blayout(1) == "col_major"
    assert normalize_blayout(None) == "row_major"
    print("✓ test_normalize_blayout passed")


def test_normalize_slayout():
    """Test normalize_slayout."""
    assert normalize_slayout("none_box") == "none_box"
    assert normalize_slayout(0) == "none_box"
    assert normalize_slayout(1) == "row_major"
    assert normalize_slayout(None) == "none_box"
    print("✓ test_normalize_slayout passed")


def test_operand_stable_key_tile():
    """Test OperandStableKey for tile."""
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
    print("✓ test_operand_stable_key_tile passed")


def test_operand_stable_key_view_enhanced():
    """Test OperandStableKey for view (ENHANCED)."""
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
    print("✓ test_operand_stable_key_view_enhanced passed")


def test_operand_stable_key_scalar():
    """Test OperandStableKey for scalar."""
    key = OperandStableKey(kind="scalar", dtype="f32")
    hash_input = key.to_hash_input()
    assert hash_input == "scalar:f32"
    print("✓ test_operand_stable_key_scalar passed")


def test_stable_key_hash():
    """Test StableKey hash."""
    key = StableKey(
        target="a5",
        op="tadd",
        operand_keys=(OperandStableKey(kind="tile", dtype="f32"),),
        context_key=frozenset(),
    )
    
    hash1 = key.to_hash()
    assert len(hash1) == 16
    assert hash1.isalnum()
    print("✓ test_stable_key_hash passed")


def test_same_params_same_hash():
    """Test same parameters produce same hash."""
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
    print("✓ test_same_params_same_hash passed")


def test_different_params_different_hash():
    """Test different parameters produce different hash."""
    key1 = StableKey(
        target="a5",
        op="tadd",
        operand_keys=(OperandStableKey(kind="tile", dtype="f32"),),
        context_key=frozenset(),
    )
    
    key2 = StableKey(
        target="a5",
        op="tadd",
        operand_keys=(OperandStableKey(kind="tile", dtype="f16"),),
        context_key=frozenset(),
    )
    
    assert key1.to_hash() != key2.to_hash()
    print("✓ test_different_params_different_hash passed")


def test_compute_stable_key_tile():
    """Test compute_stable_key with tile operands."""
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
    ]
    
    key = compute_stable_key(target="a5", op="tadd", operand_specs=operand_specs)
    
    assert key.target == "a5"
    assert key.op == "tadd"
    assert len(key.operand_keys) == 1
    assert key.operand_keys[0].kind == "tile"
    print("✓ test_compute_stable_key_tile passed")


def test_compute_stable_key_view_enhanced():
    """Test compute_stable_key with view operands (ENHANCED)."""
    operand_specs = [
        {
            "kind": "view",
            "dtype": "f32",
            "shape": [16, 64],
            "strides": [64, 1],
            "memory_space": "gm",
        },
    ]
    
    key = compute_stable_key(target="a5", op="tload", operand_specs=operand_specs)
    
    ok = key.operand_keys[0]
    assert ok.kind == "view"
    assert ok.view_shape == (16, 64)
    assert ok.view_strides == (64, 1)
    assert ok.view_memory_space == "gm"
    print("✓ test_compute_stable_key_view_enhanced passed")


def test_view_different_strides_different_key():
    """CRITICAL: Test different strides produce different key."""
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
    
    assert key1.to_hash() != key2.to_hash()
    print("✓ test_view_different_strides_different_key passed (CRITICAL)")


def test_view_different_shape_different_key():
    """CRITICAL: Test different shape produce different key."""
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
    
    assert key1.to_hash() != key2.to_hash()
    print("✓ test_view_different_shape_different_key passed (CRITICAL)")


def test_view_different_memory_space_different_key():
    """CRITICAL: Test different memory_space produce different key."""
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
    
    assert key1.to_hash() != key2.to_hash()
    print("✓ test_view_different_memory_space_different_key passed (CRITICAL)")


def test_context_attrs():
    """Test context_attrs."""
    operand_specs = [
        {
            "kind": "tile",
            "dtype": "f32",
            "shape": [16, 64],
            "memory_space": "ub",
        },
    ]
    
    context_attrs = {"round_mode": "RINT"}
    
    key = compute_stable_key(
        target="a5",
        op="tcvt",
        operand_specs=operand_specs,
        context_attrs=context_attrs,
    )
    
    # context_key is frozenset of tuples: (("round_mode", "RINT"),)
    assert ("round_mode", "RINT") in key.context_key
    print("✓ test_context_attrs passed")


def test_mixed_operands():
    """Test mixed tile/view/scalar operands."""
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
    
    key = compute_stable_key(target="a5", op="tadd", operand_specs=operand_specs)
    
    assert len(key.operand_keys) == 3
    assert key.operand_keys[0].kind == "tile"
    assert key.operand_keys[1].kind == "view"
    assert key.operand_keys[2].kind == "scalar"
    print("✓ test_mixed_operands passed")


def test_real_tadd_scenario():
    """Test real scenario from ExpandTileOp.cpp."""
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
    
    key = compute_stable_key(target="a5", op="tadd", operand_specs=operand_specs)
    
    assert key.target == "a5"
    assert key.op == "tadd"
    assert len(key.operand_keys) == 3
    
    hash1 = key.to_hash()
    hash2 = key.to_hash()
    assert hash1 == hash2
    print("✓ test_real_tadd_scenario passed")


def run_all_tests():
    """Run all tests."""
    print("=" * 60)
    print("Running stable_key tests...")
    print("=" * 60)
    
    tests = [
        test_normalize_dtype,
        test_normalize_blayout,
        test_normalize_slayout,
        test_operand_stable_key_tile,
        test_operand_stable_key_view_enhanced,
        test_operand_stable_key_scalar,
        test_stable_key_hash,
        test_same_params_same_hash,
        test_different_params_different_hash,
        test_compute_stable_key_tile,
        test_compute_stable_key_view_enhanced,
        test_view_different_strides_different_key,
        test_view_different_shape_different_key,
        test_view_different_memory_space_different_key,
        test_context_attrs,
        test_mixed_operands,
        test_real_tadd_scenario,
    ]
    
    failed = 0
    for test in tests:
        try:
            test()
        except AssertionError as e:
            print(f"✗ {test.__name__} FAILED: {e}")
            failed += 1
        except Exception as e:
            print(f"✗ {test.__name__} ERROR: {e}")
            failed += 1
    
    print("=" * 60)
    if failed == 0:
        print("All tests passed! ✓")
    else:
        print(f"{failed} tests failed! ✗")
    print("=" * 60)
    
    return failed == 0


if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)