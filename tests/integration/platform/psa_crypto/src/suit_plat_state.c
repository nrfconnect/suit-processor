/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <suit_platform.h>


/** Check that the provided sequence number is recent enough. */
int suit_plat_check_sequence_num(unsigned int seq_num)
{
	return SUIT_SUCCESS;
}

/** Update the sequence number against which new manifests will be checked. */
int suit_plat_commit_sequence_num(unsigned int seq_num)
{
	return SUIT_SUCCESS;
}

/** File a report on a command result. */
int suit_plat_report(unsigned int rep_policy, struct suit_report *report)
{
	return SUIT_ERR_UNSUPPORTED_COMMAND;
}

/** Store an opaque state struct to non-volatile memory.
 *
 *  The SUIT operation must be able to cope with random resets, both unexpected
 *  resets and resets required by the firmware update procedures.
 */
int suit_plat_set_state(unsigned int state_id, unsigned char *state, size_t state_len)
{
	return SUIT_ERR_CRASH;
}

/** Retrieve an opaque state struct from non-volatile memory. */
int suit_plat_get_state(unsigned int state_id, unsigned char *state, size_t state_len)
{
	return SUIT_ERR_CRASH;
}
