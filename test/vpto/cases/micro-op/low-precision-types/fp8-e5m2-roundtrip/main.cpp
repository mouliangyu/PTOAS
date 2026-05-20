// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

#include "test_common.h"
#include "acl/acl.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>

using namespace PtoTestCommon;

#ifndef TMRGSORT_HPP
struct MrgSortExecutedNumList {
  uint16_t mrgSortList0;
  uint16_t mrgSortList1;
  uint16_t mrgSortList2;
  uint16_t mrgSortList3;
};
#endif

#define ACL_CHECK(expr)                                                                          \
  do {                                                                                           \
    const aclError _ret = (expr);                                                                \
    if (_ret != ACL_SUCCESS) {                                                                   \
      std::fprintf(stderr, "[ERROR] %s failed: %d (%s:%d)\n", #expr, (int)_ret, __FILE__, __LINE__); \
      const char *_recent = aclGetRecentErrMsg();                                                \
      if (_recent != nullptr && _recent[0] != '\0') {                                            \
        std::fprintf(stderr, "[ERROR] RecentErrMsg: %s\n", _recent);                             \
      }                                                                                          \
      rc = 1;                                                                                    \
      goto cleanup;                                                                              \
    }                                                                                            \
  } while (0)

void LaunchLow_precision_fp8_e5m2_roundtrip(uint8_t *v1, uint8_t *v2, void *stream);

int main() {
  constexpr size_t kBytes = 256;
  size_t fileSizeV1 = kBytes;
  size_t fileSizeV2 = kBytes;
  uint8_t *v1Host = nullptr;
  uint8_t *v1Device = nullptr;
  uint8_t *v2Host = nullptr;
  uint8_t *v2Device = nullptr;

  int rc = 0;
  bool aclInited = false;
  bool deviceSet = false;
  int deviceId = 0;
  aclrtStream stream = nullptr;

  ACL_CHECK(aclInit(nullptr));
  aclInited = true;
  if (const char *envDevice = std::getenv("ACL_DEVICE_ID")) {
    deviceId = std::atoi(envDevice);
  }
  ACL_CHECK(aclrtSetDevice(deviceId));
  deviceSet = true;
  ACL_CHECK(aclrtCreateStream(&stream));

  ACL_CHECK(aclrtMallocHost((void **)(&v1Host), kBytes));
  ACL_CHECK(aclrtMallocHost((void **)(&v2Host), kBytes));
  ACL_CHECK(aclrtMalloc((void **)&v1Device, kBytes, ACL_MEM_MALLOC_HUGE_FIRST));
  ACL_CHECK(aclrtMalloc((void **)&v2Device, kBytes, ACL_MEM_MALLOC_HUGE_FIRST));

  ReadFile("./v1.bin", fileSizeV1, v1Host, kBytes);
  ReadFile("./v2.bin", fileSizeV2, v2Host, kBytes);
  ACL_CHECK(aclrtMemcpy(v1Device, kBytes, v1Host, kBytes, ACL_MEMCPY_HOST_TO_DEVICE));
  ACL_CHECK(aclrtMemcpy(v2Device, kBytes, v2Host, kBytes, ACL_MEMCPY_HOST_TO_DEVICE));

  LaunchLow_precision_fp8_e5m2_roundtrip(v1Device, v2Device, stream);

  ACL_CHECK(aclrtSynchronizeStream(stream));
  ACL_CHECK(aclrtMemcpy(v2Host, kBytes, v2Device, kBytes, ACL_MEMCPY_DEVICE_TO_HOST));

  WriteFile("./v2.bin", v2Host, kBytes);

cleanup:
  aclrtFree(v1Device);
  aclrtFree(v2Device);
  aclrtFreeHost(v1Host);
  aclrtFreeHost(v2Host);
  if (stream != nullptr) {
    const aclError _ret = aclrtDestroyStream(stream);
    if (_ret != ACL_SUCCESS) {
      std::fprintf(stderr, "[ERROR] %s failed: %d (%s:%d)\n",
                   "aclrtDestroyStream(stream)", (int)_ret, __FILE__, __LINE__);
    }
    stream = nullptr;
  }
  if (deviceSet) {
    const aclError _ret = aclrtResetDevice(deviceId);
    if (_ret != ACL_SUCCESS) {
      std::fprintf(stderr, "[ERROR] %s failed: %d (%s:%d)\n",
                   "aclrtResetDevice(deviceId)", (int)_ret, __FILE__, __LINE__);
    }
  }
  if (aclInited) {
    const aclError _ret = aclFinalize();
    if (_ret != ACL_SUCCESS) {
      std::fprintf(stderr, "[ERROR] %s failed: %d (%s:%d)\n",
                   "aclFinalize()", (int)_ret, __FILE__, __LINE__);
    }
  }

  return rc;
}
