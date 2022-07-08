/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUIT_H__
#define SUIT_H__

#include "suit_types.h"
#include <stdint.h>

/** Decode the string into a manifest envelope and validate the data structure.
 */
int suit_decode_envelope(uint8_t manifest_str, size_t manifest_len,
	struct suit_processor_state *state);

/** Validate higher level rules, such as those outlined in the SUIT information
 *  model and manifest specification. Then check the signature of the manifest.
 */
int suit_validate_envelope(struct suit_processor_state *state);

/** Decode the string into a manifest envelope and validate the data structure.
 */
int suit_decode_manifest(struct suit_processor_state *state);

/** Validate higher level rules, such as those outlined in the SUIT information
 *  model and manifest specification.
 */
int suit_validate_manifest(struct suit_processor_state *state);

/** Do the specified step in the manifest.
 */
int suit_process_manifest(struct suit_processor_state *state,
	enum suit_manifest_step step);


#endif /* SUIT_H__ */
