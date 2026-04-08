// RUN: bash -lc 'set +e; ptoas --pto-backend=vpto --emit-vpto %s -o - 2>&1; echo EXIT:$?' | FileCheck %s

// CHECK: error: 'pto.vldus' op align type must be threaded through scf.for iter_args when used inside a loop
// CHECK: EXIT:1
module {
  func.func @load_align_loop_capture_negative(
      %src: !pto.ptr<f32, ub>) attributes {pto.version_selection_applied} {
    %c0 = arith.constant 0 : index
    %c2 = arith.constant 2 : index
    %c1 = arith.constant 1 : index
    pto.vecscope {
      %align0 = pto.vldas %src : !pto.ptr<f32, ub> -> !pto.align
      scf.for %iv = %c0 to %c2 step %c1 {
        %v, %next_align = pto.vldus %src, %align0
            : !pto.ptr<f32, ub>, !pto.align -> !pto.vreg<64xf32>, !pto.align
      }
    }
    return
  }
}
