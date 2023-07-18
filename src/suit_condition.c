/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <suit_condition.h>
#include <suit_platform.h>
#include <manifest_decode.h>
#include <suit_seq_exec.h>
#include <suit_schedule_seq.h>
#include <suit_manifest.h>


int suit_condition_vendor_identifier(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	if (!component_params->vid_set) {
		SUIT_ERR("Failed to check vendor ID: value not set (handle: %p)\r\n", (void *)component_params->component_handle);
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
		SUIT_ERR("Failed to check class ID: value not set (handle: %p)\r\n", (void *)component_params->component_handle);
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
		SUIT_ERR("Failed to check device ID: value not set (handle: %p)\r\n", (void *)component_params->component_handle);
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
	struct SUIT_Digest digest;
	size_t bytes_processed;

	if (!component_params->image_digest_set
		|| !component_params->image_size_set) {
		SUIT_ERR("Failed to check image digest: size or digest not set (handle: %p)\r\n", (void *)component_params->component_handle);
		return SUIT_ERR_UNAVAILABLE_PARAMETER;
	}

	int ret = cbor_decode_SUIT_Digest(
		component_params->image_digest.value,
		component_params->image_digest.len,
		&digest, &bytes_processed);
	if ((ret != 0) || (bytes_processed != component_params->image_digest.len)) {
		return SUIT_ERR_DECODING;
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return SUIT_SUCCESS;
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
	return suit_plat_check_image_match(suit_cose_sha256,
			&digest._SUIT_Digest_suit_digest_bytes,
			component_params->image_size,
			component_params->component_handle);
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
	return suit_plat_check_image_match(component_params->component_handle,
			suit_cose_sha256,
			&digest._SUIT_Digest_suit_digest_bytes,
			component_params->image_size);
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
}


int suit_condition_component_slot(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	if (!component_params->component_slot_set) {
		SUIT_ERR("Failed to check slot: value not set (handle: %p)\r\n", (void *)component_params->component_handle);
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

#ifndef SUIT_PLATFORM_LEGACY_API_SUPPORT
int suit_condition_dependency_integrity(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	uint8_t *envelope_str;
	size_t envelope_len;
	struct suit_seq_exec_state *seq_exec_state;
	struct suit_manifest_state *manifest_state;

	if (component_params->is_dependency == suit_bool_false) {
		SUIT_ERR("Unsupported component id (not a dependency manifest)\r\n");
		return SUIT_FAIL_CONDITION;
	} else if (component_params->is_dependency != suit_bool_true) {
		SUIT_ERR("Unsupported component id (invalid dependency flag value)\r\n");
		return SUIT_ERR_TAMP;
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return SUIT_SUCCESS;
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	int retval = suit_seq_exec_state_get(state, &seq_exec_state);
	if (retval != SUIT_SUCCESS) {
		return retval;
	}

	SUIT_DBG("Current manifest dependency seq state: %d\r\n", seq_exec_state->cmd_exec_state);
	manifest_state = &state->manifest_stack[state->manifest_stack_height - 1];

	if (seq_exec_state->cmd_exec_state == SUIT_SEQ_EXEC_DEFAULT_STATE) {
		/** Return a pointer to the manifest contents, stored inside the component. */
		retval = suit_plat_retrive_manifest(component_params->component_handle, &envelope_str, &envelope_len);

		if (retval == SUIT_SUCCESS) {
			retval = suit_processor_load_envelope(state, envelope_str, envelope_len);
		}

		if (retval == SUIT_SUCCESS) {
			SUIT_DBG("Validate sequences\r\n");
			seq_exec_state->cmd_exec_state = SUIT_SEQ_SHARED;
			seq_exec_state->retval = SUIT_SUCCESS;
			retval = SUIT_ERR_AGAIN;
		} else {
			seq_exec_state->retval = retval;
		}

	} else if ((seq_exec_state->cmd_exec_state >= SUIT_SEQ_SHARED) && (seq_exec_state->cmd_exec_state < SUIT_SEQ_MAX)) {
		if (seq_exec_state->retval != SUIT_SUCCESS) {
			retval = seq_exec_state->retval;
		} else {
			seq_exec_state->retval = suit_schedule_validation(state, manifest_state, seq_exec_state->cmd_exec_state);
			if (seq_exec_state->retval == SUIT_ERR_UNAVAILABLE_COMMAND_SEQ) {
				seq_exec_state->retval = SUIT_SUCCESS;
			}
			retval = SUIT_ERR_AGAIN;
		}

		seq_exec_state->cmd_exec_state++;

	} else {
		if (seq_exec_state->retval == SUIT_SUCCESS) {
			component_params->integrity_checked = true;
		}

		retval = seq_exec_state->retval;
	}

	if ((retval != SUIT_ERR_AGAIN) && (seq_exec_state->cmd_exec_state != SUIT_SEQ_EXEC_DEFAULT_STATE)) {
		SUIT_DBG("Release manifest\r\n");
		/* Remove the checked manifest from the stack */
		int ret = suit_manifest_release(manifest_state);
		state->manifest_stack_height--;
		if (retval == SUIT_SUCCESS) {
			seq_exec_state->retval = ret;
		}
	}

	return retval;
}

int suit_condition_is_dependency(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	if (component_params->is_dependency == suit_bool_true) {
		return SUIT_SUCCESS;
	} else if (component_params->is_dependency == suit_bool_false) {
		return SUIT_FAIL_CONDITION;
	}

	return SUIT_ERR_TAMP;
}
#endif /* !SUIT_PLATFORM_LEGACY_API_SUPPORT */
