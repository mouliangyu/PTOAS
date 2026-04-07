// RUN: ! ptoas --pto-backend=vpto --emit-vpto %s -o /dev/null 2>&1 | FileCheck %s

// CHECK: error: 'pto.vbitsort' op must not be nested under pto.vecscope/pto.strict_vecscope; pto.vbitsort is a UB helper op rather than a vecscope op
// CHECK: Error: Failed to parse MLIR.

module {
  func.func @vbitsort_must_stay_outside_vecscope(
      %dst: !pto.ptr<ui32, ub>, %src: !pto.ptr<f32, ub>,
      %idxs: !pto.ptr<ui32, ub>) attributes {pto.version_selection_applied} {
    %c1 = arith.constant 1 : index
    pto.vecscope {
      pto.vbitsort %dst, %src, %idxs, %c1 : !pto.ptr<ui32, ub>, !pto.ptr<f32, ub>, !pto.ptr<ui32, ub>, index
    }
    return
  }
}
