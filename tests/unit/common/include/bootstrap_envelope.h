/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef BOOTSTRAP_ENVELOPE_H__
#define BOOTSTRAP_ENVELOPE_H__

#include <suit.h>
#include <suit_processor.h>

#define ASSIGNED_COMPONENT_HANDLE 0x1E054000

/** @brief Configure the manifest processor state with empty, validated and decoded envelope
 *
 * @param  state  Manifest processor state to be modified.
 */
void bootstrap_envelope_empty(struct suit_processor_state *state);

/** @brief Reset the manifest internal step counter.
 *
 * @note Use this API to execute arbitrary command sequence.
 *
 * @param  state  Manifest processor state to be modified.
 */
void bootstrap_envelope_reset_step(struct suit_processor_state *state);

/** @brief Specify the command sequence inside the envelope
 *
 * @param  state  Manifest processor state to be modified.
 * @param  step   Step number, in which the command sequence should be executed.
 *                If set to the SUIT_NO_STEP, the shared command sequence is configured.
 * @param  seq    Structure describing command sequence encoded as ZCBOR byte string.
 */
void bootstrap_envelope_sequence(struct suit_processor_state *state, enum suit_command_sequence seq, struct zcbor_string *seq_name);

/** @brief Preconfigure components inside the SUIT processor state.
 *
 * @param  state           Manifest processor state to be modified.
 * @param  num_components  The number of components to be preconfigured.
 */
void bootstrap_envelope_components(struct suit_processor_state *state, size_t num_components);

/** @brief Preconfigure dependency components inside the SUIT processor state.
 *
 * @param  state           Manifest processor state to be modified.
 * @param  num_components  The number of dependency components to be preconfigured.
 */
void bootstrap_envelope_dependency_components(struct suit_processor_state *state, size_t num_components);

#endif /* BOOTSTRAP_ENVELOPE_H__ */