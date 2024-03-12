/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_PLATFORM_H__
#define SUIT_PLATFORM_H__

#include "suit_types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef SUIT_DBG
#define SUIT_DBG(...)
#endif
#ifndef SUIT_DBG_RAW
#define SUIT_DBG_RAW(...)
#endif
#ifndef SUIT_INF
#define SUIT_INF(...)
#endif
#ifndef SUIT_WRN
#define SUIT_WRN(...)
#endif
#ifndef SUIT_ERR
#define SUIT_ERR(...)
#endif

/** @brief Check the provided payload against the provided digest.
 *
 * @param[in] alg_id   The digest verification algorithm to use.
 * @param[in] digest   Expected diest value.
 * @param[in] payload  The payload to verify.
 *
 * @returns SUIT_SUCCESS if the digest matches, error code otherwise.
 */
int suit_plat_check_digest(enum suit_cose_alg alg_id,
		struct zcbor_string *digest,
		struct zcbor_string *payload);

/** @brief Authenticate the given manifest against the given signature.
 *
 * @param[in] manifest_component_id  The manifest component ID, identifying
 *                                   the type of manifest in the system.
 * @param[in] alg_id                 The signature verification algorithm to use.
 * @param[in] key_id                 The key to check the signature with.
 * @param[in] signature              The signature to check.
 * @param[in] data                   The data that is signed by the @p signature.
 *
 * @returns SUIT_SUCCESS if the signature is correct, error code otherwise.
 */
int suit_plat_authenticate_manifest(struct zcbor_string *manifest_component_id,
		enum suit_cose_alg alg_id, struct zcbor_string *key_id,
		struct zcbor_string *signature, struct zcbor_string *data);

/** @brief Check that the provided manifest is allowed to be unsigned.
 *
 * @param[in] manifest_component_id  The manifest component ID, identifying
 *                                   the type of manifest in the system.
 *
 * @returns SUIT_SUCCESS if the manifest can be processed, error code otherwise.
 */
int suit_plat_authorize_unsigned_manifest(struct zcbor_string *manifest_component_id);

/** @brief Check that the given component ID exists, is valid, and is authorized.
 *         If so, create and return a component handle for it.
 *
 * @param[in]  component_id      The CBOR-encoded component identifier.
 * @param[out] component_handle  A reference for use with other functions in
 *                               this API, instead of always passing the
 *                               @p parts.
 *
 * @returns SUIT_SUCCESS if the component handle was created, error code otherwise.
 */
int suit_plat_create_component_handle(struct zcbor_string *component_id,
		suit_component_t *handle);

/** @brief Release loaded component properties and handles assigned to them.
 *
 * @param[in] component_handle  The platform-specific component handle value.
 *
 * @returns SUIT_SUCCESS if the component handle was released, error code otherwise.
 */
int suit_plat_release_component_handle(suit_component_t handle);

/** @brief Check the provided payload against the provided digest.
 *
 * @param[in] handle      A reference to the checked component.
 * @param[in] alg_id      The digest verification algorithm to use.
 * @param[in] digest      Expected diest value.
 *
 * @returns SUIT_SUCCESS if the image digest matches, error code otherwise.
 */
int suit_plat_check_image_match(suit_component_t handle,
		enum suit_cose_alg alg_id, struct zcbor_string *digest);

/** @brief Check the provided payload against the component value.
 *
 * @param[in] handle   A reference to the checked component.
 * @param[in] content  A reference to the buffer, describing the content.
 *
 * @returns SUIT_SUCCESS if the component contents matches, error code otherwise.
 */
int suit_plat_check_content(suit_component_t handle, struct zcbor_string *content);

/** @brief Check whether the given slot is active for the given component.
 *
 * @note This API is currently not supported.
 *
 * @param[in] handle  A reference to the checked component.
 * @param[in] slot    Expected active slot number.
 *
 * @returns SUIT_SUCCESS if the component slot matches, error code otherwise.
 */
int suit_plat_check_slot(suit_component_t handle, unsigned int slot);

/** @brief Check whether the given vendor ID applies to the given component.
 *
 * @param[in] handle    A reference to the checked component.
 * @param[in] vid_uuid  A reference to the expected UUID value.
 *
 * @returns SUIT_SUCCESS if the vendor UUID matches, error code otherwise.
 */
int suit_plat_check_vid(suit_component_t handle, struct zcbor_string *vid_uuid);

/** @brief Check whether the given class ID applies to the given component.
 *
 * @param[in] handle    A reference to the checked component.
 * @param[in] cid_uuid  A reference to the expected UUID value.
 *
 * @returns SUIT_SUCCESS if the class UUID matches, error code otherwise.
 */
int suit_plat_check_cid(suit_component_t handle, struct zcbor_string *cid_uuid);

/** @brief Check whether the given device ID applies to the given component.
 *
 * @param[in] handle    A reference to the checked component.
 * @param[in] did_uuid  A reference to the expected UUID value.
 *
 * @returns SUIT_SUCCESS if the device UUID matches, error code otherwise.
 */
int suit_plat_check_did(suit_component_t handle, struct zcbor_string *did_uuid);

/** @brief Check that the provided sequence number for a given manifest is recent enough.
 *
 * @param[in] seq_name               The currently processed SUIT manifest sequence.
 * @param[in] manifest_component_id  The manifest component ID, identifying
 *                                   the type of manifest in the system.
 * @param[in] seq_num                The manifest sequence number value.
 *
 * @returns SUIT_SUCCESS if the sequence is allowed to be executed with a given sequence number, error code otherwise.
 */
int suit_plat_authorize_sequence_num(enum suit_command_sequence seq_name, struct zcbor_string *manifest_component_id, unsigned int seq_num);

/** Check that the provided component ID is supported by the given manifest.
 *
 * @param[in] manifest_component_id  The manifest component ID, identifying
 *                                   the type of manifest in the system.
 * @param[in] component_id           The CBOR-encoded component identifier to verify.
 *
 * @returns SUIT_SUCCESS if the component ID is allowed, error code otherwise.
 */
int suit_plat_authorize_component_id(struct zcbor_string *manifest_component_id, struct zcbor_string *component_id);

/** @brief Fetch the payload from the given @p uri into @p dst.
 *
 * @param[in] dst_handle  A reference to the destination component.
 * @param[in] uri         A reference to the buffer, containing the URI to be fetched.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_fetch(suit_component_t dst_handle, struct zcbor_string *uri);

/** @brief Fetch the given integrated payload into @p dst.
 *
 * @param[in] dst_handle  A reference to the destination component.
 * @param[in] payload     A reference to the buffer, describing the fetched content.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_fetch_integrated(suit_component_t dst_handle, struct zcbor_string *payload);

/** @brief Copy a payload from @p src_handle to @p dst_handle.
 *
 * @param[in] dst_handle  A reference to the destination component.
 * @param[in] src_handle  A reference to the source component.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_copy(suit_component_t dst_handle, suit_component_t src_handle);

/** @brief Swap a payload from @p src_handle to @p dst_handle.
 *
 * @param[in] dst_handle  A reference to the destination component.
 * @param[in] src_handle  A reference to the source component.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_swap(suit_component_t dst_handle, suit_component_t src_handle);

/** @brief Write a payload from @p content to @p dst_handle.
 *
 * @param[in] dst_handle  A reference to the destination component.
 * @param[in] content     A reference to the buffer, describing the content.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_write(suit_component_t dst_handle, struct zcbor_string *content);

/** @brief Invoke the given image.
 *
 * @param[in] image_handle  A reference to the invoked component.
 * @param[in] invoke_args   A reference to the buffer with platform-specific invoke arguments.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_invoke(suit_component_t image_handle, struct zcbor_string *invoke_args);

/** @brief File a report on a command result.
 *
 * @note This API is currently not supported.
 *
 * @param[in] rep_policy  Reporting policy.
 * @param[in] report      Reference to the structure with the SUIT report to store.
 *
 * @returns SUIT_SUCCESS if the report was recorded, error code otherwise.
 */
int suit_plat_report(unsigned int rep_policy, struct suit_report *report);

/** @brief A callback function, informing about the sequence completion.
 *
 * @param[in] seq_name               The finished SUIT manifest sequence.
 * @param[in] manifest_component_id  The manifest component ID, identifying
 *                                   the type of manifest in the system.
 * @param[in] envelope_str           A reference to the SUIT envelope that was processed.
 * @param[in] envelope_len           The length of the processed envelope.
 *
 * @returns SUIT_SUCCESS if the callback succeeds, error code otherwise.
 */
int suit_plat_sequence_completed(enum suit_command_sequence seq_name, struct zcbor_string *manifest_component_id, const uint8_t *envelope_str, size_t envelope_len);

/** @brief Return a pointer to the SUIT envelope, stored inside the component.
 *
 * @param[in]  component_handle  A reference to the component, describing the SUIT envelope.
 * @param[out] envelope_str      A reference to the SUIT envelope, represented by the component.
 * @param[out] envelope_len      The length of the returned envelope.
 *
 * @returns SUIT_SUCCESS if the manifest was returned, error code otherwise.
 */
int suit_plat_retrieve_manifest(suit_component_t component_handle, const uint8_t **envelope_str, size_t *envelope_len);

/** @brief Set the current image size value, stored inside the platform component metadata.
 *
 * @param[in]  handle  A reference to the component, describing the SUIT envelope.
 * @param[in]  size    The component size, set by the manifest command.
 *
 * @returns SUIT_SUCCESS if the size was set, error code otherwise.
 */
int suit_plat_override_image_size(suit_component_t handle, size_t size);

/** @brief Authorize execution of the given sequence of the dependency manifest.
 *
 * @details This function allow to restrict different manifest topologies, depending on the execution context.
 *          For example, it may be allowed to include a manifest as dependency for the update procedure, but prohibited
 *          to boot it's components.
 *
 * @param[in]  parent_component_id  Parent manifest component ID.
 * @param[in]  child_component_id   Child manifest component ID.
 * @param[in]  seq_name             The sequence to authorize.
 *
 * @returns SUIT_SUCCESS if the manifest was returned, error code otherwise.
 */
int suit_plat_authorize_process_dependency(struct zcbor_string *parent_component_id, struct zcbor_string *child_component_id, enum suit_command_sequence seq_name);

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
/** @brief Check that the given fetch operation can be performed.
 *
 * @param[in] dst_handle  A reference to the destination component.
 * @param[in] uri         A reference to the buffer, containing the URI to be fetched.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_check_fetch(suit_component_t dst_handle, struct zcbor_string *uri);

/** @brief Check that the given fetch of integrated payload can be performed.
 *
 * @param[in] dst_handle  A reference to the destination component.
 * @param[in] payload     A reference to the buffer, describing the fetched content.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_check_fetch_integrated(suit_component_t dst_handle, struct zcbor_string *payload);

/** @brief Check that the given copy operation can be performed.
 *
 * @param[in] dst_handle  A reference to the destination component.
 * @param[in] src_handle  A reference to the source component.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_check_copy(suit_component_t dst_handle, suit_component_t src_handle);

/** @brief Check that the given swap operation can be performed.
 *
 * @param[in] dst_handle  A reference to the destination component.
 * @param[in] src_handle  A reference to the source component.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_check_swap(suit_component_t dst_handle, suit_component_t src_handle);

/** @brief Check that the given invoke operation can be performed.
 *
 * @param[in] dst_handle  A reference to the destination component.
 * @param[in] content     A reference to the buffer, describing the content.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_check_write(suit_component_t dst_handle, struct zcbor_string *content);

/** @brief Check that the given invoke operation can be performed.
 *
 * @param[in] image_handle  A reference to the invoked component.
 * @param[in] invoke_args   A reference to the buffer with platform-specific invoke arguments.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_plat_check_invoke(suit_component_t image_handle, struct zcbor_string *invoke_args);

#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUIT_PLATFORM_H__ */
