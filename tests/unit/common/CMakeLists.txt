#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Define complex arg library
add_library(complex_arg)
target_sources(complex_arg PRIVATE
  src/complex_arg.c
  )
target_include_directories(complex_arg PUBLIC
  include
  )
target_link_libraries(complex_arg PRIVATE zephyr_interface)
