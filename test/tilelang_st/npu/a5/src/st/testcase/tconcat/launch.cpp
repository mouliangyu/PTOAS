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

extern "C" __global__ AICORE void TCONCAT_f32_16x64(__gm__ float *src0, __gm__ float *src1, __gm__ float *dst);
extern "C" __global__ AICORE void TCONCAT_f32_16x128(__gm__ float *src0, __gm__ float *src1, __gm__ float *dst);
extern "C" __global__ AICORE void TCONCAT_f32_128x16(__gm__ float *src0, __gm__ float *src1, __gm__ float *dst);
extern "C" __global__ AICORE void TCONCAT_f16_16x128(__gm__ half *src0, __gm__ half *src1, __gm__ half *dst);
extern "C" __global__ AICORE void TCONCAT_f16_128x16(__gm__ half *src0, __gm__ half *src1, __gm__ half *dst);
extern "C" __global__ AICORE void TCONCAT_f32_8x64(__gm__ float *src0, __gm__ float *src1, __gm__ int32_t *idx0, __gm__ int32_t *idx1, __gm__ float *dst);

void LaunchTCONCAT_f32_16x64(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream) {
    (void)idx0;
    (void)idx1;
    TCONCAT_f32_16x64<<<1, nullptr, stream>>>(
        (__gm__ float *)src0,
        (__gm__ float *)src1,
        (__gm__ float *)dst
    );
}

void LaunchTCONCAT_f32_16x128(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream) {
    (void)idx0;
    (void)idx1;
    TCONCAT_f32_16x128<<<1, nullptr, stream>>>(
        (__gm__ float *)src0,
        (__gm__ float *)src1,
        (__gm__ float *)dst
    );
}

void LaunchTCONCAT_f32_128x16(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream) {
    (void)idx0;
    (void)idx1;
    TCONCAT_f32_128x16<<<1, nullptr, stream>>>(
        (__gm__ float *)src0,
        (__gm__ float *)src1,
        (__gm__ float *)dst
    );
}

void LaunchTCONCAT_f16_16x128(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream) {
    (void)idx0;
    (void)idx1;
    TCONCAT_f16_16x128<<<1, nullptr, stream>>>(
        (__gm__ half *)src0,
        (__gm__ half *)src1,
        (__gm__ half *)dst
    );
}

void LaunchTCONCAT_f16_128x16(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream) {
    (void)idx0;
    (void)idx1;
    TCONCAT_f16_128x16<<<1, nullptr, stream>>>(
        (__gm__ half *)src0,
        (__gm__ half *)src1,
        (__gm__ half *)dst
    );
}
void LaunchTCONCAT_f32_8x64(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream) {
    TCONCAT_f32_8x64<<<1, nullptr, stream>>>(
        (__gm__ float *)src0,
        (__gm__ float *)src1,
        (__gm__ int32_t *)idx0,
        (__gm__ int32_t *)idx1,
        (__gm__ float *)dst
    );
}

