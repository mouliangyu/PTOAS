// Copyright (c) 2026 Huawei Technologies Co., Ltd.
// This program is free software, you can redistribute it and/or modify it under the terms and conditions of
// CANN Open Software License Agreement Version 2.0 (the "License").
// Please refer to the License for details. You may not use this file except in compliance with the License.
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
// See LICENSE in the root of the software repository for the full text of the License.

#include "PTOASDriver.h"

#include "PTO/IR/PTO.h"
#include "PTO/Transforms/VPTOLLVMEmitter.h"
#include "PTO/Transforms/Passes.h"
#include "PTO/Transforms/BufferizableOpInterfaceImpl.h"
#include "ObjectEmission.h"
#include "VPTOHostStubEmission.h"
#include "TilelangDaemon.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Diagnostics.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/SymbolTable.h"
#include "mlir/InitAllDialects.h"
#include "mlir/InitAllPasses.h"
#include "mlir/Parser/Parser.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/Math/IR/Math.h"
#include <cctype>
#include <cstring>
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/Target/Cpp/CppEmitter.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/FileSystem.h" // [Fix] Required for OF_None
#include "llvm/Support/Path.h"
#include "ptobc/ptobc_decode.h"
#include "mlir/Dialect/Bufferization/Transforms/OneShotAnalysis.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlowOps.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/EmitC/IR/EmitC.h"
#include "mlir/Dialect/EmitC/Transforms/Passes.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Regex.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/ScopeExit.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/Program.h"
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

extern "C" {
extern char **environ;
}

using namespace mlir;
using namespace pto;

#ifndef PTOAS_RELEASE_VERSION
#define PTOAS_RELEASE_VERSION "unknown"
#endif

extern "C" int main(int argc, char **argv);

static void printPTOASVersion(llvm::raw_ostream &os) {
  os << "ptoas " << PTOAS_RELEASE_VERSION << "\n";
}

static std::string getParentDir(llvm::StringRef path) {
  llvm::SmallString<256> parent(path);
  llvm::sys::path::remove_filename(parent);
  llvm::sys::path::remove_dots(parent, true);
  return std::string(parent);
}

static bool pathExists(llvm::StringRef path) {
  return !path.empty() && llvm::sys::fs::exists(path);
}

static std::string joinPath(llvm::StringRef lhs, llvm::StringRef rhs) {
  llvm::SmallString<256> joined(lhs);
  llvm::sys::path::append(joined, rhs);
  llvm::sys::path::remove_dots(joined, true);
  return std::string(joined);
}

static std::string detectInstalledTilelangPath(const char *argv0) {
  std::string exePath = llvm::sys::fs::getMainExecutable(argv0, (void *)&main);
  if (exePath.empty())
    return {};

  const std::string exeDir = getParentDir(exePath);
  const std::string prefixDir = getParentDir(exeDir);
  const std::string installedTileOps = joinPath(prefixDir, "share/ptoas/TileOps");
  if (pathExists(installedTileOps))
    return installedTileOps;
  return {};
}

static std::string detectInstalledTilelangPkgPath(const char *argv0) {
  std::string exePath = llvm::sys::fs::getMainExecutable(argv0, (void *)&main);
  if (exePath.empty())
    return {};

  const std::string exeDir = getParentDir(exePath);
  const std::string prefixDir = getParentDir(exeDir);
  const std::string installedPkgRoot = prefixDir;
  const std::string installedPkg = joinPath(installedPkgRoot, "tilelang_dsl");
  if (pathExists(installedPkg))
    return installedPkgRoot;
  return {};
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

static LogicalResult applyConfiguredPassManagerCLOptions(
    PassManager &pm, llvm::StringRef pipelineName,
    llvm::raw_ostream &diagOS = llvm::errs()) {
  if (succeeded(mlir::applyPassManagerCLOptions(pm)))
    return success();
  diagOS << "Error: failed to apply MLIR pass manager command-line options for "
         << pipelineName << ".\n";
  return failure();
}

static LogicalResult reorderEmitCFunctions(ModuleOp module) {
  SmallVector<emitc::FuncOp> declarations;
  SmallVector<emitc::FuncOp> definitions;
  llvm::DenseMap<StringAttr, emitc::FuncOp> definitionsByName;

  for (auto func : module.getOps<emitc::FuncOp>()) {
    if (func.isDeclaration()) {
      declarations.push_back(func);
      continue;
    }
    definitions.push_back(func);
    definitionsByName[func.getSymNameAttr()] = func;
  }

  llvm::DenseMap<Operation *, unsigned> indegree;
  llvm::DenseMap<Operation *, SmallVector<Operation *>> outgoing;
  for (auto func : definitions)
    indegree[func.getOperation()] = 0;

  for (auto caller : definitions) {
    Operation *callerOp = caller.getOperation();
    llvm::SmallPtrSet<Operation *, 8> seenCallees;
    bool hasCycle = false;
    caller.walk([&](emitc::CallOp call) {
      auto calleeAttr = call.getCalleeAttr();
      if (!calleeAttr)
        return;
      auto it = definitionsByName.find(calleeAttr.getLeafReference());
      if (it == definitionsByName.end())
        return;
      Operation *calleeOp = it->second.getOperation();
      if (calleeOp == callerOp) {
        hasCycle = true;
        return;
      }
      if (!seenCallees.insert(calleeOp).second)
        return;
      outgoing[calleeOp].push_back(callerOp);
      ++indegree[callerOp];
    });
    if (hasCycle) {
      return caller.emitOpError()
             << "recursive function calls are not supported for EmitC C++ "
                "emission";
    }
  }

  SmallVector<Operation *> ready;
  for (auto func : definitions) {
    if (indegree[func.getOperation()] == 0)
      ready.push_back(func.getOperation());
  }

  SmallVector<emitc::FuncOp> sortedDefinitions;
  while (!ready.empty()) {
    Operation *next = ready.front();
    ready.erase(ready.begin());
    auto nextFunc = cast<emitc::FuncOp>(next);
    sortedDefinitions.push_back(nextFunc);

    for (Operation *user : outgoing[next]) {
      unsigned &userIndegree = indegree[user];
      if (--userIndegree == 0)
        ready.push_back(user);
    }
  }

  if (sortedDefinitions.size() != definitions.size()) {
    return module.emitError()
           << "cyclic function call graph is not supported for EmitC C++ emission";
  }

  if (declarations.empty() && definitions.size() <= 1)
    return success();

  SmallVector<emitc::FuncOp> desiredOrder;
  desiredOrder.append(declarations.begin(), declarations.end());
  desiredOrder.append(sortedDefinitions.begin(), sortedDefinitions.end());

  Block &body = module.getBodyRegion().front();
  Operation *anchor = nullptr;
  for (Operation &op : body.getOperations()) {
    if (isa<emitc::FuncOp>(op)) {
      anchor = &op;
      break;
    }
  }
  if (!anchor)
    return success();

  auto advanceAnchor = [&]() {
    while (anchor) {
      anchor = anchor->getNextNode();
      if (!anchor || isa<emitc::FuncOp>(anchor))
        return;
    }
  };

  for (auto func : desiredOrder) {
    if (func.getOperation() == anchor) {
      advanceAnchor();
      continue;
    }
    if (anchor)
      func->moveBefore(anchor);
    else
      func->moveBefore(&body, body.end());
  }

  return success();
}

// --------------------------------------------------------------------------
// Command Line Options
// --------------------------------------------------------------------------
static llvm::cl::opt<std::string> inputFilename(llvm::cl::Positional,
                                                llvm::cl::desc("<input file>"),
                                                llvm::cl::init("-"));

static llvm::cl::opt<std::string> outputFilename("o",
                                                 llvm::cl::desc("Output filename"),
                                                 llvm::cl::value_desc("filename"),
                                                 llvm::cl::init("-"));

static llvm::cl::opt<bool> enableInsertSync("enable-insert-sync",
                                            llvm::cl::desc("Enable automatic synchronization insertion pass"),
                                            llvm::cl::init(false));

static llvm::cl::opt<bool> enableInjectBarrierAllSync(
    "enable-inject-barrier-all-sync",
    llvm::cl::desc("Enable conservative synchronization by inserting "
                   "pto.barrier PIPE_ALL before memory-effecting PTO pipe ops"),
    llvm::cl::init(false));

static llvm::cl::opt<bool> enableGraphSyncSolver(
    "enable-graph-sync-solver",
    llvm::cl::desc("Enable the graph-based intra-core sync solver "
                   "(experimental). Mutually exclusive with "
                   "--enable-insert-sync and "
                   "--enable-inject-barrier-all-sync."),
    llvm::cl::init(false));

static llvm::cl::opt<int> graphSyncSolverEventIdMax(
    "graph-sync-solver-event-id-max",
    llvm::cl::desc(
        "Maximum EVENT_ID slots for the graph sync solver (default 8). "
        "Lower values exercise the PIPE_ALL coloring fallback sooner."),
    llvm::cl::init(8));

static llvm::cl::opt<bool> enableTileOpExpand(
    "enable-tile-op-expand",
    llvm::cl::desc(
        "Deprecated compatibility flag. TileOp expansion is controlled by "
        "--pto-backend=vpto."),
    llvm::cl::init(false));

#ifndef PTOAS_DEFAULT_TILELANG_PATH
#define PTOAS_DEFAULT_TILELANG_PATH ""
#endif
#ifndef PTOAS_DEFAULT_TILELANG_PKG_PATH
#define PTOAS_DEFAULT_TILELANG_PKG_PATH ""
#endif

static llvm::cl::opt<std::string> tilelangPath(
    "tilelang-path",
    llvm::cl::desc("Path to directory of .py tilelang DSL template files "
                   "(default: <source>/lib/TileOps, baked in at build time)"),
    llvm::cl::init(PTOAS_DEFAULT_TILELANG_PATH));

static llvm::cl::opt<std::string> tilelangPkgPath(
    "tilelang-pkg-path",
    llvm::cl::desc("PYTHONPATH for tilelang_dsl package "
                   "(default: <source>/tilelang-dsl/python, baked in at build time)"),
    llvm::cl::init(PTOAS_DEFAULT_TILELANG_PKG_PATH));

static llvm::cl::opt<std::string> daemonSocketPath(
    "daemon-socket-path",
    llvm::cl::desc("Path to Unix domain socket for daemon RPC "
                   "(default: /tmp/tilelang_daemon_{pid}.sock)"),
    llvm::cl::init(""));

static pto::ExpandTileOpOptions resolveExpandTileOpOptions(int argc,
                                                           char **argv) {
  pto::ExpandTileOpOptions expandOpts;
  expandOpts.tilelangPath = tilelangPath;
  expandOpts.tilelangPkgPath = tilelangPkgPath;

  if (!hasCLIOption(argc, argv, "--tilelang-path")) {
    std::string detectedTilelangPath = detectInstalledTilelangPath(argv[0]);
    if (!detectedTilelangPath.empty())
      expandOpts.tilelangPath = detectedTilelangPath;
  }

  if (!hasCLIOption(argc, argv, "--tilelang-pkg-path")) {
    std::string detectedTilelangPkgPath = detectInstalledTilelangPkgPath(argv[0]);
    if (!detectedTilelangPkgPath.empty())
      expandOpts.tilelangPkgPath = detectedTilelangPkgPath;
  }

  // Daemon mode is default (no CLI option needed)
  // Automatically start daemon for instance caching
  if (!expandOpts.tilelangPath.empty()) {
    std::string socket = daemonSocketPath;
    if (socket.empty())
      socket = ptoas::DaemonManager::generateSocketPath();

    // Register cleanup handler (daemon will be stopped on PTOAS exit)
    ptoas::registerDaemonCleanup();

    // Try to start daemon automatically
    if (ptoas::DaemonManager::start(socket, expandOpts.tilelangPath, expandOpts.tilelangPkgPath)) {
      expandOpts.daemonSocketPath = socket;
      llvm::errs() << "Info: TileLang daemon started successfully\n";
    } else {
      // Fallback: daemon failed, use subprocess mode (current approach)
      expandOpts.daemonSocketPath = "";
      llvm::errs() << "Warning: Failed to start daemon, using subprocess mode (fallback)\n";
    }
  }

  return expandOpts;
}

static llvm::cl::opt<bool> disableInferLayout(
    "disable-infer-layout",
    llvm::cl::desc("Disable PTO layout inference pass (static-only)"),
    llvm::cl::init(false));

static llvm::cl::opt<bool> emitAddPtrTrace(
    "emit-addptr-trace",
    llvm::cl::desc("Emit addptr trace comments in generated C++ output"),
    llvm::cl::init(false));

static llvm::cl::opt<bool> emitMlirIR(
    "emit-pto-ir",
    llvm::cl::desc("Emit PTO IR after lowering instead of C++"),
    llvm::cl::init(false));

static llvm::cl::opt<std::string> ptoTargetArch(
    "pto-arch",
    llvm::cl::desc("Target Ascend architecture for codegen: a3 or a5 (default: a3)"),
    llvm::cl::value_desc("a3|a5"),
    llvm::cl::init("a3"));

static llvm::cl::opt<std::string> ptoBuildLevel(
    "pto-level",
    llvm::cl::desc("Build level for pass pipeline: level1, level2, or level3 (default: level2)"),
    llvm::cl::value_desc("level1|level2|level3"),
    llvm::cl::init("level2"));

static llvm::cl::opt<std::string> ptoBackend(
    "pto-backend",
    llvm::cl::desc("Final PTOAS backend: emitc or vpto (default: emitc)"),
    llvm::cl::value_desc("emitc|vpto"), llvm::cl::init("emitc"));

static llvm::cl::opt<bool> emitVPTO(
    "emit-vpto",
    llvm::cl::desc("Write final post-pass VPTO IR to -o"),
    llvm::cl::init(false));

static llvm::cl::opt<bool> vptoPrintIR(
    "vpto-print-ir",
    llvm::cl::desc("Print post-pass VPTO backend IR to stderr"),
    llvm::cl::init(false));

static llvm::cl::opt<std::string> vptoLoweringStrategy(
    "vpto-lowering-strategy",
    llvm::cl::desc("VPTO vector lowering strategy: post-update or no-post-update"),
    llvm::cl::value_desc("post-update|no-post-update"),
    llvm::cl::init("post-update"));

static llvm::cl::opt<bool> dumpVPTOIR(
    "dump-vpto-ir",
    llvm::cl::desc("Print post-pass VPTO backend IR to stderr"),
    llvm::cl::init(false));

static llvm::cl::opt<bool> ptoPrintSeamIR(
    "pto-print-seam-ir",
    llvm::cl::desc("Print shared pre-backend seam IR to stderr"),
    llvm::cl::init(false));

static llvm::cl::opt<std::string> ptoSeamIRFile(
    "pto-seam-ir-file",
    llvm::cl::desc("Write shared pre-backend seam IR to a file"),
    llvm::cl::value_desc("path"),
    llvm::cl::init(""));

enum class PTOBuildLevel {
  Level1,
  Level2,
  Level3,
};

enum class PTOBackend {
  EmitC,
  VPTO,
};

static PTOBuildLevel defaultBuildLevel() {
  return PTOBuildLevel::Level2;
}

static bool parseBuildLevel(llvm::StringRef levelStr, PTOBuildLevel &out) {
  std::string s = levelStr.str();
  for (char &c : s)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  if (s == "level1") {
    out = PTOBuildLevel::Level1;
    return true;
  }
  if (s == "level2") {
    out = PTOBuildLevel::Level2;
    return true;
  }
  if (s == "level3") {
    out = PTOBuildLevel::Level3;
    return true;
  }
  return false;
}

static constexpr llvm::StringLiteral kAutoSyncTailPolicyBarrierAll =
    "barrier_all";
static constexpr llvm::StringLiteral kAutoSyncTailPolicyMte3ToSEvent0 =
    "setwait_mte3_to_s_event0";

static bool parseAutoSyncTailHint(llvm::StringRef hintStr, std::string &normalized) {
  std::string s = hintStr.str();
  for (char &c : s)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  if (s == "barrier-all" || s == "barrier_all" || s == "default") {
    normalized = kAutoSyncTailPolicyBarrierAll.str();
    return true;
  }
  if (s == "mte3-to-s-event0" || s == "mte3_to_s_event0" ||
      s == "setwait-mte3-to-s-event0" ||
      s == "setwait_mte3_to_s_event0") {
    normalized = kAutoSyncTailPolicyMte3ToSEvent0.str();
    return true;
  }
  return false;
}

static bool parseBackend(llvm::StringRef backendStr, PTOBackend &out) {
  std::string s = backendStr.str();
  for (char &c : s)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  if (s == "emitc") {
    out = PTOBackend::EmitC;
    return true;
  }
  if (s == "vpto") {
    out = PTOBackend::VPTO;
    return true;
  }
  return false;
}

static LogicalResult parsePTOBackendAttr(Operation *op,
                                         std::optional<PTOBackend> &backend) {
  backend = std::nullopt;
  Attribute rawBackendAttr = op->getAttr("pto.backend");
  if (!rawBackendAttr)
    return success();

  auto backendAttr = dyn_cast<mlir::StringAttr>(rawBackendAttr);
  if (!backendAttr) {
    return op->emitError("invalid pto.backend attribute. Expected string "
                         "value 'emitc' or 'vpto'.");
  }

  PTOBackend attrBackend = PTOBackend::EmitC;
  if (!parseBackend(backendAttr.getValue(), attrBackend)) {
    return op->emitError("invalid pto.backend '")
           << backendAttr.getValue() << "'. Expected 'emitc' or 'vpto'.";
  }

  backend = attrBackend;
  return success();
}

static LogicalResult validatePTOBackendAttrs(ModuleOp module) {
  LogicalResult status = success();
  module.walk([&](ModuleOp nested) {
    if (failed(status))
      return WalkResult::interrupt();
    std::optional<PTOBackend> backend;
    if (failed(parsePTOBackendAttr(nested.getOperation(), backend))) {
      status = failure();
      return WalkResult::interrupt();
    }
    return WalkResult::advance();
  });
  return status;
}

static LogicalResult verifyBackendChildShape(ModuleOp module,
                                             bool cliBackendSpecified) {
  if (cliBackendSpecified || module->hasAttr("pto.backend"))
    return success();

  bool sawEmitC = false;
  bool sawVPTO = false;
  SmallVector<ModuleOp, 4> missingBackendChildren;
  for (ModuleOp child : module.getOps<ModuleOp>()) {
    std::optional<PTOBackend> childBackend;
    if (failed(parsePTOBackendAttr(child.getOperation(), childBackend)))
      return failure();
    if (!childBackend) {
      missingBackendChildren.push_back(child);
      continue;
    }
    sawEmitC |= *childBackend == PTOBackend::EmitC;
    sawVPTO |= *childBackend == PTOBackend::VPTO;
  }

  if (!sawEmitC || !sawVPTO)
    return success();

  if (!missingBackendChildren.empty()) {
    return missingBackendChildren.front().emitError()
           << "mixed-backend container child module is missing pto.backend";
  }

  return success();
}

static LogicalResult resolveModuleBackend(ModuleOp module,
                                          bool cliBackendSpecified,
                                          bool &mixedBackendMode,
                                          PTOBackend &effectiveBackend) {
  mixedBackendMode = false;
  if (cliBackendSpecified)
    return success();

  std::optional<PTOBackend> topBackend;
  if (failed(parsePTOBackendAttr(module.getOperation(), topBackend)))
    return failure();
  if (topBackend) {
    effectiveBackend = *topBackend;
    return success();
  }

  std::optional<PTOBackend> childBackend;
  bool sawEmitCChild = false;
  bool sawVPTOChild = false;
  SmallVector<ModuleOp, 4> missingBackendChildren;
  for (ModuleOp child : module.getOps<ModuleOp>()) {
    std::optional<PTOBackend> parsedChildBackend;
    if (failed(parsePTOBackendAttr(child.getOperation(), parsedChildBackend)))
      return failure();
    if (!parsedChildBackend) {
      missingBackendChildren.push_back(child);
      continue;
    }
    sawEmitCChild |= *parsedChildBackend == PTOBackend::EmitC;
    sawVPTOChild |= *parsedChildBackend == PTOBackend::VPTO;

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
      continue;
    }
  }

  if (childBackend)
    effectiveBackend = *childBackend;
  if (sawEmitCChild && sawVPTOChild && !missingBackendChildren.empty()) {
    return missingBackendChildren.front().emitError()
           << "mixed-backend container child module is missing pto.backend";
  }
  return success();
}

static bool hasFunctionKernelKind(ModuleOp module) {
  return static_cast<bool>(module->getAttrOfType<FunctionKernelKindAttr>(
      FunctionKernelKindAttr::name));
}

static llvm::StringRef getPublicABISuffix(llvm::StringRef symName) {
  if (symName.ends_with(".vector"))
    return ".vector";
  if (symName.ends_with(".cube"))
    return ".cube";
  return {};
}

static llvm::StringRef getDeviceABISuffix(llvm::StringRef symName) {
  if (symName.ends_with("_mix_aiv"))
    return "_mix_aiv";
  if (symName.ends_with("_mix_aic"))
    return "_mix_aic";
  return {};
}

static LogicalResult verifyVPTOSourceSymbolNames(ModuleOp module) {
  LogicalResult status = success();
  module.walk([&](func::FuncOp func) {
    if (failed(status))
      return WalkResult::interrupt();

    llvm::StringRef symName = func.getSymName();
    if (func->hasAttr("pto.aicore")) {
      llvm::StringRef suffix = getDeviceABISuffix(symName);
      if (!suffix.empty()) {
        status = func.emitError()
                 << "source-level pto.aicore function '" << symName
                 << "' must not use reserved device ABI suffix '" << suffix
                 << "'; PTOAS adds it from pto.kernel_kind";
        return WalkResult::interrupt();
      }
      return WalkResult::advance();
    }

    if (!func.isPublic())
      return WalkResult::advance();

    llvm::StringRef suffix = getPublicABISuffix(symName);
    if (!suffix.empty()) {
      status = func.emitError()
               << "source-level public non-pto.aicore function '" << symName
               << "' must not use reserved VPTO ABI suffix '" << suffix
               << "'; PTOAS adds it from pto.kernel_kind";
      return WalkResult::interrupt();
    }
    return WalkResult::advance();
  });
  return status;
}

static LogicalResult verifyVPTOBackendChild(ModuleOp child) {
  if (!hasFunctionKernelKind(child)) {
    return child.emitError()
           << "vpto backend child module must carry 'pto.kernel_kind'";
  }
  return verifyVPTOSourceSymbolNames(child);
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
  ModuleOp sourceModule;
  ModuleOp module;
  OwningOpRef<ModuleOp> ownedModule;
  PTOBackend backend = PTOBackend::EmitC;
  std::optional<FunctionKernelKind> kernelKind;
  SmallVector<BackendExportPlan, 4> exports;
  SmallVector<BackendImportPlan, 4> imports;
  SmallVector<std::string, 4> sourceDeviceSymbols;
  SmallVector<std::string, 4> deviceABISymbols;
};

enum class CompilationMode {
  EmitC,
  VPTO,
  MixedFatobj,
};

enum class BackendArtifactKind {
  VectorObject,
  CubeObject,
  GenericObject,
  Fatobj,
};

struct BackendArtifact {
  PTOBackend backend = PTOBackend::EmitC;
  BackendArtifactKind kind = BackendArtifactKind::GenericObject;
  std::string path;
  SmallVector<std::string, 4> exportedABISymbols;
};

struct BackendFatobjGroup {
  PTOBackend backend = PTOBackend::EmitC;
  BackendArtifactKind kind = BackendArtifactKind::GenericObject;
  SmallVector<std::string, 2> objectPaths;
};

struct CompilationPlan {
  CompilationMode mode = CompilationMode::EmitC;
  PTOBackend effectiveBackend = PTOBackend::EmitC;
  SmallVector<BackendChildPlan, 4> children;
};

static std::optional<FunctionKernelKind> getModuleKernelKind(ModuleOp module) {
  auto kernelKind = module->getAttrOfType<FunctionKernelKindAttr>(
      FunctionKernelKindAttr::name);
  if (!kernelKind)
    return std::nullopt;
  return kernelKind.getKernelKind();
}

static llvm::StringRef getPublicABISuffix(FunctionKernelKind kind) {
  switch (kind) {
  case FunctionKernelKind::Vector:
    return ".vector";
  case FunctionKernelKind::Cube:
    return ".cube";
  }
  llvm_unreachable("unknown function kernel kind");
}

static llvm::StringRef getDeviceABISuffix(FunctionKernelKind kind) {
  switch (kind) {
  case FunctionKernelKind::Vector:
    return "_mix_aiv";
  case FunctionKernelKind::Cube:
    return "_mix_aic";
  }
  llvm_unreachable("unknown function kernel kind");
}

static void collectBackendChildSymbols(BackendChildPlan &plan) {
  for (func::FuncOp func : plan.module.getOps<func::FuncOp>()) {
    llvm::StringRef symName = func.getSymName();
    if (func->hasAttr("pto.aicore")) {
      plan.sourceDeviceSymbols.push_back(symName.str());
      if (plan.backend == PTOBackend::VPTO && plan.kernelKind) {
        plan.deviceABISymbols.push_back(
            (symName + getDeviceABISuffix(*plan.kernelKind)).str());
      }
      continue;
    }

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
    if (plan.backend == PTOBackend::VPTO && plan.kernelKind) {
      exportPlan.abiName =
          (symName + getPublicABISuffix(*plan.kernelKind)).str();
    } else {
      exportPlan.abiName = symName.str();
    }
    plan.exports.push_back(std::move(exportPlan));
  }
}

static OwningOpRef<ModuleOp> cloneBackendChildModule(ModuleOp outer,
                                                     ModuleOp child) {
  Operation *clonedOp = child.getOperation()->clone();
  auto clonedModule = cast<ModuleOp>(clonedOp);

  for (NamedAttribute attr : outer->getAttrs()) {
    StringRef attrName = attr.getName().getValue();
    if (attrName == SymbolTable::getSymbolAttrName() || attrName == "pto.backend")
      continue;
    if (!clonedModule->hasAttr(attr.getName()))
      clonedModule->setAttr(attr.getName(), attr.getValue());
  }

  return OwningOpRef<ModuleOp>(clonedModule);
}

static LogicalResult
collectBackendChildren(ModuleOp module,
                       SmallVectorImpl<BackendChildPlan> &children) {
  for (ModuleOp child : module.getOps<ModuleOp>()) {
    std::optional<PTOBackend> childBackend;
    if (failed(parsePTOBackendAttr(child.getOperation(), childBackend)))
      return failure();
    if (!childBackend)
      continue;
    BackendChildPlan plan;
    plan.sourceModule = child;
    plan.ownedModule = cloneBackendChildModule(module, child);
    plan.module = plan.ownedModule.get();
    plan.backend = *childBackend;
    plan.kernelKind = getModuleKernelKind(plan.module);
    collectBackendChildSymbols(plan);
    children.push_back(std::move(plan));
  }
  return success();
}

static CompilationMode getCompilationMode(PTOBackend effectiveBackend,
                                          ArrayRef<BackendChildPlan> children) {
  bool sawEmitC = false;
  bool sawVPTO = false;
  for (const BackendChildPlan &child : children) {
    sawEmitC |= child.backend == PTOBackend::EmitC;
    sawVPTO |= child.backend == PTOBackend::VPTO;
  }
  if (sawEmitC && sawVPTO)
    return CompilationMode::MixedFatobj;
  return effectiveBackend == PTOBackend::VPTO ? CompilationMode::VPTO
                                              : CompilationMode::EmitC;
}

static LogicalResult verifyUniqueBackendABIExports(
    SmallVectorImpl<BackendChildPlan> &children) {
  llvm::StringMap<Location> seen;
  for (BackendChildPlan &child : children) {
    for (const BackendExportPlan &exportPlan : child.exports) {
      auto inserted = seen.try_emplace(exportPlan.abiName, child.module.getLoc());
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
      if (func->hasAttr("pto.aicore")) {
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
      const BackendChildPlan *matchedChild = nullptr;
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
          matchedChild = &exportingChild;
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
      (void)matchedChild;
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

static LogicalResult applyVPTOPublicABIExportNames(ModuleOp module) {
  LogicalResult status = success();
  module.walk([&](ModuleOp child) {
    if (failed(status))
      return WalkResult::interrupt();
    std::optional<FunctionKernelKind> kernelKind = getModuleKernelKind(child);
    if (!kernelKind)
      return WalkResult::advance();

    llvm::StringMap<std::string> renameMap;
    for (func::FuncOp func : child.getOps<func::FuncOp>()) {
      if (!func.isPublic() || func.isDeclaration() ||
          func->hasAttr("pto.aicore"))
        continue;
      llvm::StringRef oldName = func.getSymName();
      std::string newName = (oldName + getPublicABISuffix(*kernelKind)).str();
      renameMap[oldName] = newName;
    }

    for (func::FuncOp func : child.getOps<func::FuncOp>()) {
      func.walk([&](func::CallOp call) {
        auto it = renameMap.find(call.getCallee());
        if (it != renameMap.end())
          call.setCallee(it->second);
      });
    }

    for (func::FuncOp func : child.getOps<func::FuncOp>()) {
      auto it = renameMap.find(func.getSymName());
      if (it != renameMap.end())
        func.setSymName(it->second);
    }
    return WalkResult::advance();
  });
  return status;
}

static LogicalResult verifyCompilationPlan(ModuleOp module,
                                           bool cliBackendSpecified,
                                           CompilationPlan &plan) {
  if (failed(validatePTOBackendAttrs(module)))
    return failure();
  if (failed(verifyBackendChildShape(module, cliBackendSpecified)))
    return failure();

  bool checkedVPTOChild = false;
  for (ModuleOp child : module.getOps<ModuleOp>()) {
    std::optional<PTOBackend> childBackend;
    if (failed(parsePTOBackendAttr(child.getOperation(), childBackend)))
      return failure();
    if (childBackend == PTOBackend::VPTO) {
      checkedVPTOChild = true;
      if (failed(verifyVPTOBackendChild(child)))
        return failure();
    } else if (!childBackend && plan.effectiveBackend == PTOBackend::VPTO &&
               hasFunctionKernelKind(child)) {
      checkedVPTOChild = true;
      if (failed(verifyVPTOSourceSymbolNames(child)))
        return failure();
    }
  }

  if (!checkedVPTOChild && plan.effectiveBackend == PTOBackend::VPTO)
    return verifyVPTOSourceSymbolNames(module);

  if (failed(verifyUniqueBackendABIExports(plan.children)))
    return failure();
  if (failed(verifyExternalImportShape(plan.children)))
    return failure();
  if (failed(resolveExternalImports(plan.children)))
    return failure();

  return success();
}

static LogicalResult buildCompilationPlan(ModuleOp module,
                                          bool cliBackendSpecified,
                                          PTOBackend cliOrDefaultBackend,
                                          CompilationPlan &plan) {
  plan = CompilationPlan{};
  plan.effectiveBackend = cliOrDefaultBackend;

  bool mixedBackendMode = false;
  if (failed(resolveModuleBackend(module, cliBackendSpecified, mixedBackendMode,
                                  plan.effectiveBackend)))
    return failure();
  if (failed(collectBackendChildren(module, plan.children)))
    return failure();
  plan.mode = mixedBackendMode ? CompilationMode::MixedFatobj
                               : getCompilationMode(plan.effectiveBackend,
                                                    plan.children);
  if (failed(verifyCompilationPlan(module, cliBackendSpecified, plan)))
    return failure();
  return success();
}

static LogicalResult emitSharedPreBackendSeamIR(ModuleOp module,
                                                llvm::StringRef outputPath) {
  if (outputPath.empty())
    return success();

  if (outputPath == "-") {
    module->print(llvm::outs());
    llvm::outs() << "\n";
    llvm::outs().flush();
    return success();
  }

  std::error_code ec;
  llvm::ToolOutputFile outputFile(outputPath, ec, llvm::sys::fs::OF_None);
  if (ec) {
    llvm::errs() << "Error: failed to open seam IR file '" << outputPath
                 << "': " << ec.message() << "\n";
    return failure();
  }

  module->print(outputFile.os());
  outputFile.os() << "\n";
  outputFile.keep();
  return success();
}

static bool hasUnexpandedTileOps(ModuleOp module) {
  bool found = false;
  module.walk([&](Operation *op) {
    if (found)
      return;
    if (isa<pto::OpPipeInterface>(op))
      found = true;
  });
  return found;
}

static bool hasTilelangInlineHelpers(ModuleOp module) {
  bool found = false;
  module.walk([&](func::FuncOp func) {
    if (found)
      return;
    if (func->hasAttr("pto.tilelang.inline_proc"))
      found = true;
  });
  return found;
}

// --------------------------------------------------------------------------
// Post-process C++ output: rewrite marker calls into Tile member calls.
//
// We emit marker calls in EmitC IR because EmitC currently does not provide a
// first-class op for member-function invocation. After translation, we rewrite:
//   PTOAS__TILE_SET_VALUE(dst, offset, val) -> dst.SetValue(offset, val)
//   PTOAS__TILE_GET_VALUE(src, offset)      -> src.GetValue(offset)
//   PTOAS__TILE_DATA(obj)                   -> obj.data()
//   PTOAS__TILE_SET_VALIDSHAPE(obj, r, c)   -> obj.SetValidShape(r, c)
//   PTOAS__PTR_LOAD(ptr, offset)            -> ptr[offset]
//   PTOAS__PTR_STORE(ptr, offset, val)      -> ptr[offset] = val
//   PTOAS__EVENTID_ARRAY_LOAD(arr, idx)     -> arr[idx]
//   PTOAS__EVENTID_ARRAY_STORE(arr, idx, v) -> arr[idx] = v
// --------------------------------------------------------------------------
struct ParsedMarkerCall {
  size_t markerPos = std::string::npos;
  size_t rparenPos = std::string::npos;
  llvm::SmallVector<llvm::StringRef, 4> args;
};

struct MarkerRewriteSpec {
  llvm::StringRef marker;
  llvm::StringRef memberName;
  unsigned expectedNumArgs = 0;
};

struct MarkerSubscriptRewriteSpec {
  llvm::StringRef marker;
  unsigned expectedNumArgs = 0;
  bool isStore = false;
};

static bool parseMarkerArgs(llvm::StringRef argsRef,
                            llvm::SmallVectorImpl<llvm::StringRef> &args) {
  size_t partBegin = 0;
  int parenDepth = 0;
  for (size_t i = 0; i < argsRef.size(); ++i) {
    char c = argsRef[i];
    if (c == '(') {
      ++parenDepth;
      continue;
    }
    if (c == ')') {
      if (parenDepth > 0)
        --parenDepth;
      continue;
    }
    if (c == ',' && parenDepth == 0) {
      args.push_back(argsRef.slice(partBegin, i).trim());
      partBegin = i + 1;
    }
  }
  if (partBegin > argsRef.size())
    return false;
  args.push_back(argsRef.drop_front(partBegin).trim());
  return true;
}

static std::optional<ParsedMarkerCall>
findNextMarkerCall(const std::string &cpp, llvm::StringRef marker,
                   size_t searchPos) {
  ParsedMarkerCall call;
  call.markerPos = cpp.find(marker.str(), searchPos);
  if (call.markerPos == std::string::npos)
    return std::nullopt;

  size_t lparenPos = call.markerPos + marker.size();
  if (lparenPos >= cpp.size() || cpp[lparenPos] != '(')
    return ParsedMarkerCall{call.markerPos, std::string::npos, {}};

  size_t argsBegin = lparenPos + 1;
  int parenDepth = 0;
  for (size_t i = argsBegin; i < cpp.size(); ++i) {
    char c = cpp[i];
    if (c == '(') {
      ++parenDepth;
      continue;
    }
    if (c != ')')
      continue;
    if (parenDepth == 0) {
      call.rparenPos = i;
      break;
    }
    --parenDepth;
  }
  if (call.rparenPos == std::string::npos)
    return call;

  llvm::StringRef argsRef(cpp.data() + argsBegin, call.rparenPos - argsBegin);
  if (!parseMarkerArgs(argsRef, call.args))
    call.args.clear();
  return call;
}

template <typename BuildReplacementFn>
static bool rewriteMarkerCalls(std::string &cpp, llvm::StringRef marker,
                               BuildReplacementFn buildReplacement) {
  size_t searchPos = 0;
  bool changed = false;
  for (auto call = findNextMarkerCall(cpp, marker, searchPos); call;
       call = findNextMarkerCall(cpp, marker, searchPos)) {
    if (call->rparenPos == std::string::npos) {
      searchPos = call->markerPos + marker.size();
      continue;
    }

    std::optional<std::string> replacement = buildReplacement(*call);
    if (!replacement) {
      searchPos = call->rparenPos + 1;
      continue;
    }

    cpp.replace(call->markerPos, (call->rparenPos - call->markerPos) + 1,
                *replacement);
    changed = true;
    searchPos = call->markerPos + replacement->size();
  }
  return changed;
}

static bool rewriteMarkerCallToMember(std::string &cpp, llvm::StringRef marker,
                                      llvm::StringRef memberName,
                                      unsigned expectedNumArgs) {
  return rewriteMarkerCalls(
      cpp, marker, [&](const ParsedMarkerCall &call) -> std::optional<std::string> {
        if (call.args.size() != expectedNumArgs)
          return std::nullopt;

        std::string replacement;
        replacement.reserve(marker.size() + 16);
        replacement.append(call.args[0].str());
        replacement.push_back('.');
        replacement.append(memberName.str());
        replacement.push_back('(');
        if (expectedNumArgs >= 2)
          replacement.append(call.args[1].str());
        if (expectedNumArgs == 3) {
          replacement.append(", ");
          replacement.append(call.args[2].str());
        }
        replacement.push_back(')');
        return replacement;
      });
}

static void rewriteMarkerCallsToMembers(
    std::string &cpp, llvm::ArrayRef<MarkerRewriteSpec> rewrites) {
  bool changed = true;
  while (changed) {
    changed = false;
    for (const MarkerRewriteSpec &rewrite : rewrites) {
      changed |= rewriteMarkerCallToMember(cpp, rewrite.marker,
                                           rewrite.memberName,
                                           rewrite.expectedNumArgs);
    }
  }
}

static void rewriteTileGetSetValueMarkers(std::string &cpp) {
  static const MarkerRewriteSpec kTileMarkerRewrites[] = {
      {"PTOAS__TILE_SET_VALUE", "SetValue", 3},
      {"PTOAS__TILE_GET_VALUE", "GetValue", 2},
      {"PTOAS__TILE_DATA", "data", 1},
      {"PTOAS__TILE_SET_VALIDSHAPE", "SetValidShape", 3},
  };
  rewriteMarkerCallsToMembers(cpp, kTileMarkerRewrites);
}

static void rewriteAsyncEventMarkers(std::string &cpp) {
  static const MarkerRewriteSpec kAsyncEventMarkerRewrites[] = {
      {"PTOAS__ASYNC_EVENT_WAIT", "Wait", 2},
      {"PTOAS__ASYNC_EVENT_TEST", "Test", 2},
  };
  rewriteMarkerCallsToMembers(cpp, kAsyncEventMarkerRewrites);
}

// --------------------------------------------------------------------------
// EmitC cleanup: drop empty emitc.expression ops.
//
// After FormExpressions + CSE, EmitC expressions can become empty when their
// root op is CSE'd with an equivalent dominating value outside the expression
// region. Such expressions crash mlir::emitc::translateToCpp because
// ExpressionOp::getRootOp() returns nullptr.
// --------------------------------------------------------------------------
static void dropEmptyEmitCExpressions(Operation *rootOp) {
  llvm::SmallVector<emitc::ExpressionOp, 8> toErase;
  rootOp->walk([&](emitc::ExpressionOp expr) {
    if (expr.getRootOp())
      return;
    Block *body = expr.getBody();
    if (!body)
      return;
    auto yield = dyn_cast<emitc::YieldOp>(body->getTerminator());
    if (!yield || yield.getNumOperands() != 1)
      return;
    Value yielded = yield.getOperand(0);
    expr.getResult().replaceAllUsesWith(yielded);
    toErase.push_back(expr);
  });
  for (emitc::ExpressionOp expr : llvm::reverse(toErase))
    expr.erase();
}

static Attribute getDefaultEmitCVariableInitAttr(OpBuilder &builder, Type type) {
  if (auto intTy = dyn_cast<IntegerType>(type))
    return builder.getIntegerAttr(intTy, 0);
  if (isa<IndexType>(type))
    return builder.getIndexAttr(0);
  if (auto floatTy = dyn_cast<FloatType>(type))
    return builder.getFloatAttr(floatTy, 0.0);
  if (isa<emitc::OpaqueType, emitc::PointerType>(type))
    return emitc::OpaqueAttr::get(builder.getContext(), "");
  return Attribute{};
}

// FormExpressions may inline conditions into emitc.expression, but the C++
// emitter prints cf.br/cf.cond_br operands by variable name rather than by
// recursively emitting an expression. Materialize such operands so CFG-based
// lowering (e.g. scf.while -> cf.*) stays valid.
static void materializeControlFlowOperands(Operation *rootOp) {
  llvm::SmallVector<Operation *, 16> branches;
  rootOp->walk([&](Operation *op) {
    if (isa<cf::BranchOp, cf::CondBranchOp>(op))
      branches.push_back(op);
  });

  OpBuilder builder(rootOp->getContext());
  for (Operation *op : branches) {
    builder.setInsertionPoint(op);
    for (OpOperand &operand : op->getOpOperands()) {
      Value value = operand.get();
      auto expr = dyn_cast_or_null<emitc::ExpressionOp>(value.getDefiningOp());
      if (!expr)
        continue;

      Attribute initAttr =
          getDefaultEmitCVariableInitAttr(builder, value.getType());
      if (!initAttr)
        continue;

      Value tmp =
          builder.create<emitc::VariableOp>(op->getLoc(), value.getType(),
                                            initAttr)
              .getResult();
      builder.create<emitc::AssignOp>(op->getLoc(), tmp, value);
      operand.set(tmp);
    }
  }
}

static bool rewriteMarkerCallToSubscript(std::string &cpp, llvm::StringRef marker,
                                         unsigned expectedNumArgs,
                                         bool isStore) {
  return rewriteMarkerCalls(
      cpp, marker, [&](const ParsedMarkerCall &call) -> std::optional<std::string> {
        if (call.args.size() != expectedNumArgs)
          return std::nullopt;
        if (isStore) {
          return (call.args[0] + "[" + call.args[1] + "] = " + call.args[2])
              .str();
        }
        return (call.args[0] + "[" + call.args[1] + "]").str();
      });
}

static void rewriteMarkerCallsToSubscripts(
    std::string &cpp, llvm::ArrayRef<MarkerSubscriptRewriteSpec> rewrites) {
  bool changed = true;
  while (changed) {
    changed = false;
    for (const MarkerSubscriptRewriteSpec &rewrite : rewrites) {
      changed |= rewriteMarkerCallToSubscript(cpp, rewrite.marker,
                                              rewrite.expectedNumArgs,
                                              rewrite.isStore);
    }
  }
}

static void rewritePtrScalarMarkers(std::string &cpp) {
  static const MarkerSubscriptRewriteSpec kPtrMarkerRewrites[] = {
      {"PTOAS__PTR_LOAD", 2, false},
      {"PTOAS__PTR_STORE", 3, true},
  };
  rewriteMarkerCallsToSubscripts(cpp, kPtrMarkerRewrites);
}

static void rewriteEventIdArrayMarkers(std::string &cpp) {
  static const MarkerSubscriptRewriteSpec kEventIdMarkerRewrites[] = {
      {"PTOAS__EVENTID_ARRAY_LOAD", 2, false},
      {"PTOAS__EVENTID_ARRAY_STORE", 3, true},
  };
  rewriteMarkerCallsToSubscripts(cpp, kEventIdMarkerRewrites);
}

static bool rewriteAddPtrTraceMarkers(std::string &cpp, bool showTrace) {
  size_t searchPos = 0;
  bool changed = false;
  for (auto call = findNextMarkerCall(cpp, "PTOAS__ADDPTR_TRACE", searchPos);
       call; call = findNextMarkerCall(cpp, "PTOAS__ADDPTR_TRACE", searchPos)) {
    if (call->rparenPos == std::string::npos) {
      searchPos = call->markerPos + 1;
      continue;
    }
    if (call->args.size() != 3) {
      searchPos = call->rparenPos + 1;
      continue;
    }

    std::string replacement;
    if (showTrace) {
      replacement.reserve(64);
      replacement.append("/* ADDPTR_TRACE: ");
      replacement.append(call->args[0].str());
      replacement.append(" = ");
      replacement.append(call->args[1].str());
      replacement.append(" + ");
      replacement.append(call->args[2].str());
      replacement.append(" */");
    }

    size_t replaceEnd = call->rparenPos;
    if (!showTrace) {
      size_t i = call->rparenPos + 1;
      while (i < cpp.size() && std::isspace(static_cast<unsigned char>(cpp[i])))
        ++i;
      if (i < cpp.size() && cpp[i] == ';')
        replaceEnd = i;
    }

    cpp.replace(call->markerPos, (replaceEnd - call->markerPos) + 1,
                replacement);
    changed = true;
    searchPos = call->markerPos + replacement.size();
  }
  return changed;
}

static bool isGeneratedGlobalTensorDecl(llvm::StringRef trimmed,
                                        llvm::StringRef &decl,
                                        llvm::StringRef &varName) {
  if (!trimmed.starts_with("GlobalTensor<") || !trimmed.ends_with(";") ||
      trimmed.contains('=') || trimmed.contains('(')) {
    return false;
  }

  decl = trimmed.drop_back().rtrim();
  size_t lastWs = decl.find_last_of(" \t");
  if (lastWs == llvm::StringRef::npos)
    return false;
  varName = decl.drop_front(lastWs + 1);
  if (!varName.starts_with("v") || varName.size() <= 1)
    return false;
  return llvm::all_of(varName.drop_front(1),
                      [](char c) { return std::isdigit(c); });
}

static void rewriteHoistedGlobalTensorDecls(std::string &cpp) {
  // When `declareVariablesAtTop` is enabled, the C++ emitter hoists SSA value
  // declarations to the top of the function and emits assignments later. This
  // requires the C++ type to be default-constructible.
  //
  // `GlobalTensor<...>` from pto-isa does NOT have a default constructor, so a
  // hoisted declaration like:
  //   GlobalTensor<...> v42;
  // fails to compile. Initialize those hoisted temporaries with a null pointer
  // so they are constructible:
  //   GlobalTensor<...> v42(nullptr);
  //
  // We keep the assignment later; the null-initialized value is never used.
  std::string out;
  out.reserve(cpp.size() + 64);

  llvm::StringRef ref(cpp);
  while (!ref.empty()) {
    auto split = ref.split('\n');
    llvm::StringRef line = split.first;
    llvm::StringRef rest = split.second;

    llvm::StringRef trimmed = line.trim();
    bool rewritten = false;
    llvm::StringRef decl;
    llvm::StringRef varName;
    if (isGeneratedGlobalTensorDecl(trimmed, decl, varName)) {
      size_t indentLen = line.find_first_not_of(" \t");
      if (indentLen == std::string::npos)
        indentLen = 0;
      llvm::StringRef indent = line.take_front(indentLen);

      out.append(indent.str());
      out.append(decl.str());
      out.append("(nullptr);");
      rewritten = true;
    }

    if (!rewritten)
      out.append(line.str());
    if (!rest.empty())
      out.push_back('\n');
    ref = rest;
  }

  cpp.swap(out);
}

namespace {
struct ConstantDeclCandidate {
  size_t declLine = 0;
  std::string indent;
  std::string type;
  bool hasInitializer = false;
  std::string initializer;
  size_t assignmentCount = 0;
  size_t assignmentLine = 0;
  std::string assignmentRhs;
};
} // namespace

static bool isGeneratedValueName(llvm::StringRef name) {
  if (!name.consume_front("v") || name.empty())
    return false;
  return llvm::all_of(name, [](char c) { return std::isdigit(c); });
}

static bool isConstFoldableScalarType(llvm::StringRef type) {
  type = type.trim();
  if (type.starts_with("const ") || type.starts_with("constexpr "))
    return false;
  return llvm::StringSwitch<bool>(type)
      .Cases("bool", "float", "double", "half", "bfloat16_t", true)
      .Cases("int8_t", "uint8_t", "int16_t", "uint16_t", true)
      .Cases("int32_t", "uint32_t", "int64_t", "uint64_t", true)
      .Default(false);
}

static bool isLiteralInitializer(llvm::StringRef rhs) {
  rhs = rhs.trim();
  if (rhs.empty())
    return false;
  if (rhs == "true" || rhs == "false" || rhs == "nullptr")
    return true;

  static const llvm::Regex kIntLiteral(
      R"(^[+-]?(0[xX][0-9A-Fa-f]+|[0-9]+)[uUlL]*$)");
  static const llvm::Regex kFloatLiteral(
      R"(^[+-]?(([0-9]+\.[0-9]*|\.[0-9]+|[0-9]+)([eE][+-]?[0-9]+)?|[0-9]+[eE][+-]?[0-9]+)[fF]?$)");
  static const llvm::Regex kHexFloatLiteral(
      R"(^[+-]?0[xX]([0-9A-Fa-f]+\.[0-9A-Fa-f]*|[0-9A-Fa-f]+|\.[0-9A-Fa-f]+)[pP][+-]?[0-9]+[fF]?$)");
  static const llvm::Regex kSpecialFloatLiteral(
      R"(^[+-]?(nan|inf)[fF]?$)");

  return kIntLiteral.match(rhs) || kFloatLiteral.match(rhs) ||
         kHexFloatLiteral.match(rhs) || kSpecialFloatLiteral.match(rhs);
}

static std::string normalizeConstInitializer(llvm::StringRef type,
                                             llvm::StringRef rhs) {
  type = type.trim();
  rhs = rhs.trim();
  if (type == "bool") {
    if (rhs == "0" || rhs == "false")
      return "false";
    if (rhs == "1" || rhs == "-1" || rhs == "true")
      return "true";
  }
  return rhs.str();
}

static bool parseConstantDeclarationLine(llvm::StringRef line,
                                         ConstantDeclCandidate &candidate,
                                         std::string &valueName) {
  llvm::StringRef trimmed = line.trim();
  if (trimmed.empty() || trimmed.starts_with("#") || trimmed.starts_with("//") ||
      !trimmed.ends_with(";"))
    return false;

  llvm::StringRef body = trimmed.drop_back().rtrim();
  if (body.starts_with("return") || body.starts_with("goto ") ||
      body.starts_with("if ") || body.starts_with("if(") ||
      body.starts_with("switch ") || body.starts_with("switch(") ||
      body.starts_with("for ") || body.starts_with("for(") ||
      body.starts_with("while ") || body.starts_with("while(") ||
      body.starts_with("case ") || body == "default")
    return false;

  llvm::StringRef lhs = body;
  llvm::StringRef rhs;
  if (size_t eqPos = body.find('='); eqPos != llvm::StringRef::npos) {
    lhs = body.take_front(eqPos).rtrim();
    rhs = body.drop_front(eqPos + 1).trim();
  }

  size_t lastWs = lhs.find_last_of(" \t");
  if (lastWs == llvm::StringRef::npos)
    return false;

  llvm::StringRef type = lhs.take_front(lastWs).rtrim();
  llvm::StringRef name = lhs.drop_front(lastWs + 1).trim();
  if (!isGeneratedValueName(name) || !isConstFoldableScalarType(type))
    return false;

  size_t indentLen = line.find_first_not_of(" \t");
  if (indentLen == llvm::StringRef::npos)
    indentLen = 0;
  candidate.indent = line.take_front(indentLen).str();
  candidate.type = type.str();
  valueName = name.str();

  if (!rhs.empty()) {
    if (!isLiteralInitializer(rhs))
      return false;
    candidate.hasInitializer = true;
    candidate.initializer = normalizeConstInitializer(type, rhs);
  }

  return true;
}

static bool parseGeneratedValueAssignment(llvm::StringRef line,
                                          llvm::StringRef &valueName,
                                          llvm::StringRef &rhs) {
  llvm::StringRef trimmed = line.trim();
  if (trimmed.empty() || trimmed.starts_with("#") || trimmed.starts_with("//") ||
      !trimmed.ends_with(";"))
    return false;

  llvm::StringRef body = trimmed.drop_back().rtrim();
  size_t eqPos = body.find('=');
  if (eqPos == llvm::StringRef::npos)
    return false;

  llvm::StringRef lhs = body.take_front(eqPos).rtrim();
  rhs = body.drop_front(eqPos + 1).trim();
  if (!isGeneratedValueName(lhs))
    return false;
  valueName = lhs;
  return true;
}

static void rewriteScalarConstantDecls(std::string &cpp) {
  llvm::SmallVector<std::string, 0> lines;
  for (llvm::StringRef ref(cpp); !ref.empty(); ref = ref.split('\n').second) {
    auto split = ref.split('\n');
    lines.push_back(split.first.str());
  }

  llvm::SmallVector<bool, 0> eraseLine(lines.size(), false);
  auto rewriteSegment = [&](size_t beginLine, size_t endLine) {
    llvm::StringMap<ConstantDeclCandidate> candidates;

    for (size_t i = beginLine; i <= endLine; ++i) {
      ConstantDeclCandidate candidate;
      std::string valueName;
      if (parseConstantDeclarationLine(lines[i], candidate, valueName)) {
        candidate.declLine = i;
        candidates[valueName] = std::move(candidate);
        continue;
      }

      llvm::StringRef assignedName;
      llvm::StringRef rhs;
      if (!parseGeneratedValueAssignment(lines[i], assignedName, rhs))
        continue;

      auto it = candidates.find(assignedName);
      if (it == candidates.end())
        continue;

      ConstantDeclCandidate &info = it->second;
      ++info.assignmentCount;
      info.assignmentLine = i;
      info.assignmentRhs = rhs.str();
    }

    for (auto &entry : candidates) {
      llvm::StringRef valueName = entry.getKey();
      ConstantDeclCandidate &info = entry.getValue();

      std::string initializer;
      if (info.hasInitializer) {
        if (info.assignmentCount != 0)
          continue;
        initializer = info.initializer;
      } else {
        if (info.assignmentCount != 1)
          continue;
        if (!isLiteralInitializer(info.assignmentRhs))
          continue;
        initializer = normalizeConstInitializer(
            info.type, llvm::StringRef(info.assignmentRhs));
        eraseLine[info.assignmentLine] = true;
      }

      lines[info.declLine] = (info.indent + "const " + info.type + " " +
                              valueName.str() + " = " + initializer + ";");
    }
  };

  int braceDepth = 0;
  size_t segmentStart = 0;
  for (size_t i = 0; i < lines.size(); ++i) {
    int depthBefore = braceDepth;
    for (char c : lines[i]) {
      if (c == '{')
        ++braceDepth;
      else if (c == '}')
        --braceDepth;
    }

    if (depthBefore == 0 && braceDepth > 0)
      segmentStart = i;
    if (depthBefore > 0 && braceDepth == 0)
      rewriteSegment(segmentStart, i);
  }

  std::string out;
  out.reserve(cpp.size());
  for (size_t i = 0; i < lines.size(); ++i) {
    if (eraseLine[i])
      continue;
    out.append(lines[i]);
    if (i + 1 != lines.size())
      out.push_back('\n');
  }
  cpp.swap(out);
}

static bool shouldDeclareVariablesAtTop(ModuleOp module) {
  auto hasMultiBlockFunc = [](auto func) { return func.getBlocks().size() > 1; };
  return llvm::any_of(module.getOps<func::FuncOp>(), hasMultiBlockFunc) ||
         llvm::any_of(module.getOps<emitc::FuncOp>(), hasMultiBlockFunc);
}

static LogicalResult emitEmitCSourceArtifact(ModuleOp module,
                                             bool emitAddPtrTraceComments,
                                             std::string &cppOutput,
                                             llvm::raw_ostream &diagOS) {
  dropEmptyEmitCExpressions(module);
  materializeControlFlowOperands(module);
  if (failed(reorderEmitCFunctions(module))) {
    diagOS << "Error: Failed to order emitted functions for C++ emission.\n";
    return failure();
  }

  cppOutput.clear();
  llvm::raw_string_ostream cppOS(cppOutput);
  bool declareVariablesAtTop = shouldDeclareVariablesAtTop(module);
  if (failed(emitc::translateToCpp(
          module, cppOS, /*declareVariablesAtTop=*/declareVariablesAtTop))) {
    diagOS << "Error: Failed to emit C++.\n";
    return failure();
  }
  cppOS.flush();
  rewriteTileGetSetValueMarkers(cppOutput);
  rewriteAsyncEventMarkers(cppOutput);
  rewritePtrScalarMarkers(cppOutput);
  rewriteEventIdArrayMarkers(cppOutput);
  rewriteAddPtrTraceMarkers(cppOutput, emitAddPtrTraceComments);
  rewriteScalarConstantDecls(cppOutput);
  rewriteHoistedGlobalTensorDecls(cppOutput);
  return success();
}

static LogicalResult configureSharedPTOPipeline(PassManager &pm,
                                                PTOBuildLevel effectiveLevel) {
  if (failed(applyPassManagerCLOptions(pm)))
    return failure();

  pm.addNestedPass<mlir::func::FuncOp>(
      pto::createPTOAssignDefaultFrontendPipeIdPass());
  pm.addNestedPass<mlir::func::FuncOp>(
      pto::createPTOLowerFrontendPipeOpsPass());
  // pm.addNestedPass<mlir::func::FuncOp>(pto::createPTOVerifyTFreePass());
  pm.addPass(pto::createPTOInferValidatePipeInitPass());
  pm.addNestedPass<mlir::func::FuncOp>(pto::createLoweringSyncToPipePass());

  if (!disableInferLayout)
    pm.addNestedPass<mlir::func::FuncOp>(pto::createInferPTOLayoutPass());
  pm.addNestedPass<mlir::func::FuncOp>(pto::createPTOA5NormalizeTMovPass());
  pm.addPass(pto::createPTOViewToMemrefPass());

  if (effectiveLevel != PTOBuildLevel::Level3) {
    PlanMemoryOptions planMemoryOption;
    planMemoryOption.memMode = MemPlanMode::LOCAL_MEM_PLAN;
    planMemoryOption.enableGlobalReuse = false;
    planMemoryOption.enablePrintMemoryAllocatedSize = false;
    pm.addPass(pto::createPlanMemoryPass(planMemoryOption));
  }
  pm.addPass(pto::createPTOResolveReservedBuffersPass());

  // Conditionally add one automatic synchronization mode. Barrier-all is a
  // conservative standalone pass; InsertSync and GraphSyncSolver are set/wait
  // solvers.
  if (enableInsertSync)
    pm.addNestedPass<mlir::func::FuncOp>(pto::createPTOInsertSyncPass());
  else if (enableInjectBarrierAllSync)
    pm.addNestedPass<mlir::func::FuncOp>(
        pto::createPTOInjectBarrierAllSyncPass());
  else if (enableGraphSyncSolver) {
    PTOGraphSyncSolverOptions graphSyncOpts;
    graphSyncOpts.eventIdNumMax = graphSyncSolverEventIdMax;
    pm.addNestedPass<mlir::func::FuncOp>(
        pto::createPTOGraphSyncSolverPass(graphSyncOpts));
  }

  return success();
}

static void addSharedPTOSeamPasses(PassManager &pm) {
  // Reintroduce tile-native handles once on the shared mainline so both
  // backends consume the same post-planning seam IR.
  pm.addPass(pto::createPTOMaterializeTileHandlesPass());
  pm.addPass(createCSEPass());
}

static void prepareVPTOForEmission(PassManager &pm) {
  auto &kernelModulePM = pm.nest<ModuleOp>();
  kernelModulePM.addPass(createCanonicalizerPass());
  kernelModulePM.addPass(createCSEPass());
  kernelModulePM.addPass(pto::createVPTOPtrNormalizePass());
  kernelModulePM.addPass(pto::createVPTOPtrCastCleanupPass());
  kernelModulePM.addPass(createReconcileUnrealizedCastsPass());
  kernelModulePM.addNestedPass<func::FuncOp>(
      createVPTOExpandWrapperOpsPass());
  kernelModulePM.addPass(createCSEPass());
  kernelModulePM.addNestedPass<func::FuncOp>(
      pto::createPTOInferVPTOVecScopePass());
  kernelModulePM.addPass(createCanonicalizerPass());
  kernelModulePM.addPass(createCSEPass());
  kernelModulePM.addPass(pto::createPTOValidateVPTOEmissionIRPass());
}

static void lowerPTOToVPTOBackend(PassManager &pm, int argc, char **argv) {
  // TileOp Expand path:
  //   1. ExpandTileOp: instantiate TileLang DSL templates, replace tile ops
  //      with func.call to template functions (tile_buf params)
  //   2. InlineLibCall: inline template function bodies
  //   3. FoldTileBufIntrinsics: fold tile_buf_addr / tile_valid_rows /
  //      tile_valid_cols to concrete memref/constant values
  auto &kernelModulePM = pm.nest<ModuleOp>();
  pto::ExpandTileOpOptions expandOpts = resolveExpandTileOpOptions(argc, argv);
  kernelModulePM.addPass(pto::createExpandTileOpPass(expandOpts));

  kernelModulePM.addPass(pto::createPTOInlineLibCallPass());
  kernelModulePM.addNestedPass<mlir::func::FuncOp>(
      pto::createFoldTileBufIntrinsicsPass());
  // FoldTileBufIntrinsics materializes many constant branch conditions.
  // Clean them up immediately on the TileOp expansion path before the
  // authoring-stage VPTO verifier and let the existing CSE passes remove the
  // resulting dead values later in the pipeline.
  kernelModulePM.addPass(mlir::createSCCPPass());
  kernelModulePM.addPass(mlir::createCanonicalizerPass());
}

static void inlineTilelangHelpersOnVPTOInput(PassManager &pm) {
  auto &kernelModulePM = pm.nest<ModuleOp>();
  kernelModulePM.addPass(pto::createPTOInlineLibCallPass());
  kernelModulePM.addPass(mlir::createSCCPPass());
  kernelModulePM.addPass(mlir::createCanonicalizerPass());
}

static pto::VPTOEmissionOptions buildVPTOEmissionOptions() {
  pto::VPTOEmissionOptions options;
  options.dumpVPTOIR = false;
  options.targetTriple = "hiipu64-hisilicon-cce";
  return options;
}

struct CompiledBackendChild {
  const BackendChildPlan *plan = nullptr;
  PTOBackend backend = PTOBackend::EmitC;
  std::optional<FunctionKernelKind> kernelKind;
  ModuleOp vptoModule;
  std::string cppSource;
  pto::EmittedLLVMModule cubeModule;
  pto::EmittedLLVMModule vectorModule;
};

static LogicalResult runVPTOBackendPipeline(ModuleOp module, int argc,
                                            char **argv,
                                            bool hasTileOpsToExpand,
                                            bool hasTilelangHelpers);

static LogicalResult runSharedPTOToSeamPipeline(ModuleOp module,
                                                PTOBuildLevel effectiveLevel,
                                                llvm::StringRef arch,
                                                llvm::raw_ostream &diagOS) {
  PassManager pm(module.getContext());
  if (failed(configureSharedPTOPipeline(pm, effectiveLevel)))
    return failure();
  addSharedPTOSeamPasses(pm);
  if (failed(applyConfiguredPassManagerCLOptions(pm, "mixed child PTOAS pipeline")))
    return failure();
  module->setAttr("pto.target_arch", StringAttr::get(module.getContext(), arch));
  if (failed(pm.run(module))) {
    diagOS << "Error: mixed child PTO pass execution failed.\n";
    return failure();
  }
  return success();
}

static LogicalResult reconcileExternalDeclarationTypes(ModuleOp module) {
  SymbolTable symbolTable(module);
  llvm::StringMap<SmallVector<Type>> operandTypesByExternalCallee;
  LogicalResult status = success();

  module.walk([&](func::CallOp call) {
    if (failed(status))
      return WalkResult::interrupt();
    auto callee = symbolTable.lookup<func::FuncOp>(call.getCallee());
    if (!callee || !callee.isDeclaration())
      return WalkResult::advance();

    SmallVector<Type> operandTypes(call.getOperandTypes().begin(),
                                   call.getOperandTypes().end());
    auto [it, inserted] =
        operandTypesByExternalCallee.try_emplace(call.getCallee(), operandTypes);
    if (!inserted && it->second != operandTypes) {
      status = call.emitError()
               << "external declaration '" << call.getCallee()
               << "' is called with inconsistent operand types after PTO "
                  "normalization";
      return WalkResult::interrupt();
    }
    return WalkResult::advance();
  });
  if (failed(status))
    return failure();

  for (func::FuncOp func : module.getOps<func::FuncOp>()) {
    if (!func.isDeclaration())
      continue;
    auto it = operandTypesByExternalCallee.find(func.getSymName());
    if (it == operandTypesByExternalCallee.end())
      continue;
    if (func.getResultTypes().empty() &&
        llvm::equal(func.getArgumentTypes(), it->second))
      continue;
    if (!func.getResultTypes().empty()) {
      return func.emitError()
             << "external declarations with results are not supported in mixed "
                "backend object emission";
    }
    func.setFunctionType(
        FunctionType::get(func.getContext(), it->second, TypeRange{}));
  }
  return success();
}

static LogicalResult runEmitCBackendPipeline(ModuleOp module,
                                             PTOBuildLevel effectiveLevel,
                                             llvm::StringRef arch,
                                             llvm::raw_ostream &diagOS) {
  PassManager sharedPM(module.getContext());
  // Shared PTO normalization can rewrite pto.aicore argument types before the
  // private external declarations that call them are reconciled below.
  sharedPM.enableVerifier(false);
  if (failed(configureSharedPTOPipeline(sharedPM, effectiveLevel)))
    return failure();
  addSharedPTOSeamPasses(sharedPM);
  if (failed(applyConfiguredPassManagerCLOptions(sharedPM,
                                                 "mixed child shared pipeline")))
    return failure();
  module->setAttr("pto.target_arch", StringAttr::get(module.getContext(), arch));
  if (failed(sharedPM.run(module))) {
    diagOS << "Error: mixed EmitC child shared pass execution failed.\n";
    return failure();
  }

  if (failed(reconcileExternalDeclarationTypes(module)))
    return failure();

  PassManager emitCPM(module.getContext());
  if (arch == "a3")
    emitCPM.addPass(pto::createEmitPTOManualPass(pto::PTOArch::A3));
  else
    emitCPM.addPass(pto::createEmitPTOManualPass(pto::PTOArch::A5));
  emitCPM.addPass(emitc::createFormExpressionsPass());
  emitCPM.addPass(mlir::createCSEPass());
  if (failed(applyConfiguredPassManagerCLOptions(emitCPM,
                                                 "mixed child EmitC pipeline")))
    return failure();
  if (failed(emitCPM.run(module))) {
    diagOS << "Error: mixed EmitC child pass execution failed.\n";
    return failure();
  }
  return success();
}

static LogicalResult compileMixedEmitCChild(const BackendChildPlan &plan,
                                            PTOBuildLevel effectiveLevel,
                                            llvm::StringRef arch,
                                            CompiledBackendChild &compiled,
                                            llvm::raw_ostream &diagOS) {
  ModuleOp module = plan.module;
  if (failed(runEmitCBackendPipeline(module, effectiveLevel, arch, diagOS)))
    return failure();

  compiled.plan = &plan;
  compiled.backend = PTOBackend::EmitC;
  compiled.kernelKind = plan.kernelKind;
  if (failed(emitEmitCSourceArtifact(module, emitAddPtrTrace,
                                     compiled.cppSource, diagOS)))
    return failure();
  return success();
}

static LogicalResult compileMixedVPTOChild(const BackendChildPlan &plan,
                                           PTOBuildLevel effectiveLevel,
                                           llvm::StringRef arch, int argc,
                                           char **argv,
                                           CompiledBackendChild &compiled,
                                           llvm::raw_ostream &diagOS) {
  ModuleOp module = plan.module;
  const bool hasTileOpsToExpand = hasUnexpandedTileOps(module);
  const bool hasTilelangHelpers = hasTilelangInlineHelpers(module);

  if (hasTileOpsToExpand) {
    if (failed(runSharedPTOToSeamPipeline(module, effectiveLevel, arch, diagOS)))
      return failure();
  }

  if (failed(runVPTOBackendPipeline(module, argc, argv, hasTileOpsToExpand,
                                    hasTilelangHelpers)))
    return failure();

  compiled.plan = &plan;
  compiled.backend = PTOBackend::VPTO;
  compiled.kernelKind = plan.kernelKind;
  compiled.vptoModule = module;

  pto::VPTOEmissionOptions options = buildVPTOEmissionOptions();
  if (failed(pto::lowerVPTOModuleToLLVMModules(
          module, options, compiled.cubeModule, compiled.vectorModule, diagOS))) {
    diagOS << "Error: Failed to lower mixed VPTO child to LLVM modules.\n";
    return failure();
  }
  return success();
}

static LogicalResult compilePTOChildren(CompilationPlan &plan,
                                        PTOBuildLevel effectiveLevel,
                                        llvm::StringRef arch, int argc,
                                        char **argv,
                                        SmallVectorImpl<CompiledBackendChild>
                                            &compiledChildren,
                                        llvm::raw_ostream &diagOS) {
  for (BackendChildPlan &child : plan.children) {
    if (failed(applyResolvedExternalImports(child)))
      return failure();

    CompiledBackendChild compiled;
    if (child.backend == PTOBackend::EmitC) {
      if (failed(compileMixedEmitCChild(child, effectiveLevel, arch, compiled,
                                        diagOS)))
        return failure();
    } else {
      if (failed(compileMixedVPTOChild(child, effectiveLevel, arch, argc, argv,
                                       compiled, diagOS)))
        return failure();
    }
    compiledChildren.push_back(std::move(compiled));
  }
  return success();
}

static LogicalResult createTempPath(llvm::StringRef prefix,
                                    llvm::StringRef suffix,
                                    std::string &path,
                                    llvm::raw_ostream &diagOS) {
  llvm::SmallString<128> tempPath;
  int fd = -1;
  std::error_code ec =
      llvm::sys::fs::createTemporaryFile(prefix, suffix, fd, tempPath);
  if (ec) {
    diagOS << "Error: failed to create temporary file for " << prefix << suffix
           << ": " << ec.message() << "\n";
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

static LogicalResult compileMixedDeviceObjects(
    ArrayRef<CompiledBackendChild> compiledChildren,
    const ObjectEmissionToolchain &toolchain,
    SmallVectorImpl<BackendArtifact> &artifacts,
    SmallVectorImpl<std::string> &tempPaths, llvm::raw_ostream &diagOS) {
  for (const CompiledBackendChild &child : compiledChildren) {
    if (child.backend == PTOBackend::EmitC) {
      std::string cppPath;
      std::string objPath;
      std::string stderrPath;
      if (failed(createTempPath("ptoas-mixed-emitc", ".cpp", cppPath, diagOS)) ||
          failed(createTempPath("ptoas-mixed-emitc", ".o", objPath, diagOS)) ||
          failed(createTempPath("ptoas-mixed-emitc", ".log", stderrPath,
                                diagOS)))
        return failure();
      tempPaths.push_back(cppPath);
      tempPaths.push_back(objPath);
      tempPaths.push_back(stderrPath);

      if (failed(emitCppFatobj(child.cppSource, cppPath, objPath, toolchain,
                               stderrPath, diagOS)))
        return failure();

      BackendArtifact artifact;
      artifact.backend = PTOBackend::EmitC;
      artifact.kind = BackendArtifactKind::Fatobj;
      artifact.path = objPath;
      if (child.plan)
        for (const BackendExportPlan &exportPlan : child.plan->exports)
          artifact.exportedABISymbols.push_back(exportPlan.abiName);
      artifacts.push_back(std::move(artifact));
      continue;
    }

    auto emitVPTOObject = [&](llvm::Module *llvmModule,
                              ObjectEmissionDeviceTarget target,
                              BackendArtifactKind artifactKind) -> LogicalResult {
      if (!llvmModule)
        return success();
      std::string llPath;
      std::string objPath;
      std::string stderrPath;
      if (failed(createTempPath("ptoas-mixed-vpto", ".ll", llPath, diagOS)) ||
          failed(createTempPath("ptoas-mixed-vpto", ".o", objPath, diagOS)) ||
          failed(createTempPath("ptoas-mixed-vpto", ".log", stderrPath, diagOS)))
        return failure();
      tempPaths.push_back(llPath);
      tempPaths.push_back(objPath);
      tempPaths.push_back(stderrPath);

      if (failed(writeLLVMModule(*llvmModule, llPath, diagOS)))
        return failure();
      if (failed(compileLLVMToDeviceObject(llPath, objPath, target, toolchain,
                                           stderrPath, diagOS)))
        return failure();

      BackendArtifact artifact;
      artifact.backend = PTOBackend::VPTO;
      artifact.kind = artifactKind;
      artifact.path = objPath;
      if (child.plan)
        for (const BackendExportPlan &exportPlan : child.plan->exports)
          artifact.exportedABISymbols.push_back(exportPlan.abiName);
      artifacts.push_back(std::move(artifact));
      return success();
    };

    if (failed(emitVPTOObject(child.vectorModule.module.get(),
                              ObjectEmissionDeviceTarget::Vector,
                              BackendArtifactKind::VectorObject)))
      return failure();
    if (failed(emitVPTOObject(child.cubeModule.module.get(),
                              ObjectEmissionDeviceTarget::Cube,
                              BackendArtifactKind::CubeObject)))
      return failure();
  }

  if (artifacts.empty()) {
    diagOS << "Error: mixed fatobj compilation produced no device objects.\n";
    return failure();
  }
  return success();
}

static LogicalResult buildMixedHostStubSource(
    ArrayRef<CompiledBackendChild> compiledChildren, std::string &stubSource,
    llvm::raw_ostream &diagOS) {
  SmallVector<ModuleOp, 4> stubModules;
  for (const CompiledBackendChild &child : compiledChildren) {
    if (child.backend != PTOBackend::VPTO || !child.vptoModule)
      continue;
    bool hasAICoreKernel = false;
    ModuleOp vptoModule = child.vptoModule;
    for (func::FuncOp func : vptoModule.getOps<func::FuncOp>()) {
      if (!func.isExternal() && func->hasAttr("pto.aicore")) {
        hasAICoreKernel = true;
        break;
      }
    }
    if (hasAICoreKernel)
      stubModules.push_back(child.vptoModule);
  }
  if (stubModules.empty()) {
    stubSource = "#ifndef __global__\n#define __global__\n#endif\n\n"
                 "#ifndef __gm__\n#define __gm__\n#endif\n\n";
    return success();
  }
  if (failed(pto::emitVPTOHostStubSource(stubModules, stubSource, diagOS))) {
    diagOS << "Error: Failed to emit mixed VPTO host stub source.\n";
    return failure();
  }
  return success();
}

static LogicalResult linkMixedFatobj(
    ArrayRef<BackendArtifact> artifacts, llvm::StringRef stubSource,
    llvm::StringRef outputPath, const ObjectEmissionToolchain &toolchain,
    SmallVectorImpl<std::string> &tempPaths, llvm::raw_ostream &diagOS) {
  std::string stubPath;
  std::string stderrPath;
  if (failed(createTempPath("ptoas-mixed-stub", ".cpp", stubPath, diagOS)) ||
      failed(createTempPath("ptoas-mixed", ".log", stderrPath, diagOS)))
    return failure();
  tempPaths.push_back(stubPath);
  tempPaths.push_back(stderrPath);

  if (failed(writeHostStubSource(stubSource, stubPath, diagOS)))
    return failure();

  std::string sanitizedModuleId =
      outputPath.empty() || outputPath == "-" ? "ptoas_mixed_fatobj"
                                              : outputPath.str();
  for (char &c : sanitizedModuleId)
    if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_')
      c = '_';

  SmallVector<std::string, 4> fatobjPaths;
  for (size_t i = 0, e = artifacts.size(); i < e; ++i) {
    const BackendArtifact &artifact = artifacts[i];
    if (artifact.kind == BackendArtifactKind::Fatobj) {
      fatobjPaths.push_back(artifact.path);
      continue;
    }

    std::string mergedObjPath;
    std::string fatobjPath;
    if (failed(createTempPath("ptoas-mixed-backend-merged", ".o", mergedObjPath,
                              diagOS)) ||
        failed(createTempPath("ptoas-mixed-backend-fatobj", ".o", fatobjPath,
                              diagOS)))
      return failure();
    tempPaths.push_back(mergedObjPath);
    tempPaths.push_back(fatobjPath);

    if (failed(mergeDeviceObjects(ArrayRef<std::string>(artifact.path),
                                  mergedObjPath, toolchain,
                                  stderrPath, diagOS)))
      return failure();

    std::string groupModuleId = sanitizedModuleId + "_backend_" +
                                std::to_string(i);
    if (failed(compileStubToFatobj(stubPath, mergedObjPath, fatobjPath,
                                   groupModuleId, toolchain, stderrPath,
                                   diagOS)))
      return failure();
    fatobjPaths.push_back(fatobjPath);
  }

  if (fatobjPaths.empty()) {
    diagOS << "Error: mixed fatobj linking produced no backend fatobjs.\n";
    return failure();
  }
  if (fatobjPaths.size() == 1) {
    if (std::error_code ec =
            llvm::sys::fs::copy_file(fatobjPaths.front(), outputPath)) {
      diagOS << "Error: failed to copy mixed backend fatobj to " << outputPath
             << ": " << ec.message() << "\n";
      return failure();
    }
    return success();
  }

  if (failed(linkFatobjs(fatobjPaths, outputPath, toolchain, stderrPath,
                         diagOS)))
    return failure();
  return success();
}

static int runMixedFatobjPlan(CompilationPlan &plan,
                              PTOBuildLevel effectiveLevel,
                              llvm::StringRef arch, int argc, char **argv,
                              llvm::StringRef outputPath,
                              llvm::raw_ostream &diagOS) {
  if (emitMlirIR || emitVPTO || ptoPrintSeamIR || !ptoSeamIRFile.empty()) {
    diagOS << "Error: mixed pto.backend fatobj mode does not support "
              "debug IR output flags.\n";
    return 1;
  }
  if (outputPath.empty() || outputPath == "-") {
    diagOS << "Error: mixed pto.backend fatobj mode requires an explicit "
              "file path passed with -o.\n";
    return 1;
  }

  SmallVector<CompiledBackendChild, 4> compiledChildren;
  if (failed(compilePTOChildren(plan, effectiveLevel, arch, argc, argv,
                                compiledChildren, diagOS)))
    return 1;

  std::string stubSource;
  if (failed(buildMixedHostStubSource(compiledChildren, stubSource, diagOS)))
    return 1;

  ObjectEmissionToolchain toolchain;
  if (failed(discoverObjectEmissionToolchain(toolchain, diagOS)))
    return 1;

  SmallVector<std::string, 16> tempPaths;
  auto cleanup = llvm::make_scope_exit([&]() { removeTempPaths(tempPaths); });

  SmallVector<BackendArtifact, 4> artifacts;
  if (failed(compileMixedDeviceObjects(compiledChildren, toolchain, artifacts,
                                       tempPaths, diagOS)))
    return 1;

  if (failed(linkMixedFatobj(artifacts, stubSource, outputPath, toolchain,
                             tempPaths, diagOS)))
    return 1;
  return 0;
}

static int emitVPTOBackendResult(ModuleOp module,
                                 llvm::ToolOutputFile &outputFile) {
  if (emitVPTO) {
    module.print(outputFile.os());
    outputFile.os() << "\n";
    outputFile.keep();
    return 0;
  }

  pto::VPTOEmissionOptions options = buildVPTOEmissionOptions();
  std::string stubSource;
  if (failed(pto::emitVPTOHostStubSource(module, stubSource, llvm::errs()))) {
    llvm::errs() << "Error: Failed to emit VPTO host stub source.\n";
    return 1;
  }

  pto::EmittedLLVMModule cubeModule;
  pto::EmittedLLVMModule vectorModule;
  if (failed(
          pto::lowerVPTOModuleToLLVMModules(module, options, cubeModule,
                                            vectorModule, llvm::errs()))) {
    llvm::errs() << "Error: Failed to lower VPTO to LLVM modules.\n";
    return 1;
  }

  if (failed(pto::emitVPTOFatobjFromLLVMModules(
          cubeModule.module.get(), vectorModule.module.get(), stubSource,
          outputFile, llvm::errs()))) {
    llvm::errs() << "Error: Failed to emit VPTO fatobj.\n";
    return 1;
  }
  outputFile.keep();
  return 0;
}

static LogicalResult runVPTOBackendPipeline(ModuleOp module, int argc,
                                            char **argv,
                                            bool hasTileOpsToExpand,
                                            bool hasTilelangHelpers) {
  PassManager pm(module.getContext());
  pm.enableVerifier();
  pm.addPass(pto::createVPTOSplitCVModulePass());
  pm.addPass(pto::createVPTONormalizeContainerPass());
  if (!hasTileOpsToExpand && hasTilelangHelpers)
    inlineTilelangHelpersOnVPTOInput(pm);
  if (hasTileOpsToExpand)
    lowerPTOToVPTOBackend(pm, argc, argv);
  prepareVPTOForEmission(pm);
  if (failed(applyConfiguredPassManagerCLOptions(
          pm, "VPTO unified emission pipeline")))
    return failure();
  if (failed(pm.run(module))) {
    llvm::errs() << "Error: VPTO emission pipeline failed.\n";
    return failure();
  }
  if (failed(applyVPTOPublicABIExportNames(module)))
    return failure();
  return success();
}

int mlir::pto::runPTOASDriver(int argc, char **argv) {
  DialectRegistry registry;
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
  pto::registerBufferizableOpInterfaceExternalModels(registry);

  registry.insert<emitc::EmitCDialect>();
  registry.insert<mlir::LLVM::LLVMDialect>();
  mlir::registerAllPasses();
  ::registerPTOPasses();
  mlir::pto::registerPTOViewToMemrefPass();
  ::registerPTOInlineLibCall();
  ::registerFoldTileBufIntrinsics();
  ::registerExpandTileOp();
  mlir::registerPassManagerCLOptions();

  llvm::cl::SetVersionPrinter(printPTOASVersion);

  bool cliArchSpecified = false;
  for (int i = 1; i < argc; ++i) {
    llvm::StringRef arg(argv[i]);
    if (arg == "--pto-arch" || arg.starts_with("--pto-arch="))
      cliArchSpecified = true;
  }
  bool cliBackendSpecified = hasCLIOption(argc, argv, "--pto-backend");

  // Parse command line options
  llvm::cl::ParseCommandLineOptions(argc, argv, "PTO Assembler (ptoas)\n");

  PTOBackend effectiveBackend = PTOBackend::EmitC;
  if (!parseBackend(ptoBackend, effectiveBackend)) {
    llvm::errs() << "Error: invalid --pto-backend='" << ptoBackend
                 << "'. Expected 'emitc' or 'vpto'.\n";
    return 1;
  }

  // Read whole input first (so we can auto-detect .ptobc by magic).
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

  context.getOrLoadDialect<emitc::EmitCDialect>();
  context.getOrLoadDialect<mlir::pto::PTODialect>();
  context.getOrLoadDialect<func::FuncDialect>();
  context.getOrLoadDialect<arith::ArithDialect>();
  context.getOrLoadDialect<math::MathDialect>();
  context.getOrLoadDialect<memref::MemRefDialect>();
  context.getOrLoadDialect<affine::AffineDialect>();
  context.getOrLoadDialect<mlir::LLVM::LLVMDialect>();

  OwningOpRef<ModuleOp> module;
  llvm::StringRef buf = (*fileOrErr)->getBuffer();
  const bool isPTOBC = (buf.size() >= 6 && std::memcmp(buf.data(), "PTOBC\0", 6) == 0);

  auto normalizeArch = [](llvm::StringRef archValue) {
    std::string normalized = archValue.str();
    for (char &c : normalized)
      c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return normalized;
  };
  auto detectTextualModuleArch = [&](llvm::StringRef text) -> std::optional<std::string> {
    llvm::SmallVector<llvm::StringRef, 4> matches;
    llvm::Regex archRegex(
        R"ptoarch("?(pto\.target_arch)"?[[:space:]]*=[[:space:]]*"([[:alpha:][:digit:]_]+)")ptoarch");
    if (!archRegex.match(text, &matches) || matches.size() < 3)
      return std::nullopt;
    return normalizeArch(matches[2]);
  };

  std::string arch = normalizeArch(ptoTargetArch);
  if (cliArchSpecified) {
    if (arch != "a3" && arch != "a5") {
      llvm::errs() << "Error: invalid --pto-arch='" << ptoTargetArch
                   << "'. Expected 'a3' or 'a5'.\n";
      return 1;
    }
  } else if (!isPTOBC) {
    if (auto detectedArch = detectTextualModuleArch(buf))
      arch = *detectedArch;
  }
  if (arch != "a3" && arch != "a5")
    arch = "a3";

  if (isPTOBC) {
    // Decode PTO bytecode directly into an MLIR module.
    llvm::ArrayRef<uint8_t> bytes(reinterpret_cast<const uint8_t *>(buf.data()), buf.size());
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
    // Parse textual MLIR (.pto).
    llvm::SourceMgr sourceMgr;
    sourceMgr.AddNewSourceBuffer(std::move(*fileOrErr), llvm::SMLoc());
    pto::ScopedPTOParserTargetArch scopedParserArch(
        &context, arch == "a5" ? pto::PTOParserTargetArch::A5
                               : pto::PTOParserTargetArch::A3);
    module = parseSourceFile<ModuleOp>(sourceMgr, &context);
    if (!module) {
      llvm::errs() << "Error: Failed to parse MLIR.\n";
      return 1;
    }
  }

  // If the CLI explicitly requested an arch, it overrides the input module.
  // Otherwise, preserve the textual module's arch when present and only fall
  // back to the effective default.
  if (cliArchSpecified || !module->getOperation()->hasAttr("pto.target_arch")) {
    module->getOperation()->setAttr("pto.target_arch",
                                    mlir::StringAttr::get(&context, arch));
  }

  CompilationPlan compilationPlan;
  if (failed(buildCompilationPlan(module.get(), cliBackendSpecified,
                                  effectiveBackend, compilationPlan)))
    return 1;
  effectiveBackend = compilationPlan.effectiveBackend;

  if (effectiveBackend != PTOBackend::VPTO &&
      compilationPlan.mode != CompilationMode::MixedFatobj &&
      (emitVPTO || ptoPrintSeamIR || !ptoSeamIRFile.empty())) {
    llvm::errs() << "Error: VPTO-specific flags require "
                    "--pto-backend=vpto or pto.backend = \"vpto\".\n";
    return 1;
  }

  PTOBuildLevel effectiveLevel = defaultBuildLevel();
  if (!parseBuildLevel(ptoBuildLevel, effectiveLevel)) {
    llvm::errs() << "Error: invalid --pto-level='" << ptoBuildLevel
                 << "'. Expected 'level1', 'level2', or 'level3'.\n";
    return 1;
  }

  bool invalidAutoSyncTailHint = false;
  module->walk([&](mlir::func::FuncOp func) {
    auto hintAttr =
        func->getAttrOfType<mlir::StringAttr>("pto.auto_sync_tail_hint");
    if (!hintAttr)
      return;

    std::string normalizedHint;
    if (!parseAutoSyncTailHint(hintAttr.getValue(), normalizedHint)) {
      func.emitError("invalid pto.auto_sync_tail_hint '")
          << hintAttr.getValue()
          << "'. Expected 'barrier-all' (or 'default') or "
             "'mte3-to-s-event0'.";
      invalidAutoSyncTailHint = true;
      return;
    }
    func->setAttr("pto.auto_sync_tail_hint",
                  mlir::StringAttr::get(&context, normalizedHint));
  });
  if (invalidAutoSyncTailHint)
    return 1;

  bool hasTAssign = false;
  module->walk([&](pto::TAssignOp) { hasTAssign = true; });

  if (hasTAssign && effectiveLevel != PTOBuildLevel::Level3) {
    llvm::errs() << "Error: pto.tassign is only supported when "
                    "--pto-level=level3.\n";
    return 1;
  }

  if (hasTAssign && enableInsertSync) {
    llvm::errs() << "Error: pto.tassign requires --enable-insert-sync to be "
                    "disabled.\n";
    return 1;
  }

  int enabledAutoSyncModes =
      (enableInsertSync ? 1 : 0) + (enableInjectBarrierAllSync ? 1 : 0) +
      (enableGraphSyncSolver ? 1 : 0);
  if (enabledAutoSyncModes > 1) {
    llvm::errs() << "Error: --enable-insert-sync, "
                    "--enable-inject-barrier-all-sync, and "
                    "--enable-graph-sync-solver are mutually exclusive.\n";
    return 1;
  }
  if (hasTAssign && enableInjectBarrierAllSync) {
    llvm::errs() << "Error: pto.tassign requires "
                    "--enable-inject-barrier-all-sync to be disabled.\n";
    return 1;
  }
  if (hasTAssign && enableGraphSyncSolver) {
    llvm::errs() << "Error: pto.tassign requires --enable-graph-sync-solver "
                    "to be disabled.\n";
    return 1;
  }

  if (effectiveLevel == PTOBuildLevel::Level3) {
    bool missing = false;
    module->walk([&](pto::AllocTileOp op) {
      if (!op.getAddr()) {
        op.emitError("requires 'addr' operand when --pto-level=level3");
        missing = true;
      }
    });
    if (missing)
      return 1;
  } else {
    bool hasAddr = false;
    module->walk([&](pto::AllocTileOp op) {
      if (op.getAddr()) {
        op.emitError(
            "unexpected 'addr' operand: only supported when --pto-level=level3");
        hasAddr = true;
      }
    });
    if (hasAddr)
      return 1;
  }

  if (compilationPlan.mode == CompilationMode::MixedFatobj)
    return runMixedFatobjPlan(compilationPlan, effectiveLevel, arch, argc, argv,
                              outputFilename, llvm::errs());

  // [Fix] ToolOutputFile Usage
  std::error_code ec;
  llvm::ToolOutputFile outputFile(outputFilename, ec, llvm::sys::fs::OF_None);
  if (ec) {
    llvm::errs() << ec.message() << "\n";
    return 1;
  }

  const bool hasTileOpsToExpand = hasUnexpandedTileOps(*module);
  const bool hasTilelangHelpers = hasTilelangInlineHelpers(*module);

  if (compilationPlan.mode == CompilationMode::VPTO && !hasTileOpsToExpand) {
    if (ptoPrintSeamIR || !ptoSeamIRFile.empty()) {
      llvm::errs() << "Error: shared pre-backend seam IR is unavailable when "
                      "skipping the shared PTO-to-VPTO lowering pipeline.\n";
      return 1;
    }
    if (failed(runVPTOBackendPipeline(module.get(), argc, argv, hasTileOpsToExpand,
                                      hasTilelangHelpers)))
      return 1;
    return emitVPTOBackendResult(module.get(), outputFile);
  }

  // Main PassManager
  PassManager pm(&context);

  if (failed(configureSharedPTOPipeline(pm, effectiveLevel)))
    return 1;

  llvm::raw_ostream *outputOS = &outputFile.os();

  if (emitMlirIR) {
    if (failed(pm.run(*module))) {
      llvm::errs() << "Error: Pass execution failed.\n";
      return 1;
    }
    module->print(*outputOS);
    outputFile.keep();
    return 0;
  }

  addSharedPTOSeamPasses(pm);
  if (failed(applyConfiguredPassManagerCLOptions(pm, "main PTOAS pipeline")))
    return 1;

  module->getOperation()->setAttr("pto.target_arch",
                                  mlir::StringAttr::get(&context, arch));

  if (compilationPlan.mode == CompilationMode::VPTO) {
    if (failed(pm.run(*module))) {
      llvm::errs() << "Error: Pass execution failed.\n";
      return 1;
    }

    if (ptoPrintSeamIR) {
      module->print(llvm::errs());
      llvm::errs() << "\n";
    }
    if (failed(emitSharedPreBackendSeamIR(*module, ptoSeamIRFile)))
      return 1;

    if (failed(runVPTOBackendPipeline(module.get(), argc, argv, hasTileOpsToExpand,
                                      hasTilelangHelpers)))
      return 1;
    return emitVPTOBackendResult(module.get(), outputFile);
  }

  if (arch == "a3") {
    pm.addPass(pto::createEmitPTOManualPass(pto::PTOArch::A3));
  } else {
    pm.addPass(pto::createEmitPTOManualPass(pto::PTOArch::A5));
  }
  pm.addPass(emitc::createFormExpressionsPass());
  pm.addPass(mlir::createCSEPass());

  if (failed(pm.run(*module))) {
    llvm::errs() << "Error: Pass execution failed.\n";
    return 1;
  }

  std::string cppOutput;
  if (failed(emitEmitCSourceArtifact(module.get(), emitAddPtrTrace, cppOutput,
                                     llvm::errs())))
    return 1;

  *outputOS << cppOutput;
  outputOS->flush();

  outputFile.keep(); // Success, keep the file

  return 0;
}
