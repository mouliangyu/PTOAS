/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Interface Definitions                                                      *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

/// Return the left matrix operand.
::mlir::Value mlir::pto::MadRawOpInterface::getLhs() {
      return getImpl()->getLhs(getImpl(), getOperation());
  }
/// Return the right matrix operand.
::mlir::Value mlir::pto::MadRawOpInterface::getRhs() {
      return getImpl()->getRhs(getImpl(), getOperation());
  }
/// Return the accumulator/destination operand.
::mlir::Value mlir::pto::MadRawOpInterface::getDst() {
      return getImpl()->getDst(getImpl(), getOperation());
  }
/// Return X_t.
::mlir::Value mlir::pto::MadRawOpInterface::getXt() {
      return getImpl()->getXt(getImpl(), getOperation());
  }
/// Return true for MX MAD forms.
bool mlir::pto::MadRawOpInterface::isMadMxFamily() {
      return getImpl()->isMadMxFamily(getImpl(), getOperation());
  }
/// Return true if the op carries a bias operand.
bool mlir::pto::MadRawOpInterface::hasBiasOperand() {
      return getImpl()->hasBiasOperand(getImpl(), getOperation());
  }
/// Return the bias operand, or null for non-bias forms.
::mlir::Value mlir::pto::MadRawOpInterface::getBiasOrNull() {
      return getImpl()->getBiasOrNull(getImpl(), getOperation());
  }
/// Return the left matrix operand.
::mlir::Value mlir::pto::MadSemanticOpInterface::getLhs() {
      return getImpl()->getLhs(getImpl(), getOperation());
  }
/// Return the right matrix operand.
::mlir::Value mlir::pto::MadSemanticOpInterface::getRhs() {
      return getImpl()->getRhs(getImpl(), getOperation());
  }
/// Return the accumulator/destination operand.
::mlir::Value mlir::pto::MadSemanticOpInterface::getDst() {
      return getImpl()->getDst(getImpl(), getOperation());
  }
/// Return M.
::mlir::Value mlir::pto::MadSemanticOpInterface::getM() {
      return getImpl()->getM(getImpl(), getOperation());
  }
/// Return N.
::mlir::Value mlir::pto::MadSemanticOpInterface::getN() {
      return getImpl()->getN(getImpl(), getOperation());
  }
/// Return K.
::mlir::Value mlir::pto::MadSemanticOpInterface::getK() {
      return getImpl()->getK(getImpl(), getOperation());
  }
/// Return true for MX MAD forms.
bool mlir::pto::MadSemanticOpInterface::isMadMxFamily() {
      return getImpl()->isMadMxFamily(getImpl(), getOperation());
  }
/// Return true if the op carries a bias operand.
bool mlir::pto::MadSemanticOpInterface::hasBiasOperand() {
      return getImpl()->hasBiasOperand(getImpl(), getOperation());
  }
/// Return true if the op reads the existing accumulator.
bool mlir::pto::MadSemanticOpInterface::readsAccumulator() {
      return getImpl()->readsAccumulator(getImpl(), getOperation());
  }
/// Return true if TF32 mode is a legal semantic attribute.
bool mlir::pto::MadSemanticOpInterface::supportsTf32Mode() {
      return getImpl()->supportsTf32Mode(getImpl(), getOperation());
  }
/// Return the bias operand, or null for non-bias forms.
::mlir::Value mlir::pto::MadSemanticOpInterface::getBiasOrNull() {
      return getImpl()->getBiasOrNull(getImpl(), getOperation());
  }
/// Return true if the accumulator source is bias.
bool mlir::pto::MadSemanticOpInterface::initializesAccumulatorWithBias() {
      return getImpl()->initializesAccumulatorWithBias(getImpl(), getOperation());
  }
/// Return true if the accumulator is initialized to zero.
bool mlir::pto::MadSemanticOpInterface::initializesAccumulatorWithZero() {
      return getImpl()->initializesAccumulatorWithZero(getImpl(), getOperation());
  }
/// Return unit_flag_mode.
::mlir::Attribute mlir::pto::MadSemanticOpInterface::getUnitFlagModeAttr() {
      return getImpl()->getUnitFlagModeAttr(getImpl(), getOperation());
  }
/// Return true if disable_gemv is set.
bool mlir::pto::MadSemanticOpInterface::getDisableGemv() {
      return getImpl()->getDisableGemv(getImpl(), getOperation());
  }
/// Return sat_mode.
::mlir::Attribute mlir::pto::MadSemanticOpInterface::getSatModeAttr() {
      return getImpl()->getSatModeAttr(getImpl(), getOperation());
  }
/// Return tf32_mode.
::mlir::Attribute mlir::pto::MadSemanticOpInterface::getTf32ModeAttr() {
      return getImpl()->getTf32ModeAttr(getImpl(), getOperation());
  }
/// Return true if n_dir is set.
bool mlir::pto::MadSemanticOpInterface::getNDir() {
      return getImpl()->getNDir(getImpl(), getOperation());
  }
