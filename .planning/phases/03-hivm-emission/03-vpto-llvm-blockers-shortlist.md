# VPTO LLVM Blockers Shortlist

## LLVM 编译不过

- `pto.vsel`
  - docs-signature: `pto.vsel %src0, %src1, %mask : !pto.vreg<...>, !pto.vreg<...>, !pto.mask -> !pto.vreg<...>`
  - vpto-signature: `pto.vsel %src0, %src1, %mask : !pto.vreg<...>, !pto.vreg<...>, !pto.mask -> !pto.vreg<...>`
  - llvm-signature: `declare <64 x float> @llvm.hivm.vsel.v64f32.z(<64 x float>, <64 x float>, <256 x i1>)`; `declare <128 x i16> @llvm.hivm.vsel.v128s16.z(<128 x i16>, <128 x i16>, <256 x i1>)`
  - blocker: `instruction selection` / `type legalization`
  - cases: `micro-op/compare-select/vsel`, `micro-op/compare-select/vsel-tail`, `micro-op/compare-select/vsel-i16`, `micro-op/compare-select/vsel-predicate-edge`

- `pto.pset_b8`, `pto.pset_b16`, `pto.pset_b32`
  - docs-signature: `pto.pset_b* "PAT_*" : !pto.mask`
  - vpto-signature: `pto.pset_b* "PAT_*" : !pto.mask`
  - llvm-signature: `declare <256 x i1> @llvm.hivm.pset.b8(i64)`; `declare <256 x i1> @llvm.hivm.pset.b16(i64)`; `declare <256 x i1> @llvm.hivm.pset.b32(i64)`
  - blocker: `Intrinsic has incorrect argument type`
  - cases: `micro-op/materialization-predicate/pset-pattern`, `micro-op/materialization-predicate/pset-pattern-fragment`, `micro-op/materialization-predicate/pdintlv_b8`, `micro-op/materialization-predicate/pdintlv_b8-nontrivial`, `micro-op/materialization-predicate/pintlv_b16`, `micro-op/materialization-predicate/pintlv_b16-nontrivial`
  - solve: 这三条的llvm入参都是i32

- `pto.pge_b8`, `pto.pge_b16`, `pto.pge_b32`
  - docs-signature: `pto.pge_b* %dist : i32 -> !pto.mask`
  - vpto-signature: `pto.pge_b* %dist : i32 -> !pto.mask`
  - llvm-signature: `declare <256 x i1> @llvm.hivm.pge.b8(i64, i64)`; `declare <256 x i1> @llvm.hivm.pge.b16(i64, i64)`; `declare <256 x i1> @llvm.hivm.pge.b32(i64, i64)`
  - blocker: `Intrinsic has incorrect argument type`
  - cases: `micro-op/materialization-predicate/pge-tail-mask`, `micro-op/materialization-predicate/pge-tail-mask-boundary`
  - solve: llvm入参为i32

- `pto.ppack`, `pto.punpack`
  - docs-signature: `pto.ppack %mask, %elems : !pto.mask, i64 -> !pto.mask`; `pto.punpack %mask, %elems : !pto.mask, i64 -> !pto.mask`
  - vpto-signature: `pto.ppack %mask, %elems : !pto.mask, i64 -> !pto.mask`; `pto.punpack %mask, %elems : !pto.mask, i64 -> !pto.mask`
  - llvm-signature: `declare <256 x i1> @llvm.hivm.ppack.z(<256 x i1>, i64)`; `declare <256 x i1> @llvm.hivm.punpack(<256 x i1>, i64)`
  - blocker: `Intrinsic has incorrect argument type`
  - cases: `micro-op/materialization-predicate/ppack-punpack`, `micro-op/materialization-predicate/ppack-punpack-nontrivial`
  - solve: llvm的elems入参为i32

- `pto.pst`, `pto.pld`
  - docs-signature: `pto.pst %mask, %base[%off] : !pto.mask, !pto.ptr<ui8, ub>`; `pto.pld %base[%off] : !pto.ptr<ui8, ub> -> !pto.mask`
  - vpto-signature: `pto.pst %mask, %base[%off] : !pto.mask, !pto.ptr<ui8, ub>`; `pto.pld %base[%off] : !pto.ptr<ui8, ub> -> !pto.mask`
  - llvm-signature: `declare void @llvm.hivm.pst.b8(<256 x i1>, ptr, i32, i32, i32)`; `declare <256 x i1> @llvm.hivm.pld.b8(ptr, i32, i32, i32)`
  - blocker: `Intrinsic has incorrect argument type`
  - cases: `micro-op/predicate-load-store/pst-pld`

- `pto.psti`, `pto.pldi`
  - docs-signature: `pto.psti unresolved`; `pto.pldi unresolved`
  - vpto-signature: `pto.psti unresolved`; `pto.pldi unresolved`
  - llvm-signature: `declare void @llvm.hivm.psti.b8(...)`; `declare <256 x i1> @llvm.hivm.pldi.b8(...)`
  - blocker: `instruction selection`
  - cases: `micro-op/predicate-load-store/psti-pldi`

- `pto.vsld`
  - docs-signature: `pto.vsld unresolved`
  - vpto-signature: `pto.vsld unresolved`
  - llvm-signature: `declare <64 x float> @llvm.hivm.vsld(ptr addrspace(6), i32, i32, i32)`
  - blocker: `Intrinsic has incorrect argument type`
  - cases: `micro-op/vector-load-store/vsld`, `micro-op/vector-load-store/vsld-vsst-stride-boundary`

- `pto.vldx2`, `pto.vstx2`
  - docs-signature: `pto.vldx2 unresolved`; `pto.vstx2 unresolved`
  - vpto-signature: `pto.vldx2 unresolved`; `pto.vstx2 unresolved`
  - llvm-signature: `declare ... @llvm.hivm.vldx2(...)`; `declare ... @llvm.hivm.vstx2(...)`
  - blocker: `Intrinsic has incorrect argument type`
  - cases: `micro-op/vector-load-store/vldx2-vstx2`, `micro-op/vector-load-store/vldx2-layout-check`, `micro-op/vector-load-store/vstx2-layout-check`

- `pto.vgatherb`
  - docs-signature: `pto.vgatherb unresolved`
  - vpto-signature: `pto.vgatherb unresolved`
  - llvm-signature: `declare ... @llvm.hivm.vgatherb.v300.*(...)`
  - blocker: `Intrinsic has incorrect argument type`
  - cases: `micro-op/gather-scatter/vgatherb`, `micro-op/gather-scatter/vgatherb-block-boundary`

- `pto.vaxpy`
  - docs-signature: `pto.vaxpy unresolved`
  - vpto-signature: `pto.vaxpy unresolved`
  - llvm-signature: `declare <64 x float> @llvm.hivm.vaxpy.v64f32.x(...)`
  - blocker: `instruction selection`
  - cases: `micro-op/dsa-sfu/vaxpy-f32`

- `pto.vci`
  - docs-signature: `pto.vci unresolved`
  - vpto-signature: `pto.vci unresolved`
  - llvm-signature: `declare <64 x i32> @llvm.hivm.vci.v64s32(i32, i64)`
  - blocker: `Intrinsic has incorrect argument type`
  - cases: `micro-op/dsa-sfu/vci`

## Docs / VPTO / LLVM 不对称

- `pto.vselr`
  - docs-signature: `pto.vselr unresolved`
  - vpto-signature: `pto.vselr unresolved`
  - llvm-signature: `declare ... @llvm.hivm.vselr.*(...)`; `declare ... @llvm.hivm.vselrv2.*(...)`
  - blocker: `docs semantic unresolved`
  - cases: `micro-op/compare-select/vselr`

- `pto.vmov`
  - docs-signature: `pto.vmov %input, %mask : !pto.vreg<...>, !pto.mask -> !pto.vreg<...>`
  - vpto-signature: `pto.vmov %input, %mask : !pto.vreg<...>, !pto.mask -> !pto.vreg<...>`
  - llvm-signature: `declare ... @llvm.hivm.vmov.*.m(...)`
  - blocker: `docs/vpto/llvm arity mismatch`
  - cases: `micro-op/unary-vector/vmov`, `micro-op/unary-vector/vmov-tail`

- `pto.vsubs`
  - docs-signature: `pto.vsubs %input, %scalar, %mask : !pto.vreg<...>, T, !pto.mask -> !pto.vreg<...>`
  - vpto-signature: `pto.vsubs %input, %scalar, %mask : !pto.vreg<...>, T, !pto.mask -> !pto.vreg<...>`
  - llvm-signature: `not observed`
  - blocker: `docs/toolchain mismatch`
  - cases: `micro-op/vec-scalar/vsubs`, `micro-op/vec-scalar/vsubs-tail`

- `pto.vands`, `pto.vors`, `pto.vxors`
  - docs-signature: `pto.vands|vors|vxors %input, %scalar, %mask : !pto.vreg<...>, T, !pto.mask -> !pto.vreg<...>`
  - vpto-signature: `pto.vands|vors|vxors %input, %scalar, %mask : !pto.vreg<...>, T, !pto.mask -> !pto.vreg<...>`
  - llvm-signature: `not observed`
  - blocker: `docs/toolchain mismatch`
  - cases: `micro-op/vec-scalar/vands`, `micro-op/vec-scalar/vands-mask-edge`, `micro-op/vec-scalar/vors`, `micro-op/vec-scalar/vors-mask-edge`, `micro-op/vec-scalar/vxors`, `micro-op/vec-scalar/vxors-mask-edge`

- `pto.vshls`, `pto.vshrs`
  - docs-signature: `pto.vshls|vshrs %input, %scalar, %mask : !pto.vreg<...>, T, !pto.mask -> !pto.vreg<...>`
  - vpto-signature: `pto.vshls|vshrs %input, %scalar : !pto.vreg<...>, T -> !pto.vreg<...>`
  - llvm-signature: `unconfirmed`
  - blocker: `docs/vpto signature mismatch`
  - cases: `micro-op/vec-scalar/vshls`, `micro-op/vec-scalar/vshls-shift-boundary`, `micro-op/vec-scalar/vshrs`, `micro-op/vec-scalar/vshrs-shift-boundary`

- `pto.vlds` with `BRC_B32`
  - docs-signature: `pto.vlds unresolved`
  - vpto-signature: `pto.vlds unresolved`
  - llvm-signature: `unconfirmed`
  - blocker: `docs/vpto legality mismatch`
  - cases: `micro-op/vector-load-store/vlds-brc-b32`

- `pto.vsldb`, `pto.vsstb`
  - docs-signature: `pto.vsldb unresolved`; `pto.vsstb unresolved`
  - vpto-signature: `pto.vsldb unresolved`; `pto.vsstb unresolved`
  - llvm-signature: `unconfirmed`
  - blocker: `packed offset encoding unresolved`
  - cases: `micro-op/vector-load-store/vsldb`, `micro-op/vector-load-store/vsstb`

- `pto.vstu`, `pto.vstus`, `pto.vstur`, `pto.vsta`, `pto.vstas`
  - docs-signature: `unresolved`
  - vpto-signature: `unresolved`
  - llvm-signature: `unconfirmed`
  - blocker: `docs/vpto signature mismatch`
  - cases: `micro-op/vector-load-store/vstu`, `micro-op/vector-load-store/vstus`, `micro-op/vector-load-store/vstur`, `micro-op/vector-load-store/vsta`, `micro-op/vector-load-store/vstas`, `micro-op/vector-load-store/vsta-state-advance`, `micro-op/vector-load-store/vstu-state-advance`, `micro-op/vector-load-store/vstas-vstus-offset-update`

- `pto.vgatherb`
  - docs-signature: `pto.vgatherb unresolved`
  - vpto-signature: `pto.vgatherb unresolved`
  - llvm-signature: `wrapper: vgatherb(dst, base, vector_u32 indexOffset)`
  - blocker: `vpto/llvm signature mismatch`
  - cases: `micro-op/gather-scatter/vgatherb`, `micro-op/gather-scatter/vgatherb-block-boundary`

- `pto.vusqz`
  - docs-signature: `pto.vusqz unresolved`
  - vpto-signature: `pto.vusqz unresolved`
  - llvm-signature: `unconfirmed`
  - blocker: `docs/vpto input model unresolved`
  - cases: `micro-op/rearrangement/vusqz`, `micro-op/rearrangement/vusqz-nontrivial-mask`

- `pto.vpack`
  - docs-signature: `pto.vpack unresolved`
  - vpto-signature: `pto.vpack unresolved`
  - llvm-signature: `wrapper: vpack(dst, src, part, mode)`
  - blocker: `vpto/llvm signature mismatch`
  - cases: `micro-op/rearrangement/vpack`

- `pto.vshift`
  - docs-signature: `pto.vshift unresolved`
  - vpto-signature: `pto.vshift unresolved`
  - llvm-signature: `unresolved`
  - blocker: `docs/llvm mismatch`
  - cases: `micro-op/rearrangement/vshift`, `micro-op/rearrangement/vshift-tail-zero-fill`

- `pto.vprelu`, `pto.vexpdiff`, `pto.vaddrelu`, `pto.vsubrelu`
  - docs-signature: `unresolved`
  - vpto-signature: `unresolved`
  - llvm-signature: `wrapper: vprelu(dst, src0, src1, mask, mode)`; `vexpdiff: not observed`; `vaddrelu: not observed`; `vsubrelu: not observed`
  - blocker: `docs/vpto/llvm signature mismatch`
  - cases: `micro-op/dsa-sfu/vprelu-f32`, `micro-op/dsa-sfu/vprelu-tail`, `micro-op/dsa-sfu/vexpdiff-f32`, `micro-op/dsa-sfu/vexpdiff-boundary`, `micro-op/dsa-sfu/vaddrelu-f32`, `micro-op/dsa-sfu/vsubrelu-f32`

- `pto.vaddreluconv`, `pto.vmulconv`
  - docs-signature: `unresolved`
  - vpto-signature: `unresolved`
  - llvm-signature: `unconfirmed`
  - blocker: `docs/vpto signature mismatch`
  - cases: `micro-op/dsa-sfu/vaddreluconv`, `micro-op/dsa-sfu/vmulconv`

- `pto.vtranspose`
  - docs-signature: `pto.vtranspose %dest, %src, %config`
  - vpto-signature: `pto.vtranspose %dest, %src, %config`
  - llvm-signature: `not observed as single intrinsic`
  - blocker: `docs/llvm lowering mismatch`
  - cases: `micro-op/dsa-sfu/vtranspose`, `micro-op/dsa-sfu/vtranspose-multi-config`

- `pto.pstu`
  - docs-signature: `pto.pstu %mask, %base[...] : !pto.mask, !pto.ptr<T, ub>`
  - vpto-signature: `pto.pstu %mask, %base[...] : !pto.mask, !pto.ptr<T, ub>`
  - llvm-signature: `wrapper: pstu_b16|pstu_b32`
  - blocker: `type-set mismatch`
  - cases: `micro-op/predicate-load-store/pstu`, `micro-op/predicate-load-store/pstu-state-advance-boundary`

- packed predicate roundtrip
  - docs-signature: `unresolved`
  - vpto-signature: `unresolved`
  - llvm-signature: `unconfirmed`
  - blocker: `docs/vpto surface mismatch`
  - cases: `micro-op/predicate-load-store/psts-plds-packed-prefix-boundary`
