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

void test_seq_execution_condition_check_content_no_content(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x06, /* uint(suit-condition-check-content) */
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

void test_seq_execution_condition_check_content(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x12, /* uint(suit-parameter-content) */
				0x48, /* bytes (8) */
					0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78, 0x89,
			0x06, /* uint(suit-condition-check-content) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};

	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_check_content_ExpectComplexArgsAndReturn(
		ASSIGNED_COMPONENT_HANDLE,
		&exp_content,
		SUIT_SUCCESS);

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

void test_seq_execution_condition_dependency_fail(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x08, /* uint(suit-condition-is_dependency) */
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

void test_seq_execution_condition_dependency_success(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x08, /* uint(suit-condition-is_dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}

void test_seq_execution_condition_dependency_invalid(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x08, /* uint(suit-condition-is_dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	state.components[0].is_dependency = true;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_TAMP, retval);
}

void test_seq_execution_condition_dependency_mixed(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x0c, /* uint(suit-directive-set-component-index) */
			0xf5, /* True */
			0x08, /* uint(suit-condition-is_dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 3);
	state.components[1].is_dependency = suit_bool_false;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_FAIL_CONDITION, retval);
}

void test_seq_execution_condition_dependency_multiple_dependencies(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x0c, /* uint(suit-directive-set-component-index) */
			0xf5, /* True */
			0x08, /* uint(suit-condition-is_dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 3);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}

void test_seq_execution_condition_dependency_integrity_fail(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x07, /* uint(suit-condition-dependency-integrity) */
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

void test_seq_execution_condition_dependency_integrity_invalid(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x07, /* uint(suit-condition-dependency-integrity) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	state.components[0].is_dependency = true;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_TAMP, retval);
}
