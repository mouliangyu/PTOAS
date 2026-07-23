# Issue: bisheng CSE eliminates vlds after vscatter + mem_bar("VST_VLD")

## Summary

When `pto.vscatter` writes to UB memory and a subsequent `pto.vlds` reads from
the same UB address, bisheng's CSE (Common Subexpression Elimination) pass
eliminates the second `vlds` — **even when a `pto.mem_bar("VST_VLD")` is placed
between the scatter and the load**.

This makes `vscatter` unusable for in-place masking of UB data that is later
reloaded, because the reload reads stale (pre-scatter) data.

## Root Cause

1. **ptoas correctly preserves both loads and the barrier** in its LLVM IR
   output. The `--emit-vpto-llvm-ir` output shows:
   - First `@llvm.hivm.vldsx1.v64f32` call (load1)
   - `@llvm.hivm.vscatter.v64f32.v300` call (scatter)
   - `@llvm.hivm.mem.bar.vst.vld()` call (barrier)
   - Second `@llvm.hivm.vldsx1.v64f32` call (load2)

2. **bisheng's optimizer eliminates load2**, treating it as a CSE of load1
   since both calls have identical arguments (`ptr null, i32 0, i32 0, i32 0`).
   The `mem.bar.vst.vld` intrinsic is apparently not treated as a barrier that
   invalidates cached load results.

3. **In contrast, `vsts` IS recognized as a store** by bisheng's CSE pass.
   Replacing `vscatter` with `vsts` (contiguous store) + `mem_bar("VST_VLD")`
   correctly prevents CSE of the subsequent `vlds`.

## Reproducer

### VPTO-level lit test

File: `test/lit/vpto/vscatter_cse_repro.pto`

```
ptoas --pto-arch=a5 --pto-backend=vpto test/lit/vpto/vscatter_cse_repro.pto \
  -o /tmp/out.o --emit-vpto-llvm-ir
```

The LLVM IR shows both `vldsx1` calls and the `mem.bar.vst.vld` between them.
After bisheng compilation, only one `vldsx1` survives in the device binary.

### VMI-level reproducer

File: `vmi-demo/moe/topk_gate/vscatter_repro/vscatter_repro.py`

A VMI kernel that:
1. Loads 64 floats (all 1.0) from UB, stores to output[0..63]
2. Scatters 42.0 to UB[3]
3. `mem_bar("VST_VLD")`
4. Reloads UB[0..63], stores to output[64..127]
5. Outputs both pre-scatter and post-scatter data

Run:
```
CMAKE_PREFIX_PATH="" \
ASCEND_HOME_PATH=... \
PTOAS_BIN=.../ptoas \
PYTHONPATH=... bash .../sim_dsl.sh --soc-version Ascend950PR_9599 \
  --output /tmp/vscatter_repro_out \
  vmi-demo/moe/topk_gate/vscatter_repro/vscatter_repro.py
```

**Expected**: post[3] = 42.0 (scatter visible after reload)
**Actual**: post[0:8] = [0. 0. 0. 0. 0. 0. 0. 0.] (scatter lost — only 1 RV_VLDI in binary, second load CSE'd)

The instruction trace confirms only 1 `RV_VLDI` in the bisheng-compiled binary:
```
RV_VLDI,282120452,RVECLD,1,9,0.010000,""
RV_VSTI,282120456,RVECST,1,10,0.010000,""
RV_VSCATTER,282120472,RVECST,1,13,0.010000,""
RV_VSTI,282120480,RVECST,1,13,0.010000,""
```
(Only 1 VLDI — the second load was CSE'd. 2 VSTI = pre-scatter store + post-scatter store.)

## Impact

- `vscatter` cannot be used for any pattern that modifies UB in-place and
  reloads the modified data in the same kernel.
- **Workaround**: Use `vsts` (contiguous store) + `mem_bar("VST_VLD")` instead.
  This works but requires reloading + masking + storing entire vregs, which is
  significantly more instructions than a single `vscatter`.

### Real-world impact (topk_gate argmax kernel)

In the topk_gate argmax kernel, Phase 3 (masking the winner to -inf) could use
a single `vscatter` per token per K iteration (1 instruction) instead of
`vcmp + vsel + vsts` per vreg per token per K (12 instructions for 4 vregs).

| Approach | Phase 3 instructions | wall-cyc (T=8 CB=4) |
|----------|---------------------:|--------------------:|
| vreg vsel (no UB write) | 8 (vsel only) | 1937 |
| vscatter (broken) | 1 (but CSE'd) | N/A (incorrect — 2nd load eliminated) |
| vsts + mem_bar (workaround) | 13 (vload+vcmp+vsel+vsts+mem_bar) | 4500 |

The workaround is 2.3× slower than the optimal vreg-only approach.

## Expected Fix

bisheng's CSE pass should treat `@llvm.hivm.vscatter` as a write to UB memory
that invalidates cached `@llvm.hivm.vldsx1` results, just as it does for
`@llvm.hivm.vstsx1`. Alternatively, `@llvm.hivm.mem.bar.vst.vld` should act
as a CSE barrier for all vector load intrinsics.
