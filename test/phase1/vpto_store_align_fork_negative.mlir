// RUN: bash -lc 'set +e; ptoas --pto-backend=vpto --emit-vpto %s -o - 2>&1; echo EXIT:$?' | FileCheck %s

// CHECK: error: 'pto.pstu' op !pto.align value must form a single linear store-state chain
// CHECK: EXIT:1
module {
  func.func @store_align_fork_negative(
      %mask_base0: !pto.ptr<ui32, ub>,
      %mask_base1: !pto.ptr<ui32, ub>) attributes {pto.version_selection_applied} {
    %c0_i32 = arith.constant 0 : i32
    pto.vecscope {
      %align0 = pto.init_align : !pto.align
      %mask = pto.pset_b32 "PAT_ALL" : !pto.mask<b32>
      %align1, %base1 = pto.pstu %align0, %mask, %mask_base0
          : !pto.align, !pto.mask<b32>, !pto.ptr<ui32, ub> -> !pto.align, !pto.ptr<ui32, ub>
      %align2, %base2 = pto.pstu %align0, %mask, %mask_base1
          : !pto.align, !pto.mask<b32>, !pto.ptr<ui32, ub> -> !pto.align, !pto.ptr<ui32, ub>
      pto.vstas %align1, %base1, %c0_i32 : !pto.align, !pto.ptr<ui32, ub>, i32
      pto.vstas %align2, %base2, %c0_i32 : !pto.align, !pto.ptr<ui32, ub>, i32
    }
    return
  }
}
