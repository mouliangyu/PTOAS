// RUN: ptoas --pto-backend=vpto --emit-vpto %s -o - 2>/dev/null | FileCheck %s --check-prefix=IR
// RUN: ptoas --pto-arch=a5 --pto-backend=vpto --vpto-emit-hivm-llvm %s -o - 2>/dev/null | FileCheck %s --check-prefix=LLVM

// IR-LABEL: func.func @vdup_scalar(
// IR: %[[ALL:.+]] = pto.pset_b32 "PAT_ALL" : !pto.mask<b32>
// IR: %[[ACTIVE:.+]], %[[NEXT:.+]] = pto.plt_b32 %{{.+}} : i32 -> !pto.mask<b32>, i32
// IR: %[[DUP:.+]] = pto.vdup %{{.+}}, %[[ACTIVE]] : f32, !pto.mask<b32> -> !pto.vreg<64xf32>
// IR: pto.vsts %[[DUP]], %arg1[%arg2], %[[ALL]] : !pto.vreg<64xf32>, !pto.ptr<f32, ub>, !pto.mask<b32>

// IR-LABEL: func.func @vdup_lane_highest(
// IR: %[[SRC:.+]] = pto.vlds %arg0[%arg2] : !pto.ptr<f32, ub> -> !pto.vreg<64xf32>
// IR: %[[DUP:.+]] = pto.vdup %[[SRC]], %{{.+}} {position = "HIGHEST"} : !pto.vreg<64xf32>, !pto.mask<b32> -> !pto.vreg<64xf32>

// IR-LABEL: func.func @vdup_lane_lowest(
// IR: %[[SRC:.+]] = pto.vlds %arg0[%arg2] : !pto.ptr<f32, ub> -> !pto.vreg<64xf32>
// IR: %[[DUP:.+]] = pto.vdup %[[SRC]], %{{.+}} : !pto.vreg<64xf32>, !pto.mask<b32> -> !pto.vreg<64xf32>

// IR-LABEL: func.func @vdup_scalar_f16(
// IR: %[[ALL:.+]] = pto.pset_b16 "PAT_ALL" : !pto.mask<b16>
// IR: %[[ACTIVE:.+]], %[[NEXT:.+]] = pto.plt_b16 %{{.+}} : i32 -> !pto.mask<b16>, i32
// IR: %[[DUP:.+]] = pto.vdup %{{.+}}, %[[ACTIVE]] : f16, !pto.mask<b16> -> !pto.vreg<128xf16>
// IR: pto.vsts %[[DUP]], %arg1[%arg2], %[[ALL]] : !pto.vreg<128xf16>, !pto.ptr<f16, ub>, !pto.mask<b16>

// IR-LABEL: func.func @vdup_scalar_i8(
// IR: %[[ALL:.+]] = pto.pset_b8 "PAT_ALL" : !pto.mask<b8>
// IR: %[[ACTIVE:.+]], %[[NEXT:.+]] = pto.plt_b8 %{{.+}} : i32 -> !pto.mask<b8>, i32
// IR: %[[DUP:.+]] = pto.vdup %{{.+}}, %[[ACTIVE]] : i8, !pto.mask<b8> -> !pto.vreg<256xi8>
// IR: pto.vsts %[[DUP]], %arg1[%arg2], %[[ALL]] : !pto.vreg<256xi8>, !pto.ptr<i8, ub>, !pto.mask<b8>

// LLVM-LABEL: define{{.*}} @vdup_scalar(
// LLVM: call <64 x float> @llvm.hivm.vdups.v64f32.z(
// LLVM-LABEL: define{{.*}} @vdup_lane_highest(
// LLVM: call <64 x float> @llvm.hivm.vdupm.v64f32.z(
// LLVM-LABEL: define{{.*}} @vdup_lane_lowest(
// LLVM: call <64 x float> @llvm.hivm.vdup.v64f32.z(
// LLVM-LABEL: define{{.*}} @vdup_scalar_f16(
// LLVM: call <128 x half> @llvm.hivm.vdups.v128f16.z(
// LLVM-LABEL: define{{.*}} @vdup_scalar_i8(
// LLVM: call <256 x i8> @llvm.hivm.vdups.v256s8.z(

module {
  func.func @vdup_scalar(
      %src: !pto.ptr<f32, ub>,
      %dst: !pto.ptr<f32, ub>,
      %idx: index) attributes {pto.version_selection_applied} {
    %c13 = arith.constant 13 : i32
    %scalar = arith.constant 7.250000e+00 : f32
    pto.vecscope {
      %all = pto.pset_b32 "PAT_ALL" : !pto.mask<b32>
      %active, %next = pto.plt_b32 %c13 : i32 -> !pto.mask<b32>, i32
      %dup = pto.vdup %scalar, %active : f32, !pto.mask<b32> -> !pto.vreg<64xf32>
      pto.vsts %dup, %dst[%idx], %all : !pto.vreg<64xf32>, !pto.ptr<f32, ub>, !pto.mask<b32>
    }
    return
  }

  func.func @vdup_lane_highest(
      %src: !pto.ptr<f32, ub>,
      %dst: !pto.ptr<f32, ub>,
      %idx: index) attributes {pto.version_selection_applied} {
    %c13 = arith.constant 13 : i32
    pto.vecscope {
      %all = pto.pset_b32 "PAT_ALL" : !pto.mask<b32>
      %active, %next = pto.plt_b32 %c13 : i32 -> !pto.mask<b32>, i32
      %src_vec = pto.vlds %src[%idx] : !pto.ptr<f32, ub> -> !pto.vreg<64xf32>
      %dup = pto.vdup %src_vec, %active {position = "HIGHEST"} : !pto.vreg<64xf32>, !pto.mask<b32> -> !pto.vreg<64xf32>
      pto.vsts %dup, %dst[%idx], %all : !pto.vreg<64xf32>, !pto.ptr<f32, ub>, !pto.mask<b32>
    }
    return
  }

  func.func @vdup_lane_lowest(
      %src: !pto.ptr<f32, ub>,
      %dst: !pto.ptr<f32, ub>,
      %idx: index) attributes {pto.version_selection_applied} {
    %c13 = arith.constant 13 : i32
    pto.vecscope {
      %all = pto.pset_b32 "PAT_ALL" : !pto.mask<b32>
      %active, %next = pto.plt_b32 %c13 : i32 -> !pto.mask<b32>, i32
      %src_vec = pto.vlds %src[%idx] : !pto.ptr<f32, ub> -> !pto.vreg<64xf32>
      %dup = pto.vdup %src_vec, %active : !pto.vreg<64xf32>, !pto.mask<b32> -> !pto.vreg<64xf32>
      pto.vsts %dup, %dst[%idx], %all : !pto.vreg<64xf32>, !pto.ptr<f32, ub>, !pto.mask<b32>
    }
    return
  }

  func.func @vdup_scalar_f16(
      %src: !pto.ptr<f16, ub>,
      %dst: !pto.ptr<f16, ub>,
      %idx: index) attributes {pto.version_selection_applied} {
    %c29 = arith.constant 29 : i32
    %scalar = arith.constant 1.250000e+00 : f16
    pto.vecscope {
      %all = pto.pset_b16 "PAT_ALL" : !pto.mask<b16>
      %active, %next = pto.plt_b16 %c29 : i32 -> !pto.mask<b16>, i32
      %dup = pto.vdup %scalar, %active : f16, !pto.mask<b16> -> !pto.vreg<128xf16>
      pto.vsts %dup, %dst[%idx], %all : !pto.vreg<128xf16>, !pto.ptr<f16, ub>, !pto.mask<b16>
    }
    return
  }

  func.func @vdup_scalar_i8(
      %src: !pto.ptr<i8, ub>,
      %dst: !pto.ptr<i8, ub>,
      %idx: index) attributes {pto.version_selection_applied} {
    %c97 = arith.constant 97 : i32
    %scalar = arith.constant -83 : i8
    pto.vecscope {
      %all = pto.pset_b8 "PAT_ALL" : !pto.mask<b8>
      %active, %next = pto.plt_b8 %c97 : i32 -> !pto.mask<b8>, i32
      %dup = pto.vdup %scalar, %active : i8, !pto.mask<b8> -> !pto.vreg<256xi8>
      pto.vsts %dup, %dst[%idx], %all : !pto.vreg<256xi8>, !pto.ptr<i8, ub>, !pto.mask<b8>
    }
    return
  }
}
