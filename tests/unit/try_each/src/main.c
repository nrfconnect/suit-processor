/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unity.h>
#include <stdint.h>
#include <suit.h>
#include <bootstrap_envelope.h>
#include <bootstrap_seq.h>
#include "suit_platform/mock_suit_platform.h"


static struct suit_processor_state state;

static uint8_t try_each_cmd[] = {
	0x84, /* list (4 elements - 2 commands) */
	0x14, /* uint(suit-directive-override-parameters) */
		0xa1, /* map (1 element) */
		0x01, /* uint(suit-parameter-vendor-identifier) */
		0x50, /* bytes (16) */
			0x2b, 0xdc, 0x1c, 0x07, 0xe0, 0xd1, 0x54, 0x84, /* RFC4122_UUID(nordicsemi.no) */
			0xbe, 0x50, 0x63, 0x17, 0x4d, 0x5a, 0x74, 0xc3,

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
			0x2b, 0xdc, 0x1c, 0x07, 0xe0, 0xd1, 0x54, 0x84, /* RFC4122_UUID(nordicsemi.no) */
			0xbe, 0x50, 0x63, 0x17, 0x4d, 0x5a, 0x74, 0xc3,

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


void test_try_each_prepare_reset_state(void)
{
	/* It is required to call platform reset API in case of SUIT processor state reset. */
	__wrap_suit_plat_reset_components_Expect();

	suit_reset_state(&state);
}

void test_try_each_first_succeeds(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, try_each_cmd, sizeof(try_each_cmd));
	content_size = sizeof(try_each_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;
	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);

	__wrap_suit_plat_check_vid_IgnoreAndReturn(SUIT_SUCCESS);
	retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_second_succeeds(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, try_each_cmd, sizeof(try_each_cmd));
	content_size = sizeof(try_each_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;
	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);

	__wrap_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
	__wrap_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
	retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_default_succeeds(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, try_each_cmd, sizeof(try_each_cmd));
	content_size = sizeof(try_each_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;
	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);

	__wrap_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
	__wrap_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_FAIL_CONDITION);
	retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_second_aborts(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, try_each_cmd, sizeof(try_each_cmd));
	content_size = sizeof(try_each_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;
	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);

	__wrap_suit_plat_check_vid_IgnoreAndReturn(SUIT_FAIL_CONDITION);
	__wrap_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
	retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_out_of_cases(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, try_each_silent_abort_cmd, sizeof(try_each_silent_abort_cmd));
	content_size = sizeof(try_each_silent_abort_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;
	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);

	retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_ERR_CRASH, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_nested_invalid(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	memcpy(cmd_buf, try_each_nested_invalid_cmd, sizeof(try_each_nested_invalid_cmd));
	content_size = sizeof(try_each_nested_invalid_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;
	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);

	retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_ERR_DECODING, retval);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

void test_try_each_component_overrides(void)
{
	int retval;
	struct zcbor_string invoke_seq;
	uint8_t cmd_buf[256];
	size_t content_size = 0;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 4);

	memcpy(cmd_buf, try_each_component_overrides_cmd, sizeof(try_each_component_overrides_cmd));
	content_size = sizeof(try_each_component_overrides_cmd);

	invoke_seq.value = cmd_buf;
	invoke_seq.len = content_size;
	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);

	/* 1st case - fail */
	__wrap_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_FAIL_CONDITION);
	/* 2nd case - override component to a single one */
	/* sequence in 2nd case - override component to all components */
	for (size_t seq_i = 0; seq_i < 4; seq_i++) {
		/* sequence in 2nd case - execute 4 times command for each component. */
		__wrap_suit_plat_check_vid_ExpectAndReturn(NULL, ASSIGNED_COMPONENT_HANDLE + seq_i, SUIT_SUCCESS);
		__wrap_suit_plat_check_vid_IgnoreArg_vid_uuid();
	}
	/* 2nd case - fail*/
	__wrap_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_FAIL_CONDITION);
	/* 3rd case - succeed, component set by the 2nd case */
	__wrap_suit_plat_check_vid_ExpectAndReturn(NULL, ASSIGNED_COMPONENT_HANDLE, SUIT_SUCCESS);
	__wrap_suit_plat_check_vid_IgnoreArg_vid_uuid();

	/* 2nd component, 1st case - succeed */
	__wrap_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + 1, NULL, SUIT_SUCCESS);

	/* 3rd component, 1st case - fail */
	__wrap_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + 2, NULL, SUIT_FAIL_CONDITION);
	/* 3rd component, 2nd case - fail (executed for all components, but the first hard failure ends the whole sequence)*/
	__wrap_suit_plat_check_vid_ExpectAndReturn(NULL, ASSIGNED_COMPONENT_HANDLE, SUIT_FAIL_CONDITION);
	__wrap_suit_plat_check_vid_IgnoreArg_vid_uuid();
	/* 3rd component, 3rd case - succeed, component set bythe 2nd case */
	__wrap_suit_plat_check_vid_ExpectAndReturn(NULL, ASSIGNED_COMPONENT_HANDLE, SUIT_SUCCESS);
	__wrap_suit_plat_check_vid_IgnoreArg_vid_uuid();

	/* 4th component, 1st case - fail */
	__wrap_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + 3, NULL, SUIT_FAIL_CONDITION);
	/* 4th component, 2nd case - fail (executed for all components, but the first hard failure ends the whole sequence)*/
	__wrap_suit_plat_check_vid_ExpectAndReturn(NULL, ASSIGNED_COMPONENT_HANDLE, SUIT_FAIL_CONDITION);
	__wrap_suit_plat_check_vid_IgnoreArg_vid_uuid();
	/* 4th component, 3rd case - fail, component set bythe 2nd case */
	__wrap_suit_plat_check_vid_ExpectAndReturn(NULL, ASSIGNED_COMPONENT_HANDLE, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
	__wrap_suit_plat_check_vid_IgnoreArg_vid_uuid();

	retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval);
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
