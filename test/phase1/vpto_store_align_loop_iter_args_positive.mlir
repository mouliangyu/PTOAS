// RUN: ptoas --pto-backend=vpto --emit-vpto %s -o - | FileCheck %s

// CHECK-LABEL: @store_align_loop_iter_args_positive
// CHECK: %[[ALIGN0:.+]] = pto.init_align : !pto.align
// CHECK: %[[RES:.*]]:2 = scf.for
// CHECK: iter_args(%[[ITER_ALIGN:.+]] = %[[ALIGN0]]
// CHECK: %[[NEXT_ALIGN:.+]], %[[NEXT_BASE:.+]] = pto.pstu %[[ITER_ALIGN]], %[[MASK:.+]], %[[ITER_BASE:.+]]
// CHECK: scf.yield %[[NEXT_ALIGN]], %[[NEXT_BASE]]
// CHECK: pto.vstas %[[RES]]#0, %[[RES]]#1, %[[C0:.+]]
module {
  func.func @store_align_loop_iter_args_positive(
      %mask_base: !pto.ptr<ui32, ub>) attributes {pto.version_selection_applied} {
    %c0 = arith.constant 0 : index
    %c2 = arith.constant 2 : index
    %c1 = arith.constant 1 : index
    %c0_i32 = arith.constant 0 : i32
    pto.vecscope {
      %align0 = pto.init_align : !pto.align
      %final_align, %final_base = scf.for %iv = %c0 to %c2 step %c1
          iter_args(%align = %align0, %base = %mask_base)
          -> (!pto.align, !pto.ptr<ui32, ub>) {
        %mask = pto.pset_b32 "PAT_ALL" : !pto.mask<b32>
        %next_align, %next_base = pto.pstu %align, %mask, %base
            : !pto.align, !pto.mask<b32>, !pto.ptr<ui32, ub> -> !pto.align, !pto.ptr<ui32, ub>
        scf.yield %next_align, %next_base : !pto.align, !pto.ptr<ui32, ub>
      }
      pto.vstas %final_align, %final_base, %c0_i32 : !pto.align, !pto.ptr<ui32, ub>, i32
    }
    return
  }
}
