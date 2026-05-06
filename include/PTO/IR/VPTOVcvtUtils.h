// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

#ifndef PTO_IR_VPTOVCVTUTILS_H
#define PTO_IR_VPTOVCVTUTILS_H

#include "mlir/IR/BuiltinTypes.h"
#include "llvm/ADT/StringRef.h"

#include <optional>

namespace mlir::pto {

enum class VcvtElemKind {
  Invalid,
  F16,
  BF16,
  F32,
  F8E4M3FN,
  F8E5M2,
  HiF8,
  F4E1M2x2,
  F4E2M1x2,
  S8,
  U8,
  S16,
  U16,
  S32,
  U32,
  S64,
};

struct VcvtContract {
  const char *intrinsic = nullptr;
  bool requiresRnd = false;
  bool requiresSat = false;
  bool requiresPart = false;
  unsigned maskBitWidth = 0;
  bool satBeforeRnd = false;
};

enum class VcvtPartFamily {
  EvenOdd,
  Packed4,
};

std::optional<llvm::StringRef> normalizeVcvtRoundModeToken(llvm::StringRef token);
std::optional<llvm::StringRef>
normalizeVcvtSaturationToken(llvm::StringRef token);
std::optional<llvm::StringRef> normalizeVcvtPartToken(llvm::StringRef token);

VcvtElemKind classifyVcvtElemType(Type type);
std::optional<unsigned> getVcvtElemBitWidth(VcvtElemKind kind);
std::optional<VcvtPartFamily> classifyVcvtPartFamily(unsigned srcBits,
                                                      unsigned dstBits);
bool isValidVcvtPartForFamily(llvm::StringRef part, VcvtPartFamily family);
std::optional<VcvtContract> lookupVcvtContract(VcvtElemKind src,
                                               VcvtElemKind dst);

} // namespace mlir::pto

#endif // PTO_IR_VPTOVCVTUTILS_H
