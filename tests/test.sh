#!/bin/bash
#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

if [ -z "${ZEPHYR_BASE}" ]; then
       echo "Please set the ZEPHYR_BASE variable and rerun this script."
       exit 1
fi

$ZEPHYR_BASE/scripts/twister -M -v -T . -W --platform native_posix --platform native_posix/native/64 $*
