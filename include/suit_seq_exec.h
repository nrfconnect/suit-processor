/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_SEQ_EXEC_H__
#define SUIT_SEQ_EXEC_H__

#include "suit_processor.h"
#include <manifest_types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief Schedule the command sequence for execution.
 *
 * @note This function only schedules the execution and returns the SUIT_ERR_AGAIN afterwards.
 *       The command execution is done from the suit_seq_exec_step(..) calls.
 *
 * @param[in]  state             The SUIT processor state to modify.
 * @param[in]  manifest          Reference to the manifest structure, for which the sequence will be executed.
 * @param[in]  command_sequence  Command sequence to be scheduled.
 * @param[in]  soft_failure      The initial value of the suit-parameter-soft-failure for the sequence.
 * @param[in]  cmd_processor     Processor to execute commands in the sequence.
 *
 * @retval SUIT_ERR_AGAIN     If the command has been successfully scheduled.
 * @retval SUIT_ERR_OVERFLOW  If the command execution stack was too small to schedule the new sequence.
 */
int suit_seq_exec_schedule(struct suit_processor_state *state, struct suit_manifest_state *manifest, struct zcbor_string *command_sequence, enum suit_bool soft_failure, seq_exec_processor_t cmd_processor);

/** @brief Execute the scheduled command sequence.
 *
 * @note This function only executes the sequence as long as it does not require execution of nested sequence.
 *       In such case, the API returns SUIT_ERR_AGAIN and should be called again to continue the execution.
 *
 * @param[in]  state  The SUIT processor state to modify.
 *
 * @retval SUIT_SUCCESS       If the command has finished.
 * @retval SUIT_ERR_AGAIN     If the command has not completed.
 * @retval SUIT_ERR_OVERFLOW  If the command execution stack was too small to execute the sequence.
 */
int suit_seq_exec_step(struct suit_processor_state *state);

/** @brief Get the current command sequence execution state.
 *
 * @param[in]   state           The SUIT processor state to be used.
 * @param[out]  seq_exec_state  Pointer to the reference that will be set to the current command sequence.
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

/** @brief Get the current component index, on which the sequence will be executed.
 *
 * @note This API is used to implement the loop, which executes nested command sequence on
 *       a list of components in a on-by-one order.
 * @note If the API is called for the first time on an execution context, it will create a backup
 *       of the list of currently selected components and select the first component index afterwards.
 *
 * @param[in]   seq_exec_state  The SUIT processor execution state to be used.
 * @param[out]  component_idx   Pointer to the variable, that will be set with the current component index value.
 *
 * @returns SUIT_SUCCESS if the current component was successfully returned, error code otherwise.
 */
int suit_seq_exec_component_idx_get(struct suit_seq_exec_state *seq_exec_state, size_t *component_idx);

/** @brief Switch to the next component index, on which the sequence will be executed.
 *
 * @note This API is used to implement the loop, which executes nested command sequence on
 *       a list of components in a on-by-one order.
 * @note If the list of selected components is exhausted, the component_idx will be set to SUIT_MAX_NUM_COMPONENTS
 *       and this API will automatically restore the list of selected components from the backup.
 *
 * @param[in]   seq_exec_state  The SUIT processor execution state to be used.
 * @param[out]  component_idx   Pointer to the variable, that will be set with the current component index value.
 *
 * @returns SUIT_SUCCESS if the current component was successfully returned, error code otherwise.
 */
int suit_seq_exec_component_idx_next(struct suit_seq_exec_state *seq_exec_state, size_t *component_idx);

/** @brief Add a component specified by an index to the list of selected components.
 *
 * @param[in]  seq_exec_state  The SUIT processor execution state to be used.
 * @param[in]  index           The component index of the current manifest.
 *
 * @returns SUIT_SUCCESS if the component was selected, error code otherwise.
 */
int suit_exec_select_component_idx(struct suit_seq_exec_state *seq_exec_state, size_t index);

/** @brief Add all components from the current manifest to the list of selected components.
 *
 * @param[in]  seq_exec_state  The SUIT processor execution state to be used.
 *
 * @returns SUIT_SUCCESS if components were selected, error code otherwise.
 */
int suit_exec_select_all_components(struct suit_seq_exec_state *seq_exec_state);

/** @brief Remove all components from the current manifest from the list of selected components.
 *
 * @param[in]  seq_exec_state  The SUIT processor execution state to be used.
 *
 * @returns SUIT_SUCCESS if components were removed, error code otherwise.
 */
int suit_exec_deselect_all_components(struct suit_seq_exec_state *seq_exec_state);

/** @brief Resolve the component index of the current manifest to the component handle.
 *
 * @param[in]   seq_exec_state  The SUIT processor execution state to be used.
 * @param[in]   component_idx   The component index of the current manifest.
 * @param[out]  handle          Pointer to the variable, that will be set with the resolved component handle.
 *
 * @returns SUIT_SUCCESS if the component handle was found and returned, error code otherwise.
 */
int suit_exec_component_handle_from_idx(struct suit_seq_exec_state *seq_exec_state, size_t component_idx, suit_component_t *handle);

/** @brief Look for URI in the list of integrated payload of the current manifest.
 *
 * @param[in]   seq_exec_state  The SUIT processor execution state to be used.
 * @param[in]   uri             URI to look for.
 * @param[out]  payload         Pointer to the variable, that will be set with the reference and size of the integrated payload.
 *
 * @returns SUIT_SUCCESS if the integrated payload was found and returned, error code otherwise.
 */
int suit_exec_find_integrated_payload(struct suit_seq_exec_state *seq_exec_state, struct zcbor_string *uri, struct zcbor_string *payload);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUIT_SEQ_EXEC_H__ */
