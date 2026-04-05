# VPTO VISA Type Support Notes

来源：`visa.txt`

口径约定：

- 每条指令一项
- `VCVT` 作为一条合并记录，不拆成 `VCVTFI/VCVTFF/VCVTIF/VCVTII`
- 只记录 `visa.txt` 中直接出现的 type / conversion pair，不做推断

## Arithmetic / Unary / Vec-Scalar

- `VDIV`
  - 支持类型：`u16, s16, u32, s32, f16, f32`

- `VABS`
  - 支持类型：`s8, s16, s32, f16, f32`

- `VEXP`
  - 支持类型：`f16, f32`

- `VADDS`
  - 支持类型：`u8, s8, u16, s16, u32, s32, f16, f32, bf16`

- `VMULS`
  - 支持类型：`u8, s8, u16, s16, u32, s32, f16, f32, bf16`

- `VMAXS`
  - 支持类型：`u8, s8, u16, s16, u32, s32, f16, f32, bf16`

- `VMINS`
  - 支持类型：`u8, s8, u16, s16, u32, s32, f16, f32, bf16`

- `VCMPS`
  - 支持类型：`u8, s8, u16, s16, u32, s32, f16, f32, bf16`

## Conversion

- `VCVT`
  - 支持的 conversion pair：
  - `f32 -> s64`
  - `f32 -> s32`
  - `f32 -> s16`
  - `f32 -> f16`
  - `f32 -> bf16`
  - `f32 -> HiF8`
  - `f16 -> s32`
  - `f16 -> s16`
  - `f16 -> s8`
  - `f16 -> u8`
  - `f16 -> s4`
  - `f16 -> f32`
  - `f16 -> HiF8`
  - `bf16 -> s32`
  - `bf16 -> f32`
  - `u8 -> f16`
  - `u8 -> u16`
  - `u8 -> u32`
  - `s8 -> f16`
  - `s8 -> s16`
  - `s8 -> s32`
  - `u16 -> u8`
  - `u16 -> u32`
  - `s16 -> f16`
  - `s16 -> f32`
  - `s16 -> u8`
  - `s16 -> u32`
  - `s16 -> s4`
  - `s16 -> s32`
  - `u32 -> u8`
  - `u32 -> u16`
  - `u32 -> s16`
  - `s32 -> f32`
  - `s32 -> u8`
  - `s32 -> u16`
  - `s32 -> s16`
  - `s32 -> s64`
  - `s64 -> f32`
  - `s64 -> s32`
  - `s4 -> f16`
  - `s4 -> s16`
  - `HiF8 -> f32`
  - `HiF8 -> f16`
