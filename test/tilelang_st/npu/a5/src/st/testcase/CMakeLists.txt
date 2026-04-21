# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.

# --------------------------------------------------------------------------
# pto_tilelang_vec_st(NAME)
#
# CMake macro for TileLang ST vector test cases.  Unlike pto-isa's pto_vec_st()
# which compiles a hand-written kernel.cpp with -xcce, this macro:
#   1. Runs ptoas to compile <NAME>.pto → kernel.ll  (LLVM IR)
#   2. Runs bisheng -x ir   kernel.ll  → device .o
#   3. Repacks the device .o into a host-linkable fatobj object
#   4. Links the repacked object → shared library
#   5. Builds host executable from main.cpp (no GTest — comparison via compare.py)
# --------------------------------------------------------------------------
set(PTO_TILELANG_ST_TESTCASE_DIR ${CMAKE_CURRENT_LIST_DIR})

function(pto_tilelang_vec_st NAME)
    # Step 1: ptoas  .pto → kernel.ll
    set(PTO_SRC ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.pto)
    set(KERNEL_LL ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_kernel.ll)
    set(PTOAS_CAPTURE_SCRIPT
        ${PTO_TILELANG_ST_TESTCASE_DIR}/run_ptoas_to_file.cmake)
    add_custom_command(
        OUTPUT ${KERNEL_LL}
        COMMAND ${CMAKE_COMMAND}
            -DPTOAS_BIN=${PTOAS_BIN}
            -DPTO_SRC=${PTO_SRC}
            -DKERNEL_LL=${KERNEL_LL}
            -P ${PTOAS_CAPTURE_SCRIPT}
        DEPENDS ${PTO_SRC} ${PTOAS_CAPTURE_SCRIPT}
        COMMENT "ptoas: ${NAME}.pto -> ${NAME}_kernel.ll"
        VERBATIM
    )

    # Step 2: bisheng  kernel.ll → device .o
    set(KERNEL_DEVICE_OBJ ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_kernel_device.o)
    add_custom_command(
        OUTPUT ${KERNEL_DEVICE_OBJ}
        COMMAND bisheng
            --target=hiipu64-hisilicon-cce
            -march=dav-c310-vec
            --cce-aicore-arch=dav-c310-vec
            --cce-aicore-only
            -O2
            -c -x ir ${KERNEL_LL}
            -o ${KERNEL_DEVICE_OBJ}
        DEPENDS ${KERNEL_LL}
        COMMENT "bisheng: ${NAME}_kernel.ll -> ${NAME}_kernel_device.o"
        VERBATIM
    )

    # Step 3: repack device .o into a host-linkable fatobj object using a
    # generated stub source derived from launch.cpp's kernel declarations.
    set(REPACK_SCRIPT ${PTO_TILELANG_ST_TESTCASE_DIR}/repack_tilelang_kernel.sh)
    set(KERNEL_LAUNCH_SRC ${CMAKE_CURRENT_SOURCE_DIR}/launch.cpp)
    set(KERNEL_REPACK_OBJ ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_kernel_repack.o)
    string(MD5 KERNEL_MODULE_ID_HASH "${NAME}")
    string(SUBSTRING "${KERNEL_MODULE_ID_HASH}" 0 16 KERNEL_MODULE_ID)
    add_custom_command(
        OUTPUT ${KERNEL_REPACK_OBJ}
        COMMAND bash ${REPACK_SCRIPT}
            ${ASCEND_HOME_PATH}
            ${PTO_ISA_ROOT}
            dav-c310-vec
            ${KERNEL_LAUNCH_SRC}
            ${KERNEL_DEVICE_OBJ}
            ${KERNEL_REPACK_OBJ}
            ${KERNEL_MODULE_ID}
        DEPENDS ${KERNEL_LAUNCH_SRC} ${KERNEL_DEVICE_OBJ} ${REPACK_SCRIPT}
        COMMENT "repack: ${NAME}_kernel_device.o -> ${NAME}_kernel_repack.o"
        VERBATIM
    )

    # Step 4: launch.cpp (-xcce) + repack object → shared library
    add_library(${NAME}_kernel SHARED launch.cpp ${KERNEL_REPACK_OBJ})
    set_source_files_properties(${KERNEL_REPACK_OBJ}
        PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
    target_compile_options(${NAME}_kernel PRIVATE
        ${CMAKE_CCE_COMPILE_OPTIONS} --cce-aicore-arch=dav-c310-vec -std=c++17)
    target_include_directories(${NAME}_kernel PRIVATE
        ${ASCEND_HOME_PATH}/pkg_inc/
        ${ASCEND_HOME_PATH}/pkg_inc/profiling/
        ${ASCEND_HOME_PATH}/pkg_inc/runtime/runtime
    )
    target_link_options(${NAME}_kernel PRIVATE --cce-fatobj-link)

    # Step 5: main.cpp → host executable
    add_executable(${NAME} main.cpp)
    target_compile_options(${NAME} PRIVATE ${CMAKE_CPP_COMPILE_OPTIONS})
    target_include_directories(${NAME} PRIVATE
        ${PTO_TILELANG_ST_COMMON_DIR}
    )

    target_link_directories(${NAME} PUBLIC
        ${ASCEND_HOME_PATH}/lib64
        ${ASCEND_HOME_PATH}/tools/simulator/${SOC_VERSION}/lib
    )

    target_link_libraries(${NAME} PRIVATE
        ${NAME}_kernel
        $<BUILD_INTERFACE:$<$<STREQUAL:${RUN_MODE},sim>:runtime_camodel>>
        $<BUILD_INTERFACE:$<$<STREQUAL:${RUN_MODE},npu>:runtime>>
        stdc++ ascendcl m tiling_api platform c_sec dl nnopbase pthread
    )
endfunction()

# --------------------------------------------------------------------------
# pto_tilelang_cube_st(NAME)
#
# Same pipeline as pto_tilelang_vec_st, but targets dav-c310-cube for cube
# (MAT / L0A–L0B–L0C) intrinsics in emitted LLVM IR and in bisheng/repack.
# --------------------------------------------------------------------------
function(pto_tilelang_cube_st NAME)
    set(PTO_SRC ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.pto)
    set(KERNEL_LL ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_kernel.ll)
    set(PTOAS_CAPTURE_SCRIPT
        ${PTO_TILELANG_ST_TESTCASE_DIR}/run_ptoas_to_file.cmake)
    add_custom_command(
        OUTPUT ${KERNEL_LL}
        COMMAND ${CMAKE_COMMAND}
            -DPTOAS_BIN=${PTOAS_BIN}
            -DPTO_SRC=${PTO_SRC}
            -DKERNEL_LL=${KERNEL_LL}
            -P ${PTOAS_CAPTURE_SCRIPT}
        DEPENDS ${PTO_SRC} ${PTOAS_CAPTURE_SCRIPT}
        COMMENT "ptoas: ${NAME}.pto -> ${NAME}_kernel.ll"
        VERBATIM
    )

    set(KERNEL_DEVICE_OBJ ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_kernel_device.o)
    add_custom_command(
        OUTPUT ${KERNEL_DEVICE_OBJ}
        COMMAND bisheng
            --target=hiipu64-hisilicon-cce
            -march=dav-c310-cube
            --cce-aicore-arch=dav-c310-cube
            --cce-aicore-only
            -O2
            -c -x ir ${KERNEL_LL}
            -o ${KERNEL_DEVICE_OBJ}
        DEPENDS ${KERNEL_LL}
        COMMENT "bisheng: ${NAME}_kernel.ll -> ${NAME}_kernel_device.o"
        VERBATIM
    )

    set(REPACK_SCRIPT ${PTO_TILELANG_ST_TESTCASE_DIR}/repack_tilelang_kernel.sh)
    set(KERNEL_LAUNCH_SRC ${CMAKE_CURRENT_SOURCE_DIR}/launch.cpp)
    set(KERNEL_REPACK_OBJ ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_kernel_repack.o)
    string(MD5 KERNEL_MODULE_ID_HASH "${NAME}")
    string(SUBSTRING "${KERNEL_MODULE_ID_HASH}" 0 16 KERNEL_MODULE_ID)
    add_custom_command(
        OUTPUT ${KERNEL_REPACK_OBJ}
        COMMAND bash ${REPACK_SCRIPT}
            ${ASCEND_HOME_PATH}
            ${PTO_ISA_ROOT}
            dav-c310-cube
            ${KERNEL_LAUNCH_SRC}
            ${KERNEL_DEVICE_OBJ}
            ${KERNEL_REPACK_OBJ}
            ${KERNEL_MODULE_ID}
        DEPENDS ${KERNEL_LAUNCH_SRC} ${KERNEL_DEVICE_OBJ} ${REPACK_SCRIPT}
        COMMENT "repack: ${NAME}_kernel_device.o -> ${NAME}_kernel_repack.o"
        VERBATIM
    )

    add_library(${NAME}_kernel SHARED launch.cpp ${KERNEL_REPACK_OBJ})
    set_source_files_properties(${KERNEL_REPACK_OBJ}
        PROPERTIES EXTERNAL_OBJECT TRUE GENERATED TRUE)
    target_compile_options(${NAME}_kernel PRIVATE
        ${CMAKE_CCE_COMPILE_OPTIONS} --cce-aicore-arch=dav-c310-cube -std=c++17)
    target_include_directories(${NAME}_kernel PRIVATE
        ${ASCEND_HOME_PATH}/pkg_inc/
        ${ASCEND_HOME_PATH}/pkg_inc/profiling/
        ${ASCEND_HOME_PATH}/pkg_inc/runtime/runtime
    )
    target_link_options(${NAME}_kernel PRIVATE --cce-fatobj-link)

    add_executable(${NAME} main.cpp)
    target_compile_options(${NAME} PRIVATE ${CMAKE_CPP_COMPILE_OPTIONS})
    target_include_directories(${NAME} PRIVATE
        ${PTO_TILELANG_ST_COMMON_DIR}
    )

    target_link_directories(${NAME} PUBLIC
        ${ASCEND_HOME_PATH}/lib64
        ${ASCEND_HOME_PATH}/tools/simulator/${SOC_VERSION}/lib
    )

    target_link_libraries(${NAME} PRIVATE
        ${NAME}_kernel
        $<BUILD_INTERFACE:$<$<STREQUAL:${RUN_MODE},sim>:runtime_camodel>>
        $<BUILD_INTERFACE:$<$<STREQUAL:${RUN_MODE},npu>:runtime>>
        stdc++ ascendcl m tiling_api platform c_sec dl nnopbase pthread
    )
endfunction()

# --------------------------------------------------------------------------
# Test case registry — vector ST vs cube ST
# --------------------------------------------------------------------------
set(ALL_VEC_TESTCASES
    tadd
    tcvt
    tload
)
set(ALL_CUBE_TESTCASES
    tmatmul
)
set(ALL_ST_TESTCASES ${ALL_VEC_TESTCASES} ${ALL_CUBE_TESTCASES})

if(DEFINED TEST_CASE)
    if(NOT (TEST_CASE STREQUAL "all" OR TEST_CASE IN_LIST ALL_ST_TESTCASES))
        message(FATAL_ERROR "not found TEST_CASE: ${TEST_CASE}, supported: all ${ALL_ST_TESTCASES}")
    endif()
endif()

if(DEFINED TEST_CASE)
    message(STATUS "run: ${TEST_CASE}")
else()
    message(STATUS "run: all (default)")
endif()

foreach(TESTCASE ${ALL_VEC_TESTCASES})
    if((DEFINED TEST_CASE AND (TEST_CASE STREQUAL TESTCASE OR TEST_CASE STREQUAL "all")) OR (NOT DEFINED TEST_CASE))
        add_subdirectory(${TESTCASE})
    endif()
endforeach()

foreach(TESTCASE ${ALL_CUBE_TESTCASES})
    if((DEFINED TEST_CASE AND (TEST_CASE STREQUAL TESTCASE OR TEST_CASE STREQUAL "all")) OR (NOT DEFINED TEST_CASE))
        add_subdirectory(${TESTCASE})
    endif()
endforeach()
