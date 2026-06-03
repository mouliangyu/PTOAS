/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* TypeDef Definitions                                                        *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifdef GET_TYPEDEF_LIST
#undef GET_TYPEDEF_LIST

::mlir::pto::PtrType,
::mlir::pto::TensorViewType,
::mlir::pto::PartitionTensorViewType,
::mlir::pto::TileType,
::mlir::pto::TileBufType,
::mlir::pto::EventIdArrayType,
::mlir::pto::LocalArrayType,
::mlir::pto::PipeType,
::mlir::pto::AsyncSessionType,
::mlir::pto::AsyncEventType,
::mlir::pto::HiF8Type,
::mlir::pto::HiF8x2Type,
::mlir::pto::F4E1M2x2Type,
::mlir::pto::F4E2M1x2Type,
::mlir::pto::VRegType,
::mlir::pto::MaskType,
::mlir::pto::AlignType

#endif  // GET_TYPEDEF_LIST

#ifdef GET_TYPEDEF_CLASSES
#undef GET_TYPEDEF_CLASSES

static ::mlir::OptionalParseResult generatedTypeParser(::mlir::AsmParser &parser, ::llvm::StringRef *mnemonic, ::mlir::Type &value) {
  return ::mlir::AsmParser::KeywordSwitch<::mlir::OptionalParseResult>(parser)
    .Case(::mlir::pto::PtrType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::PtrType::parse(parser);
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::TensorViewType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::TensorViewType::parse(parser);
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::PartitionTensorViewType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::PartitionTensorViewType::parse(parser);
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::TileType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::TileType::parse(parser);
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::TileBufType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::TileBufType::parse(parser);
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::EventIdArrayType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::EventIdArrayType::parse(parser);
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::LocalArrayType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::LocalArrayType::parse(parser);
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::PipeType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::PipeType::get(parser.getContext());
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::AsyncSessionType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::AsyncSessionType::get(parser.getContext());
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::AsyncEventType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::AsyncEventType::get(parser.getContext());
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::HiF8Type::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::HiF8Type::get(parser.getContext());
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::HiF8x2Type::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::HiF8x2Type::get(parser.getContext());
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::F4E1M2x2Type::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::F4E1M2x2Type::get(parser.getContext());
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::F4E2M1x2Type::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::F4E2M1x2Type::get(parser.getContext());
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::VRegType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::VRegType::parse(parser);
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::MaskType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::MaskType::parse(parser);
      return ::mlir::success(!!value);
    })
    .Case(::mlir::pto::AlignType::getMnemonic(), [&](llvm::StringRef, llvm::SMLoc) {
      value = ::mlir::pto::AlignType::get(parser.getContext());
      return ::mlir::success(!!value);
    })
    .Default([&](llvm::StringRef keyword, llvm::SMLoc) {
      *mnemonic = keyword;
      return std::nullopt;
    });
}

static ::llvm::LogicalResult generatedTypePrinter(::mlir::Type def, ::mlir::AsmPrinter &printer) {
  return ::llvm::TypeSwitch<::mlir::Type, ::llvm::LogicalResult>(def)    .Case<::mlir::pto::PtrType>([&](auto t) {
      printer << ::mlir::pto::PtrType::getMnemonic();
t.print(printer);
      return ::mlir::success();
    })
    .Case<::mlir::pto::TensorViewType>([&](auto t) {
      printer << ::mlir::pto::TensorViewType::getMnemonic();
t.print(printer);
      return ::mlir::success();
    })
    .Case<::mlir::pto::PartitionTensorViewType>([&](auto t) {
      printer << ::mlir::pto::PartitionTensorViewType::getMnemonic();
t.print(printer);
      return ::mlir::success();
    })
    .Case<::mlir::pto::TileType>([&](auto t) {
      printer << ::mlir::pto::TileType::getMnemonic();
t.print(printer);
      return ::mlir::success();
    })
    .Case<::mlir::pto::TileBufType>([&](auto t) {
      printer << ::mlir::pto::TileBufType::getMnemonic();
t.print(printer);
      return ::mlir::success();
    })
    .Case<::mlir::pto::EventIdArrayType>([&](auto t) {
      printer << ::mlir::pto::EventIdArrayType::getMnemonic();
t.print(printer);
      return ::mlir::success();
    })
    .Case<::mlir::pto::LocalArrayType>([&](auto t) {
      printer << ::mlir::pto::LocalArrayType::getMnemonic();
t.print(printer);
      return ::mlir::success();
    })
    .Case<::mlir::pto::PipeType>([&](auto t) {
      printer << ::mlir::pto::PipeType::getMnemonic();
      return ::mlir::success();
    })
    .Case<::mlir::pto::AsyncSessionType>([&](auto t) {
      printer << ::mlir::pto::AsyncSessionType::getMnemonic();
      return ::mlir::success();
    })
    .Case<::mlir::pto::AsyncEventType>([&](auto t) {
      printer << ::mlir::pto::AsyncEventType::getMnemonic();
      return ::mlir::success();
    })
    .Case<::mlir::pto::HiF8Type>([&](auto t) {
      printer << ::mlir::pto::HiF8Type::getMnemonic();
      return ::mlir::success();
    })
    .Case<::mlir::pto::HiF8x2Type>([&](auto t) {
      printer << ::mlir::pto::HiF8x2Type::getMnemonic();
      return ::mlir::success();
    })
    .Case<::mlir::pto::F4E1M2x2Type>([&](auto t) {
      printer << ::mlir::pto::F4E1M2x2Type::getMnemonic();
      return ::mlir::success();
    })
    .Case<::mlir::pto::F4E2M1x2Type>([&](auto t) {
      printer << ::mlir::pto::F4E2M1x2Type::getMnemonic();
      return ::mlir::success();
    })
    .Case<::mlir::pto::VRegType>([&](auto t) {
      printer << ::mlir::pto::VRegType::getMnemonic();
t.print(printer);
      return ::mlir::success();
    })
    .Case<::mlir::pto::MaskType>([&](auto t) {
      printer << ::mlir::pto::MaskType::getMnemonic();
t.print(printer);
      return ::mlir::success();
    })
    .Case<::mlir::pto::AlignType>([&](auto t) {
      printer << ::mlir::pto::AlignType::getMnemonic();
      return ::mlir::success();
    })
    .Default([](auto) { return ::mlir::failure(); });
}

namespace mlir {
namespace pto {
namespace detail {
struct PtrTypeStorage : public ::mlir::TypeStorage {
  using KeyTy = std::tuple<mlir::Type, mlir::pto::AddressSpaceAttr>;
  PtrTypeStorage(mlir::Type elementType, mlir::pto::AddressSpaceAttr memorySpace) : elementType(std::move(elementType)), memorySpace(std::move(memorySpace)) {}

  KeyTy getAsKey() const {
    return KeyTy(elementType, memorySpace);
  }

  bool operator==(const KeyTy &tblgenKey) const {
    return (elementType == std::get<0>(tblgenKey)) && (memorySpace == std::get<1>(tblgenKey));
  }

  static ::llvm::hash_code hashKey(const KeyTy &tblgenKey) {
    return ::llvm::hash_combine(std::get<0>(tblgenKey), std::get<1>(tblgenKey));
  }

  static PtrTypeStorage *construct(::mlir::TypeStorageAllocator &allocator, KeyTy &&tblgenKey) {
    auto elementType = std::move(std::get<0>(tblgenKey));
    auto memorySpace = std::move(std::get<1>(tblgenKey));
    return new (allocator.allocate<PtrTypeStorage>()) PtrTypeStorage(std::move(elementType), std::move(memorySpace));
  }

  mlir::Type elementType;
  mlir::pto::AddressSpaceAttr memorySpace;
};
} // namespace detail
PtrType PtrType::get(::mlir::MLIRContext *context, Type elementType) {
  return Base::get(context, elementType,
                   mlir::pto::AddressSpaceAttr::get(context,
                                                    mlir::pto::AddressSpace::GM));
}

PtrType PtrType::get(::mlir::MLIRContext *context, Type elementType, mlir::pto::AddressSpaceAttr memorySpace) {
  return Base::get(context, elementType, memorySpace);
}

mlir::Type PtrType::getElementType() const {
  return getImpl()->elementType;
}

mlir::pto::AddressSpaceAttr PtrType::getMemorySpace() const {
  return getImpl()->memorySpace;
}

} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::PtrType)
namespace mlir {
namespace pto {
namespace detail {
struct TensorViewTypeStorage : public ::mlir::TypeStorage {
  using KeyTy = std::tuple<::llvm::ArrayRef<int64_t>, mlir::Type>;
  TensorViewTypeStorage(::llvm::ArrayRef<int64_t> shape, mlir::Type elementType) : shape(std::move(shape)), elementType(std::move(elementType)) {}

  KeyTy getAsKey() const {
    return KeyTy(shape, elementType);
  }

  bool operator==(const KeyTy &tblgenKey) const {
    return (shape == std::get<0>(tblgenKey)) && (elementType == std::get<1>(tblgenKey));
  }

  static ::llvm::hash_code hashKey(const KeyTy &tblgenKey) {
    return ::llvm::hash_combine(std::get<0>(tblgenKey), std::get<1>(tblgenKey));
  }

  static TensorViewTypeStorage *construct(::mlir::TypeStorageAllocator &allocator, KeyTy &&tblgenKey) {
    auto shape = std::move(std::get<0>(tblgenKey));
    auto elementType = std::move(std::get<1>(tblgenKey));
    shape = allocator.copyInto(shape);
    return new (allocator.allocate<TensorViewTypeStorage>()) TensorViewTypeStorage(std::move(shape), std::move(elementType));
  }

  ::llvm::ArrayRef<int64_t> shape;
  mlir::Type elementType;
};
} // namespace detail
TensorViewType TensorViewType::get(::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType) {
  return Base::get(context, std::move(shape), std::move(elementType));
}

TensorViewType TensorViewType::get(::mlir::MLIRContext *context, int64_t rank, Type elementType) {
  SmallVector<int64_t, 4> shp(rank, mlir::ShapedType::kDynamic);
  return Base::get(context, shp, elementType);
}

::llvm::ArrayRef<int64_t> TensorViewType::getShape() const {
  return getImpl()->shape;
}

mlir::Type TensorViewType::getElementType() const {
  return getImpl()->elementType;
}

} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::TensorViewType)
namespace mlir {
namespace pto {
namespace detail {
struct PartitionTensorViewTypeStorage : public ::mlir::TypeStorage {
  using KeyTy = std::tuple<::llvm::ArrayRef<int64_t>, mlir::Type>;
  PartitionTensorViewTypeStorage(::llvm::ArrayRef<int64_t> shape, mlir::Type elementType) : shape(std::move(shape)), elementType(std::move(elementType)) {}

  KeyTy getAsKey() const {
    return KeyTy(shape, elementType);
  }

  bool operator==(const KeyTy &tblgenKey) const {
    return (shape == std::get<0>(tblgenKey)) && (elementType == std::get<1>(tblgenKey));
  }

  static ::llvm::hash_code hashKey(const KeyTy &tblgenKey) {
    return ::llvm::hash_combine(std::get<0>(tblgenKey), std::get<1>(tblgenKey));
  }

  static PartitionTensorViewTypeStorage *construct(::mlir::TypeStorageAllocator &allocator, KeyTy &&tblgenKey) {
    auto shape = std::move(std::get<0>(tblgenKey));
    auto elementType = std::move(std::get<1>(tblgenKey));
    shape = allocator.copyInto(shape);
    return new (allocator.allocate<PartitionTensorViewTypeStorage>()) PartitionTensorViewTypeStorage(std::move(shape), std::move(elementType));
  }

  ::llvm::ArrayRef<int64_t> shape;
  mlir::Type elementType;
};
} // namespace detail
PartitionTensorViewType PartitionTensorViewType::get(::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType) {
  return Base::get(context, std::move(shape), std::move(elementType));
}

PartitionTensorViewType PartitionTensorViewType::get(::mlir::MLIRContext *context, int64_t rank, Type elementType) {
  SmallVector<int64_t, 4> shp(rank, mlir::ShapedType::kDynamic);
  return Base::get(context, shp, elementType);
}

::llvm::ArrayRef<int64_t> PartitionTensorViewType::getShape() const {
  return getImpl()->shape;
}

mlir::Type PartitionTensorViewType::getElementType() const {
  return getImpl()->elementType;
}

} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::PartitionTensorViewType)
namespace mlir {
namespace pto {
namespace detail {
struct TileTypeStorage : public ::mlir::TypeStorage {
  using KeyTy = std::tuple<::llvm::ArrayRef<int64_t>, mlir::Type>;
  TileTypeStorage(::llvm::ArrayRef<int64_t> shape, mlir::Type elementType) : shape(std::move(shape)), elementType(std::move(elementType)) {}

  KeyTy getAsKey() const {
    return KeyTy(shape, elementType);
  }

  bool operator==(const KeyTy &tblgenKey) const {
    return (shape == std::get<0>(tblgenKey)) && (elementType == std::get<1>(tblgenKey));
  }

  static ::llvm::hash_code hashKey(const KeyTy &tblgenKey) {
    return ::llvm::hash_combine(std::get<0>(tblgenKey), std::get<1>(tblgenKey));
  }

  static TileTypeStorage *construct(::mlir::TypeStorageAllocator &allocator, KeyTy &&tblgenKey) {
    auto shape = std::move(std::get<0>(tblgenKey));
    auto elementType = std::move(std::get<1>(tblgenKey));
    shape = allocator.copyInto(shape);
    return new (allocator.allocate<TileTypeStorage>()) TileTypeStorage(std::move(shape), std::move(elementType));
  }

  ::llvm::ArrayRef<int64_t> shape;
  mlir::Type elementType;
};
} // namespace detail
TileType TileType::get(::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType) {
  return Base::get(context, std::move(shape), std::move(elementType));
}

::llvm::ArrayRef<int64_t> TileType::getShape() const {
  return getImpl()->shape;
}

mlir::Type TileType::getElementType() const {
  return getImpl()->elementType;
}

} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::TileType)
namespace mlir {
namespace pto {
namespace detail {
struct TileBufTypeStorage : public ::mlir::TypeStorage {
  using KeyTy = std::tuple<::llvm::ArrayRef<int64_t>, mlir::Type, mlir::Attribute, ::llvm::ArrayRef<int64_t>, mlir::pto::TileBufConfigAttr>;
  TileBufTypeStorage(::llvm::ArrayRef<int64_t> shape, mlir::Type elementType, mlir::Attribute memorySpace, ::llvm::ArrayRef<int64_t> validShape, mlir::pto::TileBufConfigAttr config) : shape(std::move(shape)), elementType(std::move(elementType)), memorySpace(std::move(memorySpace)), validShape(std::move(validShape)), config(std::move(config)) {}

  KeyTy getAsKey() const {
    return KeyTy(shape, elementType, memorySpace, validShape, config);
  }

  bool operator==(const KeyTy &tblgenKey) const {
    return (shape == std::get<0>(tblgenKey)) && (elementType == std::get<1>(tblgenKey)) && (memorySpace == std::get<2>(tblgenKey)) && (validShape == std::get<3>(tblgenKey)) && (config == std::get<4>(tblgenKey));
  }

  static ::llvm::hash_code hashKey(const KeyTy &tblgenKey) {
    return ::llvm::hash_combine(std::get<0>(tblgenKey), std::get<1>(tblgenKey), std::get<2>(tblgenKey), std::get<3>(tblgenKey), std::get<4>(tblgenKey));
  }

  static TileBufTypeStorage *construct(::mlir::TypeStorageAllocator &allocator, KeyTy &&tblgenKey) {
    auto shape = std::move(std::get<0>(tblgenKey));
    auto elementType = std::move(std::get<1>(tblgenKey));
    auto memorySpace = std::move(std::get<2>(tblgenKey));
    auto validShape = std::move(std::get<3>(tblgenKey));
    auto config = std::move(std::get<4>(tblgenKey));
    shape = allocator.copyInto(shape);
    validShape = allocator.copyInto(validShape);
    return new (allocator.allocate<TileBufTypeStorage>()) TileBufTypeStorage(std::move(shape), std::move(elementType), std::move(memorySpace), std::move(validShape), std::move(config));
  }

  ::llvm::ArrayRef<int64_t> shape;
  mlir::Type elementType;
  mlir::Attribute memorySpace;
  ::llvm::ArrayRef<int64_t> validShape;
  mlir::pto::TileBufConfigAttr config;
};
} // namespace detail
TileBufType TileBufType::get(::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType, mlir::Attribute memorySpace, ::llvm::ArrayRef<int64_t> validShape, mlir::pto::TileBufConfigAttr config) {
  return Base::get(context, std::move(shape), std::move(elementType), std::move(memorySpace), std::move(validShape), std::move(config));
}

TileBufType TileBufType::get(::mlir::MLIRContext *context, ArrayRef<int64_t> shape, Type elementType, Attribute memorySpace) {
  auto cfg = mlir::pto::TileBufConfigAttr::getDefault(context);

  // ✅ 默认 validShape = shape（你要求的语义）
  SmallVector<int64_t, 2> vs(shape.begin(), shape.end());
  for (int64_t &dim : vs)
    if (dim < 0)
      dim = mlir::ShapedType::kDynamic;

  return Base::get(context, shape, elementType, memorySpace, vs, cfg);
}

TileBufType TileBufType::get(::mlir::MLIRContext *context, ArrayRef<int64_t> shape, Type elementType, Attribute memorySpace, mlir::pto::TileBufConfigAttr config) {
  // ✅ 默认 validShape = shape
  SmallVector<int64_t, 2> vs(shape.begin(), shape.end());
  for (int64_t &dim : vs)
    if (dim < 0)
      dim = mlir::ShapedType::kDynamic;
  return Base::get(context, shape, elementType, memorySpace, vs, config);
}

TileBufType TileBufType::get(::mlir::MLIRContext *context, ArrayRef<int64_t> shape, Type elementType, Attribute memorySpace, ArrayRef<int64_t> validShape) {
  auto cfg = mlir::pto::TileBufConfigAttr::getDefault(context);
  SmallVector<int64_t, 4> vs(validShape.begin(), validShape.end());
  for (int64_t &dim : vs)
    if (dim < 0)
      dim = mlir::ShapedType::kDynamic;
  return Base::get(context, shape, elementType, memorySpace, vs, cfg);
}

::llvm::ArrayRef<int64_t> TileBufType::getShape() const {
  return getImpl()->shape;
}

mlir::Type TileBufType::getElementType() const {
  return getImpl()->elementType;
}

mlir::Attribute TileBufType::getMemorySpace() const {
  return getImpl()->memorySpace;
}

::llvm::ArrayRef<int64_t> TileBufType::getValidShape() const {
  return getImpl()->validShape;
}

mlir::pto::TileBufConfigAttr TileBufType::getConfig() const {
  return getImpl()->config;
}

} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::TileBufType)
namespace mlir {
namespace pto {
namespace detail {
struct EventIdArrayTypeStorage : public ::mlir::TypeStorage {
  using KeyTy = std::tuple<int64_t>;
  EventIdArrayTypeStorage(int64_t size) : size(std::move(size)) {}

  KeyTy getAsKey() const {
    return KeyTy(size);
  }

  bool operator==(const KeyTy &tblgenKey) const {
    return (size == std::get<0>(tblgenKey));
  }

  static ::llvm::hash_code hashKey(const KeyTy &tblgenKey) {
    return ::llvm::hash_combine(std::get<0>(tblgenKey));
  }

  static EventIdArrayTypeStorage *construct(::mlir::TypeStorageAllocator &allocator, KeyTy &&tblgenKey) {
    auto size = std::move(std::get<0>(tblgenKey));
    return new (allocator.allocate<EventIdArrayTypeStorage>()) EventIdArrayTypeStorage(std::move(size));
  }

  int64_t size;
};
} // namespace detail
EventIdArrayType EventIdArrayType::get(::mlir::MLIRContext *context, int64_t size) {
  return Base::get(context, std::move(size));
}

::mlir::Type EventIdArrayType::parse(::mlir::AsmParser &odsParser) {
  ::mlir::Builder odsBuilder(odsParser.getContext());
  ::llvm::SMLoc odsLoc = odsParser.getCurrentLocation();
  (void) odsLoc;
  ::mlir::FailureOr<int64_t> _result_size;
  // Parse literal '<'
  if (odsParser.parseLess()) return {};

  // Parse variable 'size'
  _result_size = ::mlir::FieldParser<int64_t>::parse(odsParser);
  if (::mlir::failed(_result_size)) {
    odsParser.emitError(odsParser.getCurrentLocation(), "failed to parse EventIdArrayType parameter 'size' which is to be a `int64_t`");
    return {};
  }
  // Parse literal '>'
  if (odsParser.parseGreater()) return {};
  assert(::mlir::succeeded(_result_size));
  return EventIdArrayType::get(odsParser.getContext(),
      int64_t((*_result_size)));
}

void EventIdArrayType::print(::mlir::AsmPrinter &odsPrinter) const {
  ::mlir::Builder odsBuilder(getContext());
  odsPrinter << "<";
  odsPrinter.printStrippedAttrOrType(getSize());
  odsPrinter << ">";
}

int64_t EventIdArrayType::getSize() const {
  return getImpl()->size;
}

} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::EventIdArrayType)
namespace mlir {
namespace pto {
namespace detail {
struct LocalArrayTypeStorage : public ::mlir::TypeStorage {
  using KeyTy = std::tuple<::llvm::ArrayRef<int64_t>, mlir::Type>;
  LocalArrayTypeStorage(::llvm::ArrayRef<int64_t> shape, mlir::Type elementType) : shape(std::move(shape)), elementType(std::move(elementType)) {}

  KeyTy getAsKey() const {
    return KeyTy(shape, elementType);
  }

  bool operator==(const KeyTy &tblgenKey) const {
    return (shape == std::get<0>(tblgenKey)) && (elementType == std::get<1>(tblgenKey));
  }

  static ::llvm::hash_code hashKey(const KeyTy &tblgenKey) {
    return ::llvm::hash_combine(std::get<0>(tblgenKey), std::get<1>(tblgenKey));
  }

  static LocalArrayTypeStorage *construct(::mlir::TypeStorageAllocator &allocator, KeyTy &&tblgenKey) {
    auto shape = std::move(std::get<0>(tblgenKey));
    auto elementType = std::move(std::get<1>(tblgenKey));
    shape = allocator.copyInto(shape);
    return new (allocator.allocate<LocalArrayTypeStorage>()) LocalArrayTypeStorage(std::move(shape), std::move(elementType));
  }

  ::llvm::ArrayRef<int64_t> shape;
  mlir::Type elementType;
};
} // namespace detail
LocalArrayType LocalArrayType::get(::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType) {
  return Base::get(context, std::move(shape), std::move(elementType));
}

LocalArrayType LocalArrayType::getChecked(::llvm::function_ref<::mlir::InFlightDiagnostic()> emitError, ::mlir::MLIRContext *context, ::llvm::ArrayRef<int64_t> shape, mlir::Type elementType) {
  return Base::getChecked(emitError, context, shape, elementType);
}

::llvm::ArrayRef<int64_t> LocalArrayType::getShape() const {
  return getImpl()->shape;
}

mlir::Type LocalArrayType::getElementType() const {
  return getImpl()->elementType;
}

} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::LocalArrayType)
namespace mlir {
namespace pto {
} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::PipeType)
namespace mlir {
namespace pto {
} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::AsyncSessionType)
namespace mlir {
namespace pto {
} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::AsyncEventType)
namespace mlir {
namespace pto {
} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::HiF8Type)
namespace mlir {
namespace pto {
} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::HiF8x2Type)
namespace mlir {
namespace pto {
} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::F4E1M2x2Type)
namespace mlir {
namespace pto {
} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::F4E2M1x2Type)
namespace mlir {
namespace pto {
namespace detail {
struct VRegTypeStorage : public ::mlir::TypeStorage {
  using KeyTy = std::tuple<int64_t, Type>;
  VRegTypeStorage(int64_t elementCount, Type elementType) : elementCount(std::move(elementCount)), elementType(std::move(elementType)) {}

  KeyTy getAsKey() const {
    return KeyTy(elementCount, elementType);
  }

  bool operator==(const KeyTy &tblgenKey) const {
    return (elementCount == std::get<0>(tblgenKey)) && (elementType == std::get<1>(tblgenKey));
  }

  static ::llvm::hash_code hashKey(const KeyTy &tblgenKey) {
    return ::llvm::hash_combine(std::get<0>(tblgenKey), std::get<1>(tblgenKey));
  }

  static VRegTypeStorage *construct(::mlir::TypeStorageAllocator &allocator, KeyTy &&tblgenKey) {
    auto elementCount = std::move(std::get<0>(tblgenKey));
    auto elementType = std::move(std::get<1>(tblgenKey));
    return new (allocator.allocate<VRegTypeStorage>()) VRegTypeStorage(std::move(elementCount), std::move(elementType));
  }

  int64_t elementCount;
  Type elementType;
};
} // namespace detail
VRegType VRegType::get(::mlir::MLIRContext *context, int64_t elementCount, Type elementType) {
  return Base::get(context, std::move(elementCount), std::move(elementType));
}

VRegType VRegType::getChecked(::llvm::function_ref<::mlir::InFlightDiagnostic()> emitError, ::mlir::MLIRContext *context, int64_t elementCount, Type elementType) {
  return Base::getChecked(emitError, context, elementCount, elementType);
}

int64_t VRegType::getElementCount() const {
  return getImpl()->elementCount;
}

Type VRegType::getElementType() const {
  return getImpl()->elementType;
}

} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::VRegType)
namespace mlir {
namespace pto {
namespace detail {
struct MaskTypeStorage : public ::mlir::TypeStorage {
  using KeyTy = std::tuple<::llvm::StringRef>;
  MaskTypeStorage(::llvm::StringRef granularity) : granularity(std::move(granularity)) {}

  KeyTy getAsKey() const {
    return KeyTy(granularity);
  }

  bool operator==(const KeyTy &tblgenKey) const {
    return (granularity == std::get<0>(tblgenKey));
  }

  static ::llvm::hash_code hashKey(const KeyTy &tblgenKey) {
    return ::llvm::hash_combine(std::get<0>(tblgenKey));
  }

  static MaskTypeStorage *construct(::mlir::TypeStorageAllocator &allocator, KeyTy &&tblgenKey) {
    auto granularity = std::move(std::get<0>(tblgenKey));
    granularity = allocator.copyInto(granularity);
    return new (allocator.allocate<MaskTypeStorage>()) MaskTypeStorage(std::move(granularity));
  }

  ::llvm::StringRef granularity;
};
} // namespace detail
MaskType MaskType::get(::mlir::MLIRContext *context, ::llvm::StringRef granularity) {
  return Base::get(context, std::move(granularity));
}

MaskType MaskType::getChecked(::llvm::function_ref<::mlir::InFlightDiagnostic()> emitError, ::mlir::MLIRContext *context, ::llvm::StringRef granularity) {
  return Base::getChecked(emitError, context, granularity);
}

::llvm::StringRef MaskType::getGranularity() const {
  return getImpl()->granularity;
}

} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::MaskType)
namespace mlir {
namespace pto {
} // namespace pto
} // namespace mlir
MLIR_DEFINE_EXPLICIT_TYPE_ID(::mlir::pto::AlignType)
namespace mlir {
namespace pto {

/// Parse a type registered to this dialect.
::mlir::Type PTODialect::parseType(::mlir::DialectAsmParser &parser) const {
  ::llvm::SMLoc typeLoc = parser.getCurrentLocation();
  ::llvm::StringRef mnemonic;
  ::mlir::Type genType;
  auto parseResult = generatedTypeParser(parser, &mnemonic, genType);
  if (parseResult.has_value())
    return genType;
  
  parser.emitError(typeLoc) << "unknown  type `"
      << mnemonic << "` in dialect `" << getNamespace() << "`";
  return {};
}
/// Print a type registered to this dialect.
void PTODialect::printType(::mlir::Type type,
                    ::mlir::DialectAsmPrinter &printer) const {
  if (::mlir::succeeded(generatedTypePrinter(type, printer)))
    return;
  
}
} // namespace pto
} // namespace mlir

#endif  // GET_TYPEDEF_CLASSES

