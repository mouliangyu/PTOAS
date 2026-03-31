// -----------------------------------------------------------------------------
// case: micro-op/compare-select/vselr
// family: compare-select
// target_ops: pto.vselr
// scenarios: core-f32, full-mask, reversed-select
// NOTE: bulk-generated coverage skeleton. Parser/verifier/lowering failure is
// still a valid test conclusion in the current coverage-first phase.
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
