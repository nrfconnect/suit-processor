
#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

# Define sample SUIT platform library
add_library(suit_plat_psa_crypto)
target_include_directories(suit_plat_psa_crypto PRIVATE
  include
  )
target_sources(suit_plat_psa_crypto PRIVATE
  src/suit_plat_state.c
  src/suit_plat_crypto_psa.c
  src/suit_plat_properties.c
  src/suit_plat_commands.c
  src/suit_plat_rambuf.c
  )
target_link_libraries(suit_plat_psa_crypto PUBLIC suit)
