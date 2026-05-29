// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

#include <stdint.h>

#ifndef AICORE
#define AICORE [aicore]
#endif

// ---- case1: basic TGEMV f16 x f16 -> f32, 1x300x60 ----
extern "C" __global__ AICORE void TGEMV_f16_1x300x60(__gm__ uint16_t *a, __gm__ uint16_t *b, __gm__ float *c);
void LaunchTGEMV_f16_1x300x60(void *a, void *b, void *c, void *stream) {
    TGEMV_f16_1x300x60<<<1, nullptr, stream>>>((__gm__ uint16_t *)a, (__gm__ uint16_t *)b, (__gm__ float *)c);
}

// ---- case2: TGEMV_BIAS + TGEMV_ACC f16 x f16 -> f32, 1x512x85 (split-K with bias) ----
extern "C" __global__ AICORE void TGEMV_BIAS_f16_1x512x85(__gm__ uint16_t *a, __gm__ uint16_t *b, __gm__ float *bias, __gm__ float *c);
void LaunchTGEMV_BIAS_f16_1x512x85(void *a, void *b, void *bias, void *c, void *stream) {
    TGEMV_BIAS_f16_1x512x85<<<1, nullptr, stream>>>((__gm__ uint16_t *)a, (__gm__ uint16_t *)b, (__gm__ float *)bias, (__gm__ float *)c);
}
