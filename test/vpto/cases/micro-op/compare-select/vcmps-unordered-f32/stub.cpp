// -----------------------------------------------------------------------------
// case: micro-op/compare-select/vcmps-unordered-f32
// family: compare-select
// target_ops: pto.vcmps
// scenarios: core-f32, full-mask, scalar-operand, exceptional-values
// NOTE: blocked placeholder case. The current PTO surface and docs only expose
// eq/ne/lt/le/gt/ge compare modes for pto.vcmps, so a true unordered compare
// case cannot be expressed yet.
// -----------------------------------------------------------------------------
#include <pto/common/type.hpp>

#ifndef __global__
#define __global__
#endif

#ifndef __gm__
#define __gm__
#endif

extern "C" __global__ AICORE void vcmp_eq_kernel_2d(__gm__ float *v1,
                                                    __gm__ float *v2,
                                                    __gm__ unsigned char *v3) {
  (void)v1;
  (void)v2;
  (void)v3;
}
