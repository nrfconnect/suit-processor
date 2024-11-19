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

function(zcbor_generate_library name)
  cmake_parse_arguments(arg "DECODE;ENCODE" "" "CDDL_FILES;ENTRY_TYPES" ${ARGN})

  # Setup zcbor command arguments
  list(TRANSFORM arg_CDDL_FILES PREPEND "--cddl;" OUTPUT_VARIABLE zcbor_code_args)
  if(arg_DECODE)
    list(APPEND zcbor_code_args --decode)
  endif()
  if(arg_ENCODE)
    list(APPEND zcbor_code_args --encode)
  endif()
  list(APPEND zcbor_code_args
    --entry-types ${arg_ENTRY_TYPES}
    --output-cmake ${name}.cmake
    --copy-sources
  )
  execute_process(
    COMMAND ${ZCBOR_COMMAND} code ${zcbor_code_args}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND_ERROR_IS_FATAL ANY
  )
  include(${CMAKE_CURRENT_BINARY_DIR}/${name}.cmake)

  # Track changes to input cddl files
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${arg_CDDL_FILES})
endfunction()

# Generate and add COSE parser code
zcbor_generate_library(cose
  CDDL_FILES
    ${CMAKE_CURRENT_LIST_DIR}/cddl/cose_sign.cddl
    ${CMAKE_CURRENT_LIST_DIR}/cddl/cose_encrypt.cddl
  DECODE ENCODE
  ENTRY_TYPES
    COSE_Sign1_Tagged Sig_structure1
    COSE_Encrypt_Tagged Enc_structure
)

# Generate and add SUIT envelope parser code
zcbor_generate_library(manifest
  CDDL_FILES
    ${CMAKE_CURRENT_LIST_DIR}/cddl/manifest.cddl
    ${CMAKE_CURRENT_LIST_DIR}/cddl/trust_domains.cddl
    ${CMAKE_CURRENT_LIST_DIR}/cddl/update_management.cddl
    ${CMAKE_CURRENT_LIST_DIR}/cddl/firmware_encryption.cddl
  DECODE
  ENTRY_TYPES
    SUIT_Envelope_Tagged SUIT_Manifest SUIT_Shared_Sequence SUIT_Command_Sequence
    SUIT_Condition SUIT_Directive SUIT_Shared_Commands SUIT_Text_Map SUIT_Digest
    SUIT_Condition_Version_Comparison_Value SUIT_Parameter_Version_Match
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
