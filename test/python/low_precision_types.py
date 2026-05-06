#!/usr/bin/env python3
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

from mlir.ir import Context
from mlir.dialects import pto
from mlir.dialects.pto import F4E1M2x2Type, F4E2M1x2Type, HiF8Type


def expect_equal(actual: str, expected: str, label: str) -> None:
    if actual != expected:
        raise AssertionError(
            f"{label} mismatch\nexpected: {expected}\nactual:   {actual}"
        )


def check_type_binding(type_cls, expected_text: str, ctx: Context) -> None:
    module_cls = getattr(pto, type_cls.__name__)
    if module_cls is not type_cls:
        raise AssertionError(
            f"{type_cls.__name__} is not re-exported from mlir.dialects.pto"
        )

    ty = type_cls.get(ctx)
    expect_equal(str(ty), expected_text, f"{type_cls.__name__} print")

    ptr_ty = pto.PtrType.get(ty, context=ctx)
    expect_equal(
        str(ptr_ty),
        f"!pto.ptr<{expected_text}, gm>",
        f"{type_cls.__name__} pointer print",
    )
    expect_equal(
        str(ptr_ty.element_type),
        expected_text,
        f"{type_cls.__name__} pointer element print",
    )


def main() -> None:
    with Context() as ctx:
        pto.register_dialect(ctx)

        check_type_binding(HiF8Type, "!pto.hif8", ctx)
        check_type_binding(F4E1M2x2Type, "!pto.f4E1M2x2", ctx)
        check_type_binding(F4E2M1x2Type, "!pto.f4E2M1x2", ctx)

    print("low_precision_types: PASS")


if __name__ == "__main__":
    main()
