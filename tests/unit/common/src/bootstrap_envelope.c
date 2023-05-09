/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <bootstrap_envelope.h>

void bootstrap_envelope_empty(struct suit_processor_state *state)
{
	struct SUIT_Common *common = &state->manifest._SUIT_Manifest_suit_common_cbor;
	struct SUIT_Severable_Members_Choice_ *severable = &state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice;
	struct SUIT_Unseverable_Members_ *unseverable = &state->manifest._SUIT_Manifest__SUIT_Unseverable_Members;

	if (state == NULL) {
		return;
	}

	/* Pretend that the envelope has been both decoded and validated. */
	state->envelope_decoded = suit_bool_true;
	state->envelope_validated = suit_bool_true;
	state->manifest_decoded = suit_bool_true;
	state->manifest_validated = suit_bool_true;
	state->current_step = 0;
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	state->dry_run = suit_bool_false;
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	common->_SUIT_Common_suit_shared_sequence_present = false;
	severable->_SUIT_Severable_Members_Choice_suit_payload_fetch_present = false;
	severable->_SUIT_Severable_Members_Choice_suit_install_present = false;
	unseverable->_SUIT_Unseverable_Members_suit_validate_present = false;
	unseverable->_SUIT_Unseverable_Members_suit_load_present = false;
	unseverable->_SUIT_Unseverable_Members_suit_invoke_present = false;

	/* Disable all components. */
	for (int i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		state->current_components[i] = false;
	}

	/* Clear execution stack. */
	state->seq_stack_height = 0;
}

void bootstrap_envelope_select_component(struct suit_processor_state *state, size_t component_idx)
{
	if (component_idx < state->num_components) {
		state->current_components[component_idx] = true;
	}
}

void bootstrap_envelope_reset_step(struct suit_processor_state *state)
{
	if (state == NULL) {
		return;
	}

	state->current_step = 0;
}

void bootstrap_envelope_sequence(struct suit_processor_state *state, enum suit_manifest_step step, struct zcbor_string *seq)
{
	struct SUIT_Common *common = &state->manifest._SUIT_Manifest_suit_common_cbor;
	struct SUIT_Severable_Members_Choice_ *severable = &state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice;
	struct SUIT_Unseverable_Members_ *unseverable = &state->manifest._SUIT_Manifest__SUIT_Unseverable_Members;

	if (state == NULL) {
		return;
	}

	switch (step) {
		case SUIT_NO_STEP:
			common->_SUIT_Common_suit_shared_sequence_present = true;
			common->_SUIT_Common_suit_shared_sequence._SUIT_Common_suit_shared_sequence.value = seq->value;
			common->_SUIT_Common_suit_shared_sequence._SUIT_Common_suit_shared_sequence.len = seq->len;
			break;
		case SUIT_PAYLOAD_FETCH:
			severable->_SUIT_Severable_Members_Choice_suit_payload_fetch_present = true;
			severable->_SUIT_Severable_Members_Choice_suit_payload_fetch._SUIT_Severable_Members_Choice_suit_payload_fetch.value = seq->value;
			severable->_SUIT_Severable_Members_Choice_suit_payload_fetch._SUIT_Severable_Members_Choice_suit_payload_fetch.len = seq->len;
			break;
		case SUIT_INSTALL:
			severable->_SUIT_Severable_Members_Choice_suit_install_present = true;
	       		severable->_SUIT_Severable_Members_Choice_suit_install._SUIT_Severable_Members_Choice_suit_install.value = seq->value;
			severable->_SUIT_Severable_Members_Choice_suit_install._SUIT_Severable_Members_Choice_suit_install.len = seq->len;
			break;
		case SUIT_VALIDATE:
			unseverable->_SUIT_Unseverable_Members_suit_validate_present = true;
	       		unseverable->_SUIT_Unseverable_Members_suit_validate._SUIT_Unseverable_Members_suit_validate.value = seq->value;
			unseverable->_SUIT_Unseverable_Members_suit_validate._SUIT_Unseverable_Members_suit_validate.len = seq->len;
			break;
		case SUIT_LOAD:
			unseverable->_SUIT_Unseverable_Members_suit_load_present = true;
	       		unseverable->_SUIT_Unseverable_Members_suit_load._SUIT_Unseverable_Members_suit_load.value = seq->value;
			unseverable->_SUIT_Unseverable_Members_suit_load._SUIT_Unseverable_Members_suit_load.len = seq->len;
			break;
		case SUIT_INVOKE:
			unseverable->_SUIT_Unseverable_Members_suit_invoke_present = true;
	       		unseverable->_SUIT_Unseverable_Members_suit_invoke._SUIT_Unseverable_Members_suit_invoke.value = seq->value;
			unseverable->_SUIT_Unseverable_Members_suit_invoke._SUIT_Unseverable_Members_suit_invoke.len = seq->len;
			break;
		default:
			break;
	}
}

void bootstrap_envelope_components(struct suit_processor_state *state, size_t num_components)
{
	state->num_components = num_components;

	if (state == NULL) {
		return;
	}

	for (size_t i = 0; i < num_components; i++) {
		state->components[i].component_handle = ASSIGNED_COMPONENT_HANDLE + i;
	}
}
