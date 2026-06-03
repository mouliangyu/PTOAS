/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Interface Declarations                                                     *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

namespace mlir {
namespace pto {
class OpPipeInterface;
namespace detail {
struct OpPipeInterfaceInterfaceTraits {
  struct Concept {
    /// The methods defined by the interface.
    ::mlir::pto::PIPE (*getPipe)(const Concept *impl, ::mlir::Operation *);
  };
  template<typename ConcreteOp>
  class Model : public Concept {
  public:
    using Interface = ::mlir::pto::OpPipeInterface;
    Model() : Concept{getPipe} {}

    static inline ::mlir::pto::PIPE getPipe(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
  };
  template<typename ConcreteOp>
  class FallbackModel : public Concept {
  public:
    using Interface = ::mlir::pto::OpPipeInterface;
    FallbackModel() : Concept{getPipe} {}

    static inline ::mlir::pto::PIPE getPipe(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
  };
  template<typename ConcreteModel, typename ConcreteOp>
  class ExternalModel : public FallbackModel<ConcreteModel> {
  public:
    using ConcreteEntity = ConcreteOp;
  };
};
template <typename ConcreteOp>
struct OpPipeInterfaceTrait;

} // namespace detail
class OpPipeInterface : public ::mlir::OpInterface<OpPipeInterface, detail::OpPipeInterfaceInterfaceTraits> {
public:
  using ::mlir::OpInterface<OpPipeInterface, detail::OpPipeInterfaceInterfaceTraits>::OpInterface;
  template <typename ConcreteOp>
  struct Trait : public detail::OpPipeInterfaceTrait<ConcreteOp> {};
  /// Get the hardware pipe where this operation executes.
  ::mlir::pto::PIPE getPipe();
};
namespace detail {
  template <typename ConcreteOp>
  struct OpPipeInterfaceTrait : public ::mlir::OpInterface<OpPipeInterface, detail::OpPipeInterfaceInterfaceTraits>::Trait<ConcreteOp> {
  };
}// namespace detail
} // namespace pto
} // namespace mlir
namespace mlir {
namespace pto {
class PTO_DpsInitOpInterface;
namespace detail {
struct PTO_DpsInitOpInterfaceInterfaceTraits {
  struct Concept {
    /// The methods defined by the interface.
    ::mlir::MutableOperandRange (*getDpsInitsMutable)(const Concept *impl, ::mlir::Operation *);
  };
  template<typename ConcreteOp>
  class Model : public Concept {
  public:
    using Interface = ::mlir::pto::PTO_DpsInitOpInterface;
    Model() : Concept{getDpsInitsMutable} {}

    static inline ::mlir::MutableOperandRange getDpsInitsMutable(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
  };
  template<typename ConcreteOp>
  class FallbackModel : public Concept {
  public:
    using Interface = ::mlir::pto::PTO_DpsInitOpInterface;
    FallbackModel() : Concept{getDpsInitsMutable} {}

    static inline ::mlir::MutableOperandRange getDpsInitsMutable(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
  };
  template<typename ConcreteModel, typename ConcreteOp>
  class ExternalModel : public FallbackModel<ConcreteModel> {
  public:
    using ConcreteEntity = ConcreteOp;
  };
};
template <typename ConcreteOp>
struct PTO_DpsInitOpInterfaceTrait;

} // namespace detail
class PTO_DpsInitOpInterface : public ::mlir::OpInterface<PTO_DpsInitOpInterface, detail::PTO_DpsInitOpInterfaceInterfaceTraits> {
public:
  using ::mlir::OpInterface<PTO_DpsInitOpInterface, detail::PTO_DpsInitOpInterfaceInterfaceTraits>::OpInterface;
  template <typename ConcreteOp>
  struct Trait : public detail::PTO_DpsInitOpInterfaceTrait<ConcreteOp> {};
  /// Return the mutable range of destination (init) operands.
  ::mlir::MutableOperandRange getDpsInitsMutable();

    ::mlir::OperandRange getDpsInits() {
      return (*this).getDpsInitsMutable();
    }
};
namespace detail {
  template <typename ConcreteOp>
  struct PTO_DpsInitOpInterfaceTrait : public ::mlir::OpInterface<PTO_DpsInitOpInterface, detail::PTO_DpsInitOpInterfaceInterfaceTraits>::Trait<ConcreteOp> {

    ::mlir::OperandRange getDpsInits() {
      return (*static_cast<ConcreteOp *>(this)).getDpsInitsMutable();
    }
  
  };
}// namespace detail
} // namespace pto
} // namespace mlir
namespace mlir {
namespace pto {
template<typename ConcreteOp>
::mlir::pto::PIPE detail::OpPipeInterfaceInterfaceTraits::Model<ConcreteOp>::getPipe(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getPipe();
}
template<typename ConcreteOp>
::mlir::pto::PIPE detail::OpPipeInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getPipe(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getPipe(tablegen_opaque_val);
}
} // namespace pto
} // namespace mlir
namespace mlir {
namespace pto {
template<typename ConcreteOp>
::mlir::MutableOperandRange detail::PTO_DpsInitOpInterfaceInterfaceTraits::Model<ConcreteOp>::getDpsInitsMutable(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getDpsInitsMutable();
}
template<typename ConcreteOp>
::mlir::MutableOperandRange detail::PTO_DpsInitOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getDpsInitsMutable(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getDpsInitsMutable(tablegen_opaque_val);
}
} // namespace pto
} // namespace mlir
