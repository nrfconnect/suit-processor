#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(integration_test_fetch_integrated_payload)
include(../../cmake/test_template.cmake)

# Include basic platform implementation
add_subdirectory("../platform/psa_crypto" "${PROJECT_BINARY_DIR}/suit_plat")
target_link_libraries(suit_plat_psa_crypto PRIVATE zephyr_interface)
# Link platform to the suit library to keep the correct linking order
target_link_libraries(suit PUBLIC suit_plat_psa_crypto)

target_include_directories(app PRIVATE include)

target_link_libraries(app PRIVATE zephyr_interface)
