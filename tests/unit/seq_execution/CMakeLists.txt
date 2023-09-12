#
# Copyright (c) 2023 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(unit_test_seq_execution)
include(../../cmake/test_template.cmake)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../common" "${PROJECT_BINARY_DIR}/test_common")

# generate runner for the test
test_runner_generate(src/main.c)

# create mocks for suit_platform functions
cmock_handle(${SUIT_PROCESSOR_DIR}/include/suit_platform.h suit_platform)

target_include_directories(app PRIVATE include)

target_link_libraries(app PRIVATE zephyr_interface)

# Link app with bootstrap_envelope library
target_link_libraries(app PUBLIC bootstrap_envelope)

# Link app with complex arg library
target_link_libraries(app PUBLIC complex_arg)
