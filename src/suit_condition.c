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
#include <suit.h>


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

	return suit_plat_check_vid(component_params->component_handle,
				&component_params->vid);
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

	return suit_plat_check_cid(component_params->component_handle,
				&component_params->cid);
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

	return suit_plat_check_did(component_params->component_handle,
				&component_params->did);
}


int suit_condition_image_match(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	struct SUIT_Digest digest;
	size_t bytes_processed;

	if (!component_params->image_digest_set) {
		SUIT_ERR("Failed to check image digest: digest not set (handle: %p)\r\n", (void *)component_params->component_handle);
		return SUIT_ERR_UNAVAILABLE_PARAMETER;
	}

	int ret = cbor_decode_SUIT_Digest(
		component_params->image_digest.value,
		component_params->image_digest.len,
		&digest, &bytes_processed);
	if ((ret != 0) || (bytes_processed != component_params->image_digest.len)) {
		return SUIT_ERR_DECODING;
	}

	if (digest._SUIT_Digest_suit_digest_algorithm_id._suit_cose_hash_algs_choice == _suit_cose_hash_algs__cose_alg_sha_256) {
		/* The SHA256 algorithm is allowed by CDDL. Verify the digest length. */
		if (digest._SUIT_Digest_suit_digest_bytes.len != 32) {
			return SUIT_ERR_DECODING;
		}
	} else if (digest._SUIT_Digest_suit_digest_algorithm_id._suit_cose_hash_algs_choice == _suit_cose_hash_algs__cose_alg_sha_512) {
		/* The SHA512 algorithm is allowed by CDDL. Verify the digest length. */
		if (digest._SUIT_Digest_suit_digest_bytes.len != 64) {
			return SUIT_ERR_DECODING;
		}
	} else {
		/* Other algorithms are not supported. */
		return SUIT_ERR_DECODING;
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return ret;
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	if (component_params->is_dependency == suit_bool_true) {
		uint8_t *envelope_str;
		size_t envelope_len;
		struct zcbor_string manifest_digest;
		enum suit_cose_alg alg;

		ret = suit_plat_retrieve_manifest(component_params->component_handle, &envelope_str, &envelope_len);
		if (ret != SUIT_SUCCESS) {
			SUIT_ERR("Failed to check image digest: unable to retrieve manifest contents (handle: %p)\r\n", (void *)component_params->component_handle);
			return ret;
		}

		ret = suit_processor_get_manifest_metadata(envelope_str, envelope_len, false, NULL, &manifest_digest, &alg, NULL);
		if (ret != SUIT_SUCCESS) {
			SUIT_ERR("Failed to check image digest: unable to read manifest digest (handle: %p)\r\n", (void *)component_params->component_handle);
			return ret;
		}

		if ((enum suit_cose_alg)digest._SUIT_Digest_suit_digest_algorithm_id._suit_cose_hash_algs_choice != alg) {
			SUIT_ERR("Manifest digest check failed: digest algorithm does not match (handle: %p)\r\n", (void *)component_params->component_handle);
			ret = SUIT_FAIL_CONDITION;
		} else if (!suit_compare_zcbor_strings(&digest._SUIT_Digest_suit_digest_bytes, &manifest_digest)) {
			SUIT_ERR("Manifest digest check failed: digest values does not match (handle: %p)\r\n", (void *)component_params->component_handle);
			ret = SUIT_FAIL_CONDITION;
		}
	} else {
		ret = suit_plat_check_image_match(component_params->component_handle,
			digest._SUIT_Digest_suit_digest_algorithm_id._suit_cose_hash_algs_choice,
			&digest._SUIT_Digest_suit_digest_bytes);
	}

	return ret;
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

int suit_condition_dependency_integrity(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	uint8_t *envelope_str = NULL;
	size_t envelope_len = 0;
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
		retval = suit_plat_retrieve_manifest(component_params->component_handle, &envelope_str, &envelope_len);

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
