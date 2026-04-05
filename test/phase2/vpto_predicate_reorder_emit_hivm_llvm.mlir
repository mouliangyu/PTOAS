// RUN: ptoas --pto-arch=a5 --pto-backend=vpto --vpto-emit-hivm-llvm %s -o - 2>/dev/null | FileCheck %s

// CHECK-LABEL: define{{.*}} @predicate_reorder(
// CHECK: call { <256 x i1>, <256 x i1> } @llvm.hivm.pdintlv.b8
// CHECK: call { <256 x i1>, <256 x i1> } @llvm.hivm.pdintlv.b16
// CHECK: call { <256 x i1>, <256 x i1> } @llvm.hivm.pdintlv.b32
// CHECK: call { <256 x i1>, <256 x i1> } @llvm.hivm.pintlv.b8
// CHECK: call { <256 x i1>, <256 x i1> } @llvm.hivm.pintlv.b16
// CHECK: call { <256 x i1>, <256 x i1> } @llvm.hivm.pintlv.b32

module {
  func.func @predicate_reorder(
      %b8_lhs: !pto.mask<b8>, %b8_rhs: !pto.mask<b8>,
      %b16_lhs: !pto.mask<b16>, %b16_rhs: !pto.mask<b16>,
      %b32_lhs: !pto.mask<b32>, %b32_rhs: !pto.mask<b32>)
      attributes {pto.version_selection_applied} {
    pto.vecscope {
      %pd8_low, %pd8_high = pto.pdintlv_b8 %b8_lhs, %b8_rhs
        : !pto.mask<b8>, !pto.mask<b8> -> !pto.mask<b8>, !pto.mask<b8>
      %pd16_low, %pd16_high = pto.pdintlv_b16 %b16_lhs, %b16_rhs
        : !pto.mask<b16>, !pto.mask<b16> -> !pto.mask<b16>, !pto.mask<b16>
      %pd32_low, %pd32_high = pto.pdintlv_b32 %b32_lhs, %b32_rhs
        : !pto.mask<b32>, !pto.mask<b32> -> !pto.mask<b32>, !pto.mask<b32>
      %pi8_low, %pi8_high = pto.pintlv_b8 %b8_lhs, %b8_rhs
        : !pto.mask<b8>, !pto.mask<b8> -> !pto.mask<b8>, !pto.mask<b8>
      %pi16_low, %pi16_high = pto.pintlv_b16 %b16_lhs, %b16_rhs
        : !pto.mask<b16>, !pto.mask<b16> -> !pto.mask<b16>, !pto.mask<b16>
      %pi32_low, %pi32_high = pto.pintlv_b32 %b32_lhs, %b32_rhs
        : !pto.mask<b32>, !pto.mask<b32> -> !pto.mask<b32>, !pto.mask<b32>
    }
    return
  }
}
