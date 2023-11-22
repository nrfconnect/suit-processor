/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_REPORT_H__
#define SUIT_REPORT_H__

#include "suit_types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Construct a report for the given command.
 *
 *  @param[in]  command  The command that was executed.
 *  @param[in]  result  The result of the command.
 *  @param[in]  parameters  The current parameters.
 *  @param[out] report  The constructed report.
 */
int suit_construct_report(unsigned int command, int result,
		struct suit_manifest_params *parameters,
		struct suit_report *report);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUIT_REPORT_H__ */
