/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Enum Utility Declarations                                                  *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|* From: PTOOps.td                                                            *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

namespace mlir {
namespace pto {
// PTO accumulator store atomic op
enum class AccStoreAtomicOp : uint32_t {
  Add = 0,
  Max = 1,
  Min = 2,
};

::std::optional<AccStoreAtomicOp> symbolizeAccStoreAtomicOp(uint32_t);
::llvm::StringRef stringifyAccStoreAtomicOp(AccStoreAtomicOp);
::std::optional<AccStoreAtomicOp> symbolizeAccStoreAtomicOp(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForAccStoreAtomicOp() {
  return 2;
}


inline ::llvm::StringRef stringifyEnum(AccStoreAtomicOp enumValue) {
  return stringifyAccStoreAtomicOp(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<AccStoreAtomicOp> symbolizeEnum<AccStoreAtomicOp>(::llvm::StringRef str) {
  return symbolizeAccStoreAtomicOp(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::AccStoreAtomicOp, ::mlir::pto::AccStoreAtomicOp> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::AccStoreAtomicOp> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO accumulator store atomic op");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::AccStoreAtomicOp> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::AccStoreAtomicOp>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO accumulator store atomic op specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::AccStoreAtomicOp value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::AccStoreAtomicOp> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::AccStoreAtomicOp getEmptyKey() {
    return static_cast<::mlir::pto::AccStoreAtomicOp>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::AccStoreAtomicOp getTombstoneKey() {
    return static_cast<::mlir::pto::AccStoreAtomicOp>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::AccStoreAtomicOp &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::AccStoreAtomicOp &lhs, const ::mlir::pto::AccStoreAtomicOp &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO accumulator store atomic type
enum class AccStoreAtomicType : uint32_t {
  F32 = 1,
  F16 = 2,
  S16 = 3,
  S32 = 4,
  S8 = 5,
  BF16 = 6,
};

::std::optional<AccStoreAtomicType> symbolizeAccStoreAtomicType(uint32_t);
::llvm::StringRef stringifyAccStoreAtomicType(AccStoreAtomicType);
::std::optional<AccStoreAtomicType> symbolizeAccStoreAtomicType(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForAccStoreAtomicType() {
  return 6;
}


inline ::llvm::StringRef stringifyEnum(AccStoreAtomicType enumValue) {
  return stringifyAccStoreAtomicType(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<AccStoreAtomicType> symbolizeEnum<AccStoreAtomicType>(::llvm::StringRef str) {
  return symbolizeAccStoreAtomicType(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::AccStoreAtomicType, ::mlir::pto::AccStoreAtomicType> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::AccStoreAtomicType> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO accumulator store atomic type");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::AccStoreAtomicType> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::AccStoreAtomicType>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO accumulator store atomic type specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::AccStoreAtomicType value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::AccStoreAtomicType> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::AccStoreAtomicType getEmptyKey() {
    return static_cast<::mlir::pto::AccStoreAtomicType>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::AccStoreAtomicType getTombstoneKey() {
    return static_cast<::mlir::pto::AccStoreAtomicType>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::AccStoreAtomicType &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::AccStoreAtomicType &lhs, const ::mlir::pto::AccStoreAtomicType &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO accumulator store layout conversion mode
enum class AccStoreMode : uint32_t {
  Nz2nd = 0,
  Nz2dn = 1,
  Nz2nz = 2,
};

::std::optional<AccStoreMode> symbolizeAccStoreMode(uint32_t);
::llvm::StringRef stringifyAccStoreMode(AccStoreMode);
::std::optional<AccStoreMode> symbolizeAccStoreMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForAccStoreMode() {
  return 2;
}


inline ::llvm::StringRef stringifyEnum(AccStoreMode enumValue) {
  return stringifyAccStoreMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<AccStoreMode> symbolizeEnum<AccStoreMode>(::llvm::StringRef str) {
  return symbolizeAccStoreMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::AccStoreMode, ::mlir::pto::AccStoreMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::AccStoreMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO accumulator store layout conversion mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::AccStoreMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::AccStoreMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO accumulator store layout conversion mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::AccStoreMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::AccStoreMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::AccStoreMode getEmptyKey() {
    return static_cast<::mlir::pto::AccStoreMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::AccStoreMode getTombstoneKey() {
    return static_cast<::mlir::pto::AccStoreMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::AccStoreMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::AccStoreMode &lhs, const ::mlir::pto::AccStoreMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO accumulator store pre-quantization mode
enum class AccStoreQuantPreMode : uint32_t {
  NoConvert = 0,
  F32F16 = 1,
  QF322HIF8PreVec = 2,
  QF322HIF8PreScalar = 3,
  QF322HIF8PreHybridVec = 4,
  QF322HIF8PreHybridScalar = 5,
  DEQS32IntVec = 6,
  DEQS32IntScalar = 7,
  REQ8Vec = 8,
  REQ8Scalar = 9,
  DEQF16Vec = 10,
  DEQF16Scalar = 11,
  QF322FP8PreVec = 12,
  QF322FP8PreScalar = 13,
  QF322F32PreVec = 14,
  QF322F32PreScalar = 15,
  F32BF16 = 16,
  QF162B8PreVec = 17,
  QF162B8PreScalar = 18,
  QF162S4PreVec = 19,
  QF162S4PreScalar = 20,
  REQ4Vec = 21,
  REQ4Scalar = 22,
  QF322B8PreVec = 23,
  QF322B8PreScalar = 24,
  QF322S4PreVec = 25,
  QF322S4PreScalar = 26,
  DEQS16Vec = 27,
  DEQS16Scalar = 28,
  QF162S16PreVec = 29,
  QF162S16PreScalar = 30,
  QF322F16PreVec = 31,
  QF322F16PreScalar = 32,
  QF322BF16PreVec = 33,
  QF322BF16PreScalar = 34,
  QS322BF16PreVec = 35,
  QS322BF16PreScalar = 36,
};

::std::optional<AccStoreQuantPreMode> symbolizeAccStoreQuantPreMode(uint32_t);
::llvm::StringRef stringifyAccStoreQuantPreMode(AccStoreQuantPreMode);
::std::optional<AccStoreQuantPreMode> symbolizeAccStoreQuantPreMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForAccStoreQuantPreMode() {
  return 36;
}


inline ::llvm::StringRef stringifyEnum(AccStoreQuantPreMode enumValue) {
  return stringifyAccStoreQuantPreMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<AccStoreQuantPreMode> symbolizeEnum<AccStoreQuantPreMode>(::llvm::StringRef str) {
  return symbolizeAccStoreQuantPreMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::AccStoreQuantPreMode, ::mlir::pto::AccStoreQuantPreMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::AccStoreQuantPreMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO accumulator store pre-quantization mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::AccStoreQuantPreMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::AccStoreQuantPreMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO accumulator store pre-quantization mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::AccStoreQuantPreMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::AccStoreQuantPreMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::AccStoreQuantPreMode getEmptyKey() {
    return static_cast<::mlir::pto::AccStoreQuantPreMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::AccStoreQuantPreMode getTombstoneKey() {
    return static_cast<::mlir::pto::AccStoreQuantPreMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::AccStoreQuantPreMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::AccStoreQuantPreMode &lhs, const ::mlir::pto::AccStoreQuantPreMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO accumulator store saturation mode
enum class AccStoreSatMode : uint32_t {
  Sat = 0,
  NoSat = 1,
  SatPreserveNan = 2,
};

::std::optional<AccStoreSatMode> symbolizeAccStoreSatMode(uint32_t);
::llvm::StringRef stringifyAccStoreSatMode(AccStoreSatMode);
::std::optional<AccStoreSatMode> symbolizeAccStoreSatMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForAccStoreSatMode() {
  return 2;
}


inline ::llvm::StringRef stringifyEnum(AccStoreSatMode enumValue) {
  return stringifyAccStoreSatMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<AccStoreSatMode> symbolizeEnum<AccStoreSatMode>(::llvm::StringRef str) {
  return symbolizeAccStoreSatMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::AccStoreSatMode, ::mlir::pto::AccStoreSatMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::AccStoreSatMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO accumulator store saturation mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::AccStoreSatMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::AccStoreSatMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO accumulator store saturation mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::AccStoreSatMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::AccStoreSatMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::AccStoreSatMode getEmptyKey() {
    return static_cast<::mlir::pto::AccStoreSatMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::AccStoreSatMode getTombstoneKey() {
    return static_cast<::mlir::pto::AccStoreSatMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::AccStoreSatMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::AccStoreSatMode &lhs, const ::mlir::pto::AccStoreSatMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO accumulator store UB destination mode
enum class AccStoreUbDstMode : uint32_t {
  Single = 0,
  SplitM = 1,
  SplitN = 2,
};

::std::optional<AccStoreUbDstMode> symbolizeAccStoreUbDstMode(uint32_t);
::llvm::StringRef stringifyAccStoreUbDstMode(AccStoreUbDstMode);
::std::optional<AccStoreUbDstMode> symbolizeAccStoreUbDstMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForAccStoreUbDstMode() {
  return 2;
}


inline ::llvm::StringRef stringifyEnum(AccStoreUbDstMode enumValue) {
  return stringifyAccStoreUbDstMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<AccStoreUbDstMode> symbolizeEnum<AccStoreUbDstMode>(::llvm::StringRef str) {
  return symbolizeAccStoreUbDstMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::AccStoreUbDstMode, ::mlir::pto::AccStoreUbDstMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::AccStoreUbDstMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO accumulator store UB destination mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::AccStoreUbDstMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::AccStoreUbDstMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO accumulator store UB destination mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::AccStoreUbDstMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::AccStoreUbDstMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::AccStoreUbDstMode getEmptyKey() {
    return static_cast<::mlir::pto::AccStoreUbDstMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::AccStoreUbDstMode getTombstoneKey() {
    return static_cast<::mlir::pto::AccStoreUbDstMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::AccStoreUbDstMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::AccStoreUbDstMode &lhs, const ::mlir::pto::AccStoreUbDstMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// TMOV acc-to-vec mode
enum class AccToVecMode : uint32_t {
  SingleModeVec0 = 0,
  SingleModeVec1 = 1,
  DualModeSplitM = 2,
  DualModeSplitN = 3,
};

::std::optional<AccToVecMode> symbolizeAccToVecMode(uint32_t);
::llvm::StringRef stringifyAccToVecMode(AccToVecMode);
::std::optional<AccToVecMode> symbolizeAccToVecMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForAccToVecMode() {
  return 3;
}


inline ::llvm::StringRef stringifyEnum(AccToVecMode enumValue) {
  return stringifyAccToVecMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<AccToVecMode> symbolizeEnum<AccToVecMode>(::llvm::StringRef str) {
  return symbolizeAccToVecMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::AccToVecMode, ::mlir::pto::AccToVecMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::AccToVecMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for TMOV acc-to-vec mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::AccToVecMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::AccToVecMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid TMOV acc-to-vec mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::AccToVecMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::AccToVecMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::AccToVecMode getEmptyKey() {
    return static_cast<::mlir::pto::AccToVecMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::AccToVecMode getTombstoneKey() {
    return static_cast<::mlir::pto::AccToVecMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::AccToVecMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::AccToVecMode &lhs, const ::mlir::pto::AccToVecMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO Address Space
enum class AddressSpace : uint32_t {
  Zero = 0,
  GM = 1,
  MAT = 2,
  LEFT = 3,
  RIGHT = 4,
  ACC = 5,
  VEC = 6,
  BIAS = 7,
  SCALING = 8,
};

::std::optional<AddressSpace> symbolizeAddressSpace(uint32_t);
::llvm::StringRef stringifyAddressSpace(AddressSpace);
::std::optional<AddressSpace> symbolizeAddressSpace(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForAddressSpace() {
  return 8;
}


inline ::llvm::StringRef stringifyEnum(AddressSpace enumValue) {
  return stringifyAddressSpace(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<AddressSpace> symbolizeEnum<AddressSpace>(::llvm::StringRef str) {
  return symbolizeAddressSpace(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::AddressSpace, ::mlir::pto::AddressSpace> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::AddressSpace> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO Address Space");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::AddressSpace> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::AddressSpace>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO Address Space specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::AddressSpace value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::AddressSpace> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::AddressSpace getEmptyKey() {
    return static_cast<::mlir::pto::AddressSpace>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::AddressSpace getTombstoneKey() {
    return static_cast<::mlir::pto::AddressSpace>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::AddressSpace &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::AddressSpace &lhs, const ::mlir::pto::AddressSpace &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO TSTORE atomic mode
enum class AtomicType : uint32_t {
  AtomicNone = 0,
  AtomicAdd = 1,
};

::std::optional<AtomicType> symbolizeAtomicType(uint32_t);
::llvm::StringRef stringifyAtomicType(AtomicType);
::std::optional<AtomicType> symbolizeAtomicType(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForAtomicType() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(AtomicType enumValue) {
  return stringifyAtomicType(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<AtomicType> symbolizeEnum<AtomicType>(::llvm::StringRef str) {
  return symbolizeAtomicType(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::AtomicType, ::mlir::pto::AtomicType> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::AtomicType> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO TSTORE atomic mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::AtomicType> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::AtomicType>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO TSTORE atomic mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::AtomicType value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::AtomicType> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::AtomicType getEmptyKey() {
    return static_cast<::mlir::pto::AtomicType>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::AtomicType getTombstoneKey() {
    return static_cast<::mlir::pto::AtomicType>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::AtomicType &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::AtomicType &lhs, const ::mlir::pto::AtomicType &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// Base layout
enum class BLayout : uint32_t {
  RowMajor = 0,
  ColMajor = 1,
};

::std::optional<BLayout> symbolizeBLayout(uint32_t);
::llvm::StringRef stringifyBLayout(BLayout);
::std::optional<BLayout> symbolizeBLayout(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForBLayout() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(BLayout enumValue) {
  return stringifyBLayout(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<BLayout> symbolizeEnum<BLayout>(::llvm::StringRef str) {
  return symbolizeBLayout(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::BLayout, ::mlir::pto::BLayout> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::BLayout> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for Base layout");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::BLayout> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::BLayout>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid Base layout specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::BLayout value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::BLayout> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::BLayout getEmptyKey() {
    return static_cast<::mlir::pto::BLayout>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::BLayout getTombstoneKey() {
    return static_cast<::mlir::pto::BLayout>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::BLayout &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::BLayout &lhs, const ::mlir::pto::BLayout &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO compare mode
enum class CmpMode : uint32_t {
  EQ = 0,
  NE = 1,
  LT = 2,
  LE = 3,
  GT = 4,
  GE = 5,
};

::std::optional<CmpMode> symbolizeCmpMode(uint32_t);
::llvm::StringRef stringifyCmpMode(CmpMode);
::std::optional<CmpMode> symbolizeCmpMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForCmpMode() {
  return 5;
}


inline ::llvm::StringRef stringifyEnum(CmpMode enumValue) {
  return stringifyCmpMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<CmpMode> symbolizeEnum<CmpMode>(::llvm::StringRef str) {
  return symbolizeCmpMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::CmpMode, ::mlir::pto::CmpMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::CmpMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO compare mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::CmpMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::CmpMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO compare mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::CmpMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::CmpMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::CmpMode getEmptyKey() {
    return static_cast<::mlir::pto::CmpMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::CmpMode getTombstoneKey() {
    return static_cast<::mlir::pto::CmpMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::CmpMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::CmpMode &lhs, const ::mlir::pto::CmpMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// Tile compact mode
enum class CompactMode : uint32_t {
  Null = 0,
  Normal = 1,
  RowPlusOne = 2,
};

::std::optional<CompactMode> symbolizeCompactMode(uint32_t);
::llvm::StringRef stringifyCompactMode(CompactMode);
::std::optional<CompactMode> symbolizeCompactMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForCompactMode() {
  return 2;
}


inline ::llvm::StringRef stringifyEnum(CompactMode enumValue) {
  return stringifyCompactMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<CompactMode> symbolizeEnum<CompactMode>(::llvm::StringRef str) {
  return symbolizeCompactMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::CompactMode, ::mlir::pto::CompactMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::CompactMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for Tile compact mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::CompactMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::CompactMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid Tile compact mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::CompactMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::CompactMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::CompactMode getEmptyKey() {
    return static_cast<::mlir::pto::CompactMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::CompactMode getTombstoneKey() {
    return static_cast<::mlir::pto::CompactMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::CompactMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::CompactMode &lhs, const ::mlir::pto::CompactMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO cube fractal load source layout mode
enum class CubeLoadFracMode : uint32_t {
  Nd2nz = 0,
  Dn2nz = 1,
};

::std::optional<CubeLoadFracMode> symbolizeCubeLoadFracMode(uint32_t);
::llvm::StringRef stringifyCubeLoadFracMode(CubeLoadFracMode);
::std::optional<CubeLoadFracMode> symbolizeCubeLoadFracMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForCubeLoadFracMode() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(CubeLoadFracMode enumValue) {
  return stringifyCubeLoadFracMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<CubeLoadFracMode> symbolizeEnum<CubeLoadFracMode>(::llvm::StringRef str) {
  return symbolizeCubeLoadFracMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::CubeLoadFracMode, ::mlir::pto::CubeLoadFracMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::CubeLoadFracMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO cube fractal load source layout mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::CubeLoadFracMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::CubeLoadFracMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO cube fractal load source layout mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::CubeLoadFracMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::CubeLoadFracMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::CubeLoadFracMode getEmptyKey() {
    return static_cast<::mlir::pto::CubeLoadFracMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::CubeLoadFracMode getTombstoneKey() {
    return static_cast<::mlir::pto::CubeLoadFracMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::CubeLoadFracMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::CubeLoadFracMode &lhs, const ::mlir::pto::CubeLoadFracMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// Event ID for Synchronization
enum class EVENT : uint32_t {
  EVENT_ID0 = 0,
  EVENT_ID1 = 1,
  EVENT_ID2 = 2,
  EVENT_ID3 = 3,
  EVENT_ID4 = 4,
  EVENT_ID5 = 5,
  EVENT_ID6 = 6,
  EVENT_ID7 = 7,
};

::std::optional<EVENT> symbolizeEVENT(uint32_t);
::llvm::StringRef stringifyEVENT(EVENT);
::std::optional<EVENT> symbolizeEVENT(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForEVENT() {
  return 7;
}


inline ::llvm::StringRef stringifyEnum(EVENT enumValue) {
  return stringifyEVENT(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<EVENT> symbolizeEnum<EVENT>(::llvm::StringRef str) {
  return symbolizeEVENT(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::EVENT, ::mlir::pto::EVENT> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::EVENT> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for Event ID for Synchronization");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::EVENT> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::EVENT>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid Event ID for Synchronization specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::EVENT value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::EVENT> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::EVENT getEmptyKey() {
    return static_cast<::mlir::pto::EVENT>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::EVENT getTombstoneKey() {
    return static_cast<::mlir::pto::EVENT>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::EVENT &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::EVENT &lhs, const ::mlir::pto::EVENT &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO Function Kernel Kind
enum class FunctionKernelKind : uint32_t {
  Cube = 0,
  Vector = 1,
};

::std::optional<FunctionKernelKind> symbolizeFunctionKernelKind(uint32_t);
::llvm::StringRef stringifyFunctionKernelKind(FunctionKernelKind);
::std::optional<FunctionKernelKind> symbolizeFunctionKernelKind(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForFunctionKernelKind() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(FunctionKernelKind enumValue) {
  return stringifyFunctionKernelKind(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<FunctionKernelKind> symbolizeEnum<FunctionKernelKind>(::llvm::StringRef str) {
  return symbolizeFunctionKernelKind(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::FunctionKernelKind, ::mlir::pto::FunctionKernelKind> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::FunctionKernelKind> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO Function Kernel Kind");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::FunctionKernelKind> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::FunctionKernelKind>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO Function Kernel Kind specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::FunctionKernelKind value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::FunctionKernelKind> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::FunctionKernelKind getEmptyKey() {
    return static_cast<::mlir::pto::FunctionKernelKind>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::FunctionKernelKind getTombstoneKey() {
    return static_cast<::mlir::pto::FunctionKernelKind>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::FunctionKernelKind &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::FunctionKernelKind &lhs, const ::mlir::pto::FunctionKernelKind &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO MGATHER out-of-bounds mode
enum class GatherOOB : uint32_t {
  Undefined = 0,
  Clamp = 1,
  Wrap = 2,
  Zero = 3,
};

::std::optional<GatherOOB> symbolizeGatherOOB(uint32_t);
::llvm::StringRef stringifyGatherOOB(GatherOOB);
::std::optional<GatherOOB> symbolizeGatherOOB(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForGatherOOB() {
  return 3;
}


inline ::llvm::StringRef stringifyEnum(GatherOOB enumValue) {
  return stringifyGatherOOB(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<GatherOOB> symbolizeEnum<GatherOOB>(::llvm::StringRef str) {
  return symbolizeGatherOOB(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::GatherOOB, ::mlir::pto::GatherOOB> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::GatherOOB> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO MGATHER out-of-bounds mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::GatherOOB> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::GatherOOB>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO MGATHER out-of-bounds mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::GatherOOB value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::GatherOOB> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::GatherOOB getEmptyKey() {
    return static_cast<::mlir::pto::GatherOOB>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::GatherOOB getTombstoneKey() {
    return static_cast<::mlir::pto::GatherOOB>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::GatherOOB &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::GatherOOB &lhs, const ::mlir::pto::GatherOOB &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO L1 cache control
enum class L1Cache : uint32_t {
  Uncache = 0,
  Cache = 1,
};

::std::optional<L1Cache> symbolizeL1Cache(uint32_t);
::llvm::StringRef stringifyL1Cache(L1Cache);
::std::optional<L1Cache> symbolizeL1Cache(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForL1Cache() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(L1Cache enumValue) {
  return stringifyL1Cache(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<L1Cache> symbolizeEnum<L1Cache>(::llvm::StringRef str) {
  return symbolizeL1Cache(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::L1Cache, ::mlir::pto::L1Cache> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::L1Cache> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO L1 cache control");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::L1Cache> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::L1Cache>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO L1 cache control specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::L1Cache value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::L1Cache> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::L1Cache getEmptyKey() {
    return static_cast<::mlir::pto::L1Cache>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::L1Cache getTombstoneKey() {
    return static_cast<::mlir::pto::L1Cache>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::L1Cache &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::L1Cache &lhs, const ::mlir::pto::L1Cache &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// Global tensor layout (row/col/fractal)
enum class Layout : uint32_t {
  ND = 0,
  DN = 1,
  NZ = 2,
};

::std::optional<Layout> symbolizeLayout(uint32_t);
::llvm::StringRef stringifyLayout(Layout);
::std::optional<Layout> symbolizeLayout(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForLayout() {
  return 2;
}


inline ::llvm::StringRef stringifyEnum(Layout enumValue) {
  return stringifyLayout(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<Layout> symbolizeEnum<Layout>(::llvm::StringRef str) {
  return symbolizeLayout(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::Layout, ::mlir::pto::Layout> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::Layout> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for Global tensor layout (row/col/fractal)");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::Layout> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::Layout>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid Global tensor layout (row/col/fractal) specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::Layout value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::Layout> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::Layout getEmptyKey() {
    return static_cast<::mlir::pto::Layout>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::Layout getTombstoneKey() {
    return static_cast<::mlir::pto::Layout>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::Layout &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::Layout &lhs, const ::mlir::pto::Layout &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO load L2 cache control
enum class LdL2Cache : uint32_t {
  NMFV = 0,
  NMLV = 1,
  NMPRS = 2,
  NMPREF = 3,
  NAKEEP = 4,
  NACLEAN = 5,
  NADROP = 6,
  IDSFV = 8,
  IDSLV = 9,
  IDSPRS = 10,
  IDSPREF = 11,
  EXFV = 12,
  EXLV = 13,
  EXPRS = 14,
  EXPREF = 15,
};

::std::optional<LdL2Cache> symbolizeLdL2Cache(uint32_t);
::llvm::StringRef stringifyLdL2Cache(LdL2Cache);
::std::optional<LdL2Cache> symbolizeLdL2Cache(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForLdL2Cache() {
  return 15;
}


inline ::llvm::StringRef stringifyEnum(LdL2Cache enumValue) {
  return stringifyLdL2Cache(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<LdL2Cache> symbolizeEnum<LdL2Cache>(::llvm::StringRef str) {
  return symbolizeLdL2Cache(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::LdL2Cache, ::mlir::pto::LdL2Cache> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::LdL2Cache> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO load L2 cache control");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::LdL2Cache> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::LdL2Cache>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO load L2 cache control specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::LdL2Cache value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::LdL2Cache> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::LdL2Cache getEmptyKey() {
    return static_cast<::mlir::pto::LdL2Cache>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::LdL2Cache getTombstoneKey() {
    return static_cast<::mlir::pto::LdL2Cache>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::LdL2Cache &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::LdL2Cache &lhs, const ::mlir::pto::LdL2Cache &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO MAD floating exceptional-value saturation mode
enum class MadSatMode : uint32_t {
  Sat = 0,
  NoSat = 1,
};

::std::optional<MadSatMode> symbolizeMadSatMode(uint32_t);
::llvm::StringRef stringifyMadSatMode(MadSatMode);
::std::optional<MadSatMode> symbolizeMadSatMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForMadSatMode() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(MadSatMode enumValue) {
  return stringifyMadSatMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<MadSatMode> symbolizeEnum<MadSatMode>(::llvm::StringRef str) {
  return symbolizeMadSatMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::MadSatMode, ::mlir::pto::MadSatMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::MadSatMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO MAD floating exceptional-value saturation mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::MadSatMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::MadSatMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO MAD floating exceptional-value saturation mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::MadSatMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::MadSatMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::MadSatMode getEmptyKey() {
    return static_cast<::mlir::pto::MadSatMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::MadSatMode getTombstoneKey() {
    return static_cast<::mlir::pto::MadSatMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::MadSatMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::MadSatMode &lhs, const ::mlir::pto::MadSatMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO MAD producer unit-flag mode
enum class MadUnitFlagMode : uint32_t {
  CheckOnly = 0,
  CheckAndSet = 1,
};

::std::optional<MadUnitFlagMode> symbolizeMadUnitFlagMode(uint32_t);
::llvm::StringRef stringifyMadUnitFlagMode(MadUnitFlagMode);
::std::optional<MadUnitFlagMode> symbolizeMadUnitFlagMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForMadUnitFlagMode() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(MadUnitFlagMode enumValue) {
  return stringifyMadUnitFlagMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<MadUnitFlagMode> symbolizeEnum<MadUnitFlagMode>(::llvm::StringRef str) {
  return symbolizeMadUnitFlagMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::MadUnitFlagMode, ::mlir::pto::MadUnitFlagMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::MadUnitFlagMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO MAD producer unit-flag mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::MadUnitFlagMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::MadUnitFlagMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO MAD producer unit-flag mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::MadUnitFlagMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::MadUnitFlagMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::MadUnitFlagMode getEmptyKey() {
    return static_cast<::mlir::pto::MadUnitFlagMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::MadUnitFlagMode getTombstoneKey() {
    return static_cast<::mlir::pto::MadUnitFlagMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::MadUnitFlagMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::MadUnitFlagMode &lhs, const ::mlir::pto::MadUnitFlagMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO mask pattern
enum class MaskPattern : uint32_t {
  P0101 = 1,
  P1010 = 2,
  P0001 = 3,
  P0010 = 4,
  P0100 = 5,
  P1000 = 6,
  P1111 = 7,
};

::std::optional<MaskPattern> symbolizeMaskPattern(uint32_t);
::llvm::StringRef stringifyMaskPattern(MaskPattern);
::std::optional<MaskPattern> symbolizeMaskPattern(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForMaskPattern() {
  return 7;
}


inline ::llvm::StringRef stringifyEnum(MaskPattern enumValue) {
  return stringifyMaskPattern(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<MaskPattern> symbolizeEnum<MaskPattern>(::llvm::StringRef str) {
  return symbolizeMaskPattern(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::MaskPattern, ::mlir::pto::MaskPattern> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::MaskPattern> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO mask pattern");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::MaskPattern> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::MaskPattern>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO mask pattern specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::MaskPattern value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::MaskPattern> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::MaskPattern getEmptyKey() {
    return static_cast<::mlir::pto::MaskPattern>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::MaskPattern getTombstoneKey() {
    return static_cast<::mlir::pto::MaskPattern>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::MaskPattern &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::MaskPattern &lhs, const ::mlir::pto::MaskPattern &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO low-level memory barrier kind
enum class MemBarKind : uint32_t {
  VV_ALL = 0,
  VST_VLD = 1,
  VLD_VST = 2,
  VST_VST = 3,
  VS_ALL = 4,
  VST_LD = 5,
  VLD_ST = 6,
  VST_ST = 7,
  SV_ALL = 8,
  ST_VLD = 9,
  LD_VST = 10,
  ST_VST = 11,
  SS_ALL = 12,
  ST_LD = 13,
  LD_ST = 14,
  ST_ST = 15,
};

::std::optional<MemBarKind> symbolizeMemBarKind(uint32_t);
::llvm::StringRef stringifyMemBarKind(MemBarKind);
::std::optional<MemBarKind> symbolizeMemBarKind(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForMemBarKind() {
  return 15;
}


inline ::llvm::StringRef stringifyEnum(MemBarKind enumValue) {
  return stringifyMemBarKind(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<MemBarKind> symbolizeEnum<MemBarKind>(::llvm::StringRef str) {
  return symbolizeMemBarKind(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::MemBarKind, ::mlir::pto::MemBarKind> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::MemBarKind> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO low-level memory barrier kind");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::MemBarKind> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::MemBarKind>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO low-level memory barrier kind specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::MemBarKind value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::MemBarKind> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::MemBarKind getEmptyKey() {
    return static_cast<::mlir::pto::MemBarKind>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::MemBarKind getTombstoneKey() {
    return static_cast<::mlir::pto::MemBarKind>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::MemBarKind &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::MemBarKind &lhs, const ::mlir::pto::MemBarKind &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// Mem Plan Mode
enum class MemPlanMode : uint32_t {
  LOCAL_MEM_PLAN = 0,
  GLOBAL_WORKSPACE_PLAN = 1,
};

::std::optional<MemPlanMode> symbolizeMemPlanMode(uint32_t);
::llvm::StringRef stringifyMemPlanMode(MemPlanMode);
::std::optional<MemPlanMode> symbolizeMemPlanMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForMemPlanMode() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(MemPlanMode enumValue) {
  return stringifyMemPlanMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<MemPlanMode> symbolizeEnum<MemPlanMode>(::llvm::StringRef str) {
  return symbolizeMemPlanMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::MemPlanMode, ::mlir::pto::MemPlanMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::MemPlanMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for Mem Plan Mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::MemPlanMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::MemPlanMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid Mem Plan Mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::MemPlanMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::MemPlanMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::MemPlanMode getEmptyKey() {
    return static_cast<::mlir::pto::MemPlanMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::MemPlanMode getTombstoneKey() {
    return static_cast<::mlir::pto::MemPlanMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::MemPlanMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::MemPlanMode &lhs, const ::mlir::pto::MemPlanMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO communication notify op
enum class NotifyOp : uint32_t {
  AtomicAdd = 0,
  Set = 1,
};

::std::optional<NotifyOp> symbolizeNotifyOp(uint32_t);
::llvm::StringRef stringifyNotifyOp(NotifyOp);
::std::optional<NotifyOp> symbolizeNotifyOp(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForNotifyOp() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(NotifyOp enumValue) {
  return stringifyNotifyOp(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<NotifyOp> symbolizeEnum<NotifyOp>(::llvm::StringRef str) {
  return symbolizeNotifyOp(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::NotifyOp, ::mlir::pto::NotifyOp> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::NotifyOp> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO communication notify op");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::NotifyOp> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::NotifyOp>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO communication notify op specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::NotifyOp value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::NotifyOp> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::NotifyOp getEmptyKey() {
    return static_cast<::mlir::pto::NotifyOp>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::NotifyOp getTombstoneKey() {
    return static_cast<::mlir::pto::NotifyOp>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::NotifyOp &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::NotifyOp &lhs, const ::mlir::pto::NotifyOp &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// Pad Mode for LoadOp
enum class PadMode : uint32_t {
  PadNull = 0,
  PadFirstElem = 1,
  PadValue = 2,
};

::std::optional<PadMode> symbolizePadMode(uint32_t);
::llvm::StringRef stringifyPadMode(PadMode);
::std::optional<PadMode> symbolizePadMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForPadMode() {
  return 2;
}


inline ::llvm::StringRef stringifyEnum(PadMode enumValue) {
  return stringifyPadMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<PadMode> symbolizeEnum<PadMode>(::llvm::StringRef str) {
  return symbolizePadMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::PadMode, ::mlir::pto::PadMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::PadMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for Pad Mode for LoadOp");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::PadMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::PadMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid Pad Mode for LoadOp specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::PadMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::PadMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::PadMode getEmptyKey() {
    return static_cast<::mlir::pto::PadMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::PadMode getTombstoneKey() {
    return static_cast<::mlir::pto::PadMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::PadMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::PadMode &lhs, const ::mlir::pto::PadMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// Pad value policy
enum class PadValue : uint32_t {
  Null = 0,
  Zero = 1,
  Max = 2,
  Min = 3,
};

::std::optional<PadValue> symbolizePadValue(uint32_t);
::llvm::StringRef stringifyPadValue(PadValue);
::std::optional<PadValue> symbolizePadValue(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForPadValue() {
  return 3;
}


inline ::llvm::StringRef stringifyEnum(PadValue enumValue) {
  return stringifyPadValue(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<PadValue> symbolizeEnum<PadValue>(::llvm::StringRef str) {
  return symbolizePadValue(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::PadValue, ::mlir::pto::PadValue> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::PadValue> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for Pad value policy");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::PadValue> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::PadValue>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid Pad value policy specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::PadValue value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::PadValue> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::PadValue getEmptyKey() {
    return static_cast<::mlir::pto::PadValue>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::PadValue getTombstoneKey() {
    return static_cast<::mlir::pto::PadValue>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::PadValue &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::PadValue &lhs, const ::mlir::pto::PadValue &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO Op Pipe
enum class PIPE : uint32_t {
  PIPE_S = 0,
  PIPE_V = 1,
  PIPE_M = 2,
  PIPE_MTE1 = 3,
  PIPE_MTE2 = 4,
  PIPE_MTE3 = 5,
  PIPE_ALL = 6,
  PIPE_MTE4 = 7,
  PIPE_MTE5 = 8,
  PIPE_V2 = 9,
  PIPE_FIX = 10,
  VIRTUAL_PIPE_MTE2_L1A = 11,
  VIRTUAL_PIPE_MTE2_L1B = 12,
  PIPE_NUM = 13,
  PIPE_UNASSIGNED = 99,
};

::std::optional<PIPE> symbolizePIPE(uint32_t);
::llvm::StringRef stringifyPIPE(PIPE);
::std::optional<PIPE> symbolizePIPE(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForPIPE() {
  return 99;
}


inline ::llvm::StringRef stringifyEnum(PIPE enumValue) {
  return stringifyPIPE(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<PIPE> symbolizeEnum<PIPE>(::llvm::StringRef str) {
  return symbolizePIPE(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::PIPE, ::mlir::pto::PIPE> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::PIPE> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO Op Pipe");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::PIPE> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::PIPE>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO Op Pipe specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::PIPE value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::PIPE> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::PIPE getEmptyKey() {
    return static_cast<::mlir::pto::PIPE>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::PIPE getTombstoneKey() {
    return static_cast<::mlir::pto::PIPE>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::PIPE &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::PIPE &lhs, const ::mlir::pto::PIPE &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO precision mode for elementwise math ops
enum class PrecisionMode : uint32_t {
  DEFAULT = 0,
  HIGH_PRECISION = 1,
};

::std::optional<PrecisionMode> symbolizePrecisionMode(uint32_t);
::llvm::StringRef stringifyPrecisionMode(PrecisionMode);
::std::optional<PrecisionMode> symbolizePrecisionMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForPrecisionMode() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(PrecisionMode enumValue) {
  return stringifyPrecisionMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<PrecisionMode> symbolizeEnum<PrecisionMode>(::llvm::StringRef str) {
  return symbolizePrecisionMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::PrecisionMode, ::mlir::pto::PrecisionMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::PrecisionMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO precision mode for elementwise math ops");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::PrecisionMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::PrecisionMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO precision mode for elementwise math ops specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::PrecisionMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::PrecisionMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::PrecisionMode getEmptyKey() {
    return static_cast<::mlir::pto::PrecisionMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::PrecisionMode getTombstoneKey() {
    return static_cast<::mlir::pto::PrecisionMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::PrecisionMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::PrecisionMode &lhs, const ::mlir::pto::PrecisionMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO quantization type
enum class QuantType : uint32_t {
  INT8_SYM = 0,
  INT8_ASYM = 1,
};

::std::optional<QuantType> symbolizeQuantType(uint32_t);
::llvm::StringRef stringifyQuantType(QuantType);
::std::optional<QuantType> symbolizeQuantType(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForQuantType() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(QuantType enumValue) {
  return stringifyQuantType(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<QuantType> symbolizeEnum<QuantType>(::llvm::StringRef str) {
  return symbolizeQuantType(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::QuantType, ::mlir::pto::QuantType> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::QuantType> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO quantization type");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::QuantType> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::QuantType>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO quantization type specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::QuantType value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::QuantType> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::QuantType getEmptyKey() {
    return static_cast<::mlir::pto::QuantType>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::QuantType getTombstoneKey() {
    return static_cast<::mlir::pto::QuantType>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::QuantType &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::QuantType &lhs, const ::mlir::pto::QuantType &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO communication reduce operation
enum class ReduceOp : uint32_t {
  Sum = 0,
  Max = 1,
  Min = 2,
};

::std::optional<ReduceOp> symbolizeReduceOp(uint32_t);
::llvm::StringRef stringifyReduceOp(ReduceOp);
::std::optional<ReduceOp> symbolizeReduceOp(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForReduceOp() {
  return 2;
}


inline ::llvm::StringRef stringifyEnum(ReduceOp enumValue) {
  return stringifyReduceOp(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<ReduceOp> symbolizeEnum<ReduceOp>(::llvm::StringRef str) {
  return symbolizeReduceOp(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::ReduceOp, ::mlir::pto::ReduceOp> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::ReduceOp> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO communication reduce operation");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::ReduceOp> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::ReduceOp>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO communication reduce operation specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::ReduceOp value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::ReduceOp> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::ReduceOp getEmptyKey() {
    return static_cast<::mlir::pto::ReduceOp>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::ReduceOp getTombstoneKey() {
    return static_cast<::mlir::pto::ReduceOp>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::ReduceOp &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::ReduceOp &lhs, const ::mlir::pto::ReduceOp &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO TSTORE relu pre mode
enum class ReluPreMode : uint32_t {
  NoRelu = 0,
  NormalRelu = 1,
  ScalarRelu = 2,
  VectorRelu = 3,
  Pwl = 4,
};

::std::optional<ReluPreMode> symbolizeReluPreMode(uint32_t);
::llvm::StringRef stringifyReluPreMode(ReluPreMode);
::std::optional<ReluPreMode> symbolizeReluPreMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForReluPreMode() {
  return 4;
}


inline ::llvm::StringRef stringifyEnum(ReluPreMode enumValue) {
  return stringifyReluPreMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<ReluPreMode> symbolizeEnum<ReluPreMode>(::llvm::StringRef str) {
  return symbolizeReluPreMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::ReluPreMode, ::mlir::pto::ReluPreMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::ReluPreMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO TSTORE relu pre mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::ReluPreMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::ReluPreMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO TSTORE relu pre mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::ReluPreMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::ReluPreMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::ReluPreMode getEmptyKey() {
    return static_cast<::mlir::pto::ReluPreMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::ReluPreMode getTombstoneKey() {
    return static_cast<::mlir::pto::ReluPreMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::ReluPreMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::ReluPreMode &lhs, const ::mlir::pto::ReluPreMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO rounding mode
enum class RoundMode : uint32_t {
  NONE = 0,
  RINT = 1,
  ROUND = 2,
  FLOOR = 3,
  CEIL = 4,
  TRUNC = 5,
  ODD = 6,
  CAST_RINT = 7,
};

::std::optional<RoundMode> symbolizeRoundMode(uint32_t);
::llvm::StringRef stringifyRoundMode(RoundMode);
::std::optional<RoundMode> symbolizeRoundMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForRoundMode() {
  return 7;
}


inline ::llvm::StringRef stringifyEnum(RoundMode enumValue) {
  return stringifyRoundMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<RoundMode> symbolizeEnum<RoundMode>(::llvm::StringRef str) {
  return symbolizeRoundMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::RoundMode, ::mlir::pto::RoundMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::RoundMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO rounding mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::RoundMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::RoundMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO rounding mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::RoundMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::RoundMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::RoundMode getEmptyKey() {
    return static_cast<::mlir::pto::RoundMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::RoundMode getTombstoneKey() {
    return static_cast<::mlir::pto::RoundMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::RoundMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::RoundMode &lhs, const ::mlir::pto::RoundMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO SIMT conversion rounding selector
enum class Rounding : uint32_t {
  R = 0,
  A = 1,
  F = 2,
  C = 3,
  Z = 4,
  O = 5,
  H = 6,
};

::std::optional<Rounding> symbolizeRounding(uint32_t);
::llvm::StringRef stringifyRounding(Rounding);
::std::optional<Rounding> symbolizeRounding(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForRounding() {
  return 6;
}


inline ::llvm::StringRef stringifyEnum(Rounding enumValue) {
  return stringifyRounding(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<Rounding> symbolizeEnum<Rounding>(::llvm::StringRef str) {
  return symbolizeRounding(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::Rounding, ::mlir::pto::Rounding> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::Rounding> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO SIMT conversion rounding selector");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::Rounding> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::Rounding>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO SIMT conversion rounding selector specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::Rounding value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::Rounding> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::Rounding getEmptyKey() {
    return static_cast<::mlir::pto::Rounding>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::Rounding getTombstoneKey() {
    return static_cast<::mlir::pto::Rounding>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::Rounding &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::Rounding &lhs, const ::mlir::pto::Rounding &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// Secondary layout
enum class SLayout : uint32_t {
  NoneBox = 0,
  RowMajor = 1,
  ColMajor = 2,
};

::std::optional<SLayout> symbolizeSLayout(uint32_t);
::llvm::StringRef stringifySLayout(SLayout);
::std::optional<SLayout> symbolizeSLayout(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForSLayout() {
  return 2;
}


inline ::llvm::StringRef stringifyEnum(SLayout enumValue) {
  return stringifySLayout(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<SLayout> symbolizeEnum<SLayout>(::llvm::StringRef str) {
  return symbolizeSLayout(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::SLayout, ::mlir::pto::SLayout> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::SLayout> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for Secondary layout");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::SLayout> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::SLayout>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid Secondary layout specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::SLayout value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::SLayout> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::SLayout getEmptyKey() {
    return static_cast<::mlir::pto::SLayout>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::SLayout getTombstoneKey() {
    return static_cast<::mlir::pto::SLayout>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::SLayout &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::SLayout &lhs, const ::mlir::pto::SLayout &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO TSTORE store phase
enum class STPhase : uint32_t {
  Unspecified = 0,
  Partial = 2,
  Final = 3,
};

::std::optional<STPhase> symbolizeSTPhase(uint32_t);
::llvm::StringRef stringifySTPhase(STPhase);
::std::optional<STPhase> symbolizeSTPhase(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForSTPhase() {
  return 3;
}


inline ::llvm::StringRef stringifyEnum(STPhase enumValue) {
  return stringifySTPhase(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<STPhase> symbolizeEnum<STPhase>(::llvm::StringRef str) {
  return symbolizeSTPhase(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::STPhase, ::mlir::pto::STPhase> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::STPhase> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO TSTORE store phase");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::STPhase> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::STPhase>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO TSTORE store phase specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::STPhase value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::STPhase> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::STPhase getEmptyKey() {
    return static_cast<::mlir::pto::STPhase>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::STPhase getTombstoneKey() {
    return static_cast<::mlir::pto::STPhase>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::STPhase &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::STPhase &lhs, const ::mlir::pto::STPhase &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO SIMT conversion saturation selector
enum class Saturation : uint32_t {
  Disable = 0,
  Enable = 1,
};

::std::optional<Saturation> symbolizeSaturation(uint32_t);
::llvm::StringRef stringifySaturation(Saturation);
::std::optional<Saturation> symbolizeSaturation(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForSaturation() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(Saturation enumValue) {
  return stringifySaturation(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<Saturation> symbolizeEnum<Saturation>(::llvm::StringRef str) {
  return symbolizeSaturation(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::Saturation, ::mlir::pto::Saturation> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::Saturation> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO SIMT conversion saturation selector");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::Saturation> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::Saturation>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO SIMT conversion saturation selector specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::Saturation value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::Saturation> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::Saturation getEmptyKey() {
    return static_cast<::mlir::pto::Saturation>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::Saturation getTombstoneKey() {
    return static_cast<::mlir::pto::Saturation>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::Saturation &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::Saturation &lhs, const ::mlir::pto::Saturation &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO saturation mode
enum class SaturationMode : uint32_t {
  ON = 0,
  OFF = 1,
};

::std::optional<SaturationMode> symbolizeSaturationMode(uint32_t);
::llvm::StringRef stringifySaturationMode(SaturationMode);
::std::optional<SaturationMode> symbolizeSaturationMode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForSaturationMode() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(SaturationMode enumValue) {
  return stringifySaturationMode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<SaturationMode> symbolizeEnum<SaturationMode>(::llvm::StringRef str) {
  return symbolizeSaturationMode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::SaturationMode, ::mlir::pto::SaturationMode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::SaturationMode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO saturation mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::SaturationMode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::SaturationMode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO saturation mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::SaturationMode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::SaturationMode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::SaturationMode getEmptyKey() {
    return static_cast<::mlir::pto::SaturationMode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::SaturationMode getTombstoneKey() {
    return static_cast<::mlir::pto::SaturationMode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::SaturationMode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::SaturationMode &lhs, const ::mlir::pto::SaturationMode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO MSCATTER atomic mode
enum class ScatterAtomicOp : uint32_t {
  None = 0,
  Add = 1,
  Max = 2,
  Min = 3,
};

::std::optional<ScatterAtomicOp> symbolizeScatterAtomicOp(uint32_t);
::llvm::StringRef stringifyScatterAtomicOp(ScatterAtomicOp);
::std::optional<ScatterAtomicOp> symbolizeScatterAtomicOp(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForScatterAtomicOp() {
  return 3;
}


inline ::llvm::StringRef stringifyEnum(ScatterAtomicOp enumValue) {
  return stringifyScatterAtomicOp(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<ScatterAtomicOp> symbolizeEnum<ScatterAtomicOp>(::llvm::StringRef str) {
  return symbolizeScatterAtomicOp(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::ScatterAtomicOp, ::mlir::pto::ScatterAtomicOp> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::ScatterAtomicOp> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO MSCATTER atomic mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::ScatterAtomicOp> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::ScatterAtomicOp>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO MSCATTER atomic mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::ScatterAtomicOp value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::ScatterAtomicOp> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::ScatterAtomicOp getEmptyKey() {
    return static_cast<::mlir::pto::ScatterAtomicOp>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::ScatterAtomicOp getTombstoneKey() {
    return static_cast<::mlir::pto::ScatterAtomicOp>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::ScatterAtomicOp &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::ScatterAtomicOp &lhs, const ::mlir::pto::ScatterAtomicOp &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO MSCATTER out-of-bounds mode
enum class ScatterOOB : uint32_t {
  Undefined = 0,
  Skip = 1,
  Clamp = 2,
  Wrap = 3,
};

::std::optional<ScatterOOB> symbolizeScatterOOB(uint32_t);
::llvm::StringRef stringifyScatterOOB(ScatterOOB);
::std::optional<ScatterOOB> symbolizeScatterOOB(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForScatterOOB() {
  return 3;
}


inline ::llvm::StringRef stringifyEnum(ScatterOOB enumValue) {
  return stringifyScatterOOB(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<ScatterOOB> symbolizeEnum<ScatterOOB>(::llvm::StringRef str) {
  return symbolizeScatterOOB(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::ScatterOOB, ::mlir::pto::ScatterOOB> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::ScatterOOB> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO MSCATTER out-of-bounds mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::ScatterOOB> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::ScatterOOB>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO MSCATTER out-of-bounds mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::ScatterOOB value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::ScatterOOB> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::ScatterOOB getEmptyKey() {
    return static_cast<::mlir::pto::ScatterOOB>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::ScatterOOB getTombstoneKey() {
    return static_cast<::mlir::pto::ScatterOOB>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::ScatterOOB &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::ScatterOOB &lhs, const ::mlir::pto::ScatterOOB &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO integer signedness
enum class Signedness : uint32_t {
  Signed = 0,
  Unsigned = 1,
};

::std::optional<Signedness> symbolizeSignedness(uint32_t);
::llvm::StringRef stringifySignedness(Signedness);
::std::optional<Signedness> symbolizeSignedness(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForSignedness() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(Signedness enumValue) {
  return stringifySignedness(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<Signedness> symbolizeEnum<Signedness>(::llvm::StringRef str) {
  return symbolizeSignedness(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::Signedness, ::mlir::pto::Signedness> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::Signedness> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO integer signedness");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::Signedness> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::Signedness>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO integer signedness specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::Signedness value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::Signedness> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::Signedness getEmptyKey() {
    return static_cast<::mlir::pto::Signedness>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::Signedness getTombstoneKey() {
    return static_cast<::mlir::pto::Signedness>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::Signedness &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::Signedness &lhs, const ::mlir::pto::Signedness &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO store/atomic L2 cache control
enum class StL2Cache : uint32_t {
  NMFV = 0,
  NMLV = 1,
  NMPRS = 2,
  NMRED = 3,
  NACI = 4,
  NAPW = 5,
  NAPI = 6,
  NARED = 7,
  WBHFV = 8,
  WBHLV = 9,
  WBHPRS = 10,
  WBHRED = 11,
  WTSFV = 12,
  WTSLV = 13,
  WTSPRS = 14,
  WTSRED = 15,
};

::std::optional<StL2Cache> symbolizeStL2Cache(uint32_t);
::llvm::StringRef stringifyStL2Cache(StL2Cache);
::std::optional<StL2Cache> symbolizeStL2Cache(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForStL2Cache() {
  return 15;
}


inline ::llvm::StringRef stringifyEnum(StL2Cache enumValue) {
  return stringifyStL2Cache(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<StL2Cache> symbolizeEnum<StL2Cache>(::llvm::StringRef str) {
  return symbolizeStL2Cache(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::StL2Cache, ::mlir::pto::StL2Cache> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::StL2Cache> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO store/atomic L2 cache control");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::StL2Cache> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::StL2Cache>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO store/atomic L2 cache control specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::StL2Cache value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::StL2Cache> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::StL2Cache getEmptyKey() {
    return static_cast<::mlir::pto::StL2Cache>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::StL2Cache getTombstoneKey() {
    return static_cast<::mlir::pto::StL2Cache>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::StL2Cache &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::StL2Cache &lhs, const ::mlir::pto::StL2Cache &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO High Level Sync Op Type
enum class SyncOpType : uint32_t {
  TLOAD = 0,
  TSTORE_ACC = 1,
  TSTORE_VEC = 2,
  TMOV_M2L = 3,
  TMOV_M2S = 4,
  TMOV_M2B = 5,
  TMOV_M2V = 6,
  TMOV_V2M = 7,
  TMATMUL = 8,
  TVEC = 9,
  TVECWAIT_EVENT = 10,
};

::std::optional<SyncOpType> symbolizeSyncOpType(uint32_t);
::llvm::StringRef stringifySyncOpType(SyncOpType);
::std::optional<SyncOpType> symbolizeSyncOpType(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForSyncOpType() {
  return 10;
}


inline ::llvm::StringRef stringifyEnum(SyncOpType enumValue) {
  return stringifySyncOpType(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<SyncOpType> symbolizeEnum<SyncOpType>(::llvm::StringRef str) {
  return symbolizeSyncOpType(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::SyncOpType, ::mlir::pto::SyncOpType> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::SyncOpType> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO High Level Sync Op Type");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::SyncOpType> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::SyncOpType>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO High Level Sync Op Type specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::SyncOpType value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::SyncOpType> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::SyncOpType getEmptyKey() {
    return static_cast<::mlir::pto::SyncOpType>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::SyncOpType getTombstoneKey() {
    return static_cast<::mlir::pto::SyncOpType>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::SyncOpType &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::SyncOpType &lhs, const ::mlir::pto::SyncOpType &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO Function Core Type
enum class TFuncCoreType : uint32_t {
  AIC = 1,
  AIV = 2,
  MIX = 3,
  AIC_OR_AIV = 4,
};

::std::optional<TFuncCoreType> symbolizeTFuncCoreType(uint32_t);
::llvm::StringRef stringifyTFuncCoreType(TFuncCoreType);
::std::optional<TFuncCoreType> symbolizeTFuncCoreType(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForTFuncCoreType() {
  return 4;
}


inline ::llvm::StringRef stringifyEnum(TFuncCoreType enumValue) {
  return stringifyTFuncCoreType(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<TFuncCoreType> symbolizeEnum<TFuncCoreType>(::llvm::StringRef str) {
  return symbolizeTFuncCoreType(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::TFuncCoreType, ::mlir::pto::TFuncCoreType> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::TFuncCoreType> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO Function Core Type");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::TFuncCoreType> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::TFuncCoreType>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO Function Core Type specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::TFuncCoreType value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::TFuncCoreType> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::TFuncCoreType getEmptyKey() {
    return static_cast<::mlir::pto::TFuncCoreType>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::TFuncCoreType getTombstoneKey() {
    return static_cast<::mlir::pto::TFuncCoreType>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::TFuncCoreType &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::TFuncCoreType &lhs, const ::mlir::pto::TFuncCoreType &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO Module Core Type
enum class TModuleCoreType : uint32_t {
  AIC = 1,
  AIV = 2,
  MIX = 3,
};

::std::optional<TModuleCoreType> symbolizeTModuleCoreType(uint32_t);
::llvm::StringRef stringifyTModuleCoreType(TModuleCoreType);
::std::optional<TModuleCoreType> symbolizeTModuleCoreType(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForTModuleCoreType() {
  return 3;
}


inline ::llvm::StringRef stringifyEnum(TModuleCoreType enumValue) {
  return stringifyTModuleCoreType(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<TModuleCoreType> symbolizeEnum<TModuleCoreType>(::llvm::StringRef str) {
  return symbolizeTModuleCoreType(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::TModuleCoreType, ::mlir::pto::TModuleCoreType> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::TModuleCoreType> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO Module Core Type");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::TModuleCoreType> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::TModuleCoreType>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO Module Core Type specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::TModuleCoreType value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::TModuleCoreType> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::TModuleCoreType getEmptyKey() {
    return static_cast<::mlir::pto::TModuleCoreType>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::TModuleCoreType getTombstoneKey() {
    return static_cast<::mlir::pto::TModuleCoreType>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::TModuleCoreType &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::TModuleCoreType &lhs, const ::mlir::pto::TModuleCoreType &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO MAD TF32 rounding mode
enum class Tf32Mode : uint32_t {
  RoundEven = 0,
  RoundAway = 1,
};

::std::optional<Tf32Mode> symbolizeTf32Mode(uint32_t);
::llvm::StringRef stringifyTf32Mode(Tf32Mode);
::std::optional<Tf32Mode> symbolizeTf32Mode(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForTf32Mode() {
  return 1;
}


inline ::llvm::StringRef stringifyEnum(Tf32Mode enumValue) {
  return stringifyTf32Mode(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<Tf32Mode> symbolizeEnum<Tf32Mode>(::llvm::StringRef str) {
  return symbolizeTf32Mode(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::Tf32Mode, ::mlir::pto::Tf32Mode> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::Tf32Mode> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO MAD TF32 rounding mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::Tf32Mode> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::Tf32Mode>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO MAD TF32 rounding mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::Tf32Mode value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::Tf32Mode> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::Tf32Mode getEmptyKey() {
    return static_cast<::mlir::pto::Tf32Mode>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::Tf32Mode getTombstoneKey() {
    return static_cast<::mlir::pto::Tf32Mode>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::Tf32Mode &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::Tf32Mode &lhs, const ::mlir::pto::Tf32Mode &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO accumulator store unit-flag mode
enum class AccStoreUnitFlagCtrl : uint32_t {
  Off = 0,
  CheckOnly = 2,
  CheckAndClear = 3,
};

::std::optional<AccStoreUnitFlagCtrl> symbolizeAccStoreUnitFlagCtrl(uint32_t);
::llvm::StringRef stringifyAccStoreUnitFlagCtrl(AccStoreUnitFlagCtrl);
::std::optional<AccStoreUnitFlagCtrl> symbolizeAccStoreUnitFlagCtrl(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForAccStoreUnitFlagCtrl() {
  return 3;
}


inline ::llvm::StringRef stringifyEnum(AccStoreUnitFlagCtrl enumValue) {
  return stringifyAccStoreUnitFlagCtrl(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<AccStoreUnitFlagCtrl> symbolizeEnum<AccStoreUnitFlagCtrl>(::llvm::StringRef str) {
  return symbolizeAccStoreUnitFlagCtrl(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::AccStoreUnitFlagCtrl, ::mlir::pto::AccStoreUnitFlagCtrl> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::AccStoreUnitFlagCtrl> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO accumulator store unit-flag mode");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::AccStoreUnitFlagCtrl> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::AccStoreUnitFlagCtrl>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO accumulator store unit-flag mode specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::AccStoreUnitFlagCtrl value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::AccStoreUnitFlagCtrl> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::AccStoreUnitFlagCtrl getEmptyKey() {
    return static_cast<::mlir::pto::AccStoreUnitFlagCtrl>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::AccStoreUnitFlagCtrl getTombstoneKey() {
    return static_cast<::mlir::pto::AccStoreUnitFlagCtrl>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::AccStoreUnitFlagCtrl &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::AccStoreUnitFlagCtrl &lhs, const ::mlir::pto::AccStoreUnitFlagCtrl &rhs) {
    return lhs == rhs;
  }
};
}

namespace mlir {
namespace pto {
// PTO communication wait/test compare
enum class WaitCmp : uint32_t {
  EQ = 0,
  NE = 1,
  GT = 2,
  GE = 3,
  LT = 4,
  LE = 5,
};

::std::optional<WaitCmp> symbolizeWaitCmp(uint32_t);
::llvm::StringRef stringifyWaitCmp(WaitCmp);
::std::optional<WaitCmp> symbolizeWaitCmp(::llvm::StringRef);
inline constexpr unsigned getMaxEnumValForWaitCmp() {
  return 5;
}


inline ::llvm::StringRef stringifyEnum(WaitCmp enumValue) {
  return stringifyWaitCmp(enumValue);
}

template <typename EnumType>
::std::optional<EnumType> symbolizeEnum(::llvm::StringRef);

template <>
inline ::std::optional<WaitCmp> symbolizeEnum<WaitCmp>(::llvm::StringRef str) {
  return symbolizeWaitCmp(str);
}
} // namespace pto
} // namespace mlir

namespace mlir {
template <typename T, typename>
struct FieldParser;

template<>
struct FieldParser<::mlir::pto::WaitCmp, ::mlir::pto::WaitCmp> {
  template <typename ParserT>
  static FailureOr<::mlir::pto::WaitCmp> parse(ParserT &parser) {
    // Parse the keyword/string containing the enum.
    std::string enumKeyword;
    auto loc = parser.getCurrentLocation();
    if (failed(parser.parseOptionalKeywordOrString(&enumKeyword)))
      return parser.emitError(loc, "expected keyword for PTO communication wait/test compare");

    // Symbolize the keyword.
    if (::std::optional<::mlir::pto::WaitCmp> attr = ::mlir::pto::symbolizeEnum<::mlir::pto::WaitCmp>(enumKeyword))
      return *attr;
    return parser.emitError(loc, "invalid PTO communication wait/test compare specification: ") << enumKeyword;
  }
};
} // namespace mlir

namespace llvm {
inline ::llvm::raw_ostream &operator<<(::llvm::raw_ostream &p, ::mlir::pto::WaitCmp value) {
  auto valueStr = stringifyEnum(value);
  return p << valueStr;
}
} // namespace llvm

namespace llvm {
template<> struct DenseMapInfo<::mlir::pto::WaitCmp> {
  using StorageInfo = ::llvm::DenseMapInfo<uint32_t>;

  static inline ::mlir::pto::WaitCmp getEmptyKey() {
    return static_cast<::mlir::pto::WaitCmp>(StorageInfo::getEmptyKey());
  }

  static inline ::mlir::pto::WaitCmp getTombstoneKey() {
    return static_cast<::mlir::pto::WaitCmp>(StorageInfo::getTombstoneKey());
  }

  static unsigned getHashValue(const ::mlir::pto::WaitCmp &val) {
    return StorageInfo::getHashValue(static_cast<uint32_t>(val));
  }

  static bool isEqual(const ::mlir::pto::WaitCmp &lhs, const ::mlir::pto::WaitCmp &rhs) {
    return lhs == rhs;
  }
};
}

