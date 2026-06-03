/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Enum Utility Definitions                                                   *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|* From: PTOOps.td                                                            *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

namespace mlir {
namespace pto {
::llvm::StringRef stringifyAccStoreAtomicOp(AccStoreAtomicOp val) {
  switch (val) {
    case AccStoreAtomicOp::Add: return "add";
    case AccStoreAtomicOp::Max: return "max";
    case AccStoreAtomicOp::Min: return "min";
  }
  return "";
}

::std::optional<AccStoreAtomicOp> symbolizeAccStoreAtomicOp(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<AccStoreAtomicOp>>(str)
      .Case("add", AccStoreAtomicOp::Add)
      .Case("max", AccStoreAtomicOp::Max)
      .Case("min", AccStoreAtomicOp::Min)
      .Default(::std::nullopt);
}
::std::optional<AccStoreAtomicOp> symbolizeAccStoreAtomicOp(uint32_t value) {
  switch (value) {
  case 0: return AccStoreAtomicOp::Add;
  case 1: return AccStoreAtomicOp::Max;
  case 2: return AccStoreAtomicOp::Min;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyAccStoreAtomicType(AccStoreAtomicType val) {
  switch (val) {
    case AccStoreAtomicType::F32: return "f32";
    case AccStoreAtomicType::F16: return "f16";
    case AccStoreAtomicType::S16: return "s16";
    case AccStoreAtomicType::S32: return "s32";
    case AccStoreAtomicType::S8: return "s8";
    case AccStoreAtomicType::BF16: return "bf16";
  }
  return "";
}

::std::optional<AccStoreAtomicType> symbolizeAccStoreAtomicType(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<AccStoreAtomicType>>(str)
      .Case("f32", AccStoreAtomicType::F32)
      .Case("f16", AccStoreAtomicType::F16)
      .Case("s16", AccStoreAtomicType::S16)
      .Case("s32", AccStoreAtomicType::S32)
      .Case("s8", AccStoreAtomicType::S8)
      .Case("bf16", AccStoreAtomicType::BF16)
      .Default(::std::nullopt);
}
::std::optional<AccStoreAtomicType> symbolizeAccStoreAtomicType(uint32_t value) {
  switch (value) {
  case 1: return AccStoreAtomicType::F32;
  case 2: return AccStoreAtomicType::F16;
  case 3: return AccStoreAtomicType::S16;
  case 4: return AccStoreAtomicType::S32;
  case 5: return AccStoreAtomicType::S8;
  case 6: return AccStoreAtomicType::BF16;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyAccStoreMode(AccStoreMode val) {
  switch (val) {
    case AccStoreMode::Nz2nd: return "nz2nd";
    case AccStoreMode::Nz2dn: return "nz2dn";
    case AccStoreMode::Nz2nz: return "nz2nz";
  }
  return "";
}

::std::optional<AccStoreMode> symbolizeAccStoreMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<AccStoreMode>>(str)
      .Case("nz2nd", AccStoreMode::Nz2nd)
      .Case("nz2dn", AccStoreMode::Nz2dn)
      .Case("nz2nz", AccStoreMode::Nz2nz)
      .Default(::std::nullopt);
}
::std::optional<AccStoreMode> symbolizeAccStoreMode(uint32_t value) {
  switch (value) {
  case 0: return AccStoreMode::Nz2nd;
  case 1: return AccStoreMode::Nz2dn;
  case 2: return AccStoreMode::Nz2nz;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyAccStoreQuantPreMode(AccStoreQuantPreMode val) {
  switch (val) {
    case AccStoreQuantPreMode::NoConvert: return "no_convert";
    case AccStoreQuantPreMode::F32F16: return "f32_f16";
    case AccStoreQuantPreMode::QF322HIF8PreVec: return "qf322hif8_pre_vec";
    case AccStoreQuantPreMode::QF322HIF8PreScalar: return "qf322hif8_pre_scalar";
    case AccStoreQuantPreMode::QF322HIF8PreHybridVec: return "qf322hif8_pre_hybrid_vec";
    case AccStoreQuantPreMode::QF322HIF8PreHybridScalar: return "qf322hif8_pre_hybrid_scalar";
    case AccStoreQuantPreMode::DEQS32IntVec: return "deqs32_int_vec";
    case AccStoreQuantPreMode::DEQS32IntScalar: return "deqs32_int_scalar";
    case AccStoreQuantPreMode::REQ8Vec: return "req8_vec";
    case AccStoreQuantPreMode::REQ8Scalar: return "req8_scalar";
    case AccStoreQuantPreMode::DEQF16Vec: return "deqf16_vec";
    case AccStoreQuantPreMode::DEQF16Scalar: return "deqf16_scalar";
    case AccStoreQuantPreMode::QF322FP8PreVec: return "qf322fp8_pre_vec";
    case AccStoreQuantPreMode::QF322FP8PreScalar: return "qf322fp8_pre_scalar";
    case AccStoreQuantPreMode::QF322F32PreVec: return "qf322f32_pre_vec";
    case AccStoreQuantPreMode::QF322F32PreScalar: return "qf322f32_pre_scalar";
    case AccStoreQuantPreMode::F32BF16: return "f32_bf16";
    case AccStoreQuantPreMode::QF162B8PreVec: return "qf162b8_pre_vec";
    case AccStoreQuantPreMode::QF162B8PreScalar: return "qf162b8_pre_scalar";
    case AccStoreQuantPreMode::QF162S4PreVec: return "qf162s4_pre_vec";
    case AccStoreQuantPreMode::QF162S4PreScalar: return "qf162s4_pre_scalar";
    case AccStoreQuantPreMode::REQ4Vec: return "req4_vec";
    case AccStoreQuantPreMode::REQ4Scalar: return "req4_scalar";
    case AccStoreQuantPreMode::QF322B8PreVec: return "qf322b8_pre_vec";
    case AccStoreQuantPreMode::QF322B8PreScalar: return "qf322b8_pre_scalar";
    case AccStoreQuantPreMode::QF322S4PreVec: return "qf322s4_pre_vec";
    case AccStoreQuantPreMode::QF322S4PreScalar: return "qf322s4_pre_scalar";
    case AccStoreQuantPreMode::DEQS16Vec: return "deqs16_vec";
    case AccStoreQuantPreMode::DEQS16Scalar: return "deqs16_scalar";
    case AccStoreQuantPreMode::QF162S16PreVec: return "qf162s16_pre_vec";
    case AccStoreQuantPreMode::QF162S16PreScalar: return "qf162s16_pre_scalar";
    case AccStoreQuantPreMode::QF322F16PreVec: return "qf322f16_pre_vec";
    case AccStoreQuantPreMode::QF322F16PreScalar: return "qf322f16_pre_scalar";
    case AccStoreQuantPreMode::QF322BF16PreVec: return "qf322bf16_pre_vec";
    case AccStoreQuantPreMode::QF322BF16PreScalar: return "qf322bf16_pre_scalar";
    case AccStoreQuantPreMode::QS322BF16PreVec: return "qs322bf16_pre_vec";
    case AccStoreQuantPreMode::QS322BF16PreScalar: return "qs322bf16_pre_scalar";
  }
  return "";
}

::std::optional<AccStoreQuantPreMode> symbolizeAccStoreQuantPreMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<AccStoreQuantPreMode>>(str)
      .Case("no_convert", AccStoreQuantPreMode::NoConvert)
      .Case("f32_f16", AccStoreQuantPreMode::F32F16)
      .Case("qf322hif8_pre_vec", AccStoreQuantPreMode::QF322HIF8PreVec)
      .Case("qf322hif8_pre_scalar", AccStoreQuantPreMode::QF322HIF8PreScalar)
      .Case("qf322hif8_pre_hybrid_vec", AccStoreQuantPreMode::QF322HIF8PreHybridVec)
      .Case("qf322hif8_pre_hybrid_scalar", AccStoreQuantPreMode::QF322HIF8PreHybridScalar)
      .Case("deqs32_int_vec", AccStoreQuantPreMode::DEQS32IntVec)
      .Case("deqs32_int_scalar", AccStoreQuantPreMode::DEQS32IntScalar)
      .Case("req8_vec", AccStoreQuantPreMode::REQ8Vec)
      .Case("req8_scalar", AccStoreQuantPreMode::REQ8Scalar)
      .Case("deqf16_vec", AccStoreQuantPreMode::DEQF16Vec)
      .Case("deqf16_scalar", AccStoreQuantPreMode::DEQF16Scalar)
      .Case("qf322fp8_pre_vec", AccStoreQuantPreMode::QF322FP8PreVec)
      .Case("qf322fp8_pre_scalar", AccStoreQuantPreMode::QF322FP8PreScalar)
      .Case("qf322f32_pre_vec", AccStoreQuantPreMode::QF322F32PreVec)
      .Case("qf322f32_pre_scalar", AccStoreQuantPreMode::QF322F32PreScalar)
      .Case("f32_bf16", AccStoreQuantPreMode::F32BF16)
      .Case("qf162b8_pre_vec", AccStoreQuantPreMode::QF162B8PreVec)
      .Case("qf162b8_pre_scalar", AccStoreQuantPreMode::QF162B8PreScalar)
      .Case("qf162s4_pre_vec", AccStoreQuantPreMode::QF162S4PreVec)
      .Case("qf162s4_pre_scalar", AccStoreQuantPreMode::QF162S4PreScalar)
      .Case("req4_vec", AccStoreQuantPreMode::REQ4Vec)
      .Case("req4_scalar", AccStoreQuantPreMode::REQ4Scalar)
      .Case("qf322b8_pre_vec", AccStoreQuantPreMode::QF322B8PreVec)
      .Case("qf322b8_pre_scalar", AccStoreQuantPreMode::QF322B8PreScalar)
      .Case("qf322s4_pre_vec", AccStoreQuantPreMode::QF322S4PreVec)
      .Case("qf322s4_pre_scalar", AccStoreQuantPreMode::QF322S4PreScalar)
      .Case("deqs16_vec", AccStoreQuantPreMode::DEQS16Vec)
      .Case("deqs16_scalar", AccStoreQuantPreMode::DEQS16Scalar)
      .Case("qf162s16_pre_vec", AccStoreQuantPreMode::QF162S16PreVec)
      .Case("qf162s16_pre_scalar", AccStoreQuantPreMode::QF162S16PreScalar)
      .Case("qf322f16_pre_vec", AccStoreQuantPreMode::QF322F16PreVec)
      .Case("qf322f16_pre_scalar", AccStoreQuantPreMode::QF322F16PreScalar)
      .Case("qf322bf16_pre_vec", AccStoreQuantPreMode::QF322BF16PreVec)
      .Case("qf322bf16_pre_scalar", AccStoreQuantPreMode::QF322BF16PreScalar)
      .Case("qs322bf16_pre_vec", AccStoreQuantPreMode::QS322BF16PreVec)
      .Case("qs322bf16_pre_scalar", AccStoreQuantPreMode::QS322BF16PreScalar)
      .Default(::std::nullopt);
}
::std::optional<AccStoreQuantPreMode> symbolizeAccStoreQuantPreMode(uint32_t value) {
  switch (value) {
  case 0: return AccStoreQuantPreMode::NoConvert;
  case 1: return AccStoreQuantPreMode::F32F16;
  case 2: return AccStoreQuantPreMode::QF322HIF8PreVec;
  case 3: return AccStoreQuantPreMode::QF322HIF8PreScalar;
  case 4: return AccStoreQuantPreMode::QF322HIF8PreHybridVec;
  case 5: return AccStoreQuantPreMode::QF322HIF8PreHybridScalar;
  case 6: return AccStoreQuantPreMode::DEQS32IntVec;
  case 7: return AccStoreQuantPreMode::DEQS32IntScalar;
  case 8: return AccStoreQuantPreMode::REQ8Vec;
  case 9: return AccStoreQuantPreMode::REQ8Scalar;
  case 10: return AccStoreQuantPreMode::DEQF16Vec;
  case 11: return AccStoreQuantPreMode::DEQF16Scalar;
  case 12: return AccStoreQuantPreMode::QF322FP8PreVec;
  case 13: return AccStoreQuantPreMode::QF322FP8PreScalar;
  case 14: return AccStoreQuantPreMode::QF322F32PreVec;
  case 15: return AccStoreQuantPreMode::QF322F32PreScalar;
  case 16: return AccStoreQuantPreMode::F32BF16;
  case 17: return AccStoreQuantPreMode::QF162B8PreVec;
  case 18: return AccStoreQuantPreMode::QF162B8PreScalar;
  case 19: return AccStoreQuantPreMode::QF162S4PreVec;
  case 20: return AccStoreQuantPreMode::QF162S4PreScalar;
  case 21: return AccStoreQuantPreMode::REQ4Vec;
  case 22: return AccStoreQuantPreMode::REQ4Scalar;
  case 23: return AccStoreQuantPreMode::QF322B8PreVec;
  case 24: return AccStoreQuantPreMode::QF322B8PreScalar;
  case 25: return AccStoreQuantPreMode::QF322S4PreVec;
  case 26: return AccStoreQuantPreMode::QF322S4PreScalar;
  case 27: return AccStoreQuantPreMode::DEQS16Vec;
  case 28: return AccStoreQuantPreMode::DEQS16Scalar;
  case 29: return AccStoreQuantPreMode::QF162S16PreVec;
  case 30: return AccStoreQuantPreMode::QF162S16PreScalar;
  case 31: return AccStoreQuantPreMode::QF322F16PreVec;
  case 32: return AccStoreQuantPreMode::QF322F16PreScalar;
  case 33: return AccStoreQuantPreMode::QF322BF16PreVec;
  case 34: return AccStoreQuantPreMode::QF322BF16PreScalar;
  case 35: return AccStoreQuantPreMode::QS322BF16PreVec;
  case 36: return AccStoreQuantPreMode::QS322BF16PreScalar;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyAccStoreSatMode(AccStoreSatMode val) {
  switch (val) {
    case AccStoreSatMode::Sat: return "sat";
    case AccStoreSatMode::NoSat: return "nosat";
    case AccStoreSatMode::SatPreserveNan: return "sat_preserve_nan";
  }
  return "";
}

::std::optional<AccStoreSatMode> symbolizeAccStoreSatMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<AccStoreSatMode>>(str)
      .Case("sat", AccStoreSatMode::Sat)
      .Case("nosat", AccStoreSatMode::NoSat)
      .Case("sat_preserve_nan", AccStoreSatMode::SatPreserveNan)
      .Default(::std::nullopt);
}
::std::optional<AccStoreSatMode> symbolizeAccStoreSatMode(uint32_t value) {
  switch (value) {
  case 0: return AccStoreSatMode::Sat;
  case 1: return AccStoreSatMode::NoSat;
  case 2: return AccStoreSatMode::SatPreserveNan;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyAccStoreUbDstMode(AccStoreUbDstMode val) {
  switch (val) {
    case AccStoreUbDstMode::Single: return "single";
    case AccStoreUbDstMode::SplitM: return "split_m";
    case AccStoreUbDstMode::SplitN: return "split_n";
  }
  return "";
}

::std::optional<AccStoreUbDstMode> symbolizeAccStoreUbDstMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<AccStoreUbDstMode>>(str)
      .Case("single", AccStoreUbDstMode::Single)
      .Case("split_m", AccStoreUbDstMode::SplitM)
      .Case("split_n", AccStoreUbDstMode::SplitN)
      .Default(::std::nullopt);
}
::std::optional<AccStoreUbDstMode> symbolizeAccStoreUbDstMode(uint32_t value) {
  switch (value) {
  case 0: return AccStoreUbDstMode::Single;
  case 1: return AccStoreUbDstMode::SplitM;
  case 2: return AccStoreUbDstMode::SplitN;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyAccToVecMode(AccToVecMode val) {
  switch (val) {
    case AccToVecMode::SingleModeVec0: return "single_mode_vec0";
    case AccToVecMode::SingleModeVec1: return "single_mode_vec1";
    case AccToVecMode::DualModeSplitM: return "dual_mode_split_m";
    case AccToVecMode::DualModeSplitN: return "dual_mode_split_n";
  }
  return "";
}

::std::optional<AccToVecMode> symbolizeAccToVecMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<AccToVecMode>>(str)
      .Case("single_mode_vec0", AccToVecMode::SingleModeVec0)
      .Case("single_mode_vec1", AccToVecMode::SingleModeVec1)
      .Case("dual_mode_split_m", AccToVecMode::DualModeSplitM)
      .Case("dual_mode_split_n", AccToVecMode::DualModeSplitN)
      .Default(::std::nullopt);
}
::std::optional<AccToVecMode> symbolizeAccToVecMode(uint32_t value) {
  switch (value) {
  case 0: return AccToVecMode::SingleModeVec0;
  case 1: return AccToVecMode::SingleModeVec1;
  case 2: return AccToVecMode::DualModeSplitM;
  case 3: return AccToVecMode::DualModeSplitN;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyAddressSpace(AddressSpace val) {
  switch (val) {
    case AddressSpace::Zero: return "zero";
    case AddressSpace::GM: return "gm";
    case AddressSpace::MAT: return "mat";
    case AddressSpace::LEFT: return "left";
    case AddressSpace::RIGHT: return "right";
    case AddressSpace::ACC: return "acc";
    case AddressSpace::VEC: return "vec";
    case AddressSpace::BIAS: return "bias";
    case AddressSpace::SCALING: return "scaling";
  }
  return "";
}

::std::optional<AddressSpace> symbolizeAddressSpace(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<AddressSpace>>(str)
      .Case("zero", AddressSpace::Zero)
      .Case("gm", AddressSpace::GM)
      .Case("mat", AddressSpace::MAT)
      .Case("left", AddressSpace::LEFT)
      .Case("right", AddressSpace::RIGHT)
      .Case("acc", AddressSpace::ACC)
      .Case("vec", AddressSpace::VEC)
      .Case("bias", AddressSpace::BIAS)
      .Case("scaling", AddressSpace::SCALING)
      .Default(::std::nullopt);
}
::std::optional<AddressSpace> symbolizeAddressSpace(uint32_t value) {
  switch (value) {
  case 0: return AddressSpace::Zero;
  case 1: return AddressSpace::GM;
  case 2: return AddressSpace::MAT;
  case 3: return AddressSpace::LEFT;
  case 4: return AddressSpace::RIGHT;
  case 5: return AddressSpace::ACC;
  case 6: return AddressSpace::VEC;
  case 7: return AddressSpace::BIAS;
  case 8: return AddressSpace::SCALING;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyAtomicType(AtomicType val) {
  switch (val) {
    case AtomicType::AtomicNone: return "atomic_none";
    case AtomicType::AtomicAdd: return "atomic_add";
  }
  return "";
}

::std::optional<AtomicType> symbolizeAtomicType(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<AtomicType>>(str)
      .Case("atomic_none", AtomicType::AtomicNone)
      .Case("atomic_add", AtomicType::AtomicAdd)
      .Default(::std::nullopt);
}
::std::optional<AtomicType> symbolizeAtomicType(uint32_t value) {
  switch (value) {
  case 0: return AtomicType::AtomicNone;
  case 1: return AtomicType::AtomicAdd;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyBLayout(BLayout val) {
  switch (val) {
    case BLayout::RowMajor: return "row_major";
    case BLayout::ColMajor: return "col_major";
  }
  return "";
}

::std::optional<BLayout> symbolizeBLayout(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<BLayout>>(str)
      .Case("row_major", BLayout::RowMajor)
      .Case("col_major", BLayout::ColMajor)
      .Default(::std::nullopt);
}
::std::optional<BLayout> symbolizeBLayout(uint32_t value) {
  switch (value) {
  case 0: return BLayout::RowMajor;
  case 1: return BLayout::ColMajor;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyCmpMode(CmpMode val) {
  switch (val) {
    case CmpMode::EQ: return "eq";
    case CmpMode::NE: return "ne";
    case CmpMode::LT: return "lt";
    case CmpMode::LE: return "le";
    case CmpMode::GT: return "gt";
    case CmpMode::GE: return "ge";
  }
  return "";
}

::std::optional<CmpMode> symbolizeCmpMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<CmpMode>>(str)
      .Case("eq", CmpMode::EQ)
      .Case("ne", CmpMode::NE)
      .Case("lt", CmpMode::LT)
      .Case("le", CmpMode::LE)
      .Case("gt", CmpMode::GT)
      .Case("ge", CmpMode::GE)
      .Default(::std::nullopt);
}
::std::optional<CmpMode> symbolizeCmpMode(uint32_t value) {
  switch (value) {
  case 0: return CmpMode::EQ;
  case 1: return CmpMode::NE;
  case 2: return CmpMode::LT;
  case 3: return CmpMode::LE;
  case 4: return CmpMode::GT;
  case 5: return CmpMode::GE;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyCompactMode(CompactMode val) {
  switch (val) {
    case CompactMode::Null: return "null";
    case CompactMode::Normal: return "normal";
    case CompactMode::RowPlusOne: return "row_plus_one";
  }
  return "";
}

::std::optional<CompactMode> symbolizeCompactMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<CompactMode>>(str)
      .Case("null", CompactMode::Null)
      .Case("normal", CompactMode::Normal)
      .Case("row_plus_one", CompactMode::RowPlusOne)
      .Default(::std::nullopt);
}
::std::optional<CompactMode> symbolizeCompactMode(uint32_t value) {
  switch (value) {
  case 0: return CompactMode::Null;
  case 1: return CompactMode::Normal;
  case 2: return CompactMode::RowPlusOne;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyCubeLoadFracMode(CubeLoadFracMode val) {
  switch (val) {
    case CubeLoadFracMode::Nd2nz: return "nd2nz";
    case CubeLoadFracMode::Dn2nz: return "dn2nz";
  }
  return "";
}

::std::optional<CubeLoadFracMode> symbolizeCubeLoadFracMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<CubeLoadFracMode>>(str)
      .Case("nd2nz", CubeLoadFracMode::Nd2nz)
      .Case("dn2nz", CubeLoadFracMode::Dn2nz)
      .Default(::std::nullopt);
}
::std::optional<CubeLoadFracMode> symbolizeCubeLoadFracMode(uint32_t value) {
  switch (value) {
  case 0: return CubeLoadFracMode::Nd2nz;
  case 1: return CubeLoadFracMode::Dn2nz;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyEVENT(EVENT val) {
  switch (val) {
    case EVENT::EVENT_ID0: return "EVENT_ID0";
    case EVENT::EVENT_ID1: return "EVENT_ID1";
    case EVENT::EVENT_ID2: return "EVENT_ID2";
    case EVENT::EVENT_ID3: return "EVENT_ID3";
    case EVENT::EVENT_ID4: return "EVENT_ID4";
    case EVENT::EVENT_ID5: return "EVENT_ID5";
    case EVENT::EVENT_ID6: return "EVENT_ID6";
    case EVENT::EVENT_ID7: return "EVENT_ID7";
  }
  return "";
}

::std::optional<EVENT> symbolizeEVENT(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<EVENT>>(str)
      .Case("EVENT_ID0", EVENT::EVENT_ID0)
      .Case("EVENT_ID1", EVENT::EVENT_ID1)
      .Case("EVENT_ID2", EVENT::EVENT_ID2)
      .Case("EVENT_ID3", EVENT::EVENT_ID3)
      .Case("EVENT_ID4", EVENT::EVENT_ID4)
      .Case("EVENT_ID5", EVENT::EVENT_ID5)
      .Case("EVENT_ID6", EVENT::EVENT_ID6)
      .Case("EVENT_ID7", EVENT::EVENT_ID7)
      .Default(::std::nullopt);
}
::std::optional<EVENT> symbolizeEVENT(uint32_t value) {
  switch (value) {
  case 0: return EVENT::EVENT_ID0;
  case 1: return EVENT::EVENT_ID1;
  case 2: return EVENT::EVENT_ID2;
  case 3: return EVENT::EVENT_ID3;
  case 4: return EVENT::EVENT_ID4;
  case 5: return EVENT::EVENT_ID5;
  case 6: return EVENT::EVENT_ID6;
  case 7: return EVENT::EVENT_ID7;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyFunctionKernelKind(FunctionKernelKind val) {
  switch (val) {
    case FunctionKernelKind::Cube: return "cube";
    case FunctionKernelKind::Vector: return "vector";
  }
  return "";
}

::std::optional<FunctionKernelKind> symbolizeFunctionKernelKind(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<FunctionKernelKind>>(str)
      .Case("cube", FunctionKernelKind::Cube)
      .Case("vector", FunctionKernelKind::Vector)
      .Default(::std::nullopt);
}
::std::optional<FunctionKernelKind> symbolizeFunctionKernelKind(uint32_t value) {
  switch (value) {
  case 0: return FunctionKernelKind::Cube;
  case 1: return FunctionKernelKind::Vector;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyGatherOOB(GatherOOB val) {
  switch (val) {
    case GatherOOB::Undefined: return "undefined";
    case GatherOOB::Clamp: return "clamp";
    case GatherOOB::Wrap: return "wrap";
    case GatherOOB::Zero: return "zero";
  }
  return "";
}

::std::optional<GatherOOB> symbolizeGatherOOB(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<GatherOOB>>(str)
      .Case("undefined", GatherOOB::Undefined)
      .Case("clamp", GatherOOB::Clamp)
      .Case("wrap", GatherOOB::Wrap)
      .Case("zero", GatherOOB::Zero)
      .Default(::std::nullopt);
}
::std::optional<GatherOOB> symbolizeGatherOOB(uint32_t value) {
  switch (value) {
  case 0: return GatherOOB::Undefined;
  case 1: return GatherOOB::Clamp;
  case 2: return GatherOOB::Wrap;
  case 3: return GatherOOB::Zero;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyL1Cache(L1Cache val) {
  switch (val) {
    case L1Cache::Uncache: return "uncache";
    case L1Cache::Cache: return "cache";
  }
  return "";
}

::std::optional<L1Cache> symbolizeL1Cache(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<L1Cache>>(str)
      .Case("uncache", L1Cache::Uncache)
      .Case("cache", L1Cache::Cache)
      .Default(::std::nullopt);
}
::std::optional<L1Cache> symbolizeL1Cache(uint32_t value) {
  switch (value) {
  case 0: return L1Cache::Uncache;
  case 1: return L1Cache::Cache;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyLayout(Layout val) {
  switch (val) {
    case Layout::ND: return "nd";
    case Layout::DN: return "dn";
    case Layout::NZ: return "nz";
  }
  return "";
}

::std::optional<Layout> symbolizeLayout(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<Layout>>(str)
      .Case("nd", Layout::ND)
      .Case("dn", Layout::DN)
      .Case("nz", Layout::NZ)
      .Default(::std::nullopt);
}
::std::optional<Layout> symbolizeLayout(uint32_t value) {
  switch (value) {
  case 0: return Layout::ND;
  case 1: return Layout::DN;
  case 2: return Layout::NZ;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyLdL2Cache(LdL2Cache val) {
  switch (val) {
    case LdL2Cache::NMFV: return "nmfv";
    case LdL2Cache::NMLV: return "nmlv";
    case LdL2Cache::NMPRS: return "nmprs";
    case LdL2Cache::NMPREF: return "nmpref";
    case LdL2Cache::NAKEEP: return "nakeep";
    case LdL2Cache::NACLEAN: return "naclean";
    case LdL2Cache::NADROP: return "nadrop";
    case LdL2Cache::IDSFV: return "idsfv";
    case LdL2Cache::IDSLV: return "idslv";
    case LdL2Cache::IDSPRS: return "idsprs";
    case LdL2Cache::IDSPREF: return "idspref";
    case LdL2Cache::EXFV: return "exfv";
    case LdL2Cache::EXLV: return "exlv";
    case LdL2Cache::EXPRS: return "exprs";
    case LdL2Cache::EXPREF: return "expref";
  }
  return "";
}

::std::optional<LdL2Cache> symbolizeLdL2Cache(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<LdL2Cache>>(str)
      .Case("nmfv", LdL2Cache::NMFV)
      .Case("nmlv", LdL2Cache::NMLV)
      .Case("nmprs", LdL2Cache::NMPRS)
      .Case("nmpref", LdL2Cache::NMPREF)
      .Case("nakeep", LdL2Cache::NAKEEP)
      .Case("naclean", LdL2Cache::NACLEAN)
      .Case("nadrop", LdL2Cache::NADROP)
      .Case("idsfv", LdL2Cache::IDSFV)
      .Case("idslv", LdL2Cache::IDSLV)
      .Case("idsprs", LdL2Cache::IDSPRS)
      .Case("idspref", LdL2Cache::IDSPREF)
      .Case("exfv", LdL2Cache::EXFV)
      .Case("exlv", LdL2Cache::EXLV)
      .Case("exprs", LdL2Cache::EXPRS)
      .Case("expref", LdL2Cache::EXPREF)
      .Default(::std::nullopt);
}
::std::optional<LdL2Cache> symbolizeLdL2Cache(uint32_t value) {
  switch (value) {
  case 0: return LdL2Cache::NMFV;
  case 1: return LdL2Cache::NMLV;
  case 2: return LdL2Cache::NMPRS;
  case 3: return LdL2Cache::NMPREF;
  case 4: return LdL2Cache::NAKEEP;
  case 5: return LdL2Cache::NACLEAN;
  case 6: return LdL2Cache::NADROP;
  case 8: return LdL2Cache::IDSFV;
  case 9: return LdL2Cache::IDSLV;
  case 10: return LdL2Cache::IDSPRS;
  case 11: return LdL2Cache::IDSPREF;
  case 12: return LdL2Cache::EXFV;
  case 13: return LdL2Cache::EXLV;
  case 14: return LdL2Cache::EXPRS;
  case 15: return LdL2Cache::EXPREF;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyMadSatMode(MadSatMode val) {
  switch (val) {
    case MadSatMode::Sat: return "sat";
    case MadSatMode::NoSat: return "nosat";
  }
  return "";
}

::std::optional<MadSatMode> symbolizeMadSatMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<MadSatMode>>(str)
      .Case("sat", MadSatMode::Sat)
      .Case("nosat", MadSatMode::NoSat)
      .Default(::std::nullopt);
}
::std::optional<MadSatMode> symbolizeMadSatMode(uint32_t value) {
  switch (value) {
  case 0: return MadSatMode::Sat;
  case 1: return MadSatMode::NoSat;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyMadUnitFlagMode(MadUnitFlagMode val) {
  switch (val) {
    case MadUnitFlagMode::CheckOnly: return "check_only";
    case MadUnitFlagMode::CheckAndSet: return "check_and_set";
  }
  return "";
}

::std::optional<MadUnitFlagMode> symbolizeMadUnitFlagMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<MadUnitFlagMode>>(str)
      .Case("check_only", MadUnitFlagMode::CheckOnly)
      .Case("check_and_set", MadUnitFlagMode::CheckAndSet)
      .Default(::std::nullopt);
}
::std::optional<MadUnitFlagMode> symbolizeMadUnitFlagMode(uint32_t value) {
  switch (value) {
  case 0: return MadUnitFlagMode::CheckOnly;
  case 1: return MadUnitFlagMode::CheckAndSet;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyMaskPattern(MaskPattern val) {
  switch (val) {
    case MaskPattern::P0101: return "P0101";
    case MaskPattern::P1010: return "P1010";
    case MaskPattern::P0001: return "P0001";
    case MaskPattern::P0010: return "P0010";
    case MaskPattern::P0100: return "P0100";
    case MaskPattern::P1000: return "P1000";
    case MaskPattern::P1111: return "P1111";
  }
  return "";
}

::std::optional<MaskPattern> symbolizeMaskPattern(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<MaskPattern>>(str)
      .Case("P0101", MaskPattern::P0101)
      .Case("P1010", MaskPattern::P1010)
      .Case("P0001", MaskPattern::P0001)
      .Case("P0010", MaskPattern::P0010)
      .Case("P0100", MaskPattern::P0100)
      .Case("P1000", MaskPattern::P1000)
      .Case("P1111", MaskPattern::P1111)
      .Default(::std::nullopt);
}
::std::optional<MaskPattern> symbolizeMaskPattern(uint32_t value) {
  switch (value) {
  case 1: return MaskPattern::P0101;
  case 2: return MaskPattern::P1010;
  case 3: return MaskPattern::P0001;
  case 4: return MaskPattern::P0010;
  case 5: return MaskPattern::P0100;
  case 6: return MaskPattern::P1000;
  case 7: return MaskPattern::P1111;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyMemBarKind(MemBarKind val) {
  switch (val) {
    case MemBarKind::VV_ALL: return "VV_ALL";
    case MemBarKind::VST_VLD: return "VST_VLD";
    case MemBarKind::VLD_VST: return "VLD_VST";
    case MemBarKind::VST_VST: return "VST_VST";
    case MemBarKind::VS_ALL: return "VS_ALL";
    case MemBarKind::VST_LD: return "VST_LD";
    case MemBarKind::VLD_ST: return "VLD_ST";
    case MemBarKind::VST_ST: return "VST_ST";
    case MemBarKind::SV_ALL: return "SV_ALL";
    case MemBarKind::ST_VLD: return "ST_VLD";
    case MemBarKind::LD_VST: return "LD_VST";
    case MemBarKind::ST_VST: return "ST_VST";
    case MemBarKind::SS_ALL: return "SS_ALL";
    case MemBarKind::ST_LD: return "ST_LD";
    case MemBarKind::LD_ST: return "LD_ST";
    case MemBarKind::ST_ST: return "ST_ST";
  }
  return "";
}

::std::optional<MemBarKind> symbolizeMemBarKind(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<MemBarKind>>(str)
      .Case("VV_ALL", MemBarKind::VV_ALL)
      .Case("VST_VLD", MemBarKind::VST_VLD)
      .Case("VLD_VST", MemBarKind::VLD_VST)
      .Case("VST_VST", MemBarKind::VST_VST)
      .Case("VS_ALL", MemBarKind::VS_ALL)
      .Case("VST_LD", MemBarKind::VST_LD)
      .Case("VLD_ST", MemBarKind::VLD_ST)
      .Case("VST_ST", MemBarKind::VST_ST)
      .Case("SV_ALL", MemBarKind::SV_ALL)
      .Case("ST_VLD", MemBarKind::ST_VLD)
      .Case("LD_VST", MemBarKind::LD_VST)
      .Case("ST_VST", MemBarKind::ST_VST)
      .Case("SS_ALL", MemBarKind::SS_ALL)
      .Case("ST_LD", MemBarKind::ST_LD)
      .Case("LD_ST", MemBarKind::LD_ST)
      .Case("ST_ST", MemBarKind::ST_ST)
      .Default(::std::nullopt);
}
::std::optional<MemBarKind> symbolizeMemBarKind(uint32_t value) {
  switch (value) {
  case 0: return MemBarKind::VV_ALL;
  case 1: return MemBarKind::VST_VLD;
  case 2: return MemBarKind::VLD_VST;
  case 3: return MemBarKind::VST_VST;
  case 4: return MemBarKind::VS_ALL;
  case 5: return MemBarKind::VST_LD;
  case 6: return MemBarKind::VLD_ST;
  case 7: return MemBarKind::VST_ST;
  case 8: return MemBarKind::SV_ALL;
  case 9: return MemBarKind::ST_VLD;
  case 10: return MemBarKind::LD_VST;
  case 11: return MemBarKind::ST_VST;
  case 12: return MemBarKind::SS_ALL;
  case 13: return MemBarKind::ST_LD;
  case 14: return MemBarKind::LD_ST;
  case 15: return MemBarKind::ST_ST;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyMemPlanMode(MemPlanMode val) {
  switch (val) {
    case MemPlanMode::LOCAL_MEM_PLAN: return "LOCAL_MEM_PLAN";
    case MemPlanMode::GLOBAL_WORKSPACE_PLAN: return "GLOBAL_WORKSPACE_PLAN";
  }
  return "";
}

::std::optional<MemPlanMode> symbolizeMemPlanMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<MemPlanMode>>(str)
      .Case("LOCAL_MEM_PLAN", MemPlanMode::LOCAL_MEM_PLAN)
      .Case("GLOBAL_WORKSPACE_PLAN", MemPlanMode::GLOBAL_WORKSPACE_PLAN)
      .Default(::std::nullopt);
}
::std::optional<MemPlanMode> symbolizeMemPlanMode(uint32_t value) {
  switch (value) {
  case 0: return MemPlanMode::LOCAL_MEM_PLAN;
  case 1: return MemPlanMode::GLOBAL_WORKSPACE_PLAN;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyNotifyOp(NotifyOp val) {
  switch (val) {
    case NotifyOp::AtomicAdd: return "atomic_add";
    case NotifyOp::Set: return "set";
  }
  return "";
}

::std::optional<NotifyOp> symbolizeNotifyOp(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<NotifyOp>>(str)
      .Case("atomic_add", NotifyOp::AtomicAdd)
      .Case("set", NotifyOp::Set)
      .Default(::std::nullopt);
}
::std::optional<NotifyOp> symbolizeNotifyOp(uint32_t value) {
  switch (value) {
  case 0: return NotifyOp::AtomicAdd;
  case 1: return NotifyOp::Set;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyPadMode(PadMode val) {
  switch (val) {
    case PadMode::PadNull: return "PadNull";
    case PadMode::PadFirstElem: return "PadFirstElem";
    case PadMode::PadValue: return "PadValue";
  }
  return "";
}

::std::optional<PadMode> symbolizePadMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<PadMode>>(str)
      .Case("PadNull", PadMode::PadNull)
      .Case("PadFirstElem", PadMode::PadFirstElem)
      .Case("PadValue", PadMode::PadValue)
      .Default(::std::nullopt);
}
::std::optional<PadMode> symbolizePadMode(uint32_t value) {
  switch (value) {
  case 0: return PadMode::PadNull;
  case 1: return PadMode::PadFirstElem;
  case 2: return PadMode::PadValue;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyPadValue(PadValue val) {
  switch (val) {
    case PadValue::Null: return "null";
    case PadValue::Zero: return "zero";
    case PadValue::Max: return "max";
    case PadValue::Min: return "min";
  }
  return "";
}

::std::optional<PadValue> symbolizePadValue(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<PadValue>>(str)
      .Case("null", PadValue::Null)
      .Case("zero", PadValue::Zero)
      .Case("max", PadValue::Max)
      .Case("min", PadValue::Min)
      .Default(::std::nullopt);
}
::std::optional<PadValue> symbolizePadValue(uint32_t value) {
  switch (value) {
  case 0: return PadValue::Null;
  case 1: return PadValue::Zero;
  case 2: return PadValue::Max;
  case 3: return PadValue::Min;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyPIPE(PIPE val) {
  switch (val) {
    case PIPE::PIPE_S: return "PIPE_S";
    case PIPE::PIPE_V: return "PIPE_V";
    case PIPE::PIPE_M: return "PIPE_M";
    case PIPE::PIPE_MTE1: return "PIPE_MTE1";
    case PIPE::PIPE_MTE2: return "PIPE_MTE2";
    case PIPE::PIPE_MTE3: return "PIPE_MTE3";
    case PIPE::PIPE_ALL: return "PIPE_ALL";
    case PIPE::PIPE_MTE4: return "PIPE_MTE4";
    case PIPE::PIPE_MTE5: return "PIPE_MTE5";
    case PIPE::PIPE_V2: return "PIPE_V2";
    case PIPE::PIPE_FIX: return "PIPE_FIX";
    case PIPE::VIRTUAL_PIPE_MTE2_L1A: return "VIRTUAL_PIPE_MTE2_L1A";
    case PIPE::VIRTUAL_PIPE_MTE2_L1B: return "VIRTUAL_PIPE_MTE2_L1B";
    case PIPE::PIPE_NUM: return "PIPE_NUM";
    case PIPE::PIPE_UNASSIGNED: return "PIPE_UNASSIGNED";
  }
  return "";
}

::std::optional<PIPE> symbolizePIPE(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<PIPE>>(str)
      .Case("PIPE_S", PIPE::PIPE_S)
      .Case("PIPE_V", PIPE::PIPE_V)
      .Case("PIPE_M", PIPE::PIPE_M)
      .Case("PIPE_MTE1", PIPE::PIPE_MTE1)
      .Case("PIPE_MTE2", PIPE::PIPE_MTE2)
      .Case("PIPE_MTE3", PIPE::PIPE_MTE3)
      .Case("PIPE_ALL", PIPE::PIPE_ALL)
      .Case("PIPE_MTE4", PIPE::PIPE_MTE4)
      .Case("PIPE_MTE5", PIPE::PIPE_MTE5)
      .Case("PIPE_V2", PIPE::PIPE_V2)
      .Case("PIPE_FIX", PIPE::PIPE_FIX)
      .Case("VIRTUAL_PIPE_MTE2_L1A", PIPE::VIRTUAL_PIPE_MTE2_L1A)
      .Case("VIRTUAL_PIPE_MTE2_L1B", PIPE::VIRTUAL_PIPE_MTE2_L1B)
      .Case("PIPE_NUM", PIPE::PIPE_NUM)
      .Case("PIPE_UNASSIGNED", PIPE::PIPE_UNASSIGNED)
      .Default(::std::nullopt);
}
::std::optional<PIPE> symbolizePIPE(uint32_t value) {
  switch (value) {
  case 0: return PIPE::PIPE_S;
  case 1: return PIPE::PIPE_V;
  case 2: return PIPE::PIPE_M;
  case 3: return PIPE::PIPE_MTE1;
  case 4: return PIPE::PIPE_MTE2;
  case 5: return PIPE::PIPE_MTE3;
  case 6: return PIPE::PIPE_ALL;
  case 7: return PIPE::PIPE_MTE4;
  case 8: return PIPE::PIPE_MTE5;
  case 9: return PIPE::PIPE_V2;
  case 10: return PIPE::PIPE_FIX;
  case 11: return PIPE::VIRTUAL_PIPE_MTE2_L1A;
  case 12: return PIPE::VIRTUAL_PIPE_MTE2_L1B;
  case 13: return PIPE::PIPE_NUM;
  case 99: return PIPE::PIPE_UNASSIGNED;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyPrecisionMode(PrecisionMode val) {
  switch (val) {
    case PrecisionMode::DEFAULT: return "DEFAULT";
    case PrecisionMode::HIGH_PRECISION: return "HIGH_PRECISION";
  }
  return "";
}

::std::optional<PrecisionMode> symbolizePrecisionMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<PrecisionMode>>(str)
      .Case("DEFAULT", PrecisionMode::DEFAULT)
      .Case("HIGH_PRECISION", PrecisionMode::HIGH_PRECISION)
      .Default(::std::nullopt);
}
::std::optional<PrecisionMode> symbolizePrecisionMode(uint32_t value) {
  switch (value) {
  case 0: return PrecisionMode::DEFAULT;
  case 1: return PrecisionMode::HIGH_PRECISION;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyQuantType(QuantType val) {
  switch (val) {
    case QuantType::INT8_SYM: return "INT8_SYM";
    case QuantType::INT8_ASYM: return "INT8_ASYM";
  }
  return "";
}

::std::optional<QuantType> symbolizeQuantType(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<QuantType>>(str)
      .Case("INT8_SYM", QuantType::INT8_SYM)
      .Case("INT8_ASYM", QuantType::INT8_ASYM)
      .Default(::std::nullopt);
}
::std::optional<QuantType> symbolizeQuantType(uint32_t value) {
  switch (value) {
  case 0: return QuantType::INT8_SYM;
  case 1: return QuantType::INT8_ASYM;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyReduceOp(ReduceOp val) {
  switch (val) {
    case ReduceOp::Sum: return "sum";
    case ReduceOp::Max: return "max";
    case ReduceOp::Min: return "min";
  }
  return "";
}

::std::optional<ReduceOp> symbolizeReduceOp(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<ReduceOp>>(str)
      .Case("sum", ReduceOp::Sum)
      .Case("max", ReduceOp::Max)
      .Case("min", ReduceOp::Min)
      .Default(::std::nullopt);
}
::std::optional<ReduceOp> symbolizeReduceOp(uint32_t value) {
  switch (value) {
  case 0: return ReduceOp::Sum;
  case 1: return ReduceOp::Max;
  case 2: return ReduceOp::Min;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyReluPreMode(ReluPreMode val) {
  switch (val) {
    case ReluPreMode::NoRelu: return "no_relu";
    case ReluPreMode::NormalRelu: return "normal_relu";
    case ReluPreMode::ScalarRelu: return "scalar_relu";
    case ReluPreMode::VectorRelu: return "vector_relu";
    case ReluPreMode::Pwl: return "pwl";
  }
  return "";
}

::std::optional<ReluPreMode> symbolizeReluPreMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<ReluPreMode>>(str)
      .Case("no_relu", ReluPreMode::NoRelu)
      .Case("normal_relu", ReluPreMode::NormalRelu)
      .Case("scalar_relu", ReluPreMode::ScalarRelu)
      .Case("vector_relu", ReluPreMode::VectorRelu)
      .Case("pwl", ReluPreMode::Pwl)
      .Default(::std::nullopt);
}
::std::optional<ReluPreMode> symbolizeReluPreMode(uint32_t value) {
  switch (value) {
  case 0: return ReluPreMode::NoRelu;
  case 1: return ReluPreMode::NormalRelu;
  case 2: return ReluPreMode::ScalarRelu;
  case 3: return ReluPreMode::VectorRelu;
  case 4: return ReluPreMode::Pwl;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyRoundMode(RoundMode val) {
  switch (val) {
    case RoundMode::NONE: return "NONE";
    case RoundMode::RINT: return "RINT";
    case RoundMode::ROUND: return "ROUND";
    case RoundMode::FLOOR: return "FLOOR";
    case RoundMode::CEIL: return "CEIL";
    case RoundMode::TRUNC: return "TRUNC";
    case RoundMode::ODD: return "ODD";
    case RoundMode::CAST_RINT: return "CAST_RINT";
  }
  return "";
}

::std::optional<RoundMode> symbolizeRoundMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<RoundMode>>(str)
      .Case("NONE", RoundMode::NONE)
      .Case("RINT", RoundMode::RINT)
      .Case("ROUND", RoundMode::ROUND)
      .Case("FLOOR", RoundMode::FLOOR)
      .Case("CEIL", RoundMode::CEIL)
      .Case("TRUNC", RoundMode::TRUNC)
      .Case("ODD", RoundMode::ODD)
      .Case("CAST_RINT", RoundMode::CAST_RINT)
      .Default(::std::nullopt);
}
::std::optional<RoundMode> symbolizeRoundMode(uint32_t value) {
  switch (value) {
  case 0: return RoundMode::NONE;
  case 1: return RoundMode::RINT;
  case 2: return RoundMode::ROUND;
  case 3: return RoundMode::FLOOR;
  case 4: return RoundMode::CEIL;
  case 5: return RoundMode::TRUNC;
  case 6: return RoundMode::ODD;
  case 7: return RoundMode::CAST_RINT;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyRounding(Rounding val) {
  switch (val) {
    case Rounding::R: return "r";
    case Rounding::A: return "a";
    case Rounding::F: return "f";
    case Rounding::C: return "c";
    case Rounding::Z: return "z";
    case Rounding::O: return "o";
    case Rounding::H: return "h";
  }
  return "";
}

::std::optional<Rounding> symbolizeRounding(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<Rounding>>(str)
      .Case("r", Rounding::R)
      .Case("a", Rounding::A)
      .Case("f", Rounding::F)
      .Case("c", Rounding::C)
      .Case("z", Rounding::Z)
      .Case("o", Rounding::O)
      .Case("h", Rounding::H)
      .Default(::std::nullopt);
}
::std::optional<Rounding> symbolizeRounding(uint32_t value) {
  switch (value) {
  case 0: return Rounding::R;
  case 1: return Rounding::A;
  case 2: return Rounding::F;
  case 3: return Rounding::C;
  case 4: return Rounding::Z;
  case 5: return Rounding::O;
  case 6: return Rounding::H;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifySLayout(SLayout val) {
  switch (val) {
    case SLayout::NoneBox: return "none_box";
    case SLayout::RowMajor: return "row_major";
    case SLayout::ColMajor: return "col_major";
  }
  return "";
}

::std::optional<SLayout> symbolizeSLayout(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<SLayout>>(str)
      .Case("none_box", SLayout::NoneBox)
      .Case("row_major", SLayout::RowMajor)
      .Case("col_major", SLayout::ColMajor)
      .Default(::std::nullopt);
}
::std::optional<SLayout> symbolizeSLayout(uint32_t value) {
  switch (value) {
  case 0: return SLayout::NoneBox;
  case 1: return SLayout::RowMajor;
  case 2: return SLayout::ColMajor;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifySTPhase(STPhase val) {
  switch (val) {
    case STPhase::Unspecified: return "unspecified";
    case STPhase::Partial: return "partial";
    case STPhase::Final: return "final";
  }
  return "";
}

::std::optional<STPhase> symbolizeSTPhase(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<STPhase>>(str)
      .Case("unspecified", STPhase::Unspecified)
      .Case("partial", STPhase::Partial)
      .Case("final", STPhase::Final)
      .Default(::std::nullopt);
}
::std::optional<STPhase> symbolizeSTPhase(uint32_t value) {
  switch (value) {
  case 0: return STPhase::Unspecified;
  case 2: return STPhase::Partial;
  case 3: return STPhase::Final;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifySaturation(Saturation val) {
  switch (val) {
    case Saturation::Disable: return "nosat";
    case Saturation::Enable: return "sat";
  }
  return "";
}

::std::optional<Saturation> symbolizeSaturation(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<Saturation>>(str)
      .Case("nosat", Saturation::Disable)
      .Case("sat", Saturation::Enable)
      .Default(::std::nullopt);
}
::std::optional<Saturation> symbolizeSaturation(uint32_t value) {
  switch (value) {
  case 0: return Saturation::Disable;
  case 1: return Saturation::Enable;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifySaturationMode(SaturationMode val) {
  switch (val) {
    case SaturationMode::ON: return "ON";
    case SaturationMode::OFF: return "OFF";
  }
  return "";
}

::std::optional<SaturationMode> symbolizeSaturationMode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<SaturationMode>>(str)
      .Case("ON", SaturationMode::ON)
      .Case("OFF", SaturationMode::OFF)
      .Default(::std::nullopt);
}
::std::optional<SaturationMode> symbolizeSaturationMode(uint32_t value) {
  switch (value) {
  case 0: return SaturationMode::ON;
  case 1: return SaturationMode::OFF;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyScatterAtomicOp(ScatterAtomicOp val) {
  switch (val) {
    case ScatterAtomicOp::None: return "none";
    case ScatterAtomicOp::Add: return "add";
    case ScatterAtomicOp::Max: return "max";
    case ScatterAtomicOp::Min: return "min";
  }
  return "";
}

::std::optional<ScatterAtomicOp> symbolizeScatterAtomicOp(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<ScatterAtomicOp>>(str)
      .Case("none", ScatterAtomicOp::None)
      .Case("add", ScatterAtomicOp::Add)
      .Case("max", ScatterAtomicOp::Max)
      .Case("min", ScatterAtomicOp::Min)
      .Default(::std::nullopt);
}
::std::optional<ScatterAtomicOp> symbolizeScatterAtomicOp(uint32_t value) {
  switch (value) {
  case 0: return ScatterAtomicOp::None;
  case 1: return ScatterAtomicOp::Add;
  case 2: return ScatterAtomicOp::Max;
  case 3: return ScatterAtomicOp::Min;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyScatterOOB(ScatterOOB val) {
  switch (val) {
    case ScatterOOB::Undefined: return "undefined";
    case ScatterOOB::Skip: return "skip";
    case ScatterOOB::Clamp: return "clamp";
    case ScatterOOB::Wrap: return "wrap";
  }
  return "";
}

::std::optional<ScatterOOB> symbolizeScatterOOB(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<ScatterOOB>>(str)
      .Case("undefined", ScatterOOB::Undefined)
      .Case("skip", ScatterOOB::Skip)
      .Case("clamp", ScatterOOB::Clamp)
      .Case("wrap", ScatterOOB::Wrap)
      .Default(::std::nullopt);
}
::std::optional<ScatterOOB> symbolizeScatterOOB(uint32_t value) {
  switch (value) {
  case 0: return ScatterOOB::Undefined;
  case 1: return ScatterOOB::Skip;
  case 2: return ScatterOOB::Clamp;
  case 3: return ScatterOOB::Wrap;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifySignedness(Signedness val) {
  switch (val) {
    case Signedness::Signed: return "signed";
    case Signedness::Unsigned: return "unsigned";
  }
  return "";
}

::std::optional<Signedness> symbolizeSignedness(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<Signedness>>(str)
      .Case("signed", Signedness::Signed)
      .Case("unsigned", Signedness::Unsigned)
      .Default(::std::nullopt);
}
::std::optional<Signedness> symbolizeSignedness(uint32_t value) {
  switch (value) {
  case 0: return Signedness::Signed;
  case 1: return Signedness::Unsigned;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyStL2Cache(StL2Cache val) {
  switch (val) {
    case StL2Cache::NMFV: return "nmfv";
    case StL2Cache::NMLV: return "nmlv";
    case StL2Cache::NMPRS: return "nmprs";
    case StL2Cache::NMRED: return "nmred";
    case StL2Cache::NACI: return "naci";
    case StL2Cache::NAPW: return "napw";
    case StL2Cache::NAPI: return "napi";
    case StL2Cache::NARED: return "nared";
    case StL2Cache::WBHFV: return "wbhfv";
    case StL2Cache::WBHLV: return "wbhlv";
    case StL2Cache::WBHPRS: return "wbhprs";
    case StL2Cache::WBHRED: return "wbhred";
    case StL2Cache::WTSFV: return "wtsfv";
    case StL2Cache::WTSLV: return "wtslv";
    case StL2Cache::WTSPRS: return "wtsprs";
    case StL2Cache::WTSRED: return "wtsred";
  }
  return "";
}

::std::optional<StL2Cache> symbolizeStL2Cache(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<StL2Cache>>(str)
      .Case("nmfv", StL2Cache::NMFV)
      .Case("nmlv", StL2Cache::NMLV)
      .Case("nmprs", StL2Cache::NMPRS)
      .Case("nmred", StL2Cache::NMRED)
      .Case("naci", StL2Cache::NACI)
      .Case("napw", StL2Cache::NAPW)
      .Case("napi", StL2Cache::NAPI)
      .Case("nared", StL2Cache::NARED)
      .Case("wbhfv", StL2Cache::WBHFV)
      .Case("wbhlv", StL2Cache::WBHLV)
      .Case("wbhprs", StL2Cache::WBHPRS)
      .Case("wbhred", StL2Cache::WBHRED)
      .Case("wtsfv", StL2Cache::WTSFV)
      .Case("wtslv", StL2Cache::WTSLV)
      .Case("wtsprs", StL2Cache::WTSPRS)
      .Case("wtsred", StL2Cache::WTSRED)
      .Default(::std::nullopt);
}
::std::optional<StL2Cache> symbolizeStL2Cache(uint32_t value) {
  switch (value) {
  case 0: return StL2Cache::NMFV;
  case 1: return StL2Cache::NMLV;
  case 2: return StL2Cache::NMPRS;
  case 3: return StL2Cache::NMRED;
  case 4: return StL2Cache::NACI;
  case 5: return StL2Cache::NAPW;
  case 6: return StL2Cache::NAPI;
  case 7: return StL2Cache::NARED;
  case 8: return StL2Cache::WBHFV;
  case 9: return StL2Cache::WBHLV;
  case 10: return StL2Cache::WBHPRS;
  case 11: return StL2Cache::WBHRED;
  case 12: return StL2Cache::WTSFV;
  case 13: return StL2Cache::WTSLV;
  case 14: return StL2Cache::WTSPRS;
  case 15: return StL2Cache::WTSRED;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifySyncOpType(SyncOpType val) {
  switch (val) {
    case SyncOpType::TLOAD: return "TLOAD";
    case SyncOpType::TSTORE_ACC: return "TSTORE_ACC";
    case SyncOpType::TSTORE_VEC: return "TSTORE_VEC";
    case SyncOpType::TMOV_M2L: return "TMOV_M2L";
    case SyncOpType::TMOV_M2S: return "TMOV_M2S";
    case SyncOpType::TMOV_M2B: return "TMOV_M2B";
    case SyncOpType::TMOV_M2V: return "TMOV_M2V";
    case SyncOpType::TMOV_V2M: return "TMOV_V2M";
    case SyncOpType::TMATMUL: return "TMATMUL";
    case SyncOpType::TVEC: return "TVEC";
    case SyncOpType::TVECWAIT_EVENT: return "TVECWAIT_EVENT";
  }
  return "";
}

::std::optional<SyncOpType> symbolizeSyncOpType(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<SyncOpType>>(str)
      .Case("TLOAD", SyncOpType::TLOAD)
      .Case("TSTORE_ACC", SyncOpType::TSTORE_ACC)
      .Case("TSTORE_VEC", SyncOpType::TSTORE_VEC)
      .Case("TMOV_M2L", SyncOpType::TMOV_M2L)
      .Case("TMOV_M2S", SyncOpType::TMOV_M2S)
      .Case("TMOV_M2B", SyncOpType::TMOV_M2B)
      .Case("TMOV_M2V", SyncOpType::TMOV_M2V)
      .Case("TMOV_V2M", SyncOpType::TMOV_V2M)
      .Case("TMATMUL", SyncOpType::TMATMUL)
      .Case("TVEC", SyncOpType::TVEC)
      .Case("TVECWAIT_EVENT", SyncOpType::TVECWAIT_EVENT)
      .Default(::std::nullopt);
}
::std::optional<SyncOpType> symbolizeSyncOpType(uint32_t value) {
  switch (value) {
  case 0: return SyncOpType::TLOAD;
  case 1: return SyncOpType::TSTORE_ACC;
  case 2: return SyncOpType::TSTORE_VEC;
  case 3: return SyncOpType::TMOV_M2L;
  case 4: return SyncOpType::TMOV_M2S;
  case 5: return SyncOpType::TMOV_M2B;
  case 6: return SyncOpType::TMOV_M2V;
  case 7: return SyncOpType::TMOV_V2M;
  case 8: return SyncOpType::TMATMUL;
  case 9: return SyncOpType::TVEC;
  case 10: return SyncOpType::TVECWAIT_EVENT;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyTFuncCoreType(TFuncCoreType val) {
  switch (val) {
    case TFuncCoreType::AIC: return "AIC";
    case TFuncCoreType::AIV: return "AIV";
    case TFuncCoreType::MIX: return "MIX";
    case TFuncCoreType::AIC_OR_AIV: return "AIC_OR_AIV";
  }
  return "";
}

::std::optional<TFuncCoreType> symbolizeTFuncCoreType(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<TFuncCoreType>>(str)
      .Case("AIC", TFuncCoreType::AIC)
      .Case("AIV", TFuncCoreType::AIV)
      .Case("MIX", TFuncCoreType::MIX)
      .Case("AIC_OR_AIV", TFuncCoreType::AIC_OR_AIV)
      .Default(::std::nullopt);
}
::std::optional<TFuncCoreType> symbolizeTFuncCoreType(uint32_t value) {
  switch (value) {
  case 1: return TFuncCoreType::AIC;
  case 2: return TFuncCoreType::AIV;
  case 3: return TFuncCoreType::MIX;
  case 4: return TFuncCoreType::AIC_OR_AIV;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyTModuleCoreType(TModuleCoreType val) {
  switch (val) {
    case TModuleCoreType::AIC: return "AIC";
    case TModuleCoreType::AIV: return "AIV";
    case TModuleCoreType::MIX: return "MIX";
  }
  return "";
}

::std::optional<TModuleCoreType> symbolizeTModuleCoreType(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<TModuleCoreType>>(str)
      .Case("AIC", TModuleCoreType::AIC)
      .Case("AIV", TModuleCoreType::AIV)
      .Case("MIX", TModuleCoreType::MIX)
      .Default(::std::nullopt);
}
::std::optional<TModuleCoreType> symbolizeTModuleCoreType(uint32_t value) {
  switch (value) {
  case 1: return TModuleCoreType::AIC;
  case 2: return TModuleCoreType::AIV;
  case 3: return TModuleCoreType::MIX;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyTf32Mode(Tf32Mode val) {
  switch (val) {
    case Tf32Mode::RoundEven: return "round_even";
    case Tf32Mode::RoundAway: return "round_away";
  }
  return "";
}

::std::optional<Tf32Mode> symbolizeTf32Mode(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<Tf32Mode>>(str)
      .Case("round_even", Tf32Mode::RoundEven)
      .Case("round_away", Tf32Mode::RoundAway)
      .Default(::std::nullopt);
}
::std::optional<Tf32Mode> symbolizeTf32Mode(uint32_t value) {
  switch (value) {
  case 0: return Tf32Mode::RoundEven;
  case 1: return Tf32Mode::RoundAway;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyAccStoreUnitFlagCtrl(AccStoreUnitFlagCtrl val) {
  switch (val) {
    case AccStoreUnitFlagCtrl::Off: return "off";
    case AccStoreUnitFlagCtrl::CheckOnly: return "check_only";
    case AccStoreUnitFlagCtrl::CheckAndClear: return "check_and_clear";
  }
  return "";
}

::std::optional<AccStoreUnitFlagCtrl> symbolizeAccStoreUnitFlagCtrl(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<AccStoreUnitFlagCtrl>>(str)
      .Case("off", AccStoreUnitFlagCtrl::Off)
      .Case("check_only", AccStoreUnitFlagCtrl::CheckOnly)
      .Case("check_and_clear", AccStoreUnitFlagCtrl::CheckAndClear)
      .Default(::std::nullopt);
}
::std::optional<AccStoreUnitFlagCtrl> symbolizeAccStoreUnitFlagCtrl(uint32_t value) {
  switch (value) {
  case 0: return AccStoreUnitFlagCtrl::Off;
  case 2: return AccStoreUnitFlagCtrl::CheckOnly;
  case 3: return AccStoreUnitFlagCtrl::CheckAndClear;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

namespace mlir {
namespace pto {
::llvm::StringRef stringifyWaitCmp(WaitCmp val) {
  switch (val) {
    case WaitCmp::EQ: return "eq";
    case WaitCmp::NE: return "ne";
    case WaitCmp::GT: return "gt";
    case WaitCmp::GE: return "ge";
    case WaitCmp::LT: return "lt";
    case WaitCmp::LE: return "le";
  }
  return "";
}

::std::optional<WaitCmp> symbolizeWaitCmp(::llvm::StringRef str) {
  return ::llvm::StringSwitch<::std::optional<WaitCmp>>(str)
      .Case("eq", WaitCmp::EQ)
      .Case("ne", WaitCmp::NE)
      .Case("gt", WaitCmp::GT)
      .Case("ge", WaitCmp::GE)
      .Case("lt", WaitCmp::LT)
      .Case("le", WaitCmp::LE)
      .Default(::std::nullopt);
}
::std::optional<WaitCmp> symbolizeWaitCmp(uint32_t value) {
  switch (value) {
  case 0: return WaitCmp::EQ;
  case 1: return WaitCmp::NE;
  case 2: return WaitCmp::GT;
  case 3: return WaitCmp::GE;
  case 4: return WaitCmp::LT;
  case 5: return WaitCmp::LE;
  default: return ::std::nullopt;
  }
}

} // namespace pto
} // namespace mlir

