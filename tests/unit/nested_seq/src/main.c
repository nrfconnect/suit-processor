/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include <stdint.h>
#include <suit.h>
#include <bootstrap_envelope.h>
#include <bootstrap_seq.h>
#include "suit_platform/cmock_suit_platform.h"


static struct suit_processor_state state;

static struct zcbor_string exp_manifest_id = {
	.value = NULL,
	.len = 0,
};

static uint8_t invoke_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
	0x17, /* uint(suit-directive-invoke) */
	0x00, /* uint(SUIT_Rep_Policy::None) */
};

static uint8_t invoke_hard_condition_cmd[] = {
	0x84, /* list (4 elements - 2 commands) */
	0x14, /* uint(suit-directive-override-parameters) */
		0xa1, /* map (1 element) */
		0x01, /* uint(suit-parameter-vendor-identifier) */
		0x50, /* bytes (16) */
			0x2b, 0xdc, 0x1c, 0x07, 0xe0, 0xd1, 0x54, 0x84, /* RFC4122_UUID(nordicsemi.no) */
			0xbe, 0x50, 0x63, 0x17, 0x4d, 0x5a, 0x74, 0xc3,

	0x01, /* uint(suit-condition-vendor-identifier) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};

static uint8_t invoke_soft_condition_cmd[] = {
	0x84, /* list (4 elements - 2 commands) */
	0x18, 0x20, /* uint(suit-directive-run-sequence) */
	0x58, 0x1b, /* bytes(27) */

		0x86, /* list (6 elements - 3 commands) */
		0x14, /* uint(suit-directive-override-parameters) */
			0xa2, /* map (2 elements) */
			0x0d, /* uint(suit-parameter-soft-failure) */
			0xf5, /* True */

			0x01, /* uint(suit-parameter-vendor-identifier) */
			0x50, /* bytes (16) */
				0x2b, 0xdc, 0x1c, 0x07, 0xe0, 0xd1, 0x54, 0x84, /* RFC4122_UUID(nordicsemi.no) */
				0xbe, 0x50, 0x63, 0x17, 0x4d, 0x5a, 0x74, 0xc3,

		0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
		0x17, /* uint(suit-directive-invoke) */
			0x00, /* uint(SUIT_Rep_Policy::None) */

	0x17, /* uint(suit-directive-invoke) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};

static uint8_t invoke_nested_soft_and_hard_condition_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
	0x18, 0x20, /* uint(suit-directive-run-sequence) */
	0x58, 0x29, /* bytes(41) */

	0x82, /* list (2 elements - 1 command) */
	0x18, 0x20, /* uint(suit-directive-run-sequence) */
	0x58, 0x24, /* bytes(36) */

	0x86, /* list (6 elements - 3 commands) */
	0x18, 0x20, /* uint(suit-directive-run-sequence) */
	0x58, 0x1b, /* bytes(27) */

		0x86, /* list (6 elements - 3 commands) */
		0x14, /* uint(suit-directive-override-parameters) */
			0xa2, /* map (2 elements) */
			0x0d, /* uint(suit-parameter-soft-failure) */
			0xf5, /* True */

			0x01, /* uint(suit-parameter-vendor-identifier) */
			0x50, /* bytes (16) */
				0x2b, 0xdc, 0x1c, 0x07, 0xe0, 0xd1, 0x54, 0x84, /* RFC4122_UUID(nordicsemi.no) */
				0xbe, 0x50, 0x63, 0x17, 0x4d, 0x5a, 0x74, 0xc3,

		0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
		0x17, /* uint(suit-directive-invoke) */
			0x00, /* uint(SUIT_Rep_Policy::None) */

	0x01, /* uint(suit-condition-vendor-identifier) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
	0x17, /* uint(suit-directive-invoke) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};

static uint8_t invoke_soft_condition_series_cmd[] = {
	0x8C, /* list (12 elements - 6 commands) */

	0x0c, /* uint(suit-directive-set-component-index) */
	0xf5, /* true */

	0x18, 0x20, /* uint(suit-directive-run-sequence) */
	0x58, 0x1b, /* bytes(27) */

		0x86, /* list (6 elements - 3 commands) */
		0x14, /* uint(suit-directive-override-parameters) */
			0xa2, /* map (2 elements) */
			0x0d, /* uint(suit-parameter-soft-failure) */
			0xf5, /* True */

			0x01, /* uint(suit-parameter-vendor-identifier) */
			0x50, /* bytes (16) */
				0x2b, 0xdc, 0x1c, 0x07, 0xe0, 0xd1, 0x54, 0x84, /* RFC4122_UUID(nordicsemi.no) */
				0xbe, 0x50, 0x63, 0x17, 0x4d, 0x5a, 0x74, 0xc3,

		0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
		0x17, /* uint(suit-directive-invoke) */
			0x00, /* uint(SUIT_Rep_Policy::None) */

	0x18, 0x20, /* uint(suit-directive-run-sequence) */
	0x58, 0x1b, /* bytes(27) */

		0x86, /* list (6 elements - 3 commands) */
		0x14, /* uint(suit-directive-override-parameters) */
			0xa2, /* map (2 elements) */
			0x0d, /* uint(suit-parameter-soft-failure) */
			0xf5, /* True */

			0x01, /* uint(suit-parameter-vendor-identifier) */
			0x50, /* bytes (16) */
				0x2b, 0xdc, 0x1c, 0x07, 0xe0, 0xd1, 0x54, 0x84, /* RFC4122_UUID(nordicsemi.no) */
				0xbe, 0x50, 0x63, 0x17, 0x4d, 0x5a, 0x74, 0xc3,

		0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
		0x17, /* uint(suit-directive-invoke) */
			0x00, /* uint(SUIT_Rep_Policy::None) */

	0x18, 0x20, /* uint(suit-directive-run-sequence) */
	0x58, 0x1b, /* bytes(27) */

		0x86, /* list (6 elements - 3 commands) */
		0x14, /* uint(suit-directive-override-parameters) */
			0xa2, /* map (2 elements) */
			0x0d, /* uint(suit-parameter-soft-failure) */
			0xf5, /* True */

			0x01, /* uint(suit-parameter-vendor-identifier) */
			0x50, /* bytes (16) */
				0x2b, 0xdc, 0x1c, 0x07, 0xe0, 0xd1, 0x54, 0x84, /* RFC4122_UUID(nordicsemi.no) */
				0xbe, 0x50, 0x63, 0x17, 0x4d, 0x5a, 0x74, 0xc3,

		0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
		0x17, /* uint(suit-directive-invoke) */
			0x00, /* uint(SUIT_Rep_Policy::None) */

	0x01, /* uint(suit-condition-vendor-identifier) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
	0x17, /* uint(suit-directive-invoke) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};

static uint8_t nested_component_overrides_expand_cmd[] = {
	0x8A, /* list (10 elements - 5 commands) */

	0x0c, /* uint(suit-directive-set-component-index) */
	0x00, /* uint(0) */

	0x14, /* uint(suit-directive-override-parameters) */
		0xa1, /* map (1 element) */
		0x01, /* uint(suit-parameter-vendor-identifier) */
		0x50, /* bytes (16) */
			0x2b, 0xdc, 0x1c, 0x07, 0xe0, 0xd1, 0x54, 0x84, /* RFC4122_UUID(nordicsemi.no) */
			0xbe, 0x50, 0x63, 0x17, 0x4d, 0x5a, 0x74, 0xc3,

	0x18, 0x20, /* uint(suit-directive-run-sequence) */
	0x45, /* bytes(5) */

		0x84, /* list (4 elements - 2 commands) */
		0x0c, /* uint(suit-directive-set-component-index) */
		0xf5, /* true */

		0x17, /* uint(suit-directive-invoke) */
			0x00, /* uint(SUIT_Rep_Policy::None) */

	0x18, 0x20, /* uint(suit-directive-run-sequence) */
	0x43, /* bytes(3) */

		0x82, /* list (2 elements - 1 command) */
		0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */

	0x17, /* uint(suit-directive-invoke) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};

static uint8_t nested_component_overrides_limit_cmd[] = {
	0x8A, /* list (10 elements - 5 commands) */

	0x0c, /* uint(suit-directive-set-component-index) */
	0xf5, /* true */

	0x14, /* uint(suit-directive-override-parameters) */
		0xa1, /* map (1 element) */
		0x01, /* uint(suit-parameter-vendor-identifier) */
		0x50, /* bytes (16) */
			0x2b, 0xdc, 0x1c, 0x07, 0xe0, 0xd1, 0x54, 0x84, /* RFC4122_UUID(nordicsemi.no) */
			0xbe, 0x50, 0x63, 0x17, 0x4d, 0x5a, 0x74, 0xc3,

	0x18, 0x20, /* uint(suit-directive-run-sequence) */
	0x45, /* bytes(5) */

		0x84, /* list (4 elements - 2 commands) */
		0x0c, /* uint(suit-directive-set-component-index) */
		0x00, /* uint(0) */

		0x17, /* uint(suit-directive-invoke) */
			0x00, /* uint(SUIT_Rep_Policy::None) */

	0x18, 0x20, /* uint(suit-directive-run-sequence) */
	0x43, /* bytes(3) */

		0x82, /* list (2 elements - 1 command) */
		0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */

	0x17, /* uint(suit-directive-invoke) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};


void test_nested_seq_prepare_reset_state(void)
{
	suit_reset_state(&state);
}

void test_nested_seq_invoke_successful(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, invoke_cmd, sizeof(invoke_cmd));
	content_size = sizeof(invoke_cmd);

	for (size_t depth = 0; depth <= SUIT_MAX_SEQ_DEPTH; depth++) {
		invoke_seq.value = cmd_buf;
		invoke_seq.len = content_size;
		bootstrap_envelope_reset_step(&state);
		bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
		__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

		if (depth < SUIT_MAX_SEQ_DEPTH) {
			__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
			__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, NULL, 0, SUIT_SUCCESS);
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
		} else {
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_ERR_OVERFLOW, retval);
		}
		TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

		resetTest();
		content_size = bootsrap_seq_hdr_run_sequence(cmd_buf, sizeof(cmd_buf), content_size);
	};
}

void test_nested_seq_invoke_failed(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, invoke_cmd, sizeof(invoke_cmd));
	content_size = sizeof(invoke_cmd);

	for (size_t depth = 0; depth <= SUIT_MAX_SEQ_DEPTH; depth++) {
		invoke_seq.value = cmd_buf;
		invoke_seq.len = content_size;
		bootstrap_envelope_reset_step(&state);
		bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
		__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

		if (depth < SUIT_MAX_SEQ_DEPTH) {
			__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
		} else {
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_ERR_OVERFLOW, retval);
		}
		TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

		resetTest();
		content_size = bootsrap_seq_hdr_run_sequence(cmd_buf, sizeof(cmd_buf), content_size);
	};
}

void test_nested_seq_condition_hard_successful(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, invoke_hard_condition_cmd, sizeof(invoke_hard_condition_cmd));
	content_size = sizeof(invoke_hard_condition_cmd);

	for (size_t depth = 0; depth <= SUIT_MAX_SEQ_DEPTH; depth++) {
		invoke_seq.value = cmd_buf;
		invoke_seq.len = content_size;
		bootstrap_envelope_reset_step(&state);
		bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
		__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

		if (depth < SUIT_MAX_SEQ_DEPTH) {
			__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
			__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, NULL, 0, SUIT_SUCCESS);
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
		} else {
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_ERR_OVERFLOW, retval);
		}
		TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

		resetTest();
		content_size = bootsrap_seq_hdr_run_sequence(cmd_buf, sizeof(cmd_buf), content_size);
	};
}

void test_nested_seq_condition_hard_failed(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, invoke_hard_condition_cmd, sizeof(invoke_hard_condition_cmd));
	content_size = sizeof(invoke_hard_condition_cmd);

	for (size_t depth = 0; depth <= SUIT_MAX_SEQ_DEPTH; depth++) {
		invoke_seq.value = cmd_buf;
		invoke_seq.len = content_size;
		bootstrap_envelope_reset_step(&state);
		bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
		__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

		if (depth < SUIT_MAX_SEQ_DEPTH) {
			__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_FAIL_CONDITION, retval);
		} else {
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_ERR_OVERFLOW, retval);
		}
		TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

		resetTest();
		content_size = bootsrap_seq_hdr_run_sequence(cmd_buf, sizeof(cmd_buf), content_size);
	};
}

void test_nested_seq_condition_soft_successful(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, invoke_soft_condition_cmd, sizeof(invoke_soft_condition_cmd));
	content_size = sizeof(invoke_soft_condition_cmd);

	for (size_t depth = 0; depth <= (SUIT_MAX_SEQ_DEPTH - 1); depth++) {
		invoke_seq.value = cmd_buf;
		invoke_seq.len = content_size;
		bootstrap_envelope_reset_step(&state);
		bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
		__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

		if (depth < (SUIT_MAX_SEQ_DEPTH - 1)) {
			__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
			__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
			__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
			__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, NULL, 0, SUIT_SUCCESS);
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
		} else {
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_ERR_OVERFLOW, retval);
		}
		TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

		resetTest();
		content_size = bootsrap_seq_hdr_run_sequence(cmd_buf, sizeof(cmd_buf), content_size);
	};
}

void test_nested_seq_condition_soft_failed(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, invoke_soft_condition_cmd, sizeof(invoke_soft_condition_cmd));
	content_size = sizeof(invoke_soft_condition_cmd);

	for (size_t depth = 0; depth <= (SUIT_MAX_SEQ_DEPTH - 1); depth++) {
		invoke_seq.value = cmd_buf;
		invoke_seq.len = content_size;
		bootstrap_envelope_reset_step(&state);
		bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
		__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

		if (depth < (SUIT_MAX_SEQ_DEPTH - 1)) {
			__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
			__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
			__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, NULL, 0, SUIT_SUCCESS);
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
		} else {
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_ERR_OVERFLOW, retval);
		}
		TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

		resetTest();
		content_size = bootsrap_seq_hdr_run_sequence(cmd_buf, sizeof(cmd_buf), content_size);
	};
}

void test_nested_seq_condition_soft_aborted(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, invoke_soft_condition_cmd, sizeof(invoke_soft_condition_cmd));
	content_size = sizeof(invoke_soft_condition_cmd);

	for (size_t depth = 0; depth <= (SUIT_MAX_SEQ_DEPTH - 1); depth++) {
		invoke_seq.value = cmd_buf;
		invoke_seq.len = content_size;
		bootstrap_envelope_reset_step(&state);
		bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
		__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

		if (depth < (SUIT_MAX_SEQ_DEPTH - 1)) {
			__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
		} else {
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_ERR_OVERFLOW, retval);
		}
		TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

		resetTest();
		content_size = bootsrap_seq_hdr_run_sequence(cmd_buf, sizeof(cmd_buf), content_size);
	};
}

void test_nested_seq_condition_soft_failed_hard_succeed(void)
{
	int retval;
	struct zcbor_string invoke_seq;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	invoke_seq.value = invoke_nested_soft_and_hard_condition_cmd;
	invoke_seq.len = sizeof(invoke_nested_soft_and_hard_condition_cmd);

	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);

	__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
	__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, NULL, 0, SUIT_SUCCESS);
	retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_nested_seq_condition_soft_failed_hard_failed(void)
{
	int retval;
	struct zcbor_string invoke_seq;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	invoke_seq.value = invoke_nested_soft_and_hard_condition_cmd;
	invoke_seq.len = sizeof(invoke_nested_soft_and_hard_condition_cmd);

	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);

	__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
	__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
	retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_FAIL_CONDITION, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_nested_seq_condition_soft_series_mixed_hard_succeed(void)
{
	int retval;
	struct zcbor_string invoke_seq;

	invoke_seq.value = invoke_soft_condition_series_cmd;
	invoke_seq.len = sizeof(invoke_soft_condition_series_cmd);

	for (size_t i = 0; i < 8; i++) {
		bootstrap_envelope_empty(&state);
		bootstrap_envelope_components(&state, 1);
		bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
		__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

		for (size_t seq = 0; seq < 3; seq++) {
			if (i & (1 << seq)) {
				/* Fail soft sequence */
				__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
			} else {
				/* Pass soft sequence */
				__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
				__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
			}
		}
		/* Pass hard sequence */
		__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
		__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
		__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, NULL, 0, SUIT_SUCCESS);

		retval = suit_process_manifest(&state, SUIT_INVOKE);
		TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
		TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

		resetTest();
	}
}

void test_nested_seq_condition_soft_series_mixed_hard_failed(void)
{
	int retval;
	struct zcbor_string invoke_seq;

	invoke_seq.value = invoke_soft_condition_series_cmd;
	invoke_seq.len = sizeof(invoke_soft_condition_series_cmd);

	for (size_t i = 0; i < 8; i++) {
		bootstrap_envelope_empty(&state);
		bootstrap_envelope_components(&state, 1);
		bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
		__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

		for (size_t seq = 0; seq < 3; seq++) {
			if (i & (1 << seq)) {
				/* Fail soft sequence */
				__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
			} else {
				/* Pass soft sequence */
				__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
				__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
			}
		}
		/* Fail hard sequence */
		__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);

		retval = suit_process_manifest(&state, SUIT_INVOKE);
		TEST_ASSERT_EQUAL(SUIT_FAIL_CONDITION, retval);
		TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

		resetTest();
	}
}

void test_nested_seq_condition_soft_series_aborted(void)
{
	int retval;
	struct zcbor_string invoke_seq;

	invoke_seq.value = invoke_soft_condition_series_cmd;
	invoke_seq.len = sizeof(invoke_soft_condition_series_cmd);

	for (size_t i = 0; i < 8; i++) {
		bootstrap_envelope_empty(&state);
		bootstrap_envelope_components(&state, 1);
		bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
		__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

		for (size_t seq = 0; seq < 3; seq++) {
			if (i & (1 << seq)) {
				/* Abort soft sequence */
				__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
				break;
			} else {
				/* Pass soft sequence */
				__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
				__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
			}
		}
		if (i == 0) {
			/* Abort hard sequence */
			__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
		}

		retval = suit_process_manifest(&state, SUIT_INVOKE);
		TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
		TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

		resetTest();
	}
}

void test_nested_seq_condition_soft_series_mixed_hard_succeed_multiple_components(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	size_t seq = 0;
	size_t comp_seq = 0;

	invoke_seq.value = invoke_soft_condition_series_cmd;
	invoke_seq.len = sizeof(invoke_soft_condition_series_cmd);

	for (size_t cond_fail = 0; cond_fail < 8; cond_fail++) {
		for (size_t component_fail = 0; component_fail < 4; component_fail++) {
			bootstrap_envelope_empty(&state);
			bootstrap_envelope_components(&state, 2);
			bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
			__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

			for (seq = 0; seq < 3; seq++) {
				for (comp_seq = 0; comp_seq < 2; comp_seq++) {
					if ((cond_fail & (1 << seq)) && (component_fail & (1 << comp_seq))) {
						/* Fail soft sequence */
						__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
					} else {
						/* Pass soft sequence */
						__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
						__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + comp_seq, NULL, SUIT_SUCCESS);
					}
				}
			}
			for (comp_seq = 0; comp_seq < 2; comp_seq++) {
				/* Pass hard sequence */
				__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
				__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + comp_seq, NULL, SUIT_SUCCESS);
			}

			__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, NULL, 0, SUIT_SUCCESS);
			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
			TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

			resetTest();
		}
	}
}

void test_nested_seq_condition_soft_series_mixed_aborted_multiple_components(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	size_t seq = 0;
	size_t comp_seq = 0;

	invoke_seq.value = invoke_soft_condition_series_cmd;
	invoke_seq.len = sizeof(invoke_soft_condition_series_cmd);

	for (size_t cond_fail = 0; cond_fail < 8; cond_fail++) {
		for (size_t component_fail = 0; component_fail < 4; component_fail++) {
			bootstrap_envelope_empty(&state);
			bootstrap_envelope_components(&state, 2);
			bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
			__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

			bool group_failed = false;
			for (seq = 0; seq < 3; seq++) {
				for (comp_seq = 0; comp_seq < 2; comp_seq++) {
					if ((cond_fail & (1 << seq)) && (component_fail & (1 << comp_seq))) {
						/* Fail soft sequence */
						__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
						group_failed = true;
						break;
					} else {
						/* Pass soft sequence */
						__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
						__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + comp_seq, NULL, SUIT_SUCCESS);
					}
				}
				if (group_failed) {
					break;
				}
			}
			if (!group_failed) {
				/* Executed only once, for the first component */
				__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
			}

			retval = suit_process_manifest(&state, SUIT_INVOKE);
			TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
			TEST_ASSERT_EQUAL(state.seq_stack_height, 0);

			resetTest();
		}
	}
}

void test_nested_seq_component_overrides_expand(void)
{
	int retval;
	struct zcbor_string invoke_seq;

	invoke_seq.value = nested_component_overrides_expand_cmd;
	invoke_seq.len = sizeof(nested_component_overrides_expand_cmd);

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, SUIT_MAX_NUM_COMPONENTS);
	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
	__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

	/* Component index set to 0, and then to true - expect 4 different calls. */
	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + i, NULL, SUIT_SUCCESS);
	}

	/* Component index remains unmodified - expect 1 call. */
	__cmock_suit_plat_check_vid_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_IgnoreArg_vid_uuid();

	/* Outside run-sequence body - expect 1 call. */
	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, NULL, 0, SUIT_SUCCESS);

	retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_nested_seq_component_overrides_limit(void)
{
	int retval;
	struct zcbor_string invoke_seq;

	invoke_seq.value = nested_component_overrides_limit_cmd;
	invoke_seq.len = sizeof(nested_component_overrides_limit_cmd);

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, SUIT_MAX_NUM_COMPONENTS);
	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
	__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, 0, SUIT_SUCCESS);

	/* Component index set to true, and then to 0 - expect 4 identical calls. */
	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
	}

	/* Component index remains unmodified - expect 4 different calls. */
	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		__cmock_suit_plat_check_vid_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + i, NULL, SUIT_SUCCESS);
		__cmock_suit_plat_check_vid_IgnoreArg_vid_uuid();
	}

	/* Outside run-sequence body - expect 4 different calls. */
	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + i, NULL, SUIT_SUCCESS);
	}

	__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_INVOKE, &exp_manifest_id, NULL, 0, SUIT_SUCCESS);
	retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}


/* It is required to be added to each test. That is because unity is using
 * different main signature (returns int) and zephyr expects main which does
 * not return value.
 */
extern int unity_main(void);

void main(void)
{
	(void)unity_main();
}
