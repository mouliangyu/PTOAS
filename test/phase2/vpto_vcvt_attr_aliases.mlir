// RUN: ptoas --pto-arch=a5 --pto-backend=vpto --emit-vpto %s -o - 2>/dev/null | FileCheck %s

// CHECK-LABEL: func.func @vcvt_attr_aliases(
// CHECK: %[[VEC:.+]] = pto.vlds
// CHECK: %[[TR:.+]] = pto.vtrc %[[VEC]], "Z" : !pto.vreg<64xf32> -> !pto.vreg<64xf32>
// CHECK: %[[CVT:.+]] = pto.vcvt %[[TR]] {part = "ODD", rnd = "A", sat = "SAT"} : !pto.vreg<64xf32> -> !pto.vreg<128xbf16>

module {
  func.func @vcvt_attr_aliases(%src: !pto.ptr<f32, ub>, %dst: !pto.ptr<bf16, ub>, %idx: index) attributes {pto.version_selection_applied} {
    pto.vecscope {
      %mask = pto.pset_b16 "PAT_ALL" : !pto.mask<b16>
      %vec = pto.vlds %src[%idx] : !pto.ptr<f32, ub> -> !pto.vreg<64xf32>
      %tr = pto.vtrc %vec, "Z" : !pto.vreg<64xf32> -> !pto.vreg<64xf32>
      %cvt = pto.vcvt %tr {rnd = "A", sat = "SAT", part = "ODD"} : !pto.vreg<64xf32> -> !pto.vreg<128xbf16>
      pto.vsts %cvt, %dst[%idx], %mask : !pto.vreg<128xbf16>, !pto.ptr<bf16, ub>, !pto.mask<b16>
    }
    return
  }
}
