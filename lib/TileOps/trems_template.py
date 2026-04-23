"""TileLang DSL template for pto.trems

Note: A5 hardware implements trems as:
  - float/half: dst = src - trunc(src/scalar) * scalar
  - integer: dst = src % scalar

This template uses:
  - float/half: vbr + vdiv + vtrc + vmuls + vsub
  - integer: software-expanded integer remainder sequence
Only supports tile, scalar, tmp order (matching TRemS.hpp).
"""

import sys
from pathlib import Path
import tilelang_dsl as pto


@pto.vkernel(
    target="a5",
    op="pto.trems",
    dtypes=[(pto.f32, pto.f32, pto.f32, pto.f32), (pto.f16, pto.f16, pto.f16, pto.f16),
            (pto.i32, pto.i32, pto.i32, pto.i32), (pto.i16, pto.i16, pto.i16, pto.i16)],
    advanced=True,
)
def template_trems(src: pto.Tile, scalar: pto.AnyType, tmp: pto.Tile, dst: pto.Tile):
    dtype = src.element_type
    valid_rows, valid_cols = src.valid_shape

    for row in range(0, valid_rows, 1):
        remained = valid_cols
        for col in range(0, valid_cols, pto.get_lanes(dtype)):
            chunk_remaining = remained
            mask, remained = pto.make_mask(dtype, remained)
            vec = pto.vlds(src[row, col:])
            scalar_vec = pto.vbr(scalar)
            if pto.constexpr(dtype == pto.f32 or dtype == pto.f16):
                quotient = pto.vdiv(vec, scalar_vec, mask)
                # For floating-point: trunc(quotient) towards zero (rnd="Z")
                truncated = pto.vtrc(quotient, mask, rnd="Z")
                # remainder = vec - truncated * scalar
                product = pto.vmuls(truncated, scalar, mask)
                result = pto.vsub(vec, product, mask)
            else:
                if pto.constexpr(dtype == pto.i16):
                    zero = pto.i16(0)
                    neg_one = pto.i16(-1)
                    zero_i32 = pto.i32(0)
                    one_i32 = pto.i32(1)
                    fp32_zero = pto.f32(0.0)
                    fp32_one = pto.f32(1.0)
                    false_mask = pto.pset_b32(pto.PAT.ALLF)
                    low_mask, high_remaining = pto.make_mask(pto.i32, chunk_remaining)
                    high_mask, _high_remaining = pto.make_mask(pto.i32, high_remaining)
                    zero_divisor = pto.vcmps(scalar_vec, zero, mask, pto.CmpMode.EQ)

                    x_low = pto.vsunpack(vec, pto.i32(0))
                    x_high = pto.vsunpack(vec, pto.i32(1))
                    y_low = pto.vsunpack(scalar_vec, pto.i32(0))
                    y_high = pto.vsunpack(scalar_vec, pto.i32(1))
                    active_low = pto.vcmps(y_low, zero_i32, low_mask, pto.CmpMode.NE)
                    active_high = pto.vcmps(y_high, zero_i32, high_mask, pto.CmpMode.NE)

                    abs_x_low = pto.vabs(x_low, active_low)
                    abs_x_high = pto.vabs(x_high, active_high)
                    abs_y_low = pto.vabs(y_low, active_low)
                    abs_y_high = pto.vabs(y_high, active_high)
                    x_xor_y_low = pto.vxor(x_low, y_low, active_low)
                    x_xor_y_high = pto.vxor(x_high, y_high, active_high)
                    p_pos_low = pto.vcmps(x_xor_y_low, zero_i32, active_low, pto.CmpMode.GE)
                    p_pos_high = pto.vcmps(x_xor_y_high, zero_i32, active_high, pto.CmpMode.GE)

                    y_low_float = pto.vcvt(abs_y_low, pto.f32, active_low, rnd=pto.VcvtRoundMode.R)
                    y_high_float = pto.vcvt(abs_y_high, pto.f32, active_high, rnd=pto.VcvtRoundMode.R)
                    y_low_rec = pto.vdiv(pto.vbr(fp32_one), y_low_float, active_low)
                    y_high_rec = pto.vdiv(pto.vbr(fp32_one), y_high_float, active_high)
                    low_f_z_tmp_bits = pto.vadds(pto.vbitcast(y_low_rec, pto.i32), pto.i32(0x0ffffffe), active_low)
                    high_f_z_tmp_bits = pto.vadds(pto.vbitcast(y_high_rec, pto.i32), pto.i32(0x0ffffffe), active_high)

                    low_low_mask, low_high_mask = pto.pintlv_b32(active_low, false_mask)
                    high_low_mask, high_high_mask = pto.pintlv_b32(active_high, false_mask)
                    zero_bits = pto.vbr(zero_i32)
                    low_lower_bits, low_higher_bits = pto.vintlv(low_f_z_tmp_bits, zero_bits)
                    high_lower_bits, high_higher_bits = pto.vintlv(high_f_z_tmp_bits, zero_bits)
                    low_lower_i64 = pto.vcvt(
                        pto.vbitcast(low_lower_bits, pto.f32),
                        pto.i64,
                        low_low_mask,
                        rnd=pto.VcvtRoundMode.F,
                        sat=pto.VcvtSatMode.NOSAT,
                        part=pto.VcvtPartMode.EVEN,
                    )
                    low_higher_i64 = pto.vcvt(
                        pto.vbitcast(low_higher_bits, pto.f32),
                        pto.i64,
                        low_high_mask,
                        rnd=pto.VcvtRoundMode.F,
                        sat=pto.VcvtSatMode.NOSAT,
                        part=pto.VcvtPartMode.EVEN,
                    )
                    high_lower_i64 = pto.vcvt(
                        pto.vbitcast(high_lower_bits, pto.f32),
                        pto.i64,
                        high_low_mask,
                        rnd=pto.VcvtRoundMode.F,
                        sat=pto.VcvtSatMode.NOSAT,
                        part=pto.VcvtPartMode.EVEN,
                    )
                    high_higher_i64 = pto.vcvt(
                        pto.vbitcast(high_higher_bits, pto.f32),
                        pto.i64,
                        high_high_mask,
                        rnd=pto.VcvtRoundMode.F,
                        sat=pto.VcvtSatMode.NOSAT,
                        part=pto.VcvtPartMode.EVEN,
                    )
                    z_low, _z_low_waste = pto.vdintlv(pto.vbitcast(low_lower_i64, pto.i32), pto.vbitcast(low_higher_i64, pto.i32))
                    z_high, _z_high_waste = pto.vdintlv(pto.vbitcast(high_lower_i64, pto.i32), pto.vbitcast(high_higher_i64, pto.i32))

                    f_z_tmp_low = pto.vbitcast(low_f_z_tmp_bits, pto.f32)
                    f_z_tmp_high = pto.vbitcast(high_f_z_tmp_bits, pto.f32)
                    fz_negative_low = pto.vcmps(f_z_tmp_low, fp32_zero, active_low, pto.CmpMode.LT)
                    fz_negative_high = pto.vcmps(f_z_tmp_high, fp32_zero, active_high, pto.CmpMode.LT)
                    z_low = pto.vsel(zero_bits, z_low, fz_negative_low)
                    z_high = pto.vsel(zero_bits, z_high, fz_negative_high)

                    tmp0_low = pto.vmul(z_low, abs_y_low, active_low)
                    tmp0_high = pto.vmul(z_high, abs_y_high, active_high)
                    tmp0_low = pto.vneg(tmp0_low, active_low)
                    tmp0_high = pto.vneg(tmp0_high, active_high)
                    _z_low_lo, z_low_hi = pto.vmull(z_low, tmp0_low, active_low)
                    _z_high_lo, z_high_hi = pto.vmull(z_high, tmp0_high, active_high)
                    z_low = pto.vadd(z_low, z_low_hi, active_low)
                    z_high = pto.vadd(z_high, z_high_hi, active_high)

                    _q_low_lo, q_tmp_low = pto.vmull(abs_x_low, z_low, active_low)
                    _q_high_lo, q_tmp_high = pto.vmull(abs_x_high, z_high, active_high)
                    yq_tmp_low = pto.vmul(q_tmp_low, abs_y_low, active_low)
                    yq_tmp_high = pto.vmul(q_tmp_high, abs_y_high, active_high)
                    r_tmp_low = pto.vsub(abs_x_low, yq_tmp_low, active_low)
                    r_tmp_high = pto.vsub(abs_x_high, yq_tmp_high, active_high)

                    ge_mask_low = pto.vcmp(r_tmp_low, abs_y_low, active_low, pto.CmpMode.GE)
                    ge_mask_high = pto.vcmp(r_tmp_high, abs_y_high, active_high, pto.CmpMode.GE)
                    refined_r_low = pto.vsub(r_tmp_low, abs_y_low, active_low)
                    refined_r_high = pto.vsub(r_tmp_high, abs_y_high, active_high)
                    r_tmp_low = pto.vsel(refined_r_low, r_tmp_low, ge_mask_low)
                    r_tmp_high = pto.vsel(refined_r_high, r_tmp_high, ge_mask_high)
                    q_inc_low = pto.vadds(q_tmp_low, one_i32, active_low)
                    q_inc_high = pto.vadds(q_tmp_high, one_i32, active_high)
                    q_tmp_low = pto.vsel(q_inc_low, q_tmp_low, ge_mask_low)
                    q_tmp_high = pto.vsel(q_inc_high, q_tmp_high, ge_mask_high)

                    ge_mask_low = pto.vcmp(r_tmp_low, abs_y_low, active_low, pto.CmpMode.GE)
                    ge_mask_high = pto.vcmp(r_tmp_high, abs_y_high, active_high, pto.CmpMode.GE)
                    refined_r_low = pto.vsub(r_tmp_low, abs_y_low, active_low)
                    refined_r_high = pto.vsub(r_tmp_high, abs_y_high, active_high)
                    r_tmp_low = pto.vsel(refined_r_low, r_tmp_low, ge_mask_low)
                    r_tmp_high = pto.vsel(refined_r_high, r_tmp_high, ge_mask_high)
                    q_inc_low = pto.vadds(q_tmp_low, one_i32, active_low)
                    q_inc_high = pto.vadds(q_tmp_high, one_i32, active_high)
                    q_tmp_low = pto.vsel(q_inc_low, q_tmp_low, ge_mask_low)
                    q_tmp_high = pto.vsel(q_inc_high, q_tmp_high, ge_mask_high)

                    neg_q_low = pto.vneg(q_tmp_low, active_low)
                    neg_q_high = pto.vneg(q_tmp_high, active_high)
                    q_low = pto.vsel(q_tmp_low, neg_q_low, p_pos_low)
                    q_high = pto.vsel(q_tmp_high, neg_q_high, p_pos_high)

                    qy_low = pto.vmul(q_low, y_low, active_low)
                    qy_high = pto.vmul(q_high, y_high, active_high)
                    remainder_low = pto.vsub(x_low, qy_low, active_low)
                    remainder_high = pto.vsub(x_high, qy_high, active_high)

                    nonzero_remainder_low = pto.vcmps(r_tmp_low, zero_i32, active_low, pto.CmpMode.NE)
                    nonzero_remainder_high = pto.vcmps(r_tmp_high, zero_i32, active_high, pto.CmpMode.NE)
                    nonnegative_dividend_low = pto.vcmps(x_low, zero_i32, active_low, pto.CmpMode.GE)
                    nonnegative_dividend_high = pto.vcmps(x_high, zero_i32, active_high, pto.CmpMode.GE)
                    nonnegative_divisor_low = pto.vcmps(y_low, zero_i32, active_low, pto.CmpMode.GE)
                    nonnegative_divisor_high = pto.vcmps(y_high, zero_i32, active_high, pto.CmpMode.GE)
                    sign_diff_low = pto.pxor(nonnegative_dividend_low, nonnegative_divisor_low, active_low)
                    sign_diff_high = pto.pxor(nonnegative_dividend_high, nonnegative_divisor_high, active_high)
                    need_floor_fix_low = pto.pand(sign_diff_low, nonzero_remainder_low, active_low)
                    need_floor_fix_high = pto.pand(sign_diff_high, nonzero_remainder_high, active_high)
                    amended_remainder_low = pto.vadd(y_low, remainder_low, active_low)
                    amended_remainder_high = pto.vadd(y_high, remainder_high, active_high)
                    remainder_low = pto.vsel(amended_remainder_low, remainder_low, need_floor_fix_low)
                    remainder_high = pto.vsel(amended_remainder_high, remainder_high, need_floor_fix_high)

                    packed_low = pto.vpack(remainder_low, pto.PredicatePart.LOWER)
                    packed_high = pto.vpack(remainder_high, pto.PredicatePart.HIGHER)
                    remainder = pto.vbitcast(pto.vor(packed_low, packed_high, mask), pto.i16)
                    result = pto.vsel(pto.vbr(neg_one), remainder, zero_divisor)
                else:
                    zero = pto.i32(0)
                    one = pto.i32(1)
                    neg_one = pto.i32(-1)
                    fp32_zero = pto.f32(0.0)
                    fp32_one = pto.f32(1.0)
                    false_mask = pto.pset_b32(pto.PAT.ALLF)

                    zero_divisor = pto.vcmps(scalar_vec, zero, mask, pto.CmpMode.EQ)
                    active_mask = pto.pnot(zero_divisor, mask)

                    abs_x = pto.vabs(vec, active_mask)
                    abs_y = pto.vabs(scalar_vec, active_mask)
                    x_xor_y = pto.vxor(vec, scalar_vec, active_mask)
                    p_pos = pto.vcmps(x_xor_y, zero, active_mask, pto.CmpMode.GE)

                    y_float = pto.vcvt(abs_y, pto.f32, active_mask, rnd=pto.VcvtRoundMode.R)
                    y_rec = pto.vdiv(pto.vbr(fp32_one), y_float, active_mask)
                    y_rec_bits = pto.vbitcast(y_rec, pto.i32)
                    f_z_tmp_bits = pto.vadds(y_rec_bits, pto.i32(0x0ffffffe), active_mask)

                    low_mask, high_mask = pto.pintlv_b32(active_mask, false_mask)
                    zero_bits = pto.vbr(zero)
                    lower_bits, higher_bits = pto.vintlv(f_z_tmp_bits, zero_bits)
                    lower_f32 = pto.vbitcast(lower_bits, pto.f32)
                    higher_f32 = pto.vbitcast(higher_bits, pto.f32)
                    lower_i64 = pto.vcvt(
                        lower_f32,
                        pto.i64,
                        low_mask,
                        rnd=pto.VcvtRoundMode.F,
                        sat=pto.VcvtSatMode.NOSAT,
                        part=pto.VcvtPartMode.EVEN,
                    )
                    higher_i64 = pto.vcvt(
                        higher_f32,
                        pto.i64,
                        high_mask,
                        rnd=pto.VcvtRoundMode.F,
                        sat=pto.VcvtSatMode.NOSAT,
                        part=pto.VcvtPartMode.EVEN,
                    )
                    lower_i32 = pto.vbitcast(lower_i64, pto.i32)
                    higher_i32 = pto.vbitcast(higher_i64, pto.i32)
                    z, _z_waste = pto.vdintlv(lower_i32, higher_i32)

                    f_z_tmp = pto.vbitcast(f_z_tmp_bits, pto.f32)
                    fz_negative = pto.vcmps(f_z_tmp, fp32_zero, active_mask, pto.CmpMode.LT)
                    z = pto.vsel(zero_bits, z, fz_negative)

                    tmp_0 = pto.vmul(z, abs_y, active_mask)
                    tmp_0 = pto.vneg(tmp_0, active_mask)
                    _z_low, z_high = pto.vmull(z, tmp_0, active_mask)
                    z = pto.vadd(z, z_high, active_mask)

                    _q_low, q_tmp = pto.vmull(abs_x, z, active_mask)
                    yq_tmp = pto.vmul(q_tmp, abs_y, active_mask)
                    r_tmp = pto.vsub(abs_x, yq_tmp, active_mask)

                    ge_mask = pto.vcmp(r_tmp, abs_y, active_mask, pto.CmpMode.GE)
                    refined_r = pto.vsub(r_tmp, abs_y, active_mask)
                    r_tmp = pto.vsel(refined_r, r_tmp, ge_mask)
                    q_inc = pto.vadds(q_tmp, one, active_mask)
                    q_tmp = pto.vsel(q_inc, q_tmp, ge_mask)

                    ge_mask = pto.vcmp(r_tmp, abs_y, active_mask, pto.CmpMode.GE)
                    refined_r = pto.vsub(r_tmp, abs_y, active_mask)
                    r_tmp = pto.vsel(refined_r, r_tmp, ge_mask)
                    q_inc = pto.vadds(q_tmp, one, active_mask)
                    q_tmp = pto.vsel(q_inc, q_tmp, ge_mask)

                    neg_q = pto.vneg(q_tmp, active_mask)
                    q = pto.vsel(q_tmp, neg_q, p_pos)

                    qy = pto.vmul(q, scalar_vec, active_mask)
                    remainder = pto.vsub(vec, qy, active_mask)

                    nonzero_remainder = pto.vcmps(r_tmp, zero, active_mask, pto.CmpMode.NE)
                    nonnegative_dividend = pto.vcmps(vec, zero, active_mask, pto.CmpMode.GE)
                    nonnegative_divisor = pto.vcmps(scalar_vec, zero, active_mask, pto.CmpMode.GE)
                    sign_diff = pto.pxor(nonnegative_dividend, nonnegative_divisor, active_mask)
                    need_floor_fix = pto.pand(sign_diff, nonzero_remainder, active_mask)
                    amended_remainder = pto.vadd(scalar_vec, remainder, active_mask)
                    remainder = pto.vsel(amended_remainder, remainder, need_floor_fix)
                    result = pto.vsel(pto.vbr(neg_one), remainder, zero_divisor)
            pto.vsts(result, dst[row, col:], mask)
    return
