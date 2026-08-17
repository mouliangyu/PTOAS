# topk_gate VF — run guide + 4-case camodel report

**Shape:** N=4, E=384, K=9, `K_TOKEN_TILE=4` (same outer schedule as staged: one MTE2→V→MTE3 wave).  
**Kernel:** `pto-skills/testing-pto-kernels/generated/moe_a5/topk_gate/topk_gate_vf.cpp`  
**Host/sim:** `./run_vf_sim.sh`

## How to run

```bash
cd pto-skills/testing-pto-kernels/generated/moe_a5/topk_gate

# 1) ABCABC, misched OFF
K_E=384 K_K=9 K_N=4 K_TOKEN_TILE=4 USE_AABBCC=0 MISCHED=0 ./run_vf_sim.sh

# 2) ABCABC, misched ON (default compiler vec-misched)
K_E=384 K_K=9 K_N=4 K_TOKEN_TILE=4 USE_AABBCC=0 MISCHED=1 ./run_vf_sim.sh

# 3) AABBCC (dual-token phase group), misched OFF  ← best
K_E=384 K_K=9 K_N=4 K_TOKEN_TILE=4 USE_AABBCC=1 MISCHED=0 ./run_vf_sim.sh

# 4) AABBCC, misched ON  ← preg spill regression
K_E=384 K_K=9 K_N=4 K_TOKEN_TILE=4 USE_AABBCC=1 MISCHED=1 ./run_vf_sim.sh
```

**Flags**
- `USE_AABBCC=0` → per-token **ABCABC** (`vmax→vcmax→match→vmin→vcmin→mask` fully for t0, then t1, …).
- `USE_AABBCC=1` → hand **AABBCC** across 2 tokens (`vmax∥vmax`, then `vcmax×2`, then match/`vmin`/`vcmin×2`).
- `MISCHED=0` → `-mllvm -cce-aicore-vec-misched=0` (disable CCE vector MI reschedule).
- `MISCHED=1` → leave compiler misched on.

Build dirs: `build_vf_e384_k9_t4_aabb{0|1}_ms{0|1}/`  
PMU: `camodel_log/core0_summary_log`, spills in `*.instr_log.dump` / `*.IDU.dump` (`PLDI`/`PSTI`/`RV_SMEM_BAR`).

## 4-case performance (camodel)

| case | ticks | simd | EXIPC | dual% | PLDI (IDU) | PSTI (IDU) | SMEM_BAR (instr) |
|---|---:|---:|---:|---:|---:|---:|---:|
| ABCABC MISCHED=0 | 5604 | 2907 | 0.531 | 43.5% | ~0 | ~0 | 0 |
| ABCABC MISCHED=1 | 5604 | 2907 | 0.531 | 40.9% | 288 | 16 | 40 |
| **AABBCC MISCHED=0** | **4345** | **1609** | **0.935** | **63.5%** | 20 | 16 | 8 |
| AABBCC MISCHED=1 | 8315 | 5489 | 0.274 | 35.7% | **1832** | **1760** | **272** |

**Reads**
- Same tile/MTE schedule for all four; only VF issue order + misched differ.
- AABBCC + misched off is best (~1.8× lower simd than ABCABC).
- AABBCC + misched on: **same EXU op mix**, but massive **`PLDI`/`PSTI` + `SMEM_BAR`** → simd ~3.4× worse. Misched undoes hand AABBCC via **preg spill**, not by changing VMAX/VCMAX counts.
- ABCABC is almost insensitive to misched (already long RAW chains).

## Ask for PTO-AS

Need instruction scheduling that:
1. Preserves / enables **AABBCC-style** dual-token (or multi-token) latency hiding for VF.
2. **Avoids preg/vreg spill** (`PLDI`/`PSTI`) and excess `SMEM_BAR` when pressure is high.
3. Ideally matches `MISCHED=0` quality without requiring users to disable the entire misched pass.
