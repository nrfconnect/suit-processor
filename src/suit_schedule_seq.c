/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdint.h>
#include <zcbor_decode.h>
#include <zcbor_common.h>
#include <manifest_types.h>
#include <manifest_decode.h>
#include <suit_platform.h>
#include <suit_types.h>
#include <suit_condition.h>
#include <suit_directive.h>
#include <suit_platform.h>
#include <suit_seq_exec.h>
#include <suit_manifest.h>


static int suit_validate_single_command(struct suit_processor_state *state, suit_command_t *command, bool is_shared_sequence)
{
	struct suit_seq_exec_state *seq_exec_state;
	size_t component_idx;
	int retval = SUIT_ERR_DECODING;

	if ((command->type == SUIT_COMMAND_DIRECTIVE) &&
	    ((command->directive._SUIT_Directive_choice == _SUIT_Directive___suit_directive_set_component_index) ||
	     (command->directive._SUIT_Directive_choice == _SUIT_Directive___suit_directive_override_parameters))) {
		switch (command->directive._SUIT_Directive_choice) {
		case _SUIT_Directive___suit_directive_set_component_index:
			retval = suit_directive_set_current_components(state,
				&command->directive._SUIT_Directive___suit_directive_set_component_index__IndexArg);
			if (retval == SUIT_ERR_MISSING_COMPONENT) {
				retval = SUIT_ERR_MANIFEST_VALIDATION;
			}
			break;
		case _SUIT_Directive___suit_directive_override_parameters:
			SUIT_DBG("Found valid directive: %d\r\n", command->directive._SUIT_Directive_choice);
			retval = SUIT_SUCCESS;
			break;
		default:
			SUIT_ERR("Found invalid directive: %d\r\n", command->directive._SUIT_Directive_choice);
			retval = SUIT_ERR_MANIFEST_VALIDATION;
			break;
		}

		SUIT_DBG("Single command validated (status: %d)\r\n", retval);

		return retval;
	}

	retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	/* Get the current component index. */
	retval = suit_seq_exec_component_idx_get(seq_exec_state, &component_idx);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	/* If the command has finished and the component list was not exhausted, reschedule the command. */
	while ((retval == SUIT_SUCCESS) && (component_idx != SUIT_MAX_NUM_COMPONENTS)) {
		if (command->type == SUIT_COMMAND_CONDITION) {
			switch (command->condition._SUIT_Condition_choice) {
			case _SUIT_Condition___suit_condition_vendor_identifier:
			case _SUIT_Condition___suit_condition_class_identifier:
			case _SUIT_Condition___suit_condition_device_identifier:
			case _SUIT_Condition___suit_condition_image_match:
			case _SUIT_Condition___suit_condition_component_slot:
			case _SUIT_Condition___suit_condition_abort:
				SUIT_DBG("Found valid condition: %d\r\n", command->condition._SUIT_Condition_choice);
				retval = SUIT_SUCCESS;
				break;
			default:
				SUIT_ERR("Found invalid condition: %d\r\n", command->condition._SUIT_Condition_choice);
				retval = SUIT_ERR_MANIFEST_VALIDATION;
				break;
			}
		}

		else if (command->type == SUIT_COMMAND_DIRECTIVE) {
			switch (command->directive._SUIT_Directive_choice) {
			/* It is safe to call directive-run-sequence as well as directive-try-each during validation,
			 * because they only parses the command string and populates the execution stack.
			 */
			case _SUIT_Directive___suit_directive_try_each:
				SUIT_DBG("Found valid directive: %d\r\n", command->directive._SUIT_Directive_choice);
				retval = suit_directive_try_each(state, &command->directive._SUIT_Directive___suit_directive_try_each__SUIT_Directive_Try_Each_Argument, true);
				break;
			case _SUIT_Directive___suit_directive_run_sequence:
				SUIT_DBG("Validating run-sequence directive: %d\r\n", command->directive._SUIT_Directive_choice);
				retval = suit_directive_run_sequence(state,
					&command->directive._SUIT_Directive___suit_directive_run_sequence_SUIT_Command_Sequence_bstr);
				break;
			case _SUIT_Directive___suit_directive_fetch:
			case _SUIT_Directive___suit_directive_copy:
			case _SUIT_Directive___suit_directive_invoke:
				if (!is_shared_sequence) {
					SUIT_DBG("Found valid directive: %d\r\n", command->directive._SUIT_Directive_choice);
					retval = SUIT_SUCCESS;
					break;
				}
				/* fall-through in case of shared command sequence */
			default:
				SUIT_ERR("Found invalid directive: %d\r\n", command->directive._SUIT_Directive_choice);
				retval = SUIT_ERR_MANIFEST_VALIDATION;
				break;
			}
		} else {
			retval = SUIT_ERR_MANIFEST_VALIDATION;
		}

		/* Sequence finished - execute it for the next component. */
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

	SUIT_DBG("Single command validated (status: %d)\r\n", retval);

	return retval;
}

static inline int suit_validate_single_shared_command(struct suit_processor_state *state, suit_command_t *command)
{
	return suit_validate_single_command(state, command, true);
}

static inline int suit_validate_single_common_command(struct suit_processor_state *state, suit_command_t *command)
{
	return suit_validate_single_command(state, command, false);
}

static int suit_run_single_command(struct suit_processor_state *state, suit_command_t *command)
{
	struct suit_seq_exec_state *seq_exec_state;
	struct suit_manifest_params *params;
	size_t component_idx;
	int retval = SUIT_ERR_DECODING;

	if ((command->type == SUIT_COMMAND_DIRECTIVE) &&
	    ((command->directive._SUIT_Directive_choice == _SUIT_Directive___suit_directive_set_component_index) ||
	     (command->directive._SUIT_Directive_choice == _SUIT_Directive___suit_directive_override_parameters))) {
		switch (command->directive._SUIT_Directive_choice) {
		case _SUIT_Directive___suit_directive_set_component_index:
			retval = suit_directive_set_current_components(state,
				&command->directive._SUIT_Directive___suit_directive_set_component_index__IndexArg);
			break;
		case _SUIT_Directive___suit_directive_override_parameters:
			retval = suit_directive_override_parameters(state,
				command->directive.___suit_directive_override_parameters_map__SUIT_Parameters,
				command->directive.___suit_directive_override_parameters_map__SUIT_Parameters_count);
			break;
		default:
			retval = SUIT_ERR_DECODING;
			break;
		}

		SUIT_ERR("Single command (%d) executed (status: %d)\r\n", command->directive._SUIT_Directive_choice, retval);

		return retval;
	}

	retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	/* Get the current component index. */
	retval = suit_seq_exec_component_idx_get(seq_exec_state, &component_idx);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	/* If the command has finished and the component list was not exhausted, reschedule the command. */
	while ((retval == SUIT_SUCCESS) && (component_idx != SUIT_MAX_NUM_COMPONENTS)) {
		retval = suit_manifest_get_component_params(seq_exec_state->manifest, component_idx, &params);
		if (retval != SUIT_SUCCESS) {
			return retval;
		}

		if (command->type == SUIT_COMMAND_CONDITION) {
			SUIT_DBG("Execute condition %d for component idx: %d (manifest: %p, handle: %p)\r\n",
				command->condition._SUIT_Condition_choice, component_idx,
				seq_exec_state->manifest, params->component_handle);

			switch (command->condition._SUIT_Condition_choice) {
			case _SUIT_Condition___suit_condition_vendor_identifier:
				retval = suit_condition_vendor_identifier(state, params);
				break;
			case _SUIT_Condition___suit_condition_class_identifier:
				retval = suit_condition_class_identifier(state, params);
				break;
			case _SUIT_Condition___suit_condition_device_identifier:
				retval = suit_condition_device_identifier(state, params);
				break;
			case _SUIT_Condition___suit_condition_image_match:
				retval = suit_condition_image_match(state, params);
				break;
			case _SUIT_Condition___suit_condition_component_slot:
				retval = suit_condition_component_slot(state, params);
				break;
			case _SUIT_Condition___suit_condition_abort:
				retval = suit_condition_abort(state, params);
				break;
			default:
				retval = SUIT_ERR_DECODING;
				break;
			}
		}

		else if (command->type == SUIT_COMMAND_DIRECTIVE) {
			SUIT_DBG("Execute directive %d for component idx: %d (manifest: %p, handle: %p)\r\n",
				command->directive._SUIT_Directive_choice, component_idx,
				seq_exec_state->manifest, params->component_handle);

			switch (command->directive._SUIT_Directive_choice) {
			case _SUIT_Directive___suit_directive_run_sequence:
				retval = suit_directive_run_sequence(state,
					&command->directive._SUIT_Directive___suit_directive_run_sequence_SUIT_Command_Sequence_bstr);
				break;
			case _SUIT_Directive___suit_directive_try_each:
				retval = suit_directive_try_each(state, &command->directive._SUIT_Directive___suit_directive_try_each__SUIT_Directive_Try_Each_Argument, false);
				break;
			case _SUIT_Directive___suit_directive_copy:
				retval = suit_directive_copy(state, params);
				break;
			case _SUIT_Directive___suit_directive_invoke:
				retval = suit_directive_invoke(state, params);
				break;
			case _SUIT_Directive___suit_directive_fetch:
				retval = suit_directive_fetch(state, params);
				break;
			default:
				retval = SUIT_ERR_DECODING;
				break;
			}
		} else {
			retval = SUIT_ERR_DECODING;
		}

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

	SUIT_ERR("Single command (%d) executed (status: %d)\r\n", command->directive._SUIT_Directive_choice, retval);

	return retval;
}


int suit_schedule_execution(struct suit_processor_state *state, struct suit_manifest_state *manifest, enum suit_command_sequence seq_name)
{
	struct zcbor_string *cmd_seq_str;

	if (manifest == NULL) {
		return SUIT_ERR_CRASH;
	}

	cmd_seq_str = suit_manifest_get_command_seq(manifest, seq_name);
	if (cmd_seq_str == NULL) {
		return SUIT_ERR_UNAVAILABLE_COMMAND_SEQ;
	}

	SUIT_DBG("Command sequence (%d) scheduled for execution\r\n", seq_name);
	return suit_seq_exec_schedule(state, manifest, cmd_seq_str, suit_bool_false, suit_run_single_command);
}

int suit_schedule_validation(struct suit_processor_state *state, struct suit_manifest_state *manifest, enum suit_command_sequence seq_name)
{
	struct zcbor_string *cmd_seq_str;

	if (manifest == NULL) {
		return SUIT_ERR_ORDER;
	}

	cmd_seq_str = suit_manifest_get_command_seq(manifest, seq_name);
	if (cmd_seq_str == NULL) {
		return SUIT_ERR_UNAVAILABLE_COMMAND_SEQ;
	}

	if (seq_name == SUIT_SEQ_SHARED) {
		SUIT_DBG("Shared sequence scheduled for validation\r\n");
		return suit_seq_exec_schedule(state, manifest, cmd_seq_str, suit_bool_false, suit_validate_single_shared_command);
	} else {
		SUIT_DBG("Command sequence (%d) scheduled for validation\r\n", seq_name);
		return suit_seq_exec_schedule(state, manifest, cmd_seq_str, suit_bool_false, suit_validate_single_common_command);
	}
}

int suit_process_scheduled(struct suit_processor_state *state)
{
	int retval = SUIT_ERR_AGAIN;

	while (retval == SUIT_ERR_AGAIN) {
		retval = suit_seq_exec_step(state);
		if (retval != SUIT_ERR_AGAIN) {
			/* Drop a single element and pass the returned value through the execution stack.
			 * If the last element on the stack is dropped, this API will return the error code
			 * passed as an argument.
			 * If there are still elements on the stack, this API will return SUIT_ERR_AGAIN.
			 */
			retval = suit_seq_exec_finalize(state, retval);
		}
	}

	return retval;
}
