# PTODSL Tests

`ptodsl/tests/` is the canonical home for PTODSL-specific regression checks.

This directory intentionally keeps three PTODSL testing layers close together:

- `ptodsl_*.py`: compile-only / diagnostics / frontend-handoff regressions for the public PTODSL surface
- `ptodsl_docs_as_test.py`: docs-as-test coverage for `ptodsl/docs/user_guide/`
- `test_*.py`: focused Python unit tests for PTODSL surface helpers and namespace contracts

Related PTODSL validation still lives nearby, but with different roles:

- `ptodsl/examples/`: launchable example programs; these stay user-facing and are validated by regressions here
- `test/dsl-st/`: simulator / ST cases for PTODSL kernels that need runtime execution rather than compile-only checks

Typical local runs:

```bash
cd $PTOAS_REPO_ROOT
python3 ptodsl/tests/ptodsl_jit_compile.py
python3 ptodsl/tests/ptodsl_docs_as_test.py
python3 -m unittest discover -s ptodsl/tests -p 'test_*.py'
```
