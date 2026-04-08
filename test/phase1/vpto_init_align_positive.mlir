// RUN: ptoas --pto-backend=vpto --emit-vpto %s -o - | FileCheck %s

// CHECK-LABEL: @init_align_positive
// CHECK: %[[DST:.+]] = pto.castptr %arg0 : i64 -> !pto.ptr<f32, ub>
// CHECK: %[[ALIGN0:.+]] = pto.init_align : !pto.align
// CHECK: %[[V:.+]] = pto.vlds %[[DST]][%arg1] : !pto.ptr<f32, ub> -> !pto.vreg<64xf32>
// CHECK: %[[ALIGN1:.+]] = pto.vstur %[[ALIGN0]], %[[V]], %[[DST]], "NO_POST_UPDATE" : !pto.align, !pto.vreg<64xf32>, !pto.ptr<f32, ub> -> !pto.align
// CHECK: pto.vstar %[[ALIGN1]], %[[DST]] : !pto.align, !pto.ptr<f32, ub>
module {
  func.func @init_align_positive(%dst_addr: i64, %index: index) attributes {pto.version_selection_applied} {
    %dst = pto.castptr %dst_addr : i64 -> !pto.ptr<f32, ub>
    pto.vecscope {
      %align0 = pto.init_align : !pto.align
      %v = pto.vlds %dst[%index] : !pto.ptr<f32, ub> -> !pto.vreg<64xf32>
      %align1 = pto.vstur %align0, %v, %dst, "NO_POST_UPDATE"
          : !pto.align, !pto.vreg<64xf32>, !pto.ptr<f32, ub> -> !pto.align
      pto.vstar %align1, %dst : !pto.align, !pto.ptr<f32, ub>
    }
    return
  }
}
