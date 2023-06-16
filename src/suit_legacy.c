/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <manifest_decode.h>
#include <suit.h>
#include <suit_types.h>
#include <suit_platform.h>
#include <suit_schedule_seq.h>
#include <cose_encode.h>
#include <cose_decode.h>
#include <suit_directive.h>
#include <suit_decoder.h>
#include <suit_manifest.h>


static inline void suit_reset_params(struct suit_manifest_params *params)
{
	suit_component_t bak = params->component_handle;
	struct zcbor_string component_id = params->component_id;
	uint_fast32_t ref_count = params->ref_count;

	memset(params, 0, sizeof(*params));

	params->component_handle = bak;
	params->component_id = component_id;
	params->ref_count = ref_count;
}

/** @brief Reset all component's parameter as well as the list of active components.
 *
 * @param  state  Manifest processor state to be modified.
*/
static int prepare_params(struct suit_processor_state *state)
{
	struct suit_manifest_state *manifest_state = &state->manifest_stack[0];
	int retval = SUIT_SUCCESS;

	if (state->manifest_stack_height != 1) {
		SUIT_ERR("Failed to prepare params before execution\r\n");
		return SUIT_ERR_ORDER;
	}

	/* Reset SUIT parameters */
	for (int i = 0; i < manifest_state->components_count; i++) {
		struct suit_manifest_params *params = NULL;

		retval = suit_manifest_get_component_params(manifest_state, i, &params);
		if (retval == SUIT_SUCCESS) {
			suit_reset_params(params);
		}
	}

	return retval;
}


static int suit_processor_process_manifest(struct suit_processor_state *state, enum suit_manifest_step step)
{
	int ret = SUIT_SUCCESS;

	if (state->manifest_stack_height != 1) {
		SUIT_ERR("Failed to execute sequence %d: manifest not found\r\n", step);
		return SUIT_ERR_ORDER;
	}

	state->current_seq = step;
	struct suit_manifest_state *manifest_state = &state->manifest_stack[0];

	ret = prepare_params(state);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	struct zcbor_string *step_seq = suit_manifest_get_command_seq(manifest_state, step);

	if (step_seq == NULL) {
		SUIT_ERR("Failed to execute sequence %d: sequence not found\r\n", step);
		return SUIT_ERR_UNAVAILABLE_COMMAND_SEQ;
	}

	if (step_seq != NULL) {
		/* Execute shared command sequence */
		ret = suit_schedule_execution(state, manifest_state, SUIT_SEQ_SHARED);
		if (ret == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
			ret = SUIT_SUCCESS;
		} else if (ret == SUIT_ERR_AGAIN) {
			ret = suit_process_scheduled(state);
		}

		if (ret == SUIT_SUCCESS) {
			ret = suit_schedule_execution(state, manifest_state, state->current_seq);
		}
		if (ret == SUIT_ERR_AGAIN) {
			ret = suit_process_scheduled(state);
		}

#ifndef SUIT_PLATFORM_LEGACY_API_SUPPORT
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
		if (state->dry_run == suit_bool_false)
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
		{
			if (ret == SUIT_SUCCESS) {
				ret = suit_plat_sequence_completed(state->current_seq,
					&manifest_state->manifest_component_id,
					manifest_state->envelope_str.value,
					manifest_state->envelope_str.len);
			}
		}
#endif /* !SUIT_PLATFORM_LEGACY_API_SUPPORT */
	}

	return ret;
}


/** Reset the internal state of the SUIT manifest processor.
 */
void suit_reset_state(struct suit_processor_state *state)
{
	if (state == NULL) {
		return;
	}

	if (state->manifest_stack_height <= ZCBOR_ARRAY_SIZE(state->manifest_stack)) {
		for (size_t i = 0; i < state->manifest_stack_height; i++) {
			(void)suit_manifest_release(&state->manifest_stack[i]);
		}
	}

#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
	suit_plat_reset_components();
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */

	memset(state, 0, sizeof(*state));
	state->current_seq = SUIT_SEQ_INVALID;

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	state->dry_run = suit_bool_true;
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	(void)suit_manifest_params_init(state->components, ZCBOR_ARRAY_SIZE(state->components));
}

/** Decode the string into a manifest envelope and validate the data structure.
 */
int suit_decode_envelope(uint8_t *envelope_str, size_t envelope_len,
	struct suit_processor_state *state)
{
	int ret = SUIT_SUCCESS;

	if ((state == NULL) || (state->manifest_stack_height > 1)) {
		return SUIT_ERR_TAMP;
	}

	if (state->manifest_stack_height == 1) {
		ret = suit_manifest_release(&state->manifest_stack[0]);
		state->manifest_stack_height--;
	}

	if (ret == SUIT_SUCCESS) {
		ret = suit_decoder_init(&state->decoder_state, &state->manifest_stack[0]);
	}

	if (ret == SUIT_SUCCESS) {
		ret = suit_decoder_decode_envelope(&state->decoder_state, envelope_str, envelope_len);
	}

	return ret;
}


int suit_validate_envelope(struct suit_processor_state *state)
{
	if ((state == NULL) || (state->manifest_stack_height != 0)) {
		return SUIT_ERR_TAMP;
	}

	int ret = suit_decoder_check_manifest_digest(&state->decoder_state);

	if (ret == SUIT_SUCCESS) {
		ret = suit_decoder_decode_manifest(&state->decoder_state);
	}

	if (ret == SUIT_SUCCESS) {
		ret = suit_decoder_authenticate_manifest(&state->decoder_state);
	}

	return ret;
}


int suit_decode_manifest(struct suit_processor_state *state)
{
	if ((state == NULL) || (state->manifest_stack_height != 0)) {
		return SUIT_ERR_TAMP;
	}

	if (state->decoder_state.step != MANIFEST_AUTHENTICATED) {
		return SUIT_ERR_ORDER;
	}

	/* In order to call authentication APIs, the manifest decoding
	 * is done inside suit_validate_envelope(..)
	 */
	return SUIT_SUCCESS;
}


#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
static int suit_dry_run_manifest(struct suit_processor_state *state)
{
	int ret = SUIT_ERR_TAMP;

	state->dry_run = suit_bool_true;

	for (enum suit_command_sequence seq = SUIT_SEQ_DEP_RESOLUTION; seq < SUIT_SEQ_MAX; seq++) {
		ret = suit_processor_process_manifest(state, seq);
		if (ret == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
			/* The lack of a command sequence (i.e. fetch step) does not invalidate
			 * the whole manifest
			 */
			ret = SUIT_SUCCESS;
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


int suit_validate_manifest(struct suit_processor_state *state)
{
	struct suit_manifest_state *manifest_state;

	if ((state == NULL) || (state->manifest_stack_height != 0)) {
		return SUIT_ERR_TAMP;
	}

	manifest_state = &state->manifest_stack[0];

	int ret = suit_decoder_authorize_manifest(&state->decoder_state);

	if (ret == SUIT_SUCCESS) {
		ret = suit_decoder_decode_sequences(&state->decoder_state);
	}

	if (ret == SUIT_SUCCESS) {
		ret = suit_decoder_create_components(&state->decoder_state);
	}

	if (ret == SUIT_SUCCESS) {
		state->manifest_stack_height++;
	}

	/* Verify shared command sequence */
	if (ret == SUIT_SUCCESS) {
		ret = suit_schedule_validation(state, manifest_state, SUIT_SEQ_SHARED);
		if (ret == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
			ret = SUIT_SUCCESS;
		} else if (ret == SUIT_ERR_AGAIN) {
			ret = prepare_params(state);
			if (ret == SUIT_SUCCESS) {
				ret = suit_process_scheduled(state);
				SUIT_DBG("Manifest shared sequence validated (%d)\r\n", ret);
			}
		}
	}

	/* Verify regular command sequences */
	if (ret == SUIT_SUCCESS) {
		for (enum suit_command_sequence seq = SUIT_SEQ_DEP_RESOLUTION; seq < SUIT_SEQ_MAX; seq++) {
			ret = suit_schedule_validation(state, manifest_state, seq);
			if (ret == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
				ret = SUIT_SUCCESS;
			} else if (ret == SUIT_ERR_AGAIN) {
				ret = prepare_params(state);
				if (ret == SUIT_SUCCESS) {
					ret = suit_process_scheduled(state);
					SUIT_DBG("Manifest sequence %d validated (%d)\r\n", seq, ret);
				}
			}

			if (ret != SUIT_SUCCESS) {
				break;
			}
		}
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (ret == SUIT_SUCCESS) {
		ret = suit_dry_run_manifest(state);
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	if ((ret != SUIT_SUCCESS) && (state->manifest_stack_height > 0)) {
		(void)suit_manifest_release(manifest_state);
		state->manifest_stack_height--;
	}

	return ret;
}


int suit_process_manifest(struct suit_processor_state *state, enum suit_manifest_step step)
{
	struct suit_manifest_state *manifest_state = &state->manifest_stack[0];

	if (state->manifest_stack_height != 1) {
		SUIT_ERR("Failed to fetch manifest (stack height: %d)\r\n", state->manifest_stack_height);
		return SUIT_ERR_ORDER;
	}

	struct zcbor_string *step_seq = suit_manifest_get_command_seq(manifest_state, step);

	if (step_seq == NULL) {
		SUIT_ERR("Failed to execute sequence %d: sequence not found\r\n", step);
		return SUIT_ERR_UNAVAILABLE_COMMAND_SEQ;
	}

#ifndef SUIT_PLATFORM_LEGACY_API_SUPPORT
	int ret = suit_plat_authorize_sequence_num(
		step,
		&manifest_state->manifest_component_id,
		manifest_state->sequence_number);
	if (ret != SUIT_SUCCESS) {
		SUIT_ERR("Failed to authorize sequence number %d for sequence %d\r\n", manifest_state->sequence_number, step);
		return ret;
	}
#endif /* !SUIT_PLATFORM_LEGACY_API_SUPPORT */

	return suit_processor_process_manifest(state, step);
}
