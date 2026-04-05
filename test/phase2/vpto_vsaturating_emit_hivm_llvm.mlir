// RUN: ptoas --pto-arch=a5 --pto-backend=vpto --vpto-emit-hivm-llvm %s -o - 2>/dev/null | FileCheck %s

// CHECK-LABEL: define{{.*}} @vsadd_s16(
// CHECK: call <128 x i16> @llvm.hivm.vsadd.v128s16.x(
// CHECK: call void @llvm.hivm.vstsx1{{.*}}(

// CHECK-LABEL: define{{.*}} @vssub_s16(
// CHECK: call <128 x i16> @llvm.hivm.vssub.v128s16.x(
// CHECK: call void @llvm.hivm.vstsx1{{.*}}(

// CHECK-LABEL: define{{.*}} @vsadds_s16(
// CHECK: call <128 x i16> @llvm.hivm.vsadds.v128s16.x(
// CHECK: call void @llvm.hivm.vstsx1{{.*}}(

module {
  func.func @vsadd_s16(%src0: !pto.ptr<i16, ub>, %src1: !pto.ptr<i16, ub>, %dst: !pto.ptr<i16, ub>, %idx: index) attributes {pto.version_selection_applied} {
    pto.vecscope {
      %mask = pto.pset_b16 "PAT_ALL" : !pto.mask<b16>
      %lhs = pto.vlds %src0[%idx] : !pto.ptr<i16, ub> -> !pto.vreg<128xi16>
      %rhs = pto.vlds %src1[%idx] : !pto.ptr<i16, ub> -> !pto.vreg<128xi16>
      %sum = pto.vsadd %lhs, %rhs, %mask : !pto.vreg<128xi16>, !pto.vreg<128xi16>, !pto.mask<b16> -> !pto.vreg<128xi16>
      pto.vsts %sum, %dst[%idx], %mask : !pto.vreg<128xi16>, !pto.ptr<i16, ub>, !pto.mask<b16>
    }
    return
  }

  func.func @vssub_s16(%src0: !pto.ptr<i16, ub>, %src1: !pto.ptr<i16, ub>, %dst: !pto.ptr<i16, ub>, %idx: index) attributes {pto.version_selection_applied} {
    pto.vecscope {
      %mask = pto.pset_b16 "PAT_ALL" : !pto.mask<b16>
      %lhs = pto.vlds %src0[%idx] : !pto.ptr<i16, ub> -> !pto.vreg<128xi16>
      %rhs = pto.vlds %src1[%idx] : !pto.ptr<i16, ub> -> !pto.vreg<128xi16>
      %diff = pto.vssub %lhs, %rhs, %mask : !pto.vreg<128xi16>, !pto.vreg<128xi16>, !pto.mask<b16> -> !pto.vreg<128xi16>
      pto.vsts %diff, %dst[%idx], %mask : !pto.vreg<128xi16>, !pto.ptr<i16, ub>, !pto.mask<b16>
    }
    return
  }

  func.func @vsadds_s16(%src: !pto.ptr<i16, ub>, %dst: !pto.ptr<i16, ub>, %idx: index) attributes {pto.version_selection_applied} {
    %c7 = arith.constant 7 : i16
    pto.vecscope {
      %mask = pto.pset_b16 "PAT_ALL" : !pto.mask<b16>
      %input = pto.vlds %src[%idx] : !pto.ptr<i16, ub> -> !pto.vreg<128xi16>
      %sum = pto.vsadds %input, %c7, %mask : !pto.vreg<128xi16>, i16, !pto.mask<b16> -> !pto.vreg<128xi16>
      pto.vsts %sum, %dst[%idx], %mask : !pto.vreg<128xi16>, !pto.ptr<i16, ub>, !pto.mask<b16>
    }
    return
  }
}
