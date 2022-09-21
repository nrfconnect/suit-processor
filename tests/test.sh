#!/bin/bash
#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

if [ -z "${ZEPHYR_BASE}" ]; then
       echo "Please set the ZEPHYR_BASE variable and rerun this script."
       exit 1
fi

$ZEPHYR_BASE/scripts/twister -M -v -T . -W --platform native_posix --platform native_posix_64 $*
