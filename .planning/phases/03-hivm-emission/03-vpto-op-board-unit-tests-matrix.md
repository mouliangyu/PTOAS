# VPTO Op Board Unit Tests Matrix

## Legend

- `infra`: 基础设施 op，只作为其他 case 的准备动作或收尾动作复用
- `planned`: 已纳入本轮范围，待补 case
- `implemented`: case 已落地；即便 parser / verifier / lowering / codegen / runtime / board 失败，也仍记为 `implemented`，并在 `notes` 写明失败点
- `compiled`: case 已落地，且 compile-only 路径已走到编译产物；尚未完成 runtime / board 验证
- `board-passed`: 已完成上板验证
- `blocked`: 当前无法仅依据 `docs/vpto-spec.md` 与 `docs/isa/` 写出语义明确的 case；只用于文档层面的不可写阻塞

## Latest Compile Scan

- 最新 compile-only 扫描命令：
  - `source scripts/ptoas_env.sh && WORK_SPACE=/tmp/vpto-compile-only-20260404-171409 COMPILE_ONLY=1 DEVICE=SIM JOBS=64 test/vpto/scripts/run_host_vpto_validation_parallel.sh`
- 最新结果：
  - 总计 `219`，`PASS=219`，`FAIL=0`
  - 汇总文件：`/tmp/vpto-compile-only-20260404-171409/parallel-summary.tsv`
- 本轮 matrix 刷新原则：
  - 只更新最新 rerun 已确认的进度与失败归因
  - 不因为 compile 失败回退 `implemented`
  - 仅保留开发者已确认的 `blocked` 项；其余失败继续记录为实现缺口、surface 漂移或文档缺口

## Infrastructure Ops

| op | family | doc_source | in_scope | case | scenarios | status | notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `pto.set_flag` | pipeline-sync | `docs/isa/01-pipeline-sync.md` | no | reused-by-others | cross-pipe handoff | infra | 不单独立项测试 |
| `pto.wait_flag` | pipeline-sync | `docs/isa/01-pipeline-sync.md` | no | reused-by-others | cross-pipe handoff | infra | 不单独立项测试 |
| `pto.get_buf` | pipeline-sync | `docs/isa/01-pipeline-sync.md` | no | reused-by-others | buffer acquire | infra | 不单独立项测试 |
| `pto.rls_buf` | pipeline-sync | `docs/isa/01-pipeline-sync.md` | no | reused-by-others | buffer release | infra | 不单独立项测试 |
| `pto.pipe_barrier` / `pto.mem_bar` | pipeline-sync | `docs/isa/01-pipeline-sync.md` | no | reused-by-others | ordering / fence | infra | 仅作为支撑动作复用 |
| `pto.set_loop_size_outtoub` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.set_loop1_stride_outtoub` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.set_loop2_stride_outtoub` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.set_loop_size_ubtoout` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.set_loop1_stride_ubtoout` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.set_loop2_stride_ubtoout` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.copy_gm_to_ubuf` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | GM to UB feed | infra | 作为输入准备动作 |
| `pto.copy_ubuf_to_gm` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | UB to GM drain | infra | 作为输出导回动作 |
| `pto.copy_ubuf_to_ubuf` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | UB to UB move | infra | 仅在需要时复用 |

## Op Summary

| op | family | doc_source | in_scope | case | scenarios | notes |
| --- | --- | --- | --- | --- | --- | --- |
| `pto.vlds` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vlds` | `core-f32, contiguous, full-mask, aligned, dist-norm` | 已补 `vlds-tail`、`vlds-brc-b32` 与 representative dist 变体：`vlds-brc-b16`、`vlds-us-b16`、`vlds-ds-b16`、`vlds-brc-blk` |
| `pto.vldas` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vldas-vldus` | `core-f32, full-mask, unaligned, stream-state` | 与 `pto.vldus` 成组验证 |
| `pto.vldus` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vldas-vldus` | `core-f32, full-mask, unaligned, stream-state` | 与 `pto.vldas` 成组验证 |
| `pto.vldsx2` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vldsx2-vstsx2` | `core-f32, full-mask, paired-roundtrip, dintlv` | 与 `pto.vstsx2` 成组验证 |
| `pto.vsldb` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vsldb` | `core-f32, full-mask, block-strided-load, block-mask` | |
| `pto.vgather2` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | `micro-op/gather-scatter/vgather2` | `core-f32, full-mask, non-contiguous, explicit-index-pattern, load-effect-validation, no-alias` | |
| `pto.vgatherb` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | `micro-op/gather-scatter/vgatherb` | `core-f32, full-mask, block-gather, aligned-base, load-effect-validation, no-alias` | |
| `pto.vgather2_bc` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | `micro-op/gather-scatter/vgather2_bc` | `core-f32, full-mask, non-contiguous, masked-gather, load-effect-validation, no-alias` | |
| `pto.vsts` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vsts` | `core-f32, contiguous, full-mask, aligned, dist-norm` | |
| `pto.vstsx2` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vldsx2-vstsx2` | `core-f32, full-mask, paired-roundtrip, dintlv` | 与 `pto.vldsx2` 成组验证 |
| `pto.vsstb` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vsstb` | `core-f32, full-mask, block-strided-store, block-mask` | |
| `pto.vscatter` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | `micro-op/gather-scatter/vscatter` | `core-f32, full-mask, non-contiguous, explicit-index-pattern, scatter-store, store-effect-validation, no-alias` | |
| `pto.vstas` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vstas-vstus-offset-update` | `core-f32, full-mask, aligned, immediate-offset, state-update` | 与 `pto.vstus` 成组验证 |
| `pto.vstar` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vstar` | `core-f32, full-mask, aligned, state-update` | |
| `pto.vstus` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vstas-vstus-offset-update` | `core-f32, full-mask, unaligned, immediate-offset, state-update` | 与 `pto.vstas` 成组验证 |
| `pto.vstur` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vstur` | `core-f32, full-mask, unaligned, state-update` | |
| `pto.plds` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/psts-pk-plds-us`; `micro-op/predicate-load-store/psts-norm-plds-ds`; `micro-op/predicate-load-store/psts-pk-plds-us-prefix-boundary` | `predicate-load-store-composition, dynamic-offset, load-store-pair-preservation, representative-logical-elements` | 与 `pto.psts` 成组验证 |
| `pto.pldi` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/psti-pk-pldi-us`; `micro-op/predicate-load-store/psti-norm-pldi-ds` | `predicate-load-store-composition, immediate-offset, load-store-pair-preservation, representative-logical-elements` | 与 `pto.psti` 成组验证 |
| `pto.psts` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/psts-pk-plds-us`; `micro-op/predicate-load-store/psts-norm-plds-ds`; `micro-op/predicate-load-store/psts-pk-plds-us-prefix-boundary` | `predicate-load-store-composition, dynamic-offset, load-store-pair-preservation, representative-logical-elements` | 与 `pto.plds` 成组验证 |
| `pto.psti` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/psti-pk-pldi-us`; `micro-op/predicate-load-store/psti-norm-pldi-ds` | `predicate-load-store-composition, immediate-offset, load-store-pair-preservation, representative-logical-elements` | 与 `pto.pldi` 成组验证 |
| `pto.pstu` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/pstu` | `unaligned-predicate-store, state-update, representative-logical-elements` | |
| `pto.vbr` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/vbr-f32` | `core-f32, scalar-broadcast` | 另有 `vbr-i32` 变体 |
| `pto.vdup` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/vdup-scalar` | `core-f32, scalar-operand` | 另有 `vdup-lane` 变体 |
| `pto.pset_b8` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pset-pattern` | `pattern-mask, pat-all, pat-vl` | 与 `pto.pset_b16` / `pto.pset_b32` 共用样板 |
| `pto.pset_b16` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pset-pattern` | `pattern-mask, pat-all, pat-vl` | 与 `pto.pset_b8` / `pto.pset_b32` 共用样板 |
| `pto.pset_b32` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pset-pattern` | `pattern-mask, pat-all, pat-vl` | 与 `pto.pset_b8` / `pto.pset_b16` 共用样板 |
| `pto.pge_b8` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pge-tail-mask` | `tail-mask` | 与 `pto.pge_b16` / `pto.pge_b32` 共用样板 |
| `pto.pge_b16` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pge-tail-mask` | `tail-mask` | 与 `pto.pge_b8` / `pto.pge_b32` 共用样板 |
| `pto.pge_b32` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pge-tail-mask` | `tail-mask` | 与 `pto.pge_b8` / `pto.pge_b16` 共用样板 |
| `pto.plt_b8` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/plt-tail-mask` | `tail-mask, scalar-carry-out` | 与 `pto.plt_b16` / `pto.plt_b32` 共用样板 |
| `pto.plt_b16` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/plt-tail-mask` | `tail-mask, scalar-carry-out` | 与 `pto.plt_b8` / `pto.plt_b32` 共用样板 |
| `pto.plt_b32` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/plt-tail-mask` | `tail-mask, scalar-carry-out` | 与 `pto.plt_b8` / `pto.plt_b16` 共用样板 |
| `pto.ppack` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/ppack-punpack` | `pack-unpack-roundtrip` | 与 `pto.punpack` 成组验证 |
| `pto.punpack` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/ppack-punpack` | `pack-unpack-roundtrip` | 与 `pto.ppack` 成组验证 |
| `pto.pand` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pand` | `predicate-transform` | |
| `pto.por` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/por` | `predicate-transform` | |
| `pto.pxor` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pxor` | `predicate-transform` | |
| `pto.pnot` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pnot` | `predicate-transform` | |
| `pto.psel` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/psel` | `predicate-transform, predicate-select` | |
| `pto.pdintlv_b8` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pdintlv_b8` | `predicate-transform, lane-order` | |
| `pto.pdintlv_b16` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pdintlv_b16` | `predicate-transform, lane-order` | |
| `pto.pdintlv_b32` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pdintlv_b32` | `predicate-transform, lane-order` | |
| `pto.pintlv_b8` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pintlv_b8` | `predicate-transform, lane-order` | |
| `pto.pintlv_b16` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pintlv_b16` | `predicate-transform, lane-order` | |
| `pto.pintlv_b32` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pintlv_b32` | `predicate-transform, lane-order` | |
| `pto.vabs` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vabs` | `core-f32, full-mask` | 另有 `vabs-tail`、整型与异常值变体；包含 blocked 子项 |
| `pto.vneg` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vneg` | `core-f32, full-mask` | |
| `pto.vexp` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vexp` | `core-f32, full-mask` | 另有 `vexp-tail`、`vexp-f16` 与异常值/上下溢变体；包含 blocked 子项 |
| `pto.vln` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vln` | `core-f32, full-mask, domain-positive` | |
| `pto.vsqrt` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vsqrt` | `core-f32, full-mask, domain-nonnegative` | |
| `pto.vrsqrt` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vrsqrt` | `core-f32, full-mask, exceptional-values` | |
| `pto.vrec` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vrec` | `core-f32, full-mask, exceptional-values` | |
| `pto.vrelu` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vrelu` | `core-f32, full-mask` | |
| `pto.vnot` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vnot` | `core-i16-signed, full-mask` | |
| `pto.vbcnt` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vbcnt` | `core-i16-unsigned, full-mask` | |
| `pto.vcls` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vcls` | `core-i16-signed, full-mask` | |
| `pto.vadd` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vadd` | `core-f32, full-mask` | 另有 tail、`f16`、整型与异常值/溢出变体 |
| `pto.vsadd` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vsadd` | `core-i16-signed, full-mask` | |
| `pto.vsub` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vsub` | `core-f32, full-mask` | |
| `pto.vssub` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vssub` | `core-i16-signed, full-mask` | |
| `pto.vmul` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vmul` | `core-f32, full-mask` | |
| `pto.vdiv` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vdiv` | `core-f32, full-mask` | 另有 tail、`f16` 与异常值变体 |
| `pto.vmax` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vmax` | `core-f32, full-mask` | |
| `pto.vmin` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vmin` | `core-f32, full-mask` | 另有 tail、`f16`、整型与异常值变体 |
| `pto.vand` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vand` | `core-i16-unsigned, full-mask` | |
| `pto.vor` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vor` | `core-i16-unsigned, full-mask` | |
| `pto.vxor` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vxor` | `core-i16-unsigned, full-mask` | |
| `pto.vshl` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vshl` | `core-i16-unsigned, full-mask` | |
| `pto.vshr` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vshr` | `core-i16-unsigned, full-mask` | |
| `pto.vaddc` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vaddc` | `core-i16-unsigned, full-mask, carry-chain` | |
| `pto.vsubc` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vsubc` | `core-i16-unsigned, full-mask, carry-chain` | |
| `pto.vadds` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vadds` | `core-f32, full-mask, scalar-operand` | 另有 tail、类型扩展、异常值与整型溢出变体 |
| `pto.vsadds` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vsadds` | `core-i16-signed, full-mask, scalar-operand` | |
| `pto.vmuls` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vmuls` | `core-f32, full-mask, scalar-operand` | |
| `pto.vmaxs` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vmaxs` | `core-f32, full-mask, scalar-operand` | |
| `pto.vmins` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vmins` | `core-f32, full-mask, scalar-operand` | |
| `pto.vshls` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vshls` | `core-i16-unsigned, full-mask, scalar-operand` | |
| `pto.vshrs` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vshrs` | `core-i16-unsigned, full-mask, scalar-operand` | |
| `pto.vlrelu` | dsa-sfu | `docs/isa/08-vec-scalar-ops.md`, `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vlrelu-f32` | `core-f32, scalar-operand, full-mask` | 文档双重收录；测试台账统一归入 `dsa-sfu` |
| `pto.vaddcs` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vaddcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | |
| `pto.vsubcs` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vsubcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | |
| `pto.vcvt` | conversion | `docs/isa/09-conversion-ops.md` | yes | `micro-op/conversion/vcvt-f32-to-f16` | `f32-to-f16, full-mask` | 另有 widening、tail、异常值与整型溢出变体 |
| `pto.vtrc` | conversion | `docs/isa/09-conversion-ops.md` | yes | `micro-op/conversion/vtrc-f32-rounding` | `core-f32, round-r, round-z, round-f` | 另有特殊值变体 |
| `pto.vcadd` | reduction | `docs/isa/10-reduction-ops.md` | yes | `micro-op/reduction/vcadd` | `core-f32, result-placement` | 另有 `vcadd-tail` 变体 |
| `pto.vcmax` | reduction | `docs/isa/10-reduction-ops.md` | yes | `micro-op/reduction/vcmax` | `core-f32, result-placement` | |
| `pto.vcmin` | reduction | `docs/isa/10-reduction-ops.md` | yes | `micro-op/reduction/vcmin` | `core-f32, result-placement` | |
| `pto.vcgadd` | reduction | `docs/isa/10-reduction-ops.md` | yes | `micro-op/reduction/vcgadd` | `group-reduction, result-placement` | |
| `pto.vcgmax` | reduction | `docs/isa/10-reduction-ops.md` | yes | `micro-op/reduction/vcgmax` | `group-reduction, result-placement` | |
| `pto.vcgmin` | reduction | `docs/isa/10-reduction-ops.md` | yes | `micro-op/reduction/vcgmin` | `group-reduction, result-placement` | |
| `pto.vcpadd` | reduction | `docs/isa/10-reduction-ops.md` | yes | `micro-op/reduction/vcpadd` | `prefix-op, full-mask` | |
| `pto.vcmp` | compare-select | `docs/isa/11-compare-select.md` | yes | `micro-op/compare-select/vcmp-eq` | `core-f32, full-mask, relation-eq` | 另有 `vcmp-lt`、tail、整型与异常值变体 |
| `pto.vcmps` | compare-select | `docs/isa/11-compare-select.md` | yes | `micro-op/compare-select/vcmps-f32` | `core-f32, full-mask, scalar-operand` | 另有 tail、整型与异常值变体 |
| `pto.vsel` | compare-select | `docs/isa/11-compare-select.md` | yes | `micro-op/compare-select/vsel` | `core-f32, full-mask` | 另有 tail 与整型变体 |
| `pto.vselr` | compare-select | `docs/isa/11-compare-select.md` | yes | `micro-op/compare-select/vselr` | `core-f32, full-mask, explicit-lane-index` | 另有 `f16` 与 `u8` 类型变体 |
| `pto.vintlv` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vintlv-vdintlv` | `paired-roundtrip, lane-order` | 与 `pto.vdintlv` 成组验证 |
| `pto.vdintlv` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vintlv-vdintlv` | `paired-roundtrip, lane-order` | 与 `pto.vintlv` 成组验证 |
| `pto.vslide` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vslide` | `lane-order, slide-window` | |
| `pto.vsqz` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vsqz` | `predicate-driven-rearrangement, stable-order` | |
| `pto.vusqz` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vusqz` | `predicate-driven-rearrangement, placement` | |
| `pto.vpack` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vpack` | `pack-unpack, narrowing, half-placement, zero-fill-other-half` | |
| `pto.vsunpack` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vsunpack` | `pack-unpack, sign-extend` | |
| `pto.vzunpack` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vzunpack` | `pack-unpack, zero-extend` | |
| `pto.vprelu` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vprelu-f32` | `core-f32, vector-alpha` | |
| `pto.vexpdiff` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vexpdiff-f32` | `core-f32, fused-expdiff` | 输入 `f16|f32`、输出固定 `f32`，并要求位置参数字符串 `EVEN|ODD` |
| `pto.vaxpy` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vaxpy-f32` | `core-f32, scalar-operand, fused-op` | |
| `pto.vmull` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vmull` | `widening-op, hi-lo-split` | |
| `pto.vmula` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vmula` | `core-f32, fused-op, accumulator` | |
| `pto.vci` | dsa-sfu / conversion | `docs/isa/13-dsa-sfu-ops.md`, `docs/isa/09-conversion-ops.md` | yes | `micro-op/dsa-sfu/vci` | `index-generation` | |
| `pto.vbitsort` | dsa-sfu | `docs/vpto-spec.md`, `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vbitsort` | `index-generation, layout-transform` | 已按 `visa.txt` 的 `VBS32` 补齐排序方向、tie-break、proposal record 布局与 repeat 语义；LLVM emitter 已按 installed frontend 真实形态接到 `llvm.hivm.VBS32.V300.{f16|f32}`，当前 `f32` case 已完成定向 `COMPILE_ONLY` |

## Case Matrix

| case | family | target_ops | scenarios | status | notes |
| --- | --- | --- | --- | --- | --- |
| `micro-op/binary-vector/vadd` | binary-vector | `pto.vadd` | `core-f32, full-mask` | board-passed | |
| `micro-op/binary-vector/vadd-tail` | binary-vector | `pto.vadd` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vadd-f16` | binary-vector | `pto.vadd` | `core-f16, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vadd-bf16` | binary-vector | `pto.vadd` | `core-bf16, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vadd-i16-signed` | binary-vector | `pto.vadd` | `core-i16-signed, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vadd-i16-unsigned` | binary-vector | `pto.vadd` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vadd-i16-signed-overflow` | binary-vector | `pto.vadd` | `core-i16-signed, full-mask, integer-overflow` | compiled | 已补充 `i16` wraparound oracle 用例，并完成定向 `COMPILE_ONLY` |
| `micro-op/binary-vector/vadd-i16-unsigned-overflow` | binary-vector | `pto.vadd` | `core-i16-unsigned, full-mask, integer-overflow` | compiled | 已补充 `u16` wraparound oracle 用例，并完成定向 `COMPILE_ONLY` |
| `micro-op/binary-vector/vadd-f32-exceptional` | binary-vector | `pto.vadd` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vsadd` | binary-vector | `pto.vsadd` | `core-i16-signed, full-mask` | compiled | `2026-04-04` 定向 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/binary-vector/vsadd'` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vsub` | binary-vector | `pto.vsub` | `core-f32, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vssub` | binary-vector | `pto.vssub` | `core-i16-signed, full-mask` | compiled | `2026-04-04` 定向 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/binary-vector/vssub'` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vmul` | binary-vector | `pto.vmul` | `core-f32, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vdiv` | binary-vector | `pto.vdiv` | `core-f32, full-mask` | board-passed | |
| `micro-op/binary-vector/vdiv-tail` | binary-vector | `pto.vdiv` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vdiv-f16` | binary-vector | `pto.vdiv` | `core-f16, full-mask` | compiled | 已修正为真实 `f16`/`b16` 路径，并完成定向 `COMPILE_ONLY` |
| `micro-op/binary-vector/vdiv-f32-exceptional` | binary-vector | `pto.vdiv` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vmax` | binary-vector | `pto.vmax` | `core-f32, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vmin` | binary-vector | `pto.vmin` | `core-f32, full-mask` | board-passed | |
| `micro-op/binary-vector/vmin-tail` | binary-vector | `pto.vmin` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vmin-f16` | binary-vector | `pto.vmin` | `core-f16, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vmin-bf16` | binary-vector | `pto.vmin` | `core-bf16, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vmin-i16-signed` | binary-vector | `pto.vmin` | `core-i16-signed, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vmin-i16-unsigned` | binary-vector | `pto.vmin` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vmin-f32-exceptional` | binary-vector | `pto.vmin` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-02` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vand` | binary-vector | `pto.vand` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vor` | binary-vector | `pto.vor` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vxor` | binary-vector | `pto.vxor` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vshl` | binary-vector | `pto.vshl` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vshr` | binary-vector | `pto.vshr` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vaddc` | binary-vector | `pto.vaddc` | `core-i16-unsigned, full-mask, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` 覆盖 carry-chain 路径 |
| `micro-op/binary-vector/vsubc` | binary-vector | `pto.vsubc` | `core-i16-unsigned, full-mask, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` 覆盖 carry-chain 路径 |
| `micro-op/vec-scalar/vadds` | vec-scalar | `pto.vadds` | `core-f32, full-mask, scalar-operand` | board-passed | |
| `micro-op/vec-scalar/vadds-tail` | vec-scalar | `pto.vadds` | `core-f32, tail-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vadds-f16` | vec-scalar | `pto.vadds` | `core-f16, full-mask, scalar-operand` | compiled | `2026-04-05` 已按 `f16 + mask` surface 修正 case/oracle；`DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library。repo-generated `.ll` 已确认发射为 `llvm.hivm.vadds.v128f16.x` |
| `micro-op/vec-scalar/vadds-bf16` | vec-scalar | `pto.vadds` | `core-bf16, full-mask, scalar-operand` | compiled | `2026-04-05` 已按 `bf16 + mask` surface 修正 case/oracle；`DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library。repo-generated `.ll` 已确认发射为 `llvm.hivm.vadds.v128bf16.x` |
| `micro-op/vec-scalar/vadds-i16-signed` | vec-scalar | `pto.vadds` | `core-i16-signed, full-mask, scalar-operand` | compiled | `2026-04-05` 已将误写的 `vsadds` case 收敛回 `vadds s16`，并用 signless `arith.constant ... : i16` 覆盖 signed vec-scalar 标量接入规则；当前 case 已按 `visa.txt` 的 `..., Pg` surface 保留显式 `%mask`。`DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library。repo-generated `.ll` 已确认发射为 `llvm.hivm.vadds.v128s16.x` |
| `micro-op/vec-scalar/vadds-i16-unsigned` | vec-scalar | `pto.vadds` | `core-i16-unsigned, full-mask, scalar-operand` | compiled | `2026-04-05` 已将错误 skeleton 收敛回 `vadds u16`，并用 signless `arith.constant ... : i16` 覆盖 unsigned vec-scalar 标量接入规则；当前 case 已按 `visa.txt` 的 `..., Pg` surface 保留显式 `%mask`。`DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library。repo-generated `.ll` 已确认发射为 `llvm.hivm.vadds.v128u16.x` |
| `micro-op/vec-scalar/vadds-f32-exceptional` | vec-scalar | `pto.vadds` | `core-f32, full-mask, scalar-operand, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vadds-i16-signed-overflow` | vec-scalar | `pto.vadds` | `core-i16-signed, full-mask, scalar-operand, integer-overflow` | compiled | `2026-04-05` 已将误写的 `vsadds` case 收敛回真实 `vadds` overflow case，输入前缀显式覆盖 `INT16_MAX`/`INT16_MIN` 邻域，golden 按 `SPR.CTRL[53]=0` 的 truncation 基线做 `int32 -> int16` wraparound；定向 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/vec-scalar/vadds-i16-signed-overflow'` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vadds-i16-unsigned-overflow` | vec-scalar | `pto.vadds` | `core-i16-unsigned, full-mask, scalar-operand, integer-overflow` | compiled | `2026-04-05` 已移除误植的 `vsubcs` skeleton，改为真实 `vadds ui16` overflow case，输入前缀显式覆盖 `UINT16_MAX` 邻域，golden 按 `SPR.CTRL[53]=0` 的 truncation 基线做 `uint32 -> uint16` wraparound；定向 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/vec-scalar/vadds-i16-unsigned-overflow'` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vsadds` | vec-scalar | `pto.vsadds` | `core-i16-signed, full-mask, scalar-operand` | compiled | `2026-04-04` 定向 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/vec-scalar/vsadds'` 已走到 step `4/6` 并产出 kernel shared library；repo case 以 signless `arith.constant ... : i16` 覆盖 `%scalar` 接入规则，并按 `visa.txt` 的 `..., Pg` surface 保留显式 `%mask` |
| `micro-op/vec-scalar/vmuls` | vec-scalar | `pto.vmuls` | `core-f32, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vmaxs` | vec-scalar | `pto.vmaxs` | `core-f32, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vmins` | vec-scalar | `pto.vmins` | `core-f32, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vshls` | vec-scalar | `pto.vshls` | `core-i16-unsigned, full-mask, scalar-operand` | compiled | `2026-04-02` 已按 docs 收口为 masked surface，并在本地 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step `4/6` |
| `micro-op/vec-scalar/vshrs` | vec-scalar | `pto.vshrs` | `core-i16-unsigned, full-mask, scalar-operand` | compiled | `2026-04-02` 已按 docs 收口为 masked surface，并在本地 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step `4/6` |
| `micro-op/vec-scalar/vaddcs` | vec-scalar | `pto.vaddcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` + carry mask 覆盖 carry-chain 路径 |
| `micro-op/vec-scalar/vsubcs` | vec-scalar | `pto.vsubcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` + carry mask 覆盖 carry-chain 路径 |
| `micro-op/unary-vector/vabs` | unary-vector | `pto.vabs` | `core-f32, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-tail` | unary-vector | `pto.vabs` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-f16` | unary-vector | `pto.vabs` | `core-f16, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-i16-signed` | unary-vector | `pto.vabs` | `core-i16-signed, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-i16-unsigned` | unary-vector | `pto.vabs` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-f32-exceptional` | unary-vector | `pto.vabs` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-i16-signed-overflow-edge` | unary-vector | `pto.vabs` | `core-i16-signed, full-mask, integer-overflow` | compiled | `2026-04-05` 已补齐静态 case，输入前缀显式覆盖 `INT16_MIN`、`INT16_MIN+1` 等边界值；后续已修正 `vabs` LLVM emission 的类型/lanes 选择，不再硬编码 `v64f32`；定向 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/unary-vector/vabs-i16-signed-overflow-edge'` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vexp` | unary-vector | `pto.vexp` | `core-f32, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vexp-tail` | unary-vector | `pto.vexp` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vexp-f16` | unary-vector | `pto.vexp` | `core-f16, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vexp-f32-exceptional` | unary-vector | `pto.vexp` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vexp-f32-over-underflow` | unary-vector | `pto.vexp` | `core-f32, full-mask, floating-overflow-underflow` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vneg` | unary-vector | `pto.vneg` | `core-f32, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；`VPTOLLVMEmitter` 已支持 `pto.vneg` |
| `micro-op/unary-vector/vln` | unary-vector | `pto.vln` | `core-f32, full-mask, domain-positive` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前已越过 `unsupported op` 阶段 |
| `micro-op/unary-vector/vsqrt` | unary-vector | `pto.vsqrt` | `core-f32, full-mask, domain-nonnegative` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前已越过 `unsupported op` 阶段 |
| `micro-op/unary-vector/vrsqrt` | unary-vector | `pto.vrsqrt` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；`VPTOLLVMEmitter` 已支持 `pto.vrsqrt` |
| `micro-op/unary-vector/vrec` | unary-vector | `pto.vrec` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前已越过 `unsupported op` 阶段 |
| `micro-op/unary-vector/vrelu` | unary-vector | `pto.vrelu` | `core-f32, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前已越过 `unsupported op` 阶段 |
| `micro-op/unary-vector/vnot` | unary-vector | `pto.vnot` | `core-i16-signed, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；`VPTOLLVMEmitter` 已支持 `pto.vnot` |
| `micro-op/unary-vector/vbcnt` | unary-vector | `pto.vbcnt` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；`VPTOLLVMEmitter` 已支持 `pto.vbcnt` |
| `micro-op/unary-vector/vcls` | unary-vector | `pto.vcls` | `core-i16-signed, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；`VPTOLLVMEmitter` 已支持 `pto.vcls` |
| `micro-op/compare-select/vcmp-eq` | compare-select | `pto.vcmp` | `core-f32, full-mask, relation-eq` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmp-lt` | compare-select | `pto.vcmp` | `core-f32, full-mask, relation-lt` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmp-tail` | compare-select | `pto.vcmp` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmp-i16-signed` | compare-select | `pto.vcmp` | `core-i16-signed, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmp-i16-unsigned` | compare-select | `pto.vcmp` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmp-f32-exceptional` | compare-select | `pto.vcmp` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vsel` | compare-select | `pto.vsel` | `core-f32, full-mask` | compiled | `2026-04-03` 本地 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64 CASE_PREFIX=micro-op` 并行回归已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vsel-tail` | compare-select | `pto.vsel` | `core-f32, tail-mask` | compiled | `2026-04-03` 本地 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64 CASE_PREFIX=micro-op` 并行回归已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vsel-i16` | compare-select | `pto.vsel` | `core-i16-signed, full-mask` | compiled | `2026-04-03` 本地 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64 CASE_PREFIX=micro-op` 并行回归已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vselr` | compare-select | `pto.vselr` | `core-f32, full-mask, explicit-lane-index` | compiled | `2026-04-03` 已按 `%result = pto.vselr %src, %idx : !pto.vreg<64xf32>, !pto.vreg<64xi32> -> !pto.vreg<64xf32>` 重写 case；`f32` 路径在 emitter 中按 `bitcast <64xf32> -> <64xi32> -> llvm.hivm.vselr.v64u32 -> bitcast <64xf32>` 方式桥接，单 case `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vselr-f16` | compare-select | `pto.vselr` | `core-f16, full-mask, explicit-lane-index` | compiled | `2026-04-03` 已新增 `f16 + u16 idx -> f16` lane-select case；本地 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/compare-select/vselr-f16'` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vselr-u8` | compare-select | `pto.vselr` | `core-u8, full-mask, explicit-lane-index` | compiled | `2026-04-03` 已新增 `u8 + u8 idx -> u8` lane-select case；本地 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/compare-select/vselr-u8'` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmps-f32` | compare-select | `pto.vcmps` | `core-f32, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmps-tail` | compare-select | `pto.vcmps` | `core-f32, tail-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmps-i16-signed` | compare-select | `pto.vcmps` | `core-i16-signed, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmps-i16-unsigned` | compare-select | `pto.vcmps` | `core-i16-unsigned, full-mask, scalar-operand` | compiled | `2026-04-02` case 仍保持 `ui16` scalar-operand 目标，但改为 `i16 constant + unrealized_conversion_cast -> ui16` 避开与目标无关的 scalar-load lowering 噪音；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/compare-select/vcmps-f32-exceptional` | compare-select | `pto.vcmps` | `core-f32, full-mask, scalar-operand, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/conversion/vcvt-f32-to-f16` | conversion | `pto.vcvt` | `f32-to-f16, full-mask` | compiled | `2026-04-01` 已按 `f32 -> f16` 宽度变化模式重写为 `even/odd + vor`；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/conversion/vcvt-f16-to-f32` | conversion | `pto.vcvt` | `f16-to-f32, full-mask` | compiled | `2026-04-01` 已按 `UNPK_B16 + PART_EVEN/PART_ODD` widening 模式重写；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/conversion/vcvt-tail` | conversion | `pto.vcvt` | `f32-to-f16, tail-mask` | compiled | `2026-04-01` 已按 `LOGICAL_ELEMS=1000` 前缀场景重写为真实 `f32 -> f16` tail case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/conversion/vcvt-f32-special` | conversion | `pto.vcvt` | `f32-to-f16, exceptional-values` | compiled | `2026-04-01` 已按异常值输入重写为真实 `f32 -> f16` narrowing case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/conversion/vcvt-i32-to-i16-overflow` | conversion | `pto.vcvt` | `i32-to-i16, integer-overflow` | compiled | `2026-04-05` 已补齐静态 case，按 `sat = "SAT"` + `part = "EVEN/ODD"` 组合覆盖 `s32 -> s16` narrowing overflow；golden 采用 `int16` 饱和裁剪。当前 `VcvtOp::verify()` 与 `VPTOLLVMEmitter` 已按 `docs/isa/09-conversion-ops.md` 刷新 `vcvt` contract，定向 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step 4/6；同轮 `vcvt-f32-to-f16`、`vcvt-f16-to-f32`、`vcvt-tail`、`vcvt-f32-special`、`vcvt-f16-special`、`vcvt-tail-special` 也均重新通过 compile-only |
| `micro-op/conversion/vtrc-f32-rounding` | conversion | `pto.vtrc` | `core-f32, round-r, round-z, round-f` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/conversion/vtrc-f32-special` | conversion | `pto.vtrc` | `core-f32, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/vbr-f32` | materialization-predicate | `pto.vbr` | `core-f32, scalar-broadcast` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/vbr-i32` | materialization-predicate | `pto.vbr` | `core-i32-signed, scalar-broadcast` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/vdup-scalar` | materialization-predicate | `pto.vdup` | `core-f32, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/vdup-lane` | materialization-predicate | `pto.vdup` | `core-f32, lane-select` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/pset-pattern` | materialization-predicate | `pto.pset_b16`, `pto.pset_b32`, `pto.pset_b8` | `pattern-mask, pat-all, pat-vl` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library。`2026-04-05` repo-generated `.ll` 已确认发射为 `llvm.hivm.pset.b8` / `llvm.hivm.pset.b16` / `llvm.hivm.pset.b32` |
| `micro-op/materialization-predicate/pge-tail-mask` | materialization-predicate | `pto.pge_b16`, `pto.pge_b32`, `pto.pge_b8` | `tail-mask` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library。`2026-04-05` repo-generated `.ll` 已确认发射为 `llvm.hivm.pge.b8` / `llvm.hivm.pge.b16` / `llvm.hivm.pge.b32` |
| `micro-op/materialization-predicate/plt-tail-mask` | materialization-predicate | `pto.plt_b16`, `pto.plt_b32`, `pto.plt_b8` | `tail-mask, scalar-carry-out` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；`plt_b8` emitter 缺口已收敛。`2026-04-05` repo-generated `.ll` 已确认发射为 `{mask, i32}` 双结果 `llvm.hivm.plt.*.v300`，当前只覆盖第二结果存在性/可链式传递，不固定数值递推公式 |
| `micro-op/materialization-predicate/ppack-punpack` | materialization-predicate | `pto.ppack`, `pto.punpack` | `pack-unpack-roundtrip` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library。`2026-04-05` repo-generated `.ll` 已确认发射为 `llvm.hivm.ppack.z` / `llvm.hivm.punpack` |
| `micro-op/materialization-predicate/pdintlv_b8` | materialization-predicate | `pto.pdintlv_b8` | `predicate-transform, lane-order` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library。`2026-04-05` repo-generated `.ll` 已确认发射为 `llvm.hivm.pdintlv.b8` |
| `micro-op/materialization-predicate/pdintlv_b16` | materialization-predicate | `pto.pdintlv_b16` | `predicate-transform, lane-order` | compiled | `2026-04-05` 定向 `DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library。repo-generated `.ll` 已确认发射为 `llvm.hivm.pdintlv.b16` |
| `micro-op/materialization-predicate/pdintlv_b32` | materialization-predicate | `pto.pdintlv_b32` | `predicate-transform, lane-order` | compiled | `2026-04-05` 定向 `DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library。repo-generated `.ll` 已确认发射为 `llvm.hivm.pdintlv.b32` |
| `micro-op/materialization-predicate/pintlv_b8` | materialization-predicate | `pto.pintlv_b8` | `predicate-transform, lane-order` | compiled | `2026-04-05` 定向 `DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library。repo-generated `.ll` 已确认发射为 `llvm.hivm.pintlv.b8` |
| `micro-op/materialization-predicate/pintlv_b16` | materialization-predicate | `pto.pintlv_b16` | `predicate-transform, lane-order` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library。`2026-04-05` repo-generated `.ll` 已确认发射为 `llvm.hivm.pintlv.b16` |
| `micro-op/materialization-predicate/pintlv_b32` | materialization-predicate | `pto.pintlv_b32` | `predicate-transform, lane-order` | compiled | `2026-04-05` 定向 `DEVICE=SIM COMPILE_ONLY=1` 已通过并产出 kernel shared library。repo-generated `.ll` 已确认发射为 `llvm.hivm.pintlv.b32` |
| `micro-op/materialization-predicate/pand` | materialization-predicate | `pto.pand` | `predicate-transform` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；先前 `scf.for` dialect 注册缺口已收敛。`2026-04-05` repo-generated `.ll` 已确认发射为 `llvm.hivm.pand.z` |
| `micro-op/materialization-predicate/por` | materialization-predicate | `pto.por` | `predicate-transform` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；先前 `scf.for` dialect 注册缺口已收敛。`2026-04-05` repo-generated `.ll` 已确认发射为 `llvm.hivm.por.z` |
| `micro-op/materialization-predicate/pxor` | materialization-predicate | `pto.pxor` | `predicate-transform` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；先前 `scf.for` dialect 注册缺口已收敛。`2026-04-05` repo-generated `.ll` 已确认发射为 `llvm.hivm.pxor.z` |
| `micro-op/materialization-predicate/pnot` | materialization-predicate | `pto.pnot` | `predicate-transform` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library。`2026-04-05` repo-generated `.ll` 已确认发射为 `llvm.hivm.pnot.z` |
| `micro-op/materialization-predicate/psel` | materialization-predicate | `pto.psel` | `predicate-transform, predicate-select` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library。`2026-04-05` repo-generated `.ll` 已确认发射为 `llvm.hivm.psel` |
| `micro-op/predicate-load-store/psts-pk-plds-us` | predicate-load-store | `pto.plds`, `pto.psts` | `predicate-load-store-composition, dynamic-offset, load-store-pair-preservation, representative-logical-elements` | compiled | `2026-04-05` 由旧的泛化 `psts-plds` 收紧为明确 `PK -> US` 组合 case；本地 `ptoas --vpto-emit-hivm-llvm` 与 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/predicate-load-store/psts-pk-plds-us'` 已通过 |
| `micro-op/predicate-load-store/psti-pk-pldi-us` | predicate-load-store | `pto.pldi`, `pto.psti` | `predicate-load-store-composition, immediate-offset, load-store-pair-preservation, representative-logical-elements` | compiled | `2026-04-05` 由旧的泛化 `psti-pldi` 收紧为明确 `PK -> US` 组合 case；本地 `ptoas --vpto-emit-hivm-llvm` 与 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/predicate-load-store/psti-pk-pldi-us'` 已通过 |
| `micro-op/predicate-load-store/pstu` | predicate-load-store | `pto.pstu` | `unaligned-predicate-store, state-update, representative-logical-elements` | compiled | `2026-04-02` 已将 case 收紧到 typed `mask<b32> + !pto.ptr<ui32, ub>` contract，并补齐 `llvm.hivm.pstu.b32` emission；本地 `DEVICE=SIM COMPILE_ONLY=1 WORK_SPACE=/tmp/vpto_pstu_compile_only CASE_NAME='micro-op/predicate-load-store/pstu' bash test/vpto/scripts/run_host_vpto_validation.sh` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/predicate-load-store/psts-norm-plds-ds` | predicate-load-store | `pto.plds`, `pto.psts` | `predicate-load-store-composition, dynamic-offset, load-store-pair-preservation, representative-logical-elements` | compiled | `2026-04-05` 新增明确 `NORM -> DS` 组合 case，用于覆盖 load/store 非对偶组合的可观察结果；本地 `ptoas --vpto-emit-hivm-llvm` 与 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/predicate-load-store/psts-norm-plds-ds'` 已通过 |
| `micro-op/predicate-load-store/psti-norm-pldi-ds` | predicate-load-store | `pto.pldi`, `pto.psti` | `predicate-load-store-composition, immediate-offset, load-store-pair-preservation, representative-logical-elements` | compiled | `2026-04-05` 新增明确 `NORM -> DS` 组合 case，用于覆盖 load/store 非对偶组合的可观察结果；本地 `ptoas --vpto-emit-hivm-llvm` 与 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/predicate-load-store/psti-norm-pldi-ds'` 已通过 |
| `micro-op/reduction/vcadd` | reduction | `pto.vcadd` | `core-f32, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcadd-tail` | reduction | `pto.vcadd` | `core-f32, tail-mask, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcmax` | reduction | `pto.vcmax` | `core-f32, result-placement` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library |
| `micro-op/reduction/vcmin` | reduction | `pto.vcmin` | `core-f32, result-placement` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library |
| `micro-op/reduction/vcgadd` | reduction | `pto.vcgadd` | `group-reduction, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcgmax` | reduction | `pto.vcgmax` | `group-reduction, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcgmin` | reduction | `pto.vcgmin` | `group-reduction, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcpadd` | reduction | `pto.vcpadd` | `prefix-op, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vlds` | vector-load-store | `pto.vlds` | `core-f32, contiguous, full-mask, aligned, dist-norm` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vlds-tail` | vector-load-store | `pto.vlds` | `core-f32, contiguous, tail-mask, aligned, dist-norm` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vlds-brc-b32` | vector-load-store | `pto.vlds` | `core-f32, full-mask, aligned, dist-brc-b32` | compiled | `2026-04-03` 已按 `tmp-vlds-dist.md` 收口 docs/verifier/emitter；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vlds-brc-b16` | vector-load-store | `pto.vlds` | `core-f16, full-mask, aligned, dist-brc-b16` | compiled | `2026-04-03` 新增 representative dist case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vlds-us-b16` | vector-load-store | `pto.vlds` | `core-i16, full-mask, aligned, dist-us-b16` | compiled | `2026-04-03` 新增 representative dist case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vlds-ds-b16` | vector-load-store | `pto.vlds` | `core-i16, full-mask, aligned, dist-ds-b16` | compiled | `2026-04-03` 新增 representative dist case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vlds-brc-blk` | vector-load-store | `pto.vlds` | `core-u8, full-mask, aligned, dist-brc-blk` | compiled | `2026-04-03` 新增 representative dist case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vsts` | vector-load-store | `pto.vsts` | `core-f32, contiguous, full-mask, aligned, dist-norm` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vsts-1pt-b16` | vector-load-store | `pto.vsts` | `core-i16, full-mask, aligned, dist-1pt-b16` | compiled | `2026-04-03` 已按 `tmp-vsts-dist.md` 收口 docs/verifier/emitter；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vsts-pk-b16` | vector-load-store | `pto.vsts` | `core-i16, full-mask, aligned, dist-pk-b16` | compiled | `2026-04-03` 新增 representative dist case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vsts-mrg2chn-b16` | vector-load-store | `pto.vsts` | `core-i16, full-mask, aligned, dist-mrg2chn-b16` | compiled | `2026-04-03` 新增 representative dist case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vsts-mrg4chn-b8` | vector-load-store | `pto.vsts` | `core-i8, full-mask, aligned, dist-mrg4chn-b8` | compiled | `2026-04-03` 新增 representative dist case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vldas-vldus` | vector-load-store | `pto.vldas`, `pto.vldus` | `core-f32, full-mask, unaligned, stream-state` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vldsx2-vstsx2` | vector-load-store | `pto.vldsx2`, `pto.vstsx2` | `core-f32, full-mask, paired-roundtrip, dintlv` | compiled | `2026-04-04` 已按与 `vsts` 同构的 typed LLVM ABI 收口；`vstsx2` 仅把单 `src` 扩成双 `src`。本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vsldb` | vector-load-store | `pto.vsldb` | `core-f32, full-mask, block-strided-load, block-mask` | compiled | `2026-04-03` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；case 已切到 `%block_stride + %repeat_stride` surface |
| `micro-op/gather-scatter/vscatter` | gather-scatter | `pto.vscatter` | `core-f32, full-mask, non-contiguous, explicit-index-pattern, scatter-store, store-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vsstb` | vector-load-store | `pto.vsstb` | `core-f32, full-mask, block-strided-store, block-mask` | compiled | `2026-04-03` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；case 已切到 `%block_stride + %repeat_stride` surface |
| `micro-op/vector-load-store/vstar` | vector-load-store | `pto.vstar` | `core-f32, full-mask, aligned, state-update` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vstur` | vector-load-store | `pto.vstur` | `core-f32, full-mask, unaligned, state-update` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library |
| `micro-op/gather-scatter/vgather2` | gather-scatter | `pto.vgather2` | `core-f32, full-mask, non-contiguous, explicit-index-pattern, load-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/gather-scatter/vgatherb` | gather-scatter | `pto.vgatherb` | `core-f32, full-mask, block-gather, aligned-base, load-effect-validation, no-alias` | compiled | `2026-04-04` 已按 `visa.txt` 把 surface 收口为 `%source + %offsets + %mask`，并将 offset 语义改为 32B block byte-offset；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/gather-scatter/vgather2_bc` | gather-scatter | `pto.vgather2_bc` | `core-f32, full-mask, non-contiguous, masked-gather, load-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/rearrangement/vintlv-vdintlv` | rearrangement | `pto.vdintlv`, `pto.vintlv` | `paired-roundtrip, lane-order` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；`vintlv/vdintlv` 双结果 LLVM emission 已接通 |
| `micro-op/rearrangement/vslide` | rearrangement | `pto.vslide` | `lane-order, slide-window` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/rearrangement/vsqz` | rearrangement | `pto.vsqz` | `predicate-driven-rearrangement, stable-order` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；`store` 参数收紧为 installed stub 明确的 `i32` 后通过 |
| `micro-op/rearrangement/vusqz` | rearrangement | `pto.vusqz` | `predicate-driven-rearrangement, placement` | compiled | `2026-04-02` docs/ODS/verifier 已统一为 `pto.vusqz %src, %mask`，LLVM emitter 已接入 `llvm.hivm.vusqz.*.m`；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/rearrangement/vpack` | rearrangement | `pto.vpack` | `pack-unpack, narrowing, half-placement, zero-fill-other-half` | compiled | `2026-04-02` docs/ODS/verifier 已统一为单输入 `%src, "LOWER"|"HIGHER"`；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`，case 同时覆盖 LOWER/HIGHER 半区落点与另一半补零 |
| `micro-op/rearrangement/vsunpack` | rearrangement | `pto.vsunpack` | `pack-unpack, sign-extend` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；`part` 参数收紧为 installed stub 明确的 `i32` 后通过 |
| `micro-op/rearrangement/vzunpack` | rearrangement | `pto.vzunpack` | `pack-unpack, zero-extend` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；case 类型收紧为 `ui16 -> ui32` 且 `part` 收紧为 `i32` 后通过 |
| `micro-op/rearrangement/vintlv-vdintlv-lane-boundary` | rearrangement | `pto.vdintlv`, `pto.vintlv` | `paired-roundtrip, lane-order` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；与主 case 共享同一双结果 emission 路径 |
| `micro-op/rearrangement/vslide-tail-window` | rearrangement | `pto.vslide` | `lane-order, slide-window, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；与主 case 共享同一 emission 路径 |
| `micro-op/rearrangement/vsqz-nontrivial-mask` | rearrangement | `pto.vsqz` | `predicate-driven-rearrangement, stable-order` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；与主 case 共享同一 `vsqz` emission 路径 |
| `micro-op/rearrangement/vusqz-nontrivial-mask` | rearrangement | `pto.vusqz` | `predicate-driven-rearrangement, placement` | compiled | `2026-04-02` 已改为稀疏 placement mask 的有效 testcase；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/dsa-sfu/vlrelu-f32` | dsa-sfu | `pto.vlrelu` | `core-f32, scalar-operand, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vlrelu-tail` | dsa-sfu | `pto.vlrelu` | `core-f32, tail-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vlrelu-f16` | dsa-sfu | `pto.vlrelu` | `core-f16, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vprelu-f32` | dsa-sfu | `pto.vprelu` | `core-f32, vector-alpha` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library |
| `micro-op/dsa-sfu/vexpdiff-f32` | dsa-sfu | `pto.vexpdiff` | `core-f32, fused-expdiff` | compiled | `2026-04-03` 已按 installed wrapper contract 收口为 `input/max + part -> f32`，本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/dsa-sfu/vexpdiff-f16-part` | dsa-sfu | `pto.vexpdiff` | `core-f16, fused-expdiff, part-even-odd` | compiled | `2026-04-03` 已新增 `f16 -> f32` case，同一 kernel 内分别验证 `EVEN`/`ODD` 两种 part；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/dsa-sfu/vaxpy-f32` | dsa-sfu | `pto.vaxpy` | `core-f32, scalar-operand, fused-op` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library |
| `micro-op/dsa-sfu/vmull` | dsa-sfu | `pto.vmull` | `widening-op, hi-lo-split` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；`vmull` emitter 缺口已收敛 |
| `micro-op/dsa-sfu/vmula` | dsa-sfu | `pto.vmula` | `core-f32, fused-op, accumulator` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vci` | dsa-sfu / conversion | `pto.vci` | `index-generation` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library |
| `micro-op/dsa-sfu/vbitsort` | dsa-sfu | `pto.vbitsort` | `index-generation, layout-transform` | compiled | `2026-04-05` 已补齐静态 case：`f32 score + u32 index -> packed proposal records`，覆盖降序排序、stable tie-break 与 8B record 布局。随后按 installed Bisheng frontend trace 将 emitter 接到 `llvm.hivm.VBS32.V300.f32(ptr6 dst, ptr6 src0, ptr6 src1, i64 config)`，并按 wrapper 语义将 `%repeat_times` 打包为 `repeat << 56`；定向 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/dsa-sfu/vbitsort'` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vmuls-tail` | vec-scalar | `pto.vmuls` | `core-f32, tail-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vmaxs-tail` | vec-scalar | `pto.vmaxs` | `core-f32, tail-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vmins-tail` | vec-scalar | `pto.vmins` | `core-f32, tail-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vshls-shift-boundary` | vec-scalar | `pto.vshls` | `core-i16-unsigned, full-mask, scalar-operand` | compiled | `2026-04-02` 已按 docs 收口为 masked surface，并在本地 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step `4/6` |
| `micro-op/vec-scalar/vshrs-shift-boundary` | vec-scalar | `pto.vshrs` | `core-i16-unsigned, full-mask, scalar-operand` | compiled | `2026-04-02` 已按 docs 收口为 masked surface，并在本地 `DEVICE=SIM COMPILE_ONLY=1` 下走到 step `4/6` |
| `micro-op/vec-scalar/vaddcs-carry-boundary` | vec-scalar | `pto.vaddcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` + carry mask 覆盖 boundary 场景 |
| `micro-op/vec-scalar/vsubcs-borrow-boundary` | vec-scalar | `pto.vsubcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` + carry mask 覆盖 boundary 场景 |
| `micro-op/unary-vector/vln-domain-boundary` | unary-vector | `pto.vln` | `core-f32, domain-positive, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前 compile-only 可过 |
| `micro-op/unary-vector/vsqrt-domain-boundary` | unary-vector | `pto.vsqrt` | `core-f32, domain-nonnegative, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前 compile-only 可过 |
| `micro-op/unary-vector/vrsqrt-zero-inf` | unary-vector | `pto.vrsqrt` | `core-f32, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前 compile-only 可过 |
| `micro-op/unary-vector/vrec-zero-inf` | unary-vector | `pto.vrec` | `core-f32, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前 compile-only 可过 |
| `micro-op/unary-vector/vneg-f32-exceptional` | unary-vector | `pto.vneg` | `core-f32, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前 compile-only 可过 |
| `micro-op/binary-vector/vsub-tail` | binary-vector | `pto.vsub` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vmul-tail` | binary-vector | `pto.vmul` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vmax-tail` | binary-vector | `pto.vmax` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vand-mask-edge` | binary-vector | `pto.vand` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vor-mask-edge` | binary-vector | `pto.vor` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vxor-mask-edge` | binary-vector | `pto.vxor` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vshl-shift-boundary` | binary-vector | `pto.vshl` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vshr-shift-boundary` | binary-vector | `pto.vshr` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vaddc-carry-boundary` | binary-vector | `pto.vaddc` | `core-i16-unsigned, full-mask, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` 覆盖 boundary 场景 |
| `micro-op/binary-vector/vsubc-borrow-boundary` | binary-vector | `pto.vsubc` | `core-i16-unsigned, full-mask, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` 覆盖 boundary 场景 |
| `micro-op/conversion/vcvt-f16-special` | conversion | `pto.vcvt` | `f16-to-f32, exceptional-values` | compiled | `2026-04-01` 已按异常值输入重写为真实 `f16 -> f32` widening case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/conversion/vtrc-rounding-boundary` | conversion | `pto.vtrc` | `core-f32, round-r, round-z, round-f` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/conversion/vcvt-tail-special` | conversion | `pto.vcvt` | `f32-to-f16, tail-mask, exceptional-values` | compiled | `2026-04-01` 已按异常值前缀 + `LOGICAL_ELEMS=1000` 场景重写为真实 `f32 -> f16` tail case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/compare-select/vcmp-unordered-f32` | compare-select | `pto.vcmp` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmps-unordered-f32` | compare-select | `pto.vcmps` | `core-f32, full-mask, scalar-operand, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vsel-predicate-edge` | compare-select | `pto.vsel` | `core-f32, full-mask` | compiled | `2026-04-03` 本地 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64 CASE_PREFIX=micro-op` 并行回归已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/pset-pattern-fragment` | materialization-predicate | `pto.pset_b16`, `pto.pset_b32`, `pto.pset_b8` | `pattern-mask, pat-vl, representative-logical-elements` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library |
| `micro-op/materialization-predicate/pge-tail-mask-boundary` | materialization-predicate | `pto.pge_b16`, `pto.pge_b32`, `pto.pge_b8` | `tail-mask, representative-logical-elements` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library |
| `micro-op/materialization-predicate/plt-tail-mask-boundary` | materialization-predicate | `pto.plt_b16`, `pto.plt_b32`, `pto.plt_b8` | `tail-mask, scalar-carry-out, representative-logical-elements` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library。当前只覆盖第二结果存在性/可链式传递，不固定数值递推公式 |
| `micro-op/materialization-predicate/ppack-punpack-nontrivial` | materialization-predicate | `pto.ppack`, `pto.punpack` | `pack-unpack-roundtrip, representative-logical-elements` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library |
| `micro-op/materialization-predicate/pdintlv_b8-nontrivial` | materialization-predicate | `pto.pdintlv_b8` | `predicate-transform, lane-order` | compiled | `2026-04-03` 本地 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64 CASE_PREFIX=micro-op` 并行回归已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/pdintlv_b16-nontrivial` | materialization-predicate | `pto.pdintlv_b16` | `predicate-transform, lane-order` | compiled | `2026-04-05` 定向 `DEVICE=SIM COMPILE_ONLY=1 JOBS=8 CASE_PREFIX=micro-op/materialization-predicate/pdintlv_b` 已通过并产出 kernel shared library |
| `micro-op/materialization-predicate/pdintlv_b32-nontrivial` | materialization-predicate | `pto.pdintlv_b32` | `predicate-transform, lane-order` | compiled | `2026-04-05` 定向 `DEVICE=SIM COMPILE_ONLY=1 JOBS=8 CASE_PREFIX=micro-op/materialization-predicate/pdintlv_b` 已通过并产出 kernel shared library |
| `micro-op/materialization-predicate/pintlv_b8-nontrivial` | materialization-predicate | `pto.pintlv_b8` | `predicate-transform, lane-order` | compiled | `2026-04-05` 定向 `DEVICE=SIM COMPILE_ONLY=1 JOBS=8 CASE_PREFIX=micro-op/materialization-predicate/pintlv_b` 已通过并产出 kernel shared library |
| `micro-op/materialization-predicate/pintlv_b16-nontrivial` | materialization-predicate | `pto.pintlv_b16` | `predicate-transform, lane-order` | compiled | `2026-04-03` 本地 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64 CASE_PREFIX=micro-op` 并行回归已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/pintlv_b32-nontrivial` | materialization-predicate | `pto.pintlv_b32` | `predicate-transform, lane-order` | compiled | `2026-04-05` 定向 `DEVICE=SIM COMPILE_ONLY=1 JOBS=8 CASE_PREFIX=micro-op/materialization-predicate/pintlv_b` 已通过并产出 kernel shared library |
| `micro-op/materialization-predicate/psel-tail-predicate` | materialization-predicate | `pto.psel` | `predicate-transform, predicate-select, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/predicate-load-store/psts-pk-plds-us-prefix-boundary` | predicate-load-store | `pto.plds`, `pto.psts` | `predicate-load-store-composition, dynamic-offset, load-store-pair-preservation, representative-logical-elements` | compiled | `2026-04-05` 由旧的泛化 packed-prefix case 收紧为明确 `PK -> US` 前缀边界 case；本地 `ptoas --vpto-emit-hivm-llvm` 与 `DEVICE=SIM COMPILE_ONLY=1 CASE_NAME='micro-op/predicate-load-store/psts-pk-plds-us-prefix-boundary'` 已通过 |
| `micro-op/predicate-load-store/pstu-state-advance-boundary` | predicate-load-store | `pto.pstu` | `unaligned-predicate-store, state-update, boundary, b16-mask, typed-ptr-b16` | compiled | `2026-04-02` 已改写为 typed `mask<b16> + !pto.ptr<ui16, ub>` 的链式 `pstu` state-advance boundary case，并改为直接观测 raw packed buffer；本地 `DEVICE=SIM COMPILE_ONLY=1 WORK_SPACE=/tmp/vpto_pstu_state_advance_boundary_compile_only CASE_NAME='micro-op/predicate-load-store/pstu-state-advance-boundary' bash test/vpto/scripts/run_host_vpto_validation.sh` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcgadd-tail` | reduction | `pto.vcgadd` | `group-reduction, tail-mask, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcgmax-tie` | reduction | `pto.vcgmax` | `group-reduction, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcgmin-tie` | reduction | `pto.vcgmin` | `group-reduction, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcpadd-tail` | reduction | `pto.vcpadd` | `prefix-op, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vsts-tail` | vector-load-store | `pto.vsts` | `core-f32, contiguous, tail-mask, aligned, dist-norm` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vldas-vldus-state-chain` | vector-load-store | `pto.vldas`, `pto.vldus` | `core-f32, full-mask, unaligned, stream-state, state-update` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vldsx2-layout-check` | vector-load-store | `pto.vldsx2` | `core-f32, full-mask, paired-roundtrip, dintlv, lane-order` | compiled | `2026-04-04` `vldsx2/vstsx2` LLVM ABI 已收口；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vstsx2-layout-check` | vector-load-store | `pto.vstsx2` | `core-f32, full-mask, paired-roundtrip, dintlv, lane-order` | compiled | `2026-04-04` `vstsx2` 已按 `vsts` 同构 ABI 收口为双 `src` 形式；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vstas-vstus-offset-update` | vector-load-store | `pto.vstas`, `pto.vstus` | `core-f32, full-mask, immediate-offset, state-update` | compiled | `2026-04-02` 已补齐 `pto.vstus` / `pto.vstas` 的 VPTO LLVM emitter 支持；本地 `build/tools/ptoas/ptoas test/vpto/cases/micro-op/vector-load-store/vstas-vstus-offset-update/kernel.pto --pto-arch a5 --pto-backend=vpto --vpto-emit-hivm-llvm -o /tmp/vstas_vstus_offset_update.ll` 可成功导出 LLVM 文本，导出形态对齐 installed wrapper：`llvm.hivm.vstus.post(..., offset_bytes, align)` + `llvm.hivm.vstas(..., offset_bytes, 0)` |
| `micro-op/gather-scatter/vgather2-duplicate-index` | gather-scatter | `pto.vgather2` | `core-f32, non-contiguous, explicit-index-pattern, load-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/gather-scatter/vgather2_bc-sparse-mask` | gather-scatter | `pto.vgather2_bc` | `core-f32, masked-gather, load-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/gather-scatter/vgatherb-block-boundary` | gather-scatter | `pto.vgatherb` | `core-f32, block-gather, aligned-base, load-effect-validation, no-alias` | compiled | `2026-04-04` 已随主 case 切换到 `%source + %offsets + %mask` surface；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/gather-scatter/vscatter-out-of-order-index` | gather-scatter | `pto.vscatter` | `core-f32, explicit-index-pattern, scatter-store, store-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vlrelu-f32-exceptional` | dsa-sfu | `pto.vlrelu` | `core-f32, scalar-operand, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vprelu-tail` | dsa-sfu | `pto.vprelu` | `core-f32, vector-alpha, tail-mask` | compiled | `2026-04-04` 全量 `DEVICE=SIM COMPILE_ONLY=1 JOBS=64` 回归已通过并产出 kernel shared library |
| `micro-op/dsa-sfu/vexpdiff-boundary` | dsa-sfu | `pto.vexpdiff` | `core-f32, fused-expdiff, exceptional-values, floating-overflow-underflow` | compiled | `2026-04-03` 已按 installed wrapper contract 收口为 `input/max + part -> f32`，本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/dsa-sfu/vmula-accumulator-boundary` | dsa-sfu | `pto.vmula` | `core-f32, fused-op, accumulator` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |

## Notes

- `case` 字段记录相对 `test/vpto/cases/` 的真实 case 路径；微指令单-op 例如 `micro-op/binary-vector/vadd`
- `tileop/` 下的 case 表示 tile 级或派生组合验证，不直接计入向量单 op 覆盖完成态
- 历史 `tileop/*` 或其他已有 case 只能作为骨架参考，不能直接填写到微指令单 op 覆盖条目的 `case` 字段里。
- 已转入文档漂移核对单的口径问题，不继续在本 matrix 中单独记账；待结论明确后再回填对应条目。
- 随执行推进，这份 matrix 应同步更新 `case`、`scenarios` 和 `status`，作为唯一静态追踪来源。
