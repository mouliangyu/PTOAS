"""TileLang DSL template for pto.trowprod"""

import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.trowprod",
    advanced=True,
)
def template_trowprod(src: pto.Tile, tmp: pto.Tile, dst: pto.Tile):
    dtype = dst.element_type
    lanes = pto.get_lanes(dtype)
    valid_rows, valid_cols = src.valid_shape

    trow_prod_loop_b16 = 7
    trow_prod_loop_b32 = 6
    if pto.constexpr(dtype == pto.f16):
        n_loop = trow_prod_loop_b16
    else:
        n_loop = trow_prod_loop_b32

    mask_1, _ = pto.make_mask(dtype, 1)

    for row in range(0, valid_rows, 1):
        remained = valid_cols

        v_acc = pto.vbr(1.0)
        v_one = pto.vbr(1.0)

        for col in range(0, valid_cols, lanes):
            mask, remained = pto.make_mask(dtype, remained)
            v_src = pto.vlds(src[row, col:])
            v_prod = pto.vmul(v_acc, v_src, mask)
            v_acc = pto.vsel(v_prod, v_acc, mask)

        reduce_mask, _ = pto.make_mask(dtype, lanes)

        for _ in range(0, n_loop, 1):
            v_intlv1, v_intlv2 = pto.vintlv(v_acc, v_one)
            v_acc = pto.vmul(v_intlv1, v_intlv2, reduce_mask)

        pto.vsts(v_acc, dst[row, 0:], mask_1)
    return
