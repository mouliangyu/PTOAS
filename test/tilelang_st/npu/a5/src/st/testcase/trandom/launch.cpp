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

extern "C" __global__ AICORE void TRANDOM_ui32_4x256(__gm__ uint32_t *out, uint32_t key0, uint32_t key1, uint32_t counter0, uint32_t counter1, uint32_t counter2, uint32_t counter3);

void LaunchTRANDOM_ui32_4x256(uint32_t *out, uint32_t key0, uint32_t key1, uint32_t counter0, uint32_t counter1, uint32_t counter2, uint32_t counter3, void *stream) {
    TRANDOM_ui32_4x256<<<1, nullptr, stream>>>((__gm__ uint32_t *)out, key0, key1, counter0, counter1, counter2, counter3);
}