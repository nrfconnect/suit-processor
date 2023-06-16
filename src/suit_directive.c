/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <suit_types.h>
#include <suit_platform.h>
#include <suit_directive.h>
#include <suit_manifest.h>
#include <suit_seq_exec.h>


int suit_directive_set_current_components(struct suit_processor_state *state, struct IndexArg_ *index_arg)
{
	struct suit_seq_exec_state *seq_exec_state;

	if ((state == NULL) || (index_arg == NULL)) {
		SUIT_ERR("Unable to set current components: invalid argument\r\n");
		return SUIT_ERR_DECODING;
	}

	int retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	/* Disable all components. */
	retval = suit_exec_deselect_all_components(seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		SUIT_ERR("Unable to set current components: failed to deselect all components (%d)\r\n", retval);
		return retval;
	}

	/* Single component. */
	if (index_arg->_IndexArg_choice == _IndexArg_uint) {
		return suit_exec_select_component_idx(seq_exec_state, index_arg->_IndexArg_uint);
	/* Multiple components. */
	} else if (index_arg->_IndexArg_choice == _IndexArg__uint) {
		for (int i = 0; i < index_arg->_IndexArg__uint_uint_count; i++) {
			retval = suit_exec_select_component_idx(seq_exec_state, index_arg->_IndexArg__uint_uint[i]);
			if (retval != SUIT_SUCCESS) {
				SUIT_ERR("Unable to set current components: failed to select group of components (%d)\r\n", retval);
				return retval;
			}
		}
	/* All components (if true). The false value is not allowed by CDDL. */
	} else if (index_arg->_IndexArg_choice == _IndexArg_bool) {
		/* Enable all components. */
		return suit_exec_select_all_components(seq_exec_state);
	}

	return retval;
}

int suit_directive_try_each(struct suit_processor_state *state, struct SUIT_Directive_Try_Each_Argument *try_each_arg, bool validate)
{
	struct suit_seq_exec_state *seq_exec_state;
	struct zcbor_string *command_sequence = NULL;

	if ((state == NULL) || (try_each_arg == NULL)) {
		SUIT_ERR("Unable to execute try-each directive: invalid argument\r\n");
		return SUIT_ERR_DECODING;
	}

	/* Implement checks enforced by the CDDL, so the nested non-compliant
	 * sequence will not slip through.
	 */
	if (try_each_arg->_SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr_count < 2) {
		return SUIT_ERR_DECODING;
	}

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
		return suit_seq_exec_schedule(state, seq_exec_state->manifest, command_sequence, suit_bool_true, seq_exec_state->cmd_processor);
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


int suit_directive_run_sequence(struct suit_processor_state *state, struct zcbor_string *command_sequence)
{
	struct suit_seq_exec_state *seq_exec_state;

	if ((state == NULL) || (command_sequence == NULL)) {
		SUIT_ERR("Unable to execute run-sequence directive: invalid argument\r\n");
		return SUIT_ERR_DECODING;
	}

	/* Implement checks enforced by the CDDL, so the nested non-compliant
	 * sequence will not slip through.
	 */
	if (command_sequence->len < 3) {
		return SUIT_ERR_DECODING;
	}

	int retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	if (seq_exec_state->cmd_exec_state == SUIT_SEQ_EXEC_DEFAULT_STATE) {
		SUIT_DBG("Push the sequence to run onto the stack\r\n");
		seq_exec_state->cmd_exec_state = 1;
		return suit_seq_exec_schedule(state, seq_exec_state->manifest, command_sequence, suit_bool_false, seq_exec_state->cmd_processor);
	} else if (seq_exec_state->retval == SUIT_FAIL_SOFT_CONDITION) {
		seq_exec_state->retval = SUIT_SUCCESS;
	}

	return seq_exec_state->retval;
}


static int suit_directive_override_parameter(struct SUIT_Parameters_ *param, struct suit_manifest_params *dst)
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
		memcpy(&dst->image_digest, &param->_SUIT_Parameters_suit_parameter_image_digest, sizeof(dst->image_digest));
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
	struct suit_seq_exec_state *seq_exec_state;
	size_t component_idx;

	if ((state == NULL) || (params == NULL)) {
		SUIT_ERR("Unable to execute override-parameters directive: invalid argument\r\n");
		return SUIT_ERR_DECODING;
	}

	int retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	for (int j = 0; j < param_count; j++) {
		struct SUIT_Parameters_ *param = &params[j].___suit_directive_override_parameters_map__SUIT_Parameters;

		if (param->_SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_soft_failure) {
			/* The soft-failure may be set only within try-each or command sequence.
			 * Fail if the stack contains a single entry (the manifest command sequence entry point).
			 */
			if (state->seq_stack_height < 2) {
				return SUIT_ERR_UNSUPPORTED_COMMAND;
			}

			seq_exec_state->soft_failure = (param->_SUIT_Parameters_suit_parameter_soft_failure ? suit_bool_true : suit_bool_false);
			continue;
		}

		/* Get the current component index. */
		retval = suit_seq_exec_component_idx_get(seq_exec_state, &component_idx);
		if (retval != SUIT_SUCCESS) {
			break;
		}

		/* If the command has finished and the component list was not exhausted, reschedule the command. */
		while ((retval == SUIT_SUCCESS) && (component_idx != SUIT_MAX_NUM_COMPONENTS)) {
			struct suit_manifest_params *component_params;
			retval = suit_manifest_get_component_params(seq_exec_state->manifest, component_idx, &component_params);
			if (retval != SUIT_SUCCESS) {
				return retval;
			}

			retval = suit_directive_override_parameter(param, component_params);
			/* Command finished - execute it for the next component. */
			if (retval != SUIT_ERR_AGAIN) {
				int ret = suit_seq_exec_component_idx_next(seq_exec_state, &component_idx);
				if (ret != SUIT_SUCCESS) {
					retval = ret;
				} else if (component_idx != SUIT_MAX_NUM_COMPONENTS) {
					/* Reset the execution state to repeat the sequence. */
					seq_exec_state->cmd_exec_state = SUIT_SEQ_EXEC_DEFAULT_STATE;
				}
			}
		}

		if (retval != SUIT_SUCCESS) {
			break;
		}
	}

	return retval;
}


int suit_directive_fetch(struct suit_processor_state *state, struct suit_manifest_params *component_params)
{
	struct suit_seq_exec_state *seq_exec_state;
	bool integrated = false;
	struct zcbor_string integrated_payload;
	int ret;

	if ((state == NULL) || (component_params == NULL)) {
		SUIT_ERR("Unable to execute fetch directive: invalid argument\r\n");
		return SUIT_ERR_DECODING;
	}

	if (!component_params->uri_set) {
		return SUIT_ERR_UNAVAILABLE_PAYLOAD;
	}

	ret = suit_seq_exec_state_get(state, &seq_exec_state);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	ret = suit_exec_find_integrated_payload(seq_exec_state, &component_params->uri, &integrated_payload);
	if (ret == SUIT_SUCCESS) {
		integrated = true;
	}

	if (!integrated) {
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
		if (state->dry_run != suit_bool_false) {
			ret = suit_plat_check_fetch(component_params->component_handle, &component_params->uri);
		} else {
			ret = suit_plat_fetch(component_params->component_handle, &component_params->uri);
		}
#else /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
		ret = suit_plat_fetch(component_params->component_handle, &component_params->uri);
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
	} else {
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
		if (state->dry_run != suit_bool_false) {
			ret = suit_plat_check_fetch_integrated(component_params->component_handle, &integrated_payload);
		} else {
			ret = suit_plat_fetch_integrated(component_params->component_handle, &integrated_payload);
		}
#else /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
		ret = suit_plat_fetch_integrated(component_params->component_handle, &integrated_payload);
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
	}

	return ret;
}


int suit_directive_copy(struct suit_processor_state *state, struct suit_manifest_params *component_params)
{
	struct suit_seq_exec_state *seq_exec_state;
	suit_component_t dst_handle;
	suit_component_t src_handle;

	if ((state == NULL) || (component_params == NULL)) {
		SUIT_ERR("Unable to execute copy directive: invalid argument\r\n");
		return SUIT_ERR_DECODING;
	}

	int ret = suit_seq_exec_state_get(state, &seq_exec_state);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	dst_handle = component_params->component_handle;

	if (!component_params->source_component_set) {
		return SUIT_ERR_UNAVAILABLE_PARAMETER;
	}

	ret = suit_exec_component_handle_from_idx(seq_exec_state, component_params->source_component, &src_handle);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

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


int suit_directive_swap(struct suit_processor_state *state, struct suit_manifest_params *component_params)
{
	if ((state == NULL) || (component_params == NULL)) {
		SUIT_ERR("Unable to execute swap directive: invalid argument\r\n");
		return SUIT_ERR_DECODING;
	}

	return SUIT_ERR_UNSUPPORTED_COMMAND;
}


int suit_directive_invoke(struct suit_processor_state *state, struct suit_manifest_params *component_params)
{
	suit_component_t image_handle;
	struct zcbor_string *invoke_args = NULL;

	if ((state == NULL) || (component_params == NULL)) {
		SUIT_ERR("Unable to execute invoke directive: invalid argument\r\n");
		return SUIT_ERR_DECODING;
	}

	image_handle = component_params->component_handle;

	if (component_params->invoke_args_set) {
		invoke_args = &component_params->invoke_args;
	}

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
