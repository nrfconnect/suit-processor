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

void test_seq_execution_condition_version_no_version(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x18, 0x1c, /* uint(suit-condition-version) */
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

void test_seq_execution_condition_version_no_platform_version(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x18, 0x1c, /* uint(suit-parameter-version) */
				0x44, /* bytes (4) */
					0x82, /* list(2) */
					/* suit-condition-version-comparison-type */
					0x01, /* suit-condition-version-comparison-greater */
					/* suit-condition-version-comparison-value */
					0x81, 0x01, /* v1.*.* */
			0x18, 0x1c, /* uint(suit-condition-version) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_component_version_get_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
	__cmock_suit_plat_component_version_get_IgnoreArg_version();
	__cmock_suit_plat_component_version_get_IgnoreArg_version_len();

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
}

void test_seq_execution_condition_version_unavailable_platform_version(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x18, 0x1c, /* uint(suit-parameter-version) */
				0x44, /* bytes (4) */
					0x82, /* list(2) */
					/* suit-condition-version-comparison-type */
					0x01, /* suit-condition-version-comparison-greater */
					/* suit-condition-version-comparison-value */
					0x81, 0x01, /* v1.*.* */
			0x18, 0x1c, /* uint(suit-condition-version) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_component_version_get_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_FAIL_CONDITION);
	__cmock_suit_plat_component_version_get_IgnoreArg_version();
	__cmock_suit_plat_component_version_get_IgnoreArg_version_len();

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_FAIL_CONDITION, retval);
}

void test_seq_execution_condition_version_invalid_comparison(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x18, 0x1c, /* uint(suit-parameter-version) */
				0x44, /* bytes (4) */
					0x82, /* list(2) */
					/* suit-condition-version-comparison-type */
					0x06, /* suit-condition-version-comparison-unknown */
					/* suit-condition-version-comparison-value */
					0x81, 0x01, /* v1.*.* */
			0x18, 0x1c, /* uint(suit-condition-version) */
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

void test_seq_execution_condition_version_invalid_version(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x18, 0x1c, /* uint(suit-parameter-version) */
				0x49, /* bytes (9) */
					0x82, /* list(2) */
					/* suit-condition-version-comparison-type */
					0x01, /* suit-condition-version-comparison-greater */
					/* suit-condition-version-comparison-value */
					0x86, 0x01, 0x02, 0x03, 0x20, 0x04, 0x05, /* v1.2.3-rc.4.5 */
			0x18, 0x1c, /* uint(suit-condition-version) */
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

void test_seq_execution_condition_version(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x18, 0x1c, /* uint(suit-parameter-version) */
				0x44, /* bytes (4) */
					0x82, /* list(2) */
					/* suit-condition-version-comparison-type */
					0x01, /* suit-condition-version-comparison-greater */
					/* suit-condition-version-comparison-value */
					0x81, 0x01, /* v1.*.* */
			0x18, 0x1c, /* uint(suit-condition-version) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};
	struct suit_semver ver_200 = {
		.value = { 2, 0, 0, 0, 0 },
		.count = 4,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_component_version_get_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_component_version_get_IgnoreArg_version();
	__cmock_suit_plat_component_version_get_IgnoreArg_version_len();
	__cmock_suit_plat_component_version_get_ReturnArrayThruPtr_version(ver_200.value, ver_200.count);
	__cmock_suit_plat_component_version_get_ReturnThruPtr_version_len(&ver_200.count);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}

static int gen_comparison(struct suit_semver *version, uint8_t cmp_type, uint8_t *buf)
{
	size_t i = 0;
	buf[i++] = 0x84; /* list (4 elements - 2 commands) */
	buf[i++] = 0x14; /* uint(suit-directive-override-parameters) */
	buf[i++] = 0xa1; /* map (1) */
	buf[i++] = 0x18; buf[i++] = 0x1c; /* uint(suit-parameter-version) */
	buf[i++] = 0x43 + version->count; /* bytes (count + 3) */
	buf[i++] = 0x82; /* list(2) */
	/* suit-condition-version-comparison-type */
	buf[i++] = cmp_type;
	/* suit-condition-version-comparison-value */
	buf[i++] = 0x80 + version->count; /* list(count) */
	for (size_t v_i = 0; v_i < version->count; v_i++) {
		if (version->value[v_i] >= 0) {
			buf[i++] = version->value[v_i];
		} else {
			buf[i++] = 0x1f - version->value[v_i];
		}
	}
	buf[i++] = 0x18; buf[i++] = 0x1c; /* uint(suit-condition-version) */
	buf[i++] = 0x00; /* uint(SUIT_Rep_Policy::None) */

	return i;
}

static void exec_comparison(struct suit_semver *v1, struct suit_semver *v2, int cmp, bool pass)
{
	uint8_t seq_cmd[128];
	uint8_t msg_str[128];
	const char *cmp_log = "!!";

	switch (cmp) {
		case 0x01:
			cmp_log = " >";
			break;
		case 0x02:
			cmp_log = ">=";
			break;
		case 0x03:
			cmp_log = "==";
			break;
		case 0x04:
			cmp_log = "<=";
			break;
		case 0x05:
			cmp_log = " <";
			break;
		default:
			break;
	}

	snprintf(msg_str, sizeof(msg_str),
		"Comparison %d.%d.%d.%d.%d %s %d.%d.%d.%d.%d failed",
		v1->value[0], v1->value[1], v1->value[2], v1->value[3], v1->value[4],
		cmp_log, v2->value[0], v2->value[1], v2->value[2], v2->value[3], v2->value[4]);

	size_t cmd_len = gen_comparison(v2, cmp, seq_cmd);
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = cmd_len,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_component_version_get_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_component_version_get_IgnoreArg_version();
	__cmock_suit_plat_component_version_get_IgnoreArg_version_len();
	__cmock_suit_plat_component_version_get_ReturnArrayThruPtr_version(v1->value, v1->count);
	__cmock_suit_plat_component_version_get_ReturnThruPtr_version_len(&v1->count);

	int retval = execute_command_sequence(&state, &seq);

	if (pass) {
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, retval, msg_str);
	} else {
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_FAIL_CONDITION, retval, msg_str);
	}
}

void test_seq_execution_condition_version_semver_20(void)
{
	struct suit_semver versions[] = {
		/* 1.0.0-alpha */
		{
			.value = { 1, 0, 0, -3, 0 },
			.count = 4,
		},
		/* 1.0.0-alpha.1 */
		{
			.value = { 1, 0, 0, -3, 1 },
			.count = 5,
		},
		/* 1.0.0-beta */
		{
			.value = { 1, 0, 0, -2, 0 },
			.count = 4,
		},
		/* 1.0.0-beta.2 */
		{
			.value = { 1, 0, 0, -2, 2 },
			.count = 5,
		},
		/* 1.0.0-beta.11 */
		{
			.value = { 1, 0, 0, -2, 11 },
			.count = 5,
		},
		/* 1.0.0-rc.1 */
		{
			.value = { 1, 0, 0, -1, 1 },
			.count = 5,
		},
		/* 1.0.0 */
		{
			.value = { 1, 0, 0, 0, 0 },
			.count = 3,
		},
		/* 2.0.0-rc.1 */
		{
			.value = { 2, 0, 0, -1, 1 },
			.count = 5,
		},
		/* 2.0.0 */
		{
			.value = { 2, 0, 0, 0, 0 },
			.count = 3,
		},
	};

	for (size_t v_a = 0; v_a < ZCBOR_ARRAY_SIZE(versions); v_a++) {
		for (size_t v_b = 0; v_b < ZCBOR_ARRAY_SIZE(versions); v_b++) {
			exec_comparison(&versions[v_a], &versions[v_b], 0x01, v_a > v_b);
			exec_comparison(&versions[v_a], &versions[v_b], 0x02, v_a >= v_b);
			exec_comparison(&versions[v_a], &versions[v_b], 0x03, v_a == v_b);
			exec_comparison(&versions[v_a], &versions[v_b], 0x04, v_a <= v_b);
			exec_comparison(&versions[v_a], &versions[v_b], 0x05, v_a < v_b);
		}
	}
}

void test_seq_execution_condition_version_wildcard(void)
{
	struct suit_semver versions[] = {
		/* *.*.* */
		{
			.value = { 0, 0, 0, 0, 0 },
			.count = 0,
		},
		/* 1.*.* */
		{
			.value = { 1, 0, 0, 0, 0 },
			.count = 1,
		},
		/* 1.2.* */
		{
			.value = { 1, 2, 0, 0, 0 },
			.count = 2,
		},
		/* 1.2.3 */
		{
			.value = { 1, 2, 3, 0, 0 },
			.count = 3,
		},
	};
	/* 1.2.3-rc */
	struct suit_semver prerelease = {
		.value = { 1, 0, 0, -1, 0 },
		.count = 5,
	};

	for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(versions) - 1; i++) {
		exec_comparison(&versions[i], &versions[i + 1], 0x01, false);
		exec_comparison(&versions[i], &versions[i + 1], 0x02, true);
		exec_comparison(&versions[i], &versions[i + 1], 0x03, true);
		exec_comparison(&versions[i], &versions[i + 1], 0x04, true);
		exec_comparison(&versions[i], &versions[i + 1], 0x05, false);

		exec_comparison(&versions[i], &prerelease, 0x01, true);
		exec_comparison(&versions[i], &prerelease, 0x02, true);
		exec_comparison(&versions[i], &prerelease, 0x03, false);
		exec_comparison(&versions[i], &prerelease, 0x04, false);
		exec_comparison(&versions[i], &prerelease, 0x05, false);
	}
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
