/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <suit_platform.h>
#include <suit_platform_internal.h>


/** Check whether the given slot is active for the given component. */
int suit_plat_check_slot(suit_component_t component_handle, unsigned int slot)
{
	struct suit_component_properties props;
	int err = suit_plat_get_component_properties(component_handle, &props);

	if (err != SUIT_SUCCESS) {
		return err;
	}

	if (props.slot == slot) {
		return SUIT_SUCCESS;
	} else {
		return SUIT_FAIL_CONDITION;
	}
}

/** Check whether the given vendor ID applies to the given component. */
int suit_plat_check_vid(struct zcbor_string *vid_uuid,
		suit_component_t component_handle)
{
	struct suit_component_properties properties;
	int ret;

	ret = suit_plat_get_component_properties(component_handle, &properties);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	return (suit_compare_zcbor_strings(properties.vid_uuid, vid_uuid) ? SUIT_SUCCESS : SUIT_FAIL_CONDITION);
}

/** Check whether the given class ID applies to the given component. */
int suit_plat_check_cid(struct zcbor_string *cid_uuid,
		suit_component_t component_handle)
{
	struct suit_component_properties properties;
	int ret;

	ret = suit_plat_get_component_properties(component_handle, &properties);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	return (suit_compare_zcbor_strings(properties.cid_uuid, cid_uuid) ? SUIT_SUCCESS : SUIT_FAIL_CONDITION);
}

/** Check whether the given device ID applies to the given component. */
int suit_plat_check_did(struct zcbor_string *did_uuid,
		suit_component_t component_handle)
{
	struct suit_component_properties properties;
	int ret;

	ret = suit_plat_get_component_properties(component_handle, &properties);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	return (suit_compare_zcbor_strings(properties.did_uuid, did_uuid) ? SUIT_SUCCESS : SUIT_FAIL_CONDITION);
}

/** Check that the given fetch operation can be performed. */
int suit_plat_check_fetch(suit_component_t dst_handle, struct zcbor_string *uri)
{
	struct suit_component_properties properties_dst;

	if (suit_plat_get_component_properties(dst_handle, &properties_dst) != SUIT_SUCCESS) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	return SUIT_ERR_UNSUPPORTED_COMMAND;
}

/** Fetch the payload from the given @p uri into @p dst. */
int suit_plat_fetch(suit_component_t dst_handle, struct zcbor_string *uri)
{
	struct suit_component_properties properties_dst;

	if (suit_plat_get_component_properties(dst_handle, &properties_dst) != SUIT_SUCCESS) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	return SUIT_ERR_UNSUPPORTED_COMMAND;
}

static int fetch_integrated(suit_component_t dst_handle, struct zcbor_string *payload, bool dry_run)
{
	struct suit_component_properties properties_dst;

	if (suit_plat_get_component_properties(dst_handle, &properties_dst) != SUIT_SUCCESS) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	if ((properties_dst.driver->write == NULL) ||
	    ((properties_dst.mode & suit_comp_writable) == 0)) {
		return SUIT_ERR_UNSUPPORTED_COMMAND;
	}

	/* Verify that the payload will fit inside the destination component. */
	if (payload->len > properties_dst.write_size) {
		return SUIT_ERR_MANIFEST_VALIDATION;
	}

	return properties_dst.driver->write(
		dst_handle,
		0,
		(dry_run ? NULL : (uint8_t *)payload->value),
		payload->len);
}

/** Check that the given fetch operation can be performed. */
int suit_plat_check_fetch_integrated(suit_component_t dst_handle, struct zcbor_string *payload)
{
	return fetch_integrated(dst_handle, payload, true);
}

/** Fetch the given integrated payload into @p dst. */
int suit_plat_fetch_integrated(suit_component_t dst_handle, struct zcbor_string *payload)
{
	return fetch_integrated(dst_handle, payload, false);
}

static int copy(suit_component_t dst_handle, suit_component_t src_handle, bool dry_run)
{
	struct suit_component_properties properties_src;
	struct suit_component_properties properties_dst;
	size_t read_size = 0;
	uint8_t *read_address = NULL;

	/* Get the properties (including component driver API) of both components. */
	if ((suit_plat_get_component_properties(dst_handle, &properties_dst) != SUIT_SUCCESS) ||
	    (suit_plat_get_component_properties(src_handle, &properties_src) != SUIT_SUCCESS)) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	/* Check if both read at source and write at destination component are supported. */
	if ((properties_src.driver->read_address == NULL) ||
	    (properties_dst.driver->write == NULL) ||
	    ((properties_src.mode & suit_comp_readable) == 0) ||
	    ((properties_dst.mode & suit_comp_writable) == 0)) {
		return SUIT_ERR_UNSUPPORTED_COMMAND;
	}

	read_size = properties_src.driver->read_address(src_handle, (dry_run ? NULL : &read_address));
	return properties_dst.driver->write(
		dst_handle,
		0,
		(dry_run ? NULL : read_address),
		read_size);
}

/** Check that the given copy operation can be performed. */
int suit_plat_check_copy(suit_component_t dst_handle, suit_component_t src_handle)
{
	return copy(dst_handle, src_handle, true);
}

/** Copy a payload from @p src_handle to @p dst_handle. */
int suit_plat_copy(suit_component_t dst_handle, suit_component_t src_handle)
{
	return copy(dst_handle, src_handle, false);
}

/** Check that the given swap operation can be performed. */
int suit_plat_check_swap(suit_component_t dst_handle, suit_component_t src_handle)
{
	return SUIT_ERR_UNSUPPORTED_COMMAND;
}

/** Swap a payload from @p src_handle to @p dst_handle. */
int suit_plat_swap(suit_component_t dst_handle, suit_component_t src_handle)
{
	return SUIT_ERR_UNSUPPORTED_COMMAND;
}

/** Check that the given run operation can be performed. */
int suit_plat_check_run(suit_component_t image_handle, struct zcbor_string *run_args)
{
	struct suit_component_properties properties_dst;

	if (suit_plat_get_component_properties(image_handle, &properties_dst) != SUIT_SUCCESS) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	if ((properties_dst.mode & suit_comp_runnable) == 0) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	return SUIT_SUCCESS;
}

/** Run the given image. */
int suit_plat_run(suit_component_t image_handle, struct zcbor_string *run_args)
{
	struct suit_component_properties properties_dst;

	if (suit_plat_get_component_properties(image_handle, &properties_dst) != SUIT_SUCCESS) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	return properties_dst.driver->run(image_handle, run_args);
}
