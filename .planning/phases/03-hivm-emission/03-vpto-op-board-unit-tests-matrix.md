# VPTO Op Board Unit Tests Matrix

## Legend

- `infra`: 基础设施 op，只作为其他 case 的准备动作或收尾动作复用
- `planned`: 已纳入本轮范围，待补 case
- `implemented`: case 已落地，待完成板测闭环
- `board-passed`: 已完成上板验证
- `blocked`: 当前存在明确阻塞

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
| `pto.vselr` | compare-select | `docs/isa/11-compare-select.md` | yes | `micro-op/compare-select/vselr` | `core-f32, full-mask, reversed-select` | |
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
| `pto.vprelu` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vprelu-f32` | `core-f32, vector-alpha` | |
| `pto.vexpdiff` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vexpdiff-f32` | `core-f32, fused-expdiff` | |
| `pto.vaddrelu` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vaddrelu-f32` | `core-f32, fused-op` | |
| `pto.vsubrelu` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | `micro-op/dsa-sfu/vsubrelu-f32` | `core-f32, fused-op` | |
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
| `micro-op/binary-vector/vadd-tail` | binary-vector | `pto.vadd` | `core-f32, tail-mask` | planned | |
| `micro-op/binary-vector/vadd-f16` | binary-vector | `pto.vadd` | `core-f16, full-mask` | planned | |
| `micro-op/binary-vector/vadd-bf16` | binary-vector | `pto.vadd` | `core-bf16, full-mask` | planned | |
| `micro-op/binary-vector/vadd-i16-signed` | binary-vector | `pto.vadd` | `core-i16-signed, full-mask` | planned | |
| `micro-op/binary-vector/vadd-i16-unsigned` | binary-vector | `pto.vadd` | `core-i16-unsigned, full-mask` | planned | |
| `micro-op/binary-vector/vadd-i16-signed-overflow` | binary-vector | `pto.vadd` | `core-i16-signed, full-mask, integer-overflow` | planned | 待补充；overflow oracle 需按 `docs/isa/07-binary-vector-ops.md` 与当前实现交集固化 |
| `micro-op/binary-vector/vadd-i16-unsigned-overflow` | binary-vector | `pto.vadd` | `core-i16-unsigned, full-mask, integer-overflow` | planned | 待补充；overflow oracle 需按 `docs/isa/07-binary-vector-ops.md` 与当前实现交集固化 |
| `micro-op/binary-vector/vadd-f32-exceptional` | binary-vector | `pto.vadd` | `core-f32, full-mask, exceptional-values` | planned | |
| `micro-op/binary-vector/vsub` | binary-vector | `pto.vsub` | `core-f32, full-mask` | planned | |
| `micro-op/binary-vector/vmul` | binary-vector | `pto.vmul` | `core-f32, full-mask` | planned | |
| `micro-op/binary-vector/vdiv` | binary-vector | `pto.vdiv` | `core-f32, full-mask` | board-passed | |
| `micro-op/binary-vector/vdiv-tail` | binary-vector | `pto.vdiv` | `core-f32, tail-mask` | planned | |
| `micro-op/binary-vector/vdiv-f16` | binary-vector | `pto.vdiv` | `core-f16, full-mask` | planned | |
| `micro-op/binary-vector/vdiv-bf16` | binary-vector | `pto.vdiv` | `core-bf16, full-mask` | blocked | `docs/isa/07-binary-vector-ops.md` 当前未将 `bf16` 列入 `pto.vdiv` 的 A5 types |
| `micro-op/binary-vector/vdiv-f32-exceptional` | binary-vector | `pto.vdiv` | `core-f32, full-mask, exceptional-values` | planned | |
| `micro-op/binary-vector/vmax` | binary-vector | `pto.vmax` | `core-f32, full-mask` | planned | |
| `micro-op/binary-vector/vmin` | binary-vector | `pto.vmin` | `core-f32, full-mask` | board-passed | |
| `micro-op/binary-vector/vmin-tail` | binary-vector | `pto.vmin` | `core-f32, tail-mask` | planned | |
| `micro-op/binary-vector/vmin-f16` | binary-vector | `pto.vmin` | `core-f16, full-mask` | planned | |
| `micro-op/binary-vector/vmin-bf16` | binary-vector | `pto.vmin` | `core-bf16, full-mask` | planned | |
| `micro-op/binary-vector/vmin-i16-signed` | binary-vector | `pto.vmin` | `core-i16-signed, full-mask` | planned | |
| `micro-op/binary-vector/vmin-i16-unsigned` | binary-vector | `pto.vmin` | `core-i16-unsigned, full-mask` | planned | |
| `micro-op/binary-vector/vmin-f32-exceptional` | binary-vector | `pto.vmin` | `core-f32, full-mask, exceptional-values` | planned | |
| `micro-op/binary-vector/vand` | binary-vector | `pto.vand` | `core-i16-unsigned, full-mask` | planned | |
| `micro-op/binary-vector/vor` | binary-vector | `pto.vor` | `core-i16-unsigned, full-mask` | planned | |
| `micro-op/binary-vector/vxor` | binary-vector | `pto.vxor` | `core-i16-unsigned, full-mask` | planned | |
| `micro-op/binary-vector/vshl` | binary-vector | `pto.vshl` | `core-i16-unsigned, full-mask` | planned | |
| `micro-op/binary-vector/vshr` | binary-vector | `pto.vshr` | `core-i16-unsigned, full-mask` | planned | |
| `micro-op/binary-vector/vaddc` | binary-vector | `pto.vaddc` | `core-i16-unsigned, full-mask, carry-chain` | planned | |
| `micro-op/binary-vector/vsubc` | binary-vector | `pto.vsubc` | `core-i16-unsigned, full-mask, carry-chain` | planned | |
| `micro-op/vec-scalar/vadds` | vec-scalar | `pto.vadds` | `core-f32, full-mask, scalar-operand` | board-passed | |
| `micro-op/vec-scalar/vadds-tail` | vec-scalar | `pto.vadds` | `core-f32, tail-mask, scalar-operand` | planned | |
| `micro-op/vec-scalar/vadds-f16` | vec-scalar | `pto.vadds` | `core-f16, full-mask, scalar-operand` | blocked | `docs/isa/08-vec-scalar-ops.md` 仅给出通用 `T` 语法，尚未明确 `pto.vadds` 的 A5 type 集合 |
| `micro-op/vec-scalar/vadds-bf16` | vec-scalar | `pto.vadds` | `core-bf16, full-mask, scalar-operand` | blocked | `docs/isa/08-vec-scalar-ops.md` 仅给出通用 `T` 语法，尚未明确 `pto.vadds` 的 A5 type 集合 |
| `micro-op/vec-scalar/vadds-i16-signed` | vec-scalar | `pto.vadds` | `core-i16-signed, full-mask, scalar-operand` | blocked | signed integer legality 仍需 `docs/vpto-spec.md` 与 `docs/isa/08-vec-scalar-ops.md` 的交集进一步固化 |
| `micro-op/vec-scalar/vadds-i16-unsigned` | vec-scalar | `pto.vadds` | `core-i16-unsigned, full-mask, scalar-operand` | blocked | unsigned integer legality 仍需 `docs/vpto-spec.md` 与 `docs/isa/08-vec-scalar-ops.md` 的交集进一步固化 |
| `micro-op/vec-scalar/vadds-f32-exceptional` | vec-scalar | `pto.vadds` | `core-f32, full-mask, scalar-operand, exceptional-values` | planned | |
| `micro-op/vec-scalar/vadds-i16-signed-overflow` | vec-scalar | `pto.vadds` | `core-i16-signed, full-mask, scalar-operand, integer-overflow` | blocked | `docs/isa/08-vec-scalar-ops.md` 尚未给出明确 A5 types 与 overflow 规则，暂不固化 oracle |
| `micro-op/vec-scalar/vadds-i16-unsigned-overflow` | vec-scalar | `pto.vadds` | `core-i16-unsigned, full-mask, scalar-operand, integer-overflow` | blocked | `docs/isa/08-vec-scalar-ops.md` 尚未给出明确 A5 types 与 overflow 规则，暂不固化 oracle |
| `micro-op/vec-scalar/vsubs` | vec-scalar | `pto.vsubs` | `core-f32, full-mask, scalar-operand` | planned | |
| `micro-op/vec-scalar/vmuls` | vec-scalar | `pto.vmuls` | `core-f32, full-mask, scalar-operand` | planned | |
| `micro-op/vec-scalar/vmaxs` | vec-scalar | `pto.vmaxs` | `core-f32, full-mask, scalar-operand` | planned | |
| `micro-op/vec-scalar/vmins` | vec-scalar | `pto.vmins` | `core-f32, full-mask, scalar-operand` | planned | |
| `micro-op/vec-scalar/vands` | vec-scalar | `pto.vands` | `core-i16-unsigned, full-mask, scalar-operand` | planned | |
| `micro-op/vec-scalar/vors` | vec-scalar | `pto.vors` | `core-i16-unsigned, full-mask, scalar-operand` | planned | |
| `micro-op/vec-scalar/vxors` | vec-scalar | `pto.vxors` | `core-i16-unsigned, full-mask, scalar-operand` | planned | |
| `micro-op/vec-scalar/vshls` | vec-scalar | `pto.vshls` | `core-i16-unsigned, full-mask, scalar-operand` | planned | |
| `micro-op/vec-scalar/vshrs` | vec-scalar | `pto.vshrs` | `core-i16-unsigned, full-mask, scalar-operand` | planned | |
| `micro-op/vec-scalar/vaddcs` | vec-scalar | `pto.vaddcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | planned | |
| `micro-op/vec-scalar/vsubcs` | vec-scalar | `pto.vsubcs` | `core-i16-unsigned, full-mask, scalar-operand, carry-chain` | planned | |
| `micro-op/unary-vector/vabs` | unary-vector | `pto.vabs` | `core-f32, full-mask` | planned | |
| `micro-op/unary-vector/vabs-tail` | unary-vector | `pto.vabs` | `core-f32, tail-mask` | planned | |
| `micro-op/unary-vector/vabs-f16` | unary-vector | `pto.vabs` | `core-f16, full-mask` | planned | |
| `micro-op/unary-vector/vabs-bf16` | unary-vector | `pto.vabs` | `core-bf16, full-mask` | blocked | `docs/isa/06-unary-vector-ops.md` 当前未将 `bf16` 列入 `pto.vabs` 的 A5 types |
| `micro-op/unary-vector/vabs-i16-signed` | unary-vector | `pto.vabs` | `core-i16-signed, full-mask` | planned | |
| `micro-op/unary-vector/vabs-i16-unsigned` | unary-vector | `pto.vabs` | `core-i16-unsigned, full-mask` | planned | |
| `micro-op/unary-vector/vabs-f32-exceptional` | unary-vector | `pto.vabs` | `core-f32, full-mask, exceptional-values` | planned | |
| `micro-op/unary-vector/vabs-i16-signed-overflow-edge` | unary-vector | `pto.vabs` | `core-i16-signed, full-mask, integer-overflow` | planned | 待补充；重点检查最小负值绝对值等边界 |
| `micro-op/unary-vector/vexp` | unary-vector | `pto.vexp` | `core-f32, full-mask` | planned | |
| `micro-op/unary-vector/vexp-tail` | unary-vector | `pto.vexp` | `core-f32, tail-mask` | planned | |
| `micro-op/unary-vector/vexp-f16` | unary-vector | `pto.vexp` | `core-f16, full-mask` | planned | |
| `micro-op/unary-vector/vexp-bf16` | unary-vector | `pto.vexp` | `core-bf16, full-mask` | blocked | `docs/isa/06-unary-vector-ops.md` 当前未将 `bf16` 列入 `pto.vexp` 的 A5 types |
| `micro-op/unary-vector/vexp-f32-exceptional` | unary-vector | `pto.vexp` | `core-f32, full-mask, exceptional-values` | planned | |
| `micro-op/unary-vector/vexp-f32-over-underflow` | unary-vector | `pto.vexp` | `core-f32, full-mask, floating-overflow-underflow` | planned | |
| `micro-op/unary-vector/vneg` | unary-vector | `pto.vneg` | `core-f32, full-mask` | planned | |
| `micro-op/unary-vector/vln` | unary-vector | `pto.vln` | `core-f32, full-mask, domain-positive` | planned | |
| `micro-op/unary-vector/vsqrt` | unary-vector | `pto.vsqrt` | `core-f32, full-mask, domain-nonnegative` | planned | |
| `micro-op/unary-vector/vrsqrt` | unary-vector | `pto.vrsqrt` | `core-f32, full-mask, exceptional-values` | planned | |
| `micro-op/unary-vector/vrec` | unary-vector | `pto.vrec` | `core-f32, full-mask, exceptional-values` | planned | |
| `micro-op/unary-vector/vrelu` | unary-vector | `pto.vrelu` | `core-f32, full-mask` | planned | |
| `micro-op/unary-vector/vnot` | unary-vector | `pto.vnot` | `core-i16-signed, full-mask` | planned | |
| `micro-op/unary-vector/vbcnt` | unary-vector | `pto.vbcnt` | `core-i16-unsigned, full-mask` | planned | |
| `micro-op/unary-vector/vcls` | unary-vector | `pto.vcls` | `core-i16-signed, full-mask` | planned | |
| `micro-op/unary-vector/vmov` | unary-vector | `pto.vmov` | `core-f32, full-mask` | planned | |
| `micro-op/compare-select/vcmp-eq` | compare-select | `pto.vcmp` | `core-f32, full-mask, relation-eq` | planned | |
| `micro-op/compare-select/vcmp-lt` | compare-select | `pto.vcmp` | `core-f32, full-mask, relation-lt` | planned | |
| `micro-op/compare-select/vcmp-tail` | compare-select | `pto.vcmp` | `core-f32, tail-mask` | planned | |
| `micro-op/compare-select/vcmp-i16-signed` | compare-select | `pto.vcmp` | `core-i16-signed, full-mask` | planned | |
| `micro-op/compare-select/vcmp-i16-unsigned` | compare-select | `pto.vcmp` | `core-i16-unsigned, full-mask` | planned | |
| `micro-op/compare-select/vcmp-f32-exceptional` | compare-select | `pto.vcmp` | `core-f32, full-mask, exceptional-values` | planned | |
| `micro-op/compare-select/vsel` | compare-select | `pto.vsel` | `core-f32, full-mask` | planned | |
| `micro-op/compare-select/vsel-tail` | compare-select | `pto.vsel` | `core-f32, tail-mask` | planned | |
| `micro-op/compare-select/vsel-i16` | compare-select | `pto.vsel` | `core-i16-signed, full-mask` | planned | |
| `micro-op/compare-select/vselr` | compare-select | `pto.vselr` | `core-f32, full-mask, reversed-select` | planned | |
| `micro-op/compare-select/vcmps-f32` | compare-select | `pto.vcmps` | `core-f32, full-mask, scalar-operand` | planned | |
| `micro-op/compare-select/vcmps-tail` | compare-select | `pto.vcmps` | `core-f32, tail-mask, scalar-operand` | planned | |
| `micro-op/compare-select/vcmps-i16-signed` | compare-select | `pto.vcmps` | `core-i16-signed, full-mask, scalar-operand` | planned | |
| `micro-op/compare-select/vcmps-i16-unsigned` | compare-select | `pto.vcmps` | `core-i16-unsigned, full-mask, scalar-operand` | planned | |
| `micro-op/compare-select/vcmps-f32-exceptional` | compare-select | `pto.vcmps` | `core-f32, full-mask, scalar-operand, exceptional-values` | planned | |
| `micro-op/conversion/vcvt-f32-to-f16` | conversion | `pto.vcvt` | `f32-to-f16, full-mask` | planned | |
| `micro-op/conversion/vcvt-f16-to-f32` | conversion | `pto.vcvt` | `f16-to-f32, full-mask` | planned | |
| `micro-op/conversion/vcvt-tail` | conversion | `pto.vcvt` | `f32-to-f16, tail-mask` | planned | |
| `micro-op/conversion/vcvt-f32-special` | conversion | `pto.vcvt` | `f32-to-f16, exceptional-values` | planned | |
| `micro-op/conversion/vcvt-i32-to-i16-overflow` | conversion | `pto.vcvt` | `i32-to-i16, integer-overflow` | blocked | `docs/isa/09-conversion-ops.md` 当前未明确列出 `i32 -> i16` 这一 A5 conversion pair |
| `micro-op/conversion/vtrc-f32-rounding` | conversion | `pto.vtrc` | `core-f32, round-r, round-z, round-f` | planned | |
| `micro-op/conversion/vtrc-f32-special` | conversion | `pto.vtrc` | `core-f32, exceptional-values` | planned | |
| `micro-op/materialization-predicate/vbr-f32` | materialization-predicate | `pto.vbr` | `core-f32, scalar-broadcast` | planned | |
| `micro-op/materialization-predicate/vbr-i32` | materialization-predicate | `pto.vbr` | `core-i32-signed, scalar-broadcast` | planned | |
| `micro-op/materialization-predicate/vdup-scalar` | materialization-predicate | `pto.vdup` | `core-f32, scalar-operand` | planned | |
| `micro-op/materialization-predicate/vdup-lane` | materialization-predicate | `pto.vdup` | `core-f32, lane-select` | planned | |
| `micro-op/materialization-predicate/pset-pattern` | materialization-predicate | `pto.pset_b16`, `pto.pset_b32`, `pto.pset_b8` | `pattern-mask, pat-all, pat-vl` | planned | |
| `micro-op/materialization-predicate/pge-tail-mask` | materialization-predicate | `pto.pge_b16`, `pto.pge_b32`, `pto.pge_b8` | `tail-mask` | planned | |
| `micro-op/materialization-predicate/plt-tail-mask` | materialization-predicate | `pto.plt_b16`, `pto.plt_b32`, `pto.plt_b8` | `tail-mask, scalar-carry-out` | planned | |
| `micro-op/materialization-predicate/ppack-punpack` | materialization-predicate | `pto.ppack`, `pto.punpack` | `pack-unpack-roundtrip` | planned | |
| `micro-op/materialization-predicate/pdintlv_b8` | materialization-predicate | `pto.pdintlv_b8` | `predicate-transform, lane-order` | planned | |
| `micro-op/materialization-predicate/pintlv_b16` | materialization-predicate | `pto.pintlv_b16` | `predicate-transform, lane-order` | planned | |
| `micro-op/materialization-predicate/pand` | materialization-predicate | `pto.pand` | `predicate-transform` | planned | |
| `micro-op/materialization-predicate/por` | materialization-predicate | `pto.por` | `predicate-transform` | planned | |
| `micro-op/materialization-predicate/pxor` | materialization-predicate | `pto.pxor` | `predicate-transform` | planned | |
| `micro-op/materialization-predicate/pnot` | materialization-predicate | `pto.pnot` | `predicate-transform` | planned | |
| `micro-op/materialization-predicate/psel` | materialization-predicate | `pto.psel` | `predicate-transform, predicate-select` | planned | |
| `micro-op/predicate-load-store/psts-plds` | predicate-load-store | `pto.plds`, `pto.psts` | `packed-predicate-roundtrip, scalar-offset, load-store-pair-preservation, representative-logical-elements` | planned | |
| `micro-op/predicate-load-store/pst-pld` | predicate-load-store | `pto.pld`, `pto.pst` | `packed-predicate-roundtrip, areg-offset, load-store-pair-preservation, representative-logical-elements` | planned | |
| `micro-op/predicate-load-store/psti-pldi` | predicate-load-store | `pto.pldi`, `pto.psti` | `packed-predicate-roundtrip, immediate-offset, load-store-pair-preservation, representative-logical-elements` | planned | |
| `micro-op/predicate-load-store/pstu` | predicate-load-store | `pto.pstu` | `unaligned-packed-store, state-update, representative-logical-elements` | planned | |
| `micro-op/reduction/vcadd` | reduction | `pto.vcadd` | `core-f32, result-placement` | planned | |
| `micro-op/reduction/vcadd-tail` | reduction | `pto.vcadd` | `core-f32, tail-mask, result-placement` | planned | |
| `micro-op/reduction/vcmax` | reduction | `pto.vcmax` | `core-f32, result-placement` | planned | |
| `micro-op/reduction/vcmin` | reduction | `pto.vcmin` | `core-f32, result-placement` | planned | |
| `micro-op/reduction/vcgadd` | reduction | `pto.vcgadd` | `group-reduction, result-placement` | planned | |
| `micro-op/reduction/vcgmax` | reduction | `pto.vcgmax` | `group-reduction, result-placement` | planned | |
| `micro-op/reduction/vcgmin` | reduction | `pto.vcgmin` | `group-reduction, result-placement` | planned | |
| `micro-op/reduction/vcpadd` | reduction | `pto.vcpadd` | `prefix-op, full-mask` | planned | |
| `micro-op/vector-load-store/vlds` | vector-load-store | `pto.vlds` | `core-f32, contiguous, full-mask, aligned, dist-norm` | planned | |
| `micro-op/vector-load-store/vlds-tail` | vector-load-store | `pto.vlds` | `core-f32, contiguous, tail-mask, aligned, dist-norm` | planned | |
| `micro-op/vector-load-store/vlds-brc-b32` | vector-load-store | `pto.vlds` | `core-f32, full-mask, aligned, dist-brc-b32` | planned | |
| `micro-op/vector-load-store/vsts` | vector-load-store | `pto.vsts` | `core-f32, contiguous, full-mask, aligned, dist-norm` | planned | |
| `micro-op/vector-load-store/vldas-vldus` | vector-load-store | `pto.vldas`, `pto.vldus` | `core-f32, full-mask, unaligned, stream-state` | planned | |
| `micro-op/vector-load-store/vldx2-vstx2` | vector-load-store | `pto.vldx2`, `pto.vstx2` | `core-f32, full-mask, paired-roundtrip, dintlv` | planned | |
| `micro-op/vector-load-store/vsld` | vector-load-store | `pto.vsld` | `core-f32, full-mask, strided-load` | planned | |
| `micro-op/vector-load-store/vsldb` | vector-load-store | `pto.vsldb` | `core-f32, full-mask, block-strided-load, block-mask` | planned | |
| `micro-op/gather-scatter/vscatter` | gather-scatter | `pto.vscatter` | `core-f32, full-mask, non-contiguous, explicit-index-pattern, scatter-store, store-effect-validation, no-alias` | planned | |
| `micro-op/vector-load-store/vsst` | vector-load-store | `pto.vsst` | `core-f32, full-mask, strided-store` | planned | |
| `micro-op/vector-load-store/vsstb` | vector-load-store | `pto.vsstb` | `core-f32, full-mask, block-strided-store, block-mask` | planned | |
| `micro-op/vector-load-store/vsta` | vector-load-store | `pto.vsta` | `core-f32, full-mask, aligned, state-update` | planned | |
| `micro-op/vector-load-store/vstas` | vector-load-store | `pto.vstas` | `core-f32, full-mask, aligned, immediate-offset, state-update` | planned | |
| `micro-op/vector-load-store/vstar` | vector-load-store | `pto.vstar` | `core-f32, full-mask, aligned, state-update` | planned | |
| `micro-op/vector-load-store/vstu` | vector-load-store | `pto.vstu` | `core-f32, full-mask, unaligned, state-update` | planned | |
| `micro-op/vector-load-store/vstus` | vector-load-store | `pto.vstus` | `core-f32, full-mask, unaligned, immediate-offset, state-update` | planned | |
| `micro-op/vector-load-store/vstur` | vector-load-store | `pto.vstur` | `core-f32, full-mask, unaligned, state-update` | planned | |
| `micro-op/gather-scatter/vgather2` | gather-scatter | `pto.vgather2` | `core-f32, full-mask, non-contiguous, explicit-index-pattern, load-effect-validation, no-alias` | planned | |
| `micro-op/gather-scatter/vgatherb` | gather-scatter | `pto.vgatherb` | `core-f32, full-mask, block-gather, aligned-base, load-effect-validation, no-alias` | planned | |
| `micro-op/gather-scatter/vgather2_bc` | gather-scatter | `pto.vgather2_bc` | `core-f32, full-mask, non-contiguous, masked-gather, load-effect-validation, no-alias` | planned | |
| `micro-op/rearrangement/vintlv-vdintlv` | rearrangement | `pto.vdintlv`, `pto.vintlv` | `paired-roundtrip, lane-order` | planned | |
| `micro-op/rearrangement/vslide` | rearrangement | `pto.vslide` | `lane-order, slide-window` | planned | |
| `micro-op/rearrangement/vshift` | rearrangement | `pto.vshift` | `lane-order, zero-fill` | planned | |
| `micro-op/rearrangement/vsqz` | rearrangement | `pto.vsqz` | `predicate-driven-rearrangement, stable-order` | planned | |
| `micro-op/rearrangement/vusqz` | rearrangement | `pto.vusqz` | `predicate-driven-rearrangement, placement` | planned | |
| `micro-op/rearrangement/vperm` | rearrangement | `pto.vperm` | `lane-order, explicit-index-pattern` | planned | |
| `micro-op/rearrangement/vpack` | rearrangement | `pto.vpack` | `pack-unpack, narrowing` | planned | |
| `micro-op/rearrangement/vsunpack` | rearrangement | `pto.vsunpack` | `pack-unpack, sign-extend` | planned | |
| `micro-op/rearrangement/vzunpack` | rearrangement | `pto.vzunpack` | `pack-unpack, zero-extend` | planned | |
| `micro-op/dsa-sfu/vlrelu-f32` | dsa-sfu | `pto.vlrelu` | `core-f32, scalar-operand, full-mask` | planned | |
| `micro-op/dsa-sfu/vlrelu-tail` | dsa-sfu | `pto.vlrelu` | `core-f32, tail-mask, scalar-operand` | planned | |
| `micro-op/dsa-sfu/vlrelu-f16` | dsa-sfu | `pto.vlrelu` | `core-f16, full-mask, scalar-operand` | planned | |
| `micro-op/dsa-sfu/vprelu-f32` | dsa-sfu | `pto.vprelu` | `core-f32, vector-alpha` | planned | |
| `micro-op/dsa-sfu/vexpdiff-f32` | dsa-sfu | `pto.vexpdiff` | `core-f32, fused-expdiff` | planned | |
| `micro-op/dsa-sfu/vaddrelu-f32` | dsa-sfu | `pto.vaddrelu` | `core-f32, fused-op` | planned | |
| `micro-op/dsa-sfu/vsubrelu-f32` | dsa-sfu | `pto.vsubrelu` | `core-f32, fused-op` | planned | |
| `micro-op/dsa-sfu/vaxpy-f32` | dsa-sfu | `pto.vaxpy` | `core-f32, scalar-operand, fused-op` | planned | |
| `micro-op/dsa-sfu/vaddreluconv` | dsa-sfu | `pto.vaddreluconv` | `fused-op, conversion-result` | planned | |
| `micro-op/dsa-sfu/vmulconv` | dsa-sfu | `pto.vmulconv` | `fused-op, conversion-result` | planned | |
| `micro-op/dsa-sfu/vmull` | dsa-sfu | `pto.vmull` | `widening-op, hi-lo-split` | planned | |
| `micro-op/dsa-sfu/vmula` | dsa-sfu | `pto.vmula` | `core-f32, fused-op, accumulator` | planned | |
| `micro-op/dsa-sfu/vci` | dsa-sfu | `pto.vci` | `index-generation` | planned | |
| `micro-op/dsa-sfu/vbitsort` | dsa-sfu | `pto.vbitsort` | `index-generation, layout-transform` | blocked | `docs/vpto-spec.md` 与 `docs/isa/13-dsa-sfu-ops.md` 目前只给出 surface/接口层信息，尚未形成可稳定闭环的 oracle 语义 |
| `micro-op/dsa-sfu/vtranspose` | dsa-sfu | `pto.vtranspose` | `ub-to-ub, layout-transform, representative-config` | planned | |

## Notes

- `case` 字段记录相对 `test/vpto/cases/` 的真实 case 路径；微指令单-op 例如 `micro-op/binary-vector/vadd`
- `tileop/` 下的 case 表示 tile 级或派生组合验证，不直接计入向量单 op 覆盖完成态
- 历史 `tileop/*` 或其他已有 case 只能作为骨架参考，不能直接填写到微指令单 op 覆盖条目的 `case` 字段里。
- 已转入文档漂移核对单的口径问题，不继续在本 matrix 中单独记账；待结论明确后再回填对应条目。
- 随执行推进，这份 matrix 应同步更新 `case`、`scenarios` 和 `status`，作为唯一静态追踪来源。
