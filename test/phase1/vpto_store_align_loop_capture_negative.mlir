// RUN: bash -lc 'set +e; ptoas --pto-backend=vpto --emit-vpto %s -o - 2>&1; echo EXIT:$?' | FileCheck %s

// CHECK: error: 'pto.pstu' op align_in type must be threaded through scf.for iter_args when used inside a loop
// CHECK: EXIT:1
module {
  func.func @store_align_loop_capture_negative(
      %mask_base: !pto.ptr<ui32, ub>) attributes {pto.version_selection_applied} {
    %c0 = arith.constant 0 : index
    %c2 = arith.constant 2 : index
    %c1 = arith.constant 1 : index
    %c0_i32 = arith.constant 0 : i32
    pto.vecscope {
      %align0 = pto.init_align : !pto.align
      scf.for %iv = %c0 to %c2 step %c1 {
        %mask = pto.pset_b32 "PAT_ALL" : !pto.mask<b32>
        %next_align, %next_base = pto.pstu %align0, %mask, %mask_base
            : !pto.align, !pto.mask<b32>, !pto.ptr<ui32, ub> -> !pto.align, !pto.ptr<ui32, ub>
        pto.vstas %next_align, %next_base, %c0_i32 : !pto.align, !pto.ptr<ui32, ub>, i32
      }
    }
    return
  }
}
