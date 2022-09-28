#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE
  ${app_sources}
  )

if (VERBOSE)
  zephyr_compile_definitions(ZCBOR_VERBOSE)
endif()

if (ASSERTS)
  zephyr_compile_definitions(ZCBOR_ASSERTS)
endif()

zephyr_compile_options(-Werror)

if (CONFIG_64BIT)
  set(bit_arg -b 64)
endif()

# Include the SUIT core library and auto-generated libraries
set(SUIT_PROCESSOR_DIR ${CMAKE_CURRENT_LIST_DIR}/../../)
add_subdirectory("${SUIT_PROCESSOR_DIR}" "${PROJECT_BINARY_DIR}/suit")

# Link all libraries with zephyr_interface
target_link_libraries(suit PRIVATE zephyr_interface)
target_link_libraries(manifest PRIVATE zephyr_interface)
target_link_libraries(cose PRIVATE zephyr_interface)

# Link app with the main SUIT library
target_link_libraries(app PRIVATE suit)