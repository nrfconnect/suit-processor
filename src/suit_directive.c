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
#include <suit_schedule_seq.h>


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
		SUIT_DBG("Select single component: %d (manifest: %p)\r\n", index_arg->_IndexArg_uint, seq_exec_state->manifest);
		return suit_exec_select_component_idx(seq_exec_state, index_arg->_IndexArg_uint);
	/* Multiple components. */
	} else if (index_arg->_IndexArg_choice == _IndexArg__uint) {
		for (int i = 0; i < index_arg->_IndexArg__uint_uint_count; i++) {
			SUIT_DBG("Select component: %d (manifest: %p)\r\n", index_arg->_IndexArg__uint_uint[i], seq_exec_state->manifest);
			retval = suit_exec_select_component_idx(seq_exec_state, index_arg->_IndexArg__uint_uint[i]);
			if (retval != SUIT_SUCCESS) {
				SUIT_ERR("Unable to set current components: failed to select group of components (%d)\r\n", retval);
				return retval;
			}
		}
	/* All components (if true). The false value is not allowed by CDDL. */
	} else if (index_arg->_IndexArg_choice == _IndexArg_bool) {
		/* Enable all components. */
		SUIT_DBG("Select all components (manifest: %p)\r\n", seq_exec_state->manifest);
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
		SUIT_DBG("Override VID (handle: %p)\r\n", dst->component_handle);
		memcpy(&dst->vid, &param->_SUIT_Parameters_suit_parameter_vendor_identifier, sizeof(dst->vid));
		dst->vid_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_class_identifier:
		SUIT_DBG("Override CID (handle: %p)\r\n", dst->component_handle);
		memcpy(&dst->cid, &param->_SUIT_Parameters_suit_parameter_class_identifier, sizeof(dst->cid));
		dst->cid_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_image_digest:
		SUIT_DBG("Override digest (handle: %p)\r\n", dst->component_handle);
		memcpy(&dst->image_digest, &param->_SUIT_Parameters_suit_parameter_image_digest, sizeof(dst->image_digest));
		dst->image_digest_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_image_size:
		SUIT_DBG("Override image size (handle: %p)\r\n", dst->component_handle);
		dst->image_size = param->_SUIT_Parameters_suit_parameter_image_size;
		dst->image_size_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_component_slot:
		SUIT_DBG("Override slot (handle: %p)\r\n", dst->component_handle);
		dst->component_slot = param->_SUIT_Parameters_suit_parameter_component_slot;
		dst->component_slot_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_uri:
		SUIT_DBG("Override URI (handle: %p)\r\n", dst->component_handle);
		memcpy(&dst->uri, &param->_SUIT_Parameters_suit_parameter_uri, sizeof(dst->uri));
		dst->uri_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_source_component:
		SUIT_DBG("Override source component (handle: %p)\r\n", dst->component_handle);
		dst->source_component = param->_SUIT_Parameters_suit_parameter_source_component;
		dst->source_component_set = true;
		break;
	case _SUIT_Parameters_suit_parameter_device_identifier:
		SUIT_DBG("Override DID (handle: %p)\r\n", dst->component_handle);
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

#ifndef SUIT_PLATFORM_LEGACY_API_SUPPORT
static int suit_directive_set_parameter(struct SUIT_Parameters_ *param, struct suit_manifest_params *dst)
{
	bool parameter_set = false;

	switch (param->_SUIT_Parameters_choice) {
	case _SUIT_Parameters_suit_parameter_vendor_identifier:
		parameter_set = dst->vid_set;
		break;
	case _SUIT_Parameters_suit_parameter_class_identifier:
		parameter_set = dst->cid_set;
		break;
	case _SUIT_Parameters_suit_parameter_image_digest:
		parameter_set = dst->image_digest_set;
		break;
	case _SUIT_Parameters_suit_parameter_image_size:
		parameter_set = dst->image_size_set;
		break;
	case _SUIT_Parameters_suit_parameter_component_slot:
		parameter_set = dst->component_slot_set;
		break;
	case _SUIT_Parameters_suit_parameter_uri:
		parameter_set = dst->uri_set;
		break;
	case _SUIT_Parameters_suit_parameter_source_component:
		parameter_set = dst->source_component_set;
		break;
	case _SUIT_Parameters_suit_parameter_device_identifier:
		parameter_set = dst->did_set;
		break;
	default:
		return SUIT_ERR_UNSUPPORTED_PARAMETER;
	}

	if (parameter_set == false) {
		return suit_directive_override_parameter(param, dst);
	}

	return SUIT_SUCCESS;
}

int suit_directive_set_parameters(struct suit_processor_state *state,
		struct __suit_directive_set_parameters_map__SUIT_Parameters *params,
		uint_fast32_t param_count,
		struct suit_manifest_params *component_params)
{
	int retval = SUIT_ERR_DECODING;

	if ((state == NULL) || (params == NULL) || (component_params == NULL)) {
		SUIT_ERR("Unable to execute set-parameters directive: invalid argument\r\n");
		return SUIT_ERR_DECODING;
	}

	for (int j = 0; j < param_count; j++) {
		struct SUIT_Parameters_ *param = &params[j].___suit_directive_set_parameters_map__SUIT_Parameters;
		SUIT_DBG("Set parameter %d (handle: %p)\r\n", param->_SUIT_Parameters_choice, component_params->component_handle);

		retval = suit_directive_set_parameter(param, component_params);
		if (retval == SUIT_ERR_AGAIN) {
			/* Setting parameters must not use execution stack to take place. */
			retval = SUIT_ERR_TAMP;
		}

		if (retval != SUIT_SUCCESS) {
			break;
		}
	}

	return retval;
}

int suit_directive_process_dependency(struct suit_processor_state *state, struct suit_manifest_params *component_params)
{
	uint8_t *envelope_str;
	size_t envelope_len;
	struct suit_seq_exec_state *seq_exec_state;
	struct suit_manifest_state *manifest_state;

	if (component_params->is_dependency == suit_bool_false) {
		SUIT_ERR("Unsupported component id (not a dependency manifest)\r\n");
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	} else if (component_params->is_dependency != suit_bool_true) {
		SUIT_ERR("Unsupported component id (invalid dependency flag value)\r\n");
		return SUIT_ERR_TAMP;
	} else if (component_params->integrity_checked != true) {
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
		if (state->dry_run != suit_bool_false) {
			return SUIT_SUCCESS;
		}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

		SUIT_ERR("Manifest component integrity not checked\r\n");
		return SUIT_ERR_ORDER;
	}

	int retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	SUIT_DBG("Current manifest dependency seq state: %d\r\n", seq_exec_state->cmd_exec_state);
	manifest_state = &state->manifest_stack[state->manifest_stack_height - 1];

	if (seq_exec_state->retval != SUIT_SUCCESS) {
		retval = seq_exec_state->retval;
	} else if (seq_exec_state->cmd_exec_state == SUIT_SEQ_EXEC_DEFAULT_STATE) {
		/** Return a pointer to the manifest contents, stored inside the component. */
		retval = suit_plat_retrive_manifest(component_params->component_handle, &envelope_str, &envelope_len);

		if (retval == SUIT_SUCCESS) {
			retval = suit_processor_load_envelope(state, envelope_str, envelope_len);
		}

		if (retval == SUIT_SUCCESS) {
			SUIT_DBG("Validate sequences\r\n");
			seq_exec_state->cmd_exec_state = SUIT_SEQ_SHARED;
			seq_exec_state->retval = SUIT_SUCCESS;
			retval = SUIT_ERR_AGAIN;
		} else {
			seq_exec_state->retval = retval;
		}

	} else if ((seq_exec_state->cmd_exec_state >= SUIT_SEQ_SHARED) && (seq_exec_state->cmd_exec_state < SUIT_SEQ_MAX)) {
		seq_exec_state->retval = suit_schedule_validation(state, manifest_state, seq_exec_state->cmd_exec_state);
		if (seq_exec_state->retval == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
			seq_exec_state->retval = SUIT_SUCCESS;
		}

		retval = SUIT_ERR_AGAIN;
		seq_exec_state->cmd_exec_state++;

	} else if (seq_exec_state->cmd_exec_state == SUIT_SEQ_MAX) {
		seq_exec_state->retval = suit_schedule_execution(state, manifest_state, SUIT_SEQ_SHARED);
		SUIT_DBG("Shared sequence scheduled\r\n");

		retval = SUIT_ERR_AGAIN;
		seq_exec_state->cmd_exec_state++;

	} else if (seq_exec_state->cmd_exec_state == SUIT_SEQ_MAX + 1) {
		seq_exec_state->retval = suit_schedule_execution(state, manifest_state, state->current_seq);
		if (seq_exec_state->retval == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
			SUIT_DBG("Command sequence unavailable\r\n");
			seq_exec_state->retval = SUIT_SUCCESS;
		} else {
			SUIT_DBG("Command sequence scheduled\r\n");
		}

		retval = SUIT_ERR_AGAIN;
		seq_exec_state->cmd_exec_state++;

	} else if (seq_exec_state->cmd_exec_state == SUIT_SEQ_MAX + 2) {
		if (seq_exec_state->retval != SUIT_ERR_AGAIN) {
			SUIT_DBG("Command sequence %d executed. Status: %d\r\n", state->current_seq, seq_exec_state->retval);
		}

		retval = seq_exec_state->retval;
	}

	if ((retval != SUIT_ERR_AGAIN) && (seq_exec_state->cmd_exec_state != SUIT_SEQ_EXEC_DEFAULT_STATE)) {
		SUIT_DBG("Release manifest\r\n");
		/* Remove the checked manifest from the stack */
		int ret = suit_manifest_release(manifest_state);
		state->manifest_stack_height--;
		if (retval == SUIT_SUCCESS) {
			seq_exec_state->retval = ret;
		}
	}

	return retval;
}
#endif /* !SUIT_PLATFORM_LEGACY_API_SUPPORT */

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
