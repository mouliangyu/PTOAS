# VMI grouped strided store — ND→NZ on-the-fly layout transform (discussion)

**Status:** design discussion / RFC. Explores how the `pto.vmi` grouped-store
surface should implement an **ND → NZ** UB write when the store result feeds the
Cube, and how to keep the block-strided store (`pto.vsstb`) bandwidth-efficient
across dtypes/shapes.

**Baseline in this repo.** `pto.vmi.group_store` already takes a stride and
`num_groups` (see `test/vpto/cases/vmi/group-load-s32-stride-store`), and the ISA
exposes `pto.vsstb %value, %dest, %block_stride, %repeat_stride, %mask`
(`docs/isa/micro-isa/03-vector-load-store.md`). This doc asks: **when the consumer
is the Cube (which wants NZ), what should the strided store lower to, and how do we
keep `vsstb` bandwidth high for small-N / different-dtype cases?**

**Refs:** `docs/vpto-spec.md` §"Cube Internal Buffer Layout: NZ Fractal Format";
`docs/isa/micro-isa/03-vector-load-store.md` (`pto.vsstb`, `pto.vsldb`);
`../../../PTO-Gym/docs/pto-vmi-design-exploration.md` §5–6 (cost model + rules of
thumb — the strided/permute cost numbers used here).

---

## 0. The scenario

The `quant-f32-to-f16-tail` kernel does a plain **ND → ND** store to UB. But if the
output is consumed by the Cube, the Cube wants the tile in **NZ fractal layout**.
We want to perform the ND → NZ transform **on the fly during the vector store**,
not as a separate pass, by using a **block-strided store**.

Hardware constants (from the spec):

```
C0 = 32 bytes                          (fractal inner width, always 32B)
E  = sizeof(T)                         (bf16 -> 2, fp8 -> 1, fp32 -> 4)
N0 = K0 = C0 / E                       (bf16 -> 16, fp8 -> 32, fp32 -> 8)
M0 = 16                                (fractal inner height, always 16 rows)
fractal M0 x N0 z-block = 512 B        (bf16 16x16, fp8 16x32, fp32 16x8)
VL = 256 B = 8 x C0                    (one vector register spans 8 C0 blocks)
```

For a logical `[M, N]` tile, the Cube-input NZ fractal — the `N1 M1 M0 N0` **mat-tile**
order the Cube consumes as an **input** tile, staged in **L1/cbuf**, *not* the L0C
accumulator — re-indexes as:

```
n1 = n / N0,  n0 = n % N0
m1 = m / M0,  m0 = m % M0
NZ offset(m,n) = n1*(M1*M0*N0) + m1*(M0*N0) + m0*N0 + n0      [elements]
                 └── outer N1 ──┘└── outer M1 ─┘└ m0 ┘└ n0 ┘
```

So ND → NZ is: **reshape-split** `M→(M1,M0)`, `N→(N1,N0)`, then **move `N1` to the
outermost axis** (a transpose of the `N1` and `M1` block axes). The inner `M0×N0`
fractal (one 512B z-block) stays contiguous; only the *outer* block placement moves.

### 0.1 Picture — where each C0 goes

One row `m` of ND is a run of `N` contiguous elements = `N1` C0-blocks. In NZ those
`N1` blocks are **scattered** to `N1` different outer positions, each `M1*M0*N0`
elements apart:

```
ND row m (contiguous):   [ n1=0 |C0| ][ n1=1 |C0| ][ n1=2 |C0| ] ... [ n1=N1-1 ]
                              │            │            │                  │
                              ▼            ▼            ▼                  ▼      (stride = M1*M0*N0 each)
NZ:  ...[n1=0 block for m].........[n1=1 block for m].........[n1=2 block for m]...
        base + m1*M0*N0+m0*N0   +1*(M1*M0*N0)          +2*(M1*M0*N0)
```

That "same value, contiguous in ND, strided-by-`M1*M0*N0` in NZ" is exactly what a
**block-strided store** expresses: hand it a contiguous vreg and a per-block stride.

### 0.2 The 3D `(M, N1, N0=C0)` view — where the `M*C0` stride lives

Drop `M0`/`M1` for a moment and read the tensor as **`(M, N1, N0)`** with
`N0 = C0`. ND is **M-major** (each row's `N1` C0-blocks are contiguous); the
Cube-input NZ is **N1-major** (all `M` rows of one `n1` sit together as a "slab",
then the next `n1`). The transform is the `M ↔ N1` axis swap:

```
ND source  (M-major: a row is contiguous)            addresses grow →
                 N0=C0 elems inside each block
        n1=0        n1=1        n1=2        n1=3
      ┌─────────┬─────────┬─────────┬─────────┐
 m=0  │ C0      │ C0      │ C0      │ C0      │   ← row 0 fully contiguous (N elems)
      ├─────────┼─────────┼─────────┼─────────┤
 m=1  │ C0      │ C0      │ C0      │ C0      │
      ├─────────┼─────────┼─────────┼─────────┤
 ...  │         │         │         │         │
 m=M-1│ C0      │ C0      │ C0      │ C0      │
      └─────────┴─────────┴─────────┴─────────┘

                    ── ND→NZ = swap M ↔ N1 ──▼

NZ dest  (N1-major: a whole n1 "slab" of all M rows is contiguous)
 |◄─────── slab n1=0 = M*C0 ───────►|◄─────── slab n1=1 = M*C0 ───────►| ...
 ┌────┬────┬─── ─┬────┐              ┌────┬────┬─── ─┬────┐
 │m=0 │m=1 │ ... │mM-1│              │m=0 │m=1 │ ... │mM-1│               (each cell = one C0)
 └────┴────┴─────┴────┘              └────┴────┴─────┴────┘
  ▲                                   ▲
  base + n1=0 slab                    base + 1·(M*C0)     ← the N1 stride = M·C0 bytes
                                                            (= M1*M0 in C0 units)
```

**So the `M*C0` stride *is* the size of one `n1` slab** = `M` rows × `C0` =
`M1*M0*N0` elements. That is precisely the `vsstb` `block_stride` (§1): stepping one
block in the vreg (`n1 → n1+1`) jumps a whole `M*C0` slab in UB.

### 0.3 Why it is called **NZ** — the Z-order inside one `n1` slab

Zoom into a single `n1` slab. Its contiguous address runs **down the `M*C0`
dimension**: within each `m` row you go **right** across the `N0 = C0` lanes, then
drop **down** to the next `m` and go right again — a raster "Z" sweep. That
`N`-outer / `Z`-inner shape is what **NZ** names.

```
one n1 slab  (contiguous address ↓ runs along the M*C0 dimension)
                N0 = C0 lanes  (n0: 0 → C0-1)
              ┌───────────────────────────►┐
       m0=0   │  a0  a1  a2  ...      aC0-1 │──┐   go right across C0 …
              ├────────────────────────────┤  │
       m0=1   │  b0  b1  b2  ...      bC0-1 │◄─┘   … then down to next m (the "Z")
              ├────────────────────────────┤
       m0=2   │  c0  c1  ...                │
              │        ...                  │
       m0=M-1 │                             │
              └────────────────────────────┘
  address(m0, n0) = m0*C0 + n0      ← contiguous down the slab = the M*C0 run
```

So "go right `C0`, go down `m`" is the inner Z; stacking `N1` such slabs
side-by-side (each `M*C0` apart, §0.2) is the outer **N**. The vector store's job is
to feed each slab's `m*C0` column while hopping `N1` by the `M*C0` slab stride.

---

## 1. Baseline: `vsstb` block-strided scatter (one store = 8 C0 blocks)

`pto.vsstb` writes the vreg's 8 C0-blocks, each to `base + repeat_stride +
blk*block_stride`, with `block_stride`/`repeat_stride` in **32B-block units** and a
per-block predicate:

```c
for (int blk = 0; blk < 8; ++blk)                 // VL = 8 blocks
    if (pg[blk]) UB_block[base + repeat_stride + blk*block_stride] = src_block[blk];
```

To emit NZ, put one m-row's `N1` C0-blocks in the vreg and set
`block_stride = M1*M0` (C0 units, i.e. `M1*M0*N0` elements — the `N1` outer stride):

```
vreg (ND, one m-row):   [C0 n1=0][C0 n1=1][C0 n1=2][C0 n1=3] ...  (up to 8 blocks)
                            │        │        │        │
   vsstb block_stride=M1*M0 ▼        ▼        ▼        ▼
UB (NZ):  blk0 @ base+0 ; blk1 @ base+M1*M0 ; blk2 @ base+2*M1*M0 ; ...   (C0 units)
```

Placed on the `(M, N1, N0=C0)` slab picture from §0.2, one `vsstb` (for a fixed `m`)
drops **one C0 into each `n1` slab, all at the same `m` offset**, stepping by the
`M*C0` slab stride:

```
                  block_stride = M*C0 (one whole n1 slab)
              ┌───────────────►┬───────────────►┬───────────────►┐
 vreg[m]:  [C0 n1=0]        [C0 n1=1]        [C0 n1=2]        [C0 n1=3]
              │                │                │                │
              ▼                ▼                ▼                ▼
 NZ:  |◄─ slab n1=0 (M*C0) ─►|◄─ slab n1=1 ─►|◄─ slab n1=2 ─►|◄─ slab n1=3 ─►|
      [..|m|..............]   [..|m|........]  [..|m|........]  [..|m|........]
          ▲ base+m*C0             ▲ +1·M*C0        ▲ +2·M*C0        ▲ +3·M*C0
       (repeat_stride = m*C0 picks the m offset inside every slab)
```

So `block_stride = M*C0` = the N1 slab size, and `repeat_stride = m*C0` selects
which row inside each slab this store writes. One `vsstb` = one `m`, all `N1` blocks.

**Bandwidth rule.** `vsstb` is one 9-cycle op that moves up to **8 C0-blocks**. It
runs at full store bandwidth only when all 8 blocks are live, i.e. when the number
of C0-blocks the store scatters is **8**. The efficiency question below is entirely
*"can we keep all 8 blocks busy for this dtype/shape?"*

**Remark — UB bank-conflict padding.** The exact `block_stride = M*C0` (`= M1*M0`
C0 units) is a power-of-two-ish multiple, so the 8 concurrent C0 writes of one
`vsstb` tend to land on the **same UB banks** → a store bank conflict that serializes
the 8 blocks. The standard fix is to **pad each `n1` slab by one C0** so the stride
becomes odd relative to the bank count:

```
padded slab stride = (M1*M0 + 1)·C0   =  (16·M1 + 1)·C0        (M0 = 16)
                   = M*C0  +  one C0 of padding per n1 slab
```

i.e. the mat tile is allocated with a **padded leading dimension** `(M + 1)` rows
(in C0 units) instead of `M`, and `block_stride = 16·M1 + 1`. This costs `N1` extra
C0 of UB per tile but removes the bank conflict. The `+1` padding applies wherever a
slab stride is used — including §3's half-height slabs (`block_stride = M1_half*M0 +
1`). The consumer's mat-tile descriptor must use the same padded leading dimension.

---

## 2. The problem: small `N` underutilizes `vsstb` (bf16, N = 64)

For **bf16**, `N0 = 16`, so `N = 64` ⇒ `N1 = 4`. One m-row is only `4` C0-blocks
(`64 × 2B = 128B`). A naive per-row NZ store therefore feeds `vsstb` only **4 of 8**
blocks → **50% store bandwidth wasted**:

```
one m-row = 4 C0 (128B)          vsstb capacity = 8 C0 (256B)
vreg:  [C0 n1=0][C0 n1=1][C0 n1=2][C0 n1=3][  --  ][  --  ][  --  ][  --  ]
        \_______________ 4 live blocks _______________/ \___ 4 idle blocks ___/
```

On the slab picture (only `N1 = 4` slabs exist), one `vsstb` touches all 4 — but
that is only half the store's 8-block capacity:

```
            block_stride = M*C0
        ┌────────►┬────────►┬────────►┐        (only 4 slabs ⇒ only 4 blocks emitted)
 vreg[m]: [n1=0]  [n1=1]   [n1=2]   [n1=3]  ·· 4 idle lanes ··
            ▼        ▼        ▼        ▼
 NZ:  |◄ slab0 M*C0 ►|◄ slab1 ►|◄ slab2 ►|◄ slab3 ►|
      [..|m|.......]  [..|m|..] [..|m|..] [..|m|..]
```

Any dtype/shape with `N1 < 8` hits this: bf16 N<128, fp32 N<64, etc. The store is
correct but half-empty.

---

## 3. Optimization 1 — re-block `M` so 8 blocks are always live (bf16)

**Idea (your proposal).** Pack a *second* outer axis into the store so
`M2 × N1 = 8`. Split `M = M2 · M1_half · M0` and emit the NZ variant
`(M2, N1, M1_half, M0, N0)`. Now flatten the outer `(M2, N1)` pair into the 8
`vsstb` blocks. With bf16 N=64: `M2 = 2`, `N1 = 4` ⇒ `2 × 4 = 8` blocks.

The vreg now holds `M2 = 2` m-rows × `N = 64` = `128` elems = **256B = one full VL**
(`2 × 64 × 2B = 256B`), and the 8 blocks scatter with a **uniform** stride:

```
flatten block index  b = m2*N1 + n1   (m2 in 0..1, n1 in 0..3)
NZ offset(b) = m2*(N1*M1_half*M0*N0) + n1*(M1_half*M0*N0)
             = (m2*N1 + n1) * (M1_half*M0*N0)
             = b * (M1_half*M0*N0)            ← uniform! block_stride = M1_half*M0 (C0 units)

vreg (2 rows x 4 n1, packed):
   [ m2=0: C0 n1=0 | C0 n1=1 | C0 n1=2 | C0 n1=3 ][ m2=1: C0 n1=0 | ... | C0 n1=3 ]
      b=0     b=1     b=2      b=3        b=4       b=5    b=6     b=7
        │       │       │        │          │        │      │       │
 vsstb  ▼       ▼       ▼        ▼          ▼        ▼      ▼       ▼   (stride M1_half*M0)
UB(NZ): all 8 blocks live  →  100% store bandwidth
```

On the slab picture: reblocking makes each slab **half-height** — `M1_half*M0*C0 =
(M/M2)*C0` instead of `M*C0` — and there are now `M2*N1 = 8` such slabs. So the
`block_stride` shrinks from `M` to `M/M2` (C0 units), and one `vsstb` writes one C0
into all 8 half-slabs at once:

```
 |◄ m2=0,n1=0 (M/M2·C0) ►|◄ m2=0,n1=1 ►|◄ m2=0,n1=2 ►|◄ m2=0,n1=3 ►|◄ m2=1,n1=0 ►| ... |
   b=0                     b=1            b=2            b=3            b=4        ... b=7
   ▲ +0                    ▲ +1·(M/M2·C0) ...  uniform block_stride = M/M2 (C0 units) ...
```

**Pros.** Doubles store bandwidth for the small-N case; still one `vsstb` per
group; no register shuffles.

**Cons / caveats.**
- **The programmer must be aware of this M-reblocked layout and reassemble it at
  UB→L1.** The `M2` split means the vector core lays the result down as `M2`
  separate **vecTile** pieces in UB; staging them as a single Cube-input **mat tile**
  in L1 then needs `M2` `pto.tinsert` transfers (`2× vecTile → 1 mat tile` for
  `M2 = 2`) to merge the pieces into one contiguous mat tile (the same
  Split-M/Split-N mat-tile assembly the spec describes). That extra `tinsert`
  bookkeeping is the real cost — **the Cube still reads a normal mat tile; nothing
  about its read order changes.**
- Only clean when `M2 · N1 = 8` (or a divisor that tiles 8). `N1 = 4 → M2 = 2`;
  `N1 = 2 → M2 = 4`; `N1 = 8 → M2 = 1` (degenerates to the baseline).
- `block_stride = M1_half*M0` must still fit the `i16` control field (see §5).

---

## 4. The `pto.vmi` interface question: who expresses the M2-reblock?

The interesting design question for §3 is **not** the store lowering (that is just
`vsstb` with the right `block_stride`) — it is **how the user expresses the
optimization, and how much the compiler can infer.** The M2-reblock only pays off if
the register holds `M2` rows so the store can fill 8 blocks. Bringing `M2` rows into
one register is a **tiling decision**, and per the nxVL philosophy (programmer owns
the schedule; compiler owns *local* layout) that decision should be **user-visible**,
while the block scatter / predicate / mask stay **compiler-inferred**. That splits
the design into two proposals.

### Proposal A — user assembles a wide logical nxVL; compiler infers the NZ store

The user loads `M2 = 2` rows into **one wider logical value** and computes on it as
if it were a flat `128×f32`; the `K = 2` physical fan-out and the NZ store scatter
are inferred.

```mlir
// User expresses the M2 assembly as a strided group load (a tiling knob):
//   2 groups (m2 = 0,1), each 64xf32 = 1 VL, group stride = M1_half*M0*N in UB.
%x = pto.vmi.group_load %ub_src[%off], %m2_stride {num_groups = 2}
     : !pto.ptr<f32, ub> -> !pto.vmi.vreg<128xf32>          // K=2, two m2 rows

%s   = pto.vmi.mulf   %x, %scale                            // compute is transparent K=2 fan-out
%y   = pto.vmi.truncf %s : !pto.vmi.vreg<128xf32> -> !pto.vmi.vreg<128xbf16>   // cvt on the wide value

// Store carries the NZ intent; block scatter is inferred from (128-lane value, NZ params):
pto.vmi.store_nz %y, %ub_dst {M, N, m0 = 16, c0 = 32B}
     : !pto.vmi.vreg<128xbf16>, !pto.ptr<bf16, ub>
     // → one vsstb, 8 blocks (m2 × n1), block_stride = M1_half*M0 → 100% BW
```

- **User-visible (schedule):** the choice to pack `M2 = 2` rows — expressed as the
  `num_groups`/stride on the load. This is the only knob; it *is* the optimization.
- **Compiler-inferred (local layout):** the `K = 2` fan-out of the compute/`cvt`, the
  8-block `block_stride`, the per-block predicate, and the tail. The `cvt` and
  elementwise ops never mention M2 — they just see a `128×T` value.
- **Still user-aware at UB→L1:** because the result is laid down as `M2` vecTile
  pieces, the programmer merges them into one Cube-input mat tile with `M2`
  `pto.tinsert` calls (§3 caveat).

### Proposal B — user stays single-row; compiler does the simple half-BW store

If the user writes the natural per-row value (`64×f32`, one m-row), the compiler
just emits the 4-block `vsstb` (bf16 N=64) at **50% store bandwidth** — correct and
simplest, no M2 awareness, no `tinsert` merge. There is *no* compiler trick that
reaches 8 blocks from a single 4-block row (an "unpack/deinterleave" only reshuffles
within the 4 live blocks; it cannot invent a second row), so B is genuinely the
half-BW path.

```mlir
%x = pto.vmi.load %ub_src[%off]      : !pto.ptr<f32,ub> -> !pto.vmi.vreg<64xf32>
%y = pto.vmi.truncf (pto.vmi.mulf %x, %scale) : ... -> !pto.vmi.vreg<64xbf16>
pto.vmi.store_nz %y, %ub_dst {M, N, ...}     // → 4-block vsstb, 50% BW, no tinsert merge
```

### Division of labor (the recommendation)

| Concern | Owner | Why |
|---|---|---|
| pack `M2` rows into one register | **user** (load `num_groups`/stride) | it is a tiling/schedule choice (P8) — the compiler must not silently re-tile the loop |
| `K`-fan-out of compute / `cvt` | **compiler** | local, transparent; the value is just `M2·N × T` |
| block scatter (`block_stride`), predicate, tail | **compiler** | derived from the value shape + NZ params |
| UB→L1 mat-tile merge (`tinsert`) | **user** (with sugar) | crosses the C↔V boundary; see O-NZ.3 |

So the interface is: **a strided/grouped load (user picks `M2`) + a `store_nz` that
carries the NZ params (compiler infers the scatter).** Proposal A is opt-in for the
full-BW path; omitting the M2 grouping falls back to Proposal B automatically. This
keeps the "static shape ⇒ inferred branch" property — the compiler selects the
`block_stride` and 4-vs-8-block form from the logical value's static width and the
NZ params — **without** the user hand-writing any `PART`/`INTLV`/`block_stride`
token. The store never needs a register transpose (Appendix A).

Static-shape strategy the compiler selects (all on `vsstb`):

| Static condition | Strategy | Store |
|---|---|---|
| `N1 == 8` (e.g. fp8 N=256, fp32 N=64) | per-row scatter | `vsstb`, `block_stride = M1*M0`, 100% BW |
| `N1 < 8`, user grouped `M2·N1 == 8` (Prop A) | M2-reblock | `vsstb`, `block_stride = M1_half*M0`, 100% BW |
| `N1 < 8`, no grouping (Prop B) | single-row | `vsstb`, 4 blocks, 50% BW |
| `M1*M0` overflows `i16` (`M > 32767`) | tile `M`, scalar base advance | `vsstb` per M-tile |

---

## 5. Open questions for discussion

- **O-NZ.1** Confirmed direction (Appendix A): **no register transpose** — every
  strategy stays on the `vsstb` scatter. Double-check there is no undiscovered cheap
  VLane/C0-block transpose primitive (the library uses `vgather2`/`vscatter`, which
  is not cheap); if one exists it could reopen the contiguous-store option.
- **O-NZ.2** `i16` `block_stride`/`repeat_stride` limits: `block_stride = M1*M0 = M`
  (C0 units) overflows at `M > 32767`. Confirm the exact field width and encode the
  "tile `M` + scalar base advance" fall-over in the strategy selector.
- **O-NZ.3** UB→L1 reassembly for the §3 M-reblock: the `M2` vecTile pieces are
  merged into one Cube-input mat tile via `M2` `pto.tinsert` calls. How much does
  that `tinsert` overhead eat into the store-BW win, and can the DMA / `tinsert`
  merge be fused so the programmer does not hand-write it?
- **O-NZ.4** Tail handling: `M` not a multiple of `M0=16`, `N` not a multiple of
  `N0`. Which blocks get masked (`vsstb` per-block predicate) and how does that
  compose with §3's packed 8-block layout?
- **O-NZ.5** Should the NZ parameters live on the store op, on the destination
  pointer type, or in a tile descriptor (cf. the `#pto.vmi.tile` presets proposed in
  the PTO-Gym exploration §1.4.1)?
- **O-NZ.6** fp32 (`N0 = 8`) and other dtypes: fp32 `N1 = N/8`, so small-N
  underutilization is even more common — does §3 generalize (`M2·N1 = 8`), and what
  about `N1` not dividing 8?
- **O-NZ.7** Interaction with `repeat_stride`: can a single `vsstb` also stride the
  `m0` rows (using `repeat_stride`) to emit more than one m0 per op, or is that a
  separate loop?
- **O-NZ.8** UB bank-conflict padding (§1 remark): is the `+1·C0` slab pad
  (`block_stride = 16·M1 + 1`) always applied, or only when the unpadded stride
  aliases a bank? Confirm the exact bank count / padding rule, and make the padded
  leading dimension part of the mat-tile descriptor so the Cube consumer agrees.

---

## Appendix A — Why a register transpose is the wrong tool (fp8, N = 256)

An earlier draft proposed, for fp8 `N = 256`, transposing an `8 × 8` C0-block tile
in registers (m-major VLs → n1-major VLs) and then doing contiguous `vsts`. **This
is not worth it**, for two concrete reasons:

1. **A5 has no cheap register block-transpose.** `vselr` is a within-/two-register
   gather (`dst[i] = src[idx[i]]`); a full `8×8` cross-register block transpose is
   an `O(N²)` sequence of `vselr` (≈ 64 lane-select ops). Worse, the library's own
   transpose helper `TTransB32*` (`pto-isa .../a5/TTrans.hpp`) does **not** even use
   `vselr` — it implements transpose with **`vgather2` / `vscatter`** (≈ 27–28
   cycles each, ~0.1 op/cyc). So *every* register-/UB-transpose primitive on A5 is
   in the expensive gather/permute class. There is no `log₂N` butterfly shortcut at
   32B-block granularity either: `vintlv`/`vdintlv` are **element-granular**
   interleaves, not VLane/C0-block moves.

2. **The store already *is* the transpose — for free.** `vsstb` scatters the
   register's m-major C0 blocks straight into their NZ outer positions
   (`block_stride = M1*M0`, §1). For fp8 `N = 256`, `N1 = 8`, so **one `vsstb` per
   m-row fills all 8 blocks at 100% store bandwidth and performs the full ND→NZ
   move** — zero register shuffles. Pre-transposing just adds `O(N²)` gather/`vselr`
   work to reproduce what the store hardware already does in one 9-cycle op.

```
fp8 N=256:  vreg (one m-row) = [C0 n1=0][C0 n1=1]...[C0 n1=7]   (8 blocks, full VL)
            vsstb block_stride=M1*M0  ──►  all 8 n1 blocks scattered into NZ
            → 100% BW, no transpose needed
```

The two worries that motivated the transpose are handled without one:

- **Contiguous vs strided store.** The marginal gain of a contiguous `vsts` over a
  full-8-block `vsstb` (both 9-cycle ops) does not pay for an `O(N²)` `vselr`/gather
  transpose. Keep the strided store.
- **`i16` `block_stride` limit.** `block_stride = M1*M0` (C0 units) `= M` rows, so it
  only overflows for `M > 32767` rows — rare. When it does, **tile `M`** and advance
  the destination base pointer per tile with *scalar* arithmetic (no field limit),
  still emitting `vsstb`. A few scalar adds, not a gather transpose.

**Bottom line:** the register `8×8` transpose is dominated in every case — by the
baseline `vsstb` (fp8 full-BW), by §3 M-reblock (bf16 small-N), or by M-tiling
(large-M stride). It is kept here only to record why it was rejected.

