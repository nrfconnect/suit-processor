/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <suit_seq_exec.h>
#include <manifest_decode.h>
#include <suit_platform.h>
#include <suit_manifest.h>
#include <zcbor_decode.h>


static int backup_and_reset_components(struct suit_seq_exec_state *seq_exec_state)
{
	if (seq_exec_state == NULL) {
		return SUIT_ERR_CRASH;
	}

	if (seq_exec_state->current_component_idx != SUIT_MAX_NUM_COMPONENTS) {
		SUIT_ERR("Creating backup of components is allowed only on uninitialized frames.\r\n");
		return SUIT_ERR_ORDER;
	}

	memcpy(&seq_exec_state->current_components_backup,
		&seq_exec_state->current_components,
		SUIT_MAX_NUM_COMPONENTS * sizeof(bool));
	if (seq_exec_state->manifest->components_count > SUIT_MAX_NUM_COMPONENTS) {
		return SUIT_ERR_DECODING;
	}

	return suit_exec_deselect_all_components(seq_exec_state);
}

static int recover_components(struct suit_seq_exec_state *seq_exec_state)
{
	if (seq_exec_state == NULL) {
		return SUIT_ERR_CRASH;
	}

	/* Recover the list of selected components from backup. */
	memcpy(&seq_exec_state->current_components,
		&seq_exec_state->current_components_backup,
		SUIT_MAX_NUM_COMPONENTS * sizeof(bool));

	SUIT_DBG("%p: Selected components: ", seq_exec_state->cmd_seq_str.value);
	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		if (seq_exec_state->current_components[i]) {
			SUIT_DBG_RAW("%d ", i);
		}
	}
	SUIT_DBG_RAW("\r\n");

	return SUIT_SUCCESS;
}

static int init_selected_components(struct suit_seq_exec_state *seq_exec_state)
{
	if (seq_exec_state == NULL) {
		return SUIT_ERR_CRASH;
	}

	int retval = suit_exec_deselect_all_components(seq_exec_state);

	/* If there is only one component - set it as an active one */
	if ((retval == SUIT_SUCCESS) &&
	    (seq_exec_state->manifest->components_count == 1)) {
		retval = suit_exec_select_component_idx(seq_exec_state, 0);
	} else if (seq_exec_state->manifest->components_count == 0) {
		retval = SUIT_ERR_MANIFEST_VALIDATION;
	}

	return retval;
}

/** @brief Reset the list of active components and select the current one.
 *
 * @param[in]  state   The SUIT processor state to be modified.
 */
static void suit_seq_exec_component_reset(struct suit_seq_exec_state *seq_exec_state)
{
	if (seq_exec_state == NULL) {
		return;
	}

	for (int i = 0; i < seq_exec_state->manifest->components_count; i++) {
		if (i == seq_exec_state->current_component_idx) {
			seq_exec_state->current_components[i] = true;
		} else {
			seq_exec_state->current_components[i] = false;
		}
	}
}


int suit_exec_select_component_idx(struct suit_seq_exec_state *seq_exec_state, size_t index)
{
	if (seq_exec_state == NULL) {
		return SUIT_ERR_CRASH;
	}

	if ((seq_exec_state->manifest != NULL) && (seq_exec_state->manifest->components_count > index)) {
		seq_exec_state->current_components[index] = true;
		return SUIT_SUCCESS;
	}

	return SUIT_ERR_MISSING_COMPONENT;
}

int suit_exec_select_all_components(struct suit_seq_exec_state *seq_exec_state)
{
	if (seq_exec_state == NULL) {
		return SUIT_ERR_CRASH;
	}

	if (seq_exec_state->manifest != NULL) {
		for (size_t index = 0; index < seq_exec_state->manifest->components_count; index++) {
			seq_exec_state->current_components[index] = true;
		}

		return SUIT_SUCCESS;
	}

	return SUIT_ERR_MISSING_COMPONENT;
}

int suit_exec_deselect_all_components(struct suit_seq_exec_state *seq_exec_state)
{
	if (seq_exec_state == NULL) {
		return SUIT_ERR_CRASH;
	}

	if (seq_exec_state->manifest != NULL) {
		for (size_t index = 0; index < seq_exec_state->manifest->components_count; index++) {
			seq_exec_state->current_components[index] = false;
		}

		return SUIT_SUCCESS;
	}

	return SUIT_ERR_MISSING_COMPONENT;
}

int suit_seq_exec_schedule(struct suit_processor_state *state, struct suit_manifest_state *manifest, struct zcbor_string *command_sequence, enum suit_bool soft_failure, seq_exec_processor_t cmd_processor)
{
	if ((state == NULL) || (cmd_processor == NULL)) {
		return SUIT_ERR_CRASH;
	}

	if (state->seq_stack_height < SUIT_MAX_SEQ_DEPTH) {
		SUIT_DBG("Push sequence: %p\r\n", command_sequence->value);
		struct suit_seq_exec_state *seq_exec_state = &state->seq_stack[state->seq_stack_height];

		seq_exec_state->cmd_seq_str.value = command_sequence->value;
		seq_exec_state->cmd_seq_str.len = command_sequence->len;
		seq_exec_state->cmd_exec_state = SUIT_SEQ_EXEC_DEFAULT_STATE;
		seq_exec_state->exec_ptr = command_sequence->value;
		seq_exec_state->retval = SUIT_SUCCESS;
		seq_exec_state->soft_failure = soft_failure;
		seq_exec_state->current_component_idx = SUIT_MAX_NUM_COMPONENTS;
		seq_exec_state->n_commands = 0;
		seq_exec_state->current_command = 0;
		seq_exec_state->manifest = manifest;
		seq_exec_state->cmd_processor = cmd_processor;
		state->seq_stack_height += 1;

		if ((state->seq_stack_height > 1) &&
		    (state->seq_stack[state->seq_stack_height - 2].manifest == manifest)) {
			memcpy(&seq_exec_state->current_components,
				&state->seq_stack[state->seq_stack_height - 2].current_components,
				SUIT_MAX_NUM_COMPONENTS * sizeof(bool));
		} else {
			int ret = init_selected_components(seq_exec_state);
			if (ret != SUIT_SUCCESS) {
				SUIT_ERR("Unable to prepare components. Error: %d\r\n", ret);
				return ret;
			}
		}

		return SUIT_ERR_AGAIN;
	}

	SUIT_ERR("Unable to schedule sequence execution: execution stack overflow\r\n");

	return SUIT_ERR_OVERFLOW;
}

int suit_seq_exec_step(struct suit_processor_state *state)
{
	suit_command_t command = {0};
	size_t decoded_len = 0;
	struct suit_seq_exec_state *seq_exec_state = NULL;

	int ret = suit_seq_exec_state_get(state, &seq_exec_state);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	struct zcbor_string *cmd_seq_str = &(seq_exec_state->cmd_seq_str);
	ZCBOR_STATE_D(d_state, 1,
		seq_exec_state->exec_ptr,
		cmd_seq_str->len - (seq_exec_state->exec_ptr - cmd_seq_str->value),
		1, 0);

	/* If executed for the first time on the sequence - parse the CBOR list header,
	 * initialize command state and store the number of elements.
	 */
	if (cmd_seq_str->value == seq_exec_state->exec_ptr) {
		bool success = zcbor_list_start_decode(d_state);
		if (!success) {
			int ret = zcbor_peek_error(d_state);
			return ZCBOR_ERR_TO_SUIT_ERR((ret != ZCBOR_SUCCESS) ? ret : ZCBOR_ERR_UNKNOWN);
		}

		if (d_state->decode_state.indefinite_length_array) {
			return SUIT_ERR_DECODING;
		}

		/* Each command brings a single argument, passed as an element inside the array,
		   thus the number of commands is the half of the number of array elements. */
		if (d_state->elem_count % 2 == 1) {
			return SUIT_ERR_DECODING;
		}

		/* Initialize command processing state. */
		seq_exec_state->n_commands = d_state->elem_count / 2;
		seq_exec_state->current_command = 0;
		seq_exec_state->exec_ptr = d_state->payload;
		seq_exec_state->cmd_exec_state = SUIT_SEQ_EXEC_DEFAULT_STATE;

		SUIT_DBG("Run sequence with %d elements\r\n", seq_exec_state->n_commands);
	} else {
		SUIT_DBG("Continue sequence from %d element with %d elements\r\n",
			seq_exec_state->current_command,
			seq_exec_state->n_commands);
	}

	while (seq_exec_state->current_command < seq_exec_state->n_commands) {
		if (cbor_decode_SUIT_Condition(
			d_state->payload, d_state->payload_end - d_state->payload,
			&command.condition, &decoded_len) == ZCBOR_SUCCESS) {
			SUIT_DBG("%d: Condition %d found\r\n",
				seq_exec_state->current_command,
				command.condition.SUIT_Condition_choice);

			if (seq_exec_state->current_command == 0) {
				/* If there is only one component, or the internal sequence is executed,
				 * it is valid to skip the set-component-index command.
				 */
				if ((seq_exec_state->manifest->components_count != 1) && (state->seq_stack_height < 2)) {
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
				seq_exec_state->current_command,
				command.directive.SUIT_Directive_choice);

			if (seq_exec_state->current_command == 0) {
				/* If there is only one component, or the internal sequence is executed,
				 * it is valid to skip the set-component-index command.
				 */
				if ((seq_exec_state->manifest->components_count != 1) &&
				    (state->seq_stack_height < 2) &&
				    (command.directive.SUIT_Directive_choice
					!= SUIT_Directive_suit_directive_set_component_index_m_l_c)) {
					SUIT_ERR("Each sequence should begin with a set-component-index command\r\n");
					return SUIT_ERR_MANIFEST_VALIDATION;
				}
			}

			command.type = SUIT_COMMAND_DIRECTIVE;
			d_state->payload += decoded_len;
		} else {
			SUIT_DBG("%d: Unknown command found!\r\n", seq_exec_state->current_command);
			return SUIT_ERR_DECODING;
		}

		int retval = seq_exec_state->cmd_processor(state, &command);
		if (retval == SUIT_SUCCESS) {
			seq_exec_state->exec_ptr = d_state->payload;
			seq_exec_state->current_command++;
			seq_exec_state->cmd_exec_state = SUIT_SEQ_EXEC_DEFAULT_STATE;
			continue;
		}
		else if (retval == SUIT_ERR_AGAIN) {
			SUIT_DBG("%d: Partially processed command. Ptr: %p\r\n",
				seq_exec_state->current_command,
				seq_exec_state->exec_ptr);
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
	if ((state != NULL) && (state->seq_stack_height > 0)) {
		SUIT_DBG("Finalize sequence: %p\r\n", state->seq_stack[state->seq_stack_height - 1].cmd_seq_str.value);
		if ((state->seq_stack[state->seq_stack_height - 1].soft_failure == suit_bool_true) &&
		    (retval == SUIT_FAIL_CONDITION)) {
			retval = SUIT_FAIL_SOFT_CONDITION;
		}

		state->seq_stack_height -= 1;

		/* If there is a command sequence to handle the error code - pass it to the caller stack frame. */
		if (state->seq_stack_height > 0) {
			state->seq_stack[state->seq_stack_height - 1].retval = retval;

			memcpy(&state->seq_stack[state->seq_stack_height - 1].current_components,
				&state->seq_stack[state->seq_stack_height].current_components,
				SUIT_MAX_NUM_COMPONENTS * sizeof(bool));

			return SUIT_ERR_AGAIN;
		}

		return retval;
	}

	return SUIT_ERR_CRASH;
}

int suit_seq_exec_component_idx_get(struct suit_seq_exec_state *seq_exec_state, size_t *component_idx)
{
	int retval = SUIT_SUCCESS;

	if ((seq_exec_state == NULL) || (component_idx == NULL)) {
		return SUIT_ERR_CRASH;
	}

	*component_idx = seq_exec_state->current_component_idx;

	/* If executed for the first time - backup components and set the first active index. */
	if (*component_idx == SUIT_MAX_NUM_COMPONENTS) {
		retval = suit_seq_exec_component_idx_next(seq_exec_state, component_idx);
	}

	return retval;
}

int suit_seq_exec_component_idx_next(struct suit_seq_exec_state *seq_exec_state, size_t *component_idx)
{
	int retval = SUIT_SUCCESS;

	if ((seq_exec_state == NULL) || (component_idx == NULL)) {
		return SUIT_ERR_CRASH;
	}

	/* Invalidate current component index value before the loop execution. */
	*component_idx = SUIT_MAX_NUM_COMPONENTS;

	if (seq_exec_state->current_component_idx == SUIT_MAX_NUM_COMPONENTS) {
		/* If executed for the first time - backup components and set the first active index. */
		retval = backup_and_reset_components(seq_exec_state);
		if (retval != SUIT_SUCCESS) {
			return retval;
		}
	} else {
		suit_seq_exec_component_reset(seq_exec_state);

		seq_exec_state->current_components[seq_exec_state->current_component_idx] = false;
	}

	for (size_t i = 0; i < seq_exec_state->manifest->components_count; i++) {
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
		seq_exec_state->current_components[*component_idx] = true;
	} else {
		/* If executed for the last time - restore components from the backup. */
		retval = recover_components(seq_exec_state);
		if (retval != SUIT_SUCCESS) {
			return retval;
		}
		seq_exec_state->current_component_idx = SUIT_MAX_NUM_COMPONENTS;
	}

	return SUIT_SUCCESS;
}


int suit_exec_component_handle_from_idx(struct suit_seq_exec_state *seq_exec_state, size_t component_idx, suit_component_t *handle)
{
	struct suit_manifest_params *params;

	if (seq_exec_state == NULL) {
		return SUIT_ERR_CRASH;
	}

	int ret = suit_manifest_get_component_params(seq_exec_state->manifest, component_idx, &params);
	if (ret == SUIT_SUCCESS) {
		*handle = params->component_handle;
	}

	return ret;
}

int suit_exec_find_integrated_payload(struct suit_seq_exec_state *seq_exec_state, struct zcbor_string *uri, struct zcbor_string *payload)
{
	if (seq_exec_state == NULL) {
		return SUIT_ERR_CRASH;
	}

	return suit_manifest_get_integrated_payload(seq_exec_state->manifest, uri, payload);
}
