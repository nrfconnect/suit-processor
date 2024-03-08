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


enum comparison_type {
	comparison_greater = SUIT_Condition_Version_Comparison_Types_suit_condition_version_comparison_greater_m_c,
	comparison_greater_equal = SUIT_Condition_Version_Comparison_Types_suit_condition_version_comparison_greater_equal_m_c,
	comparison_equal = SUIT_Condition_Version_Comparison_Types_suit_condition_version_comparison_equal_m_c,
	comparison_lesser_equal = SUIT_Condition_Version_Comparison_Types_suit_condition_version_comparison_lesser_equal_m_c,
	comparison_lesser = SUIT_Condition_Version_Comparison_Types_suit_condition_version_comparison_lesser_m_c,
};

static int parse_version_check(struct zcbor_string *version_check_cbor, struct suit_semver *version, enum comparison_type *comparison)
{
	struct SUIT_Parameter_Version_Match result = {0};
	size_t result_len = 0;

	if ((version_check_cbor == NULL) || (version == NULL) || (comparison == NULL)) {
		return SUIT_ERR_CRASH;
	}

	int ret = cbor_decode_SUIT_Parameter_Version_Match(
		version_check_cbor->value,
		version_check_cbor->len,
		&result,
		&result_len
	);

	if ((ret != ZCBOR_SUCCESS) || (result_len != version_check_cbor->len)) {
		return SUIT_ERR_DECODING;
	}

	*comparison = (enum comparison_type)result
		.SUIT_Parameter_Version_Match_suit_condition_version_comparison_type
		.SUIT_Condition_Version_Comparison_Types_choice;
	version->count = result
		.SUIT_Parameter_Version_Match_suit_condition_version_comparison_value
		.SUIT_Condition_Version_Comparison_Value_int_count;
	if (version->count > ZCBOR_ARRAY_SIZE(version->value)) {
		version->count = 0;
		return SUIT_ERR_DECODING;
	}

	for (size_t i = 0; i < version->count; i++) {
		version->value[i] = result
			.SUIT_Parameter_Version_Match_suit_condition_version_comparison_value
			.SUIT_Condition_Version_Comparison_Value_int[i];
	}

	return SUIT_SUCCESS;
}

static int compare_versions(struct suit_semver *version_a, struct suit_semver *version_b)
{
	if ((version_a == NULL) || (version_b == NULL)) {
		return 0;
	}
	/* Add "0" for all pre-releases to simplify comparison. */
	if (version_a->count == 4) {
		version_a->value[4] = 0;
		version_a->count = 5;
	}
	if (version_b->count == 4) {
		version_b->value[4] = 0;
		version_b->count = 5;
	}
	const size_t max_i = MIN(version_a->count, version_b->count);

	for (size_t i = 0; i < max_i; i++) {
		if (version_a->value[i] > version_b->value[i]) {
			return 1;
		} else if (version_a->value[i] < version_b->value[i]) {
			return -1;
		}
	}

	/* If we compare pre-relase with a regular release, the latter is greater. */
	if ((version_b->count < 4) && (version_a->count >= 4)) {
		return -1;
	}
	if ((version_a->count < 4) && (version_b->count >= 4)) {
		return 1;
	}

	/* If we compare regular releases with different lengths, report equality. */
	return 0;
}

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
	struct SUIT_Digest digest = {0};
	size_t bytes_processed = 0;

	if (!component_params->image_digest_set) {
		SUIT_ERR("Failed to check image digest: digest not set (handle: %p)\r\n", (void *)component_params->component_handle);
		return SUIT_ERR_UNAVAILABLE_PARAMETER;
	}

	int ret = cbor_decode_SUIT_Digest(
		component_params->image_digest.value,
		component_params->image_digest.len,
		&digest, &bytes_processed);
	if ((ret != ZCBOR_SUCCESS) || (bytes_processed != component_params->image_digest.len)) {
		return SUIT_ERR_DECODING;
	}
	else
	{
		ret = SUIT_SUCCESS;
	}

	if (digest.SUIT_Digest_suit_digest_algorithm_id.suit_cose_hash_algs_choice == suit_cose_hash_algs_cose_alg_sha_256_m_c) {
		/* The SHA256 algorithm is allowed by CDDL. Verify the digest length. */
		if (digest.SUIT_Digest_suit_digest_bytes.len != 32) {
			return SUIT_ERR_DECODING;
		}
	} else if (digest.SUIT_Digest_suit_digest_algorithm_id.suit_cose_hash_algs_choice == suit_cose_hash_algs_cose_alg_sha_512_m_c) {
		/* The SHA512 algorithm is allowed by CDDL. Verify the digest length. */
		if (digest.SUIT_Digest_suit_digest_bytes.len != 64) {
			return SUIT_ERR_DECODING;
		}
	} else {
		/* Other algorithms are not supported. */
		return SUIT_ERR_UNSUPPORTED_ALG;
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return ret;
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	return suit_plat_check_image_match(component_params->component_handle,
		digest.SUIT_Digest_suit_digest_algorithm_id.suit_cose_hash_algs_choice,
		&digest.SUIT_Digest_suit_digest_bytes);
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

int suit_condition_check_content(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	if (!component_params->content_set) {
		SUIT_ERR("Failed to check content: value not set (handle: %p)\r\n",
			 (void *)component_params->component_handle);
		return SUIT_ERR_UNAVAILABLE_PARAMETER;
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return SUIT_SUCCESS;
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	return suit_plat_check_content(component_params->component_handle,
				       &component_params->content);
}

int suit_condition_abort(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	return SUIT_FAIL_CONDITION;
}

int suit_condition_dependency_integrity(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	const uint8_t *envelope_str = NULL;
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
			/* Any issue in loading of the manifest should fail the condition, not the processing. */
			retval = SUIT_FAIL_CONDITION;
			seq_exec_state->retval = retval;
		}

	} else if ((seq_exec_state->cmd_exec_state >= SUIT_SEQ_SHARED) && (seq_exec_state->cmd_exec_state < SUIT_SEQ_MAX)) {
		if (seq_exec_state->retval != SUIT_SUCCESS) {
			/* Any issue in validation of the manifest should fail the condition, not the processing. */
			retval = SUIT_FAIL_CONDITION;
			seq_exec_state->retval = retval;
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

		/* Do not allow component release to spread over more than one iteration. */
		if (ret == SUIT_ERR_AGAIN) {
			ret = SUIT_ERR_CRASH;
		}

		state->manifest_stack_height--;
		if (retval == SUIT_SUCCESS) {
			if (ret != SUIT_SUCCESS) {
				component_params->integrity_checked = false;
			}
			seq_exec_state->retval = ret;
			retval = ret;
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

int suit_condition_version(struct suit_processor_state *state,
		struct suit_manifest_params *component_params)
{
	struct suit_semver component_version = {
		.value = {0, 0, 0, 0, 0},
		.count = 0,
	};
	struct suit_semver cmp_version = {
		.value = {0, 0, 0, 0, 0},
		.count = 0,
	};
	enum comparison_type comparison;
	bool result = false;

	if (!component_params->version_set) {
		SUIT_ERR("Failed to check version: value not set (handle: %p)\r\n", (void *)component_params->component_handle);
		return SUIT_ERR_UNAVAILABLE_PARAMETER;
	}

	int ret = parse_version_check(&component_params->version, &cmp_version, &comparison);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	if (state->dry_run != suit_bool_false) {
		return SUIT_SUCCESS;
	}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	component_version.count = ZCBOR_ARRAY_SIZE(component_version.value);

	ret = suit_plat_component_version_get(component_params->component_handle, component_version.value, &component_version.count);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	int cmp = compare_versions(&component_version, &cmp_version);
	switch (comparison) {
		case comparison_greater:
			result = (cmp > 0);
			break;
		case comparison_greater_equal:
			result = (cmp >= 0);
			break;
		case comparison_equal:
			result = (cmp == 0);
			break;
		case comparison_lesser_equal:
			result = (cmp <= 0);
			break;
		case comparison_lesser:
			result = (cmp < 0);
			break;
		default:
			return SUIT_ERR_DECODING;
	}

	if (result) {
		return SUIT_SUCCESS;
	}

	return SUIT_FAIL_CONDITION;
}
