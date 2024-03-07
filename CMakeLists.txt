#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

if((NOT DEFINED ZEPHYR_ZCBOR_MODULE_DIR) OR (NOT EXISTS "${ZEPHYR_ZCBOR_MODULE_DIR}/zcbor/zcbor.py"))
  message(STATUS "Use zcbor command")
  set(ZCBOR_COMMAND zcbor)
else()
  message(STATUS "Use python zcbor.py file")
  set(ZCBOR_COMMAND ${PYTHON_EXECUTABLE} ${ZEPHYR_ZCBOR_MODULE_DIR}/zcbor/zcbor.py)
endif()

set(ZCBOR_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Generate and add COSE parser code
set(ZCBOR_COMMAND_COSE
  ${ZCBOR_COMMAND}
    code
    -c "${CMAKE_CURRENT_LIST_DIR}/cddl/cose_sign.cddl"
    -d -e
    -t COSE_Sign1_Tagged Sig_structure1
    --output-cmake cose.cmake
    --copy-sources
)

if(NOT EXISTS "${ZCBOR_DIR}/cose.cmake")
execute_process(
  COMMAND ${ZCBOR_COMMAND_COSE}
  WORKING_DIRECTORY ${ZCBOR_DIR}
  COMMAND_ERROR_IS_FATAL ANY
)
endif()

# Dirty fix until proper one is added to zcbor - NCSDK-23327
file(READ "${ZCBOR_DIR}/cose.cmake" COSE_CMAKE_TEXT)
string(REPLACE "\\" "/" COSE_CMAKE_TEXT "${COSE_CMAKE_TEXT}")
file(WRITE "${ZCBOR_DIR}/cose.cmake" "${COSE_CMAKE_TEXT}")

# Create cmake target to track changes in the input cddl file
add_custom_command(
  OUTPUT "${ZCBOR_DIR}/cose.cmake"
  DEPENDS "${CMAKE_CURRENT_LIST_DIR}/cddl/cose_sign.cddl"
  COMMAND ${ZCBOR_COMMAND_COSE}
  WORKING_DIRECTORY "${ZCBOR_DIR}"
  )
add_custom_target(zcbor_cose ALL
  DEPENDS "${ZCBOR_DIR}/cose.cmake"
  COMMENT "Generate cose encode/decode sources for parsing CBOR"
  )

include("${ZCBOR_DIR}/cose.cmake")
# Specify the absolute path for ZCBOR-generated code include directory
target_include_directories(cose PUBLIC
    ${ZCBOR_DIR}/include
    )

# Generate and add SUIT envelope parser code
set(ZCBOR_COMMAND_MANIFEST
  ${ZCBOR_COMMAND}
    code
    -c "${CMAKE_CURRENT_LIST_DIR}/cddl/manifest.cddl"
    -c "${CMAKE_CURRENT_LIST_DIR}/cddl/trust_domains.cddl"
    -c "${CMAKE_CURRENT_LIST_DIR}/cddl/update_management.cddl"
    -d
    -t SUIT_Envelope_Tagged SUIT_Manifest SUIT_Shared_Sequence SUIT_Command_Sequence
       SUIT_Condition SUIT_Directive SUIT_Shared_Commands SUIT_Text_Map SUIT_Digest
       SUIT_Condition_Version_Comparison_Value SUIT_Parameter_Version_Match
    --output-cmake manifest.cmake
    --copy-sources
)
if(NOT EXISTS "${ZCBOR_DIR}/manifest.cmake")
execute_process(
  COMMAND ${ZCBOR_COMMAND_MANIFEST}
  WORKING_DIRECTORY ${ZCBOR_DIR}
  COMMAND_ERROR_IS_FATAL ANY
)
endif()

# Dirty fix until proper one is added to zcbor - NCSDK-23327
file(READ "${ZCBOR_DIR}/manifest.cmake" MANIFEST_CMAKE_TEXT)
string(REPLACE "\\" "/" MANIFEST_CMAKE_TEXT "${MANIFEST_CMAKE_TEXT}")
file(WRITE "${ZCBOR_DIR}/manifest.cmake" "${MANIFEST_CMAKE_TEXT}")

# Create cmake target to track changes in the input cddl file
add_custom_command(
  OUTPUT "${ZCBOR_DIR}/manifest.cmake"
  DEPENDS "${CMAKE_CURRENT_LIST_DIR}/cddl/manifest.cddl"
  DEPENDS "${CMAKE_CURRENT_LIST_DIR}/cddl/trust_domains.cddl"
  COMMAND ${ZCBOR_COMMAND_MANIFEST}
  WORKING_DIRECTORY "${ZCBOR_DIR}"
  )
add_custom_target(zcbor_manifest ALL
  DEPENDS "${ZCBOR_DIR}/manifest.cmake"
  COMMENT "Generate manifest encode/decode sources for parsing CBOR"
  )

include("${ZCBOR_DIR}/manifest.cmake")
# Specify the absolute path for ZCBOR-generated code include directory
target_include_directories(manifest PUBLIC
    ${ZCBOR_DIR}/include
    )

# Define SUIT library
add_library(suit)
target_sources(suit PRIVATE
  src/suit_manifest.c
  src/suit_decoder.c
  src/suit_schedule_seq.c
  src/suit_seq_exec.c
  src/suit_condition.c
  src/suit_directive.c
  src/suit.c
  )
target_include_directories(suit PUBLIC
  include
  )

# Link with the auto-generated code
target_link_libraries(suit PUBLIC manifest)
target_link_libraries(suit PUBLIC cose)

# Canonical mode is required to encode the Signature1 structure correctly
add_compile_definitions(ZCBOR_CANONICAL)
