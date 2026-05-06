// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

#include "acl/acl.h"
#include "test_common.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

using namespace PtoTestCommon;

void LaunchTCONCAT_f32_16x64(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream);
void LaunchTCONCAT_f32_16x128(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream);
void LaunchTCONCAT_f32_128x16(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream);
void LaunchTCONCAT_f16_16x128(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream);
void LaunchTCONCAT_f16_128x16(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream);
void LaunchTCONCAT_f32_8x64(void *src0, void *src1, int32_t *idx0, int32_t *idx1, void *dst, void *stream);

using LaunchFn = void (*)(void *, void *, int32_t *, int32_t *, void *, void *);

struct TestCase {
    const char *name;
    LaunchFn    launch;
    size_t      rows;
    size_t      cols;
    size_t      idxRows;
    size_t      idxCols;
    size_t      elemSize;
};

static const TestCase kCases[] = {
    {"f32_16x64_plain_concat", LaunchTCONCAT_f32_16x64, 16, 64, 16, 1, sizeof(float)},
    {"f32_16x128_plain_concat", LaunchTCONCAT_f32_16x128, 16, 128, 16, 1, sizeof(float)},
    {"f32_128x16_plain_concat", LaunchTCONCAT_f32_128x16, 128, 16, 128, 1, sizeof(float)},
    {"f16_16x128_plain_concat", LaunchTCONCAT_f16_16x128, 16, 128, 16, 1, sizeof(uint16_t)},
    {"f16_128x16_plain_concat", LaunchTCONCAT_f16_128x16, 128, 16, 128, 1, sizeof(uint16_t)},
    {"f32_8x64_even_split", LaunchTCONCAT_f32_8x64, 8, 64, 8, 1, sizeof(float)},
    {"f32_8x64_clamped_split", LaunchTCONCAT_f32_8x64, 8, 64, 8, 1, sizeof(float)},
    {"f32_8x64_edge_split", LaunchTCONCAT_f32_8x64, 8, 64, 8, 1, sizeof(float)},
};
static constexpr size_t kNumCases = sizeof(kCases) / sizeof(kCases[0]);

static int RunCase(const TestCase &tc, aclrtStream stream) {
    int rc = 0;
    const size_t elemCount = tc.rows * tc.cols;
    const size_t fileSize = elemCount * tc.elemSize;
    const size_t idxElemCount = tc.idxRows * tc.idxCols;
    const size_t idxFileSize = idxElemCount * sizeof(int32_t);

    std::printf("[INFO] === case: %s (%zux%zu, idx=%zux%zu) ===\n",
                tc.name, tc.rows, tc.cols, tc.idxRows, tc.idxCols);

    std::string caseDir = std::string("./") + tc.name;
    size_t src0FileSize = fileSize;
    size_t src1FileSize = fileSize;
    size_t idx0LoadSize = idxFileSize;
    size_t idx1LoadSize = idxFileSize;

    void *src0Host = nullptr;
    void *src1Host = nullptr;
    void *dstHost = nullptr;
    int32_t *idx0Host = nullptr;
    int32_t *idx1Host = nullptr;

    void *src0Device = nullptr;
    void *src1Device = nullptr;
    void *dstDevice = nullptr;
    int32_t *idx0Device = nullptr;
    int32_t *idx1Device = nullptr;

    aclrtMallocHost((void **)(&src0Host), fileSize);
    aclrtMallocHost((void **)(&src1Host), fileSize);
    aclrtMallocHost((void **)(&dstHost), fileSize);
    aclrtMallocHost((void **)(&idx0Host), idxFileSize);
    aclrtMallocHost((void **)(&idx1Host), idxFileSize);

    aclrtMalloc((void **)&src0Device, fileSize, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc((void **)&src1Device, fileSize, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc((void **)&dstDevice, fileSize, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc((void **)&idx0Device, idxFileSize, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc((void **)&idx1Device, idxFileSize, ACL_MEM_MALLOC_HUGE_FIRST);

    if (!ReadFile((caseDir + "/input1.bin").c_str(), src0FileSize, src0Host, fileSize)) {
        std::fprintf(stderr, "[ERROR] failed to read %s/input1.bin\n", caseDir.c_str());
        rc = 1;
    }
    if (rc == 0 && !ReadFile((caseDir + "/input2.bin").c_str(), src1FileSize, src1Host, fileSize)) {
        std::fprintf(stderr, "[ERROR] failed to read %s/input2.bin\n", caseDir.c_str());
        rc = 1;
    }
    if (rc == 0 && !ReadFile((caseDir + "/idx0.bin").c_str(), idx0LoadSize, idx0Host, idxFileSize)) {
        std::fprintf(stderr, "[ERROR] failed to read %s/idx0.bin\n", caseDir.c_str());
        rc = 1;
    }
    if (rc == 0 && !ReadFile((caseDir + "/idx1.bin").c_str(), idx1LoadSize, idx1Host, idxFileSize)) {
        std::fprintf(stderr, "[ERROR] failed to read %s/idx1.bin\n", caseDir.c_str());
        rc = 1;
    }

    if (rc == 0) {
        aclrtMemcpy(src0Device, fileSize, src0Host, fileSize, ACL_MEMCPY_HOST_TO_DEVICE);
        aclrtMemcpy(src1Device, fileSize, src1Host, fileSize, ACL_MEMCPY_HOST_TO_DEVICE);
        aclrtMemcpy(idx0Device, idxFileSize, idx0Host, idxFileSize, ACL_MEMCPY_HOST_TO_DEVICE);
        aclrtMemcpy(idx1Device, idxFileSize, idx1Host, idxFileSize, ACL_MEMCPY_HOST_TO_DEVICE);

        tc.launch(src0Device, src1Device, idx0Device, idx1Device, dstDevice, stream);

        aclrtSynchronizeStream(stream);
        aclrtMemcpy(dstHost, fileSize, dstDevice, fileSize, ACL_MEMCPY_DEVICE_TO_HOST);
    }

    if (rc == 0 && !WriteFile((caseDir + "/output.bin").c_str(), dstHost, fileSize)) {
        std::fprintf(stderr, "[ERROR] failed to write %s/output.bin\n", caseDir.c_str());
        rc = 1;
    }

    if (src0Device != nullptr)
        aclrtFree(src0Device);
    if (src1Device != nullptr)
        aclrtFree(src1Device);
    if (dstDevice != nullptr)
        aclrtFree(dstDevice);
    if (idx0Device != nullptr)
        aclrtFree(idx0Device);
    if (idx1Device != nullptr)
        aclrtFree(idx1Device);

    if (src0Host != nullptr)
        aclrtFreeHost(src0Host);
    if (src1Host != nullptr)
        aclrtFreeHost(src1Host);
    if (dstHost != nullptr)
        aclrtFreeHost(dstHost);
    if (idx0Host != nullptr)
        aclrtFreeHost(idx0Host);
    if (idx1Host != nullptr)
        aclrtFreeHost(idx1Host);

    if (rc == 0)
        std::printf("[INFO] case %s done\n", tc.name);
    return rc;
}

int main(int argc, char *argv[]) {
    const char *caseFilter = (argc > 1) ? argv[1] : nullptr;

    int rc = 0;
    bool matchedCase = (caseFilter == nullptr);
    int deviceId = 0;
    aclrtStream stream = nullptr;

    aclInit(nullptr);
    if (const char *envDevice = std::getenv("ACL_DEVICE_ID")) {
        deviceId = std::atoi(envDevice);
    }
    aclrtSetDevice(deviceId);
    aclrtCreateStream(&stream);

    for (size_t i = 0; i < kNumCases; ++i) {
        if (caseFilter != nullptr && std::strcmp(kCases[i].name, caseFilter) != 0) {
            continue;
        }
        matchedCase = true;
        int ret = RunCase(kCases[i], stream);
        if (ret != 0) {
            std::fprintf(stderr, "[ERROR] case %s failed\n", kCases[i].name);
            rc = 1;
            break;
        }
    }

    if (!matchedCase) {
        std::fprintf(stderr, "[ERROR] unknown case filter: %s\n", caseFilter);
        std::fprintf(stderr, "[ERROR] supported cases:");
        for (size_t i = 0; i < kNumCases; ++i) {
            std::fprintf(stderr, " %s", kCases[i].name);
        }
        std::fprintf(stderr, "\n");
        rc = 1;
    }

    if (stream != nullptr)
        aclrtDestroyStream(stream);
    aclrtResetDevice(deviceId);
    aclFinalize();

    return rc;
}
