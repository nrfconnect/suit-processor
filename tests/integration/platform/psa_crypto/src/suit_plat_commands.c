/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <suit_platform.h>
#include <suit_platform_internal.h>


int suit_plat_check_slot(suit_component_t component_handle, unsigned int slot)
{
	const struct suit_component_impl *impl = suit_plat_component_impl_get(component_handle);

	if (impl == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	if (impl->check_slot == NULL) {
		return SUIT_FAIL_CONDITION;
	}

	return impl->check_slot(component_handle, slot);
}

int suit_plat_check_vid(struct zcbor_string *vid_uuid,
		suit_component_t component_handle)
{
	const struct suit_component_impl *impl = suit_plat_component_impl_get(component_handle);

	if (impl == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	if (impl->check_vid == NULL) {
		return SUIT_FAIL_CONDITION;
	}

	return impl->check_vid(component_handle, vid_uuid);
}

int suit_plat_check_cid(struct zcbor_string *cid_uuid,
		suit_component_t component_handle)
{
	const struct suit_component_impl *impl = suit_plat_component_impl_get(component_handle);

	if (impl == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	if (impl->check_cid == NULL) {
		return SUIT_FAIL_CONDITION;
	}

	return impl->check_cid(component_handle, cid_uuid);
}

int suit_plat_check_did(struct zcbor_string *did_uuid,
		suit_component_t component_handle)
{
	const struct suit_component_impl *impl = suit_plat_component_impl_get(component_handle);

	if (impl == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	if (impl->check_did == NULL) {
		return SUIT_FAIL_CONDITION;
	}

	return impl->check_did(component_handle, did_uuid);
}

int suit_plat_check_fetch(suit_component_t dst_handle, struct zcbor_string *uri)
{
	const struct suit_component_impl *impl = suit_plat_component_impl_get(dst_handle);

	if (impl == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	return SUIT_ERR_UNSUPPORTED_COMMAND;
}

int suit_plat_fetch(suit_component_t dst_handle, struct zcbor_string *uri)
{
	const struct suit_component_impl *impl = suit_plat_component_impl_get(dst_handle);

	if (impl == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	return SUIT_ERR_UNSUPPORTED_COMMAND;
}

static int fetch_integrated(suit_component_t dst_handle, struct zcbor_string *payload, bool dry_run)
{
	struct zcbor_string fetch_payload;
	const struct suit_component_impl *impl = suit_plat_component_impl_get(dst_handle);

	if (impl == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	if (impl->fetch_integrated == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMMAND;
	}

	fetch_payload.value = (dry_run ? NULL : payload->value);
	fetch_payload.len = payload->len;
	return impl->fetch_integrated(dst_handle, &fetch_payload);
}

int suit_plat_check_fetch_integrated(suit_component_t dst_handle, struct zcbor_string *payload)
{
	return fetch_integrated(dst_handle, payload, true);
}

int suit_plat_fetch_integrated(suit_component_t dst_handle, struct zcbor_string *payload)
{
	return fetch_integrated(dst_handle, payload, false);
}

static int copy(suit_component_t dst_handle, suit_component_t src_handle, bool dry_run)
{
	const struct suit_component_impl *src_impl = suit_plat_component_impl_get(src_handle);
	const struct suit_component_impl *dst_impl = suit_plat_component_impl_get(dst_handle);
	size_t read_size = 0;
	uint8_t *read_address = NULL;

	if ((src_impl == NULL) || (dst_impl == NULL)) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	if ((src_impl->read_address == NULL) ||
	    (dst_impl->write == NULL)) {
		return SUIT_ERR_UNSUPPORTED_COMMAND;
	}

	read_size = src_impl->read_address(src_handle, (dry_run ? NULL : &read_address));
	return dst_impl->write(
		dst_handle,
		0,
		(dry_run ? NULL : read_address),
		read_size);
}

int suit_plat_check_copy(suit_component_t dst_handle, suit_component_t src_handle)
{
	return copy(dst_handle, src_handle, true);
}

int suit_plat_copy(suit_component_t dst_handle, suit_component_t src_handle)
{
	return copy(dst_handle, src_handle, false);
}

int suit_plat_check_swap(suit_component_t dst_handle, suit_component_t src_handle)
{
	return SUIT_ERR_UNSUPPORTED_COMMAND;
}

int suit_plat_swap(suit_component_t dst_handle, suit_component_t src_handle)
{
	return SUIT_ERR_UNSUPPORTED_COMMAND;
}

int suit_plat_check_run(suit_component_t image_handle, struct zcbor_string *run_args)
{
	const struct suit_component_impl *impl = suit_plat_component_impl_get(image_handle);

	if (impl == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	if (impl->run == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMMAND;
	}

	return SUIT_SUCCESS;
}

int suit_plat_run(suit_component_t image_handle, struct zcbor_string *run_args)
{
	const struct suit_component_impl *impl = suit_plat_component_impl_get(image_handle);

	if (impl == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	if (impl->run == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMMAND;
	}

	return impl->run(image_handle, run_args);
}
