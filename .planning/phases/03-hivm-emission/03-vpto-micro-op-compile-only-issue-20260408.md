# VPTO Micro-op Blocked Cases (2026-04-08)

## Summary

- Scope: `test/vpto/cases/micro-op`
- Blocked source:
  `/.planning/phases/03-hivm-emission/03-vpto-op-board-unit-tests-matrix.md`
- Current blocked count: `48`
- Latest compile-only sweep:
  - mode: `DEVICE=SIM COMPILE_ONLY=1`
  - result: `235 total / 233 pass / 2 fail`
  - note: the two compile failures are only one subset of the blocked set, both under the unaligned-memory/stateful-stream category
- Verified full SIM run path:
  - `2026-04-08` single-case rerun passed with `DEVICE=SIM`
  - case: `micro-op/vector-load-store/vldsx2-layout-check`
  - `SIM_LIB_DIR=${ASCEND_HOME_PATH}/aarch64-linux/simulator/dav_3510/lib`
  - result: script ran to step `6/6`, simulator executed, `compare.py` reported `compare passed`

## Repro

### Full micro-op SIM sweep

```bash
source scripts/ptoas_env.sh
WORK_SPACE=$PWD/.work/micro-op-sim-20260408 \
DEVICE=SIM \
CASE_PREFIX='micro-op/' \
JOBS=64 \
SIM_LIB_DIR="${ASCEND_HOME_PATH}/aarch64-linux/simulator/dav_3510/lib" \
bash test/vpto/scripts/run_host_vpto_validation_parallel.sh
```

### Run one case

```bash
source scripts/ptoas_env.sh
WORK_SPACE=$PWD/.work/single-case \
DEVICE=SIM \
SIM_LIB_DIR="${ASCEND_HOME_PATH}/aarch64-linux/simulator/dav_3510/lib" \
CASE_NAME='micro-op/vector-load-store/vldsx2-layout-check' \
bash test/vpto/scripts/run_host_vpto_validation.sh
```

Replace `CASE_NAME` with any case listed below.

## Blocked Case Groups

### 1. Unaligned memory / stateful stream

- `micro-op/predicate-load-store/pstu`
- `micro-op/predicate-load-store/pstu-state-advance-boundary`
- `micro-op/vector-load-store/vldas-vldus-state-chain`
- `micro-op/vector-load-store/vstar`
- `micro-op/vector-load-store/vstur`
- `micro-op/vector-load-store/vstas-vstus-offset-update`

Notes:
- this group contains the only two current compile-only failures:
  - `micro-op/predicate-load-store/pstu`
  - `micro-op/predicate-load-store/pstu-state-advance-boundary`
- latest compile-only failure shape:
  - `bisheng` crashes in `HiIPU Non VF DAG->DAG Pattern Instruction Selection`
  - current emitted intrinsics:
    - `@llvm.hivm.pstu.b32(<256 x i1>, ptr addrspace(6), <32 x i8>)`
    - `@llvm.hivm.pstu.b16(<256 x i1>, ptr addrspace(6), <32 x i8>)`

### 2. Unsupported Instr/Type in SIM/runtime

- `micro-op/binary-vector/vsadd`
- `micro-op/binary-vector/vssub`
- `micro-op/vec-scalar/vsadds`
- `micro-op/unary-vector/vrsqrt`
- `micro-op/unary-vector/vrsqrt-zero-inf`
- `micro-op/unary-vector/vrec`
- `micro-op/unary-vector/vrec-zero-inf`
- `micro-op/unary-vector/vbcnt`
- `micro-op/unary-vector/vcls`
- `micro-op/rearrangement/vslide`
- `micro-op/rearrangement/vslide-tail-window`

### 3. Carry/borrow result is all zero

- `micro-op/binary-vector/vaddc`
- `micro-op/binary-vector/vaddc-carry-boundary`
- `micro-op/binary-vector/vsubc`
- `micro-op/binary-vector/vsubc-borrow-boundary`
- `micro-op/vec-scalar/vaddcs`
- `micro-op/vec-scalar/vaddcs-carry-boundary`
- `micro-op/vec-scalar/vsubcs`
- `micro-op/vec-scalar/vsubcs-borrow-boundary`

### 4. Gather compare mismatch

- `micro-op/gather-scatter/vgather2`
- `micro-op/gather-scatter/vgather2-duplicate-index`
- `micro-op/gather-scatter/vgather2_bc`
- `micro-op/gather-scatter/vgather2_bc-sparse-mask`
- `micro-op/gather-scatter/vgatherb`
- `micro-op/gather-scatter/vgatherb-block-boundary`

### 5. Reduction / prefix layout mismatch

- `micro-op/reduction/vcgadd`
- `micro-op/reduction/vcgadd-tail`
- `micro-op/reduction/vcgmax`
- `micro-op/reduction/vcgmax-tie`
- `micro-op/reduction/vcgmin`
- `micro-op/reduction/vcgmin-tie`
- `micro-op/reduction/vcpadd`
- `micro-op/reduction/vcpadd-tail`

### 6. Rearrangement runtime abnormal

- `micro-op/rearrangement/vsqz`
- `micro-op/rearrangement/vsqz-nontrivial-mask`

Notes:
- current observation is veccore `ISU stall`

### 7. Doc / semantic blocker

- `micro-op/compare-select/vcmp-unordered-f32`
- `micro-op/compare-select/vcmps-unordered-f32`
- `micro-op/rearrangement/vusqz`
- `micro-op/rearrangement/vusqz-nontrivial-mask`
- `micro-op/vector-load-store/vsts-pk-b16`
- `micro-op/vector-load-store/vsts-mrg2chn-b16`
- `micro-op/vector-load-store/vsts-mrg4chn-b8`

### 8. Other runtime/backend mismatch

- `micro-op/dsa-sfu/vexpdiff-f16-part`

## Compile-only Failure Artifacts

### `micro-op/predicate-load-store/pstu`

- LLVM artifact:
  `/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.work/micro-op-compile-20260408-simlib/micro-op_predicate-load-store_pstu/micro-op_predicate-load-store_pstu.ll`
- Validation log:
  `/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.work/micro-op-compile-20260408-simlib/micro-op_predicate-load-store_pstu/validation.log`

### `micro-op/predicate-load-store/pstu-state-advance-boundary`

- LLVM artifact:
  `/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.work/micro-op-compile-20260408-simlib/micro-op_predicate-load-store_pstu-state-advance-boundary/micro-op_predicate-load-store_pstu-state-advance-boundary.ll`
- Validation log:
  `/home/mouliangyu/projects/github.com/mouliangyu/PTOAS/.work/micro-op-compile-20260408-simlib/micro-op_predicate-load-store_pstu-state-advance-boundary/validation.log`
