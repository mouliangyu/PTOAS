// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You can not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

// Host driver for TileLang trandom ST — case-table driven.
// Each case launches a different kernel variant, reads/writes from per-case subdirectory.
// Numerical comparison is done externally by compare.py.

#include "acl/acl.h"
#include "test_common.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/stat.h>

using namespace PtoTestCommon;

// Kernel launch wrappers (defined in launch.cpp)
void LaunchTRANDOM_ui32_4x256(uint32_t *out, uint32_t key0, uint32_t key1, uint32_t counter0, uint32_t counter1, uint32_t counter2, uint32_t counter3, void *stream);

using LaunchFn = void (*)(uint32_t *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void *);

struct TestCase {
    const char *name;
    LaunchFn    launch;
    size_t      rows;       // allocated tile rows for output
    size_t      cols;       // allocated tile cols for output
    size_t      validRows;  // effective computation rows  (<= rows)
    size_t      validCols;  // effective computation cols  (<= cols)
    size_t      elemSize;   // bytes per element
    size_t      keySize;    // key (seed) size in bytes (2 * elemSize)
    size_t      counterSize; // counter size in bytes (4 * elemSize)
};

static const TestCase kCases[] = {
    {"i32_4x256", LaunchTRANDOM_ui32_4x256, 4, 256, 4, 256, sizeof(uint32_t), 2 * sizeof(uint32_t), 4 * sizeof(uint32_t)},
};
static constexpr size_t kNumCases = sizeof(kCases) / sizeof(kCases[0]);

static int RunCase(const TestCase &tc, int deviceId, aclrtStream stream) {
    int rc = 0;
    const size_t elemCount = tc.rows * tc.cols;
    const size_t fileSize  = elemCount * tc.elemSize;

    std::printf("[INFO] === case: %s (shape=%zux%zu, valid=%zux%zu) ===\n",
                tc.name, tc.rows, tc.cols, tc.validRows, tc.validCols);

    // Per-case data directory
    std::string caseDir = std::string("./") + tc.name;

    uint32_t *keyHost = nullptr, *counterHost = nullptr, *dstHost = nullptr;
    uint32_t *dstDevice = nullptr;
    size_t keyFileSize = 0, counterFileSize = 0;

    aclrtMallocHost((void **)(&keyHost), tc.keySize);
    aclrtMallocHost((void **)(&counterHost), tc.counterSize);
    aclrtMallocHost((void **)(&dstHost), fileSize);

    aclrtMalloc((void **)&dstDevice, fileSize, ACL_MEM_MALLOC_HUGE_FIRST);

    if (!ReadFile(std::string(caseDir + "/key.bin"), keyFileSize, keyHost, tc.keySize)) {
        std::fprintf(stderr, "[ERROR] failed to read %s/key.bin\n", caseDir.c_str());
        rc = 1;
    }
    if (rc == 0 && !ReadFile(std::string(caseDir + "/counter.bin"), counterFileSize, counterHost, tc.counterSize)) {
        std::fprintf(stderr, "[ERROR] failed to read %s/counter.bin\n", caseDir.c_str());
        rc = 1;
    }

    if (rc == 0) {
        tc.launch(dstDevice, keyHost[0], keyHost[1], counterHost[0], counterHost[1], counterHost[2], counterHost[3], stream);

        aclrtSynchronizeStream(stream);
        aclrtMemcpy(dstHost, fileSize, dstDevice, fileSize, ACL_MEMCPY_DEVICE_TO_HOST);
    }

    if (rc == 0 && !WriteFile((caseDir + "/output.bin").c_str(), dstHost, fileSize)) {
        std::fprintf(stderr, "[ERROR] failed to write %s/output.bin\n", caseDir.c_str());
        rc = 1;
    }

    if (dstDevice != nullptr)
        aclrtFree(dstDevice);
    if (keyHost != nullptr)
        aclrtFreeHost(keyHost);
    if (counterHost != nullptr)
        aclrtFreeHost(counterHost);
    if (dstHost != nullptr)
        aclrtFreeHost(dstHost);

    if (rc == 0)
        std::printf("[INFO] case %s done\n", tc.name);
    return rc;
}

int main(int argc, char *argv[]) {
    // Optional case filter: ./trandom [case_name]
    const char *caseFilter = (argc > 1) ? argv[1] : nullptr;

    int rc = 0;
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
        int ret = RunCase(kCases[i], deviceId, stream);
        if (ret != 0) {
            std::fprintf(stderr, "[ERROR] case %s failed\n", kCases[i].name);
            rc = 1;
            break;
        }
    }

    if (stream != nullptr)
        aclrtDestroyStream(stream);
    aclrtResetDevice(deviceId);
    aclFinalize();

    return rc;
}