# Issue #574 repro: AABBCC VF vs CCE vec-misched preg spill

Tracks https://github.com/mouliangyu/PTOAS/issues/574

Standalone **CCE intrinsic** `topk_gate` VF (not VPTO IR) for Ascend A5 camodel.
Demonstrates that hand **AABBCC** dual-token scheduling reaches high EXIPC only when
Bisheng vec-misched is **disabled**; with default misched the same binary shape suffers
heavy **`PLDI`/`PSTI` + `RV_SMEM_BAR`**.

## Files

| file | role |
|---|---|
| `topk_gate_vf.cpp` | Device VF kernel (`USE_AABBCC`, `K_TOKEN_TILE`, tiled MTE schedule) |
| `main_vf.cpp` | Host driver + golden top-k check |
| `run_vf_sim.sh` | Build + camodel run (`MISCHED=0|1`) |
| `VF_AABBCC_MISCHED_REPORT.md` | 4-case numbers + how-to |

## Dependencies

- Ascend CANN / bisheng (`ASCEND_HOME_PATH`, default `.../cann_9rel/cann-9.0.0`)
- PTO ISA headers (`PTO_ISA_INC`, default `/home/happybot/projects/pto-isa/include` — override)
- camodel runtime (`dav_3510`)

## Run (4 cases)

```bash
cd docs/issues/574-topk-gate-vf-aabbcc-misched
export PTO_ISA_INC=/path/to/pto-isa/include   # if needed

K_E=384 K_K=9 K_N=4 K_TOKEN_TILE=4 USE_AABBCC=0 MISCHED=0 ./run_vf_sim.sh
K_E=384 K_K=9 K_N=4 K_TOKEN_TILE=4 USE_AABBCC=0 MISCHED=1 ./run_vf_sim.sh
K_E=384 K_K=9 K_N=4 K_TOKEN_TILE=4 USE_AABBCC=1 MISCHED=0 ./run_vf_sim.sh  # best
K_E=384 K_K=9 K_N=4 K_TOKEN_TILE=4 USE_AABBCC=1 MISCHED=1 ./run_vf_sim.sh  # spill regress
```

`MISCHED=0` → `-mllvm -cce-aicore-vec-misched=0`.

## Snapshot results (N=4 E=384 K=9 tile=4)

| case | ticks | simd | EXIPC | PLDI (IDU) | PSTI (IDU) | SMEM_BAR |
|---|---:|---:|---:|---:|---:|---:|
| ABCABC ms0 | 5604 | 2907 | 0.531 | ~0 | ~0 | 0 |
| ABCABC ms1 | 5604 | 2907 | 0.531 | 288 | 16 | 40 |
| AABBCC ms0 | **4345** | **1609** | **0.935** | 20 | 16 | 8 |
| AABBCC ms1 | 8315 | 5489 | 0.274 | **1832** | **1760** | **272** |

Related PTOAS CLI (does not fix this CCE case by itself): `--enable-bisheng-vec-misched`
(see `test/lit/vpto/bisheng_vec_misched_cli.pto`).
