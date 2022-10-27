/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUIT_PLATFORM_INTERNAL_H__
#define SUIT_PLATFORM_INTERNAL_H__

#include <stdint.h>
#include "suit_types.h"


/** Initialize the component implementation and associate it with the component handle. */
typedef int (*suit_component_init_t)(suit_component_t handle);

/** Release the given component from the component implementation. */
typedef void (*suit_component_release_t)(suit_component_t handle);

/** Read the contents of an active slot */
typedef int (*suit_component_read_t)(suit_component_t handle, size_t offset, uint8_t *buf, size_t *len);

/** If component supports slots, write contents into the inactive slot. Overwrite the active slot otherwise. */
typedef int (*suit_component_write_t)(suit_component_t handle, size_t offset, uint8_t *buf, size_t len);

/** Start executing the code stored inside the component. */
typedef int (*suit_component_run_t)(suit_component_t handle, struct zcbor_string *run_args);

/** Get the address for direct read operations and return the number of bytes available. */
typedef size_t (*suit_component_read_address_t)(suit_component_t handle, uint8_t **read_address);

/** Check if the vendor ID matches with the component. */
typedef int (*suit_component_check_vid_t)(suit_component_t handle, struct zcbor_string *vid_uuid);

/** Check if the class ID matches with the component. */
typedef int (*suit_component_check_cid_t)(suit_component_t handle, struct zcbor_string *cid_uuid);

/** Check if the device ID matches with the component. */
typedef int (*suit_component_check_did_t)(suit_component_t handle, struct zcbor_string *did_uuid);

/** Check if the slot number matches with the active slot. */
typedef int (*suit_component_check_slot_t)(suit_component_t handle, unsigned int slot);

/** Fetch the given integrated payload into the component. */
typedef int (*suit_component_fetch_integrated_t)(suit_component_t handle, struct zcbor_string *payload);

/** Structure that holds the component implementation functions.
 *
 * @note If component does not support certain functions, it should set
 *       the corresponding field to NULL.
 */
struct suit_component_impl {
	suit_component_init_t init;
	suit_component_release_t release;

	suit_component_read_t read;
	suit_component_write_t write;
	suit_component_run_t run;
	suit_component_read_address_t read_address;

	suit_component_check_vid_t check_vid;
	suit_component_check_cid_t check_cid;
	suit_component_check_did_t check_did;
	suit_component_check_slot_t check_slot;
	suit_component_fetch_integrated_t fetch_integrated;
};

struct suit_plat_component {
	struct suit_component_impl *impl; ///! The pointer to the implementation to use to interface with the component.
	void * impl_data; ///! Slot to store the implementation-specific private data.
	struct zcbor_string *component_id; ///! Slot to store the component ID. The implementation may use it instead of building full internal context.
	struct zcbor_string **key_ids; ///! Slot to store the keys that authenticated the component.
	size_t num_key_ids; ///! Length of the key array.
};

/** Return the implementation, selected to handle the component. */
const struct suit_component_impl * suit_plat_component_impl_get(suit_component_t handle);

/** Set the pointer to the implementation-specific data. */
int suit_plat_component_impl_data_set(suit_component_t handle, void *impl_data);

/** Return the pointer to the implementation-specific data. */
int suit_plat_component_impl_data_get(suit_component_t handle, void **impl_data);

/** Return the full component ID associated with the component. */
int suit_plat_component_id_get(suit_component_t handle, struct zcbor_string **component_id);

/** Return the list of keys, used to authenticate the component. */
size_t suit_plat_component_keys_get(suit_component_t handle, struct zcbor_string ***key_ids);


/** Select the component implementation, based on component ID.
 *
 *  @param[in]  component_id  The CBOR-encoded component identifier.
 *  @param[in]  key_ids  The keys the current manifest was authenticated
 *			 against. This is to check that the manifest is
 *			 authorized to handle the given component ID.
 *  @param[in]  num_key_ids  The number of members in the @p key_ids list.
 *  @param[in]  component_handle  Assigned component reference for use with other functions.
 */
struct suit_component_impl * suit_component_select_impl(struct zcbor_string *component_id,
		struct zcbor_string *key_ids[SUIT_MAX_NUM_SIGNERS], size_t num_key_ids,
		suit_component_t component_handle);

#endif /* SUIT_PLATFORM_INTERNAL_H__ */
