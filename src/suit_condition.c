/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <suit_types.h>
#include <suit_platform.h>


int suit_condition_vendor_identifier(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	if (!component_params->vid_set) {
		return SUIT_ERR_UNAVAILABLE_PARAMETER;
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return SUIT_SUCCESS;
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
	return suit_plat_check_vid(&component_params->vid,
				component_params->component_handle);
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
	return suit_plat_check_vid(component_params->component_handle,
				&component_params->vid);
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
}


int suit_condition_class_identifier(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	if (!component_params->cid_set) {
		return SUIT_ERR_UNAVAILABLE_PARAMETER;
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return SUIT_SUCCESS;
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
	return suit_plat_check_cid(&component_params->cid,
				component_params->component_handle);
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
	return suit_plat_check_cid(component_params->component_handle,
				&component_params->cid);
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
}


int suit_condition_device_identifier(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	if (!component_params->did_set) {
		return SUIT_ERR_UNAVAILABLE_PARAMETER;
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return SUIT_SUCCESS;
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
	return suit_plat_check_did(&component_params->did,
				component_params->component_handle);
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
	return suit_plat_check_did(component_params->component_handle,
				&component_params->did);
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
}


int suit_condition_image_match(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	if (!component_params->image_digest_set
		|| !component_params->image_size_set) {
		return SUIT_ERR_UNAVAILABLE_PARAMETER;
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return SUIT_SUCCESS;
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
	return suit_plat_check_image_match(suit_cose_sha256,
			&component_params->image_digest._SUIT_Digest_suit_digest_bytes,
			component_params->image_size,
			component_params->component_handle);
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
	return suit_plat_check_image_match(component_params->component_handle,
			suit_cose_sha256,
			&component_params->image_digest._SUIT_Digest_suit_digest_bytes,
			component_params->image_size);
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
}


int suit_condition_component_slot(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	if (!component_params->component_slot_set) {
		return SUIT_ERR_UNAVAILABLE_PARAMETER;
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return SUIT_SUCCESS;
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	return suit_plat_check_slot(component_params->component_handle, component_params->component_slot);
}


int suit_condition_abort(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	return SUIT_FAIL_CONDITION;
}


