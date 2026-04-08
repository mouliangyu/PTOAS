#!/usr/bin/env python3
# case: micro-op/compare-select/vcmps-unordered-f32
# family: compare-select
# target_ops: pto.vcmps
# scenarios: core-f32, full-mask, scalar-operand, exceptional-values
# NOTE: blocked placeholder case. The current PTO surface and docs only expose
# eq/ne/lt/le/gt/ge compare modes for pto.vcmps, so a true unordered compare
# case cannot be expressed yet. This compare intentionally fails with an
# explicit blocked message instead of pretending there is a precision mismatch.

import sys

BLOCKED_REASON = (
    "blocked placeholder: unordered compare is not part of the current "
    "pto.vcmps surface; docs/isa/11-compare-select.md only defines "
    "eq/ne/lt/le/gt/ge"
)


def main():
    print(f"[BLOCKED] {BLOCKED_REASON}")
    sys.exit(3)


if __name__ == "__main__":
    main()
