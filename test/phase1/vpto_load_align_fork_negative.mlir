// RUN: bash -lc 'set +e; ptoas --pto-backend=vpto --emit-vpto %s -o - 2>&1; echo EXIT:$?' | FileCheck %s

// CHECK: error: 'pto.vldus' op !pto.align value must form a single linear load-state chain
// CHECK: EXIT:1
module {
  func.func @load_align_fork_negative(%src: !pto.ptr<f32, ub>) attributes {pto.version_selection_applied} {
    pto.vecscope {
      %align = pto.vldas %src : !pto.ptr<f32, ub> -> !pto.align
      %v0, %align1 = pto.vldus %src, %align
          : !pto.ptr<f32, ub>, !pto.align -> !pto.vreg<64xf32>, !pto.align
      %v1, %align2 = pto.vldus %src, %align
          : !pto.ptr<f32, ub>, !pto.align -> !pto.vreg<64xf32>, !pto.align
    }
    return
  }
}
