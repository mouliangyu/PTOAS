// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

#include "PTO/IR/VPTOVcvtUtils.h"

#include "PTO/IR/PTO.h"

#include <algorithm>

using namespace mlir;
using namespace mlir::pto;

std::optional<llvm::StringRef>
mlir::pto::normalizeVcvtRoundModeToken(llvm::StringRef token) {
  if (token == "R" || token == "ROUND_R")
    return llvm::StringRef("R");
  if (token == "A" || token == "ROUND_A")
    return llvm::StringRef("A");
  if (token == "F" || token == "ROUND_F")
    return llvm::StringRef("F");
  if (token == "C" || token == "ROUND_C")
    return llvm::StringRef("C");
  if (token == "Z" || token == "ROUND_Z")
    return llvm::StringRef("Z");
  if (token == "O" || token == "ROUND_O")
    return llvm::StringRef("O");
  if (token == "H" || token == "ROUND_H")
    return llvm::StringRef("H");
  return std::nullopt;
}

std::optional<llvm::StringRef>
mlir::pto::normalizeVcvtSaturationToken(llvm::StringRef token) {
  if (token == "SAT" || token == "RS_ENABLE")
    return llvm::StringRef("SAT");
  if (token == "NOSAT" || token == "RS_DISABLE")
    return llvm::StringRef("NOSAT");
  return std::nullopt;
}

std::optional<llvm::StringRef>
mlir::pto::normalizeVcvtPartToken(llvm::StringRef token) {
  if (token == "EVEN" || token == "PART_EVEN")
    return llvm::StringRef("EVEN");
  if (token == "ODD" || token == "PART_ODD")
    return llvm::StringRef("ODD");
  if (token == "P0" || token == "PART_P0")
    return llvm::StringRef("P0");
  if (token == "P1" || token == "PART_P1")
    return llvm::StringRef("P1");
  if (token == "P2" || token == "PART_P2")
    return llvm::StringRef("P2");
  if (token == "P3" || token == "PART_P3")
    return llvm::StringRef("P3");
  return std::nullopt;
}

VcvtElemKind mlir::pto::classifyVcvtElemType(Type type) {
  if (type.isF16())
    return VcvtElemKind::F16;
  if (type.isBF16())
    return VcvtElemKind::BF16;
  if (type.isF32())
    return VcvtElemKind::F32;
  if (type.isFloat8E4M3FN())
    return VcvtElemKind::F8E4M3FN;
  if (type.isFloat8E5M2())
    return VcvtElemKind::F8E5M2;
  if (isa<HiF8Type>(type))
    return VcvtElemKind::HiF8;
  if (isa<F4E1M2x2Type>(type))
    return VcvtElemKind::F4E1M2x2;
  if (isa<F4E2M1x2Type>(type))
    return VcvtElemKind::F4E2M1x2;
  if (auto intType = dyn_cast<IntegerType>(type)) {
    switch (intType.getWidth()) {
    case 8:
      return intType.isUnsigned() ? VcvtElemKind::U8 : VcvtElemKind::S8;
    case 16:
      return intType.isUnsigned() ? VcvtElemKind::U16 : VcvtElemKind::S16;
    case 32:
      return intType.isUnsigned() ? VcvtElemKind::U32 : VcvtElemKind::S32;
    case 64:
      return intType.isUnsigned() ? VcvtElemKind::Invalid : VcvtElemKind::S64;
    default:
      return VcvtElemKind::Invalid;
    }
  }
  return VcvtElemKind::Invalid;
}

std::optional<unsigned> mlir::pto::getVcvtElemBitWidth(VcvtElemKind kind) {
  switch (kind) {
  case VcvtElemKind::F16:
  case VcvtElemKind::BF16:
  case VcvtElemKind::S16:
  case VcvtElemKind::U16:
    return 16;
  case VcvtElemKind::F32:
  case VcvtElemKind::S32:
  case VcvtElemKind::U32:
    return 32;
  case VcvtElemKind::F8E4M3FN:
  case VcvtElemKind::F8E5M2:
  case VcvtElemKind::HiF8:
  case VcvtElemKind::F4E1M2x2:
  case VcvtElemKind::F4E2M1x2:
  case VcvtElemKind::S8:
  case VcvtElemKind::U8:
    return 8;
  case VcvtElemKind::S64:
    return 64;
  case VcvtElemKind::Invalid:
    return std::nullopt;
  }
  return std::nullopt;
}

std::optional<VcvtPartFamily>
mlir::pto::classifyVcvtPartFamily(unsigned srcBits, unsigned dstBits) {
  unsigned largerBits = std::max(srcBits, dstBits);
  unsigned smallerBits = std::min(srcBits, dstBits);
  if (largerBits == smallerBits * 2)
    return VcvtPartFamily::EvenOdd;
  if (largerBits == smallerBits * 4)
    return VcvtPartFamily::Packed4;
  return std::nullopt;
}

bool mlir::pto::isValidVcvtPartForFamily(llvm::StringRef part,
                                         VcvtPartFamily family) {
  switch (family) {
  case VcvtPartFamily::EvenOdd:
    return part == "EVEN" || part == "ODD";
  case VcvtPartFamily::Packed4:
    return part == "P0" || part == "P1" || part == "P2" || part == "P3";
  }
  return false;
}

std::optional<VcvtContract> mlir::pto::lookupVcvtContract(VcvtElemKind src,
                                                          VcvtElemKind dst) {
  switch (src) {
  case VcvtElemKind::F32:
    switch (dst) {
    case VcvtElemKind::F16:
      return VcvtContract{"llvm.hivm.vcvtff.f322f16.x", true, true, true, 32};
    case VcvtElemKind::BF16:
      return VcvtContract{"llvm.hivm.vcvtff.f322bf16.x", true, true, true, 32};
    case VcvtElemKind::HiF8:
      return VcvtContract{"llvm.hivm.vcvtff.f322hif8.x", true, true, true, 32};
    case VcvtElemKind::S16:
      return VcvtContract{"llvm.hivm.vcvtfi.f322s16.x", true, true, true, 32};
    case VcvtElemKind::S32:
      return VcvtContract{"llvm.hivm.vcvtfi.f322s32.x", true, true, false, 32};
    case VcvtElemKind::S64:
      return VcvtContract{"llvm.hivm.vcvtfi.f322s64.x", true, true, true, 32};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::F16:
    switch (dst) {
    case VcvtElemKind::F32:
      return VcvtContract{"llvm.hivm.vcvtff.f162f32.x", false, false, true, 16};
    case VcvtElemKind::HiF8:
      return VcvtContract{"llvm.hivm.vcvtff.f162hif8.x", true, true, true, 16};
    case VcvtElemKind::S32:
      return VcvtContract{"llvm.hivm.vcvtfi.f162s32.x", true, false, true, 16};
    case VcvtElemKind::S16:
      return VcvtContract{"llvm.hivm.vcvtfi.f162s16.x", true, true, false, 16};
    case VcvtElemKind::S8:
      return VcvtContract{"llvm.hivm.vcvtfi.f162s8.x", true, true, true, 16};
    case VcvtElemKind::U8:
      return VcvtContract{"llvm.hivm.vcvtfi.f162u8.x", true, true, true, 16};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::BF16:
    switch (dst) {
    case VcvtElemKind::F16:
      return VcvtContract{"llvm.hivm.vcvtff.bf162f16.x", true, true, false, 16,
                          true};
    case VcvtElemKind::F32:
      return VcvtContract{"llvm.hivm.vcvtff.bf162f32.x", false, false, true, 16};
    case VcvtElemKind::F4E1M2x2:
      return VcvtContract{"llvm.hivm.vcvtff2.bf162f4e1m2x2.x", true, false,
                          true, 16};
    case VcvtElemKind::F4E2M1x2:
      return VcvtContract{"llvm.hivm.vcvtff2.bf162f4e2m1x2.x", true, false,
                          true, 16};
    case VcvtElemKind::S32:
      return VcvtContract{"llvm.hivm.vcvtfi.bf162s32.x", true, true, true, 16};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::F8E4M3FN:
    switch (dst) {
    case VcvtElemKind::F32:
      return VcvtContract{"llvm.hivm.vcvtff.f8e4m32f32.x", false, false, true,
                          8};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::F8E5M2:
    switch (dst) {
    case VcvtElemKind::F32:
      return VcvtContract{"llvm.hivm.vcvtff.f8e5m22f32.x", false, false, true,
                          8};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::HiF8:
    switch (dst) {
    case VcvtElemKind::F32:
      return VcvtContract{"llvm.hivm.vcvtff.hif82f32.x", false, false, true, 8};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::F4E1M2x2:
    switch (dst) {
    case VcvtElemKind::BF16:
      return VcvtContract{"llvm.hivm.vcvtff2.f4e1m2x22bf16.x", false, false,
                          true, 8};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::F4E2M1x2:
    switch (dst) {
    case VcvtElemKind::BF16:
      return VcvtContract{"llvm.hivm.vcvtff2.f4e2m1x22bf16.x", false, false,
                          true, 8};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::U8:
    switch (dst) {
    case VcvtElemKind::F16:
      return VcvtContract{"llvm.hivm.vcvtif.u82f16.x", false, false, true, 8};
    case VcvtElemKind::U16:
      return VcvtContract{"llvm.hivm.vcvtii.u82u16.x", false, false, true, 8};
    case VcvtElemKind::U32:
      return VcvtContract{"llvm.hivm.vcvtii.u82u32.x", false, false, true, 8};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::S8:
    switch (dst) {
    case VcvtElemKind::F16:
      return VcvtContract{"llvm.hivm.vcvtif.s82f16.x", false, false, true, 8};
    case VcvtElemKind::S16:
      return VcvtContract{"llvm.hivm.vcvtii.s82s16.x", false, false, true, 8};
    case VcvtElemKind::S32:
      return VcvtContract{"llvm.hivm.vcvtii.s82s32.x", false, false, true, 8};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::U16:
    switch (dst) {
    case VcvtElemKind::U8:
      return VcvtContract{"llvm.hivm.vcvtii.u162u8.x", false, true, true, 16};
    case VcvtElemKind::U32:
      return VcvtContract{"llvm.hivm.vcvtii.u162u32.x", false, false, true, 16};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::S16:
    switch (dst) {
    case VcvtElemKind::F16:
      return VcvtContract{"llvm.hivm.vcvtif.s162f16.x", true, false, false, 16};
    case VcvtElemKind::F32:
      return VcvtContract{"llvm.hivm.vcvtif.s162f32.x", false, false, true, 16};
    case VcvtElemKind::U8:
      return VcvtContract{"llvm.hivm.vcvtii.s162u8.x", false, true, true, 16};
    case VcvtElemKind::U32:
      return VcvtContract{"llvm.hivm.vcvtii.s162u32.x", false, false, true, 16};
    case VcvtElemKind::S32:
      return VcvtContract{"llvm.hivm.vcvtii.s162s32.x", false, false, true, 16};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::U32:
    switch (dst) {
    case VcvtElemKind::U8:
      return VcvtContract{"llvm.hivm.vcvtii.u322u8.x", false, true, true, 32};
    case VcvtElemKind::U16:
      return VcvtContract{"llvm.hivm.vcvtii.u322u16.x", false, true, true, 32};
    case VcvtElemKind::S16:
      return VcvtContract{"llvm.hivm.vcvtii.u322s16.x", false, true, true, 32};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::S32:
    switch (dst) {
    case VcvtElemKind::F32:
      return VcvtContract{"llvm.hivm.vcvtif.s322f32.x", true, false, false, 32};
    case VcvtElemKind::U8:
      return VcvtContract{"llvm.hivm.vcvtii.s322u8.x", false, true, true, 32};
    case VcvtElemKind::U16:
      return VcvtContract{"llvm.hivm.vcvtii.s322u16.x", false, true, true, 32};
    case VcvtElemKind::S16:
      return VcvtContract{"llvm.hivm.vcvtii.s322s16.x", false, true, true, 32};
    case VcvtElemKind::S64:
      return VcvtContract{"llvm.hivm.vcvtii.s322s64.x", false, false, true, 32};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::S64:
    switch (dst) {
    case VcvtElemKind::F32:
      return VcvtContract{"llvm.hivm.vcvtif.s642f32.x", true, false, true, 32};
    case VcvtElemKind::S32:
      return VcvtContract{"llvm.hivm.vcvtii.s642s32.x", false, true, true, 32};
    default:
      return std::nullopt;
    }
  case VcvtElemKind::Invalid:
    return std::nullopt;
  }
  return std::nullopt;
}
