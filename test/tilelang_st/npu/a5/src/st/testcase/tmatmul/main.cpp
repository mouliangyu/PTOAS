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

void LaunchTMATMUL_f16f16_to_f32_32x32(uint16_t *a, uint16_t *b, float *c,
                                       void *stream);

using LaunchFn = void (*)(uint16_t *, uint16_t *, float *, void *);

struct TestCase {
  const char *name;
  LaunchFn launch;
  size_t rows;
  size_t cols;
  size_t validRows;
  size_t validCols;
  size_t inElemSize;
  size_t outElemSize;
};

static const TestCase kCases[] = {
    {"f16f16_to_f32_32x32", LaunchTMATMUL_f16f16_to_f32_32x32, 32, 32, 32, 32,
     sizeof(uint16_t), sizeof(float)},
};
static constexpr size_t kNumCases = sizeof(kCases) / sizeof(kCases[0]);

static int RunCase(const TestCase &tc, aclrtStream stream) {
  int rc = 0;
  const size_t elemCount = tc.rows * tc.cols;
  const size_t inFileSize = elemCount * tc.inElemSize;
  const size_t outFileSize = elemCount * tc.outElemSize;

  std::string caseDir = std::string("./") + tc.name;
  std::printf("[INFO] === case: %s (shape=%zux%zu, valid=%zux%zu) ===\n",
              tc.name, tc.rows, tc.cols, tc.validRows, tc.validCols);

  uint16_t *src0Host = nullptr;
  uint16_t *src1Host = nullptr;
  float *dstHost = nullptr;
  uint16_t *src0Device = nullptr;
  uint16_t *src1Device = nullptr;
  float *dstDevice = nullptr;

  aclrtMallocHost((void **)&src0Host, inFileSize);
  aclrtMallocHost((void **)&src1Host, inFileSize);
  aclrtMallocHost((void **)&dstHost, outFileSize);
  aclrtMalloc((void **)&src0Device, inFileSize, ACL_MEM_MALLOC_HUGE_FIRST);
  aclrtMalloc((void **)&src1Device, inFileSize, ACL_MEM_MALLOC_HUGE_FIRST);
  aclrtMalloc((void **)&dstDevice, outFileSize, ACL_MEM_MALLOC_HUGE_FIRST);

  size_t input1Size = inFileSize;
  if (!ReadFile((caseDir + "/input1.bin").c_str(), input1Size, src0Host,
                inFileSize)) {
    std::fprintf(stderr, "[ERROR] failed to read %s/input1.bin\n", caseDir.c_str());
    rc = 1;
  }
  size_t input2Size = inFileSize;
  if (rc == 0 &&
      !ReadFile((caseDir + "/input2.bin").c_str(), input2Size, src1Host,
                inFileSize)) {
    std::fprintf(stderr, "[ERROR] failed to read %s/input2.bin\n", caseDir.c_str());
    rc = 1;
  }

  if (rc == 0) {
    aclrtMemcpy(src0Device, inFileSize, src0Host, inFileSize,
                ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(src1Device, inFileSize, src1Host, inFileSize,
                ACL_MEMCPY_HOST_TO_DEVICE);
    tc.launch(src0Device, src1Device, dstDevice, stream);
    aclrtSynchronizeStream(stream);
    aclrtMemcpy(dstHost, outFileSize, dstDevice, outFileSize,
                ACL_MEMCPY_DEVICE_TO_HOST);
  }

  if (rc == 0 &&
      !WriteFile((caseDir + "/output.bin").c_str(), dstHost, outFileSize)) {
    std::fprintf(stderr, "[ERROR] failed to write %s/output.bin\n", caseDir.c_str());
    rc = 1;
  }

  if (src0Device)
    aclrtFree(src0Device);
  if (src1Device)
    aclrtFree(src1Device);
  if (dstDevice)
    aclrtFree(dstDevice);
  if (src0Host)
    aclrtFreeHost(src0Host);
  if (src1Host)
    aclrtFreeHost(src1Host);
  if (dstHost)
    aclrtFreeHost(dstHost);
  return rc;
}

int main(int argc, char *argv[]) {
  const char *caseFilter = (argc > 1) ? argv[1] : nullptr;

  int rc = 0;
  int deviceId = 0;
  aclrtStream stream = nullptr;

  aclInit(nullptr);
  if (const char *envDevice = std::getenv("ACL_DEVICE_ID"))
    deviceId = std::atoi(envDevice);
  aclrtSetDevice(deviceId);
  aclrtCreateStream(&stream);

  for (size_t i = 0; i < kNumCases; ++i) {
    if (caseFilter && std::strcmp(kCases[i].name, caseFilter) != 0)
      continue;
    if (RunCase(kCases[i], stream) != 0) {
      rc = 1;
      break;
    }
    std::printf("[INFO] case %s done\n", kCases[i].name);
  }

  if (stream)
    aclrtDestroyStream(stream);
  aclrtResetDevice(deviceId);
  aclFinalize();
  return rc;
}
