// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

//===----------------------------------------------------------------------===//
// PTOPlanFusionScope.cpp - group adjacent pto.fusion_scope into fused groups
//===----------------------------------------------------------------------===//
//
// `pto.fusion_scope` is wrapped around each expanded TileOp body by OP-Lib
// inlining (PTOInlineLibCall): one scope per TileOp. This pass merges
// adjacent scopes into fusion groups following the VMI VF Fusion rules
// (tasks/prd-vmi-vf-fusion.md US-004, docs/VMI-VF-fusion-before-after-analysis.md
// §4):
//   - F1 (same scope): fusible VMI ops may share one fusion_scope.
//   - F2 (ColMax data dependency): a ColReduce scf.for result is complete only
//     after its whole region; the reduce loop and a consumer element-wise loop
//     stay as separate for-row but may share one fusion_scope.
//   - F3 (sync break): mte_*/set_flag/wait_flag/mem_bar/pipe_barrier/vecscope/
//     unknown-effect ops close the current group.
//   - rule 1 (UB overlap): a candidate scope's UB set (alloc_tile values
//     referenced via tile_buf_addr) must be either identical to or disjoint
//     from every UB already in the group; partial overlap rejects. First
//     version is alloc-level disjoint only; unknown base provenance rejects.
//
// The pass runs in the VMI semantic pipeline, before VMILowerUnifiedToLegacy
// (so ops are still vmi.vload/vmax/...) and before PTOStripFusionScope. It
// never builds pto.vecscope; that remains PTOInferVPTOVecScope's job after
// VMIToVPTO.

#include "PTO/IR/PTO.h"
#include "PTO/Transforms/Passes.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Operation.h"
#include "mlir/Interfaces/CallInterfaces.h"
#include "mlir/Pass/Pass.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"

namespace mlir {
namespace pto {
#define GEN_PASS_DEF_PTOPLANFUSIONSCOPE
#include "PTO/Transforms/Passes.h.inc"
} // namespace pto
} // namespace mlir

using namespace mlir;
using namespace mlir::pto;

namespace {

// A UB identity after FoldTileBufIntrinsics is a `pto.pointer_cast` of a
// compile-time address into a memref<shape x dtype, vec>. Two references are
// the *same* UB iff they share the address constant value AND the memref type
// (shape + dtype); distinct addresses are disjoint at alloc level. The
// planner runs after fold, so this is the representation it must read.
struct UBIdentity {
  int64_t address = 0;
  Type memrefType;

  bool operator==(const UBIdentity &rhs) const {
    return address == rhs.address && memrefType == rhs.memrefType;
  }
};

// Resolve a `pointer_cast` address operand to a concrete int64 if it is a
// compile-time constant; return std::nullopt for unknown provenance (the
// planner then conservatively rejects merging that scope).
static std::optional<int64_t> resolveConstAddress(Value addr) {
  auto constOp = addr.getDefiningOp<arith::ConstantOp>();
  if (!constOp)
    return std::nullopt;
  if (auto iv = dyn_cast<IntegerAttr>(constOp.getValue()))
    return iv.getInt();
  return std::nullopt;
}

// Ops whose presence in a scope body closes the current fusion group (F3).
// pto.mte_gm_ub / pto.mte_ub_gm are the materialized tload/tstore DMAs.
// set_flag / wait_flag / mem_bar / pipe_barrier are syncs. vecscope /
// strict_vecscope are explicit scope carriers.
static bool isFusionBoundaryOpName(StringRef name) {
  return name == "pto.mte_gm_ub" || name == "pto.mte_ub_gm" ||
         name == "pto.set_flag" || name == "pto.wait_flag" ||
         name == "pto.mem_bar" || name == "pto.pipe_barrier" ||
         name == "pto.vecscope" || name == "pto.strict_vecscope";
}

// A scope is a sync/boundary group (F3) if its body contains any DMA/sync op
// or any op the planner cannot reason about. VMI compute ops (vload/vmax/...
// scf.for/scf.yield/arith/castptr/pointer_cast) are fine: scf.for has a region
// but is a legal compute carrier, so it is not a barrier.
static bool scopeIsFusionBoundary(FusionScopeOp scope) {
  bool boundary = false;
  scope.getBody().walk([&](Operation *op) {
    if (boundary)
      return WalkResult::interrupt();
    if (isa<scf::ForOp, scf::IfOp, scf::YieldOp>(op))
      return WalkResult::advance();
    if (op->hasTrait<OpTrait::IsTerminator>()) {
      boundary = true;
      return WalkResult::interrupt();
    }
    if (isa<CallOpInterface>(op) || !op->getRegions().empty()) {
      boundary = true;
      return WalkResult::interrupt();
    }
    if (isFusionBoundaryOpName(op->getName().getStringRef())) {
      boundary = true;
      return WalkResult::interrupt();
    }
    return WalkResult::advance();
  });
  return boundary;
}

// Collect every UB identity referenced inside a scope body, via
// `pto.pointer_cast %addr : memref<shape x dtype, vec>`.
static void collectScopeUBs(FusionScopeOp scope,
                             SmallVectorImpl<UBIdentity> &out) {
  scope.getBody().walk([&](pto::PointerCastOp pcOp) {
    if (pcOp.getOperands().empty())
      return WalkResult::advance();
    auto addr = resolveConstAddress(pcOp.getOperands()[0]);
    if (addr) {
      UBIdentity ub{*addr, pcOp.getResult().getType()};
      if (!llvm::is_contained(out, ub))
        out.push_back(ub);
    }
    return WalkResult::advance();
  });
}

// Rule 1: candidate UBs must each be identical-to or disjoint-from every UB
// already in the group. Two distinct addresses are disjoint at alloc level;
// the same address + same memref type is identical. A different memref type on
// the same address is a potential partial overlap and must reject.
static bool canMergeWithGroup(const SmallVectorImpl<UBIdentity> &candidateUBs,
                              const SmallVectorImpl<UBIdentity> &groupUBs) {
  for (const UBIdentity &ub : candidateUBs) {
    for (const UBIdentity &gub : groupUBs) {
      if (ub.address == gub.address) {
        if (ub.memrefType != gub.memrefType)
          return false; // same address, different view -> partial overlap
        continue;       // identical UB
      }
      // distinct addresses: disjoint at alloc level
    }
  }
  return true;
}

struct PTOPlanFusionScopePass
    : public mlir::pto::impl::PTOPlanFusionScopeBase<PTOPlanFusionScopePass> {
  void runOnOperation() override {
    ModuleOp module = getOperation();

    module.walk([](func::FuncOp func) {
      planInFunc(func);
    });
  }

  static void planInFunc(func::FuncOp func) {
    if (func.getBody().empty())
      return;
    Block &entry = func.getBody().front();

    FusionScopeOp currentGroup = nullptr;
    SmallVector<UBIdentity> groupUBs;

    SmallVector<FusionScopeOp, 16> scopes;
    for (Operation &op : entry.getOperations())
      if (auto fs = dyn_cast<FusionScopeOp>(op))
        scopes.push_back(fs);

    for (FusionScopeOp scope : scopes) {
      // F3: a scope containing a DMA/sync/unknown op is its own group and
      // closes any open group.
      if (scopeIsFusionBoundary(scope)) {
        currentGroup = nullptr;
        continue;
      }

      SmallVector<UBIdentity> candidateUBs;
      collectScopeUBs(scope, candidateUBs);

      if (currentGroup && canMergeWithGroup(candidateUBs, groupUBs)) {
        // Splice this scope's body into the tail of the current group scope.
        Block &dst = currentGroup.getBody().front();
        Block &src = scope.getBody().front();
        dst.getOperations().splice(dst.end(), src.getOperations(),
                                   src.getOperations().begin(),
                                   src.getOperations().end());
        scope.erase();
        for (const UBIdentity &ub : candidateUBs)
          if (!llvm::is_contained(groupUBs, ub))
            groupUBs.push_back(ub);
      } else {
        // Either no open group, or rule-1 rejected: close the group and let
        // this scope become the seed of a new one.
        currentGroup = scope;
        groupUBs.clear();
        for (const UBIdentity &ub : candidateUBs)
          groupUBs.push_back(ub);
      }
    }
  }
};

} // namespace

std::unique_ptr<Pass> mlir::pto::createPTOPlanFusionScopePass() {
  return std::make_unique<PTOPlanFusionScopePass>();
}
