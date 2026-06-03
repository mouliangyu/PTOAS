/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* AttrDef Declarations                                                       *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifdef GET_ATTRDEF_CLASSES
#undef GET_ATTRDEF_CLASSES


namespace mlir {
class AsmParser;
class AsmPrinter;
} // namespace mlir
namespace mlir {
namespace pto {
class AddressSpaceAttr;
class SignednessAttr;
class LdL2CacheAttr;
class StL2CacheAttr;
class L1CacheAttr;
class RoundingAttr;
class SaturationAttr;
class PipeAttr;
class MemBarAttr;
class PipeEventTypeAttr;
class SyncOpTypeAttr;
class LayoutAttr;
class FunctionKernelKindAttr;
class TFuncCoreTypeAttr;
class TModuleCoreTypeAttr;
class EventAttr;
class PadModeAttr;
class VectorFunctionAttr;
class CmpModeAttr;
class RoundModeAttr;
class PrecisionModeAttr;
class MadUnitFlagModeAttr;
class Tf32ModeAttr;
class MadSatModeAttr;
class SaturationModeAttr;
class STPhaseAttr;
class AtomicTypeAttr;
class NotifyOpAttr;
class WaitCmpAttr;
class ReduceOpAttr;
class ReluPreModeAttr;
class AccStoreUnitFlagCtrlAttr;
class AccStoreQuantPreModeAttr;
class AccStoreAtomicTypeAttr;
class AccStoreAtomicOpAttr;
class AccStoreModeAttr;
class AccStoreUbDstModeAttr;
class AccStoreSatModeAttr;
class CubeLoadFracModeAttr;
class GatherOOBAttr;
class ScatterAtomicOpAttr;
class ScatterOOBAttr;
class AccToVecModeAttr;
class MaskPatternAttr;
class BLayoutAttr;
class SLayoutAttr;
class PadValueAttr;
class CompactModeAttr;
class TileBufConfigAttr;
class QuantTypeAttr;
namespace detail {
struct AddressSpaceAttrStorage;
} // namespace detail
class AddressSpaceAttr : public ::mlir::Attribute::AttrBase<AddressSpaceAttr, ::mlir::Attribute, detail::AddressSpaceAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.address_space";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static AddressSpaceAttr get(::mlir::MLIRContext *context, ::mlir::pto::AddressSpace address_space);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"address_space"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::AddressSpace getAddressSpace() const;
};
namespace detail {
struct SignednessAttrStorage;
} // namespace detail
class SignednessAttr : public ::mlir::Attribute::AttrBase<SignednessAttr, ::mlir::Attribute, detail::SignednessAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.signedness";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static SignednessAttr get(::mlir::MLIRContext *context, ::mlir::pto::Signedness value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"signedness"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::Signedness getValue() const;
};
namespace detail {
struct LdL2CacheAttrStorage;
} // namespace detail
class LdL2CacheAttr : public ::mlir::Attribute::AttrBase<LdL2CacheAttr, ::mlir::Attribute, detail::LdL2CacheAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.ld_l2cache";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static LdL2CacheAttr get(::mlir::MLIRContext *context, ::mlir::pto::LdL2Cache value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"ld_l2cache"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::LdL2Cache getValue() const;
};
namespace detail {
struct StL2CacheAttrStorage;
} // namespace detail
class StL2CacheAttr : public ::mlir::Attribute::AttrBase<StL2CacheAttr, ::mlir::Attribute, detail::StL2CacheAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.st_l2cache";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static StL2CacheAttr get(::mlir::MLIRContext *context, ::mlir::pto::StL2Cache value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"st_l2cache"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::StL2Cache getValue() const;
};
namespace detail {
struct L1CacheAttrStorage;
} // namespace detail
class L1CacheAttr : public ::mlir::Attribute::AttrBase<L1CacheAttr, ::mlir::Attribute, detail::L1CacheAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.l1cache";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static L1CacheAttr get(::mlir::MLIRContext *context, ::mlir::pto::L1Cache value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"l1cache"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::L1Cache getValue() const;
};
namespace detail {
struct RoundingAttrStorage;
} // namespace detail
class RoundingAttr : public ::mlir::Attribute::AttrBase<RoundingAttr, ::mlir::Attribute, detail::RoundingAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.rounding";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static RoundingAttr get(::mlir::MLIRContext *context, ::mlir::pto::Rounding value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"rounding"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::Rounding getValue() const;
};
namespace detail {
struct SaturationAttrStorage;
} // namespace detail
class SaturationAttr : public ::mlir::Attribute::AttrBase<SaturationAttr, ::mlir::Attribute, detail::SaturationAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.saturation";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static SaturationAttr get(::mlir::MLIRContext *context, ::mlir::pto::Saturation value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"saturation"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::Saturation getValue() const;
};
namespace detail {
struct PipeAttrStorage;
} // namespace detail
class PipeAttr : public ::mlir::Attribute::AttrBase<PipeAttr, ::mlir::Attribute, detail::PipeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.pipe";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static PipeAttr get(::mlir::MLIRContext *context, ::mlir::pto::PIPE pipe);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"pipe"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::PIPE getPipe() const;
};
namespace detail {
struct MemBarAttrStorage;
} // namespace detail
class MemBarAttr : public ::mlir::Attribute::AttrBase<MemBarAttr, ::mlir::Attribute, detail::MemBarAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.membar";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static MemBarAttr get(::mlir::MLIRContext *context, ::mlir::pto::MemBarKind kind);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"membar"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::MemBarKind getKind() const;
};
namespace detail {
struct PipeEventTypeAttrStorage;
} // namespace detail
class PipeEventTypeAttr : public ::mlir::Attribute::AttrBase<PipeEventTypeAttr, ::mlir::Attribute, detail::PipeEventTypeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.pipe_event_type";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static PipeEventTypeAttr get(::mlir::MLIRContext *context, ::mlir::pto::SyncOpType op_type);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"pipe_event_type"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::SyncOpType getOpType() const;
};
namespace detail {
struct SyncOpTypeAttrStorage;
} // namespace detail
class SyncOpTypeAttr : public ::mlir::Attribute::AttrBase<SyncOpTypeAttr, ::mlir::Attribute, detail::SyncOpTypeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.sync_op_type";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static SyncOpTypeAttr get(::mlir::MLIRContext *context, ::mlir::pto::SyncOpType op_type);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"sync_op_type"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::SyncOpType getOpType() const;
};
namespace detail {
struct LayoutAttrStorage;
} // namespace detail
class LayoutAttr : public ::mlir::Attribute::AttrBase<LayoutAttr, ::mlir::Attribute, detail::LayoutAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.layout";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static LayoutAttr get(::mlir::MLIRContext *context, ::mlir::pto::Layout layout);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"layout"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::Layout getLayout() const;
};
namespace detail {
struct FunctionKernelKindAttrStorage;
} // namespace detail
class FunctionKernelKindAttr : public ::mlir::Attribute::AttrBase<FunctionKernelKindAttr, ::mlir::Attribute, detail::FunctionKernelKindAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.kernel_kind";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static FunctionKernelKindAttr get(::mlir::MLIRContext *context, ::mlir::pto::FunctionKernelKind kernelKind);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"kernel_kind"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::FunctionKernelKind getKernelKind() const;
};
namespace detail {
struct TFuncCoreTypeAttrStorage;
} // namespace detail
class TFuncCoreTypeAttr : public ::mlir::Attribute::AttrBase<TFuncCoreTypeAttr, ::mlir::Attribute, detail::TFuncCoreTypeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.func_core_type";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static TFuncCoreTypeAttr get(::mlir::MLIRContext *context, ::mlir::pto::TFuncCoreType funcCoreType);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"func_core_type"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::TFuncCoreType getFuncCoreType() const;
};
namespace detail {
struct TModuleCoreTypeAttrStorage;
} // namespace detail
class TModuleCoreTypeAttr : public ::mlir::Attribute::AttrBase<TModuleCoreTypeAttr, ::mlir::Attribute, detail::TModuleCoreTypeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.module_core_type";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static TModuleCoreTypeAttr get(::mlir::MLIRContext *context, ::mlir::pto::TModuleCoreType moduleCoreType);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"module_core_type"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::TModuleCoreType getModuleCoreType() const;
};
namespace detail {
struct EventAttrStorage;
} // namespace detail
class EventAttr : public ::mlir::Attribute::AttrBase<EventAttr, ::mlir::Attribute, detail::EventAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.event";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static EventAttr get(::mlir::MLIRContext *context, ::mlir::pto::EVENT event);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"event"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::EVENT getEvent() const;
};
namespace detail {
struct PadModeAttrStorage;
} // namespace detail
class PadModeAttr : public ::mlir::Attribute::AttrBase<PadModeAttr, ::mlir::Attribute, detail::PadModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.padmode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static PadModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::PadMode padmode);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"padmode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::PadMode getPadmode() const;
};
class VectorFunctionAttr : public ::mlir::Attribute::AttrBase<VectorFunctionAttr, ::mlir::Attribute, ::mlir::AttributeStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.vector_function";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"vector_function"};
  }

};
namespace detail {
struct CmpModeAttrStorage;
} // namespace detail
class CmpModeAttr : public ::mlir::Attribute::AttrBase<CmpModeAttr, ::mlir::Attribute, detail::CmpModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.cmp";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static CmpModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::CmpMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"cmp"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::CmpMode getValue() const;
};
namespace detail {
struct RoundModeAttrStorage;
} // namespace detail
class RoundModeAttr : public ::mlir::Attribute::AttrBase<RoundModeAttr, ::mlir::Attribute, detail::RoundModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.round_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static RoundModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::RoundMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"round_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::RoundMode getValue() const;
};
namespace detail {
struct PrecisionModeAttrStorage;
} // namespace detail
class PrecisionModeAttr : public ::mlir::Attribute::AttrBase<PrecisionModeAttr, ::mlir::Attribute, detail::PrecisionModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.precision_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static PrecisionModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::PrecisionMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"precision_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::PrecisionMode getValue() const;
};
namespace detail {
struct MadUnitFlagModeAttrStorage;
} // namespace detail
class MadUnitFlagModeAttr : public ::mlir::Attribute::AttrBase<MadUnitFlagModeAttr, ::mlir::Attribute, detail::MadUnitFlagModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.mad_unit_flag_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static MadUnitFlagModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::MadUnitFlagMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"mad_unit_flag_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::MadUnitFlagMode getValue() const;
};
namespace detail {
struct Tf32ModeAttrStorage;
} // namespace detail
class Tf32ModeAttr : public ::mlir::Attribute::AttrBase<Tf32ModeAttr, ::mlir::Attribute, detail::Tf32ModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.tf32_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static Tf32ModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::Tf32Mode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"tf32_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::Tf32Mode getValue() const;
};
namespace detail {
struct MadSatModeAttrStorage;
} // namespace detail
class MadSatModeAttr : public ::mlir::Attribute::AttrBase<MadSatModeAttr, ::mlir::Attribute, detail::MadSatModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.mad_sat_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static MadSatModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::MadSatMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"mad_sat_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::MadSatMode getValue() const;
};
namespace detail {
struct SaturationModeAttrStorage;
} // namespace detail
class SaturationModeAttr : public ::mlir::Attribute::AttrBase<SaturationModeAttr, ::mlir::Attribute, detail::SaturationModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.saturation_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static SaturationModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::SaturationMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"saturation_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::SaturationMode getValue() const;
};
namespace detail {
struct STPhaseAttrStorage;
} // namespace detail
class STPhaseAttr : public ::mlir::Attribute::AttrBase<STPhaseAttr, ::mlir::Attribute, detail::STPhaseAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.st_phase";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static STPhaseAttr get(::mlir::MLIRContext *context, ::mlir::pto::STPhase value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"st_phase"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::STPhase getValue() const;
};
namespace detail {
struct AtomicTypeAttrStorage;
} // namespace detail
class AtomicTypeAttr : public ::mlir::Attribute::AttrBase<AtomicTypeAttr, ::mlir::Attribute, detail::AtomicTypeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.atomic_type";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static AtomicTypeAttr get(::mlir::MLIRContext *context, ::mlir::pto::AtomicType value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"atomic_type"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::AtomicType getValue() const;
};
namespace detail {
struct NotifyOpAttrStorage;
} // namespace detail
class NotifyOpAttr : public ::mlir::Attribute::AttrBase<NotifyOpAttr, ::mlir::Attribute, detail::NotifyOpAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.notify_op";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static NotifyOpAttr get(::mlir::MLIRContext *context, ::mlir::pto::NotifyOp value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"notify_op"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::NotifyOp getValue() const;
};
namespace detail {
struct WaitCmpAttrStorage;
} // namespace detail
class WaitCmpAttr : public ::mlir::Attribute::AttrBase<WaitCmpAttr, ::mlir::Attribute, detail::WaitCmpAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.wait_cmp";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static WaitCmpAttr get(::mlir::MLIRContext *context, ::mlir::pto::WaitCmp value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"wait_cmp"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::WaitCmp getValue() const;
};
namespace detail {
struct ReduceOpAttrStorage;
} // namespace detail
class ReduceOpAttr : public ::mlir::Attribute::AttrBase<ReduceOpAttr, ::mlir::Attribute, detail::ReduceOpAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.reduce_op";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static ReduceOpAttr get(::mlir::MLIRContext *context, ::mlir::pto::ReduceOp value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"reduce_op"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::ReduceOp getValue() const;
};
namespace detail {
struct ReluPreModeAttrStorage;
} // namespace detail
class ReluPreModeAttr : public ::mlir::Attribute::AttrBase<ReluPreModeAttr, ::mlir::Attribute, detail::ReluPreModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.relu_pre_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static ReluPreModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::ReluPreMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"relu_pre_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::ReluPreMode getValue() const;
};
namespace detail {
struct AccStoreUnitFlagCtrlAttrStorage;
} // namespace detail
class AccStoreUnitFlagCtrlAttr : public ::mlir::Attribute::AttrBase<AccStoreUnitFlagCtrlAttr, ::mlir::Attribute, detail::AccStoreUnitFlagCtrlAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.unit_flag_ctrl";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static AccStoreUnitFlagCtrlAttr get(::mlir::MLIRContext *context, ::mlir::pto::AccStoreUnitFlagCtrl value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"unit_flag_ctrl"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::AccStoreUnitFlagCtrl getValue() const;
};
namespace detail {
struct AccStoreQuantPreModeAttrStorage;
} // namespace detail
class AccStoreQuantPreModeAttr : public ::mlir::Attribute::AttrBase<AccStoreQuantPreModeAttr, ::mlir::Attribute, detail::AccStoreQuantPreModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.quant_pre_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static AccStoreQuantPreModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::AccStoreQuantPreMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"quant_pre_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::AccStoreQuantPreMode getValue() const;
};
namespace detail {
struct AccStoreAtomicTypeAttrStorage;
} // namespace detail
class AccStoreAtomicTypeAttr : public ::mlir::Attribute::AttrBase<AccStoreAtomicTypeAttr, ::mlir::Attribute, detail::AccStoreAtomicTypeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.acc_store_atomic_type";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static AccStoreAtomicTypeAttr get(::mlir::MLIRContext *context, ::mlir::pto::AccStoreAtomicType value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"acc_store_atomic_type"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::AccStoreAtomicType getValue() const;
};
namespace detail {
struct AccStoreAtomicOpAttrStorage;
} // namespace detail
class AccStoreAtomicOpAttr : public ::mlir::Attribute::AttrBase<AccStoreAtomicOpAttr, ::mlir::Attribute, detail::AccStoreAtomicOpAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.acc_store_atomic_op";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static AccStoreAtomicOpAttr get(::mlir::MLIRContext *context, ::mlir::pto::AccStoreAtomicOp value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"acc_store_atomic_op"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::AccStoreAtomicOp getValue() const;
};
namespace detail {
struct AccStoreModeAttrStorage;
} // namespace detail
class AccStoreModeAttr : public ::mlir::Attribute::AttrBase<AccStoreModeAttr, ::mlir::Attribute, detail::AccStoreModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.acc_store_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static AccStoreModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::AccStoreMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"acc_store_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::AccStoreMode getValue() const;
};
namespace detail {
struct AccStoreUbDstModeAttrStorage;
} // namespace detail
class AccStoreUbDstModeAttr : public ::mlir::Attribute::AttrBase<AccStoreUbDstModeAttr, ::mlir::Attribute, detail::AccStoreUbDstModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.acc_store_ub_dst_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static AccStoreUbDstModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::AccStoreUbDstMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"acc_store_ub_dst_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::AccStoreUbDstMode getValue() const;
};
namespace detail {
struct AccStoreSatModeAttrStorage;
} // namespace detail
class AccStoreSatModeAttr : public ::mlir::Attribute::AttrBase<AccStoreSatModeAttr, ::mlir::Attribute, detail::AccStoreSatModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.acc_store_sat_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static AccStoreSatModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::AccStoreSatMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"acc_store_sat_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::AccStoreSatMode getValue() const;
};
namespace detail {
struct CubeLoadFracModeAttrStorage;
} // namespace detail
class CubeLoadFracModeAttr : public ::mlir::Attribute::AttrBase<CubeLoadFracModeAttr, ::mlir::Attribute, detail::CubeLoadFracModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.cube_load_frac_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static CubeLoadFracModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::CubeLoadFracMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"cube_load_frac_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::CubeLoadFracMode getValue() const;
};
namespace detail {
struct GatherOOBAttrStorage;
} // namespace detail
class GatherOOBAttr : public ::mlir::Attribute::AttrBase<GatherOOBAttr, ::mlir::Attribute, detail::GatherOOBAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.gather_oob";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static GatherOOBAttr get(::mlir::MLIRContext *context, ::mlir::pto::GatherOOB value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"gather_oob"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::GatherOOB getValue() const;
};
namespace detail {
struct ScatterAtomicOpAttrStorage;
} // namespace detail
class ScatterAtomicOpAttr : public ::mlir::Attribute::AttrBase<ScatterAtomicOpAttr, ::mlir::Attribute, detail::ScatterAtomicOpAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.scatter_atomic_op";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static ScatterAtomicOpAttr get(::mlir::MLIRContext *context, ::mlir::pto::ScatterAtomicOp value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"scatter_atomic_op"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::ScatterAtomicOp getValue() const;
};
namespace detail {
struct ScatterOOBAttrStorage;
} // namespace detail
class ScatterOOBAttr : public ::mlir::Attribute::AttrBase<ScatterOOBAttr, ::mlir::Attribute, detail::ScatterOOBAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.scatter_oob";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static ScatterOOBAttr get(::mlir::MLIRContext *context, ::mlir::pto::ScatterOOB value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"scatter_oob"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::ScatterOOB getValue() const;
};
namespace detail {
struct AccToVecModeAttrStorage;
} // namespace detail
class AccToVecModeAttr : public ::mlir::Attribute::AttrBase<AccToVecModeAttr, ::mlir::Attribute, detail::AccToVecModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.acc_to_vec_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static AccToVecModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::AccToVecMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"acc_to_vec_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::AccToVecMode getValue() const;
};
namespace detail {
struct MaskPatternAttrStorage;
} // namespace detail
class MaskPatternAttr : public ::mlir::Attribute::AttrBase<MaskPatternAttr, ::mlir::Attribute, detail::MaskPatternAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.mask_pattern";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static MaskPatternAttr get(::mlir::MLIRContext *context, ::mlir::pto::MaskPattern value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"mask_pattern"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::MaskPattern getValue() const;
};
namespace detail {
struct BLayoutAttrStorage;
} // namespace detail
class BLayoutAttr : public ::mlir::Attribute::AttrBase<BLayoutAttr, ::mlir::Attribute, detail::BLayoutAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.blayout";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static BLayoutAttr get(::mlir::MLIRContext *context, ::mlir::pto::BLayout value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"blayout"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::BLayout getValue() const;
};
namespace detail {
struct SLayoutAttrStorage;
} // namespace detail
class SLayoutAttr : public ::mlir::Attribute::AttrBase<SLayoutAttr, ::mlir::Attribute, detail::SLayoutAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.slayout";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static SLayoutAttr get(::mlir::MLIRContext *context, ::mlir::pto::SLayout value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"slayout"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::SLayout getValue() const;
};
namespace detail {
struct PadValueAttrStorage;
} // namespace detail
class PadValueAttr : public ::mlir::Attribute::AttrBase<PadValueAttr, ::mlir::Attribute, detail::PadValueAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.pad_value";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static PadValueAttr get(::mlir::MLIRContext *context, ::mlir::pto::PadValue value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"pad_value"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::PadValue getValue() const;
};
namespace detail {
struct CompactModeAttrStorage;
} // namespace detail
class CompactModeAttr : public ::mlir::Attribute::AttrBase<CompactModeAttr, ::mlir::Attribute, detail::CompactModeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.compact_mode";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static CompactModeAttr get(::mlir::MLIRContext *context, ::mlir::pto::CompactMode value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"compact_mode"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::CompactMode getValue() const;
};
namespace detail {
struct TileBufConfigAttrStorage;
} // namespace detail
class TileBufConfigAttr : public ::mlir::Attribute::AttrBase<TileBufConfigAttr, ::mlir::Attribute, detail::TileBufConfigAttrStorage> {
public:
  using Base::Base;
  static TileBufConfigAttr getDefault(MLIRContext *ctx);
  bool isDefault() const;

  static LogicalResult verify(function_ref<InFlightDiagnostic()> emitError,
                              mlir::Attribute bLayout,
                              mlir::Attribute sLayout,
                              mlir::IntegerAttr sFractalSize,
                              mlir::Attribute pad,
                              mlir::Attribute compactMode);
  static constexpr ::llvm::StringLiteral name = "pto.tile_buf_config";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static TileBufConfigAttr get(::mlir::MLIRContext *context, BLayoutAttr bLayout, SLayoutAttr sLayout, mlir::IntegerAttr sFractalSize, PadValueAttr pad, CompactModeAttr compactMode);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"tile_buf_config"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  BLayoutAttr getBLayout() const;
  SLayoutAttr getSLayout() const;
  mlir::IntegerAttr getSFractalSize() const;
  PadValueAttr getPad() const;
  CompactModeAttr getCompactMode() const;
};
namespace detail {
struct QuantTypeAttrStorage;
} // namespace detail
class QuantTypeAttr : public ::mlir::Attribute::AttrBase<QuantTypeAttr, ::mlir::Attribute, detail::QuantTypeAttrStorage> {
public:
  using Base::Base;
  static constexpr ::llvm::StringLiteral name = "pto.quant_type";
  static constexpr ::llvm::StringLiteral dialectName = "pto";
  static QuantTypeAttr get(::mlir::MLIRContext *context, ::mlir::pto::QuantType value);
  static constexpr ::llvm::StringLiteral getMnemonic() {
    return {"quant_type"};
  }

  static ::mlir::Attribute parse(::mlir::AsmParser &odsParser, ::mlir::Type odsType);
  void print(::mlir::AsmPrinter &odsPrinter) const;
  ::mlir::pto::QuantType getValue() const;
};
} // namespace pto
} // namespace mlir
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AddressSpaceAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::SignednessAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::LdL2CacheAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::StL2CacheAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::L1CacheAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::RoundingAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::SaturationAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::PipeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::MemBarAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::PipeEventTypeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::SyncOpTypeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::LayoutAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::FunctionKernelKindAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::TFuncCoreTypeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::TModuleCoreTypeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::EventAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::PadModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::VectorFunctionAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::CmpModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::RoundModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::PrecisionModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::MadUnitFlagModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::Tf32ModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::MadSatModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::SaturationModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::STPhaseAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AtomicTypeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::NotifyOpAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::WaitCmpAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::ReduceOpAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::ReluPreModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AccStoreUnitFlagCtrlAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AccStoreQuantPreModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AccStoreAtomicTypeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AccStoreAtomicOpAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AccStoreModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AccStoreUbDstModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AccStoreSatModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::CubeLoadFracModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::GatherOOBAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::ScatterAtomicOpAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::ScatterOOBAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::AccToVecModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::MaskPatternAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::BLayoutAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::SLayoutAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::PadValueAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::CompactModeAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::TileBufConfigAttr)
MLIR_DECLARE_EXPLICIT_TYPE_ID(::mlir::pto::QuantTypeAttr)

#endif  // GET_ATTRDEF_CLASSES

