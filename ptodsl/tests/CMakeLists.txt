# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

add_ptodsl_python_test(ptodsl_docs_as_test "ptodsl/tests/ptodsl_docs_as_test.py")
add_ptodsl_python_test(ptodsl_flash_attention_demo_compile "ptodsl/tests/ptodsl_flash_attention_demo_compile.py")
add_ptodsl_python_test(ptodsl_jit_compile "ptodsl/tests/ptodsl_jit_compile.py")
add_ptodsl_python_test(ptodsl_jit_diagnostics "ptodsl/tests/ptodsl_jit_diagnostics.py")
add_ptodsl_python_test(ptodsl_ptoas_frontend_verify "ptodsl/tests/ptodsl_ptoas_frontend_verify.py")
add_ptodsl_python_test(ptodsl_subkernel_diagnostics "ptodsl/tests/ptodsl_subkernel_diagnostics.py")
add_ptodsl_python_test(ptodsl_vector_cube_ops "ptodsl/tests/test_vector_cube_ops.py")
