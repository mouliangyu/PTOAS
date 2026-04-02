# VPTO Op Board Unit Tests Matrix

## Legend

- `infra`: 基础设施 op，只作为其他 case 的准备动作或收尾动作复用
- `planned`: 已纳入本轮范围，待补 case
- `implemented`: case 已落地；即便 parser / verifier / lowering / codegen / runtime / board 失败，也仍记为 `implemented`，并在 `notes` 写明失败点
- `compiled`: case 已落地，且 compile-only 路径已走到编译产物；尚未完成 runtime / board 验证
- `board-passed`: 已完成上板验证
- `blocked`: 当前无法仅依据 `docs/vpto-spec.md` 与 `docs/isa/` 写出语义明确的 case；只用于文档层面的不可写阻塞

## Latest Compile Scan

- 最新 micro-op compile-only 扫描命令：
  - `source scripts/ptoas_env.sh && WORK_SPACE=/tmp/vpto-micro-op-sim-compile-rerun DEVICE=SIM COMPILE_ONLY=1 JOBS=64 CASE_PREFIX=micro-op test/vpto/scripts/run_host_vpto_validation_parallel.sh`
- 最新结果：
  - 总计 `223`，`PASS=88`，`FAIL=135`
  - 汇总文件：`/tmp/vpto-micro-op-sim-compile-rerun/parallel-summary.tsv`
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
| `pto.vlds` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vlds` | `core-f32, contiguous, full-mask, aligned, dist-norm` | 还需补 `vlds-tail` 与 `vlds-brc-b32` 变体 |
| `pto.vldas` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vldas-vldus` | `core-f32, full-mask, unaligned, stream-state` | 与 `pto.vldus` 成组验证 |
| `pto.vldus` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vldas-vldus` | `core-f32, full-mask, unaligned, stream-state` | 与 `pto.vldas` 成组验证 |
| `pto.vldx2` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vldx2-vstx2` | `core-f32, full-mask, paired-roundtrip, dintlv` | 与 `pto.vstx2` 成组验证 |
| `pto.vsld` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vsld` | `core-f32, full-mask, strided-load` | |
| `pto.vsldb` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vsldb` | `core-f32, full-mask, block-strided-load, block-mask` | |
| `pto.vgather2` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | `micro-op/gather-scatter/vgather2` | `core-f32, full-mask, non-contiguous, explicit-index-pattern, load-effect-validation, no-alias` | |
| `pto.vgatherb` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | `micro-op/gather-scatter/vgatherb` | `core-f32, full-mask, block-gather, aligned-base, load-effect-validation, no-alias` | |
| `pto.vgather2_bc` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | `micro-op/gather-scatter/vgather2_bc` | `core-f32, full-mask, non-contiguous, masked-gather, load-effect-validation, no-alias` | |
| `pto.vsts` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vsts` | `core-f32, contiguous, full-mask, aligned, dist-norm` | |
| `pto.vstx2` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vldx2-vstx2` | `core-f32, full-mask, paired-roundtrip, dintlv` | 与 `pto.vldx2` 成组验证 |
| `pto.vsst` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vsst` | `core-f32, full-mask, strided-store` | |
| `pto.vsstb` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vsstb` | `core-f32, full-mask, block-strided-store, block-mask` | |
| `pto.vscatter` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | `micro-op/gather-scatter/vscatter` | `core-f32, full-mask, non-contiguous, explicit-index-pattern, scatter-store, store-effect-validation, no-alias` | |
| `pto.vsta` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vsta` | `core-f32, full-mask, aligned, state-update` | |
| `pto.vstas` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vstas` | `core-f32, full-mask, aligned, immediate-offset, state-update` | |
| `pto.vstar` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vstar` | `core-f32, full-mask, aligned, state-update` | |
| `pto.vstu` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vstu` | `core-f32, full-mask, unaligned, state-update` | |
| `pto.vstus` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vstus` | `core-f32, full-mask, unaligned, immediate-offset, state-update` | |
| `pto.vstur` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | `micro-op/vector-load-store/vstur` | `core-f32, full-mask, unaligned, state-update` | |
| `pto.plds` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/psts-plds` | `packed-predicate-roundtrip, scalar-offset, load-store-pair-preservation, representative-logical-elements` | 与 `pto.psts` 成组验证 |
| `pto.pld` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/pst-pld` | `packed-predicate-roundtrip, areg-offset, load-store-pair-preservation, representative-logical-elements` | 与 `pto.pst` 成组验证 |
| `pto.pldi` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/psti-pldi` | `packed-predicate-roundtrip, immediate-offset, load-store-pair-preservation, representative-logical-elements` | 与 `pto.psti` 成组验证 |
| `pto.psts` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/psts-plds` | `packed-predicate-roundtrip, scalar-offset, load-store-pair-preservation, representative-logical-elements` | 与 `pto.plds` 成组验证 |
| `pto.pst` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/pst-pld` | `packed-predicate-roundtrip, areg-offset, load-store-pair-preservation, representative-logical-elements` | 与 `pto.pld` 成组验证 |
| `pto.psti` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/psti-pldi` | `packed-predicate-roundtrip, immediate-offset, load-store-pair-preservation, representative-logical-elements` | 与 `pto.pldi` 成组验证 |
| `pto.pstu` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | `micro-op/predicate-load-store/pstu` | `unaligned-packed-store, state-update, representative-logical-elements` | |
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
| `pto.pintlv_b16` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | `micro-op/materialization-predicate/pintlv_b16` | `predicate-transform, lane-order` | |
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
| `pto.vmov` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | `micro-op/unary-vector/vmov` | `core-f32, full-mask` | |
| `pto.vadd` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vadd` | `core-f32, full-mask` | 另有 tail、`f16`、整型与异常值/溢出变体 |
| `pto.vsub` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vsub` | `core-f32, full-mask` | |
| `pto.vmul` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vmul` | `core-f32, full-mask` | |
| `pto.vdiv` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | `micro-op/binary-vector/vdiv` | `core-f32, full-mask` | 另有 tail、`f16`、`bf16` 与异常值变体 |
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
| `pto.vsubs` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vsubs` | `core-f32, full-mask, scalar-operand` | |
| `pto.vmuls` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vmuls` | `core-f32, full-mask, scalar-operand` | |
| `pto.vmaxs` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vmaxs` | `core-f32, full-mask, scalar-operand` | |
| `pto.vmins` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vmins` | `core-f32, full-mask, scalar-operand` | |
| `pto.vands` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vands` | `core-i16-unsigned, full-mask, scalar-operand` | |
| `pto.vors` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vors` | `core-i16-unsigned, full-mask, scalar-operand` | |
| `pto.vxors` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vxors` | `core-i16-unsigned, full-mask, scalar-operand` | |
| `pto.vshls` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vshls` | `core-i16-unsigned, full-mask, scalar-operand` | |
| `pto.vshrs` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vshrs` | `core-i16-unsigned, full-mask, scalar-operand` | |
| `pto.vlrelu` | dsa-sfu | `docs/isa/08-vec-scalar-ops.md`, `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vlrelu-f32` | `core-f32, scalar-operand, full-mask` | 文档双重收录；测试台账统一归入 `dsa-sfu` |
| `pto.vaddcs` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vaddcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | |
| `pto.vsubcs` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | `micro-op/vec-scalar/vsubcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | |
| `pto.vcvt` | conversion | `docs/isa/09-conversion-ops.md` | yes | `micro-op/conversion/vcvt-f32-to-f16` | `f32-to-f16, full-mask` | 另有 widening、tail、异常值与阻塞的整型溢出变体 |
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
| `pto.vselr` | compare-select | `docs/isa/11-compare-select.md` | yes | `micro-op/compare-select/vselr` | `core-f32, full-mask, reversed-select` | 语义待定，当前按 `blocked` 管理 |
| `pto.vintlv` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vintlv-vdintlv` | `paired-roundtrip, lane-order` | 与 `pto.vdintlv` 成组验证 |
| `pto.vdintlv` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vintlv-vdintlv` | `paired-roundtrip, lane-order` | 与 `pto.vintlv` 成组验证 |
| `pto.vslide` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vslide` | `lane-order, slide-window` | |
| `pto.vshift` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vshift` | `lane-order, zero-fill` | |
| `pto.vsqz` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vsqz` | `predicate-driven-rearrangement, stable-order` | |
| `pto.vusqz` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vusqz` | `predicate-driven-rearrangement, placement` | |
| `pto.vperm` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vperm` | `lane-order, explicit-index-pattern` | |
| `pto.vpack` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vpack` | `pack-unpack, narrowing` | |
| `pto.vsunpack` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vsunpack` | `pack-unpack, sign-extend` | |
| `pto.vzunpack` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | `micro-op/rearrangement/vzunpack` | `pack-unpack, zero-extend` | |
| `pto.vprelu` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vprelu-f32` | `core-f32, vector-alpha` | LLVM 定义参数列表待确认；当前观察是 `3 vreg + 1 mask`，与现有 PTO surface 未收口 |
| `pto.vexpdiff` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vexpdiff-f32` | `core-f32, fused-expdiff` | LLVM 定义参数列表待确认；当前观察是 `2 vreg + 1 mask + 1 scalar`，与现有 PTO surface 未收口 |
| `pto.vaddrelu` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vaddrelu-f32` | `core-f32, fused-op` | LLVM 定义参数列表待确认；当前 PTO surface 与 LLVM 参数列表关系未明确 |
| `pto.vsubrelu` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vsubrelu-f32` | `core-f32, fused-op` | LLVM 定义参数列表待确认；当前 PTO surface 与 LLVM 参数列表关系未明确 |
| `pto.vaxpy` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vaxpy-f32` | `core-f32, scalar-operand, fused-op` | |
| `pto.vaddreluconv` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vaddreluconv` | `fused-op, conversion-result` | 具体类型对如不稳定需转 `blocked` |
| `pto.vmulconv` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vmulconv` | `fused-op, conversion-result` | 具体类型对如不稳定需转 `blocked` |
| `pto.vmull` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vmull` | `widening-op, hi-lo-split` | |
| `pto.vmula` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vmula` | `core-f32, fused-op, accumulator` | |
| `pto.vci` | dsa-sfu / conversion | `docs/isa/13-dsa-sfu-ops.md`, `docs/isa/09-conversion-ops.md` | yes | `micro-op/dsa-sfu/vci` | `index-generation` | |
| `pto.vtranspose` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vtranspose` | `ub-to-ub, layout-transform, representative-config` | UB-to-UB op，不是 `vreg -> vreg` |
| `pto.vbitsort` | dsa-sfu | `docs/vpto-spec.md`, `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vbitsort` | `index-generation, layout-transform` | 当前文档只给出 surface/接口层信息，尚未形成稳定 oracle |

## Case Matrix

| case | family | target_ops | scenarios | status | notes |
| --- | --- | --- | --- | --- | --- |
| `micro-op/binary-vector/vadd` | binary-vector | `pto.vadd` | `core-f32, full-mask` | board-passed | |
| `micro-op/binary-vector/vadd-tail` | binary-vector | `pto.vadd` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vadd-f16` | binary-vector | `pto.vadd` | `core-f16, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vadd-bf16` | binary-vector | `pto.vadd` | `core-bf16, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vadd-i16-signed` | binary-vector | `pto.vadd` | `core-i16-signed, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vadd-i16-unsigned` | binary-vector | `pto.vadd` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vadd-i16-signed-overflow` | binary-vector | `pto.vadd` | `core-i16-signed, full-mask, integer-overflow` | blocked | `i16` overflow case 仍缺少 dtype-specific 测例骨架与稳定 overflow oracle |
| `micro-op/binary-vector/vadd-i16-unsigned-overflow` | binary-vector | `pto.vadd` | `core-i16-unsigned, full-mask, integer-overflow` | blocked | `ui16` overflow case 仍缺少 dtype-specific 测例骨架与稳定 overflow oracle |
| `micro-op/binary-vector/vadd-f32-exceptional` | binary-vector | `pto.vadd` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vsub` | binary-vector | `pto.vsub` | `core-f32, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vmul` | binary-vector | `pto.vmul` | `core-f32, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vdiv` | binary-vector | `pto.vdiv` | `core-f32, full-mask` | board-passed | |
| `micro-op/binary-vector/vdiv-tail` | binary-vector | `pto.vdiv` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vdiv-f16` | binary-vector | `pto.vdiv` | `core-f16, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/binary-vector/vdiv-bf16` | binary-vector | `pto.vdiv` | `core-bf16, full-mask` | blocked | `docs/isa/07-binary-vector-ops.md` 当前未将 `bf16` 列入 `pto.vdiv` 的 A5 types |
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
| `micro-op/vec-scalar/vadds-f16` | vec-scalar | `pto.vadds` | `core-f16, full-mask, scalar-operand` | blocked | `docs/isa/08-vec-scalar-ops.md` 仅给出通用 `T` 语法，尚未明确 `pto.vadds` 的 A5 type 集合 |
| `micro-op/vec-scalar/vadds-bf16` | vec-scalar | `pto.vadds` | `core-bf16, full-mask, scalar-operand` | blocked | `docs/isa/08-vec-scalar-ops.md` 仅给出通用 `T` 语法，尚未明确 `pto.vadds` 的 A5 type 集合 |
| `micro-op/vec-scalar/vadds-i16-signed` | vec-scalar | `pto.vadds` | `core-i16-signed, full-mask, scalar-operand` | blocked | signed integer legality 仍需 `docs/vpto-spec.md` 与 `docs/isa/08-vec-scalar-ops.md` 的交集进一步固化 |
| `micro-op/vec-scalar/vadds-i16-unsigned` | vec-scalar | `pto.vadds` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | unsigned integer legality 仍需 `docs/vpto-spec.md` 与 `docs/isa/08-vec-scalar-ops.md` 的交集进一步固化 |
| `micro-op/vec-scalar/vadds-f32-exceptional` | vec-scalar | `pto.vadds` | `core-f32, full-mask, scalar-operand, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vadds-i16-signed-overflow` | vec-scalar | `pto.vadds` | `core-i16-signed, full-mask, scalar-operand, integer-overflow` | blocked | `docs/isa/08-vec-scalar-ops.md` 尚未给出明确 A5 types 与 overflow 规则，暂不固化 oracle |
| `micro-op/vec-scalar/vadds-i16-unsigned-overflow` | vec-scalar | `pto.vadds` | `core-i16-unsigned, full-mask, scalar-operand, integer-overflow` | blocked | `docs/isa/08-vec-scalar-ops.md` 尚未给出明确 A5 types 与 overflow 规则，暂不固化 oracle |
| `micro-op/vec-scalar/vsubs` | vec-scalar | `pto.vsubs` | `core-f32, full-mask, scalar-operand` | blocked | 当前 `docs/isa/08-vec-scalar-ops.md` 定义了 `pto.vsubs` surface，但 installed Clang headers 与 `strings bisheng` 未观察到对应 `vsubs` wrapper / `llvm.hivm.vsubs.*` intrinsic；docs surface 与 installed toolchain 支持面未收口 |
| `micro-op/vec-scalar/vmuls` | vec-scalar | `pto.vmuls` | `core-f32, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vmaxs` | vec-scalar | `pto.vmaxs` | `core-f32, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vmins` | vec-scalar | `pto.vmins` | `core-f32, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vands` | vec-scalar | `pto.vands` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | `docs/isa/08-vec-scalar-ops.md` 有 surface，但 installed headers / `strings bisheng` 未观察到 `vands` 对应 contract；先按 docs/toolchain 未收口管理 |
| `micro-op/vec-scalar/vors` | vec-scalar | `pto.vors` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | `docs/isa/08-vec-scalar-ops.md` 有 surface，但 installed headers / `strings bisheng` 未观察到 `vors` 对应 contract；先按 docs/toolchain 未收口管理 |
| `micro-op/vec-scalar/vxors` | vec-scalar | `pto.vxors` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | `docs/isa/08-vec-scalar-ops.md` 有 surface，但 installed headers / `strings bisheng` 未观察到 `vxors` 对应 contract；先按 docs/toolchain 未收口管理 |
| `micro-op/vec-scalar/vshls` | vec-scalar | `pto.vshls` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | docs 要求 `input + scalar + mask`，但 `VPTOOps.td` 当前只有 `input + scalar` surface；docs/ODS 未收口前不继续猜 emitter 语义 |
| `micro-op/vec-scalar/vshrs` | vec-scalar | `pto.vshrs` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | docs 要求 `input + scalar + mask`，但 `VPTOOps.td` 当前只有 `input + scalar` surface；docs/ODS 未收口前不继续猜 emitter 语义 |
| `micro-op/vec-scalar/vaddcs` | vec-scalar | `pto.vaddcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` + carry mask 覆盖 carry-chain 路径 |
| `micro-op/vec-scalar/vsubcs` | vec-scalar | `pto.vsubcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` + carry mask 覆盖 carry-chain 路径 |
| `micro-op/unary-vector/vabs` | unary-vector | `pto.vabs` | `core-f32, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-tail` | unary-vector | `pto.vabs` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-f16` | unary-vector | `pto.vabs` | `core-f16, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-bf16` | unary-vector | `pto.vabs` | `core-bf16, full-mask` | blocked | `docs/isa/06-unary-vector-ops.md` 当前未将 `bf16` 列入 `pto.vabs` 的 A5 types |
| `micro-op/unary-vector/vabs-i16-signed` | unary-vector | `pto.vabs` | `core-i16-signed, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-i16-unsigned` | unary-vector | `pto.vabs` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-f32-exceptional` | unary-vector | `pto.vabs` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vabs-i16-signed-overflow-edge` | unary-vector | `pto.vabs` | `core-i16-signed, full-mask, integer-overflow` | blocked | `i16` edge-overflow case 仍缺少 dtype-specific 测例骨架与稳定 overflow oracle |
| `micro-op/unary-vector/vexp` | unary-vector | `pto.vexp` | `core-f32, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vexp-tail` | unary-vector | `pto.vexp` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vexp-f16` | unary-vector | `pto.vexp` | `core-f16, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/unary-vector/vexp-bf16` | unary-vector | `pto.vexp` | `core-bf16, full-mask` | blocked | `docs/isa/06-unary-vector-ops.md` 当前未将 `bf16` 列入 `pto.vexp` 的 A5 types |
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
| `micro-op/unary-vector/vmov` | unary-vector | `pto.vmov` | `core-f32, full-mask` | blocked | 当前 docs/ODS 将 `pto.vmov` 定义为 `1 vreg + 1 mask`，但 LLVM compile-only 走通形式观察为 `llvm.hivm.vmov.*.m(2 vreg + 1 mask)`；PTO surface 到 LLVM 形态的正式语义未收口，先按 `blocked` 管理 |
| `micro-op/compare-select/vcmp-eq` | compare-select | `pto.vcmp` | `core-f32, full-mask, relation-eq` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmp-lt` | compare-select | `pto.vcmp` | `core-f32, full-mask, relation-lt` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmp-tail` | compare-select | `pto.vcmp` | `core-f32, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmp-i16-signed` | compare-select | `pto.vcmp` | `core-i16-signed, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmp-i16-unsigned` | compare-select | `pto.vcmp` | `core-i16-unsigned, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmp-f32-exceptional` | compare-select | `pto.vcmp` | `core-f32, full-mask, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vsel` | compare-select | `pto.vsel` | `core-f32, full-mask` | implemented | `2026-04-02` installed wrapper/`strings bisheng` 已确认 `vsel(dst, src0, src1, mask)` 与 `llvm.hivm.vsel.v64f32` family；对 `f32/v64f32` 与 `.x/.z` 的最小 `.ll` probe 全部在 step 2 instruction selection 同一路径崩溃 |
| `micro-op/compare-select/vsel-tail` | compare-select | `pto.vsel` | `core-f32, tail-mask` | implemented | `2026-04-02` installed wrapper/`strings bisheng` 已确认 `vsel` family 存在；repo `.ll` 越过 step 1 后仍在 step 2 instruction selection 对 `@vsel_tail_kernel_2d.vector.thread` 崩溃 |
| `micro-op/compare-select/vsel-i16` | compare-select | `pto.vsel` | `core-i16-signed, full-mask` | implemented | `2026-04-02` installed wrapper 已确认 `vsel(vector_s16&, vector_s16, vector_s16, vector_bool)`；repo 生成 `llvm.hivm.vsel.v128s16.z` 后在 step 2 type legalization 阶段报 `Do not know how to expand the result of this operator!` |
| `micro-op/compare-select/vselr` | compare-select | `pto.vselr` | `core-f32, full-mask, reversed-select` | blocked | 当前无法根据 `docs/isa/11-compare-select.md`、`docs/isa/12-data-rearrangement.md` 与 `VPTOOps.td` 唯一确定 `vselr` 语义；现有 case 仍是旧的 `mask + cmp_mode` 伪接口，暂停继续收敛 |
| `micro-op/compare-select/vcmps-f32` | compare-select | `pto.vcmps` | `core-f32, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmps-tail` | compare-select | `pto.vcmps` | `core-f32, tail-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmps-i16-signed` | compare-select | `pto.vcmps` | `core-i16-signed, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/compare-select/vcmps-i16-unsigned` | compare-select | `pto.vcmps` | `core-i16-unsigned, full-mask, scalar-operand` | compiled | `2026-04-02` case 仍保持 `ui16` scalar-operand 目标，但改为 `i16 constant + unrealized_conversion_cast -> ui16` 避开与目标无关的 scalar-load lowering 噪音；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/compare-select/vcmps-f32-exceptional` | compare-select | `pto.vcmps` | `core-f32, full-mask, scalar-operand, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/conversion/vcvt-f32-to-f16` | conversion | `pto.vcvt` | `f32-to-f16, full-mask` | compiled | `2026-04-01` 已按 `f32 -> f16` 宽度变化模式重写为 `even/odd + vor`；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/conversion/vcvt-f16-to-f32` | conversion | `pto.vcvt` | `f16-to-f32, full-mask` | compiled | `2026-04-01` 已按 `UNPK_B16 + PART_EVEN/PART_ODD` widening 模式重写；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/conversion/vcvt-tail` | conversion | `pto.vcvt` | `f32-to-f16, tail-mask` | compiled | `2026-04-01` 已按 `LOGICAL_ELEMS=1000` 前缀场景重写为真实 `f32 -> f16` tail case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/conversion/vcvt-f32-special` | conversion | `pto.vcvt` | `f32-to-f16, exceptional-values` | compiled | `2026-04-01` 已按异常值输入重写为真实 `f32 -> f16` narrowing case；本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` |
| `micro-op/conversion/vcvt-i32-to-i16-overflow` | conversion | `pto.vcvt` | `i32-to-i16, integer-overflow` | blocked | `docs/isa/09-conversion-ops.md` 当前未明确列出 `i32 -> i16` 这一 A5 conversion pair |
| `micro-op/conversion/vtrc-f32-rounding` | conversion | `pto.vtrc` | `core-f32, round-r, round-z, round-f` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/conversion/vtrc-f32-special` | conversion | `pto.vtrc` | `core-f32, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/vbr-f32` | materialization-predicate | `pto.vbr` | `core-f32, scalar-broadcast` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/vbr-i32` | materialization-predicate | `pto.vbr` | `core-i32-signed, scalar-broadcast` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/vdup-scalar` | materialization-predicate | `pto.vdup` | `core-f32, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/vdup-lane` | materialization-predicate | `pto.vdup` | `core-f32, lane-select` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/pset-pattern` | materialization-predicate | `pto.pset_b16`, `pto.pset_b32`, `pto.pset_b8` | `pattern-mask, pat-all, pat-vl` | blocked | `2026-04-01` installed wrappers/`strings bisheng` 已确认 `pset_b*` family 存在，但 repo 当前生成的 `llvm.hivm.pset.b{8,16,32}(<256 x i1> <- i64)` 在 bisheng verifier 阶段报 `Intrinsic has incorrect argument type`；当前是 LLVM ABI 未收口，不继续猜参数表 |
| `micro-op/materialization-predicate/pge-tail-mask` | materialization-predicate | `pto.pge_b16`, `pto.pge_b32`, `pto.pge_b8` | `tail-mask` | blocked | `2026-04-01` installed wrappers/`strings bisheng` 已确认 `pge_b*` family 存在，但 repo 当前生成的 `llvm.hivm.pge.b{8,16,32}(<256 x i1> <- i64, i64)` 在 bisheng verifier 阶段报 `Intrinsic has incorrect argument type`；当前是 LLVM ABI 未收口，不继续猜参数表 |
| `micro-op/materialization-predicate/plt-tail-mask` | materialization-predicate | `pto.plt_b16`, `pto.plt_b32`, `pto.plt_b8` | `tail-mask, scalar-carry-out` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；`plt_b8` emitter 缺口已收敛 |
| `micro-op/materialization-predicate/ppack-punpack` | materialization-predicate | `pto.ppack`, `pto.punpack` | `pack-unpack-roundtrip` | blocked | `2026-04-01` installed wrappers/`strings bisheng` 已确认 `ppack/punpack` family 存在，但 repo 当前生成的 `llvm.hivm.ppack.z(<256 x i1>, i64)` / `llvm.hivm.punpack(<256 x i1>, i64)` 在 bisheng verifier 阶段报 `Intrinsic has incorrect argument type`；当前是 LLVM ABI 未收口，不继续猜参数表 |
| `micro-op/materialization-predicate/pdintlv_b8` | materialization-predicate | `pto.pdintlv_b8` | `predicate-transform, lane-order` | blocked | `2026-04-01` 当前 case 的输入构造仍依赖 `pset_b8`；repo 生成的 `llvm.hivm.pset.b8` 在 bisheng verifier 阶段报 `Intrinsic has incorrect argument type`，先按上游 `pset_b8` LLVM ABI 未收口管理 |
| `micro-op/materialization-predicate/pintlv_b16` | materialization-predicate | `pto.pintlv_b16` | `predicate-transform, lane-order` | blocked | `2026-04-01` `pintlv_b16` emitter 已接线，但当前 case 的输入构造依赖 `pset_b16`；repo 生成的 `llvm.hivm.pset.b16` 在 bisheng verifier 阶段报 `Intrinsic has incorrect argument type`，先按上游 `pset_b16` LLVM ABI 未收口管理 |
| `micro-op/materialization-predicate/pand` | materialization-predicate | `pto.pand` | `predicate-transform` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；先前 `scf.for` dialect 注册缺口已收敛 |
| `micro-op/materialization-predicate/por` | materialization-predicate | `pto.por` | `predicate-transform` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；先前 `scf.for` dialect 注册缺口已收敛 |
| `micro-op/materialization-predicate/pxor` | materialization-predicate | `pto.pxor` | `predicate-transform` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；先前 `scf.for` dialect 注册缺口已收敛 |
| `micro-op/materialization-predicate/pnot` | materialization-predicate | `pto.pnot` | `predicate-transform` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/psel` | materialization-predicate | `pto.psel` | `predicate-transform, predicate-select` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/predicate-load-store/psts-plds` | predicate-load-store | `pto.plds`, `pto.psts` | `packed-predicate-roundtrip, scalar-offset, load-store-pair-preservation, representative-logical-elements` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；`plds/psts` emitter 已打通 |
| `micro-op/predicate-load-store/pst-pld` | predicate-load-store | `pto.pld`, `pto.pst` | `packed-predicate-roundtrip, areg-offset, load-store-pair-preservation, representative-logical-elements` | blocked | `2026-04-01` installed wrappers/`strings bisheng` 已确认 `pst/pld` family 存在，但 repo 当前生成的 `llvm.hivm.pst.b8(<256 x i1>, ptr, i32, i32, i32)` / `llvm.hivm.pld.b8(ptr, i32, i32, i32)` 在 bisheng verifier 阶段报 `Intrinsic has incorrect argument type`；当前是 LLVM ABI 未收口 |
| `micro-op/predicate-load-store/psti-pldi` | predicate-load-store | `pto.pldi`, `pto.psti` | `packed-predicate-roundtrip, immediate-offset, load-store-pair-preservation, representative-logical-elements` | blocked | `2026-04-01` installed wrappers/`strings bisheng` 已确认 `psti/pldi` family 存在；repo 当前生成的 `llvm.hivm.pldi.b8/psti.b8` 已越过 `unsupported op`，但 bisheng 在 instruction selection 阶段仍无法选择，当前缺真实 frontend LLVM contract，先按 LLVM ABI 未收口管理 |
| `micro-op/predicate-load-store/pstu` | predicate-load-store | `pto.pstu` | `unaligned-packed-store, state-update, representative-logical-elements` | blocked | `2026-04-01` installed wrapper 只明确暴露 `__builtin_cce_pstu_b16/b32`，当前 testcase 仍以 `!pto.ptr<ui8, ub>` 书写；docs surface、case 与 installed type contract 未收口前不继续猜 emitter |
| `micro-op/reduction/vcadd` | reduction | `pto.vcadd` | `core-f32, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcadd-tail` | reduction | `pto.vcadd` | `core-f32, tail-mask, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcmax` | reduction | `pto.vcmax` | `core-f32, result-placement` | blocked | 文档说明结果包含 value/index，但当前 `docs/isa/10-reduction-ops.md` 未固定低位 packing 细节，oracle 暂未固化 |
| `micro-op/reduction/vcmin` | reduction | `pto.vcmin` | `core-f32, result-placement` | blocked | 文档说明结果包含 value/index，但当前 `docs/isa/10-reduction-ops.md` 未固定低位 packing 细节，oracle 暂未固化 |
| `micro-op/reduction/vcgadd` | reduction | `pto.vcgadd` | `group-reduction, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcgmax` | reduction | `pto.vcgmax` | `group-reduction, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcgmin` | reduction | `pto.vcgmin` | `group-reduction, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcpadd` | reduction | `pto.vcpadd` | `prefix-op, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vlds` | vector-load-store | `pto.vlds` | `core-f32, contiguous, full-mask, aligned, dist-norm` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vlds-tail` | vector-load-store | `pto.vlds` | `core-f32, contiguous, tail-mask, aligned, dist-norm` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vlds-brc-b32` | vector-load-store | `pto.vlds` | `core-f32, full-mask, aligned, dist-brc-b32` | blocked | `docs/isa` 与 verifier 对 `BRC_B32` 是否为正式合法 `dist` 尚未收口；在不改目标的前提下无法写出稳定可接受的 case |
| `micro-op/vector-load-store/vsts` | vector-load-store | `pto.vsts` | `core-f32, contiguous, full-mask, aligned, dist-norm` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vldas-vldus` | vector-load-store | `pto.vldas`, `pto.vldus` | `core-f32, full-mask, unaligned, stream-state` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vldx2-vstx2` | vector-load-store | `pto.vldx2`, `pto.vstx2` | `core-f32, full-mask, paired-roundtrip, dintlv` | blocked | `2026-04-01` repo 生成的 `llvm.hivm.vldx2/vstx2` 已不再停在 `unsupported op`，但 bisheng 仍以 `Intrinsic has incorrect argument type` 拒绝完整 module；当前是 LLVM ABI 未收口，不继续猜 emitter 参数 |
| `micro-op/vector-load-store/vsld` | vector-load-store | `pto.vsld` | `core-f32, full-mask, strided-load` | blocked | installed wrapper 与 `strings bisheng` 已确认 `vsld` surface / intrinsic 名称存在，但 repo 当前生成的 `llvm.hivm.vsld(ptr addrspace(6), i32, i32, i32)` 会被 bisheng 以 `Intrinsic has incorrect argument type` 拒绝；LLVM ABI 尚未收口，先按 `blocked` 管理 |
| `micro-op/vector-load-store/vsldb` | vector-load-store | `pto.vsldb` | `core-f32, full-mask, block-strided-load, block-mask` | blocked | `docs/isa` 只说明 `%offset` 是 packed control word，未给字段编码规则；在不改目标的前提下无法忠实写出 testcase |
| `micro-op/gather-scatter/vscatter` | gather-scatter | `pto.vscatter` | `core-f32, full-mask, non-contiguous, explicit-index-pattern, scatter-store, store-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vsst` | vector-load-store | `pto.vsst` | `core-f32, full-mask, strided-store` | blocked | installed A5 wrapper 当前只直接暴露 `S8_B16`；现有 case 使用 `STRIDE_S2_B64`，docs / case / installed toolchain stride contract 未收口，先按 `blocked` 管理 |
| `micro-op/vector-load-store/vsstb` | vector-load-store | `pto.vsstb` | `core-f32, full-mask, block-strided-store, block-mask` | blocked | `docs/isa` 只说明 `%offset` 是 packed control word，未给字段编码规则；在不改目标的前提下无法忠实写出 testcase |
| `micro-op/vector-load-store/vsta` | vector-load-store | `pto.vsta` | `core-f32, full-mask, aligned, state-update` | blocked | `vsta` flush 语义依赖 unaligned-store 上游 state，但 `vstu/vstus/vstur` 的 docs/ODS surface 仍未收口；当前无法在不改目标的前提下忠实构造 producer/consumer 链 |
| `micro-op/vector-load-store/vstas` | vector-load-store | `pto.vstas` | `core-f32, full-mask, aligned, immediate-offset, state-update` | blocked | `vstas` flush 语义同样依赖 unaligned-store 上游 state，但相关 family 的 docs/ODS surface 仍未收口；当前无法在不改目标的前提下忠实写出 |
| `micro-op/vector-load-store/vstar` | vector-load-store | `pto.vstar` | `core-f32, full-mask, aligned, state-update` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vstu` | vector-load-store | `pto.vstu` | `core-f32, full-mask, unaligned, state-update` | blocked | `docs/isa` 与 `VPTOOps.td` 对参数表和 `mode` 仍有漂移；在 surface 未收口前无法忠实写出 testcase |
| `micro-op/vector-load-store/vstus` | vector-load-store | `pto.vstus` | `core-f32, full-mask, unaligned, immediate-offset, state-update` | blocked | `docs/isa` 与 `VPTOOps.td` 对参数表和 `mode` 仍有漂移；在 surface 未收口前无法忠实写出 testcase |
| `micro-op/vector-load-store/vstur` | vector-load-store | `pto.vstur` | `core-f32, full-mask, unaligned, state-update` | blocked | `docs/isa` 与 `VPTOOps.td` 对参数表和 `mode` 仍有漂移；在 surface 未收口前无法忠实写出 testcase |
| `micro-op/gather-scatter/vgather2` | gather-scatter | `pto.vgather2` | `core-f32, full-mask, non-contiguous, explicit-index-pattern, load-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/gather-scatter/vgatherb` | gather-scatter | `pto.vgatherb` | `core-f32, full-mask, block-gather, aligned-base, load-effect-validation, no-alias` | blocked | installed A5 v300 wrapper 只暴露 `base + vector_u32 indexOffset` 形式，未观察到 docs/ODS 中的 `active_lanes` surface；contract 未收口前不继续猜 LLVM 形态 |
| `micro-op/gather-scatter/vgather2_bc` | gather-scatter | `pto.vgather2_bc` | `core-f32, full-mask, non-contiguous, masked-gather, load-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/rearrangement/vintlv-vdintlv` | rearrangement | `pto.vdintlv`, `pto.vintlv` | `paired-roundtrip, lane-order` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；`vintlv/vdintlv` 双结果 LLVM emission 已接通 |
| `micro-op/rearrangement/vslide` | rearrangement | `pto.vslide` | `lane-order, slide-window` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/rearrangement/vshift` | rearrangement | `pto.vshift` | `lane-order, zero-fill` | blocked | docs 把它定义成 single-source slide，但 installed A5 仅明确暴露 memory `vsld` 与 in-register `vslide`；在确认 `pto.vshift` 是否等价于某个真实 contract 前不继续猜 emitter |
| `micro-op/rearrangement/vsqz` | rearrangement | `pto.vsqz` | `predicate-driven-rearrangement, stable-order` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；`store` 参数收紧为 installed stub 明确的 `i32` 后通过 |
| `micro-op/rearrangement/vusqz` | rearrangement | `pto.vusqz` | `predicate-driven-rearrangement, placement` | blocked | 当前 docs 与 surface 不能为 placement 目标提供足够输入模型；在不改目标的前提下无法写出稳定 testcase |
| `micro-op/rearrangement/vperm` | rearrangement | `pto.vperm` | `lane-order, explicit-index-pattern` | blocked | docs 把它定义为 in-register permute，但 installed trace 仅观察到 memory-based `vgatherb/vgather2` family，未看到可直接对应 `%src + %index` 的 `llvm.hivm.vperm.*` contract |
| `micro-op/rearrangement/vpack` | rearrangement | `pto.vpack` | `pack-unpack, narrowing` | blocked | 当前 PTO/docs surface 是双输入 `%src0, %src1, %part`，但 installed wrapper 只明确暴露单输入 `vpack(dst, src, part, mode)`；surface 未收口前不继续猜 LLVM lowering |
| `micro-op/rearrangement/vsunpack` | rearrangement | `pto.vsunpack` | `pack-unpack, sign-extend` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；`part` 参数收紧为 installed stub 明确的 `i32` 后通过 |
| `micro-op/rearrangement/vzunpack` | rearrangement | `pto.vzunpack` | `pack-unpack, zero-extend` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；case 类型收紧为 `ui16 -> ui32` 且 `part` 收紧为 `i32` 后通过 |
| `micro-op/rearrangement/vintlv-vdintlv-lane-boundary` | rearrangement | `pto.vdintlv`, `pto.vintlv` | `paired-roundtrip, lane-order` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；与主 case 共享同一双结果 emission 路径 |
| `micro-op/rearrangement/vslide-tail-window` | rearrangement | `pto.vslide` | `lane-order, slide-window, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；与主 case 共享同一 emission 路径 |
| `micro-op/rearrangement/vshift-tail-zero-fill` | rearrangement | `pto.vshift` | `lane-order, zero-fill, tail-mask` | blocked | 与主 case 相同；single-source zero-fill slide 的 installed contract 仍未收口，不继续猜 emitter |
| `micro-op/rearrangement/vsqz-nontrivial-mask` | rearrangement | `pto.vsqz` | `predicate-driven-rearrangement, stable-order` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；与主 case 共享同一 `vsqz` emission 路径 |
| `micro-op/rearrangement/vusqz-nontrivial-mask` | rearrangement | `pto.vusqz` | `predicate-driven-rearrangement, placement` | blocked | 当前 docs 与 surface 不能为 nontrivial placement 目标提供足够输入模型；在不改目标的前提下无法写出稳定 testcase |
| `micro-op/dsa-sfu/vlrelu-f32` | dsa-sfu | `pto.vlrelu` | `core-f32, scalar-operand, full-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vlrelu-tail` | dsa-sfu | `pto.vlrelu` | `core-f32, tail-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vlrelu-f16` | dsa-sfu | `pto.vlrelu` | `core-f16, full-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vprelu-f32` | dsa-sfu | `pto.vprelu` | `core-f32, vector-alpha` | blocked | installed wrapper 已确认 `vprelu(dst, src0, src1, mask, mode)` 形式，merge 路径会引入额外 `dst`；当前 PTO surface 与 installed/LLVM 侧的 `merge-dst + 2 src + mask` 关系未收口 |
| `micro-op/dsa-sfu/vexpdiff-f32` | dsa-sfu | `pto.vexpdiff` | `core-f32, fused-expdiff` | blocked | `docs/isa` 与 PTO surface 有该 op，但 installed A5 headers、Clang wrappers、`strings bisheng` 均未观察到同名 surface / intrinsic；正式 toolchain contract 未建立 |
| `micro-op/dsa-sfu/vaddrelu-f32` | dsa-sfu | `pto.vaddrelu` | `core-f32, fused-op` | blocked | `docs/isa` 与 PTO surface 有该 op，但 installed A5 headers、Clang wrappers、`strings bisheng` 均未观察到同名 surface / intrinsic；PTO surface 到 installed contract 的关系未建立 |
| `micro-op/dsa-sfu/vsubrelu-f32` | dsa-sfu | `pto.vsubrelu` | `core-f32, fused-op` | blocked | `docs/isa` 与 PTO surface 有该 op，但 installed A5 headers、Clang wrappers、`strings bisheng` 均未观察到同名 surface / intrinsic；PTO surface 到 installed contract 的关系未建立 |
| `micro-op/dsa-sfu/vaxpy-f32` | dsa-sfu | `pto.vaxpy` | `core-f32, scalar-operand, fused-op` | blocked | `2026-04-01` emitter 已接到 `llvm.hivm.vaxpy.v64f32.x`，但 bisheng 在 instruction selection 阶段仍报 `Cannot select`; installed wrapper 只确认了 builtin family，尚未拿到真实 frontend LLVM 形状，先按 LLVM contract 未收口管理 |
| `micro-op/dsa-sfu/vaddreluconv` | dsa-sfu | `pto.vaddreluconv` | `fused-op, conversion-result` | blocked | `docs`、`ODS` 与 verifier 对输入列表和 `conversion-result` 的 result 约束未收口；在不改目标的前提下无法写出稳定 testcase |
| `micro-op/dsa-sfu/vmulconv` | dsa-sfu | `pto.vmulconv` | `fused-op, conversion-result` | blocked | `docs`、`ODS` 与 verifier 对输入列表和 `conversion-result` 的 result 约束未收口；在不改目标的前提下无法写出稳定 testcase |
| `micro-op/dsa-sfu/vmull` | dsa-sfu | `pto.vmull` | `widening-op, hi-lo-split` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6`；`vmull` emitter 缺口已收敛 |
| `micro-op/dsa-sfu/vmula` | dsa-sfu | `pto.vmula` | `core-f32, fused-op, accumulator` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vci` | dsa-sfu / conversion | `pto.vci` | `index-generation` | blocked | `2026-04-01` emitter 已接到 `llvm.hivm.vci.v64s32`，repo 当前生成 `declare <64 x i32> @llvm.hivm.vci.v64s32(i32, i64)`；bisheng verifier 仍报 `Intrinsic has incorrect argument type`，当前是 LLVM ABI 未收口 |
| `micro-op/dsa-sfu/vbitsort` | dsa-sfu | `pto.vbitsort` | `index-generation, layout-transform` | blocked | `docs/vpto-spec.md` 与 `docs/isa/13-dsa-sfu-ops.md` 目前只给出 surface/接口层信息，尚未形成可稳定闭环的 oracle 语义 |
| `micro-op/dsa-sfu/vtranspose` | dsa-sfu | `pto.vtranspose` | `ub-to-ub, layout-transform, representative-config` | blocked | `2026-04-01` installed A5 `TTrans.hpp` 未暴露同名 HIVM intrinsic，而是用 `vci/vmuls/vadds/vgather2/vsts` helper 序列实现；在 `config` 到 helper 语义未收口前，不直接猜单条 LLVM lowering |
| `micro-op/vec-scalar/vsubs-tail` | vec-scalar | `pto.vsubs` | `core-f32, tail-mask, scalar-operand` | blocked | 当前 `docs/isa/08-vec-scalar-ops.md` 定义了 `pto.vsubs` surface，但 installed Clang headers 与 `strings bisheng` 未观察到对应 `vsubs` wrapper / `llvm.hivm.vsubs.*` intrinsic；docs surface 与 installed toolchain 支持面未收口 |
| `micro-op/vec-scalar/vmuls-tail` | vec-scalar | `pto.vmuls` | `core-f32, tail-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vmaxs-tail` | vec-scalar | `pto.vmaxs` | `core-f32, tail-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vmins-tail` | vec-scalar | `pto.vmins` | `core-f32, tail-mask, scalar-operand` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vec-scalar/vands-mask-edge` | vec-scalar | `pto.vands` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | `docs/isa/08-vec-scalar-ops.md` 有 surface，但 installed headers / `strings bisheng` 未观察到 `vands` 对应 contract；先按 docs/toolchain 未收口管理 |
| `micro-op/vec-scalar/vors-mask-edge` | vec-scalar | `pto.vors` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | `docs/isa/08-vec-scalar-ops.md` 有 surface，但 installed headers / `strings bisheng` 未观察到 `vors` 对应 contract；先按 docs/toolchain 未收口管理 |
| `micro-op/vec-scalar/vxors-mask-edge` | vec-scalar | `pto.vxors` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | `docs/isa/08-vec-scalar-ops.md` 有 surface，但 installed headers / `strings bisheng` 未观察到 `vxors` 对应 contract；先按 docs/toolchain 未收口管理 |
| `micro-op/vec-scalar/vshls-shift-boundary` | vec-scalar | `pto.vshls` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | docs 要求 `input + scalar + mask`，但 `VPTOOps.td` 当前只有 `input + scalar` surface；docs/ODS 未收口前不继续猜 emitter 语义 |
| `micro-op/vec-scalar/vshrs-shift-boundary` | vec-scalar | `pto.vshrs` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | docs 要求 `input + scalar + mask`，但 `VPTOOps.td` 当前只有 `input + scalar` surface；docs/ODS 未收口前不继续猜 emitter 语义 |
| `micro-op/vec-scalar/vaddcs-carry-boundary` | vec-scalar | `pto.vaddcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` + carry mask 覆盖 boundary 场景 |
| `micro-op/vec-scalar/vsubcs-borrow-boundary` | vec-scalar | `pto.vsubcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；repo case 当前实际以 `!pto.vreg<64xi32>` + carry mask 覆盖 boundary 场景 |
| `micro-op/unary-vector/vln-domain-boundary` | unary-vector | `pto.vln` | `core-f32, domain-positive, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前 compile-only 可过 |
| `micro-op/unary-vector/vsqrt-domain-boundary` | unary-vector | `pto.vsqrt` | `core-f32, domain-nonnegative, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前 compile-only 可过 |
| `micro-op/unary-vector/vrsqrt-zero-inf` | unary-vector | `pto.vrsqrt` | `core-f32, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前 compile-only 可过 |
| `micro-op/unary-vector/vrec-zero-inf` | unary-vector | `pto.vrec` | `core-f32, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前 compile-only 可过 |
| `micro-op/unary-vector/vneg-f32-exceptional` | unary-vector | `pto.vneg` | `core-f32, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library；当前 compile-only 可过 |
| `micro-op/unary-vector/vmov-tail` | unary-vector | `pto.vmov` | `core-f32, tail-mask` | blocked | 当前 docs/ODS 将 `pto.vmov` 定义为 `1 vreg + 1 mask`，但 LLVM compile-only 走通形式观察为 `llvm.hivm.vmov.*.m(2 vreg + 1 mask)`；PTO surface 到 LLVM 形态的正式语义未收口，先按 `blocked` 管理 |
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
| `micro-op/compare-select/vsel-predicate-edge` | compare-select | `pto.vsel` | `core-f32, full-mask` | implemented | `2026-04-02` repo 已生成 `llvm.hivm.vsel.v64f32`，installed headers 与 `strings bisheng` 也确认 `vsel` family 存在；最小 `.ll` probe 的多种 spelling 复核同样全部在 step 2 instruction selection 阶段崩溃 |
| `micro-op/materialization-predicate/pset-pattern-fragment` | materialization-predicate | `pto.pset_b16`, `pto.pset_b32`, `pto.pset_b8` | `pattern-mask, pat-vl, representative-logical-elements` | blocked | `2026-04-01` 与 `micro-op/materialization-predicate/pset-pattern` 相同，repo 当前生成的 `llvm.hivm.pset.b{8,16,32}` 签名会被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝；当前是 LLVM ABI 未收口 |
| `micro-op/materialization-predicate/pge-tail-mask-boundary` | materialization-predicate | `pto.pge_b16`, `pto.pge_b32`, `pto.pge_b8` | `tail-mask, representative-logical-elements` | blocked | `2026-04-01` 与 `micro-op/materialization-predicate/pge-tail-mask` 相同，repo 当前生成的 `llvm.hivm.pge.b{8,16,32}` 签名会被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝；当前是 LLVM ABI 未收口 |
| `micro-op/materialization-predicate/plt-tail-mask-boundary` | materialization-predicate | `pto.plt_b16`, `pto.plt_b32`, `pto.plt_b8` | `tail-mask, scalar-carry-out, representative-logical-elements` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/materialization-predicate/ppack-punpack-nontrivial` | materialization-predicate | `pto.ppack`, `pto.punpack` | `pack-unpack-roundtrip, representative-logical-elements` | blocked | `2026-04-01` 与 `micro-op/materialization-predicate/ppack-punpack` 相同，repo 当前生成的 `llvm.hivm.ppack.z` / `llvm.hivm.punpack` 签名会被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝；当前是 LLVM ABI 未收口 |
| `micro-op/materialization-predicate/pdintlv_b8-nontrivial` | materialization-predicate | `pto.pdintlv_b8` | `predicate-transform, lane-order` | implemented | `2026-04-02` installed wrapper 已确认 `pdintlv_b8(dst0, dst1, src0, src1)` 采用 `__builtin_cce_pdintlv_b8(&ret, src0, src1)` 双结果 contract；repo 当前 `pdintlv` 本身能落成 `{<256xi1>, <256xi1>}`，但前驱 `llvm.hivm.pset.b8(i64) -> <256xi1>` 仍被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝 |
| `micro-op/materialization-predicate/pintlv_b16-nontrivial` | materialization-predicate | `pto.pintlv_b16` | `predicate-transform, lane-order` | implemented | `2026-04-02` installed wrapper 已确认 `pintlv_b16(dst0, dst1, src0, src1)` 采用 `__builtin_cce_pintlv_b16(&ret, src0, src1)` 双结果 contract；repo 当前 `pintlv` 本身能落成 `{<256xi1>, <256xi1>}`，但前驱 `llvm.hivm.pset.b16(i64) -> <256xi1>` 仍被 bisheng verifier 以 `Intrinsic has incorrect argument type` 拒绝 |
| `micro-op/materialization-predicate/psel-tail-predicate` | materialization-predicate | `pto.psel` | `predicate-transform, predicate-select, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/predicate-load-store/psts-plds-packed-prefix-boundary` | predicate-load-store | `pto.plds`, `pto.psts` | `packed-predicate-roundtrip, scalar-offset, load-store-pair-preservation, representative-logical-elements` | blocked | 当前文档只定义普通 scalar-offset predicate load/store，未给出与目标匹配的 packed roundtrip surface；在不改目标的前提下无法忠实写出 |
| `micro-op/predicate-load-store/pstu-state-advance-boundary` | predicate-load-store | `pto.pstu` | `unaligned-packed-store, state-update, representative-logical-elements` | blocked | `2026-04-01` 同 `micro-op/predicate-load-store/pstu`；installed 仅确认 `b16/b32` contract，当前 `ui8` case 不能视为已与 toolchain 语义对齐 |
| `micro-op/reduction/vcgadd-tail` | reduction | `pto.vcgadd` | `group-reduction, tail-mask, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcgmax-tie` | reduction | `pto.vcgmax` | `group-reduction, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcgmin-tie` | reduction | `pto.vcgmin` | `group-reduction, result-placement` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/reduction/vcpadd-tail` | reduction | `pto.vcpadd` | `prefix-op, tail-mask` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vsts-tail` | vector-load-store | `pto.vsts` | `core-f32, contiguous, tail-mask, aligned, dist-norm` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vldas-vldus-state-chain` | vector-load-store | `pto.vldas`, `pto.vldus` | `core-f32, full-mask, unaligned, stream-state, state-update` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/vector-load-store/vldx2-layout-check` | vector-load-store | `pto.vldx2` | `core-f32, full-mask, paired-roundtrip, dintlv, lane-order` | blocked | `2026-04-01` repo 生成的 `llvm.hivm.vldx2/vstx2` 已进入 bisheng verifier，但完整 module 仍报 `Intrinsic has incorrect argument type`；当前按 LLVM ABI 未收口管理 |
| `micro-op/vector-load-store/vstx2-layout-check` | vector-load-store | `pto.vstx2` | `core-f32, full-mask, paired-roundtrip, dintlv, lane-order` | blocked | `2026-04-01` repo 生成的 `llvm.hivm.vstx2` 已进入 bisheng verifier，但完整 module 仍报 `Intrinsic has incorrect argument type`；当前按 LLVM ABI 未收口管理 |
| `micro-op/vector-load-store/vsta-state-advance` | vector-load-store | `pto.vsta` | `core-f32, full-mask, aligned, state-update` | blocked | 目标依赖真实 unaligned-store state producer，但 `vstu/vstus/vstur` surface 未稳定；当前无法在不改目标的前提下忠实写出 state-advance 路径 |
| `micro-op/vector-load-store/vstu-state-advance` | vector-load-store | `pto.vstu` | `core-f32, full-mask, unaligned, state-update` | blocked | 当前 docs/isa 与 `VPTOOps.td` 对参数表和 `mode` 仍有漂移；在 surface 未收口前无法忠实写出 state-advance case |
| `micro-op/vector-load-store/vstas-vstus-offset-update` | vector-load-store | `pto.vstas`, `pto.vstus` | `core-f32, full-mask, immediate-offset, state-update` | blocked | 组合目标依赖未稳定的 unaligned-store surface；当前无法在不改目标的前提下补出合法 upstream state 链 |
| `micro-op/vector-load-store/vsld-vsst-stride-boundary` | vector-load-store | `pto.vsld`, `pto.vsst` | `core-f32, strided-load, strided-store, block-mask` | blocked | 同时依赖 `vsld` LLVM ABI 与 `vsst` stride contract；这两层 installed contract 当前都未收口，边界 case 先随主 case 按 `blocked` 管理 |
| `micro-op/gather-scatter/vgather2-duplicate-index` | gather-scatter | `pto.vgather2` | `core-f32, non-contiguous, explicit-index-pattern, load-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/gather-scatter/vgather2_bc-sparse-mask` | gather-scatter | `pto.vgather2_bc` | `core-f32, masked-gather, load-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/gather-scatter/vgatherb-block-boundary` | gather-scatter | `pto.vgatherb` | `core-f32, block-gather, aligned-base, load-effect-validation, no-alias` | blocked | installed A5 v300 wrapper 只暴露 `base + vector_u32 indexOffset` 形式，未观察到 docs/ODS 中的 `active_lanes` surface；contract 未收口前不继续猜 LLVM 形态 |
| `micro-op/gather-scatter/vscatter-out-of-order-index` | gather-scatter | `pto.vscatter` | `core-f32, explicit-index-pattern, scatter-store, store-effect-validation, no-alias` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vlrelu-f32-exceptional` | dsa-sfu | `pto.vlrelu` | `core-f32, scalar-operand, exceptional-values` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vprelu-tail` | dsa-sfu | `pto.vprelu` | `core-f32, vector-alpha, tail-mask` | blocked | 当前无法根据 docs/isa + LLVM 已确认定义唯一确定正式参数列表；LLVM 侧观察为 `3 vreg + 1 mask`，与 PTO surface 未收口，先按 `blocked` 管理 |
| `micro-op/dsa-sfu/vexpdiff-boundary` | dsa-sfu | `pto.vexpdiff` | `core-f32, fused-expdiff, exceptional-values, floating-overflow-underflow` | blocked | 当前无法根据 docs/isa + LLVM 已确认定义唯一确定正式参数列表；LLVM 侧观察为 `2 vreg + 1 mask + 1 scalar`，与 PTO surface 未收口，先按 `blocked` 管理 |
| `micro-op/dsa-sfu/vmula-accumulator-boundary` | dsa-sfu | `pto.vmula` | `core-f32, fused-op, accumulator` | compiled | `2026-04-01` 本地 `DEVICE=SIM COMPILE_ONLY=1` 已走到 step `4/6` 并产出 kernel shared library |
| `micro-op/dsa-sfu/vtranspose-multi-config` | dsa-sfu | `pto.vtranspose` | `ub-to-ub, layout-transform, representative-config` | blocked | `2026-04-01` 同 `micro-op/dsa-sfu/vtranspose`；installed A5 只观察到 helper 级实现，未确认 `config` 到 LLVM/指令序列的正式 contract |

## Notes

- `case` 字段记录相对 `test/vpto/cases/` 的真实 case 路径；微指令单-op 例如 `micro-op/binary-vector/vadd`
- `tileop/` 下的 case 表示 tile 级或派生组合验证，不直接计入向量单 op 覆盖完成态
- 历史 `tileop/*` 或其他已有 case 只能作为骨架参考，不能直接填写到微指令单 op 覆盖条目的 `case` 字段里。
- 已转入文档漂移核对单的口径问题，不继续在本 matrix 中单独记账；待结论明确后再回填对应条目。
- 随执行推进，这份 matrix 应同步更新 `case`、`scenarios` 和 `status`，作为唯一静态追踪来源。
