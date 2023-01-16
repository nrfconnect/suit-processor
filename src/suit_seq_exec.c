/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <suit_seq_exec.h>
#include <manifest_decode.h>
#include <suit_platform.h>


static int backup_and_reset_components(struct suit_processor_state *state)
{
	struct suit_seq_exec_state *seq_exec_state;

	int retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	if (seq_exec_state->current_component_idx != SUIT_MAX_NUM_COMPONENTS) {
		SUIT_ERR("Creating backup of components is allowed only on uninitialized frames.\r\n");
		return SUIT_ERR_ORDER;
	}

	SUIT_DBG("%p: Backup and reset component list\r\n",
		seq_exec_state->cmd_seq_str.value);
	memcpy(&seq_exec_state->current_components_backup,
		&state->current_components,
		SUIT_MAX_NUM_COMPONENTS * sizeof(bool));
	if (state->num_components > SUIT_MAX_NUM_COMPONENTS) {
		return SUIT_ERR_DECODING;
	}
	for (int i = 0; i < state->num_components; i++) {
		state->current_components[i] = false;
	}
	return retval;
}

static int recover_components(struct suit_processor_state *state)
{
	struct suit_seq_exec_state *seq_exec_state;

	int retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	/* Recover the list of selected components from backup. */
	memcpy(&state->current_components,
		&seq_exec_state->current_components_backup,
		SUIT_MAX_NUM_COMPONENTS * sizeof(bool));

	SUIT_DBG("%p: Recover component list\r\n",
		seq_exec_state->cmd_seq_str.value);
	SUIT_DBG("%p: Selected components: ", seq_exec_state->cmd_seq_str.value);
	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		if (state->current_components[i]) {
			SUIT_DBG_RAW("%d ", i);
		}
	}
	SUIT_DBG_RAW("\r\n");
	return retval;
}


int suit_seq_exec_schedule(struct suit_processor_state *state, struct zcbor_string *command_sequence, enum suit_bool soft_failure)
{
	if (state->seq_stack_height < SUIT_MAX_SEQ_DEPTH) {
		SUIT_DBG("Push sequence: %p\r\n", command_sequence->value);

		state->seq_stack[state->seq_stack_height].cmd_seq_str.value = command_sequence->value;
		state->seq_stack[state->seq_stack_height].cmd_seq_str.len = command_sequence->len;
		state->seq_stack[state->seq_stack_height].cmd_exec_state = SUIT_SEQ_EXEC_DEFAULT_STATE;
		state->seq_stack[state->seq_stack_height].exec_ptr = command_sequence->value;
		state->seq_stack[state->seq_stack_height].retval = SUIT_SUCCESS;
		state->seq_stack[state->seq_stack_height].soft_failure = soft_failure;
		state->seq_stack[state->seq_stack_height].current_component_idx = SUIT_MAX_NUM_COMPONENTS;
		state->seq_stack[state->seq_stack_height].n_commands = 0;
		state->seq_stack[state->seq_stack_height].current_command = 0;
		state->seq_stack_height += 1;

		return SUIT_ERR_AGAIN;
	}

	return SUIT_ERR_OVERFLOW;
}

int suit_seq_exec_step(struct suit_processor_state *state, seq_exec_processor_t cmd_processor)
{
	suit_command_t command;
	size_t decoded_len = 0;
	struct suit_seq_exec_state *cmd_seq_state = NULL;

	int ret = suit_seq_exec_state_get(state, &cmd_seq_state);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	struct zcbor_string *cmd_seq_str = &(cmd_seq_state->cmd_seq_str);
	ZCBOR_STATE_D(d_state, 1,
		cmd_seq_state->exec_ptr,
		cmd_seq_str->len - (cmd_seq_state->exec_ptr - cmd_seq_str->value),
		1);

	/* If executed for the first time on the sequence - parse the CBOR list header,
	 * initialize command state and store the number of elements.
	 */
	if (cmd_seq_str->value == cmd_seq_state->exec_ptr) {
		bool success = zcbor_list_start_decode(d_state);
		if (!success) {
			int ret = zcbor_peek_error(d_state);
			return ZCBOR_ERR_TO_SUIT_ERR((ret != ZCBOR_SUCCESS) ? ret : ZCBOR_ERR_UNKNOWN);
		}

		if (d_state->indefinite_length_array) {
			return SUIT_ERR_DECODING;
		}

		/* Each command brings a single argument, passed as an element inside the array,
		   thus the number of commands is the half of the number of array elements. */
		if (d_state->elem_count % 2 == 1) {
			return SUIT_ERR_DECODING;
		}

		/* Initialize command processing state. */
		cmd_seq_state->n_commands = d_state->elem_count / 2;
		cmd_seq_state->current_command = 0;
		cmd_seq_state->exec_ptr = d_state->payload;
		cmd_seq_state->cmd_exec_state = SUIT_SEQ_EXEC_DEFAULT_STATE;

		SUIT_DBG("Run sequence with %d elements\r\n", cmd_seq_state->n_commands);
	} else {
		SUIT_DBG("Continue sequence from %d element with %d elements\r\n",
			cmd_seq_state->current_command,
			cmd_seq_state->n_commands);
	}

	while (cmd_seq_state->current_command < cmd_seq_state->n_commands) {
		if (cbor_decode_SUIT_Condition(
			d_state->payload, d_state->payload_end - d_state->payload,
			&command.condition, &decoded_len) == ZCBOR_SUCCESS) {
			SUIT_DBG("%d: Condition %d found\r\n",
				cmd_seq_state->current_command,
				command.condition._SUIT_Condition_choice);

			if (cmd_seq_state->current_command == 0) {
				/* If there is only one component, or the internal sequence is executed,
				 * it is valid to skip the set-component-index command.
				 */
				if ((state->num_components != 1) && (state->seq_stack_height < 2)) {
					SUIT_ERR("Each sequence should begin with a set-component-index command\r\n");
					return SUIT_ERR_MANIFEST_VALIDATION;
				}
			}

			command.type = SUIT_COMMAND_CONDITION;
			d_state->payload += decoded_len;
		}

		else if (cbor_decode_SUIT_Directive(
			d_state->payload, d_state->payload_end - d_state->payload,
			&command.directive, &decoded_len) == ZCBOR_SUCCESS) {
			SUIT_DBG("%d: Directive %d found\r\n",
				cmd_seq_state->current_command,
				command.directive._SUIT_Directive_choice);

			if (cmd_seq_state->current_command == 0) {
				/* If there is only one component, or the internal sequence is executed,
				 * it is valid to skip the set-component-index command.
				 */
				if ((state->num_components != 1) &&
				    (state->seq_stack_height < 2) &&
				    (command.directive._SUIT_Directive_choice
					!= _SUIT_Directive___suit_directive_set_component_index)) {
					SUIT_ERR("Each sequence should begin with a set-component-index command\r\n");
					return SUIT_ERR_MANIFEST_VALIDATION;
				}
			}

			command.type = SUIT_COMMAND_DIRECTIVE;
			d_state->payload += decoded_len;
		} else {
			SUIT_DBG("%d: Unknown command found!\r\n", cmd_seq_state->current_command);
			return SUIT_ERR_DECODING;
		}

		SUIT_DBG("%d: Execute single command processor (ptr: %p)\r\n",
			cmd_seq_state->current_command,
			cmd_seq_state->exec_ptr);
		int retval = cmd_processor(state, &command);
		if (retval == SUIT_SUCCESS) {
			cmd_seq_state->exec_ptr = d_state->payload;
			cmd_seq_state->current_command++;
			cmd_seq_state->cmd_exec_state = SUIT_SEQ_EXEC_DEFAULT_STATE;
			continue;
		}
		else if (retval == SUIT_ERR_AGAIN) {
			SUIT_DBG("%d: Partially processed command. Ptr: %p\r\n",
				cmd_seq_state->current_command,
				cmd_seq_state->exec_ptr);
		}

		return retval;
	}

	if (d_state->payload == d_state->payload_end) {
		return SUIT_SUCCESS;
	}

	return SUIT_ERR_DECODING;
}

int suit_seq_exec_state_get(struct suit_processor_state *state, struct suit_seq_exec_state **seq_exec_state)
{
	if (seq_exec_state == NULL) {
		return SUIT_ERR_CRASH;
	}

	if (state->seq_stack_height > 0) {
		*seq_exec_state = &(state->seq_stack[state->seq_stack_height - 1]);

		return SUIT_SUCCESS;
	}

	*seq_exec_state = NULL;

	return SUIT_ERR_CRASH;
}

int suit_seq_exec_finalize(struct suit_processor_state *state, int retval)
{
	if (state->seq_stack_height > 0) {
		SUIT_DBG("Finalize sequence: %p\r\n", state->seq_stack[state->seq_stack_height - 1].cmd_seq_str.value);
		if ((state->seq_stack[state->seq_stack_height - 1].soft_failure == suit_bool_true) &&
		    (retval == SUIT_FAIL_CONDITION)) {
			retval = SUIT_FAIL_SOFT_CONDITION;
		}

		state->seq_stack_height -= 1;

		/* If there is a command sequence to handle the error code - pass it to the caller stack frame. */
		if (state->seq_stack_height > 0) {
			state->seq_stack[state->seq_stack_height - 1].retval = retval;

			return SUIT_ERR_AGAIN;
		}

		return retval;
	}

	return SUIT_ERR_CRASH;
}

void suit_seq_exec_component_reset(struct suit_processor_state *state)
{
	struct suit_seq_exec_state *seq_exec_state;

	int retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return;
	}

	SUIT_DBG("%p: Reset component list and select %d\r\n",
		seq_exec_state->cmd_seq_str.value,
		seq_exec_state->current_component_idx);

	for (int i = 0; i < state->num_components; i++) {
		if (i == seq_exec_state->current_component_idx) {
			state->current_components[i] = true;
		} else {
			state->current_components[i] = false;
		}
	}
}

int suit_seq_exec_component_idx_get(struct suit_processor_state *state, size_t *component_idx)
{
	struct suit_seq_exec_state *seq_exec_state;

	int retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	if (component_idx == NULL) {
		return SUIT_ERR_CRASH;
	}

	*component_idx = seq_exec_state->current_component_idx;

	/* If executed for the first time - backup components and set the first active index. */
	if (*component_idx == SUIT_MAX_NUM_COMPONENTS) {
		retval = suit_seq_exec_component_idx_next(state, component_idx);
	}

	return retval;
}

int suit_seq_exec_component_idx_next(struct suit_processor_state *state, size_t *component_idx)
{
	struct suit_seq_exec_state *seq_exec_state;

	int retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	if (component_idx == NULL) {
		return SUIT_ERR_CRASH;
	}

	/* Invalidate current component index value before the loop execution. */
	*component_idx = SUIT_MAX_NUM_COMPONENTS;

	if (seq_exec_state->current_component_idx == SUIT_MAX_NUM_COMPONENTS) {
		/* If executed for the first time - backup components and set the first active index. */
		retval = backup_and_reset_components(state);
		if (retval != SUIT_SUCCESS) {
			return retval;
		}
	} else {
		suit_seq_exec_component_reset(state);

		SUIT_DBG("%p: Unselect component %d\r\n",
			seq_exec_state->cmd_seq_str.value,
			seq_exec_state->current_component_idx);
		state->current_components[seq_exec_state->current_component_idx] = false;
	}

	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		if (seq_exec_state->current_components_backup[i]) {
			if ((seq_exec_state->current_component_idx == SUIT_MAX_NUM_COMPONENTS) ||
			    (seq_exec_state->current_component_idx < i)) {
				*component_idx = i;
				break;
			}
		}
	}

	if (*component_idx != SUIT_MAX_NUM_COMPONENTS) {
		seq_exec_state->current_component_idx = *component_idx;
		SUIT_DBG("%p: Select component %d\r\n",
			seq_exec_state->cmd_seq_str.value,
			*component_idx);
		state->current_components[*component_idx] = true;
	} else {
		/* If executed for the last time - restore components from the backup. */
		retval = recover_components(state);
		if (retval != SUIT_SUCCESS) {
			return retval;
		}
		seq_exec_state->current_component_idx = SUIT_MAX_NUM_COMPONENTS;
	}

	return SUIT_SUCCESS;
}
