/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* TypeDef Declarations                                                       *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifdef GET_TYPEDEF_CLASSES
#undef GET_TYPEDEF_CLASSES


namespace mlir {
class AsmParser;
class AsmPrinter;
} // namespace mlir
namespace mlir {
namespace pto {
class PtrType;
class TensorViewType;
class PartitionTensorViewType;
class TileType;
class TileBufType;
class EventIdArrayType;
class LocalArrayType;
class PipeType;
class AsyncSessionType;
class AsyncEventType;
class HiF8Type;
class HiF8x2Type;
class F4E1M2x2Type;
class F4E2M1x2Type;
class VRegType;
class MaskType;
class AlignType;
namespace detail {
struct PtrTypeStorage;
} // namespace detail
class PtrType : public ::mlir::Type::TypeBase<PtrType, ::mlir::Type, detail::PtrTypeStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.ptr";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static PtrType get(::mlir::MLIRContext *context, Type elementType);
  static PtrType get(::mlir::MLIRContext *context, Type elementType, mlir::pto::AddressSpaceAttr memorySpace);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"ptr"};
  }

  static ::mlir::Type parse(::mlir::AsmParser &odsParser);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  mlir::Type getElementType() const;
  mlir::pto::AddressSpaceAttr getMemorySpace() const;
};
namespace detail {
struct TensorViewTypeStorage;
} // namespace detail
class TensorViewType : public ::mlir::Type::TypeBase<TensorViewType, ::mlir::Type, detail::TensorViewTypeStorage> {
public:
  using Base::Base;
  int64_t getRank() const { return getShape().size(); }
  int64_t getDimSize(unsigned idx) const { return getShape()[idx]; }
  int64_t getNumElements() const {
    int64_t num = 1;
    for (int64_t dim : getShape()) {
      if (dim == mlir::ShapedType::kDynamic) return mlir::ShapedType::kDynamic;
      num *= dim;
    }
    return num;
  }
  static constexpr ::llvm::StringLiteral name = "pto.tensor_view";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static TensorViewType get(::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType);
  static TensorViewType get(::mlir::MLIRContext *context, int64_t rank, Type elementType);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"tensor_view"};
  }

  static ::mlir::Type parse(::mlir::AsmParser &odsParser);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::llvm::ArrayRef<int64_t> getShape() const;
  mlir::Type getElementType() const;
};
namespace detail {
struct PartitionTensorViewTypeStorage;
} // namespace detail
class PartitionTensorViewType : public ::mlir::Type::TypeBase<PartitionTensorViewType, ::mlir::Type, detail::PartitionTensorViewTypeStorage> {
public:
  using Base::Base;
  int64_t getRank() const { return getShape().size(); }
  int64_t getDimSize(unsigned idx) const { return getShape()[idx]; }

  int64_t getNumElements() const {
    int64_t num = 1;
    for (int64_t dim : getShape()) {
      if (dim == mlir::ShapedType::kDynamic) return mlir::ShapedType::kDynamic;
      num *= dim;
    }
    return num;
  }
  static constexpr ::llvm::StringLiteral name = "pto.partition_tensor_view";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static PartitionTensorViewType get(::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType);
  static PartitionTensorViewType get(::mlir::MLIRContext *context, int64_t rank, Type elementType);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"partition_tensor_view"};
  }

  static ::mlir::Type parse(::mlir::AsmParser &odsParser);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::llvm::ArrayRef<int64_t> getShape() const;
  mlir::Type getElementType() const;
};
namespace detail {
struct TileTypeStorage;
} // namespace detail
class TileType : public ::mlir::Type::TypeBase<TileType, ::mlir::Type, detail::TileTypeStorage> {
public:
  using Base::Base;
  int64_t getRank() const { return getShape().size(); }
  int64_t getDimSize(unsigned idx) const { return getShape()[idx]; }
  int64_t getNumElements() const {
    int64_t num = 1;
    for (int64_t dim : getShape()) num *= dim;
    return num;
  }
  static constexpr ::llvm::StringLiteral name = "pto.tile";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static TileType get(::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"tile"};
  }

  static ::mlir::Type parse(::mlir::AsmParser &odsParser);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::llvm::ArrayRef<int64_t> getShape() const;
  mlir::Type getElementType() const;
};
namespace detail {
struct TileBufTypeStorage;
} // namespace detail
class TileBufType : public ::mlir::Type::TypeBase<TileBufType, ::mlir::Type, detail::TileBufTypeStorage> {
public:
  using Base::Base;
  int64_t getRank() const { return getShape().size(); }

  bool hasValidShape() const { return !getValidShape().empty(); }
  bool hasDynamicValid() const {
    auto vs = getValidShape();
    return vs.size()==2 && vs[0] < 0 && vs[1] < 0;
  }

  mlir::pto::TileBufConfigAttr getConfigAttr() const;
  bool hasNonDefaultConfig() const;

  // ✅ 返回强类型 Attr，而不是 enum
  mlir::Attribute getBLayoutAttr() const;
  mlir::Attribute getSLayoutAttr() const;
  int32_t getSFractalSizeI32() const;
  mlir::Attribute getPadValueAttr() const;
  mlir::Attribute getCompactModeAttr() const;

  // 如果你仍然想要“数值枚举”，就提供 int getter（不会依赖 enum 类型）
  int32_t getBLayoutValueI32() const;   // 0 row_major, 1 col_major
  int32_t getSLayoutValueI32() const;   // 0 none_box, 1 row_major, 2 col_major
  int32_t getPadValueI32() const;       // 0 null, 1 zero, 2 max, 3 min
  int32_t getCompactModeI32() const;    // 0 null, 1 normal, 2 row_plus_one
  static constexpr ::llvm::StringLiteral name = "pto.tile_buf";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static TileBufType get(::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType, mlir::Attribute memorySpace, ::llvm::ArrayRef<int64_t> validShape, mlir::pto::TileBufConfigAttr config);
  static TileBufType get(::mlir::MLIRContext *context, ArrayRef<int64_t> shape, Type elementType, Attribute memorySpace);
  static TileBufType get(::mlir::MLIRContext *context, ArrayRef<int64_t> shape, Type elementType, Attribute memorySpace, mlir::pto::TileBufConfigAttr config);
  static TileBufType get(::mlir::MLIRContext *context, ArrayRef<int64_t> shape, Type elementType, Attribute memorySpace, ArrayRef<int64_t> validShape);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"tile_buf"};
  }

  static ::mlir::Type parse(::mlir::AsmParser &odsParser);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::llvm::ArrayRef<int64_t> getShape() const;
  mlir::Type getElementType() const;
  mlir::Attribute getMemorySpace() const;
  ::llvm::ArrayRef<int64_t> getValidShape() const;
  mlir::pto::TileBufConfigAttr getConfig() const;
};
namespace detail {
struct EventIdArrayTypeStorage;
} // namespace detail
class EventIdArrayType : public ::mlir::Type::TypeBase<EventIdArrayType, ::mlir::Type, detail::EventIdArrayTypeStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.eventid_array";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static EventIdArrayType get(::mlir::MLIRContext *context, int64_t size);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"eventid_array"};
  }

  static ::mlir::Type parse(::mlir::AsmParser &odsParser);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  int64_t getSize() const;
};
namespace detail {
struct LocalArrayTypeStorage;
} // namespace detail
class LocalArrayType : public ::mlir::Type::TypeBase<LocalArrayType, ::mlir::Type, detail::LocalArrayTypeStorage> {
public:
  using Base::Base;
  int64_t getRank() const { return getShape().size(); }
  int64_t getNumElements() const {
    int64_t num = 1;
    for (int64_t d : getShape()) num *= d;
    return num;
  }
  static constexpr ::llvm::StringLiteral name = "pto.local_array";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  using Base::getChecked;
  static LocalArrayType get(::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType);
  static LocalArrayType getChecked(::llvm::function_ref<::mlir::InFlightDiagnostic()> emitError, ::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType);
  static ::llvm::LogicalResult verify(::llvm::function_ref<::mlir::InFlightDiagnostic()> emitError, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"local_array"};
  }

  static ::mlir::Type parse(::mlir::AsmParser &odsParser);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::llvm::ArrayRef<int64_t> getShape() const;
  mlir::Type getElementType() const;
};
class PipeType : public ::mlir::Type::TypeBase<PipeType, ::mlir::Type, ::mlir::TypeStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.pipe";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"pipe"};
  }

};
class AsyncSessionType : public ::mlir::Type::TypeBase<AsyncSessionType, ::mlir::Type, ::mlir::TypeStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.async_session";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"async_session"};
  }

};
class AsyncEventType : public ::mlir::Type::TypeBase<AsyncEventType, ::mlir::Type, ::mlir::TypeStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.async_event";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"async_event"};
  }

};
class HiF8Type : public ::mlir::Type::TypeBase<HiF8Type, ::mlir::Type, ::mlir::TypeStorage, ::mlir::DataLayoutTypeInterface::Trait> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.hif8";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"hif8"};
  }

  ::llvm::TypeSize getTypeSizeInBits(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
  uint64_t getABIAlignment(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
  uint64_t getPreferredAlignment(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
};
class HiF8x2Type : public ::mlir::Type::TypeBase<HiF8x2Type, ::mlir::Type, ::mlir::TypeStorage, ::mlir::DataLayoutTypeInterface::Trait> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.hif8x2";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"hif8x2"};
  }

  ::llvm::TypeSize getTypeSizeInBits(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
  uint64_t getABIAlignment(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
  uint64_t getPreferredAlignment(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
};
class F4E1M2x2Type : public ::mlir::Type::TypeBase<F4E1M2x2Type, ::mlir::Type, ::mlir::TypeStorage, ::mlir::DataLayoutTypeInterface::Trait> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.f4E1M2x2";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"f4E1M2x2"};
  }

  ::llvm::TypeSize getTypeSizeInBits(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
  uint64_t getABIAlignment(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
  uint64_t getPreferredAlignment(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
};
class F4E2M1x2Type : public ::mlir::Type::TypeBase<F4E2M1x2Type, ::mlir::Type, ::mlir::TypeStorage, ::mlir::DataLayoutTypeInterface::Trait> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.f4E2M1x2";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"f4E2M1x2"};
  }

  ::llvm::TypeSize getTypeSizeInBits(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
  uint64_t getABIAlignment(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
  uint64_t getPreferredAlignment(const ::mlir::DataLayout &dataLayout, ::mlir::DataLayoutEntryListRef params) const;
};
namespace detail {
struct VRegTypeStorage;
} // namespace detail
class VRegType : public ::mlir::Type::TypeBase<VRegType, ::mlir::Type, detail::VRegTypeStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.vreg";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  using Base::getChecked;
  static VRegType get(::mlir::MLIRContext *context, int64_t elementCount, Type elementType);
  static VRegType getChecked(::llvm::function_ref<::mlir::InFlightDiagnostic()> emitError, ::mlir::MLIRContext *context, int64_t elementCount, Type elementType);
  static ::llvm::LogicalResult verify(::llvm::function_ref<::mlir::InFlightDiagnostic()> emitError, int64_t elementCount, Type elementType);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"vreg"};
  }

  static ::mlir::Type parse(::mlir::AsmParser &odsParser);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  int64_t getElementCount() const;
  Type getElementType() const;
};
namespace detail {
struct MaskTypeStorage;
} // namespace detail
class MaskType : public ::mlir::Type::TypeBase<MaskType, ::mlir::Type, detail::MaskTypeStorage> {
public:
  using Base::Base;
  static bool isSupportedGranularity(::llvm::StringRef granularity);

  bool isB8() const { return getGranularity() == "b8"; }
  bool isB16() const { return getGranularity() == "b16"; }
  bool isB32() const { return getGranularity() == "b32"; }
  static constexpr ::llvm::StringLiteral name = "pto.mask";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  using Base::getChecked;
  static MaskType get(::mlir::MLIRContext *context, ::llvm::StringRef granularity);
  static MaskType getChecked(::llvm::function_ref<::mlir::InFlightDiagnostic()> emitError, ::mlir::MLIRContext *context, ::llvm::StringRef granularity);
  static ::llvm::LogicalResult verify(::llvm::function_ref<::mlir::InFlightDiagnostic()> emitError, ::llvm::StringRef granularity);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"mask"};
  }

  static ::mlir::Type parse(::mlir::AsmParser &odsParser);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::llvm::StringRef getGranularity() const;
};
class AlignType : public ::mlir::Type::TypeBase<AlignType, ::mlir::Type, ::mlir::TypeStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.align";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"align"};
  }

};
} // namespace pto
} // namespace mlir
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::PtrType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::TensorViewType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::PartitionTensorViewType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::TileType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::TileBufType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::EventIdArrayType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::LocalArrayType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::PipeType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AsyncSessionType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AsyncEventType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::HiF8Type)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::HiF8x2Type)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::F4E1M2x2Type)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::F4E2M1x2Type)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::VRegType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::MaskType)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AlignType)

#endif  // GET_TYPEDEF_CLASSES

