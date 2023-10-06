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

void test_seq_execution_condition_vendor_identifier_no_vid(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x01, /* uint(suit-condition-vendor-identifier) */
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

void test_seq_execution_condition_vendor_identifier(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x01, /* uint(suit-parameter-vendor-identifier) */
				0x50, /* bytes (16) */
				/* RFC4122 uuid5(uuid.NAMESPACE_DNS, 'nordicsemi.com') */
				0x76, 0x17, 0xda, 0xa5, 0x71, 0xfd, 0x5a, 0x85,
				0x8f, 0x94, 0xe2, 0x8d, 0x73, 0x5c, 0xe9, 0xf4,
			0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_vid, SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}

void test_seq_execution_condition_class_identifier_no_cid(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x02, /* uint(suit-condition-class-identifier) */
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

void test_seq_execution_condition_class_identifier(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x02, /* uint(suit-parameter-class-identifier) */
				0x50, /* bytes (16) */
					/* RFC4122 uuid5(nordic_vid, 'nRF54H20_sample_app') */
					0x08, 0xc1, 0xb5, 0x99, 0x55, 0xe8, 0x5f, 0xbc,
					0x9e, 0x76, 0x7b, 0xc2, 0x9c, 0xe1, 0xb0, 0x4d,
			0x02, /* uint(suit-condition-class-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_cid, SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}

void test_seq_execution_condition_device_identifier_no_did(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x18, 0x18, /* uint(suit-condition-device-identifier) */
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

void test_seq_execution_condition_device_identifier(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x18, 0x18, /* uint(suit-parameter-device-identifier) */
				0x50, /* bytes (16) */
					/* RFC4122 uuid5(nordic_vid, 'unspecified_class') */
					0xca, 0xd8, 0x52, 0x3a, 0xf8, 0x29, 0x5a, 0x9a,
					0xba, 0x85, 0x2e, 0xa0, 0xb2, 0xf5, 0x77, 0xc9,
			0x18, 0x18, /* uint(suit-condition-device-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_check_did_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_did, SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
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

void test_seq_execution_condition_image_match_no_image_size(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x03, /* uint(suit-parameter-image-digest) */
				0x58, 0x25, /* bytes(37) */
					0x82, /* array (2 elements) */
						0x38, 0x2B, /* suit-digest-algorithm-id: cose-alg-sha-512 */
						0x58, 0x20, /* suit-digest-bytes: bytes(32) */
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

	TEST_ASSERT_EQUAL(SUIT_ERR_UNAVAILABLE_PARAMETER, retval);
}

void test_seq_execution_condition_image_match_unsupported_algorithm(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa2, /* map (2) */
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
				0x0e, /* uint(suit-parameter-image-size) */
				0x1a, /* uint (32 bit) */
				0x01, 0x02, 0x03, 0x04,
			0x03, /* uint(suit-condition-image-match) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_override_image_size_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, exp_image_size, SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_DECODING, retval);
}

void test_seq_execution_condition_image_match_invalid_digest_length(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa2, /* map (2) */
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
				0x0e, /* uint(suit-parameter-image-size) */
				0x1a, /* uint (32 bit) */
				0x01, 0x02, 0x03, 0x04,
			0x03, /* uint(suit-condition-image-match) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_override_image_size_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, exp_image_size, SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_DECODING, retval);
}

void test_seq_execution_condition_image_match_failed(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa2, /* map (2) */
				0x03, /* uint(suit-parameter-image-digest) */
				0x58, 0x24, /* bytes(36) */
				0x82, /* array (2 elements) */
					0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
					0x58, 0x20, /* suit-digest-bytes: bytes(32) */
					0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
					0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
					0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
					0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
				0x0e, /* uint(suit-parameter-image-size) */
				0x1a, /* uint (32 bit) */
				0x01, 0x02, 0x03, 0x04,
			0x03, /* uint(suit-condition-image-match) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};
	struct zcbor_string exp_decoded_digest = {
		.value = &exp_digest.value[4],
		.len = exp_digest.len - 4,
	};


	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_override_image_size_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, exp_image_size, SUIT_SUCCESS);
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(
		ASSIGNED_COMPONENT_HANDLE,
		suit_cose_sha256,
		&exp_decoded_digest,
		exp_image_size,
		SUIT_ERR_AUTHENTICATION);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_AUTHENTICATION, retval);
}

void test_seq_execution_condition_image_match(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa2, /* map (2) */
				0x03, /* uint(suit-parameter-image-digest) */
				0x58, 0x24, /* bytes(36) */
				0x82, /* array (2 elements) */
					0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
					0x58, 0x20, /* suit-digest-bytes: bytes(32) */
					0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
					0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
					0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
					0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
				0x0e, /* uint(suit-parameter-image-size) */
				0x1a, /* uint (32 bit) */
				0x01, 0x02, 0x03, 0x04,
			0x03, /* uint(suit-condition-image-match) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};
	struct zcbor_string exp_decoded_digest = {
		.value = &exp_digest.value[4],
		.len = exp_digest.len - 4,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_override_image_size_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, exp_image_size, SUIT_SUCCESS);
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(
		ASSIGNED_COMPONENT_HANDLE,
		suit_cose_sha256,
		&exp_decoded_digest,
		exp_image_size,
		SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}

void test_seq_execution_condition_component_slot_no_slot(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x05, /* uint(suit-condition-component-slot) */
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

void test_seq_execution_condition_component_slot(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x05, /* uint(suit-parameter-component-slot) */
				0x03, /* uint (3) */
			0x05, /* uint(suit-condition-component-slot) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_check_slot_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, exp_slot, SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}

void test_seq_execution_condition_abort(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_FAIL_CONDITION, retval);
}
