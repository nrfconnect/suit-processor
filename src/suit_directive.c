/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <suit_types.h>
#include <suit_platform.h>
#include <suit_directive.h>
#include <suit_seq_exec.h>


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
	/* All components (if true). The false value is not allowed by CDDL. */
	} else if (index_arg->_IndexArg_choice == _IndexArg_bool) {
		/* Enable all components. */
		for (int i = 0; i < state->num_components; i++) {
			state->current_components[i] = true;
		}
	}
	return SUIT_SUCCESS;
}

static int try_each(struct suit_processor_state *state, struct SUIT_Directive_Try_Each_Argument *try_each_arg, bool validate)
{
	struct suit_seq_exec_state *seq_exec_state;
	struct zcbor_string *command_sequence = NULL;

	int retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	if (seq_exec_state->cmd_exec_state != SUIT_SEQ_EXEC_DEFAULT_STATE) {
		if (seq_exec_state->retval == SUIT_FAIL_SOFT_CONDITION) {
			SUIT_DBG("Try sequence finished with soft failure.\r\n");
			/* Handle soft condition failure - go to the next try sequence. */
			seq_exec_state->retval = SUIT_SUCCESS;
		} else if ((validate) && (seq_exec_state->retval == SUIT_SUCCESS) &&
			   (seq_exec_state->cmd_exec_state < try_each_arg->_SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr_count)) {
			SUIT_DBG("Try sequence validated.\r\n", seq_exec_state->retval);
			/* In case of validation - go to the next sequence if the current one succeeds. */
		} else {
			SUIT_DBG("Try sequence finished. Status: %d.\r\n", seq_exec_state->retval);
			return seq_exec_state->retval;
		}
	}


	if (seq_exec_state->cmd_exec_state < try_each_arg->_SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr_count) {
		SUIT_DBG("Try the next command sequence.\r\n");
		command_sequence = &try_each_arg->_SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr[seq_exec_state->cmd_exec_state];
		seq_exec_state->cmd_exec_state++;
		return suit_seq_exec_schedule(state, command_sequence, suit_bool_true);
	}

	/* If the end of the list reached - verify if there is an empty element at the end.
	 * If we came here, it means all command sequences in the try-each block failed.
	 */
	if (try_each_arg->_SUIT_Directive_Try_Each_Argument_nil_present) {
		return SUIT_SUCCESS;
	}

	/* Cannot use SUIT_FAIL_CONDITION.
	 * If used, the outer try-each will treat it as a soft condition failure.
	 */
	return SUIT_ERR_CRASH;
}


int suit_directive_try_each(struct suit_processor_state *state, struct SUIT_Directive_Try_Each_Argument *try_each_arg, bool validate)
{
	struct suit_seq_exec_state *seq_exec_state;
	size_t component_idx;

	/* Implement checks enforced by the CDDL, so the nested non-compliant
	 * sequence will not slip through.
	 */
	if (try_each_arg->_SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr_count < 2) {
		return SUIT_ERR_DECODING;
	}

	/* Get the current component index. */
	int retval = suit_seq_exec_component_idx_get(state, &component_idx);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	/* If the sequence has finished and the component list was not exhausted, reschedule the sequence. */
	while ((retval == SUIT_SUCCESS) && (component_idx != SUIT_MAX_NUM_COMPONENTS)) {
		retval = try_each(state, try_each_arg, validate);
		/* Sequence finished - execute it for the next component. */
		if (retval != SUIT_ERR_AGAIN) {
			int ret = suit_seq_exec_component_idx_next(state, &component_idx);
			if (ret != SUIT_SUCCESS) {
				retval = ret;
			} else if (component_idx != SUIT_MAX_NUM_COMPONENTS) {
				/* Reset the execution state to repeat the sequence. */
				seq_exec_state->cmd_exec_state = SUIT_SEQ_EXEC_DEFAULT_STATE;
			}
		}
	}

	return retval;
}

int suit_directive_run_sequence(struct suit_processor_state *state,
		struct zcbor_string *command_sequence)
{
	struct suit_seq_exec_state *seq_exec_state;
	size_t component_idx;

	/* Implement checks enforced by the CDDL, so the nested non-compliant
	 * sequence will not slip through.
	 */
	if (command_sequence->len < 3) {
		return SUIT_ERR_DECODING;
	}

	/* Get the current component index. */
	int retval = suit_seq_exec_component_idx_get(state, &component_idx);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	/* If the sequence has finished and the component list was not exhausted, reschedule the sequence. */
	while ((retval == SUIT_SUCCESS) && (component_idx != SUIT_MAX_NUM_COMPONENTS)) {
		if (seq_exec_state->cmd_exec_state == SUIT_SEQ_EXEC_DEFAULT_STATE) {
			SUIT_DBG("Push the sequence to run onto the stack\r\n");
			seq_exec_state->cmd_exec_state = 1;
			return suit_seq_exec_schedule(state, command_sequence, suit_bool_false);
		} else if (seq_exec_state->retval == SUIT_FAIL_SOFT_CONDITION) {
			seq_exec_state->retval = SUIT_SUCCESS;
		}
		/* Sequence finished - execute it for the next component. */
		if (seq_exec_state->retval != SUIT_ERR_AGAIN) {
			int ret = suit_seq_exec_component_idx_next(state, &component_idx);
			if (ret != SUIT_SUCCESS) {
				seq_exec_state->retval = ret;
			} else if (component_idx != SUIT_MAX_NUM_COMPONENTS) {
				/* Reset the execution state to repeat the sequence. */
				seq_exec_state->cmd_exec_state = SUIT_SEQ_EXEC_DEFAULT_STATE;
			}
		}

		retval = seq_exec_state->retval;
	}

	return retval;
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
	for (int j = 0; j < param_count; j++) {
		struct SUIT_Parameters_ *param = &params[j].___suit_directive_override_parameters_map__SUIT_Parameters;

		if (param->_SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_soft_failure) {
			struct suit_seq_exec_state *seq_exec_state;

			/* The soft-failure may be set only within try-each or command sequence.
			 * Fail if the stack contains a single entry (the manifest command sequence entry point).
			 */
			if (state->seq_stack_height < 2) {
				return SUIT_ERR_UNSUPPORTED_COMMAND;
			}

			int retval = suit_seq_exec_state_get(state, &seq_exec_state);
			if (retval != SUIT_SUCCESS) {
				return retval;
			}

			seq_exec_state->soft_failure = (param->_SUIT_Parameters_suit_parameter_soft_failure ? suit_bool_true : suit_bool_false);
			continue;
		}

		for (int i = 0; i < state->num_components; i++) {
			if (state->current_components[i]) {
				int err = set_param(&state->components[i], param);
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
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return suit_plat_check_fetch(dst_handle, uri);
	} else {
		return suit_plat_fetch(dst_handle, uri);
	}
#else /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
	return suit_plat_fetch(dst_handle, uri);
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
}


static int plat_fetch_integrated(struct suit_processor_state *state, suit_component_t dst_handle,
			struct zcbor_string *payload)
{
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return suit_plat_check_fetch_integrated(dst_handle, payload);
	} else {
		return suit_plat_fetch_integrated(dst_handle, payload);
	}
#else /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
	return suit_plat_fetch_integrated(dst_handle, payload);
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
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
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return suit_plat_check_copy(dst_handle, src_handle);
	} else {
		return suit_plat_copy(dst_handle, src_handle);
	}
#else /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
	return suit_plat_copy(dst_handle, src_handle);
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
}


int suit_directive_copy(struct suit_processor_state *state)
{
	for (int i = 0; i < state->num_components; i++) {
		if (state->current_components[i]) {
			if (!state->components[i].source_component_set) {
				return SUIT_ERR_UNAVAILABLE_PARAMETER;
			}

			const size_t src_idx = state->components[i].source_component;
			if (src_idx >= state->num_components) {
				return SUIT_ERR_UNSUPPORTED_PARAMETER;
			}

			const suit_component_t src_handle = state->components[src_idx].component_handle;
			int ret = plat_copy(state, state->components[i].component_handle, src_handle);

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


static int plat_invoke(struct suit_processor_state *state, suit_component_t image_handle,
		struct zcbor_string *invoke_args)
{
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return suit_plat_check_invoke(image_handle, invoke_args);
	} else {
		return suit_plat_invoke(image_handle, invoke_args);
	}
#else /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
	return suit_plat_invoke(image_handle, invoke_args);
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
}


int suit_directive_invoke(struct suit_processor_state *state)
{
	for (int i = 0; i < state->num_components; i++) {
		if (state->current_components[i]) {
			struct zcbor_string *invoke_args = NULL;

			if (state->components[i].invoke_args_set) {
				invoke_args = &state->components[i].invoke_args;
			}

			int ret = plat_invoke(state, state->components[i].component_handle, invoke_args);

			if (ret != SUIT_SUCCESS) {
				return ret;
			}
		}
	}

	return SUIT_SUCCESS;
}
