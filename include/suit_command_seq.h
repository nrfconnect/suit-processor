/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_COMMAND_SEQ_H__
#define SUIT_COMMAND_SEQ_H__

#include "suit_processor.h"


/** Validate that all commands are among the commands allowed for the shared
 *  sequence, and that the commands are supported and the arguments sensible..
 *
 *  Check that it contains a vendor ID check and a class ID check.
 */
int suit_validate_shared_sequence(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str);

/** Validate that all commands are supported, and that arguments are sensible.
 */
int suit_validate_command_sequence(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str);

/** Run all commands in the sequence.
 */
int suit_run_command_sequence(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str);


#endif /* SUIT_COMMAND_SEQ_H__ */
