#!/usr/bin/env sh
#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: Apache-2.0
#

zcbor -c cddl/manifest.cddl -c cddl/cose_sign.cddl code -d -t SUIT_Envelope_Tagged SUIT_Manifest SUIT_Common_Sequence SUIT_Command_Sequence SUIT_Condition SUIT_Directive SUIT_Common_Commands --output-cmake manifest.cmake --copy-sources
