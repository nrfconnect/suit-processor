#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20.0)

if (CONFIG_SOC_NRF5420)
# Define an additional initialization sequence for nrf5420
zephyr_sources(
  src/nrf5420_init.c
  )
endif()
