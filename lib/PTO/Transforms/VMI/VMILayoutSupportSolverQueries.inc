// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

#ifndef VMILAYOUTSUPPORTSOLVERQUERIES_INC
#define VMILAYOUTSUPPORTSOLVERQUERIES_INC

//===- VMILayoutSupportSolverQueries.inc - solver-driven queries -*- C++ -*-===//
// Textual include unit of VMILayoutSupport.cpp, included at the end of the
// translation unit so the definitions below can use the file-local table
// helpers.
//
// It holds the candidate-enumerating queries the costed layout planner drives.
// The planner picks a relation for a whole component, so it enumerates the facts
// of a table for a shape instead of validating one assigned layout; upstream's
// per-layout queries answer a different question and are left untouched.  No
// upstream pass calls anything in this unit.
//===----------------------------------------------------------------------===//

FailureOr<SmallVector<VMILoadLayoutFact, mlir::pto::kValue4>>
VMILayoutSupport::getLoadLayoutFacts(VMIVRegType resultType,
                                     std::string *reason) const {
  auto fail = [&](const Twine &message)
      -> FailureOr<SmallVector<VMILoadLayoutFact, 4>> {
    if (reason) {
      *reason = message.str();
    }
    return failure();
  };

  SmallVector<VMILoadLayoutFact, mlir::pto::kValue4> facts;
  for (const DenseMemoryLayoutPattern &pattern : kDenseLoadLayoutPatterns) {
    if (!matchesElementBitsPattern(pattern.elementBits,
                                   resultType.getElementType())) {
      continue;
    }
    if (!matchesElementCountPattern(pattern.elementCounts,
                                    resultType.getElementCount())) {
      continue;
    }
    VMILayoutAttr layout =
        materializeLayoutPattern(resultType.getContext(), pattern.layout);
    if (!layout || llvm::any_of(facts, [&](const VMILoadLayoutFact &fact) {
          return fact.resultLayout == layout;
        })) {
      continue;
    }
    facts.push_back(VMILoadLayoutFact{layout});
  }
  if (facts.empty()) {
    return fail("result type does not match a supported dense load table row");
  }
  return facts;
}

FailureOr<SmallVector<VMIStoreLayoutFact, mlir::pto::kValue4>>
VMILayoutSupport::getStoreLayoutFacts(VMIVRegType valueType,
                                      std::string *reason) const {
  auto fail = [&](const Twine &message)
      -> FailureOr<SmallVector<VMIStoreLayoutFact, 4>> {
    if (reason) {
      *reason = message.str();
    }
    return failure();
  };

  SmallVector<VMIStoreLayoutFact, mlir::pto::kValue4> facts;
  for (const DenseMemoryLayoutPattern &pattern : kDenseStoreLayoutPatterns) {
    if (!matchesElementBitsPattern(pattern.elementBits,
                                   valueType.getElementType())) {
      continue;
    }
    if (!matchesElementCountPattern(pattern.elementCounts,
                                    valueType.getElementCount())) {
      continue;
    }
    VMILayoutAttr layout =
        materializeLayoutPattern(valueType.getContext(), pattern.layout);
    if (!layout || llvm::any_of(facts, [&](const VMIStoreLayoutFact &fact) {
          return fact.valueLayout == layout;
        })) {
      continue;
    }
    facts.push_back(VMIStoreLayoutFact{layout});
  }
  if (facts.empty()) {
    return fail("value type does not match a supported dense store table row");
  }
  return facts;
}

#endif // VMILAYOUTSUPPORTSOLVERQUERIES_INC
