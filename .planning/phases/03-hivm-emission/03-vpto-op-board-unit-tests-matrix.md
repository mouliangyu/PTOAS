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
| `pto.pipe_barrier` / `pto.barrier` / `pto.mem_bar` | pipeline-sync | `docs/isa/01-pipeline-sync.md` | no | reused-by-others | ordering / fence | infra | 仅作为支撑动作复用 |
| `pto.set_loop_size_outtoub` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.set_loop1_stride_outtoub` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.set_loop2_stride_outtoub` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.set_loop_size_ubtoout` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.set_loop1_stride_ubtoout` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.set_loop2_stride_ubtoout` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | dma loop setup | infra | 不单独立项测试 |
| `pto.copy_gm_to_ubuf` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | GM to UB feed | infra | 作为输入准备动作 |
| `pto.copy_ubuf_to_gm` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | UB to GM drain | infra | 作为输出导回动作 |
| `pto.copy_ubuf_to_ubuf` | dma-copy | `docs/isa/02-dma-copy.md` | no | reused-by-others | UB to UB move | infra | 仅在需要时复用 |

## In-scope Ops

| op | family | doc_source | in_scope | case | scenarios | status | notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `pto.vlds` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | contiguous load, representative dist | planned | 可参考现有 tileop/expand 类骨架，但需新增独立主语义 case |
| `pto.vlds_post` | vector-load-store | `include/PTO/IR/VPTOOps.td` | yes | tbd | post-update load | planned | 文档面需核对 |
| `pto.vldas` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | grouped-vldus | align seed | planned | 与 `vldus` 成组 |
| `pto.vldus` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | grouped-vldus | unaligned stream | planned | 与 `vldas` 成组 |
| `pto.uvld` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | ubuf load variant | planned | 需核对文档语义与现有实现 |
| `pto.vldx2` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | grouped-vldx2-vstx2 | dual/deinterleave load | planned | 与 `vstx2` 成组 |
| `pto.vsld` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | strided/slide load | planned | |
| `pto.vsldb` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | byte slide load | planned | |
| `pto.vgather2` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | tbd | non-contiguous gather | planned | |
| `pto.vgatherb` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | tbd | byte gather | planned | |
| `pto.vgather2_bc` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | tbd | gather with broadcast/carry | planned | |
| `pto.vsts` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | contiguous store, predicated store | planned | 可参考现有 tileop/expand 类骨架，但需新增独立主语义 case |
| `pto.vsts_post` | vector-load-store | `include/PTO/IR/VPTOOps.td` | yes | tbd | post-update store | planned | 文档面需核对 |
| `pto.vscatter` | gather-scatter | `docs/isa/03-vector-load-store.md` | yes | tbd | non-contiguous scatter | planned | |
| `pto.vsst` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | strided/scatter-like store | planned | |
| `pto.vstx2` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | grouped-vldx2-vstx2 | dual/deinterleave store | planned | 与 `vldx2` 成组 |
| `pto.vsstb` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | byte store variant | planned | |
| `pto.vsta` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | align-assisted store | planned | |
| `pto.vstas` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | align-assisted strided store | planned | |
| `pto.vstar` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | align-assisted reverse/rearranged store | planned | |
| `pto.vstu` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | unaligned store | planned | |
| `pto.vstus` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | unaligned strided store | planned | |
| `pto.vstur` | vector-load-store | `docs/isa/03-vector-load-store.md` | yes | tbd | unaligned rearranged store | planned | |
| `pto.plds` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | grouped-predicate-load-store | predicate load | planned | 与 `psts` 联动 |
| `pto.pld` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | grouped-predicate-offset | predicate load with areg offset | planned | 与 `pst` 联动 |
| `pto.pldi` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | grouped-predicate-immediate | predicate load with imm offset | planned | 与 `psti` 联动 |
| `pto.psts` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | grouped-predicate-load-store | predicate store | planned | 与 `plds` 联动 |
| `pto.pst` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | grouped-predicate-offset | predicate store with areg offset | planned | 与 `pld` 联动 |
| `pto.psti` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | grouped-predicate-immediate | predicate store with imm offset | planned | 与 `pldi` 联动 |
| `pto.pstu` | predicate-load-store | `docs/isa/04-predicate-load-store.md` | yes | tbd | unaligned predicate store | planned | |
| `pto.vbr` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | tbd | scalar broadcast | planned | |
| `pto.vdup` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | tbd | duplicate by position | planned | |
| `pto.pset_b8` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | grouped-mask-materialization | pattern mask | planned | 与 family 其他 pattern 共同覆盖 |
| `pto.pset_b16` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | grouped-mask-materialization | pattern mask | planned | |
| `pto.pset_b32` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | grouped-mask-materialization | pattern mask | planned | |
| `pto.pge_b8` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | grouped-tail-mask | tail mask | planned | |
| `pto.pge_b16` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | grouped-tail-mask | tail mask | planned | |
| `pto.pge_b32` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | grouped-tail-mask | tail mask | planned | |
| `pto.plt_b8` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | grouped-tail-mask | tail mask with scalar carry | planned | |
| `pto.plt_b16` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | grouped-tail-mask | tail mask with scalar carry | planned | |
| `pto.plt_b32` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | grouped-tail-mask | tail mask with scalar carry | planned | |
| `pto.ppack` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | tbd | pack predicate | planned | |
| `pto.punpack` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | tbd | unpack predicate | planned | |
| `pto.pnot` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | tbd | invert predicate | planned | |
| `pto.psel` | materialization-predicate | `docs/isa/05-materialization-predicate.md` | yes | tbd | predicate select | planned | |
| `pto.vabs` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | tbd | unary arithmetic | planned | 可参考 `tileop/abs` 的简化骨架，但需新增独立主语义 case |
| `pto.vexp` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | tbd | transcendental | planned | 可参考 `tileop/exp` 的现有链路，但需新增独立主语义 case |
| `pto.vln` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | tbd | logarithm | planned | |
| `pto.vsqrt` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | tbd | square root | planned | |
| `pto.vrec` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | tbd | reciprocal | planned | |
| `pto.vrelu` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | tbd | relu | planned | |
| `pto.vnot` | unary-vector | `docs/isa/06-unary-vector-ops.md` | yes | tbd | bitwise/logical not | planned | |
| `pto.vcadd` | reduction | `docs/isa/10-reduction-ops.md` | yes | tbd | full vector reduction | planned | |
| `pto.vcmax` | reduction | `docs/isa/10-reduction-ops.md` | yes | tbd | max reduction + placement | planned | |
| `pto.vcmin` | reduction | `docs/isa/10-reduction-ops.md` | yes | tbd | min reduction + placement | planned | |
| `pto.vadd` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | micro-op/binary-vector/vadd | core-f32, full-mask | board-passed | 当前仅覆盖最小主语义 f32 路径，未覆盖其他类型族与 tail-mask |
| `pto.vsub` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | tbd | elementwise sub | planned | 可参考 `tileop/sub` 的数据准备方式，但需新增独立主语义 case |
| `pto.vmul` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | tbd | elementwise mul | planned | 可参考 `tileop/mul` 的数据准备方式，但需新增独立主语义 case |
| `pto.vdiv` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | micro-op/binary-vector/vdiv | core-f32, full-mask | board-passed | 当前仅覆盖最小主语义 f32 路径，未覆盖 f16 与特殊值场景 |
| `pto.vmax` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | tbd | elementwise max | planned | 可参考 `tileop/max` 的数据准备方式，但需新增独立主语义 case |
| `pto.vmin` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | micro-op/binary-vector/vmin | core-f32, full-mask | board-passed | 当前仅覆盖最小主语义 f32 路径，未覆盖其他类型族与 tail-mask |
| `pto.vand` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | tbd | bitwise and | planned | |
| `pto.vor` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | tbd | bitwise or | planned | |
| `pto.vxor` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | tbd | bitwise xor | planned | |
| `pto.vshl` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | tbd | shift left | planned | |
| `pto.vshr` | binary-vector | `docs/isa/07-binary-vector-ops.md` | yes | tbd | shift right | planned | |
| `pto.vaddc` | binary-vector-special | `include/PTO/IR/VPTOOps.td` | yes | tbd | carry add | planned | 文档面需核对 |
| `pto.vsubc` | binary-vector-special | `include/PTO/IR/VPTOOps.td` | yes | tbd | carry sub | planned | 文档面需核对 |
| `pto.vaddcs` | binary-vector-special | `include/PTO/IR/VPTOOps.td` | yes | tbd | carry add scalar/state | planned | 文档面需核对 |
| `pto.vsubcs` | binary-vector-special | `include/PTO/IR/VPTOOps.td` | yes | tbd | carry sub scalar/state | planned | 文档面需核对 |
| `pto.vbcnt` | unary-vector-special | `include/PTO/IR/VPTOOps.td` | yes | tbd | bit count | planned | 文档面需核对 |
| `pto.vcls` | unary-vector-special | `include/PTO/IR/VPTOOps.td` | yes | tbd | classify/count leading sign | planned | 文档面需核对 |
| `pto.vcmp` | compare-select | `docs/isa/11-compare-select.md` | yes | tbd | representative cmp modes | planned | 可参考 `tileop/cmp` 的断言方式，但需单独补向量单 op 覆盖 |
| `pto.vcmps` | compare-select | `docs/isa/11-compare-select.md` | yes | tbd | scalar compare | planned | |
| `pto.vsel` | compare-select | `docs/isa/11-compare-select.md` | yes | tbd | mask select | planned | |
| `pto.vselr` | compare-select | `docs/isa/11-compare-select.md` | yes | tbd | reversed select | planned | |
| `pto.vselrv2` | compare-select | `docs/isa/11-compare-select.md` | yes | tbd | v2 select variant | planned | |
| `pto.vadds` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | micro-op/vec-scalar/vadds | core-f32, full-mask | board-passed | 当前仅覆盖最小主语义 f32 路径，未覆盖其他类型族、tail-mask 与标量来源扩展 |
| `pto.vmuls` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | tbd | vec * scalar | planned | 可参考 `tileop/muls` 的数据准备方式，但需新增独立主语义 case |
| `pto.vmaxs` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | tbd | vec max scalar | planned | |
| `pto.vmins` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | tbd | vec min scalar | planned | |
| `pto.vlrelu` | dsa-sfu | `docs/isa/13-dsa-sfu-ops.md` | yes | tbd | leaky relu | planned | |
| `pto.vshls` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | tbd | shift left scalar | planned | |
| `pto.vshrs` | vec-scalar | `docs/isa/08-vec-scalar-ops.md` | yes | tbd | shift right scalar | planned | |
| `pto.vtrc` | conversion-special | `include/PTO/IR/VPTOOps.td` | yes | tbd | trunc/round/convert family | planned | 文档面需核对 |
| `pto.vcvt` | conversion-special | `docs/isa/09-conversion-ops.md` | yes | tbd | type conversion | planned | |
| `pto.vci` | conversion-special | `include/PTO/IR/VPTOOps.td` | yes | tbd | convert/immediate family | planned | 文档面需核对 |
| `pto.vbitsort` | special | `include/PTO/IR/VPTOOps.td` | yes | tbd | bit sort | planned | |
| `pto.vmrgsort4` | special | `include/PTO/IR/VPTOOps.td` | yes | tbd | merge sort 4-way | planned | |
| `pto.pdintlv_b8` | rearrangement | `include/PTO/IR/VPTOOps.td` | yes | tbd | predicate/intlv family | planned | 文档面需核对 |
| `pto.pintlv_b16` | rearrangement | `include/PTO/IR/VPTOOps.td` | yes | tbd | predicate/intlv family | planned | 文档面需核对 |
| `pto.vintlv` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | grouped-intlv | interleave | planned | 与 `vdintlv` 成组 |
| `pto.vdintlv` | rearrangement | `docs/isa/12-data-rearrangement.md` | yes | grouped-intlv | deinterleave | planned | 与 `vintlv` 成组 |
| `pto.vintlvv2` | rearrangement | `include/PTO/IR/VPTOOps.td` | yes | tbd | interleave v2 | planned | 文档面需核对 |
| `pto.vdintlvv2` | rearrangement | `include/PTO/IR/VPTOOps.td` | yes | tbd | deinterleave v2 | planned | 文档面需核对 |
| `pto.vmull` | special | `include/PTO/IR/VPTOOps.td` | yes | tbd | widening multiply | planned | |
| `pto.vmula` | special | `include/PTO/IR/VPTOOps.td` | yes | tbd | mul accumulate | planned | |

## Notes

- `case` 字段记录相对 `test/vpto/cases/` 的真实 case 路径；微指令单-op 例如 `micro-op/binary-vector/vadd`
- `tileop/` 下的 case 表示 tile 级或派生组合验证，不直接计入向量单 op 覆盖完成态
- 历史 `tileop/*` 或其他已有 case 只能作为骨架参考，不能直接填写到微指令单 op 覆盖条目的 `case` 字段里。
- `include/PTO/IR/VPTOOps.td` 中存在但 `docs/isa` 尚未完整落定的 op，需要在正式落地 case 前先核对文档语义、打印形式和 oracle 可构造性。
- 随执行推进，这份 matrix 应同步更新 `case`、`scenarios` 和 `status`，作为唯一静态追踪来源。
