/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <suit_processor.h>
#include <suit_decoder.h>
#include <suit_platform.h>
#include <suit_manifest.h>
#include <suit_schedule_seq.h>
#include <manifest_decode.h>

static struct suit_processor_state processor_state;
static struct suit_processor_state *state = &processor_state;


static int suit_processor_decode_envelope(struct suit_decoder_state *decoder_state, struct suit_manifest_state *manifest,
	uint8_t *envelope_str, size_t envelope_len)
{
	int ret = SUIT_SUCCESS;

	memset(manifest, 0, sizeof(*manifest));

	if (ret == SUIT_SUCCESS) {
		SUIT_DBG("Initialize decoder context with manifest: %p\r\n", manifest);
		ret = suit_decoder_init(decoder_state, manifest);
	}

	if (ret == SUIT_SUCCESS) {
		SUIT_DBG("Decode envelope: %p (%d)\r\n", envelope_str, envelope_len);
		ret = suit_decoder_decode_envelope(decoder_state, envelope_str, envelope_len);
	}

	if (ret == SUIT_SUCCESS) {
		SUIT_DBG("Check manifest digest\r\n");
		ret = suit_decoder_check_manifest_digest(decoder_state);
	}

	if (ret == SUIT_SUCCESS) {
		SUIT_DBG("Decode manifest contents\r\n");
		ret = suit_decoder_decode_manifest(decoder_state);
	}

	return ret;
}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
static int suit_dry_run_manifest(struct suit_manifest_state *manifest_state)
{
	int ret = SUIT_ERR_TAMP;

	state->dry_run = suit_bool_true;

	for (enum suit_command_sequence seq = SUIT_SEQ_DEP_RESOLUTION; seq < SUIT_SEQ_MAX; seq++) {
		SUIT_DBG("Dry-run sequence: %d\r\n", seq);

		struct zcbor_string *step_seq = suit_manifest_get_command_seq(manifest_state, seq);
		if (step_seq == NULL) {
			SUIT_ERR("Failed to execute sequence %d: sequence not found\r\n", seq);
			continue;
		}

		/* Execute shared command sequence */
		ret = suit_schedule_execution(state, manifest_state, SUIT_SEQ_SHARED);
		if (ret == SUIT_ERR_AGAIN) {
			ret = suit_process_scheduled(state);
		}

		if (ret == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
			ret = SUIT_SUCCESS;
		} else {
			SUIT_DBG("Shared sequence executed. Status: %d\r\n", ret);
		}

		if (ret == SUIT_SUCCESS) {
			ret = suit_schedule_execution(state, manifest_state, seq);
			if (ret == SUIT_ERR_AGAIN) {
				ret = suit_process_scheduled(state);
			}

			if (ret == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
				SUIT_ERR("Failed to execute sequence %d: sequence not found\r\n", seq);
			} else {
				SUIT_DBG("Command sequence %d executed. Status: %d\r\n", seq, ret);
			}
		}

		if (ret != SUIT_SUCCESS) {
			SUIT_ERR("Manifest sequence %d dry-run failed\r\n", seq);
			break;
		}
	}

	SUIT_DBG("Manifest dry-run finished\r\n");
	state->dry_run = suit_bool_false;

	return ret;
}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */


int suit_processor_init(void)
{
	int err = suit_manifest_params_init(state->components, ZCBOR_ARRAY_SIZE(state->components));
	if (err == SUIT_ERR_ORDER) {
		/* Allow to call init even if the manifest module is already initialized. */
		return SUIT_SUCCESS;
	}

	return SUIT_SUCCESS;
}

int suit_processor_load_envelope(struct suit_processor_state *state, uint8_t *envelope_str, size_t envelope_len)
{
	struct suit_manifest_state *manifest_state = NULL;
	int retval = SUIT_SUCCESS;

	if ((state == NULL) || (envelope_str == NULL) || (envelope_len < 1)) {
		return SUIT_ERR_CRASH;
	}

	if (state->manifest_stack_height >= ZCBOR_ARRAY_SIZE(state->manifest_stack)) {
		return SUIT_ERR_OVERFLOW;
	}

	SUIT_DBG("Parse manifest: %p (%d)\r\n", envelope_str, envelope_len);
	manifest_state = &state->manifest_stack[state->manifest_stack_height];
	retval = suit_processor_decode_envelope(
		&state->decoder_state,
		manifest_state,
		envelope_str, envelope_len);

	if (retval == SUIT_SUCCESS) {
		SUIT_DBG("Authenticate manifest digest\r\n");
		retval = suit_decoder_authenticate_manifest(&state->decoder_state);
	}

	if (retval == SUIT_SUCCESS) {
		SUIT_DBG("Authorize manifest\r\n");
		retval = suit_decoder_authorize_manifest(&state->decoder_state);
	}

	if (retval == SUIT_SUCCESS) {
		SUIT_DBG("Decode sequences\r\n");
		retval = suit_decoder_decode_sequences(&state->decoder_state);
	}

#ifndef SUIT_PLATFORM_LEGACY_API_SUPPORT
	if (retval == SUIT_SUCCESS) {
		SUIT_DBG("Authorize sequence number: %d for sequence: %d\r\n", manifest_state->sequence_number, state->current_seq);
		/** Check that the provided sequence number for a given manifest is recent enough. */
		retval = suit_plat_authorize_sequence_num(
			state->current_seq,
			&manifest_state->manifest_component_id,
			manifest_state->sequence_number);
	}
#endif /* !SUIT_PLATFORM_LEGACY_API_SUPPORT */

	if (retval == SUIT_SUCCESS) {
		SUIT_DBG("Create component handles\r\n");
		retval = suit_decoder_create_components(&state->decoder_state);
	}

	if (retval == SUIT_SUCCESS) {
		SUIT_DBG("Manifest loaded\r\n");
		state->manifest_stack_height++;
	}

	return retval;
}

int suit_process_sequence(uint8_t *envelope_str, size_t envelope_len, enum suit_command_sequence seq_name)
{
	int ret = SUIT_SUCCESS;
	struct suit_manifest_state *manifest_state = NULL;

	if ((seq_name < SUIT_SEQ_PARSE) || (seq_name >= SUIT_SEQ_MAX)) {
		return SUIT_ERR_UNAVAILABLE_COMMAND_SEQ;
	}

	state->current_seq = seq_name;

	SUIT_DBG("Decode manifest: %p (%d)\r\n", envelope_str, envelope_len);
	manifest_state = &state->manifest_stack[state->manifest_stack_height];

	ret = suit_processor_load_envelope(state, envelope_str, envelope_len);

	if (ret == SUIT_SUCCESS) {
		SUIT_DBG("Validate sequences\r\n");

		/* Verify manifest members */
		for (enum suit_command_sequence seq = SUIT_SEQ_SHARED; seq < SUIT_SEQ_MAX; seq++) {
			int ret = suit_schedule_validation(state, manifest_state, seq);
			if (ret == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
				ret = SUIT_SUCCESS;
			} else if (ret == SUIT_ERR_AGAIN) {
				ret = suit_process_scheduled(state);
			}

			if (ret != SUIT_SUCCESS) {
				SUIT_ERR("Manifest sequence %d validation failed (%d)\r\n", seq, ret);
				break;
			} else {
				SUIT_DBG("Manifest sequence %d validated\r\n", seq);
			}
		}

		SUIT_DBG("Manifest validation finished\r\n");

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
		if (ret == SUIT_SUCCESS) {
			ret = suit_dry_run_manifest(manifest_state);
		}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
	} else {
		manifest_state = NULL;
	}

	if ((ret == SUIT_SUCCESS) && (seq_name > SUIT_SEQ_PARSE)) {
		SUIT_DBG("Check if sequence %d is defined inside the manifest\r\n", seq_name);

		struct zcbor_string *step_seq = suit_manifest_get_command_seq(manifest_state, seq_name);
		if (step_seq == NULL) {
			SUIT_ERR("Failed to execute sequence %d: sequence not found\r\n", seq_name);
			ret = SUIT_ERR_UNAVAILABLE_COMMAND_SEQ;
		}
	}

	if ((ret == SUIT_SUCCESS) && (seq_name > SUIT_SEQ_PARSE)) {
		SUIT_DBG("Execute sequence: %d\r\n", seq_name);

		/* Execute shared command sequence */
		ret = suit_schedule_execution(state, manifest_state, SUIT_SEQ_SHARED);
		if (ret == SUIT_ERR_AGAIN) {
			ret = suit_process_scheduled(state);
		}

		if (ret == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
			ret = SUIT_SUCCESS;
		} else {
			SUIT_DBG("Shared sequence executed. Status: %d\r\n", ret);
		}

		if (ret == SUIT_SUCCESS) {
			ret = suit_schedule_execution(state, manifest_state, seq_name);
			if (ret == SUIT_ERR_AGAIN) {
				ret = suit_process_scheduled(state);
			}

			if (ret == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
				SUIT_ERR("Failed to execute sequence %d: sequence not found\r\n", seq_name);
			} else {
				SUIT_DBG("Command sequence %d executed. Status: %d\r\n", seq_name, ret);
			}
		}

#ifndef SUIT_PLATFORM_LEGACY_API_SUPPORT
		if (ret == SUIT_SUCCESS) {
			ret = suit_plat_sequence_completed(state->current_seq,
				&manifest_state->manifest_component_id,
				manifest_state->envelope_str.value,
				manifest_state->envelope_str.len);
		}
#endif /* !SUIT_PLATFORM_LEGACY_API_SUPPORT */
	}

	if (manifest_state != NULL) {
		(void)suit_manifest_release(manifest_state);
		state->manifest_stack_height--;
	}

	return ret;
}

int suit_processor_get_manifest_metadata(uint8_t *envelope_str, size_t envelope_len, bool authenticate, struct zcbor_string *manifest_component_id, struct zcbor_string *digest, unsigned int *seq_num)
{
	int ret = SUIT_SUCCESS;
	struct suit_decoder_state *decoder_state = &state->decoder_state;
	struct suit_manifest_state *manifest;

	if ((envelope_str == NULL) || (envelope_len == 0)) {
		return SUIT_ERR_DECODING;
	}

	if (ret == SUIT_SUCCESS) {
		manifest = &state->manifest_stack[state->manifest_stack_height];
		ret = suit_processor_decode_envelope(decoder_state, manifest, envelope_str, envelope_len);
	}

	if (authenticate) {
		if (ret == SUIT_SUCCESS) {
			SUIT_DBG("Authenticate manifest digest\r\n");
			ret = suit_decoder_authenticate_manifest(decoder_state);
		}

		if (ret == SUIT_SUCCESS) {
			SUIT_DBG("Authorize manifest\r\n");
			ret = suit_decoder_authorize_manifest(decoder_state);
		}
	}

	if (ret == SUIT_SUCCESS) {
		struct SUIT_Digest digest_cbor;
		size_t bytes_processed;
		struct zcbor_string *digest_bstr = &decoder_state->manifest_digest_bytes;

		ret = cbor_decode_SUIT_Digest(digest_bstr->value, digest_bstr->len, &digest_cbor,
					      &bytes_processed);

		/* The SHA256 algorithm is enforced by CDDL, so the digest length is known. */
		if ((bytes_processed != digest_bstr->len) ||
		    (digest_cbor._SUIT_Digest_suit_digest_bytes.len != 32)) {
			ret = SUIT_ERR_DECODING;
		}

		if ((ret == SUIT_SUCCESS) && (digest != NULL)) {
			*digest = digest_cbor._SUIT_Digest_suit_digest_bytes;
		}
	}

	if (ret == SUIT_SUCCESS) {
		if (seq_num != NULL) {
			*seq_num = decoder_state->decoded_manifest->sequence_number;
		}

		if (manifest_component_id != NULL) {
			*manifest_component_id = decoder_state->decoded_manifest->manifest_component_id;
		}
	}

	/* There is no need to call suit_manifest_release(..) because
	 * the manifest was only parsed and no component were created.
	 */

	return SUIT_SUCCESS;
}
