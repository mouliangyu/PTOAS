// RUN: ! ptoas --pto-backend=vpto --emit-vpto %s -o /dev/null 2>&1 | FileCheck %s

// CHECK: error: 'pto.plds' op requires enclosing loop structure for pto.plds lowering
// CHECK: Error: Failed to parse MLIR.

module {
  func.func @plds_requires_loop(%src: !pto.ptr<ui8, ub>)
      attributes {pto.version_selection_applied} {
    pto.vecscope {
      %c0 = arith.constant 0 : index
      %m0 = pto.plds %src[%c0], "NORM" : !pto.ptr<ui8, ub>, index -> !pto.mask<b8>
    }
    return
  }
}
