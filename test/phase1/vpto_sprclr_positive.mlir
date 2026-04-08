// RUN: %ptoas --pto-backend=vpto --emit-vpto %s -o - | FileCheck %s

module attributes {pto.target_arch = "a5"} {
  func.func @ok() {
    pto.vecscope {
      pto.sprclr "AR"
    }
    return
  }
}

// CHECK: pto.sprclr "AR"
