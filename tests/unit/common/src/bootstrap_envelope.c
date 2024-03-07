/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <bootstrap_envelope.h>

void bootstrap_envelope_empty(struct suit_processor_state *state)
{
	struct suit_manifest_state *manifest_state = &state->manifest_stack[0];

	if (state == NULL) {
		return;
	}

	state->manifest_stack_height = 0;
	memset(&state->manifest_stack[0], 0, sizeof(state->manifest_stack[0]));

	/* Pretend that the envelope has been both decoded and validated. */
	state->manifest_stack_height = 1;
	state->current_seq = SUIT_SEQ_INVALID;

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	state->dry_run = suit_bool_false;
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	manifest_state->shared_sequence_status = UNAVAILABLE;
	manifest_state->dependency_resolution_seq_status = UNAVAILABLE;
	manifest_state->payload_fetch_seq_status = UNAVAILABLE;
	manifest_state->install_seq_status = UNAVAILABLE;
	manifest_state->candidate_verification_seq_status = UNAVAILABLE;
	manifest_state->validate_seq_status = UNAVAILABLE;
	manifest_state->load_seq_status = UNAVAILABLE;
	manifest_state->invoke_seq_status = UNAVAILABLE;
	manifest_state->text_status = UNAVAILABLE;

	/* Clear execution stack. */
	state->seq_stack_height = 0;
}

void bootstrap_envelope_reset_step(struct suit_processor_state *state)
{
	if (state == NULL) {
		return;
	}

	state->current_seq = SUIT_SEQ_INVALID;
}

void bootstrap_envelope_sequence(struct suit_processor_state *state, enum suit_command_sequence seq_name, struct zcbor_string *seq)
{
	struct suit_manifest_state *manifest;

	if ((state == NULL) || (state->manifest_stack_height != 1)) {
		return;
	}

	manifest = &state->manifest_stack[0];

	switch (seq_name) {
		case SUIT_SEQ_SHARED:
			manifest->shared_sequence = *seq;
			manifest->shared_sequence_status = AUTHENTICATED;
			break;
		case SUIT_SEQ_PAYLOAD_FETCH:
			manifest->payload_fetch_seq = *seq;
			manifest->payload_fetch_seq_status = AUTHENTICATED;
			break;
		case SUIT_SEQ_INSTALL:
			manifest->install_seq = *seq;
			manifest->install_seq_status = AUTHENTICATED;
			break;
		case SUIT_SEQ_VALIDATE:
			manifest->validate_seq = *seq;
			manifest->validate_seq_status = AUTHENTICATED;
			break;
		case SUIT_SEQ_LOAD:
			manifest->load_seq = *seq;
			manifest->load_seq_status = AUTHENTICATED;
			break;
		case SUIT_SEQ_INVOKE:
			manifest->invoke_seq = *seq;
			manifest->invoke_seq_status = AUTHENTICATED;
			break;
		default:
			break;
	}
}

void bootstrap_envelope_components(struct suit_processor_state *state, size_t num_components)
{
	struct suit_manifest_state *manifest;

	if ((state == NULL) || (state->manifest_stack_height != 1)) {
		return;
	}

	manifest = &state->manifest_stack[0];
	manifest->components_count = num_components;

	for (size_t i = 0; i < num_components; i++) {
		state->components[i].component_handle = ASSIGNED_COMPONENT_HANDLE + i;
		state->components[i].ref_count = 1;
		state->components[i].is_dependency = suit_bool_false;
		manifest->component_map[i] = i;
	}
}

void bootstrap_envelope_dependency_components(struct suit_processor_state *state, size_t num_components)
{
	struct suit_manifest_state *manifest;

	if ((state == NULL) || (state->manifest_stack_height != 1)) {
		return;
	}

	manifest = &state->manifest_stack[0];
	manifest->components_count = num_components;

	for (size_t i = 0; i < num_components; i++) {
		state->components[i].component_handle = ASSIGNED_COMPONENT_HANDLE + i;
		state->components[i].ref_count = 1;
		state->components[i].is_dependency = suit_bool_true;
		manifest->component_map[i] = i;
	}
}
