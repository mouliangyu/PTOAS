# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

# coding=utf-8

"""Single source of truth for tmrgsort ST test cases.

Each case defines:
  - name:        case identifier, used as subdirectory name and by main.cpp kCases[].
  - dtype:       numpy dtype (e.g. np.float32, np.float16).
  - format:      "single" for Format1 (1-list internal block sorting),
                 "multi" for Format2-4 (multi-list merge sort).
  - src_shape:   (rows, cols) - allocated source tile dimensions.
                 For Format1: single input list.
                 For multi-list: list of shapes for each input.
  - dst_shape:   (rows, cols) - allocated destination tile dimensions.
  - valid_shape: (valid_rows, valid_cols) - effective computation region.
  - block_len:   For Format1: block length in elements (must divide src_cols by 4).
  - list_num:    For multi-list: number of input lists (2, 3, or 4).
  - src_cols:    For multi-list: list of valid cols for each input list.
  - topk:        For multi-list: top-k output count.
  - exhausted:   For multi-list: whether to enable exhausted suspension.
  - eps:         tolerance for numpy.allclose (atol and rtol).

tmrgsort semantics:
  - Format1 (single list): Sorts 4 internal blocks of src using vmrgsort4.
    Each block is sorted independently, then merged.
    Output: interleaved (sorted_value, original_index) pairs.
  - Format2-4 (multi-list): Merges 2-4 sorted input lists into one sorted output.
    Each input list must already be sorted (in descending order).
    Output: top-k sorted elements from merged lists.

gen_data.py and compare.py both import this list to avoid redundant definitions.
"""

import numpy as np

CASES = [
    # Format1: single list (internal block sorting)
    # Transplanted from pto-isa case_single1: TMrgsortSingle<float, 1, 256, 1, 256, 64>
    # Shape uses FLOAT ELEMENT count (matching pto-isa kGCols convention)
    # src_cols=256 float elements = 128 (value,index) structures
    # block_len=64 float elements = 32 structures/block, 4 blocks total
    {
        "name": "f32_single_1x256_b64",
        "dtype": np.float32,
        "format": "single",
        "src_shape": (1, 256),  # kGCols=256 float elements
        "dst_shape": (1, 256),  # kGCols=256 float elements
        "valid_shape": (1, 256),
        "block_len": 64,        # float elements (=32 structures)
        "eps": 1e-6,
    },
    # Transplanted from pto-isa case_single2: TMrgsortSingle<float, 1, 320, 1, 256, 64>
    # GCols=320 > TCols=256, global memory has padding, kernel uses TCols
    # src_cols=320 float elements (global), valid_cols=256 float elements (tile)
    # block_len=64 float elements = 32 structures/block
    {
        "name": "f32_single_1x320_b64",
        "dtype": np.float32,
        "format": "single",
        "src_shape": (1, 320),  # kGCols=320 float elements (global)
        "dst_shape": (1, 320),  # kGCols=320 float elements (global)
        "valid_shape": (1, 256),  # kTCols=256 (effective tile region)
        "block_len": 64,        # float elements (=32 structures)
        "eps": 1e-6,
    },
    # Transplanted from pto-isa case_single3: TMrgsortSingle<float, 1, 512, 1, 512, 64>
    # cols=512 float elements = 256 structures
    # block_len=64 float elements = 32 structures/block, 4 blocks total
    {
        "name": "f32_single_1x512_b64",
        "dtype": np.float32,
        "format": "single",
        "src_shape": (1, 512),  # kGCols=512 float elements
        "dst_shape": (1, 512),  # kGCols=512 float elements
        "valid_shape": (1, 512),
        "block_len": 64,        # float elements (=32 structures)
        "eps": 1e-6,
    },
    # Transplanted from pto-isa case_single4: TMrgsortSingle<float, 1, 640, 1, 512, 64>
    # kGCols=640 > kTCols=512, global memory has padding, kernel uses kTCols
    # src_cols=640 float elements (global), valid_cols=512 float elements (tile)
    # block_len=64 float elements = 32 structures/block
    {
        "name": "f32_single_1x640_b64",
        "dtype": np.float32,
        "format": "single",
        "src_shape": (1, 640),  # kGCols=640 float elements (global)
        "dst_shape": (1, 640),  # kGCols=640 float elements (global)
        "valid_shape": (1, 512),  # kTCols=512 (effective tile region)
        "block_len": 64,        # float elements (=32 structures)
        "eps": 1e-6,
    },
    # Transplanted from pto-isa case_single5: TMrgsortSingle<uint16_t, 1, 256, 1, 256, 64>
    # uint16_t maps to float16 (half) in Ascend C
    # cols=256 float16 elements = 128 structures
    # block_len=64 float16 elements = 32 structures/block, 4 blocks total
    {
        "name": "f16_single_1x256_b64",
        "dtype": np.float16,
        "format": "single",
        "src_shape": (1, 256),  # kGCols=256 float16 elements
        "dst_shape": (1, 256),  # kGCols=256 float16 elements
        "valid_shape": (1, 256),
        "block_len": 64,        # float16 elements (=32 structures)
        "eps": 1e-3,            # f16 has lower precision
    },
    # Transplanted from pto-isa case_single6: TMrgsortSingle<uint16_t, 1, 320, 1, 256, 64>
    # TYPE_COEF=2: kGCols*2=640, kTCols*2=512, blockLen*2=128 (kernel internal)
    # kGCols=320 > kTCols=256, global memory has padding
    # src_cols=320 float16 elements (global), valid_cols=256 float16 elements (tile)
    # block_len=64 float16 elements = 32 structures/block
    {
        "name": "f16_single_1x320_b64",
        "dtype": np.float16,
        "format": "single",
        "src_shape": (1, 320),  # kGCols=320 float16 elements (global)
        "dst_shape": (1, 320),  # kGCols=320 float16 elements (global)
        "valid_shape": (1, 256),  # kTCols=256 (effective tile region)
        "block_len": 64,        # float16 elements (=32 structures)
        "eps": 1e-3,
    },
    # Transplanted from pto-isa case_single7: TMrgsortSingle<uint16_t, 1, 512, 1, 512, 64>
    # TYPE_COEF=2: kGCols*2=1024, kTCols*2=1024, blockLen*2=128 (kernel internal)
    # cols=1024 half elements = 256 structures
    # block_len=128 half elements = 32 structures/block, repeat_times=2
    {
        "name": "f16_single_1x512_b64",
        "dtype": np.float16,
        "format": "single",
        "src_shape": (1, 512),  # kGCols=512 float16 elements
        "dst_shape": (1, 512),  # kGCols=512 float16 elements
        "valid_shape": (1, 512),
        "block_len": 64,        # float16 elements (=32 structures)
        "eps": 1e-3,
    },
    # Transplanted from pto-isa case_single8: TMrgsortSingle<uint16_t, 1, 1024, 1, 1024, 256>
    # TYPE_COEF=2: kGCols*2=2048, kTCols*2=2048, blockLen*2=512 (kernel internal)
    # cols=2048 half elements = 512 structures
    # block_len=512 half elements = 128 structures/block, repeat_times=1
    {
        "name": "f16_single_1x1024_b256",
        "dtype": np.float16,
        "format": "single",
        "src_shape": (1, 1024),  # kGCols=1024 float16 elements
        "dst_shape": (1, 1024),  # kGCols=1024 float16 elements
        "valid_shape": (1, 1024),
        "block_len": 256,       # float16 elements (=128 structures, larger block)
        "eps": 1e-3,
    },
    # Format2: multi-list merge (2-list merge)
    {
        "name": "f32_2list_b64_basic",
        "dtype": np.float32,
        "format": "multi",
        "list_num": 2,
        "src_cols": [128, 128],
        "src_shape": [(1, 256), (1, 256)],
        "dst_shape": (1, 256),
        "valid_shape": (1, 256),
        "topk": 128,
        "exhausted": False,
        "eps": 1e-6,
    },
    {
        "name": "f16_2list_b64_basic",
        "dtype": np.float16,
        "format": "multi",
        "list_num": 2,
        "src_cols": [64, 64],  # 64 structures per list (match src_shape)
        "src_shape": [(1, 256), (1, 256)],  # 256 f16 elements = 64 structures
        "dst_shape": (1, 256),
        "valid_shape": (1, 256),
        "topk": 64,  # topk should match dst capacity
        "exhausted": False,
        "eps": 1e-3,
    },
    # Format2: exhausted=true cases
    {
        "name": "f32_2list_exhausted",
        "dtype": np.float32,
        "format": "multi",
        "list_num": 2,
        "src_cols": [32, 32],  # 32 structures per list
        "src_shape": [(1, 128), (1, 128)],  # 128 f32 elements = 32 structures
        "dst_shape": (1, 128),  # 128 f32 elements = 64 structures (match topk)
        "valid_shape": (1, 128),  # ← 必须匹配dst_shape
        "topk": 64,  # topk in structures (=dst capacity)
        "exhausted": True,
        "eps": 1e-6,
    },
    # Format3: 3-list merge sort
    {
        "name": "f32_3list_b64_basic",
        "dtype": np.float32,
        "format": "multi",
        "list_num": 3,
        "src_cols": [64, 64, 64],  # 64 structures per list
        "src_shape": [(1, 128), (1, 128), (1, 128)],  # 128 f32 elements = 64 structures each
        "dst_shape": (1, 256),  # 256 f32 elements = 128 structures
        "valid_shape": (1, 256),
        "topk": 128,  # topk structures (192 available, output 128)
        "exhausted": False,
        "eps": 1e-6,
    },
    # Format4: 4-list merge sort
    {
        "name": "f32_4list_b32_basic",
        "dtype": np.float32,
        "format": "multi",
        "list_num": 4,
        "src_cols": [32, 32, 32, 32],  # 32 structures per list
        "src_shape": [(1, 64), (1, 64), (1, 64), (1, 64)],  # 64 f32 elements = 32 structures each
        "dst_shape": (1, 256),  # 256 f32 elements = 128 structures
        "valid_shape": (1, 256),
        "topk": 128,  # topk structures (128 available, output 128)
        "exhausted": False,
        "eps": 1e-6,
    },
    {
        "name": "f16_4list_b64_basic",
        "dtype": np.float16,
        "format": "multi",
        "list_num": 4,
        "src_cols": [64, 64, 64, 64],  # 64 structures per list
        "src_shape": [(1, 256), (1, 256), (1, 256), (1, 256)],  # 256 f16 elements = 64 structures each
        "dst_shape": (1, 1024),  # 1024 f16 elements = 256 structures
        "valid_shape": (1, 1024),
        "topk": 256,  # topk structures (256 available, output 256)
        "exhausted": False,
        "eps": 1e-3,
    },
    # Format3 variants: non-uniform cols
    {
        "name": "f32_3list_non_uniform",
        "dtype": np.float32,
        "format": "multi",
        "list_num": 3,
        "src_cols": [64, 64, 32],  # non-uniform: 64,64,32 structures
        "src_shape": [(1, 128), (1, 128), (1, 64)],  # f32 elements
        "dst_shape": (1, 128),  # 128 f32 elements = 64 structures
        "valid_shape": (1, 128),
        "topk": 64,  # structures (total=160 available, output topk=64)
        "exhausted": False,
        "eps": 1e-6,
    },
    # Format3 variants: f16 exhausted (adjusted to fit tmp capacity)
    # tmp tile cols=512 can hold max 256 structures for f16 (512/2=256)
    # src_cols in STRUCTURES, srcShape in ELEMENTS (f16: 4 elems/struct)
    {
        "name": "f16_3list_exhausted",
        "dtype": np.float16,
        "format": "multi",
        "list_num": 3,
        "src_cols": [85, 85, 85],  # 85 structures per list, total=255 < 256 (tmp capacity)
        "src_shape": [(1, 340), (1, 340), (1, 340)],  # 340 f16 elements = 85 structures each (85*4)
        "dst_shape": (1, 680),  # 680 f16 elements = 170 structures (topk=170, 170*4)
        "valid_shape": (1, 680),
        "topk": 170,  # structures (total=255 available, output topk=170)
        "exhausted": True,
        "eps": 1e-3,
    },
    # Format4 variants: non-uniform cols
    {
        "name": "f32_4list_non_uniform",
        "dtype": np.float32,
        "format": "multi",
        "list_num": 4,
        "src_cols": [64, 64, 64, 32],  # non-uniform: 64,64,64,32 structures
        "src_shape": [(1, 128), (1, 128), (1, 128), (1, 64)],  # f32 elements
        "dst_shape": (1, 448),  # 448 f32 elements = 224 structures
        "valid_shape": (1, 448),
        "topk": 224,  # structures (total=224, output all)
        "exhausted": False,
        "eps": 1e-6,
    }
]