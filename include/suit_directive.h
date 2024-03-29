/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_DIRECTIVE_H__
#define SUIT_DIRECTIVE_H__

#include "suit_processor.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Set the current component indices. This decides which component(s)
 *  are affected by subsequent commands. */
int suit_directive_set_current_components(struct suit_processor_state *state, struct IndexArg_r *index_arg);

/** suit-directive-try-each */
int suit_directive_try_each(struct suit_processor_state *state,
		struct SUIT_Directive_Try_Each_Argument *try_each_arg,
		bool validate);

/** suit-directive-run-sequence */
int suit_directive_run_sequence(struct suit_processor_state *state,
		struct zcbor_string *command_sequence);

/** Copy the parameters in @p new_parameters (that are set) into the parameter
 *  list of the current component(s). */
int suit_directive_override_parameters(struct suit_processor_state *state,
		struct suit_directive_override_parameters_m_l_map_SUIT_Parameters_m *params, uint_fast32_t param_count);

/** Fetch a payload based on the configured parameters. */
int suit_directive_fetch(struct suit_processor_state *state, struct suit_manifest_params *component_params);

/** Copy a payload based on the configured parameters. */
int suit_directive_copy(struct suit_processor_state *state, struct suit_manifest_params *component_params);

/** Write a small block of data to component */
int suit_directive_write(struct suit_processor_state *state, struct suit_manifest_params *component_params);

/** Swap a payload based on the configured parameters. */
int suit_directive_swap(struct suit_processor_state *state, struct suit_manifest_params *component_params);

/** Invoke/boot a component based on the configured parameters. */
int suit_directive_invoke(struct suit_processor_state *state, struct suit_manifest_params *component_params);

/** Set the value of the parameters in @p param into the parameter list of the current component if its value is not set. */
int suit_directive_set_parameters(struct suit_processor_state *state,
		struct suit_directive_set_parameters_m_l_map_SUIT_Parameters_m *params, uint_fast32_t param_count,
		struct suit_manifest_params *component_params);

/** Process the current sequence of the dependency manifest. */
int suit_directive_process_dependency(struct suit_processor_state *state, struct suit_manifest_params *component_params);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUIT_DIRECTIVE_H__ */
