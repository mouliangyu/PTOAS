// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

//===----------------------------------------------------------------------===//
// PTOStripFusionScope.cpp - Erase pto.fusion_scope regions before VMIToVPTO
//===----------------------------------------------------------------------===//
//
// `pto.fusion_scope` is wrapped around each expanded TileOp body by OP-Lib
// inlining (PTOInlineLibCall) to give fusion passes a clean region
// granularity. Right before VMIToVPTO lowers VMI to the physical VPTO layer,
// this pass erases every `pto.fusion_scope` by moving its body operations
// back into the parent block and removing the scope op. NoTerminator scopes
// make this a straightforward splice.

#include "PTO/IR/PTO.h"
#include "PTO/Transforms/Passes.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"

namespace mlir {
namespace pto {
#define GEN_PASS_DEF_PTOSTRIPFUSIONSCOPE
#include "PTO/Transforms/Passes.h.inc"
} // namespace pto
} // namespace mlir

using namespace mlir;
using namespace mlir::pto;

namespace {

// Collect every pto.fusion_scope in `root` so erasure does not invalidate
// a live iterator.
static void collectFusionScopes(Operation *root,
                                SmallVectorImpl<pto::FusionScopeOp> &out) {
  root->walk([&](pto::FusionScopeOp scope) { out.push_back(scope); });
}

struct PTOStripFusionScopePass
    : public mlir::pto::impl::PTOStripFusionScopeBase<
          PTOStripFusionScopePass> {
  void runOnOperation() override {
    ModuleOp module = getOperation();

    SmallVector<pto::FusionScopeOp, 16> scopes;
    collectFusionScopes(module, scopes);

    for (pto::FusionScopeOp scope : scopes) {
      Block *scopeBlock = &scope.getBody().front();
      Block *parentBlock = scope->getBlock();

      // Splice the scope body in place of the scope op. FusionScopeOp is
      // NoTerminator so the body has no trailing terminator to drop.
      parentBlock->getOperations().splice(
          Block::iterator(scope), scopeBlock->getOperations(),
          scopeBlock->getOperations().begin(),
          scopeBlock->getOperations().end());

      scope->erase();
    }

    // Nothing else to do: with all scopes erased the module is plain VMI IR
    // ready for VMIToVPTO.
  }
};

} // namespace

std::unique_ptr<Pass> mlir::pto::createPTOStripFusionScopePass() {
  return std::make_unique<PTOStripFusionScopePass>();
}
