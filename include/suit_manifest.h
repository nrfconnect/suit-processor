/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_MANIFEST_H__
#define SUIT_MANIFEST_H__

#include <suit_processor.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief Assign and initialize the memory to store SUIT component parameters.
 *
 * @param[in] params  Reference to an array holding the component parameters values.
 * @param[in] count   Size of the array.
 *
 * @returns SUIT_SUCCESS if the array was successfully initialized, error code otherwise.
 */
int suit_manifest_params_init(struct suit_manifest_params *params, size_t count);

/** @brief Append a reference to a dependency manifest component to the manifest structure.
 *
 * @details This function will create a component only if the component with a given component_id
 *          is not already created.
 *
 * @param[in] manifest      Manifest structure to modify.
 * @param[in] component_id  ZCBOR string holding the component ID.
 * @param[in] prefix        ZCBOR string holding the prefix to be used within the context of this manifest.
 *                          Currently only an empty prefix is supported.
 *
 * @returns SUIT_SUCCESS if the dependency component was appended, error code otherwise.
 */
int suit_manifest_append_dependency(struct suit_manifest_state *manifest, struct zcbor_string *component_id, struct zcbor_string *prefix);

/** @brief Append a reference to a regular component to the manifest structure.
 *
 * @details This function will create a component only if the component with a given component_id
 *          is not already created.
 *
 * @param[in] manifest      Manifest structure to modify.
 * @param[in] component_id  ZCBOR string holding the component ID.
 *
 * @returns SUIT_SUCCESS if the component was appended, error code otherwise.
 */
int suit_manifest_append_component(struct suit_manifest_state *manifest, struct zcbor_string *component_id);

/** @brief Release all components, referenced by the manifest and reset the structure contents.
 *
 * @details This function will release a component through platform API only if the component
 *          is not referenced by another manifest structure.
 *
 * @param[in] manifest  Manifest structure to release.
 *
 * @returns SUIT_SUCCESS if the manifest structure was reset, error code otherwise.
 */
int suit_manifest_release(struct suit_manifest_state *manifest);

/** @brief Get the structure with SUIT component parameters for a given component index for a given manifest.
 *
 * @param[in]  manifest       Manifest structure, defining the context for the component index.
 * @param[in]  component_idx  Component index in the manifest.
 * @param[out] params         Reference to the structure with SUIT component parameters values.
 *
 * @returns SUIT_SUCCESS if the component was found and the structure was returned, error code otherwise.
 */
int suit_manifest_get_component_params(struct suit_manifest_state *manifest, size_t component_idx, struct suit_manifest_params **params);

/** @brief Get the reference to the given command sequence for a given manifest.
 *
 * @details This function will return a command sequence only if it is marked as authenticated.
 *
 * @param[in]  manifest  Manifest structure, defining the context for the command sequence.
 * @param[in]  seq_name  Name of the command sequence to return.
 * @param[out] payload   Reference to the pointer to ZCBOR string holding the sequence.
 *
 * @returns SUIT_SUCCESS if a valid, authenticated sequence was found, SUIT_ERR_UNAVAILABLE_COMMAND_SEQ if the sequence is not available,
 *                       error code otherwise.
 */
int suit_manifest_get_command_seq(struct suit_manifest_state *manifest, enum suit_command_sequence seq_name, struct zcbor_string **sequence);

/** @brief Get the reference to an integrated payload with a given URI for a given manifest.
 *
 * @param[in]  manifest  Manifest structure, in which the URI is searched for.
 * @param[in]  uri       URI of the integrated payload to find.
 * @param[out] payload   Reference to the ZCBOR string holding the integrated payload.
 *
 * @returns SUIT_SUCCESS if the integrated payload was found and the structure was returned, error code otherwise.
 */
int suit_manifest_get_integrated_payload(struct suit_manifest_state *manifest, struct zcbor_string *uri, struct zcbor_string *payload);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUIT_MANIFEST_H__ */
