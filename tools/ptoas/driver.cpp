// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

#include "ptoas.h"

#include "ObjectEmission.h"
#include "PTO/IR/PTO.h"
#include "PTO/Transforms/BufferizableOpInterfaceImpl.h"
#include "PTO/Transforms/Passes.h"
#include "VPTOHostStubEmission.h"
#include "mlir/Dialect/Bufferization/Transforms/OneShotAnalysis.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Math/IR/Math.h"
#include "mlir/Dialect/Arith/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlowOps.h"
#include "mlir/Dialect/EmitC/IR/EmitC.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/Tensor/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/SymbolTable.h"
#include "mlir/InitAllPasses.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Parser/Parser.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Process.h"
#include "llvm/ADT/ScopeExit.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/SourceMgr.h"
#include "ptobc/ptobc_decode.h"
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using namespace mlir;

#ifndef PTOAS_RELEASE_VERSION
#define PTOAS_RELEASE_VERSION "unknown"
#endif

static llvm::cl::opt<std::string> inputFilename(llvm::cl::Positional,
                                                llvm::cl::desc("<input file>"),
                                                llvm::cl::init("-"));

static llvm::cl::opt<std::string> outputFilename(
    "o", llvm::cl::desc("Output filename"), llvm::cl::value_desc("filename"),
    llvm::cl::init("-"));

static void printPTOASVersion(llvm::raw_ostream &os) {
  os << "ptoas " << PTOAS_RELEASE_VERSION << "\n";
}

static void registerPTOASDriverDialects(DialectRegistry &registry) {
  registry.insert<mlir::func::FuncDialect>();
  registry.insert<mlir::tensor::TensorDialect>();
  registry.insert<mlir::arith::ArithDialect>();
  registry.insert<mlir::memref::MemRefDialect>();
  registry.insert<mlir::affine::AffineDialect>();
  registry.insert<mlir::cf::ControlFlowDialect>();
  registry.insert<mlir::bufferization::BufferizationDialect>();
  registry.insert<mlir::scf::SCFDialect>();
  registry.insert<mlir::math::MathDialect>();

  registry.insert<mlir::pto::PTODialect>();
  arith::registerBufferizableOpInterfaceExternalModels(registry);
  tensor::registerBufferizableOpInterfaceExternalModels(registry);
  mlir::pto::registerBufferizableOpInterfaceExternalModels(registry);

  registry.insert<emitc::EmitCDialect>();
  registry.insert<mlir::LLVM::LLVMDialect>();
}

static void registerPTOASDriverPassesAndCLOptions() {
  mlir::registerAllPasses();
  mlir::pto::registerPTOPasses();
  mlir::pto::registerPTOViewToMemrefPass();
  mlir::pto::registerPTOInlineLibCall();
  mlir::pto::registerFoldTileBufIntrinsics();
  mlir::pto::registerExpandTileOp();
  mlir::registerPassManagerCLOptions();
}

static bool hasCLIOption(int argc, char **argv, llvm::StringRef option) {
  const std::string optionWithValue = (option + "=").str();
  for (int i = 1; i < argc; ++i) {
    llvm::StringRef arg(argv[i]);
    if (arg == option || arg.starts_with(optionWithValue))
      return true;
  }
  return false;
}

static void loadPTOASDialects(MLIRContext &context) {
  context.getOrLoadDialect<emitc::EmitCDialect>();
  context.getOrLoadDialect<mlir::pto::PTODialect>();
  context.getOrLoadDialect<func::FuncDialect>();
  context.getOrLoadDialect<arith::ArithDialect>();
  context.getOrLoadDialect<math::MathDialect>();
  context.getOrLoadDialect<memref::MemRefDialect>();
  context.getOrLoadDialect<affine::AffineDialect>();
  context.getOrLoadDialect<mlir::LLVM::LLVMDialect>();
}

static LogicalResult createTempPath(llvm::StringRef prefix,
                                    llvm::StringRef suffix,
                                    std::string &path) {
  llvm::SmallString<128> tempPath;
  int fd = -1;
  std::error_code ec =
      llvm::sys::fs::createTemporaryFile(prefix, suffix, fd, tempPath);
  if (ec) {
    llvm::errs() << "Error: failed to create temporary file for " << prefix
                 << suffix << ": " << ec.message() << "\n";
    return failure();
  }
  llvm::sys::Process::SafelyCloseFileDescriptor(fd);
  path = tempPath.str().str();
  return success();
}

static void removeTempPaths(ArrayRef<std::string> paths) {
  for (const std::string &path : paths)
    if (!path.empty())
      llvm::sys::fs::remove(path);
}

static std::string sanitizeModuleId(llvm::StringRef outputPath) {
  std::string moduleId =
      outputPath.empty() || outputPath == "-" ? "ptoas_fatobj" : outputPath.str();
  for (char &c : moduleId)
    if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_')
      c = '_';
  return moduleId;
}

struct BackendExportPlan {
  std::string sourceName;
  std::string abiName;
  FunctionType type;
};

struct BackendImportPlan {
  std::string sourceName;
  std::string abiName;
  FunctionType type;
};

struct BackendChildPlan {
  ModuleOp module;
  OwningOpRef<ModuleOp> ownedModule;
  mlir::pto::PTOASBackend backend = mlir::pto::PTOASBackend::EmitC;
  std::optional<mlir::pto::FunctionKernelKind> kernelKind;
  SmallVector<BackendExportPlan, 4> exports;
  SmallVector<BackendImportPlan, 4> imports;
};

static bool parseDriverBackend(llvm::StringRef backendStr,
                               mlir::pto::PTOASBackend &out) {
  std::string s = backendStr.str();
  for (char &c : s)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  if (s == "emitc") {
    out = mlir::pto::PTOASBackend::EmitC;
    return true;
  }
  if (s == "vpto") {
    out = mlir::pto::PTOASBackend::VPTO;
    return true;
  }
  return false;
}

static LogicalResult parseDriverBackendAttr(
    Operation *op, std::optional<mlir::pto::PTOASBackend> &backend) {
  backend = std::nullopt;
  Attribute rawBackendAttr = op->getAttr("pto.backend");
  if (!rawBackendAttr)
    return success();

  auto backendAttr = dyn_cast<StringAttr>(rawBackendAttr);
  if (!backendAttr) {
    return op->emitError("invalid pto.backend attribute. Expected string "
                         "value 'emitc' or 'vpto'.");
  }

  mlir::pto::PTOASBackend attrBackend = mlir::pto::PTOASBackend::EmitC;
  if (!parseDriverBackend(backendAttr.getValue(), attrBackend)) {
    return op->emitError("invalid pto.backend '")
           << backendAttr.getValue() << "'. Expected 'emitc' or 'vpto'.";
  }

  backend = attrBackend;
  return success();
}

static LogicalResult resolveDriverModuleBackend(
    ModuleOp module, bool cliBackendSpecified,
    mlir::pto::PTOASBackend &effectiveBackend, bool &mixedBackendMode) {
  mixedBackendMode = false;
  if (cliBackendSpecified)
    return success();

  std::optional<mlir::pto::PTOASBackend> topBackend;
  if (failed(parseDriverBackendAttr(module.getOperation(), topBackend)))
    return failure();
  if (topBackend) {
    effectiveBackend = *topBackend;
    return success();
  }

  std::optional<mlir::pto::PTOASBackend> childBackend;
  bool sawEmitCChild = false;
  bool sawVPTOChild = false;
  SmallVector<ModuleOp, 4> missingBackendChildren;
  for (ModuleOp child : module.getOps<ModuleOp>()) {
    std::optional<mlir::pto::PTOASBackend> parsedChildBackend;
    if (failed(parseDriverBackendAttr(child.getOperation(), parsedChildBackend)))
      return failure();
    if (!parsedChildBackend) {
      missingBackendChildren.push_back(child);
      continue;
    }
    sawEmitCChild |= *parsedChildBackend == mlir::pto::PTOASBackend::EmitC;
    sawVPTOChild |= *parsedChildBackend == mlir::pto::PTOASBackend::VPTO;

    if (!childBackend) {
      childBackend = *parsedChildBackend;
      continue;
    }
    if (*childBackend != *parsedChildBackend) {
      if (!missingBackendChildren.empty()) {
        return missingBackendChildren.front().emitError()
               << "mixed-backend container child module is missing pto.backend";
      }
      mixedBackendMode = true;
      sawEmitCChild = true;
      sawVPTOChild = true;
      continue;
    }
  }

  if (sawEmitCChild && sawVPTOChild)
    mixedBackendMode = true;
  if (childBackend)
    effectiveBackend = *childBackend;
  if (sawEmitCChild && sawVPTOChild && !missingBackendChildren.empty()) {
    return missingBackendChildren.front().emitError()
           << "mixed-backend container child module is missing pto.backend";
  }
  return success();
}

static std::optional<mlir::pto::FunctionKernelKind>
getModuleKernelKind(ModuleOp module) {
  auto kernelKind =
      module->getAttrOfType<mlir::pto::FunctionKernelKindAttr>(
          mlir::pto::FunctionKernelKindAttr::name);
  if (!kernelKind)
    return std::nullopt;
  return kernelKind.getKernelKind();
}

static llvm::StringRef
getPublicABISuffix(mlir::pto::FunctionKernelKind kind) {
  switch (kind) {
  case mlir::pto::FunctionKernelKind::Vector:
    return ".vector";
  case mlir::pto::FunctionKernelKind::Cube:
    return ".cube";
  }
  llvm_unreachable("unknown function kernel kind");
}

static OwningOpRef<ModuleOp> cloneBackendChildModule(ModuleOp outer,
                                                     ModuleOp child) {
  Operation *clonedOp = child.getOperation()->clone();
  auto clonedModule = cast<ModuleOp>(clonedOp);

  for (NamedAttribute attr : outer->getAttrs()) {
    StringRef attrName = attr.getName().getValue();
    if (attrName == SymbolTable::getSymbolAttrName() ||
        attrName == "pto.backend")
      continue;
    if (!clonedModule->hasAttr(attr.getName()))
      clonedModule->setAttr(attr.getName(), attr.getValue());
  }

  return OwningOpRef<ModuleOp>(clonedModule);
}

static void collectBackendChildSymbols(BackendChildPlan &plan) {
  for (func::FuncOp func : plan.module.getOps<func::FuncOp>()) {
    llvm::StringRef symName = func.getSymName();
    if (mlir::pto::isPTOKernelFunction(func))
      continue;

    if (func.isDeclaration()) {
      BackendImportPlan import;
      import.sourceName = symName.str();
      import.type = func.getFunctionType();
      plan.imports.push_back(std::move(import));
      continue;
    }

    if (!func.isPublic())
      continue;

    BackendExportPlan exportPlan;
    exportPlan.sourceName = symName.str();
    exportPlan.type = func.getFunctionType();
    if (plan.backend == mlir::pto::PTOASBackend::VPTO && plan.kernelKind)
      exportPlan.abiName =
          (symName + getPublicABISuffix(*plan.kernelKind)).str();
    else
      exportPlan.abiName = symName.str();
    plan.exports.push_back(std::move(exportPlan));
  }
}

static LogicalResult
collectBackendChildren(ModuleOp module,
                       SmallVectorImpl<BackendChildPlan> &children) {
  for (ModuleOp child : module.getOps<ModuleOp>()) {
    std::optional<mlir::pto::PTOASBackend> childBackend;
    if (failed(parseDriverBackendAttr(child.getOperation(), childBackend)))
      return failure();
    if (!childBackend)
      continue;

    BackendChildPlan plan;
    plan.ownedModule = cloneBackendChildModule(module, child);
    plan.module = plan.ownedModule.get();
    plan.backend = *childBackend;
    plan.kernelKind = getModuleKernelKind(plan.module);
    collectBackendChildSymbols(plan);
    children.push_back(std::move(plan));
  }
  return success();
}

static LogicalResult verifyUniqueBackendABIExports(
    SmallVectorImpl<BackendChildPlan> &children) {
  llvm::StringMap<Location> seen;
  for (BackendChildPlan &child : children) {
    for (const BackendExportPlan &exportPlan : child.exports) {
      auto inserted =
          seen.try_emplace(exportPlan.abiName, child.module.getLoc());
      if (!inserted.second) {
        return child.module.emitError()
               << "generated ABI export symbol '" << exportPlan.abiName
               << "' is defined by multiple backend child modules";
      }
    }
  }
  return success();
}

static LogicalResult verifyExternalImportShape(
    SmallVectorImpl<BackendChildPlan> &children) {
  for (BackendChildPlan &child : children) {
    for (func::FuncOp func : child.module.getOps<func::FuncOp>()) {
      if (!func.isDeclaration())
        continue;
      if (!func.isPrivate()) {
        return func.emitError()
               << "external func.func declaration must use private visibility";
      }
      if (mlir::pto::isPTOKernelFunction(func)) {
        return func.emitError()
               << "external func.func declaration must not be marked pto.aicore";
      }
    }
  }
  return success();
}

static LogicalResult resolveExternalImports(
    SmallVectorImpl<BackendChildPlan> &children) {
  for (BackendChildPlan &importingChild : children) {
    for (BackendImportPlan &importPlan : importingChild.imports) {
      const BackendExportPlan *matchedExport = nullptr;
      for (const BackendChildPlan &exportingChild : children) {
        if (&exportingChild == &importingChild)
          continue;
        for (const BackendExportPlan &exportPlan : exportingChild.exports) {
          if (exportPlan.sourceName != importPlan.sourceName)
            continue;
          if (matchedExport) {
            return importingChild.module.emitError()
                   << "cross-backend external import '" << importPlan.sourceName
                   << "' matches multiple exported definitions";
          }
          matchedExport = &exportPlan;
        }
      }

      if (!matchedExport) {
        return importingChild.module.emitError()
               << "cross-backend external import '" << importPlan.sourceName
               << "' has no matching public non-pto.aicore definition in "
                  "another backend child module";
      }

      if (matchedExport->type != importPlan.type) {
        return importingChild.module.emitError()
               << "cross-backend external import '" << importPlan.sourceName
               << "' signature does not match exported definition";
      }

      importPlan.abiName = matchedExport->abiName;
    }
  }
  return success();
}

static LogicalResult applyResolvedExternalImports(BackendChildPlan &child) {
  llvm::StringMap<std::string> importABIMap;
  for (const BackendImportPlan &importPlan : child.imports) {
    if (importPlan.abiName.empty())
      return child.module.emitError()
             << "unresolved cross-backend external import '"
             << importPlan.sourceName << "'";
    importABIMap[importPlan.sourceName] = importPlan.abiName;
  }
  if (importABIMap.empty())
    return success();

  for (func::FuncOp func : child.module.getOps<func::FuncOp>()) {
    auto it = importABIMap.find(func.getSymName());
    if (it == importABIMap.end())
      continue;
    func->setAttr("pto.external_abi",
                  StringAttr::get(func.getContext(), it->second));
  }
  return success();
}

static LogicalResult compileMixedBackendContainer(
    ModuleOp module, llvm::StringRef arch, bool cliBackendSpecified, int argc,
    char **argv, mlir::pto::PTOASCompileResult &result) {
  mlir::pto::PTOASBackend effectiveBackend = mlir::pto::PTOASBackend::EmitC;
  if (failed(mlir::pto::getPTOASCommandLineBackend(effectiveBackend)))
    return failure();

  bool mixedBackendMode = false;
  if (failed(resolveDriverModuleBackend(module, cliBackendSpecified,
                                        effectiveBackend, mixedBackendMode)))
    return failure();
  if (!mixedBackendMode)
    return mlir::pto::compilePTOASModule(module, arch, cliBackendSpecified,
                                         argc, argv, result);
  if (mlir::pto::isPTOASDebugIROutputRequested()) {
    llvm::errs() << "Error: mixed pto.backend fatobj mode does not support "
                    "debug IR output flags.\n";
    return failure();
  }

  SmallVector<BackendChildPlan, 4> children;
  if (failed(collectBackendChildren(module, children)))
    return failure();
  if (failed(verifyUniqueBackendABIExports(children)))
    return failure();
  if (failed(verifyExternalImportShape(children)))
    return failure();
  if (failed(resolveExternalImports(children)))
    return failure();

  result = mlir::pto::PTOASCompileResult{};
  result.kind = mlir::pto::PTOASCompileResultKind::MixedObject;
  SmallVector<ModuleOp, 4> stubModules;

  for (BackendChildPlan &child : children) {
    if (failed(applyResolvedExternalImports(child)))
      return failure();
    child.module->setAttr("pto.backend",
                          StringAttr::get(child.module.getContext(),
                                          child.backend ==
                                                  mlir::pto::PTOASBackend::VPTO
                                              ? "vpto"
                                              : "emitc"));

    mlir::pto::PTOASCompileResult childResult;
    if (failed(mlir::pto::compilePTOASModule(child.module, arch,
                                             /*cliBackendSpecified=*/false,
                                             argc, argv, childResult,
                                             /*emitVPTOHostStub=*/false)))
      return failure();
    if (childResult.kind == mlir::pto::PTOASCompileResultKind::Text) {
      mlir::pto::PTOASBackendObjectInput object;
      object.kind = mlir::pto::PTOASBackendObjectKind::EmitCFatobj;
      object.cppSource = std::move(childResult.textOutput);
      result.backendObjects.push_back(std::move(object));
      continue;
    }

    if (childResult.kind != mlir::pto::PTOASCompileResultKind::VPTOObject) {
      llvm::errs() << "Error: nested mixed backend compilation is not "
                      "supported.\n";
      return failure();
    }
    for (mlir::pto::PTOASBackendObjectInput &object :
         childResult.backendObjects)
      result.backendObjects.push_back(std::move(object));
    if (child.module)
      stubModules.push_back(child.module);
  }

  if (result.backendObjects.empty()) {
    llvm::errs() << "Error: mixed fatobj compilation produced no backend "
                    "objects.\n";
    return failure();
  }

  if (stubModules.empty()) {
    result.stubSource = "#ifndef __global__\n#define __global__\n#endif\n\n"
                        "#ifndef __gm__\n#define __gm__\n#endif\n\n";
    return success();
  }
  if (failed(mlir::pto::emitVPTOHostStubSource(stubModules, result.stubSource,
                                               llvm::errs()))) {
    llvm::errs() << "Error: Failed to emit mixed VPTO host stub source.\n";
    return failure();
  }
  return success();
}

static LogicalResult emitBackendObject(
    mlir::pto::PTOASBackendObjectInput &input,
    const mlir::pto::ObjectEmissionToolchain &toolchain,
    SmallVectorImpl<std::string> &tempPaths, std::string &outputPath) {
  std::string stderrPath;
  if (failed(createTempPath("ptoas-object", ".log", stderrPath)))
    return failure();
  tempPaths.push_back(stderrPath);

  if (input.kind == mlir::pto::PTOASBackendObjectKind::EmitCFatobj) {
    std::string cppPath;
    if (failed(createTempPath("ptoas-emitc", ".cpp", cppPath)) ||
        failed(createTempPath("ptoas-emitc-fatobj", ".o", outputPath)))
      return failure();
    tempPaths.push_back(cppPath);
    tempPaths.push_back(outputPath);
    return mlir::pto::emitCppFatobj(input.cppSource, cppPath, outputPath,
                                    toolchain, stderrPath, llvm::errs());
  }

  std::string llPath;
  if (failed(createTempPath("ptoas-vpto", ".ll", llPath)) ||
      failed(createTempPath("ptoas-vpto-device", ".o", outputPath)))
    return failure();
  tempPaths.push_back(llPath);
  tempPaths.push_back(outputPath);

  if (!input.llvmModule.module) {
    llvm::errs() << "Error: missing LLVM module for VPTO object emission.\n";
    return failure();
  }

  if (input.kind == mlir::pto::PTOASBackendObjectKind::VPTOVectorDeviceObject) {
    return mlir::pto::emitVPTOVectorDeviceObject(
        *input.llvmModule.module, llPath, outputPath, toolchain, stderrPath,
        llvm::errs());
  }
  return mlir::pto::emitVPTOCubeDeviceObject(
      *input.llvmModule.module, llPath, outputPath, toolchain, stderrPath,
      llvm::errs());
}

static LogicalResult writeTextOutput(llvm::StringRef output,
                                     llvm::StringRef outputPath) {
  std::error_code ec;
  llvm::ToolOutputFile outputFile(outputPath, ec, llvm::sys::fs::OF_None);
  if (ec) {
    llvm::errs() << ec.message() << "\n";
    return failure();
  }
  outputFile.os() << output;
  outputFile.os().flush();
  outputFile.keep();
  return success();
}

static LogicalResult emitObjectOutput(mlir::pto::PTOASCompileResult &result,
                                      llvm::StringRef outputPath) {
  if (outputPath.empty() || outputPath == "-") {
    if (result.kind == mlir::pto::PTOASCompileResultKind::MixedObject) {
      llvm::errs() << "Error: mixed pto.backend fatobj mode requires an "
                      "explicit file path passed with -o.\n";
      return failure();
    }
    llvm::errs() << "Error: object output requires an explicit file path "
                    "passed with -o.\n";
    return failure();
  }

  mlir::pto::ObjectEmissionToolchain toolchain;
  if (failed(mlir::pto::discoverObjectEmissionToolchain(toolchain,
                                                        llvm::errs())))
    return failure();

  SmallVector<std::string, 16> tempPaths;
  auto cleanup = llvm::make_scope_exit([&]() { removeTempPaths(tempPaths); });

  std::string stubPath;
  std::string stderrPath;
  if (failed(createTempPath("ptoas-stub", ".cpp", stubPath)) ||
      failed(createTempPath("ptoas-fatobj", ".log", stderrPath)))
    return failure();
  tempPaths.push_back(stubPath);
  tempPaths.push_back(stderrPath);
  if (failed(mlir::pto::writeHostStubSource(result.stubSource, stubPath,
                                            llvm::errs())))
    return failure();

  SmallVector<std::string, 4> fatobjPaths;
  const std::string moduleId = sanitizeModuleId(outputPath);
  if (result.kind == mlir::pto::PTOASCompileResultKind::VPTOObject) {
    SmallVector<std::string, 2> deviceObjectPaths;
    for (size_t i = 0, e = result.backendObjects.size(); i < e; ++i) {
      std::string objectPath;
      if (failed(emitBackendObject(result.backendObjects[i], toolchain,
                                   tempPaths, objectPath)))
        return failure();
      deviceObjectPaths.push_back(objectPath);
    }

    std::string mergedObjPath;
    std::string fatobjPath;
    if (failed(createTempPath("ptoas-device-merged", ".o", mergedObjPath)) ||
        failed(createTempPath("ptoas-device-fatobj", ".o", fatobjPath)))
      return failure();
    tempPaths.push_back(mergedObjPath);
    tempPaths.push_back(fatobjPath);

    if (failed(mlir::pto::mergeDeviceObjects(deviceObjectPaths, mergedObjPath,
                                             toolchain, stderrPath,
                                             llvm::errs())))
      return failure();
    if (failed(mlir::pto::compileStubToFatobj(
            stubPath, mergedObjPath, fatobjPath, moduleId, toolchain,
            stderrPath, llvm::errs())))
      return failure();

    if (std::error_code ec = llvm::sys::fs::copy_file(fatobjPath, outputPath)) {
      llvm::errs() << "Error: failed to copy fatobj to " << outputPath << ": "
                   << ec.message() << "\n";
      return failure();
    }
    return success();
  }

  for (size_t i = 0, e = result.backendObjects.size(); i < e; ++i) {
    std::string objectPath;
    if (failed(emitBackendObject(result.backendObjects[i], toolchain, tempPaths,
                                 objectPath)))
      return failure();

    if (result.backendObjects[i].kind ==
        mlir::pto::PTOASBackendObjectKind::EmitCFatobj) {
      fatobjPaths.push_back(objectPath);
      continue;
    }

    std::string mergedObjPath;
    std::string fatobjPath;
    if (failed(createTempPath("ptoas-device-merged", ".o", mergedObjPath)) ||
        failed(createTempPath("ptoas-device-fatobj", ".o", fatobjPath)))
      return failure();
    tempPaths.push_back(mergedObjPath);
    tempPaths.push_back(fatobjPath);

    if (failed(mlir::pto::mergeDeviceObjects(ArrayRef<std::string>(objectPath),
                                             mergedObjPath, toolchain,
                                             stderrPath, llvm::errs())))
      return failure();

    if (failed(mlir::pto::compileStubToFatobj(
            stubPath, mergedObjPath, fatobjPath,
            moduleId + "_backend_" + std::to_string(i), toolchain, stderrPath,
            llvm::errs())))
      return failure();
    fatobjPaths.push_back(fatobjPath);
  }

  if (fatobjPaths.empty()) {
    llvm::errs() << "Error: object emission produced no fatobjs.\n";
    return failure();
  }

  if (fatobjPaths.size() == 1) {
    if (std::error_code ec = llvm::sys::fs::copy_file(fatobjPaths.front(),
                                                      outputPath)) {
      llvm::errs() << "Error: failed to copy fatobj to " << outputPath << ": "
                   << ec.message() << "\n";
      return failure();
    }
    return success();
  }

  if (failed(mlir::pto::linkFatobjs(fatobjPaths, outputPath, toolchain,
                                    stderrPath, llvm::errs())))
    return failure();
  return success();
}

int main(int argc, char **argv) {
  DialectRegistry registry;
  registerPTOASDriverDialects(registry);
  registerPTOASDriverPassesAndCLOptions();
  llvm::cl::SetVersionPrinter(printPTOASVersion);

  const bool cliArchSpecified = hasCLIOption(argc, argv, "--pto-arch");
  const bool cliBackendSpecified = hasCLIOption(argc, argv, "--pto-backend");

  llvm::cl::ParseCommandLineOptions(argc, argv, "PTO Assembler (ptoas)\n");

  auto fileOrErr = llvm::MemoryBuffer::getFileOrSTDIN(inputFilename);
  if (!fileOrErr) {
    llvm::errs() << "Error: Could not open input file: "
                 << fileOrErr.getError().message() << "\n";
    return 1;
  }

  MLIRContext context(registry);
  // Be tolerant: ptobc decode may materialize ops from dialects that aren't
  // explicitly registered/loaded in this tool yet.
  context.allowUnregisteredDialects(true);
  loadPTOASDialects(context);

  OwningOpRef<ModuleOp> module;
  llvm::StringRef buf = (*fileOrErr)->getBuffer();
  const bool isPTOBC =
      (buf.size() >= 6 && std::memcmp(buf.data(), "PTOBC\0", 6) == 0);

  std::string arch =
      mlir::pto::normalizePTOASArch(mlir::pto::getPTOASTargetArchOption());
  if (cliArchSpecified) {
    if (!mlir::pto::isSupportedPTOASArch(arch)) {
      llvm::errs() << "Error: invalid --pto-arch='"
                   << mlir::pto::getPTOASTargetArchOption()
                   << "'. Expected 'a3' or 'a5'.\n";
      return 1;
    }
  } else if (!isPTOBC) {
    if (auto detectedArch = mlir::pto::detectPTOASTextualModuleArch(buf))
      arch = *detectedArch;
  }
  if (!mlir::pto::isSupportedPTOASArch(arch))
    arch = "a3";

  if (isPTOBC) {
    llvm::ArrayRef<uint8_t> bytes(
        reinterpret_cast<const uint8_t *>(buf.data()), buf.size());
#if defined(__cpp_exceptions) || defined(__EXCEPTIONS)
    try {
      module = ptobc::decodePTOBCToModule(bytes, context);
    } catch (...) {
      llvm::errs() << "Error: Failed to decode PTOBC.\n";
      return 1;
    }
#else
    module = ptobc::decodePTOBCToModule(bytes, context);
#endif
    if (!module) {
      llvm::errs() << "Error: Failed to decode PTOBC.\n";
      return 1;
    }
  } else {
    llvm::SourceMgr sourceMgr;
    sourceMgr.AddNewSourceBuffer(std::move(*fileOrErr), llvm::SMLoc());
    mlir::pto::ScopedPTOParserTargetArch scopedParserArch(
        &context, arch == "a5" ? mlir::pto::PTOParserTargetArch::A5
                                : mlir::pto::PTOParserTargetArch::A3);
    module = parseSourceFile<ModuleOp>(sourceMgr, &context);
    if (!module) {
      llvm::errs() << "Error: Failed to parse MLIR.\n";
      return 1;
    }
  }

  if (cliArchSpecified || !module->getOperation()->hasAttr("pto.target_arch")) {
    module->getOperation()->setAttr("pto.target_arch",
                                    mlir::StringAttr::get(&context, arch));
  }

  mlir::pto::PTOASBackend effectiveBackend = mlir::pto::PTOASBackend::EmitC;
  bool mixedBackendMode = false;
  if (failed(mlir::pto::getPTOASCommandLineBackend(effectiveBackend)) ||
      failed(resolveDriverModuleBackend(module.get(), cliBackendSpecified,
                                        effectiveBackend, mixedBackendMode)))
    return 1;
  if (mixedBackendMode && (outputFilename.empty() || outputFilename == "-")) {
    llvm::errs() << "Error: mixed pto.backend fatobj mode requires an "
                    "explicit file path passed with -o.\n";
    return 1;
  }

  mlir::pto::PTOASCompileResult result;
  if (failed(compileMixedBackendContainer(module.get(), arch,
                                          cliBackendSpecified, argc, argv,
                                          result)))
    return 1;

  if (result.kind == mlir::pto::PTOASCompileResultKind::Text)
    return failed(writeTextOutput(result.textOutput, outputFilename));

  return failed(emitObjectOutput(result, outputFilename));
}
