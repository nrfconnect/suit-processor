#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

set(ZCBOR_COMMAND zcbor)
set(ZCBOR_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Generate and add COSE parser code
set(ZCBOR_COMMAND_COSE
  ${ZCBOR_COMMAND}
    -c "${CMAKE_CURRENT_LIST_DIR}/cddl/cose_sign.cddl"
    code -d -e
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

# Create cmake taret to track changes in the input cddl file
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
    -c "${CMAKE_CURRENT_LIST_DIR}/cddl/manifest.cddl"
    code -d
    -t SUIT_Envelope_Tagged SUIT_Manifest SUIT_Common_Sequence SUIT_Command_Sequence
       SUIT_Condition SUIT_Directive SUIT_Common_Commands
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

# Create cmake taret to track changes in the input cddl file
add_custom_command(
  OUTPUT "${ZCBOR_DIR}/manifest.cmake"
  DEPENDS "${CMAKE_CURRENT_LIST_DIR}/cddl/manifest.cddl"
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
  src/suit_command_seq.c
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
