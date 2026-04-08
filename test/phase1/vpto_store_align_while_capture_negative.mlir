// RUN: bash -lc 'set +e; ptoas --pto-backend=vpto --emit-vpto %s -o - 2>&1; echo EXIT:$?' | FileCheck %s --check-prefix=ERR
// RUN: ptoas --pto-backend=vpto --emit-vpto --vpto-disable-align-chain-verification %s -o - | FileCheck %s --check-prefix=OK

// ERR: error: 'pto.pstu' op found unsupported !pto.align consumer scf.while
// ERR: EXIT:1

// OK-LABEL: @store_align_while_capture_negative
// OK: scf.while
// OK: pto.pstu
module {
  func.func @store_align_while_capture_negative(
      %mask_base: !pto.ptr<ui32, ub>) attributes {pto.version_selection_applied} {
    %c0 = arith.constant 0 : index
    %c1 = arith.constant 1 : index
    %c2 = arith.constant 2 : index
    %c0_i32 = arith.constant 0 : i32
    pto.vecscope {
      %align0 = pto.init_align : !pto.align
      %iv, %final_align, %final_base = scf.while (%iter = %c0, %align = %align0, %base = %mask_base)
          : (index, !pto.align, !pto.ptr<ui32, ub>) -> (index, !pto.align, !pto.ptr<ui32, ub>) {
        %cond = arith.cmpi slt, %iter, %c2 : index
        scf.condition(%cond) %iter, %align, %base : index, !pto.align, !pto.ptr<ui32, ub>
      } do {
      ^bb0(%iter_in: index, %align_in: !pto.align, %base_in: !pto.ptr<ui32, ub>):
        %mask = pto.pset_b32 "PAT_ALL" : !pto.mask<b32>
        %next_align, %next_base = pto.pstu %align0, %mask, %mask_base
            : !pto.align, !pto.mask<b32>, !pto.ptr<ui32, ub> -> !pto.align, !pto.ptr<ui32, ub>
        %iter_next = arith.addi %iter_in, %c1 : index
        scf.yield %iter_next, %next_align, %next_base : index, !pto.align, !pto.ptr<ui32, ub>
      }
      pto.vstas %final_align, %final_base, %c0_i32 : !pto.align, !pto.ptr<ui32, ub>, i32
      %_ = arith.addi %iv, %c0 : index
    }
    return
  }
}
