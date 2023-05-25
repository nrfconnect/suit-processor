/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_PLATFORM_H__
#define SUIT_PLATFORM_H__

#include "suit_types.h"
#include <stdint.h>


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


/** Reset all loaded component properties and handles assigned to them. */
void suit_plat_reset_components(void);

/** Check the provided payload against the provided digest */
int suit_plat_check_digest(enum suit_cose_alg alg_id,
		struct zcbor_string *digest,
		struct zcbor_string *payload);

/** Authenticate the given payload against the given signature.
 *
 *  @param[in]  alg_id  The signature verification algorithm to use.
 *  @param[in]  key_id  The key to check the signature with.
 *  @param[in]  signature  The signature to check.
 *  @param[in]  data  The data that is signed by the @p signature.
 */
int suit_plat_authenticate(enum suit_cose_alg alg_id, struct zcbor_string *key_id,
		struct zcbor_string *signature, struct zcbor_string *data);

/** Check that the given component ID exists, is valid, and is authorized.
 *
 *  If so, create and return a component handle for it.
 *
 *  @param[in]  component_id  The CBOR-encoded component identifier.
 *  @param[in]  key_ids  The keys the current manifest was authenticated
 *                       against. This is to check that the manifest is
 *                       authorized to handle the given component ID.
 *  @param[in]  num_key_ids  The number of members in the @p key_ids list.
 *  @param[out] component_handle  A reference for use with other functions in
 *                                this API, instead of always passing the
 *                                @p parts.
 */
int suit_plat_create_component_handle(struct zcbor_string *component_id,
		struct zcbor_string *key_ids[SUIT_MAX_NUM_SIGNERS], size_t num_key_ids,
		suit_component_t *component_handle);

/** Check the provided payload against the provided digest */
int suit_plat_check_image_match(enum suit_cose_alg alg_id,
		struct zcbor_string *digest, size_t image_size,
		suit_component_t image_handle);

/** Check whether the given slot is active for the given component. */
int suit_plat_check_slot(suit_component_t component_handle, unsigned int slot);

/** Check whether the given vendor ID applies to the given component. */
int suit_plat_check_vid(struct zcbor_string *vid_uuid,
		suit_component_t component_handle);

/** Check whether the given class ID applies to the given component. */
int suit_plat_check_cid(struct zcbor_string *cid_uuid,
		suit_component_t component_handle);

/** Check whether the given device ID applies to the given component. */
int suit_plat_check_did(struct zcbor_string *did_uuid,
		suit_component_t component_handle);

/** Check that the provided sequence number is recent enough. */
int suit_plat_check_sequence_num(unsigned int seq_num);

/** Update the sequence number against which new manifests will be checked. */
int suit_plat_commit_sequence_num(unsigned int seq_num);

/** Check that the given fetch operation can be performed. */
int suit_plat_check_fetch(suit_component_t dst_handle, struct zcbor_string *uri);

/** Fetch the payload from the given @p uri into @p dst. */
int suit_plat_fetch(suit_component_t dst_handle, struct zcbor_string *uri);

/** Check that the given fetch operation can be performed. */
int suit_plat_check_fetch_integrated(suit_component_t dst_handle, struct zcbor_string *payload);

/** Fetch the given integrated payload into @p dst. */
int suit_plat_fetch_integrated(suit_component_t dst_handle, struct zcbor_string *payload);

/** Check that the given copy operation can be performed. */
int suit_plat_check_copy(suit_component_t dst_handle, suit_component_t src_handle);

/** Copy a payload from @p src_handle to @p dst_handle. */
int suit_plat_copy(suit_component_t dst_handle, suit_component_t src_handle);

/** Check that the given swap operation can be performed. */
int suit_plat_check_swap(suit_component_t dst_handle, suit_component_t src_handle);

/** Swap a payload from @p src_handle to @p dst_handle. */
int suit_plat_swap(suit_component_t dst_handle, suit_component_t src_handle);

/** Check that the given invoke operation can be performed. */
int suit_plat_check_invoke(suit_component_t image_handle, struct zcbor_string *invoke_args);

/** Invoke the given image. */
int suit_plat_invoke(suit_component_t image_handle, struct zcbor_string *invoke_args);

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
