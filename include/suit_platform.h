/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUIT_PLATFORM_H__
#define SUIT_PLATFORM_H__

#include "suit_types.h"
#include <stdint.h>


/** Check the provided payload against the provided digest */
int suit_plat_check_digest(enum suit_cose_alg alg_id,
		cbor_string_type_t *digest,
		cbor_string_type_t *payload);

/** Authenticate the given payload against the given signature.
 *
 *  @param[in]  alg_id  The signature verification algorithm to use.
 *  @param[in]  key_id  The key to check the signature with.
 *  @param[in]  signature  The signature to check.
 *  @param[in]  payload  The payload that is signed by the @p signature.
 */
int suit_plat_authenticate(enum suit_cose_alg alg_id, cbor_string_type_t *key_id,
		cbor_string_type_t signature, cbor_string_type_t *payload);

/** Check that the given component ID exists, is valid, and is authorized.
 *
 *  If so, return a component handle for it.
 *
 *  @param[in]  parts  The component ID as an array of strings.
 *  @param[in]  num_parts  The number of strings in @p parts.
 *  @param[in]  key_ids  The keys the current manifest was authenticated
 *                       against. This is to check that the manifest is
 *                       authorized to handle the given component ID.
 *  @param[in]  num_key_ids  The number of members in the @p key_ids list.
 *  @param[in]  properties  The properties we need the component to support.
 *  @param[out] component_handle  A reference for use with other functions in
 *                                this API, instead of always passing the
 *                                @p parts.
 */
int suit_plat_get_component_handle(cbor_string_type_t *parts, size_t num_parts,
		cbor_string_type_t **key_ids, size_t num_key_ids,
		enum suit_component_properties properties,
		suit_component_t *component_handle);

/** Return current slot for this component.
 *
 *  @param[in]  component_handle  Reference to the component.
 *  @param[out] slot  The current slot of the component.
 */
int suit_plat_get_component_slot(suit_component_t component_handle,
		unsigned int *slot);

/** Return the properties of the given component. */
int suit_plat_get_component_properties(suit_component_t component_handle,
		enum suit_component_properties *properties);

/** Check the provided payload against the provided digest */
int suit_plat_check_image_match(enum suit_cose_alg alg_id,
		cbor_string_type_t *digest, size_t image_size,
		suit_component_t image_handle);

/** Check whether the given vendor ID applies to the given component. */
int suit_plat_check_vid(cbor_string_type_t *vid_uuid,
		suit_component_t component_handle);

/** Check whether the given class ID applies to the given component. */
int suit_plat_check_cid(cbor_string_type_t *cid_uuid,
		suit_component_t component_handle);

/** Check whether the given device ID applies to the given component. */
int suit_plat_check_did(cbor_string_type_t *did_uuid,
		suit_component_t component_handle);

/** Check that the provided sequence number is recent enough. */
int suit_plat_check_sequence_num(unsigned int seq_num);

/** Update the sequence number against which new manifests will be checked. */
int suit_plat_commit_sequence_num(unsigned int seq_num);

/** Check that the given fetch operation can be performed. */
int suit_plat_check_fetch(suit_component_t dst_handle, cbor_string_type_t uri);

/** Fetch the payload from the given @p uri in to @p dst. */
int suit_plat_fetch(suit_component_t dst_handle, cbor_string_type_t uri);

/** Check that the given copy operation can be performed. */
int suit_plat_check_copy(suit_component_t dst_handle, suit_component_t src_handle);

/** Copy a payload from @p src_handle to @p dst_handle. */
int suit_plat_copy(suit_component_t dst_handle, suit_component_t src_handle);

/** Check that the given swap operation can be performed. */
int suit_plat_check_swap(suit_component_t dst_handle, suit_component_t src_handle);

/** Swap a payload from @p src_handle to @p dst_handle. */
int suit_plat_swap(suit_component_t dst_handle, suit_component_t src_handle);

/** Check that the given run operation can be performed. */
int suit_plat_check_run(suit_component_t image_handle, cbor_string_type_t *run_args);

/** Run the given image. */
int suit_plat_run(suit_component_t image_handle, cbor_string_type_t *run_args);

/** File a report on a command result. */
int suit_plat_report(unsigned int rep_policy, struct suit_report *report);

/** Store an opaque state struct to non-volatile memory.
 *
 *  The SUIT operation must be able to cope with random resets, both unexpected
 *  resets and resets required by the firmware update procedures.
 */
int suit_plat_set_state(unsigned int state_id, unsigned char *state, size_t state_len);

/** Retrieve an opaque state struct from non-volatile memory. */
int suit_plat_get_state(unsigned int state_id, unsigned char *state, size_t state_len);

#endif /* SUIT_PLATFORM_H__ */
