/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Interface Declarations                                                     *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

namespace mlir {
namespace pto {
class MadSemanticOpInterface;
namespace detail {
struct MadSemanticOpInterfaceInterfaceTraits {
  struct Concept {
    /// The methods defined by the interface.
    ::mlir::Value (*getLhs)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Value (*getRhs)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Value (*getDst)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Value (*getM)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Value (*getN)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Value (*getK)(const Concept *impl, ::mlir::Operation *);
    bool (*isMadMxFamily)(const Concept *impl, ::mlir::Operation *);
    bool (*hasBiasOperand)(const Concept *impl, ::mlir::Operation *);
    bool (*readsAccumulator)(const Concept *impl, ::mlir::Operation *);
    bool (*supportsTf32Mode)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Value (*getBiasOrNull)(const Concept *impl, ::mlir::Operation *);
    bool (*initializesAccumulatorWithBias)(const Concept *impl, ::mlir::Operation *);
    bool (*initializesAccumulatorWithZero)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Attribute (*getUnitFlagModeAttr)(const Concept *impl, ::mlir::Operation *);
    bool (*getDisableGemv)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Attribute (*getSatModeAttr)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Attribute (*getTf32ModeAttr)(const Concept *impl, ::mlir::Operation *);
    bool (*getNDir)(const Concept *impl, ::mlir::Operation *);
  };
  template<typename ConcreteOp>
  class Model : public Concept {
  public:
    using Interface = ::mlir::pto::MadSemanticOpInterface;
    Model() : Concept{getLhs, getRhs, getDst, getM, getN, getK, isMadMxFamily, hasBiasOperand, readsAccumulator, supportsTf32Mode, getBiasOrNull, initializesAccumulatorWithBias, initializesAccumulatorWithZero, getUnitFlagModeAttr, getDisableGemv, getSatModeAttr, getTf32ModeAttr, getNDir} {}

    static inline ::mlir::Value getLhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getRhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getDst(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getM(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getN(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getK(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool isMadMxFamily(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool hasBiasOperand(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool readsAccumulator(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool supportsTf32Mode(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getBiasOrNull(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool initializesAccumulatorWithBias(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool initializesAccumulatorWithZero(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Attribute getUnitFlagModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool getDisableGemv(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Attribute getSatModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Attribute getTf32ModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool getNDir(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
  };
  template<typename ConcreteOp>
  class FallbackModel : public Concept {
  public:
    using Interface = ::mlir::pto::MadSemanticOpInterface;
    FallbackModel() : Concept{getLhs, getRhs, getDst, getM, getN, getK, isMadMxFamily, hasBiasOperand, readsAccumulator, supportsTf32Mode, getBiasOrNull, initializesAccumulatorWithBias, initializesAccumulatorWithZero, getUnitFlagModeAttr, getDisableGemv, getSatModeAttr, getTf32ModeAttr, getNDir} {}

    static inline ::mlir::Value getLhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getRhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getDst(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getM(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getN(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getK(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool isMadMxFamily(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool hasBiasOperand(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool readsAccumulator(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool supportsTf32Mode(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getBiasOrNull(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool initializesAccumulatorWithBias(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool initializesAccumulatorWithZero(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Attribute getUnitFlagModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool getDisableGemv(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Attribute getSatModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Attribute getTf32ModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool getNDir(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
  };
  template<typename ConcreteModel, typename ConcreteOp>
  class ExternalModel : public FallbackModel<ConcreteModel> {
  public:
    using ConcreteEntity = ConcreteOp;
    ::mlir::Value getLhs(::mlir::Operation *tablegen_opaque_val) const;
    ::mlir::Value getRhs(::mlir::Operation *tablegen_opaque_val) const;
    ::mlir::Value getDst(::mlir::Operation *tablegen_opaque_val) const;
    ::mlir::Value getM(::mlir::Operation *tablegen_opaque_val) const;
    ::mlir::Value getN(::mlir::Operation *tablegen_opaque_val) const;
    ::mlir::Value getK(::mlir::Operation *tablegen_opaque_val) const;
    bool initializesAccumulatorWithBias(::mlir::Operation *tablegen_opaque_val) const;
    bool initializesAccumulatorWithZero(::mlir::Operation *tablegen_opaque_val) const;
    ::mlir::Attribute getUnitFlagModeAttr(::mlir::Operation *tablegen_opaque_val) const;
    bool getDisableGemv(::mlir::Operation *tablegen_opaque_val) const;
    ::mlir::Attribute getSatModeAttr(::mlir::Operation *tablegen_opaque_val) const;
    bool getNDir(::mlir::Operation *tablegen_opaque_val) const;
  };
};
template <typename ConcreteOp>
struct MadSemanticOpInterfaceTrait;

} // namespace detail
class MadSemanticOpInterface : public ::mlir::OpInterface<MadSemanticOpInterface, detail::MadSemanticOpInterfaceInterfaceTraits> {
public:
  using ::mlir::OpInterface<MadSemanticOpInterface, detail::MadSemanticOpInterfaceInterfaceTraits>::OpInterface;
  template <typename ConcreteOp>
  struct Trait : public detail::MadSemanticOpInterfaceTrait<ConcreteOp> {};
  /// Return the left matrix operand.
  ::mlir::Value getLhs();
  /// Return the right matrix operand.
  ::mlir::Value getRhs();
  /// Return the accumulator/destination operand.
  ::mlir::Value getDst();
  /// Return M.
  ::mlir::Value getM();
  /// Return N.
  ::mlir::Value getN();
  /// Return K.
  ::mlir::Value getK();
  /// Return true for MX MAD forms.
  bool isMadMxFamily();
  /// Return true if the op carries a bias operand.
  bool hasBiasOperand();
  /// Return true if the op reads the existing accumulator.
  bool readsAccumulator();
  /// Return true if TF32 mode is a legal semantic attribute.
  bool supportsTf32Mode();
  /// Return the bias operand, or null for non-bias forms.
  ::mlir::Value getBiasOrNull();
  /// Return true if the accumulator source is bias.
  bool initializesAccumulatorWithBias();
  /// Return true if the accumulator is initialized to zero.
  bool initializesAccumulatorWithZero();
  /// Return unit_flag_mode.
  ::mlir::Attribute getUnitFlagModeAttr();
  /// Return true if disable_gemv is set.
  bool getDisableGemv();
  /// Return sat_mode.
  ::mlir::Attribute getSatModeAttr();
  /// Return tf32_mode.
  ::mlir::Attribute getTf32ModeAttr();
  /// Return true if n_dir is set.
  bool getNDir();
};
namespace detail {
  template <typename ConcreteOp>
  struct MadSemanticOpInterfaceTrait : public ::mlir::OpInterface<MadSemanticOpInterface, detail::MadSemanticOpInterfaceInterfaceTraits>::Trait<ConcreteOp> {
    /// Return the left matrix operand.
    ::mlir::Value getLhs() {
      return (*static_cast<ConcreteOp *>(this)).getLhs();
    }
    /// Return the right matrix operand.
    ::mlir::Value getRhs() {
      return (*static_cast<ConcreteOp *>(this)).getRhs();
    }
    /// Return the accumulator/destination operand.
    ::mlir::Value getDst() {
      return (*static_cast<ConcreteOp *>(this)).getDst();
    }
    /// Return M.
    ::mlir::Value getM() {
      return (*static_cast<ConcreteOp *>(this)).getM();
    }
    /// Return N.
    ::mlir::Value getN() {
      return (*static_cast<ConcreteOp *>(this)).getN();
    }
    /// Return K.
    ::mlir::Value getK() {
      return (*static_cast<ConcreteOp *>(this)).getK();
    }
    /// Return true if the accumulator source is bias.
    bool initializesAccumulatorWithBias() {
      return (*static_cast<ConcreteOp *>(this)).hasBiasOperand();
    }
    /// Return true if the accumulator is initialized to zero.
    bool initializesAccumulatorWithZero() {
      return !(*static_cast<ConcreteOp *>(this)).readsAccumulator() && !(*static_cast<ConcreteOp *>(this)).hasBiasOperand();
    }
    /// Return unit_flag_mode.
    ::mlir::Attribute getUnitFlagModeAttr() {
      return (*static_cast<ConcreteOp *>(this)).getUnitFlagModeAttr();
    }
    /// Return true if disable_gemv is set.
    bool getDisableGemv() {
      return (*static_cast<ConcreteOp *>(this)).getDisableGemv();
    }
    /// Return sat_mode.
    ::mlir::Attribute getSatModeAttr() {
      return (*static_cast<ConcreteOp *>(this)).getSatModeAttr();
    }
    /// Return true if n_dir is set.
    bool getNDir() {
      return (*static_cast<ConcreteOp *>(this)).getNDir();
    }
  };
}// namespace detail
} // namespace pto
} // namespace mlir
namespace mlir {
namespace pto {
class MadRawOpInterface;
namespace detail {
struct MadRawOpInterfaceInterfaceTraits {
  struct Concept {
    /// The methods defined by the interface.
    ::mlir::Value (*getLhs)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Value (*getRhs)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Value (*getDst)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Value (*getXt)(const Concept *impl, ::mlir::Operation *);
    bool (*isMadMxFamily)(const Concept *impl, ::mlir::Operation *);
    bool (*hasBiasOperand)(const Concept *impl, ::mlir::Operation *);
    ::mlir::Value (*getBiasOrNull)(const Concept *impl, ::mlir::Operation *);
  };
  template<typename ConcreteOp>
  class Model : public Concept {
  public:
    using Interface = ::mlir::pto::MadRawOpInterface;
    Model() : Concept{getLhs, getRhs, getDst, getXt, isMadMxFamily, hasBiasOperand, getBiasOrNull} {}

    static inline ::mlir::Value getLhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getRhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getDst(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getXt(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool isMadMxFamily(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool hasBiasOperand(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getBiasOrNull(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
  };
  template<typename ConcreteOp>
  class FallbackModel : public Concept {
  public:
    using Interface = ::mlir::pto::MadRawOpInterface;
    FallbackModel() : Concept{getLhs, getRhs, getDst, getXt, isMadMxFamily, hasBiasOperand, getBiasOrNull} {}

    static inline ::mlir::Value getLhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getRhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getDst(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getXt(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool isMadMxFamily(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline bool hasBiasOperand(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
    static inline ::mlir::Value getBiasOrNull(const Concept *impl, ::mlir::Operation *tablegen_opaque_val);
  };
  template<typename ConcreteModel, typename ConcreteOp>
  class ExternalModel : public FallbackModel<ConcreteModel> {
  public:
    using ConcreteEntity = ConcreteOp;
    ::mlir::Value getLhs(::mlir::Operation *tablegen_opaque_val) const;
    ::mlir::Value getRhs(::mlir::Operation *tablegen_opaque_val) const;
    ::mlir::Value getDst(::mlir::Operation *tablegen_opaque_val) const;
    ::mlir::Value getXt(::mlir::Operation *tablegen_opaque_val) const;
  };
};
template <typename ConcreteOp>
struct MadRawOpInterfaceTrait;

} // namespace detail
class MadRawOpInterface : public ::mlir::OpInterface<MadRawOpInterface, detail::MadRawOpInterfaceInterfaceTraits> {
public:
  using ::mlir::OpInterface<MadRawOpInterface, detail::MadRawOpInterfaceInterfaceTraits>::OpInterface;
  template <typename ConcreteOp>
  struct Trait : public detail::MadRawOpInterfaceTrait<ConcreteOp> {};
  /// Return the left matrix operand.
  ::mlir::Value getLhs();
  /// Return the right matrix operand.
  ::mlir::Value getRhs();
  /// Return the accumulator/destination operand.
  ::mlir::Value getDst();
  /// Return X_t.
  ::mlir::Value getXt();
  /// Return true for MX MAD forms.
  bool isMadMxFamily();
  /// Return true if the op carries a bias operand.
  bool hasBiasOperand();
  /// Return the bias operand, or null for non-bias forms.
  ::mlir::Value getBiasOrNull();
};
namespace detail {
  template <typename ConcreteOp>
  struct MadRawOpInterfaceTrait : public ::mlir::OpInterface<MadRawOpInterface, detail::MadRawOpInterfaceInterfaceTraits>::Trait<ConcreteOp> {
    /// Return the left matrix operand.
    ::mlir::Value getLhs() {
      return (*static_cast<ConcreteOp *>(this)).getLhs();
    }
    /// Return the right matrix operand.
    ::mlir::Value getRhs() {
      return (*static_cast<ConcreteOp *>(this)).getRhs();
    }
    /// Return the accumulator/destination operand.
    ::mlir::Value getDst() {
      return (*static_cast<ConcreteOp *>(this)).getDst();
    }
    /// Return X_t.
    ::mlir::Value getXt() {
      return (*static_cast<ConcreteOp *>(this)).getXt();
    }
  };
}// namespace detail
} // namespace pto
} // namespace mlir
namespace mlir {
namespace pto {
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getLhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getLhs();
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getRhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getRhs();
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getDst(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getDst();
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getM(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getM();
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getN(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getN();
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getK(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getK();
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::isMadMxFamily(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).isMadMxFamily();
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::hasBiasOperand(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).hasBiasOperand();
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::readsAccumulator(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).readsAccumulator();
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::supportsTf32Mode(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).supportsTf32Mode();
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getBiasOrNull(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getBiasOrNull();
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::initializesAccumulatorWithBias(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).initializesAccumulatorWithBias();
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::initializesAccumulatorWithZero(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).initializesAccumulatorWithZero();
}
template<typename ConcreteOp>
::mlir::Attribute detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getUnitFlagModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getUnitFlagModeAttr();
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getDisableGemv(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getDisableGemv();
}
template<typename ConcreteOp>
::mlir::Attribute detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getSatModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getSatModeAttr();
}
template<typename ConcreteOp>
::mlir::Attribute detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getTf32ModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getTf32ModeAttr();
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::Model<ConcreteOp>::getNDir(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getNDir();
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getLhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getLhs(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getRhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getRhs(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getDst(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getDst(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getM(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getM(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getN(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getN(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getK(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getK(tablegen_opaque_val);
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::isMadMxFamily(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->isMadMxFamily(tablegen_opaque_val);
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::hasBiasOperand(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->hasBiasOperand(tablegen_opaque_val);
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::readsAccumulator(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->readsAccumulator(tablegen_opaque_val);
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::supportsTf32Mode(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->supportsTf32Mode(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getBiasOrNull(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getBiasOrNull(tablegen_opaque_val);
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::initializesAccumulatorWithBias(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->initializesAccumulatorWithBias(tablegen_opaque_val);
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::initializesAccumulatorWithZero(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->initializesAccumulatorWithZero(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Attribute detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getUnitFlagModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getUnitFlagModeAttr(tablegen_opaque_val);
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getDisableGemv(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getDisableGemv(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Attribute detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getSatModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getSatModeAttr(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Attribute detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getTf32ModeAttr(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getTf32ModeAttr(tablegen_opaque_val);
}
template<typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getNDir(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getNDir(tablegen_opaque_val);
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getLhs(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getLhs();
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getRhs(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getRhs();
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getDst(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getDst();
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getM(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getM();
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getN(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getN();
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Value detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getK(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getK();
}
template<typename ConcreteModel, typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::initializesAccumulatorWithBias(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).hasBiasOperand();
}
template<typename ConcreteModel, typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::initializesAccumulatorWithZero(::mlir::Operation *tablegen_opaque_val) const {
return !(llvm::cast<ConcreteOp>(tablegen_opaque_val)).readsAccumulator() && !(llvm::cast<ConcreteOp>(tablegen_opaque_val)).hasBiasOperand();
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Attribute detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getUnitFlagModeAttr(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getUnitFlagModeAttr();
}
template<typename ConcreteModel, typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getDisableGemv(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getDisableGemv();
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Attribute detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getSatModeAttr(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getSatModeAttr();
}
template<typename ConcreteModel, typename ConcreteOp>
bool detail::MadSemanticOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getNDir(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getNDir();
}
} // namespace pto
} // namespace mlir
namespace mlir {
namespace pto {
template<typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::Model<ConcreteOp>::getLhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getLhs();
}
template<typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::Model<ConcreteOp>::getRhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getRhs();
}
template<typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::Model<ConcreteOp>::getDst(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getDst();
}
template<typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::Model<ConcreteOp>::getXt(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getXt();
}
template<typename ConcreteOp>
bool detail::MadRawOpInterfaceInterfaceTraits::Model<ConcreteOp>::isMadMxFamily(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).isMadMxFamily();
}
template<typename ConcreteOp>
bool detail::MadRawOpInterfaceInterfaceTraits::Model<ConcreteOp>::hasBiasOperand(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).hasBiasOperand();
}
template<typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::Model<ConcreteOp>::getBiasOrNull(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getBiasOrNull();
}
template<typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getLhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getLhs(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getRhs(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getRhs(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getDst(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getDst(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getXt(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getXt(tablegen_opaque_val);
}
template<typename ConcreteOp>
bool detail::MadRawOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::isMadMxFamily(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->isMadMxFamily(tablegen_opaque_val);
}
template<typename ConcreteOp>
bool detail::MadRawOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::hasBiasOperand(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->hasBiasOperand(tablegen_opaque_val);
}
template<typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::FallbackModel<ConcreteOp>::getBiasOrNull(const Concept *impl, ::mlir::Operation *tablegen_opaque_val) {
  return static_cast<const ConcreteOp *>(impl)->getBiasOrNull(tablegen_opaque_val);
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getLhs(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getLhs();
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getRhs(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getRhs();
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getDst(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getDst();
}
template<typename ConcreteModel, typename ConcreteOp>
::mlir::Value detail::MadRawOpInterfaceInterfaceTraits::ExternalModel<ConcreteModel, ConcreteOp>::getXt(::mlir::Operation *tablegen_opaque_val) const {
return (llvm::cast<ConcreteOp>(tablegen_opaque_val)).getXt();
}
} // namespace pto
} // namespace mlir
