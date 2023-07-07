/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include <stdint.h>
#include <suit_manifest.h>
#include <suit_schedule_seq.h>
#include <bootstrap_envelope.h>
#include <bootstrap_seq.h>
#include "suit_platform/cmock_suit_platform.h"


static struct suit_processor_state state;

static uint8_t try_each_cmd[] = {
	0x84, /* list (4 elements - 2 commands) */
	0x14, /* uint(suit-directive-override-parameters) */
		0xa1, /* map (1 element) */
		0x01, /* uint(suit-parameter-vendor-identifier) */
		0x50, /* bytes (16) */
			0x76, 0x17, 0xda, 0xa5, 0x71, 0xfd, 0x5a, 0x85, /* RFC4122_UUID(nordicsemi.com) */
			0x8f, 0x94, 0xe2, 0x8d, 0x73, 0x5c, 0xe9, 0xf4,

	0x0f, /* uint(suit-directive-try-each) */
	0x83, /* list (3 elements - sequences) */
		0x43, /* bytes (3) */
			0x82, /* list (2 elements - 1 command) */
			0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
		0x43, /* bytes (3) */
			0x82, /* list (2 elements - 1 command) */
			0x17, /* uint(suit-directive-invoke) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
		0xf6, /* null */
};

static uint8_t try_each_silent_abort_cmd[] = {
	0x82, /* list (2 elements - 1 command) */

	0x0f, /* uint(suit-directive-try-each) */
	0x85, /* list (5 elements - sequences) */
		0x43, /* bytes (3) */
			0x82, /* list (2 elements - 1 command) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
		0x47, /* bytes (7) */
			0x82, /* list (2 elements - 1 command) */
			0x18, 0x20, /* uint(suit-directive-run-sequence) */
			0x43, /* bytes (3) */
				0x82, /* list (2 elements - 1 command) */
				0x0e, /* uint(suit-condition-abort) */
				0x00, /* uint(SUIT_Rep_Policy::None) */
		0x43, /* bytes (3) */
			0x82, /* list (2 elements - 1 command) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
		0x4b, /* bytes (11) */
			0x82, /* list (2 elements - 1 command) */
			0x18, 0x20, /* uint(suit-directive-run-sequence) */
			0x47, /* bytes (7) */
				0x82, /* list (2 elements - 1 command) */
				0x18, 0x20, /* uint(suit-directive-run-sequence) */
				0x43, /* bytes (3) */
					0x82, /* list (2 elements - 1 command) */
					0x0e, /* uint(suit-condition-abort) */
					0x00, /* uint(SUIT_Rep_Policy::None) */
		0x43, /* bytes (3) */
			0x82, /* list (2 elements - 1 command) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
};

static uint8_t try_each_nested_invalid_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
	0x0f, /* uint(suit-directive-try-each) */
	0x82, /* list (2 elements - sequences) */
		0x43, /* bytes (3) */
			0x82, /* list (2 elements - 1 command) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
		0x44, /* bytes (4) */
			0x82, /* list (2 elements - 1 command) */
			0x0f, /* uint(suit-directive-try-each) */
			0x81, /* list (1 elements - sequences) */
				0xf6, /* null */
};

static uint8_t try_each_component_overrides_cmd[] = {
	0x86, /* list (6 elements - 3 commands) */
	0x0c, /* uint(suit-directive-set-component-index) */
	0xf5, /* true */

	0x14, /* uint(suit-directive-override-parameters) */
		0xa1, /* map (1 element) */
		0x01, /* uint(suit-parameter-vendor-identifier) */
		0x50, /* bytes (16) */
			0x76, 0x17, 0xda, 0xa5, 0x71, 0xfd, 0x5a, 0x85, /* RFC4122_UUID(nordicsemi.com) */
			0x8f, 0x94, 0xe2, 0x8d, 0x73, 0x5c, 0xe9, 0xf4,

	0x0f, /* uint(suit-directive-try-each) */
	0x83, /* list (3 elements - sequences) */
		0x43, /* bytes (3) */
			0x82, /* list (2 elements - 1 command) */
			0x17, /* uint(suit-directive-invoke) */
			0x00, /* uint(SUIT_Rep_Policy::None) */

		0x4d, /* bytes (13) */
			0x86, /* list (6 elements - 3 commands) */
			0x0c, /* uint(suit-directive-set-component-index) */
			0x00, /* uint(0) */

			0x18, 0x20, /* uint(suit-directive-run-sequence) */
			0x45, /* bytes (5) */
				0x84, /* list (4 elements - 2 commands) */
				0x0c, /* uint(suit-directive-set-component-index) */
				0xf5, /* true */

				0x01, /* uint(suit-condition-vendor-identifier) */
				0x00, /* uint(SUIT_Rep_Policy::None) */

			0x17, /* uint(suit-directive-invoke) */
			0x00, /* uint(SUIT_Rep_Policy::None) */

		0x43, /* bytes (3) */
			0x82, /* list (2 elements - 1 command) */
			0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
};


static int process_sequence(struct suit_processor_state *state, struct zcbor_string *seq)
{
	/* The bootstrap_envelope_* APIs create a single manifest on the stack. */
	struct suit_manifest_state *manifest = &state->manifest_stack[0];

	/* This test verifies generic execution, so any non-shared sequence could be used. */
	bootstrap_envelope_sequence(state, SUIT_SEQ_INVOKE, seq);

	int ret = suit_schedule_execution(state, manifest, SUIT_SEQ_INVOKE);
	if (ret == SUIT_ERR_AGAIN) {
		ret = suit_process_scheduled(state);
	}

	return ret;
}


void setUp(void)
{
	memset(&state, 0, sizeof(state));

	int err = suit_manifest_params_init(state.components, ZCBOR_ARRAY_SIZE(state.components));
	if (err == SUIT_ERR_ORDER) {
		/* Allow to call init even if the manifest module is already initialized. */
		err = SUIT_SUCCESS;
	}

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, err, "Unable to initialize SUIT processor");

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);
}

void test_try_each_first_succeeds(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	memcpy(cmd_buf, try_each_cmd, sizeof(try_each_cmd));
	content_size = sizeof(try_each_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;

	__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
	retval = process_sequence(&state, &invoke_seq);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_second_succeeds(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	memcpy(cmd_buf, try_each_cmd, sizeof(try_each_cmd));
	content_size = sizeof(try_each_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;

	__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
	retval = process_sequence(&state, &invoke_seq);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_default_succeeds(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	memcpy(cmd_buf, try_each_cmd, sizeof(try_each_cmd));
	content_size = sizeof(try_each_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;

	__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_FAIL_CONDITION);
	retval = process_sequence(&state, &invoke_seq);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_second_aborts(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	memcpy(cmd_buf, try_each_cmd, sizeof(try_each_cmd));
	content_size = sizeof(try_each_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;

	__cmock_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
	retval = process_sequence(&state, &invoke_seq);
	TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_out_of_cases(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	memcpy(cmd_buf, try_each_silent_abort_cmd, sizeof(try_each_silent_abort_cmd));
	content_size = sizeof(try_each_silent_abort_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;

	retval = process_sequence(&state, &invoke_seq);
	TEST_ASSERT_EQUAL(SUIT_ERR_CRASH, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_nested_invalid(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	memcpy(cmd_buf, try_each_nested_invalid_cmd, sizeof(try_each_nested_invalid_cmd));
	content_size = sizeof(try_each_nested_invalid_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;

	retval = process_sequence(&state, &invoke_seq);
	TEST_ASSERT_EQUAL(SUIT_ERR_DECODING, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_component_overrides(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_components(&state, 4);

	memcpy(cmd_buf, try_each_component_overrides_cmd, sizeof(try_each_component_overrides_cmd));
	content_size = sizeof(try_each_component_overrides_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;

	/* 1st case - fail */
	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_FAIL_CONDITION);
	/* 2nd case - override component to a single one */
	/* sequence in 2nd case - override component to all components */
	for (size_t seq_i = 0; seq_i < 4; seq_i++) {
		/* sequence in 2nd case - execute 4 times command for each component. */
		__cmock_suit_plat_check_vid_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + seq_i, NULL, SUIT_SUCCESS);
		__cmock_suit_plat_check_vid_IgnoreArg_vid_uuid();
	}
	/* 2nd case - fail*/
	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_FAIL_CONDITION);
	/* 3rd case - succeed, component set by the 2nd case */
	__cmock_suit_plat_check_vid_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_IgnoreArg_vid_uuid();

	/* 2nd component, 1st case - succeed */
	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + 1, NULL, SUIT_SUCCESS);

	/* 3rd component, 1st case - fail */
	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + 2, NULL, SUIT_FAIL_CONDITION);
	/* 3rd component, 2nd case - fail (executed for all components, but the first hard failure ends the whole sequence)*/
	__cmock_suit_plat_check_vid_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_FAIL_CONDITION);
	__cmock_suit_plat_check_vid_IgnoreArg_vid_uuid();
	/* 3rd component, 3rd case - succeed, component set bythe 2nd case */
	__cmock_suit_plat_check_vid_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_IgnoreArg_vid_uuid();

	/* 4th component, 1st case - fail */
	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + 3, NULL, SUIT_FAIL_CONDITION);
	/* 4th component, 2nd case - fail (executed for all components, but the first hard failure ends the whole sequence)*/
	__cmock_suit_plat_check_vid_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_FAIL_CONDITION);
	__cmock_suit_plat_check_vid_IgnoreArg_vid_uuid();
	/* 4th component, 3rd case - fail, component set bythe 2nd case */
	__cmock_suit_plat_check_vid_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
	__cmock_suit_plat_check_vid_IgnoreArg_vid_uuid();

	retval = process_sequence(&state, &invoke_seq);
	TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}


/* It is required to be added to each test. That is because unity's
 * main may return nonzero, while zephyr's main currently must
 * return 0 in all cases (other values are reserved).
 */
extern int unity_main(void);

int main(void)
{
	(void)unity_main();

	return 0;
}
