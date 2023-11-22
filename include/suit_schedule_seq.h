/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_SCHEDULE_SEQ_H__
#define SUIT_SCHEDULE_SEQ_H__

#include "suit_processor.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Schedule execution of all commands in the sequence from the current manifest.
 *
 * @param[in]  state     The SUIT processor state to use.
 * @param[in]  manifest  Manifest structure that holds the command sequence.
 * @param[in]  seq_name  Name of the command sequence to execute.
 *
 * @returns SUIT_ERR_AGAIN if the sequence execution was successfully scheduled, error code otherwise.
 */
int suit_schedule_execution(struct suit_processor_state *state, struct suit_manifest_state *manifest, enum suit_command_sequence seq_name);

/** Schedule validation of all commands in the sequence from the current manifest.
 *
 * @details Validate that all commands are among the commands allowed for the respective
 *          sequence.
 *
 * @param[in]  state     The SUIT processor state to use.
 * @param[in]  manifest  Manifest structure that holds the command sequence.
 * @param[in]  seq_name  Name of the command sequence to validate.
 *
 * @returns SUIT_ERR_AGAIN if the sequence validation was successfully scheduled, error code otherwise.
 */
int suit_schedule_validation(struct suit_processor_state *state, struct suit_manifest_state *manifest, enum suit_command_sequence seq_name);

/** Process all operations scheduled.
 *
 * @param[in]  state  The SUIT processor state to use.
 *
 * @returns SUIT_ERR_SUCCESS if the sequence was successfully processed, error code otherwise.
 */
int suit_process_scheduled(struct suit_processor_state *state);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUIT_SCHEDULE_SEQ_H__ */
