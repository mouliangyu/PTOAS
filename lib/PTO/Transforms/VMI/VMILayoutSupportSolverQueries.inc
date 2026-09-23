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

FailureOr<SmallVector<VMICastLayoutFact, mlir::pto::kValue4>>
VMILayoutSupport::getCastLayoutFacts(VMIVRegType sourceType,
                                     VMIVRegType resultType,
                                     std::string *reason) const {
  auto fail = [&](const Twine &message)
      -> FailureOr<SmallVector<VMICastLayoutFact, 4>> {
    if (reason) {
      *reason = message.str();
    }
    return failure();
  };

  auto [sourceBits, resultBits] = getCastElementBits(sourceType, resultType);
  MLIRContext *ctx = sourceType.getContext();

  // A group-slot cast carries its group count on the type; the enumeration has
  // no single assigned layout to read it from, so it comes from whichever side
  // is a group-slot layout.  Upstream's per-layout entry point derives the same
  // count from its port layout, which is a materialized source layout here.
  int64_t numGroups = 0;
  if (VMILayoutAttr sourceLayout = sourceType.getLayoutAttr();
      sourceLayout && sourceLayout.isGroupSlots()) {
    numGroups = sourceLayout.getNumGroups();
  }
  if (VMILayoutAttr resultLayout = resultType.getLayoutAttr();
      resultLayout && resultLayout.isGroupSlots()) {
    if (numGroups != 0 && numGroups != resultLayout.getNumGroups()) {
      return fail("source/result group-slot layouts require matching "
                  "num_groups");
    }
    numGroups = resultLayout.getNumGroups();
  }

  // The shared funnel filters by port layout, so drive it once per source
  // layout this width pair can materialize.  Two rows that agree on the source
  // layout are answered together by one call, so no relation is returned twice.
  SmallVector<VMILayoutAttr, mlir::pto::kValue4> sourceLayouts;
  for (const LegalCastLayoutPattern &pattern : kLegalCastLayoutPatterns) {
    if (!matchesElementBitsPattern(pattern.sourceBits, sourceBits) ||
        !matchesElementBitsPattern(pattern.resultBits, resultBits) ||
        !matchesCastTypeClass(pattern.typeClass, sourceType.getElementType(),
                              resultType.getElementType())) {
      continue;
    }
    VMILayoutAttr sourceLayout =
        materializeLayoutPattern(ctx, pattern.sourceLayout, numGroups);
    if (!sourceLayout ||
        llvm::any_of(sourceLayouts, [&](VMILayoutAttr seen) {
          return seen == sourceLayout;
        })) {
      continue;
    }
    sourceLayouts.push_back(sourceLayout);
  }

  SmallVector<VMICastLayoutFact, mlir::pto::kValue4> facts;
  for (VMILayoutAttr sourceLayout : sourceLayouts) {
    FailureOr<SmallVector<VMICastLayoutFact, mlir::pto::kValue4>> rowFacts =
        getCastLayoutFactsForLayout(sourceType, resultType,
                                    VMICastLayoutPort::Source, sourceLayout,
                                    /*reason=*/nullptr);
    if (failed(rowFacts)) {
      continue;
    }
    for (const VMICastLayoutFact &fact : *rowFacts) {
      // A row whose intrinsic rearrangement cost cannot be derived is not a
      // candidate for the costed planner: every candidate is priced.  This is
      // the fork's own rule (fork VMILayoutSupport.cpp:2315-2319) and it lives
      // here, in the solver-driven enumeration, never in the shared funnel,
      // where dropping a row would narrow upstream's own decisions (plan 11.5).
      FailureOr<int64_t> intrinsicCost = getCastIntrinsicRearrangementCost(
          sourceType, resultType, fact.sourceLayout, fact.resultLayout);
      if (failed(intrinsicCost)) {
        continue;
      }
      VMICastLayoutFact priced = fact;
      priced.intrinsicRearrangementCost = *intrinsicCost;
      facts.push_back(priced);
    }
  }

  if (facts.empty()) {
    return fail("requires a legal cast relation for the source/result types");
  }
  return facts;
}

FailureOr<VMICastLayoutFact> VMILayoutSupport::getSameWidthCastLayoutFact(
    VMIVRegType sourceType, VMIVRegType resultType, std::string *reason) const {
  auto fail = [&](const Twine &message) -> FailureOr<VMICastLayoutFact> {
    if (reason) {
      *reason = message.str();
    }
    return failure();
  };
  VMILayoutAttr sourceLayout = sourceType.getLayoutAttr();
  VMILayoutAttr resultLayout = resultType.getLayoutAttr();
  if (!sourceLayout || !resultLayout) {
    return fail("same-width cast requires assigned source/result layouts");
  }
  auto [sourceBits, resultBits] = getCastElementBits(sourceType, resultType);
  if (sourceBits == 0 || sourceBits != resultBits) {
    return fail("same-width cast requires equal known storage widths");
  }
  if (sourceLayout != resultLayout) {
    return fail("same-width cast requires matching source/result layouts");
  }
  FailureOr<int64_t> sourceArity = getVMIPhysicalArity(sourceType);
  FailureOr<int64_t> resultArity = getVMIPhysicalArity(resultType);
  if (failed(sourceArity) || failed(resultArity) ||
      *sourceArity != *resultArity) {
    return fail("same-width cast requires matching computable physical arity");
  }
  return makeCastLayoutFact(sourceBits, resultBits, sourceLayout, resultLayout);
}

#endif // VMILAYOUTSUPPORTSOLVERQUERIES_INC
