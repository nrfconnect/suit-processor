#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
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

# Define bootstrap_envelope library
add_library(bootstrap_envelope)
target_sources(bootstrap_envelope PRIVATE
  src/bootstrap_envelope.c
  src/bootstrap_seq.c
  )
target_include_directories(bootstrap_envelope PUBLIC
  include
  )
target_link_libraries(bootstrap_envelope PRIVATE zephyr_interface)
target_link_libraries(bootstrap_envelope PRIVATE suit)
