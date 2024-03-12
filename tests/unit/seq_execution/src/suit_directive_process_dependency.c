/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
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

static uint8_t invalid_envelope[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */
	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};
static struct zcbor_string exp_invalid_envelope = {
	.value = invalid_envelope,
	.len = sizeof(invalid_envelope),
};

static uint8_t envelope_digest[] = {
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
};

struct zcbor_string exp_envelope_digest = {
	.value = envelope_digest,
	.len = sizeof(envelope_digest),
};

static uint8_t unsupported_envelope[] = {
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
	0x52, /* bytes(18) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x4b, /* bytes(11) */
		0xA2, /* map (2 elements) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',

			0x04, /* suit-shared-sequence */
			0x43, /* bytes(3) */
			0x82, /* list (2 elements - 1 command) */
				0x1f, /* uint(suit-directive-swap) */
				0x00, /* uint(SUIT_Rep_Policy::None) */
};

static struct zcbor_string exp_unsupported_envelope = {
	.value = unsupported_envelope,
	.len = sizeof(unsupported_envelope),
};

static struct zcbor_string exp_unsupported_manifest = {
	.value = &unsupported_envelope[sizeof(unsupported_envelope) - 19],
	.len = 19,
};

static uint8_t valid_envelope[] = {
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
	0x58, 0x27, /* bytes(39) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x58, 0x1f, /* bytes(31) */
		0xA2, /* map (2 elements) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',

			0x04, /* suit-shared-sequence */
			0x57, /* bytes(23) */
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

static struct zcbor_string exp_valid_envelope = {
	.value = valid_envelope,
	.len = sizeof(valid_envelope),
};

static struct zcbor_string exp_valid_manifest = {
	.value = &valid_envelope[sizeof(valid_envelope) - 41],
	.len = 41,
};

static suit_component_t exp_component_id_handle = ASSIGNED_COMPONENT_HANDLE;

static uint8_t component_id[] = {0x81, 0x41, 'M'};

static struct zcbor_string exp_component_id = {
	.value = component_id,
	.len = sizeof(component_id),
};

static uint8_t root_component_id[] = {0x81, 0x44, 'R', 'O', 'O', 'T'};

static struct zcbor_string exp_root_component_id = {
	.value = root_component_id,
	.len = sizeof(root_component_id),
};

static struct zcbor_string unknown_manifest_component_id = {
	.value = NULL,
	.len = 0,
};


static int execute_command_sequence(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str)
{
	enum suit_command_sequence seq = SUIT_SEQ_PAYLOAD_FETCH;
	struct suit_manifest_state *manifest_state = &state->manifest_stack[0];

	bootstrap_envelope_sequence(state, seq, cmd_seq_str);

	/* Normally sequence is set in suit_process_sequence(..). */
	state->current_seq = SUIT_SEQ_PAYLOAD_FETCH;

	int ret = suit_schedule_execution(state, manifest_state, seq);
	if (ret == SUIT_ERR_AGAIN) {
		ret = suit_process_scheduled(state);
	}

	return ret;
}

void test_seq_execution_process_dependency_tampered_component(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);
	/* SUIT processor uses special values to mark components as dependencies. */
	state.components[0].is_dependency = true;
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_TAMP, retval);
}

void test_seq_execution_process_dependency_regular_component(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
}

void test_seq_execution_process_dependency_integrity_unknown(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	state.components[0].integrity_checked = false;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_ORDER, retval);
}

void test_seq_execution_process_dependency_manifest_not_found(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(
		ASSIGNED_COMPONENT_HANDLE,
		NULL,
		NULL,
		SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
}

void test_seq_execution_process_dependency_manifest_lazy_loader(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(
		ASSIGNED_COMPONENT_HANDLE,
		NULL,
		NULL,
		SUIT_ERR_AGAIN);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_CRASH, retval);
}

void test_seq_execution_process_dependency_invalid_dependency_payload(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_invalid_envelope.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_invalid_envelope.len);

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_DECODING, retval);
}

void test_seq_execution_process_dependency_unauthorized_dependency(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_unsupported_envelope.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_unsupported_envelope.len);

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(
		suit_cose_sha256,
		&exp_envelope_digest,
		&exp_unsupported_manifest,
		SUIT_SUCCESS);

	__cmock_suit_plat_authorize_unsigned_manifest_ExpectAndReturn(NULL, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_unsigned_manifest_IgnoreArg_manifest_component_id();
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&unknown_manifest_component_id, &exp_component_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_component_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&exp_component_id_handle);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &unknown_manifest_component_id, 0x10, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_component_id, &unknown_manifest_component_id, SUIT_SEQ_PAYLOAD_FETCH, SUIT_ERR_AUTHENTICATION);
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(exp_component_id_handle, SUIT_SUCCESS);

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;
	state.manifest_stack[0].manifest_component_id = exp_root_component_id;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_AUTHENTICATION, retval);
}

void test_seq_execution_process_dependency_invalid_dependency_contents(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_unsupported_envelope.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_unsupported_envelope.len);

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(
		suit_cose_sha256,
		&exp_envelope_digest,
		&exp_unsupported_manifest,
		SUIT_SUCCESS);

	__cmock_suit_plat_authorize_unsigned_manifest_ExpectAndReturn(NULL, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_unsigned_manifest_IgnoreArg_manifest_component_id();
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&unknown_manifest_component_id, &exp_component_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_component_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&exp_component_id_handle);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &unknown_manifest_component_id, 0x10, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_component_id, &unknown_manifest_component_id, SUIT_SEQ_PAYLOAD_FETCH, SUIT_SUCCESS);
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(exp_component_id_handle, SUIT_SUCCESS);

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;
	state.manifest_stack[0].manifest_component_id = exp_root_component_id;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_DECODING, retval);
}

void test_seq_execution_process_dependency_processing_failed(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_valid_envelope.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_valid_envelope.len);

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(
		suit_cose_sha256,
		&exp_envelope_digest,
		&exp_valid_manifest,
		SUIT_SUCCESS);

	__cmock_suit_plat_authorize_unsigned_manifest_ExpectAndReturn(NULL, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_unsigned_manifest_IgnoreArg_manifest_component_id();
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&unknown_manifest_component_id, &exp_component_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_component_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&exp_component_id_handle);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &unknown_manifest_component_id, 0x10, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_component_id, &unknown_manifest_component_id, SUIT_SEQ_PAYLOAD_FETCH, SUIT_SUCCESS);

	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_vid, SUIT_FAIL_CONDITION);

	__cmock_suit_plat_release_component_handle_ExpectAndReturn(exp_component_id_handle, SUIT_SUCCESS);

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;
	state.manifest_stack[0].manifest_component_id = exp_root_component_id;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_FAIL_CONDITION, retval);
}

void test_seq_execution_process_dependency_seq_completion_failed(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_valid_envelope.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_valid_envelope.len);

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(
		suit_cose_sha256,
		&exp_envelope_digest,
		&exp_valid_manifest,
		SUIT_SUCCESS);

	__cmock_suit_plat_authorize_unsigned_manifest_ExpectAndReturn(NULL, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_unsigned_manifest_IgnoreArg_manifest_component_id();
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&unknown_manifest_component_id, &exp_component_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_component_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&exp_component_id_handle);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &unknown_manifest_component_id, 0x10, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_component_id, &unknown_manifest_component_id, SUIT_SEQ_PAYLOAD_FETCH, SUIT_SUCCESS);

	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_vid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &unknown_manifest_component_id, exp_valid_envelope.value, exp_valid_envelope.len, SUIT_ERR_OVERFLOW);

	__cmock_suit_plat_release_component_handle_ExpectAndReturn(exp_component_id_handle, SUIT_SUCCESS);

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;
	state.manifest_stack[0].manifest_component_id = exp_root_component_id;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_OVERFLOW, retval);
}

void test_seq_execution_process_dependency_corrupted_component_stack(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_valid_envelope.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_valid_envelope.len);

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(
		suit_cose_sha256,
		&exp_envelope_digest,
		&exp_valid_manifest,
		SUIT_SUCCESS);

	__cmock_suit_plat_authorize_unsigned_manifest_ExpectAndReturn(NULL, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_unsigned_manifest_IgnoreArg_manifest_component_id();
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&unknown_manifest_component_id, &exp_component_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_component_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&exp_component_id_handle);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &unknown_manifest_component_id, 0x10, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_component_id, &unknown_manifest_component_id, SUIT_SEQ_PAYLOAD_FETCH, SUIT_SUCCESS);

	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_vid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &unknown_manifest_component_id, exp_valid_envelope.value, exp_valid_envelope.len, SUIT_SUCCESS);

	__cmock_suit_plat_release_component_handle_ExpectAndReturn(exp_component_id_handle, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;
	state.manifest_stack[0].manifest_component_id = exp_root_component_id;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
}

void test_seq_execution_process_dependency_lazy_component_release(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_valid_envelope.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_valid_envelope.len);

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(
		suit_cose_sha256,
		&exp_envelope_digest,
		&exp_valid_manifest,
		SUIT_SUCCESS);

	__cmock_suit_plat_authorize_unsigned_manifest_ExpectAndReturn(NULL, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_unsigned_manifest_IgnoreArg_manifest_component_id();
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&unknown_manifest_component_id, &exp_component_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_component_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&exp_component_id_handle);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &unknown_manifest_component_id, 0x10, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_component_id, &unknown_manifest_component_id, SUIT_SEQ_PAYLOAD_FETCH, SUIT_SUCCESS);

	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_vid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &unknown_manifest_component_id, exp_valid_envelope.value, exp_valid_envelope.len, SUIT_SUCCESS);

	__cmock_suit_plat_release_component_handle_ExpectAndReturn(exp_component_id_handle, SUIT_ERR_AGAIN);

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;
	state.manifest_stack[0].manifest_component_id = exp_root_component_id;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_CRASH, retval);
}

void test_seq_execution_process_dependency_valid_dependency(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0b, /* uint(suit-directive-process-dependency) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_valid_envelope.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_valid_envelope.len);

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(
		suit_cose_sha256,
		&exp_envelope_digest,
		&exp_valid_manifest,
		SUIT_SUCCESS);

	__cmock_suit_plat_authorize_unsigned_manifest_ExpectAndReturn(NULL, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_unsigned_manifest_IgnoreArg_manifest_component_id();
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&unknown_manifest_component_id, &exp_component_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_component_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&exp_component_id_handle);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &unknown_manifest_component_id, 0x10, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_component_id, &unknown_manifest_component_id, SUIT_SEQ_PAYLOAD_FETCH, SUIT_SUCCESS);

	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_vid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &unknown_manifest_component_id, exp_valid_envelope.value, exp_valid_envelope.len, SUIT_SUCCESS);

	__cmock_suit_plat_release_component_handle_ExpectAndReturn(exp_component_id_handle, SUIT_SUCCESS);

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_dependency_components(&state, 1);
	/* Pretend that condition-dependency-integrity has been successfully executed. */
	state.components[0].integrity_checked = true;
	state.manifest_stack[0].manifest_component_id = exp_root_component_id;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}
