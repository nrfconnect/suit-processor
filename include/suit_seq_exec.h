/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_SEQ_EXEC_H__
#define SUIT_SEQ_EXEC_H__

#include "suit_types.h"
#include <manifest_types.h>


/** @brief Type of the command to be handled. */
enum command_type {
	SUIT_COMMAND_INVALID,
	SUIT_COMMAND_CONDITION,
	SUIT_COMMAND_DIRECTIVE,
};

/** @brief Single SUIT command structure. */
typedef struct {
	enum command_type type;
	union {
		struct SUIT_Condition_ condition;
		struct SUIT_Directive_ directive;
	};
} suit_command_t;

/** @brief Single command handler function prototype.
 *
 * @param[in] state    The SUIT processor state.
 * @param[in] command  The pointer to the command structure to execute.
 */
typedef int (*seq_exec_processor_t)(struct suit_processor_state *state, suit_command_t *command);


/** @brief Schedule the command sequence for execution.
 *
 * @note This function only schedules the execution and returns the SUIT_ERR_AGAIN aftrewards.
 *       The command execution is done from the suit_run_command_sequence(..) calls.
 *
 * @param[in]  state             The SUIT processor state to modify.
 * @param[in]  command_sequence  Command sequence to be scheduled.
 * @param[in]  soft_failure      The initial value of the suit-parameter-soft-failure for the sequence.
 *
 * @retval SUIT_ERR_AGAIN     If the command has been successfully scheduled.
 * @retval SUIT_ERR_OVERFLOW  If the command execution stack was too small to schedule the new sequence.
 */
int suit_seq_exec_schedule(struct suit_processor_state *state, struct zcbor_string *command_sequence, enum suit_bool soft_failure);

/** @brief Execute the scheduled command sequence.
 *
 * @note This function only executes the sequence as long as it does not require execution of nested sequence.
 *       In such case, the API returns SUIT_ERR_AGAIN and should be called again to continue the execution.
 *
 * @param[in]  state             The SUIT processor state to modify.
 * @param[in]  command_sequence  Command sequence to be scheduled.
 * @param[in]  soft_failure      The initial value of the suit-parameter-soft-failure for the sequence.
 *
 * @retval SUIT_SUCCESS       If the command has finished.
 * @retval SUIT_ERR_AGAIN     If the command has not completed.
 * @retval SUIT_ERR_OVERFLOW  If the command execution stack was too small to execute the sequence.
 */
int suit_seq_exec_step(struct suit_processor_state *state, seq_exec_processor_t cmd_processor);

/** @brief Get the current command sequence execution state.
 *
 * @param[in]   state
 * @param[out]  seq_exec_state
 *
 * @retval  SUIT_SUCCESS    If the state was successfully passed through the pointer.
 * @retval  SUIT_ERR_CRASH  If the pointer is invalid or there was no state to be returned (empty stack).
 */
int suit_seq_exec_state_get(struct suit_processor_state *state, struct suit_seq_exec_state **seq_exec_state);

/** @brief Pop a single element from the execution stack and pass error code to the caller.
 *
 * @param[in]  state   The SUIT processor state to be modified.
 * @param[in]  retval  Error code to be passed to the caller.
 *
 * @returns If the execution status was passed to the caller and the execution stack is not finished,
 *          the SUIT_ERR_AGAIN error code is returnded.
 *          If the execution was finished, the result of all sequences is returned.
 */
int suit_seq_exec_finalize(struct suit_processor_state *state, int retval);

/** @brief Reset the list of active components and select the current one.
 *
 * @param[in]  state   The SUIT processor state to be modified.
 */
void suit_seq_exec_component_reset(struct suit_processor_state *state);

/** @brief Get the current component index, on which the sequence will be executed.
 *
 * @note This API is used to implement the loop, which executes nested command sequence on
 *       a list of components in a on-by-one order.
 * @note If the API is called for the first time on an execution context, it will create a backup
 *       of the list of currently selected components and select the first component index afterwards.
 *
 * @param[in]   state          The SUIT processor state to be used.
 * @param[out]  component_idx  Pointer to the variable, that will be set with the current component index value.
 *
 * @returns SUIT_SUCCESS if the current component was successfully returned, error code otherwise.
 */
int suit_seq_exec_component_idx_get(struct suit_processor_state *state, size_t *component_idx);

/** @brief Switch to the next component index, on which the sequence will be executed.
 *
 * @note This API is used to implement the loop, which executes nested command sequence on
 *       a list of components in a on-by-one order.
 * @note If the list of selected components is exhausted, the component_idx will be set to SUIT_MAX_NUM_COMPONENTS
 *       and this API will automatically restore the list of selected components from the backup.
 *
 * @param[in]   state          The SUIT processor state to be used.
 * @param[out]  component_idx  Pointer to the variable, that will be set with the current component index value.
 *
 * @returns SUIT_SUCCESS if the current component was successfully returned, error code otherwise.
 */
int suit_seq_exec_component_idx_next(struct suit_processor_state *state, size_t *component_idx);

#endif /* SUIT_SEQ_EXEC_H__ */
