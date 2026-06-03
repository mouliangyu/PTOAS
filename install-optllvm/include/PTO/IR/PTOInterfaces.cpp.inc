/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Interface Definitions                                                      *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

/// Get the hardware pipe where this operation executes.
::mlir::pto::PIPE mlir::pto::OpPipeInterface::getPipe() {
      return getImpl()->getPipe(getImpl(), getOperation());
  }
/// Return the mutable range of destination (init) operands.
::mlir::MutableOperandRange mlir::pto::PTO_DpsInitOpInterface::getDpsInitsMutable() {
      return getImpl()->getDpsInitsMutable(getImpl(), getOperation());
  }
