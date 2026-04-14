// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

#include "acl/acl.h"
#include "test_common.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

using namespace PtoTestCommon;

void LaunchTROWPROD_f32_32x32(float *src, float *dst, void *stream);
using LaunchFn = void (*)(float *, float *, void *);

struct TestCase {
  const char *name;
  LaunchFn launch;
  size_t inputRows;
  size_t inputCols;
  size_t outputRows;
  size_t outputCols;
};

static const TestCase kCases[] = {
    {"f32_32x32", LaunchTROWPROD_f32_32x32, 32, 32, 32, 1},
};

static int RunCase(const TestCase &tc, aclrtStream stream) {
  const size_t inputCount = tc.inputRows * tc.inputCols;
  const size_t outputCount = tc.outputRows * tc.outputCols;
  const size_t inputBytes = inputCount * sizeof(float);
  const size_t outputBytes = outputCount * sizeof(float);

  std::string caseDir = std::string("./") + tc.name;
  float *srcHost = nullptr;
  float *dstHost = nullptr;
  float *srcDevice = nullptr;
  float *dstDevice = nullptr;

  aclrtMallocHost((void **)&srcHost, inputBytes);
  aclrtMallocHost((void **)&dstHost, outputBytes);
  aclrtMalloc((void **)&srcDevice, inputBytes, ACL_MEM_MALLOC_HUGE_FIRST);
  aclrtMalloc((void **)&dstDevice, outputBytes, ACL_MEM_MALLOC_HUGE_FIRST);

  if (!ReadFile((caseDir + "/input.bin").c_str(), inputBytes, srcHost,
                inputBytes))
    return 1;

  aclrtMemcpy(srcDevice, inputBytes, srcHost, inputBytes,
              ACL_MEMCPY_HOST_TO_DEVICE);
  tc.launch(srcDevice, dstDevice, stream);
  aclrtSynchronizeStream(stream);
  aclrtMemcpy(dstHost, outputBytes, dstDevice, outputBytes,
              ACL_MEMCPY_DEVICE_TO_HOST);

  int rc =
      WriteFile((caseDir + "/output.bin").c_str(), dstHost, outputBytes) ? 0 : 1;

  if (srcDevice)
    aclrtFree(srcDevice);
  if (dstDevice)
    aclrtFree(dstDevice);
  if (srcHost)
    aclrtFreeHost(srcHost);
  if (dstHost)
    aclrtFreeHost(dstHost);
  return rc;
}

int main(int argc, char *argv[]) {
  const char *caseFilter = (argc > 1) ? argv[1] : nullptr;
  int deviceId = 0;
  aclrtStream stream = nullptr;

  aclInit(nullptr);
  aclrtSetDevice(deviceId);
  aclrtCreateStream(&stream);

  int rc = 0;
  for (const auto &tc : kCases) {
    if (caseFilter && std::strcmp(tc.name, caseFilter) != 0)
      continue;
    rc = RunCase(tc, stream);
    if (rc != 0)
      break;
  }

  if (stream)
    aclrtDestroyStream(stream);
  aclrtResetDevice(deviceId);
  aclFinalize();
  return rc;
}
