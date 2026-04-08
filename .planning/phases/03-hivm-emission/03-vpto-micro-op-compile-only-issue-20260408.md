# VPTO Micro-op Blocked Cases (2026-04-08)

## Summary

- Scope: `test/vpto/cases/micro-op`
- Blocked source:
  `/.planning/phases/03-hivm-emission/03-vpto-op-board-unit-tests-matrix.md`
- Current blocked count: `42`
- Latest compile-only sweep:
  - mode: `DEVICE=SIM COMPILE_ONLY=1`
  - result: `235 total / 233 pass / 2 fail`
  - note: the two compile failures were the historical `pstu` group; that group has since been fixed and validated with full `DEVICE=SIM` reruns
- Verified full SIM run path:
  - `2026-04-08` single-case rerun passed with `DEVICE=SIM`
  - case: `micro-op/vector-load-store/vldsx2-layout-check`
  - `SIM_LIB_DIR=${ASCEND_HOME_PATH}/aarch64-linux/simulator/dav_3510/lib`
  - result: script ran to step `6/6`, simulator executed, `compare.py` reported `compare passed`
- Group 1 status refresh:
  - `2026-04-08` the whole historical unaligned/stateful-stream group was rerun with the same SIM baseline
  - passed cases:
    - `micro-op/predicate-load-store/pstu`
    - `micro-op/predicate-load-store/pstu-state-advance-boundary`
    - `micro-op/vector-load-store/vldas-vldus-state-chain`
    - `micro-op/vector-load-store/vstur`
    - `micro-op/vector-load-store/vstas-vstus-offset-update`
  - consolidated evidence:
    - `/tmp/vpto-unaligned-rerun-20260408`
    - `/tmp/vpto-vstar-rerun-20260408`

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

### 1. Unsupported Instr/Type in SIM/runtime

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

### 2. Carry/borrow result is all zero

- `micro-op/binary-vector/vaddc`
- `micro-op/binary-vector/vaddc-carry-boundary`
- `micro-op/binary-vector/vsubc`
- `micro-op/binary-vector/vsubc-borrow-boundary`
- `micro-op/vec-scalar/vaddcs`
- `micro-op/vec-scalar/vaddcs-carry-boundary`
- `micro-op/vec-scalar/vsubcs`
- `micro-op/vec-scalar/vsubcs-borrow-boundary`

### 3. Gather compare mismatch

- `micro-op/gather-scatter/vgather2`
- `micro-op/gather-scatter/vgather2-duplicate-index`
- `micro-op/gather-scatter/vgather2_bc`
- `micro-op/gather-scatter/vgather2_bc-sparse-mask`
- `micro-op/gather-scatter/vgatherb`
- `micro-op/gather-scatter/vgatherb-block-boundary`

### 4. Reduction / prefix layout mismatch

- `micro-op/reduction/vcgadd`
- `micro-op/reduction/vcgadd-tail`
- `micro-op/reduction/vcgmax`
- `micro-op/reduction/vcgmax-tie`
- `micro-op/reduction/vcgmin`
- `micro-op/reduction/vcgmin-tie`
- `micro-op/reduction/vcpadd`
- `micro-op/reduction/vcpadd-tail`

### 5. Rearrangement runtime abnormal

- `micro-op/rearrangement/vsqz`
- `micro-op/rearrangement/vsqz-nontrivial-mask`

Notes:
- current observation is veccore `ISU stall`

### 6. Doc / semantic blocker

- `micro-op/compare-select/vcmp-unordered-f32`
- `micro-op/compare-select/vcmps-unordered-f32`
- `micro-op/rearrangement/vusqz`
- `micro-op/rearrangement/vusqz-nontrivial-mask`
- `micro-op/vector-load-store/vsts-pk-b16`
- `micro-op/vector-load-store/vsts-mrg2chn-b16`
- `micro-op/vector-load-store/vsts-mrg4chn-b8`

### 7. Other runtime/backend mismatch

- `micro-op/dsa-sfu/vexpdiff-f16-part`
