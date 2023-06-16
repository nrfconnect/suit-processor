/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_LEGACY_H__
#define SUIT_LEGACY_H__

#include <suit_types.h>
#include <suit_processor.h>

#define SUIT_NUM_STEPS 6

enum suit_manifest_step {
	SUIT_NO_STEP = SUIT_SEQ_SHARED,
	SUIT_DEP_RESOLUTION = SUIT_SEQ_DEP_RESOLUTION,
	SUIT_PAYLOAD_FETCH = SUIT_SEQ_PAYLOAD_FETCH,
	SUIT_INSTALL = SUIT_SEQ_INSTALL,
	SUIT_VALIDATE = SUIT_SEQ_VALIDATE,
	SUIT_LOAD = SUIT_SEQ_LOAD,
	SUIT_INVOKE = SUIT_SEQ_INVOKE,
	SUIT_LAST_STEP = SUIT_SEQ_MAX,
};

static inline void suit_reset_params(struct suit_manifest_params *params)
{
	suit_component_t bak = params->component_handle;
	memset(params, 0, sizeof(*params));
	params->component_handle = bak;
}

/** Reset the iternal state of the SUIT manifest processor.
 */
void suit_reset_state(struct suit_processor_state *state);

/** Decode the string into a manifest envelope and validate the data structure.
 */
int suit_decode_envelope(uint8_t *manifest_str, size_t manifest_len,
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

#endif /* SUIT_LEGACY_H__ */
