// RUN: not %ptoas --pto-backend=vpto --emit-vpto %s -o - 2>&1 | FileCheck %s

module attributes {pto.target_arch = "a5"} {
  func.func @bad() {
    pto.sprclr "AR"
    return
  }
}

// CHECK: must be nested under pto.vecscope/pto.strict_vecscope
