// RUN: bash -lc 'set +e; ptoas --pto-backend=vpto --emit-vpto %s -o - 2>&1; echo EXIT:$?' | FileCheck %s

// CHECK: error: 'pto.pstu' op align_in type must be produced by pto.init_align or a prior store-state op, got pto.vldas
// CHECK: EXIT:1
module {
  func.func @store_align_chain_negative(
      %src: !pto.ptr<f32, ub>,
      %mask_base: !pto.ptr<ui32, ub>) attributes {pto.version_selection_applied} {
    %c0_i32 = arith.constant 0 : i32
    pto.vecscope {
      %mask = pto.pset_b32 "PAT_ALL" : !pto.mask<b32>
      %align = pto.vldas %src : !pto.ptr<f32, ub> -> !pto.align
      %next_align, %next_base = pto.pstu %align, %mask, %mask_base : !pto.align, !pto.mask<b32>, !pto.ptr<ui32, ub> -> !pto.align, !pto.ptr<ui32, ub>
      pto.vstas %next_align, %next_base, %c0_i32 : !pto.align, !pto.ptr<ui32, ub>, i32
    }
    return
  }
}
