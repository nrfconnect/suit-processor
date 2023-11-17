/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include <stdint.h>
#include <bootstrap_envelope.h>
#include <suit_schedule_seq.h>
#include "suit_platform/cmock_suit_platform.h"
#include "suit_platform_mock_ext.h"
#include <common_parameters.h>

extern struct suit_processor_state state;

static uint8_t condition_image_match[] = {
	0x84, /* list (4 elements - 2 commands) */
		0x14, /* uint(suit-directive-override-parameters) */
		0xa1, /* map (1) */
			0x03, /* uint(suit-parameter-image-digest) */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
				0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
				0x58, 0x20, /* suit-digest-bytes: bytes(32) */
				0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
				0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
				0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
				0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
		0x03, /* uint(suit-condition-image-match) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};

static struct zcbor_string seq_condition_image_match = {
	.value = condition_image_match,
	.len = sizeof(condition_image_match),
};

static uint8_t envelope_minimal[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x27, /* bytes(39) */
		0x81, /* array (1 element) */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x11,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
};

static struct zcbor_string exp_envelope_minimal = {
	.value = envelope_minimal,
	.len = sizeof(envelope_minimal),
};

static struct zcbor_string exp_manifest_minimal = {
	.value = &envelope_minimal[55 - 9],
	.len = 9,
};

static uint8_t envelope_minimal_invalid_digest[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x27, /* bytes(39) */
		0x81, /* array (1 element) */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x11,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
};

static struct zcbor_string exp_envelope_minimal_invalid_digest = {
	.value = envelope_minimal_invalid_digest,
	.len = sizeof(envelope_minimal_invalid_digest),
};


static uint8_t invalid_digest[] = {
	0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
	0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
	0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
	0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
};

static struct zcbor_string exp_invalid_digest = {
	.value = invalid_digest,
	.len = sizeof(invalid_digest),
};


static int execute_command_sequence(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str)
{
	enum suit_command_sequence seq = SUIT_SEQ_PAYLOAD_FETCH;
	struct suit_manifest_state *manifest_state = &state->manifest_stack[0];

	bootstrap_envelope_sequence(state, seq, cmd_seq_str);

	int ret = suit_schedule_execution(state, manifest_state, seq);
	if (ret == SUIT_ERR_AGAIN) {
		ret = suit_process_scheduled(state);
	}

	return ret;
}


void test_seq_execution_condition_image_match_no_digest(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x03, /* uint(suit-condition-image-match) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_UNAVAILABLE_PARAMETER, retval);
}

void test_seq_execution_condition_image_match_unsupported_algorithm(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x03, /* uint(suit-parameter-image-digest) */
				0x58, 0x45, /* bytes(69) */
				0x82, /* array (2 elements) */
					0x38, 0x2B, /* suit-digest-algorithm-id: cose-alg-sha-512 */
					0x58, 0x40, /* suit-digest-bytes: bytes(64) */
					0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
					0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
					0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
					0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
					0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
					0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
					0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
					0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
			0x03, /* uint(suit-condition-image-match) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_DECODING, retval);
}

void test_seq_execution_condition_image_match_invalid_digest_length(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x03, /* uint(suit-parameter-image-digest) */
				0x58, 0x26, /* bytes(38) */
				0x82, /* array (2 elements) */
					0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
					0x58, 0x20, /* suit-digest-bytes: bytes(32) */
					0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
					0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
					0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
					0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
					0xff, 0xff,
			0x03, /* uint(suit-condition-image-match) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_DECODING, retval);
}

void test_seq_execution_condition_image_match_failed(void)
{
	struct zcbor_string exp_decoded_digest = {
		.value = &exp_digest.value[4],
		.len = exp_digest.len - 4,
	};


	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(
		ASSIGNED_COMPONENT_HANDLE,
		suit_cose_sha256,
		&exp_decoded_digest,
		SUIT_ERR_AUTHENTICATION);

	int retval = execute_command_sequence(&state, &seq_condition_image_match);

	TEST_ASSERT_EQUAL(SUIT_ERR_AUTHENTICATION, retval);
}

void test_seq_execution_condition_image_match(void)
{
	struct zcbor_string exp_decoded_digest = {
		.value = &exp_digest.value[4],
		.len = exp_digest.len - 4,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(
		ASSIGNED_COMPONENT_HANDLE,
		suit_cose_sha256,
		&exp_decoded_digest,
		SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq_condition_image_match);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}

void test_seq_execution_condition_image_match_dependency(void)
{
	struct zcbor_string exp_decoded_digest = {
		.value = &exp_digest.value[4],
		.len = exp_digest.len - 4,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str((uint8_t **)&exp_envelope_minimal.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_envelope_minimal.len);
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(
		suit_cose_sha256,
		&exp_decoded_digest,
		&exp_manifest_minimal,
		SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq_condition_image_match);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}

void test_seq_execution_condition_image_match_dependency_with_invalid_digest(void)
{
	struct zcbor_string exp_decoded_digest = {
		.value = &exp_digest.value[4],
		.len = exp_digest.len - 4,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str((uint8_t **)&exp_envelope_minimal.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_envelope_minimal.len);
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(
		suit_cose_sha256,
		&exp_decoded_digest,
		&exp_manifest_minimal,
		SUIT_ERR_AUTHENTICATION);

	int retval = execute_command_sequence(&state, &seq_condition_image_match);

	TEST_ASSERT_EQUAL(SUIT_ERR_AUTHENTICATION, retval);
}

void test_seq_execution_condition_image_match_dependency_with_different_digest(void)
{
	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str((uint8_t **)&exp_envelope_minimal_invalid_digest.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_envelope_minimal_invalid_digest.len);
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(
		suit_cose_sha256,
		&exp_invalid_digest,
		&exp_manifest_minimal,
		SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq_condition_image_match);

	TEST_ASSERT_EQUAL(SUIT_FAIL_CONDITION, retval);
}

void test_seq_execution_condition_image_match_dependency_missing_payload(void)
{
	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_ERR_MISSING_COMPONENT);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str((uint8_t **)&exp_envelope_minimal.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_envelope_minimal.len);

	int retval = execute_command_sequence(&state, &seq_condition_image_match);

	TEST_ASSERT_EQUAL(SUIT_ERR_MISSING_COMPONENT, retval);
}
