/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUIT_PLATFORM_INTERNAL_H__
#define SUIT_PLATFORM_INTERNAL_H__

#include "suit_types.h"


enum suit_component_mode {
	suit_comp_writable = 1,
	suit_comp_readable = 2,
	suit_comp_runnable = 4,
};

/** Read the contents of an active slot */
typedef int (*suit_component_read_t)(suit_component_t handle, size_t offset, uint8_t *buf, size_t *len);
/** If component supports slots, write contents into the inactive slot. Overwrite the active slot otherwise. */
typedef int (*suit_component_write_t)(suit_component_t handle, size_t offset, uint8_t *buf, size_t len);
/** Start executing the code inside the component. */
typedef int (*suit_component_run_t)(suit_component_t handle, struct zcbor_string *run_args);
/** Get the address for direct read operations and return the number of bytes available. */
typedef size_t (*suit_component_read_address_t)(suit_component_t handle, uint8_t **read_address);

struct suit_component_driver {
	suit_component_read_t read;
	suit_component_write_t write;
	suit_component_run_t run;
	suit_component_read_address_t read_address; /*  Set to NULL if direct reads are not supported. */
};

struct suit_component_properties {
	suit_component_t component_handle;

	struct zcbor_string *vid_uuid; ///! The assigned VID UUID of the component.
	struct zcbor_string *cid_uuid; ///! The assigned CID UUID of the component.
	struct zcbor_string *did_uuid; ///! The assigned DID UUID of the component.

	enum suit_component_mode mode; ///! The RWX mode of the current slot of the component.
	size_t write_size; ///! The capacity of the current slot.
	unsigned int slot; ///! The current slot number.
	struct suit_component_driver * driver; ///! The pointer to the driver to use to interface with the component.
};


/** Return the properties of the given component. */
int suit_plat_get_component_properties(suit_component_t component_handle,
		struct suit_component_properties *properties);

#endif /* SUIT_PLATFORM_INTERNAL_H__ */
