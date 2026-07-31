#!/usr/bin/env bash
# topk_gate_vf — 1:1 CCE intrinsic translation of asc.py, sim build/run.
#
# Usage:
#   ./run_vf_sim.sh              # default: E=64 K=6 tile=1
#   K_E=384 K_K=9 K_TOKEN_TILE=4 USE_AABBCC=1 MISCHED=0 ./run_vf_sim.sh
#
# Outer schedule matches staged when K_TOKEN_TILE>1 (batched MTE2/MTE3).
set -euo pipefail

K_E="${K_E:-64}"
K_K="${K_K:-6}"
K_N="${K_N:-4}"
K_TOKEN_TILE="${K_TOKEN_TILE:-1}"
USE_AABBCC="${USE_AABBCC:-0}"
MISCHED="${MISCHED:-1}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build_vf_e${K_E}_k${K_K}_t${K_TOKEN_TILE}_aabb${USE_AABBCC}_ms${MISCHED}"
mkdir -p "${BUILD_DIR}"

ASCEND_HOME_PATH="${ASCEND_HOME_PATH:-/usr/local/Ascend/cann_9rel/cann-9.0.0}"
BISHENG="${ASCEND_HOME_PATH}/bin/bisheng"
if [[ ! -x "${BISHENG}" ]]; then BISHENG="$(command -v bisheng)"; fi

PTO_ISA_INC="${PTO_ISA_INC:-/home/happybot/projects/pto-isa/include}"
SIM_LIB_DIR="${SIM_LIB_DIR:-${ASCEND_HOME_PATH}/aarch64-linux/simulator/dav_3510/lib}"

if [[ ! -d "${PTO_ISA_INC}/pto" && ! -d "${PTO_ISA_INC}/PTO" ]]; then
  echo "WARN: PTO_ISA_INC=${PTO_ISA_INC} may be wrong; export PTO_ISA_INC to pto-isa/include" >&2
fi

log() { echo "[$(date +'%F %T')] $*"; }
die() { echo "ERROR: $*" >&2; exit 1; }

MISCHED_FLAG=()
if [[ "${MISCHED}" == "0" ]]; then
  MISCHED_FLAG=(-mllvm -cce-aicore-vec-misched=0)
fi

log "=== topk_gate_vf (E=${K_E} K=${K_K} N=${K_N} tile=${K_TOKEN_TILE} aabb=${USE_AABBCC} misched=${MISCHED}, SIM) ==="

log "step 1: compile topk_gate_vf.cpp"
"${BISHENG}" \
  -I"${PTO_ISA_INC}" \
  -I"${ASCEND_HOME_PATH}/include" \
  -I"${ASCEND_DRIVER_PATH:-/usr/local/Ascend/driver}/kernel/inc" \
  -I"${ASCEND_HOME_PATH}/pkg_inc" \
  -I"${ASCEND_HOME_PATH}/pkg_inc/profiling" \
  -I"${ASCEND_HOME_PATH}/pkg_inc/runtime/runtime" \
  -std=gnu++17 -O2 -Wno-macro-redefined -Wno-ignored-attributes \
  -fPIC -xcce -Xhost-start -Xhost-end \
  -mllvm -cce-aicore-stack-size=0x8000 -mllvm -cce-aicore-function-stack-size=0x8000 \
  -mllvm -cce-aicore-record-overflow=true -mllvm -cce-aicore-addr-transform \
  -mllvm -cce-aicore-dcci-insert-for-scalar=false \
  ${MISCHED_FLAG[@]+"${MISCHED_FLAG[@]}"} \
  --cce-aicore-arch=dav-c310-vec -DREGISTER_BASE \
  -DK_E=${K_E} -DK_K=${K_K} -DK_N=${K_N} -DK_TOKEN_TILE=${K_TOKEN_TILE} \
  -DUSE_AABBCC=${USE_AABBCC} \
  -c "${SCRIPT_DIR}/topk_gate_vf.cpp" -o "${BUILD_DIR}/topk_gate_vf.o" 2>&1 || die "compile kernel failed"

log "step 2: link kernel .so (camodel)"
"${BISHENG}" -fPIC -shared --cce-fatobj-link \
  -Wl,-soname,libtopk_gate_vf.so \
  "${BUILD_DIR}/topk_gate_vf.o" \
  -L"${SIM_LIB_DIR}" -Wl,-rpath,"${SIM_LIB_DIR}" -Wl,--no-as-needed -lruntime_camodel \
  -o "${BUILD_DIR}/libtopk_gate_vf.so" 2>&1 || die "link .so failed"

log "step 3: build host executable"
"${BISHENG}" \
  -xc++ -include stdint.h -include stddef.h -std=gnu++17 -O2 \
  -Wno-macro-redefined -Wno-ignored-attributes \
  "${SCRIPT_DIR}/main_vf.cpp" \
  -I "${SCRIPT_DIR}" \
  -I "${ASCEND_HOME_PATH}/include" \
  -L "${BUILD_DIR}" \
  -L "${ASCEND_HOME_PATH}/lib64" \
  -L "${SIM_LIB_DIR}" -Wl,-rpath,"${SIM_LIB_DIR}" -Wl,--allow-shlib-undefined -lruntime_camodel \
  -Wl,-rpath,"${BUILD_DIR}" \
  -Wl,-rpath,"${ASCEND_HOME_PATH}/lib64" \
  -DK_E=${K_E} -DK_K=${K_K} -DK_N=${K_N} \
  -o "${BUILD_DIR}/topk_gate_vf_sim" \
  -ltopk_gate_vf \
  -lstdc++ -lascendcl -lm -ltiling_api -lplatform -lc_sec -ldl -lnnopbase 2>&1 || die "host build failed"

log "step 4: run on SIM"
CAMODEL_LOG_PATH="${BUILD_DIR}/camodel_log"
mkdir -p "${CAMODEL_LOG_PATH}" "${BUILD_DIR}/log/ub_log"
(
  cd "${BUILD_DIR}"
  export ASCEND_HOME_PATH="${ASCEND_HOME_PATH}"
  export CAMODEL_LOG_PATH="${CAMODEL_LOG_PATH}"
  if [[ -f "${ASCEND_HOME_PATH}/set_env.sh" ]]; then
    source "${ASCEND_HOME_PATH}/set_env.sh" >/dev/null 2>&1
  fi
  LD_LIBRARY_PATH="${BUILD_DIR}:${SIM_LIB_DIR}:${ASCEND_HOME_PATH}/lib64:${LD_LIBRARY_PATH:-}" \
    "./topk_gate_vf_sim" 2>&1
) || die "simulator run failed"

log "step 5: extract PMU"
SUMMARY_LOG="${CAMODEL_LOG_PATH}/core0_summary_log"
if [[ -f "${SUMMARY_LOG}" ]]; then
  log "--- PMU Summary (tile=${K_TOKEN_TILE}) ---"
  grep -E "kernal total ticks|system total ticks|rvec_veccore0_simd_busy|mte2_veccore0|mte3_veccore0|rvec_veccore0_busy|CCU.scalar" "${SUMMARY_LOG}" 2>/dev/null || true
fi

log "PASSED (E=${K_E} K=${K_K} tile=${K_TOKEN_TILE} aabb=${USE_AABBCC} misched=${MISCHED})"
