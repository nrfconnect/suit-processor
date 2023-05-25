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
#include <suit_types.h>
#include <suit_condition.h>
#include <suit_directive.h>
#include <suit_platform.h>
#include <suit_seq_exec.h>


static int suit_validate_single_command(struct suit_processor_state *state, suit_command_t *command, bool is_shared_sequence)
{
	int retval = SUIT_ERR_DECODING;

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
		case _SUIT_Directive___suit_directive_set_component_index:
			retval = suit_directive_set_current_components(state,
				&command->directive._SUIT_Directive___suit_directive_set_component_index__IndexArg);
			if (retval == SUIT_ERR_DECODING) {
				retval = SUIT_ERR_MANIFEST_VALIDATION;
			}
			break;
		case _SUIT_Directive___suit_directive_override_parameters:
			SUIT_DBG("Found valid directive: %d\r\n", command->directive._SUIT_Directive_choice);
			retval = SUIT_SUCCESS;
			break;
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
	int retval = SUIT_ERR_DECODING;

	if (command->type == SUIT_COMMAND_CONDITION) {
		for (int j = 0; j < state->num_components; j++) {
			if (state->current_components[j]) {
				switch (command->condition._SUIT_Condition_choice) {
				case _SUIT_Condition___suit_condition_vendor_identifier:
					retval = suit_condition_vendor_identifier(state, &state->components[j]);
					break;
				case _SUIT_Condition___suit_condition_class_identifier:
					retval = suit_condition_class_identifier(state, &state->components[j]);
					break;
				case _SUIT_Condition___suit_condition_device_identifier:
					retval = suit_condition_device_identifier(state, &state->components[j]);
					break;
				case _SUIT_Condition___suit_condition_image_match:
					retval = suit_condition_image_match(state, &state->components[j]);
					break;
				case _SUIT_Condition___suit_condition_component_slot:
					retval = suit_condition_component_slot(state, &state->components[j]);
					break;
				case _SUIT_Condition___suit_condition_abort:
					retval = suit_condition_abort(state, &state->components[j]);
					break;
				default:
					retval = SUIT_ERR_DECODING;
					break;
				}

				/* If a condition for any of the selected components failed,
				 * the whole condition should fail.
				 */
				if (retval != SUIT_SUCCESS) {
					break;
				}
			}
		}
	}

	else if (command->type == SUIT_COMMAND_DIRECTIVE) {
		switch (command->directive._SUIT_Directive_choice) {
		case _SUIT_Directive___suit_directive_set_component_index:
			retval = suit_directive_set_current_components(state,
				&command->directive._SUIT_Directive___suit_directive_set_component_index__IndexArg);
			break;
		case _SUIT_Directive___suit_directive_try_each:
			retval = suit_directive_try_each(state, &command->directive._SUIT_Directive___suit_directive_try_each__SUIT_Directive_Try_Each_Argument, false);
			break;
		case _SUIT_Directive___suit_directive_override_parameters:
			retval = suit_directive_override_parameters(state,
				command->directive.___suit_directive_override_parameters_map__SUIT_Parameters,
				command->directive.___suit_directive_override_parameters_map__SUIT_Parameters_count);
			break;
		case _SUIT_Directive___suit_directive_run_sequence:
			retval = suit_directive_run_sequence(state,
				&command->directive._SUIT_Directive___suit_directive_run_sequence_SUIT_Command_Sequence_bstr);
			break;
		case _SUIT_Directive___suit_directive_fetch:
			retval = suit_directive_fetch(state);
			break;
		case _SUIT_Directive___suit_directive_copy:
			retval = suit_directive_copy(state);
			break;
		case _SUIT_Directive___suit_directive_invoke:
			retval = suit_directive_invoke(state);
			break;
		default:
			retval = SUIT_ERR_DECODING;
			break;
		}
	}

	SUIT_DBG("Single command executed (status: %d)\r\n", retval);

	return retval;
}


int suit_validate_shared_sequence(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str)
{
	int retval = suit_seq_exec_schedule(state, cmd_seq_str, suit_bool_false);

	while (retval == SUIT_ERR_AGAIN) {
		retval = suit_seq_exec_step(state, suit_validate_single_shared_command);
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

int suit_validate_command_sequence(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str)
{
	int retval = suit_seq_exec_schedule(state, cmd_seq_str, suit_bool_false);

	while (retval == SUIT_ERR_AGAIN) {
		retval = suit_seq_exec_step(state, suit_validate_single_common_command);
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

int suit_run_command_sequence(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str)
{
	int retval = suit_seq_exec_schedule(state, cmd_seq_str, suit_bool_false);

	while (retval == SUIT_ERR_AGAIN) {
		retval = suit_seq_exec_step(state, suit_run_single_command);
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
