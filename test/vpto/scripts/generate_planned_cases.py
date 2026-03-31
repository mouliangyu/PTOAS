#!/usr/bin/env python3

from __future__ import annotations

import re
import shutil
from pathlib import Path


ROOT = Path(__file__).resolve().parents[3]
CASES_ROOT = ROOT / "test/vpto/cases"
MATRIX = (
    ROOT
    / ".planning/phases/03-hivm-emission/03-vpto-op-board-unit-tests-matrix.md"
)


TEMPLATE_MAP = {
    "binary-vector": "micro-op/binary-vector/vsub-tail",
    "vec-scalar": "micro-op/vec-scalar/vmuls-tail",
    "unary-vector": "micro-op/unary-vector/vabs",
    "compare-select": "micro-op/compare-select/vcmp-eq",
    "conversion": "micro-op/conversion/vtrc-f32-rounding",
    "materialization-predicate": "micro-op/materialization-predicate/vdup-scalar",
    "predicate-load-store": "micro-op/compare-select/vcmp-eq",
    "reduction": "micro-op/reduction/vcadd",
    "vector-load-store": "micro-op/unary-vector/vabs",
    "gather-scatter": "micro-op/unary-vector/vabs",
    "rearrangement": "micro-op/unary-vector/vabs",
    "dsa-sfu": "micro-op/dsa-sfu/vlrelu-f32",
    "dsa-sfu / conversion": "micro-op/dsa-sfu/vlrelu-f32",
}


PRIMARY_REPLACE = {
    "micro-op/binary-vector/vsub-tail": "pto.vsub",
    "micro-op/vec-scalar/vmuls-tail": "pto.vmuls",
    "micro-op/unary-vector/vabs": "pto.vabs",
    "micro-op/compare-select/vcmp-eq": "pto.vcmp",
    "micro-op/conversion/vtrc-f32-rounding": "pto.vtrc",
    "micro-op/materialization-predicate/vdup-scalar": "pto.vdup",
    "micro-op/reduction/vcadd": "pto.vcadd",
    "micro-op/dsa-sfu/vlrelu-f32": "pto.vlrelu",
}


REQUIRED = [
    "kernel.pto",
    "stub.cpp",
    "launch.cpp",
    "main.cpp",
    "golden.py",
    "compare.py",
]


def parse_matrix():
    lines = MATRIX.read_text().splitlines()
    rows = []
    in_case_matrix = False
    for idx, line in enumerate(lines):
        if line.strip() == "## Case Matrix":
            in_case_matrix = True
            continue
        if line.startswith("## ") and line.strip() != "## Case Matrix":
            in_case_matrix = False
        if not in_case_matrix or not line.startswith("| `"):
            continue
        parts = [p.strip() for p in line.strip().split("|")[1:-1]]
        case, family, target_ops, scenarios, status, notes = parts[:6]
        rows.append(
            {
                "idx": idx,
                "case": case.strip("`"),
                "family": family,
                "target_ops": [x.strip().strip("`") for x in target_ops.split(",")],
                "scenarios": scenarios.strip("`"),
                "status": status,
                "notes": notes,
            }
        )
    return lines, rows


def add_case_banner(text: str, row: dict) -> str:
    banner = [
        "// -----------------------------------------------------------------------------",
        f"// case: {row['case']}",
        f"// family: {row['family']}",
        f"// target_ops: {', '.join(row['target_ops'])}",
        f"// scenarios: {row['scenarios']}",
        "// NOTE: bulk-generated coverage skeleton. Parser/verifier/lowering failure is",
        "// still a valid test conclusion in the current coverage-first phase.",
        "// -----------------------------------------------------------------------------",
        "",
    ]
    return "\n".join(banner) + text


def add_py_banner(text: str, row: dict) -> str:
    banner = [
        "#!/usr/bin/env python3",
        f"# case: {row['case']}",
        f"# family: {row['family']}",
        f"# target_ops: {', '.join(row['target_ops'])}",
        f"# scenarios: {row['scenarios']}",
        "# NOTE: bulk-generated coverage skeleton.",
        "",
    ]
    body = text
    if body.startswith("#!/usr/bin/env python3\n"):
        body = body[len("#!/usr/bin/env python3\n") :]
    elif body.startswith("#!/usr/bin/python3\n"):
        body = body[len("#!/usr/bin/python3\n") :]
    return "\n".join(banner) + body


def patch_kernel(text: str, row: dict, template: str) -> str:
    out = text
    base_op = PRIMARY_REPLACE.get(template)
    target_ops = row["target_ops"]
    if base_op and target_ops:
        out = out.replace(base_op, target_ops[0])
    if len(target_ops) > 1 and "// target_ops:" not in out:
        out = add_case_banner(out, row)
    else:
        out = add_case_banner(out, row)
    return out


def patch_cpp(text: str, row: dict) -> str:
    return add_case_banner(text, row)


def patch_py(text: str, row: dict) -> str:
    return add_py_banner(text, row)


def ensure_case(row: dict):
    template = TEMPLATE_MAP[row["family"]]
    src = CASES_ROOT / template
    dst = CASES_ROOT / row["case"]
    if not src.is_dir():
        raise FileNotFoundError(f"template missing: {src}")
    if dst.exists():
        shutil.rmtree(dst)
    shutil.copytree(src, dst)
    for name in REQUIRED:
        path = dst / name
        text = path.read_text()
        if name == "kernel.pto":
            text = patch_kernel(text, row, template)
        elif name.endswith(".cpp"):
            text = patch_cpp(text, row)
        else:
            text = patch_py(text, row)
        path.write_text(text)
        if name.endswith(".py"):
            path.chmod(0o755)


def update_matrix(lines: list[str], rows: list[dict]) -> list[str]:
    updated = list(lines)
    for row in rows:
        if row["status"] != "planned":
            continue
        parts = [p.strip() for p in updated[row["idx"]].strip().split("|")[1:-1]]
        parts[4] = "implemented"
        parts[5] = (
            f"static case added under `test/vpto/cases/{row['case']}`; "
            "board closure pending"
        )
        updated[row["idx"]] = "| " + " | ".join(parts) + " |"
    return updated


def count_status(text: str, status: str) -> int:
    return len(
        re.findall(rf"^\| `[^`]+` \| .*? \| {re.escape(status)} \|", text, re.M)
    )


def main():
    lines, rows = parse_matrix()
    planned = [row for row in rows if row["status"] == "planned"]
    for row in planned:
        ensure_case(row)
    updated = update_matrix(lines, rows)
    MATRIX.write_text("\n".join(updated) + "\n")
    print(f"generated_cases={len(planned)}")
    print(f"planned_left={count_status(MATRIX.read_text(), 'planned')}")


if __name__ == "__main__":
    main()
