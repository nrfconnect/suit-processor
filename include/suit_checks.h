/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUIT_CHECKS_H__
#define SUIT_CHECKS_H__

#include "suit_types.h"


/** Check that the manifest is correctly signed with a known key.
 *
 *  This requires that the envelope has been decoded.
 */
int suit_check_signature(struct suit_processor_state *state);

/** Check that the signers are authorized to perform all the manifest actions.
 *
 *  This requires that the manfiest has been authenticated and decoded.
 */
int suit_check_authorization(struct suit_processor_state *state);

/** Check that the manifest's sequence number is sufficiently recent.
 *
 *  This requires that the manfiest has been authenticated and decoded.
 */
int suit_check_sequence_num(struct suit_processor_state *state);

/** Check the validity of the manifest's component list.
 *
 *  Check that all components exist on the system, and are valid.
 *  Check that all interdependent components are updated (if applicable).
 *
 *  This requires that the manfiest has been authenticated and decoded.
 */
int suit_check_components(struct suit_processor_state *state);

/** Check all command sequences (including the shared sequence).
 *
 *  Check that all components have a payload and a digest.
 *  Check that, if the component list is longer than 1, all sequences start with
 *  a Set Component Index.
 *
 *  This requires that the manifest has been authenticated and decoded.
 */
int suit_check_command_sequences(struct suit_processor_state *state);

#endif /* SUIT_CHECKS_H__ */
