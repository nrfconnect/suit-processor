/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUIT_COMMAND_SEQ_H__
#define SUIT_COMMAND_SEQ_H__

#include "suit_types.h"


/** Validate that all commands are among the commands allowed for the common
 *  sequence, and that the commands are supported and the arguments sensible..
 *
 *  Check that it contains a vendor ID check and a class ID check.
 */
int suit_validate_common_sequence(struct zcbor_string *cmd_seq_str);

/** Validate that all commands are supported, and that arguments are sensible.
 */
int suit_validate_command_sequence(struct zcbor_string *cmd_seq_str);

/** Run all the commands in this sequence.
 */
int suit_run_command_sequence(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str);


#endif /* SUIT_COMMAND_SEQ_H__ */
