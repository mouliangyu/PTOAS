# VPTO Host Validation

`test/vpto` provides an end-to-end A5 validation path for hand-curated VPTO
cases whose input `kernel.pto` is already VPTO MLIR.

The driver script is:

```bash
bash test/vpto/scripts/run_host_vpto_validation.sh
```

## Required Environment

Source the CANN environment first so `ASCEND_HOME_PATH` and the toolchain are
available:

```bash
source /usr/local/Ascend/cann-9.0.0/set_env.sh
export WORK_SPACE=/path/to/workspace
export PTOAS_BIN=$PWD/build/tools/ptoas/ptoas
```

`test/vpto` uses the installed PTO headers under `${ASCEND_HOME_PATH}/include`.
It does not require sourcing the repo-local `env.sh`.

Optional overrides:

```bash
export PTOAS_FLAGS="--pto-arch a5"
export VPTO_FLAGS="--pto-backend=vpto --vpto-emit-hivm-llvm"
export AICORE_ARCH=dav-c310-vec
export HOST_RUNNER="ssh root@localhost"
export CASE_NAME=tileop/abs
export DEVICE=SIM
export SIM_LIB_DIR=/path/to/camodel/lib
```

When `DEVICE=SIM`, the host executable must be linked and run against the
simulator runtime directory that contains `libruntime_camodel.so` and its
companions such as `libnpu_drv_camodel.so`. Set `SIM_LIB_DIR` explicitly to the
matching CANN simulator `lib` directory for your target environment.

Example (recommended in current environment):

```bash
export DEVICE=SIM
export SIM_LIB_DIR=${ASCEND_HOME_PATH}/aarch64-linux/simulator/dav_3510/lib
```

If `SIM_LIB_DIR` points to an incompatible simulator model directory, step 6 may
fail early with errors such as `aclrtSetDevice(deviceId) failed: 507033`.
Use `dav_3510/lib` for this repo's current A5 VPTO validation flow unless your
target environment explicitly requires a different simulator package.

The runner can auto-select
`${ASCEND_HOME_PATH}/aarch64-linux/simulator/dav_3510/lib` when `DEVICE=SIM`
and `SIM_LIB_DIR` is unset, but explicit export is still recommended for
reproducibility across hosts.

On the current machine, `${ASCEND_HOME_PATH}/aarch64-linux/simulator/dav_3510/lib`
is the verified SIM smoke baseline. Using `dav_3102/lib` can fail at
`aclrtSetDevice` before the testcase itself runs.

## Case Discovery

The runner automatically discovers every leaf case directory under
`test/vpto/cases/`. A leaf case directory is any directory that contains:

- `kernel.pto`
- `stub.cpp`
- `launch.cpp`
- `main.cpp`
- `golden.py`
- `compare.py`

- If `CASE_NAME` is unset, all cases are run.
- If `CASE_NAME=<relative-path>` is set, only `test/vpto/cases/<relative-path>/`
  is run.

## Case Layout

Each case directory must use these fixed file names:

```text
test/vpto/cases/tileop/<case-name>/
  kernel.pto
  stub.cpp
  launch.cpp
  main.cpp
  golden.py
  compare.py
```

The runner also supports grouped layouts such as:

```text
test/vpto/cases/micro-op/<family>/<case-name>/
  kernel.pto
  stub.cpp
  launch.cpp
  main.cpp
  golden.py
  compare.py
```

In that layout, set `CASE_NAME=micro-op/<family>/<case-name>` to run a single
case.

Current top-level layout:

- `test/vpto/cases/tileop/`: tile-level or derived combination validations
- `test/vpto/cases/micro-op/<family>/`: VPTO micro-op single-op validations

File roles:

- `kernel.pto`: VPTO MLIR input consumed by `ptoas`
- `stub.cpp`: host-side fatobj stub that exports the final kernel symbol
- `launch.cpp`: kernel launch wrapper
- `main.cpp`: host executable entry for validation
- `golden.py`: generates testcase inputs and expected outputs
- `compare.py`: compares runtime outputs against golden data

## Flow

For each case, the runner performs:

1. Lower `kernel.pto` to LLVM IR with `ptoas`
2. Compile LLVM IR to device object with `bisheng`
3. Build `launch.cpp` and `stub.cpp`
4. Repack and link the kernel `.so`
5. Build the host executable and generate golden data
6. Run validation on the configured host and compare outputs

## Usage

Run all cases:

```bash
source /usr/local/Ascend/cann-9.0.0/set_env.sh
export WORK_SPACE=$(mktemp -d /tmp/vpto.XXXXXX)
export PTOAS_BIN=$PWD/build/tools/ptoas/ptoas
bash test/vpto/scripts/run_host_vpto_validation.sh
```

Run a single case:

```bash
source /usr/local/Ascend/cann-9.0.0/set_env.sh
export WORK_SPACE=$(mktemp -d /tmp/vpto-abs.XXXXXX)
export PTOAS_BIN=$PWD/build/tools/ptoas/ptoas
export CASE_NAME=tileop/abs
bash test/vpto/scripts/run_host_vpto_validation.sh
```

Run a single grouped case:

```bash
source /usr/local/Ascend/cann-9.0.0/set_env.sh
export WORK_SPACE=$(mktemp -d /tmp/vpto-grouped.XXXXXX)
export PTOAS_BIN=$PWD/build/tools/ptoas/ptoas
export CASE_NAME=micro-op/binary-vector/vadd
bash test/vpto/scripts/run_host_vpto_validation.sh
```

Run a single case on simulator:

```bash
source /usr/local/Ascend/cann-9.0.0/set_env.sh
export WORK_SPACE=$(mktemp -d /tmp/vpto-abs-sim.XXXXXX)
export PTOAS_BIN=$PWD/build/tools/ptoas/ptoas
export CASE_NAME=tileop/abs
export DEVICE=SIM
export SIM_LIB_DIR=${ASCEND_HOME_PATH}/aarch64-linux/simulator/dav_3510/lib
bash test/vpto/scripts/run_host_vpto_validation.sh
```
