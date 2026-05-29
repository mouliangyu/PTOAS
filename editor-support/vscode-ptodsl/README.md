# PTODSL Syntax for VS Code

This extension provides TextMate-based syntax highlighting for PTODSL, the
Python DSL used in this repository.

What it highlights:

- PTODSL decorators such as `@pto.jit`, `@pto.simd`, `@pto.simt`, and `@pto.cube`
- PTODSL control-flow helpers such as `pto.for_`, `pto.if_`, `pto.yield_`, and `pto.vecscope`
- Public PTODSL types, enums, and namespace helpers under `pto.*`
- PTODSL scalar helpers under `scalar.*`
- The grammar also injects into ordinary Python files, so existing `.py`
  PTODSL sources can light up without renaming them first

If you want a dedicated language mode, associate files such as
`*.ptodsl.py` or `*.pto.py` with `PTODSL` in VS Code.

This package is intentionally lightweight: it only adds syntax coloring and
editor configuration, and it does not change PTODSL execution semantics.
