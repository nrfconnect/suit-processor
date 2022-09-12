/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <suit_types.h>
#include <suit_platform.h>
#include <suit_directive.h>


int suit_directive_set_current_components(struct suit_processor_state *state, struct IndexArg_ *index_arg)
{
	/* Disable all components. */
	for (int i = 0; i < state->num_components; i++) {
		state->current_components[i] = false;
	}

	/* Single component. */
	if (index_arg->_IndexArg_choice == _IndexArg_uint) {
		if (index_arg->_IndexArg_uint >= state->num_components) {
			/* Invalid index. */
			return SUIT_ERR_DECODING;
		} else {
			state->current_components[index_arg->_IndexArg_uint] = true;
		}
	/* Multiple components. */
	} else if (index_arg->_IndexArg_choice == _IndexArg__uint) {
		for (int i = 0; i < index_arg->_IndexArg__uint_uint_count; i++) {
			if (index_arg->_IndexArg__uint_uint[i] >= state->num_components) {
				/* Invalid index. */
				return SUIT_ERR_DECODING;
			}
		}
		/* All indices have been validated, enable the components. */
		for (int i = 0; i < index_arg->_IndexArg__uint_uint_count; i++) {
			state->current_components[index_arg->_IndexArg__uint_uint[i]] = true;
		}
	/* All components (if true) or no components (if false - do nothing). */
	} else if (index_arg->_IndexArg_choice == _IndexArg_bool
		&& index_arg->_IndexArg_bool) {
		/* Enable all components. */
		for (int i = 0; i < state->num_components; i++) {
			state->current_components[i] = true;
		}
	}
	return SUIT_SUCCESS;
}

static int try_each(struct suit_processor_state *state, struct SUIT_Directive_Try_Each_Argument *try_each_arg)
{
	enum suit_bool soft_failure = state->soft_failure;
	int ret;

	for (int i = 0; i < try_each_arg->_SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr_count; i++) {
		state->soft_failure = suit_bool_true;
		ret = suit_directive_run_sequence(state, &try_each_arg->_SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr[i]);

		if ((ret == SUIT_FAIL_CONDITION) && (state->soft_failure == suit_bool_true)) {
			if (try_each_arg->_SUIT_Directive_Try_Each_Argument_nil_present) {
				ret = SUIT_SUCCESS;
			}
		} else {
			break;
		}
	}

	state->soft_failure = soft_failure;

	return ret;
}

int suit_directive_try_each(struct suit_processor_state *state, struct SUIT_Directive_Try_Each_Argument *try_each_arg)
{
	int ret;
	bool current_components_backup[SUIT_MAX_NUM_COMPONENTS];
	memcpy(&current_components_backup, &state->current_components,
		SUIT_MAX_NUM_COMPONENTS * sizeof(bool));
	if (state->num_components > SUIT_MAX_NUM_COMPONENTS) {
		return SUIT_ERR_DECODING;
	}

	for (int i = 0; i < state->num_components; i++) {
		state->current_components[i] = false;
	}

	for (int i = 0; i < state->num_components; i++) {
		if (current_components_backup[i]) {
			state->current_components[i] = true;
			ret = try_each(state, try_each_arg);
			state->current_components[i] = false;

			if (ret != SUIT_SUCCESS) {
				break;
			}
		}
	}

	memcpy(&state->current_components, &current_components_backup, state->num_components);

	return ret;
}


int suit_directive_run_sequence(struct suit_processor_state *state,
		struct zcbor_string *command_sequence)
{
	return SUIT_ERR_UNSUPPORTED_COMMAND;
}


static int set_param(struct suit_manifest_params *dst, struct SUIT_Parameters_ * param)
{
	switch (param->_SUIT_Parameters_choice) {
	case _SUIT_Parameters_suit_parameter_vendor_identifier:
		memcpy(&dst->vid, &param->_SUIT_Parameters_suit_parameter_vendor_identifier, sizeof(dst->vid));
		dst->vid_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_class_identifier:
		memcpy(&dst->cid, &param->_SUIT_Parameters_suit_parameter_class_identifier, sizeof(dst->cid));
		dst->cid_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_image_digest:
		memcpy(&dst->image_digest, &param->_SUIT_Parameters_suit_parameter_image_digest_cbor, sizeof(dst->image_digest));
		dst->image_digest_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_image_size:
		dst->image_size = param->_SUIT_Parameters_suit_parameter_image_size;
		dst->image_size_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_component_slot:
		dst->component_slot = param->_SUIT_Parameters_suit_parameter_component_slot;
		dst->component_slot_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_uri:
		memcpy(&dst->uri, &param->_SUIT_Parameters_suit_parameter_uri, sizeof(dst->uri));
		dst->uri_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_source_component:
		dst->source_component = param->_SUIT_Parameters_suit_parameter_source_component;
		dst->source_component_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_device_identifier:
		memcpy(&dst->did, &param->_SUIT_Parameters_suit_parameter_device_identifier, sizeof(dst->did));
		dst->did_set = true;
		break;
	default:
		return SUIT_ERR_UNSUPPORTED_PARAMETER;
	}
	return SUIT_SUCCESS;
}


int suit_directive_override_parameters(struct suit_processor_state *state,
		struct __suit_directive_override_parameters_map__SUIT_Parameters *params,
		uint_fast32_t param_count)
{
	for (int i = 0; i < state->num_components; i++) {
		if (state->current_components[i]) {
			for (int j = 0; j < param_count; j++) {
				int err = set_param(&state->components[i],
					&params[j].___suit_directive_override_parameters_map__SUIT_Parameters);
				if (err != SUIT_SUCCESS) {
					return err;
				}
			}
		}
	}

	return SUIT_SUCCESS;
}


static int plat_fetch(struct suit_processor_state *state, suit_component_t dst_handle,
			struct zcbor_string *uri)
{
	if (state->dry_run != suit_bool_false) {
		return suit_plat_check_fetch(dst_handle, uri);
	} else {
		return suit_plat_fetch(dst_handle, uri);
	}
}


static int plat_fetch_integrated(struct suit_processor_state *state, suit_component_t dst_handle,
			struct zcbor_string *payload)
{
	if (state->dry_run != suit_bool_false) {
		return suit_plat_check_fetch_integrated(dst_handle, payload);
	} else {
		return suit_plat_fetch_integrated(dst_handle, payload);
	}
}


int suit_directive_fetch(struct suit_processor_state *state)
{

	for (int i = 0; i < state->num_components; i++) {
		if (state->current_components[i]) {
			if (!state->components[i].uri_set) {
				return SUIT_ERR_UNAVAILABLE_PAYLOAD;
			}

			bool integrated = false;
			int ret;

			for (int j = 0; j < state->envelope._SUIT_Envelope__SUIT_Integrated_Payload_count; j++) {
				if (suit_compare_zcbor_strings(&state->envelope._SUIT_Envelope__SUIT_Integrated_Payload[j]._SUIT_Envelope__SUIT_Integrated_Payload._SUIT_Integrated_Payload_suit_integrated_payload_key_key, &state->components[i].uri)) {
					ret = plat_fetch_integrated(state,
						state->components[i].component_handle,
						&state->envelope._SUIT_Envelope__SUIT_Integrated_Payload[j]._SUIT_Envelope__SUIT_Integrated_Payload._SUIT_Integrated_Payload_suit_integrated_payload_key);
					integrated = true;
				}
			}

			if (!integrated) {
				ret = plat_fetch(state, state->components[i].component_handle, &state->components[i].uri);
			}

			if (ret != SUIT_SUCCESS) {
				return ret;
			}
		}
	}

	return SUIT_SUCCESS;
}


static int plat_copy(struct suit_processor_state *state, suit_component_t dst_handle,
		suit_component_t src_handle)
{
	if (state->dry_run != suit_bool_false) {
		return suit_plat_check_copy(dst_handle, src_handle);
	} else {
		return suit_plat_copy(dst_handle, src_handle);
	}
}


int suit_directive_copy(struct suit_processor_state *state)
{
	for (int i = 0; i < state->num_components; i++) {
		if (state->current_components[i]) {
			if (!state->components[i].source_component_set) {
				return SUIT_ERR_UNAVAILABLE_PARAMETER;
			}

			int ret = plat_copy(state, state->components[i].component_handle,
						state->components[i].source_component);

			if (ret != SUIT_SUCCESS) {
				return ret;
			}
		}
	}

	return SUIT_SUCCESS;
}

int suit_directive_swap(struct suit_processor_state *state)
{
	return SUIT_ERR_UNSUPPORTED_COMMAND;
}


static int plat_run(struct suit_processor_state *state, suit_component_t image_handle,
		struct zcbor_string *run_args)
{
	if (state->dry_run != suit_bool_false) {
		return suit_plat_check_run(image_handle, run_args);
	} else {
		return suit_plat_run(image_handle, run_args);
	}
}


int suit_directive_run(struct suit_processor_state *state)
{
	for (int i = 0; i < state->num_components; i++) {
		if (state->current_components[i]) {
			struct zcbor_string *run_args = NULL;

			if (state->components[i].run_args_set) {
				run_args = &state->components[i].run_args;
			}

			int ret = plat_run(state, state->components[i].component_handle, run_args);

			if (ret != SUIT_SUCCESS) {
				return ret;
			}
		}
	}

	return SUIT_SUCCESS;
}
