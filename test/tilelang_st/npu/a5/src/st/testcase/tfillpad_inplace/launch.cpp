// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You can not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

#include <stdint.h>

#ifndef AICORE
#define AICORE [aicore]
#endif

// ========== Case 5: float, 260x16, valid=260x7, inplace ==========

extern "C" __global__ AICORE void TFILLPAD_INPLACE_f32_260x16_inplace_260x7(__gm__ float *tile);

void LaunchTFILLPAD_INPLACE_f32_260x16_inplace_260x7(float *tile, void *stream) {
    TFILLPAD_INPLACE_f32_260x16_inplace_260x7<<<1, nullptr, stream>>>((__gm__ float *)tile);
}