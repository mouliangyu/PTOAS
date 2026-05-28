// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

#ifndef PTOAS_H
#define PTOAS_H

#include "mlir/IR/BuiltinOps.h"
#include "mlir/Support/LogicalResult.h"
#include "PTO/Transforms/VPTOLLVMEmitter.h"
#include "llvm/ADT/StringRef.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace mlir::pto {

enum class PTOASBackend {
  EmitC,
  VPTO,
};

enum class PTOASCompileResultKind {
  Text,
  VPTOObject,
  MixedObject,
};

enum class PTOASBackendObjectKind {
  EmitCFatobj,
  VPTOVectorDeviceObject,
  VPTOCubeDeviceObject,
};

struct PTOASBackendObjectInput {
  PTOASBackendObjectKind kind = PTOASBackendObjectKind::EmitCFatobj;
  std::string cppSource;
  EmittedLLVMModule llvmModule;
};

struct PTOASCompileResult {
  PTOASCompileResultKind kind = PTOASCompileResultKind::Text;
  std::string textOutput;
  std::string stubSource;
  std::vector<PTOASBackendObjectInput> backendObjects;
};

llvm::StringRef getPTOASTargetArchOption();
LogicalResult getPTOASCommandLineBackend(PTOASBackend &backend);
bool isPTOASDebugIROutputRequested();
std::string normalizePTOASArch(llvm::StringRef archValue);
bool isSupportedPTOASArch(llvm::StringRef archValue);
std::optional<std::string> detectPTOASTextualModuleArch(llvm::StringRef text);

LogicalResult compilePTOASModule(ModuleOp module, llvm::StringRef arch,
                                 bool cliBackendSpecified, int argc,
                                 char **argv,
                                 PTOASCompileResult &result,
                                 bool emitVPTOHostStub = true);

} // namespace mlir::pto

#endif
