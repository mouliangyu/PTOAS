# Module Backend Driver and Object Emission

## Purpose

This design adds a module-level backend selector and reorganizes `ptoas` around
an explicit driver layer. The driver owns command-line parsing, input loading,
PTO compilation planning, backend dispatch, Bisheng compilation, and final
fatobj linking.

The design also replaces backend-specific object/fatobj emitters with one
`ObjectEmission` module. `ObjectEmission` provides both high-level helpers and
fine-grained stage APIs for compiling C++ or VPTO LLVM artifacts into device
objects and packing them into a fatobj.

## User Contract

### `pto.backend` Module Attribute

Use the `pto.backend` attribute on `module`:

```mlir
module attributes {pto.backend = "emitc"} {
  ...
}

module attributes {pto.backend = "vpto"} {
  ...
}
```

Valid values are `emitc` and `vpto`. Unknown values are invalid.

The attribute is intentionally module-level. Function-level backend selection is
not part of this contract; a function uses the backend of its nearest enclosing
backend module.

### Backend Selection Priority

`--pto-backend` remains the strongest selector.

1. If the user passes `--pto-backend=emitc` or `--pto-backend=vpto`, the command
   line forces that backend and `pto.backend` attributes are not used for backend
   selection.
2. If the user does not pass `--pto-backend`, PTOAS reads `pto.backend` attributes.
3. If neither the command line nor the input specifies a backend, PTOAS keeps
   the existing default: `emitc`.

The driver must distinguish "the user did not pass `--pto-backend`" from "the
user passed `--pto-backend=emitc`"; the current option default alone is not
enough.

### Single-Backend Input

For a single module:

```mlir
module attributes {pto.target_arch = "a5", pto.backend = "vpto"} {
  func.func @kernel(...) attributes {pto.aicore} {
    ...
  }
}
```

If `--pto-backend` is absent, this is equivalent to:

```bash
ptoas --pto-backend=vpto input.pto -o kernel.o
```

For `pto.backend = "emitc"`, PTOAS still uses the existing EmitC lowering to
produce CCE C++ source, but compilation of that source is internal to PTOAS.
The final output is a fatobj rather than externally compiled C++.

### Mixed-Backend Container

A mixed-backend input is an outer module containing backend-selected child
modules:

```mlir
module attributes {pto.target_arch = "a5"} {
  module attributes {pto.backend = "emitc"} {
    func.func public @emitc_kernel(...) attributes {pto.aicore} {
      ...
    }
  }

  module attributes {
    pto.backend = "vpto",
    pto.kernel_kind = #pto.kernel_kind<vector>
  } {
    func.func public @mixed_kernel(...) {
      ...
    }
    func.func @mixed_kernel_device(...) attributes {pto.aicore} {
      ...
    }
  }

  module attributes {
    pto.backend = "vpto",
    pto.kernel_kind = #pto.kernel_kind<cube>
  } {
    func.func public @mixed_kernel(...) {
      ...
    }
    func.func @mixed_kernel_device(...) attributes {pto.aicore} {
      ...
    }
  }
}
```

The outer module carries shared attributes such as `pto.target_arch`. Each child
module carries exactly one backend. Child modules may also carry backend-specific
attributes such as `pto.kernel_kind`.

If child modules use more than one backend and `--pto-backend` is absent, PTOAS
enters mixed fatobj mode.

If child modules use more than one backend and `--pto-backend` is present, the
command line wins. The input is treated as a forced single-backend compilation;
unsupported combinations should fail with a direct diagnostic rather than
silently compiling only part of the input.

## Export and Symbol Contract

For non-`pto.aicore` functions, PTOAS follows the normal `func.func` shape:

| `func.func` form | Meaning in a backend child module |
|------------------|-----------------------------------|
| public function with a body | exported definition |
| private function with a body | module-local helper definition |
| private function without a body | external import declaration |

A function without a body does not export a symbol from the current child
module. It declares a symbol that must be resolved by another backend child
module or by a later link input. A function with a body defines the symbol in
the current child module; it is exported only when it is public.

For `pto.aicore` functions, symbol visibility is ignored by this export
contract. A `pto.aicore` function may be public or private in MLIR symbol terms,
but that visibility does not decide whether it is exported.

Users write source-level function names without the `.vector` or `.cube` ABI
suffix. PTOAS derives the exported ABI name for every public non-`pto.aicore`
definition in a VPTO child module by appending the suffix that matches the
module's `pto.kernel_kind`:

| `pto.kernel_kind` | Source symbol | Generated ABI export symbol |
|-------------------|---------------|-----------------------------|
| `#pto.kernel_kind<vector>` | `@foo` | `@foo.vector` |
| `#pto.kernel_kind<cube>` | `@foo` | `@foo.cube` |

`pto.backend = "emitc"` modules follow the same source-level rule: users do not add
`.vector` or `.cube` in the PTO input. The CCE frontend or PTOAS object path
adds the required suffix for EmitC-generated CCE source.

Users also write `pto.aicore` function names without the `_mix_aiv` or
`_mix_aic` device ABI suffix. PTOAS derives the lowered device ABI name from
the source symbol and the target unit:

| `pto.kernel_kind` | Source `pto.aicore` symbol | Generated device ABI symbol |
|-------------------|----------------------------|-----------------------------|
| `#pto.kernel_kind<vector>` | `@foo_device` | `@foo_device_mix_aiv` |
| `#pto.kernel_kind<cube>` | `@foo_device` | `@foo_device_mix_aic` |

For VPTO modules, PTOAS therefore tracks two names for an exported entry:

1. the public non-`pto.aicore` source function name written in the input, such
   as `@foo`
2. the source `pto.aicore` device function name written in the input, such as
   `@foo_device`
3. the generated public ABI export name, such as `@foo.vector` or `@foo.cube`
4. the generated lowered device ABI name, such as `@foo_device_mix_aiv` or
   `@foo_device_mix_aic`

Any aliasing or export metadata needed to connect the public `.vector` /
`.cube` export name to the lowered device symbol is an implementation detail of
the fatobj path.

### Mixed-Backend External Call Case

Cross-backend calls are represented as normal external symbol references inside
the caller module. The callee must be provided by exactly one exported
non-`pto.aicore` function in another backend child module.

```mlir
module attributes {pto.target_arch = "a5"} {
  module attributes {pto.backend = "emitc"} {
    func.func private @vpto_post(
      %src: !pto.ptr<f32, gm>,
      %dst: !pto.ptr<f32, gm>,
      %n: index)

    func.func public @emitc_entry(
      %src: !pto.ptr<f32, gm>,
      %dst: !pto.ptr<f32, gm>,
      %n: index) attributes {pto.aicore} {
      func.call @vpto_post(%src, %dst, %n)
        : (!pto.ptr<f32, gm>, !pto.ptr<f32, gm>, index) -> ()
      return
    }
  }

  module attributes {
    pto.backend = "vpto",
    pto.kernel_kind = #pto.kernel_kind<vector>
  } {
    func.func public @vpto_post(
      %src: !pto.ptr<f32, gm>,
      %dst: !pto.ptr<f32, gm>,
      %n: index) {
      ...
      return
    }
  }
}
```

In this case, the EmitC child module owns an external import declaration and the
call site for source symbol `@vpto_post`. The VPTO child module owns the public
exported definition for source symbol `@vpto_post`. During plan verification,
the driver resolves this as a cross-backend external call, records that the
callee's generated ABI symbol is `@vpto_post.vector`, and rewrites or forwards
that ABI name to the backend object path. The backend pipelines do not inline
or lower across the child-module boundary.

Fatobj generation must allow unresolved device externals by passing `-dc` to
the fatobj repack step. The reference is resolved when the generated fatobj is
linked with the final host/kernel binary; that downstream Bisheng link must pass
`--cce-fatobj-link`.

The declaration is private because the current upstream `func.func` verifier
rejects body-less public declarations.

## Validation

PTOAS should reject:

1. Any `pto.backend` value other than `emitc` or `vpto`.
2. A mixed-backend container whose child module is missing `pto.backend`.
3. A `vpto` child module missing `pto.kernel_kind`.
4. A source-level public non-`pto.aicore` function in a VPTO child module that
   already uses the reserved `.vector` or `.cube` ABI suffix.
5. A generated public ABI export name whose suffix does not match the VPTO
   child module's `pto.kernel_kind`.
6. A source-level `pto.aicore` function in a VPTO child module that already
   uses the reserved `_mix_aiv` or `_mix_aic` device ABI suffix.
7. A generated lowered device ABI name whose suffix does not match the VPTO
   child module's `pto.kernel_kind`.
8. A cross-backend external call whose callee has no matching public
   non-`pto.aicore` definition in another backend child module.
9. A cross-backend external call whose callee matches multiple exports or whose
   signature does not match the exported definition.
10. A body-less external declaration that is not private or is marked
    `pto.aicore`.

## Architecture

```text
ptoas main
  └─ PTOASDriver::run(argc, argv)
       ├─ parse command line
       ├─ load input
       ├─ setup MLIR context
       ├─ parse .pto or decode .ptobc
       ├─ buildPlan()
       ├─ verifyPlan()
       ├─ compilePTO()
       │    ├─ emitc child ── PTO pipeline ── CCE source
       │    ├─ vpto vector ─ PTO pipeline ── VPTO LLVM
       │    └─ vpto cube ─── PTO pipeline ── VPTO LLVM
       ├─ compileDeviceObjects() ── calls ObjectEmission
       │    ├─ cpp source -> dav-c310-vec / dav-c310-cube objects
       │    └─ VPTO LLVM  -> dav-c310-vec / dav-c310-cube objects
       ├─ linkFatobj() ── calls ObjectEmission fatobj stages
       └─ write -o
```

Pass pipelines stop at compiler artifacts:

| Backend path | Pipeline output |
|--------------|-----------------|
| EmitC | CCE C++ source |
| VPTO | LLVM module |

`ObjectEmission` is not a pass and is not appended to any pass pipeline. It is a
driver-called service layer. The driver chooses which `ObjectEmission` stage
APIs to call after the relevant pipeline has returned its artifact.

## `PTOASDriver`

`ptoas` enters the driver layer immediately. Command-line parsing, input
loading, MLIR context setup, textual `.pto` parsing, and `.ptobc` decoding are
all driver responsibilities. Single-backend EmitC, single-backend VPTO, and
mixed-backend fatobj all go through this driver layer; backend-specific
shortcuts should not bypass it.

The driver should be implemented as a separate component, for example
`PTOASDriver.{h,cpp}`, rather than growing `tools/ptoas/ptoas.cpp` further.
`ptoas.cpp` should become a thin process entrypoint that registers the version
printer if needed and calls `PTOASDriver::run(argc, argv)`.

### Driver Responsibilities

The driver owns the full `run()` flow:

```text
setupAndPlan()
  -> verifyPlan()
  -> compilePTO()
  -> compileDeviceObjects()
  -> linkFatobj()
  -> writeOutput()
```

The three compile/link stages are the core artifact-production phases, but they
do not replace setup and planning. Command-line parsing, input loading, MLIR
setup, parse/decode, backend resolution, module planning, and plan verification
remain driver work before artifact production starts.

1. `setupAndPlan()`.
   - Parse PTOAS user-facing options from `argc` / `argv`.
   - Track whether `--pto-backend` appeared on the command line while parsing.
   - Load `.pto` text or `.ptobc` bytes.
   - Register and load the dialects needed by PTOAS.
   - Decode `.ptobc` inputs.
   - Parse textual `.pto` inputs with the effective parser target arch.
   - Read `pto.backend` attributes when the CLI does not force a backend.
   - Decide single-backend EmitC, single-backend VPTO, or mixed-backend fatobj
     mode.
   - Preserve shared outer attributes such as `pto.target_arch`.
   - Identify backend child modules.
   - Clone or isolate child modules before backend-specific passes mutate them.

2. `verifyPlan()`.
   - Validate `pto.backend` values and child module shape.
   - Validate VPTO `pto.kernel_kind` requirements.
   - Reject source-level public non-`pto.aicore` names that already use
     reserved `.vector` or `.cube` ABI suffixes.
   - Reject source-level `pto.aicore` names that already use reserved
     `_mix_aiv` or `_mix_aic` device ABI suffixes.
   - Derive and validate generated public ABI export names for VPTO children.
   - Derive and validate generated lowered device ABI names for VPTO children.
   - Collect source symbols and generated ABI export symbols from all backend
     child modules.
   - Resolve external calls across backend child modules by source symbol name
     and signature, then record the generated ABI callee symbol.

3. `compilePTO()`.
   - Shared PTO planning/lowering remains in the driver-controlled flow.
   - EmitC children are compiled to CCE C++ source.
   - VPTO children are compiled to VPTO LLVM modules.
   - The pass pipelines do not invoke Bisheng, object emission, or fatobj
     linking as a tail action.

4. `compileDeviceObjects()`.
   - Call `ObjectEmission` for CCE C++ source -> device objects.
   - Call `ObjectEmission` for VPTO LLVM modules -> device objects.
   - Collect `BackendArtifact` results.

5. `linkFatobj()`.
   - Call `ObjectEmission` for device object merge, host stub compilation, and
     final fatobj repack.
   - Repack the fatobj with `-dc` so cross-fatobj device externals can remain
     relocatable until the final `--cce-fatobj-link` link.

6. `writeOutput()`.
   - In fatobj mode, write the final packed object to `-o`.
   - In debug/IR-print modes, route output according to the existing debug flag
     semantics.

Current implementation status:

- The driver parses `pto.backend`, builds an explicit plan, clones backend child
  modules, propagates shared outer attributes such as `pto.target_arch`, and
  dispatches mixed children into backend-specific compiler-artifact generation.
- `ObjectEmission` owns the CCE/Bisheng object and fatobj stage APIs.
- Mixed fatobj mode requires an explicit `-o` file path and rejects debug IR
  output flags.
- Cross-backend external imports are represented by private body-less
  `func.func` declarations; the driver resolves them to exactly one exported
  definition in another backend child.
- Host stub emission for multiple VPTO children is generated from the compiled
  child modules as one merged stub source; duplicate logical kernel signatures
  must agree.

### Driver Data Model

The driver should build an explicit compilation plan before running backend
work:

```text
DriverConfig
  argc / argv
  input filename
  output filename
  target arch
  build level
  explicit CLI backend, if any
  debug output flags
  toolchain configuration

CompilationPlan
  mode: emitc | vpto | mixed-fatobj
  modules:
    - backend: emitc | vpto
      kernel kind: none | vector | cube
      module op clone
      source exports
      ABI exports
      source device symbols
      device ABI symbols
      external references
```

Each completed backend job returns device artifacts:

```text
BackendArtifact
  backend: emitc | vpto
  kind: vec-object | cube-object | generic-object
  path: temporary object path
  exported ABI symbols
```

The `ObjectEmission` fatobj stages consume `BackendArtifact` object paths. They
should not inspect MLIR modules.

## `ObjectEmission`

`ObjectEmission` owns all Bisheng-facing object and fatobj operations. It
replaces the separate C++ and VPTO fatobj emitter concepts with one module that
supports both high-level emit calls and fine-grained stage calls.

`ObjectEmission` does not decide backend selection and does not run PTO or VPTO
MLIR lowering pipelines. The driver produces C++ source and VPTO LLVM modules,
then requests object/fatobj operations from this component.

### High-Level Device Emit Interfaces

```text
emitCppVectorDeviceObject(cppSource)      -> dav-c310-vec object
emitCppCubeDeviceObject(cppSource)        -> dav-c310-cube object
emitVPTOVectorDeviceObject(llvmModule)    -> dav-c310-vec object
emitVPTOCubeDeviceObject(llvmModule)      -> dav-c310-cube object
```

The C++ source path compiles the same EmitC-generated source for both device
targets when requested:

```text
CCE C++ source
  ├─ Bisheng -xcce --cce-aicore-arch=dav-c310-vec  ── vector device object
  └─ Bisheng -xcce --cce-aicore-arch=dav-c310-cube ── cube device object
```

The VPTO path compiles VPTO LLVM modules for the matching device target:

```text
VPTO vector LLVM ── Bisheng -x ir --cce-aicore-arch=dav-c310-vec  ── vector object
VPTO cube LLVM ─── Bisheng -x ir --cce-aicore-arch=dav-c310-cube ── cube object
```

### Fine-Grained Stage API

`ObjectEmission` should expose stage-level APIs so the driver, tests, and debug
tools can run individual pieces without going through a monolithic fatobj call:

```text
writeCppSource(cppSource) -> path
writeLLVMModule(llvmModule) -> path

compileCppToDeviceObject(cppPath, target: vec|cube) -> object path
compileLLVMToDeviceObject(llPath, target: vec|cube) -> object path

mergeDeviceObjects(objectPaths) -> merged object path
writeHostStubSource(stubSource) -> path
compileHostStubToObject(stubPath, mergedObjectPath, moduleId) -> host object path
repackFatobj(hostObjectPath, moduleId, outputPath) -> output fatobj
```

The high-level emit helpers are thin compositions of these stage APIs. For
example, `emitCppVectorDeviceObject` is `writeCppSource` followed by
`compileCppToDeviceObject(..., vec)`. A full fatobj build is
`mergeDeviceObjects` followed by host stub compilation and repack.

### ObjectEmission Responsibilities

1. Discover and validate the Bisheng/cce-ld/ld.lld toolchain.
2. Own temporary-file creation and cleanup for source, LLVM IR, device objects,
   command stderr, merged object, host stub source, and host stub object.
3. Compile CCE C++ source to vector and/or cube device objects.
4. Compile VPTO LLVM modules to vector and/or cube device objects.
5. Merge an arbitrary list of device objects.
6. Compile the host stub with the merged device object embedded.
7. Repack the final fatobj to `-o` with `-dc`; the final consumer that links
   this fatobj into a host/kernel binary must use `--cce-fatobj-link`.
8. Keep diagnostics separated by stage and artifact kind.

## Implementation Plan

1. Introduce `PTOASDriver`.
   - Move command-line parsing, input loading, MLIR context setup, parse/decode,
     backend selection, module planning, backend dispatch, Bisheng compile/link
     orchestration, and final fatobj output decisions behind the driver.
   - Keep `tools/ptoas/ptoas.cpp` as the thin entrypoint that calls
     `PTOASDriver::run(argc, argv)`.

2. Add `pto.backend` attribute handling.
   - Parse and validate `pto.backend`.
   - Track whether `--pto-backend` was explicitly provided.
   - Resolve single-backend or mixed-backend mode in the driver.

3. Add mixed-container planning.
   - Identify backend child modules.
   - Clone/isolate child modules before mutation.
   - Preserve shared outer attributes such as `pto.target_arch`.
   - Record source-level exported non-`pto.aicore` symbols, generated ABI export
     symbols, source-level `pto.aicore` symbols, and generated device ABI
     symbols.

4. Refactor backend pipeline entrypoints.
   - EmitC pipeline returns CCE C++ source.
   - VPTO pipeline returns LLVM module(s).
   - Object/fatobj emission is not appended as pipeline finalization.

5. Introduce `ObjectEmission`.
   - Move generic toolchain discovery, temporary-file management, object
     compilation, object merge, host stub compilation, and repack operations
     into this module.
   - Provide cpp/vpto vector/cube high-level emit helpers.
   - Provide fine-grained stage APIs.

6. Route final output through the driver.
   - Driver collects `BackendArtifact` results.
   - Driver calls `ObjectEmission` merge/host-stub/repack stages.
   - Driver writes or keeps the final `-o` result.

7. Add focused tests.
   - `pto.backend` attr fallback when `--pto-backend` is absent.
   - CLI backend override when `--pto-backend` is present.
   - Rejection of source-level public non-`pto.aicore` symbols that already use
     reserved `.vector` / `.cube` ABI suffixes.
   - Generation of VPTO public non-`pto.aicore` `.vector` / `.cube` ABI export
     symbols from suffix-free source symbols.
   - Rejection of source-level `pto.aicore` symbols that already use reserved
     `_mix_aiv` / `_mix_aic` device ABI suffixes.
   - Generation of lowered `pto.aicore` `_mix_aiv` / `_mix_aic` device ABI
     symbols from suffix-free source symbols.
   - Driver-planned mixed backend mode.
   - ObjectEmission stage APIs through tests that do not require full pipeline
     execution when possible.
