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

void test_seq_execution_fetch_no_uri(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x15, /* uint(suit-directive-fetch) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_UNAVAILABLE_PAYLOAD, retval);
}

void test_seq_execution_fetch_external_uri(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x15, /* uint(suit-parameter-uri) */
				0x78, 0x1A, /* text (26 characters) */
				'h', 't', 't', 'p', ':', '/', '/',
				'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 'c', 'o', 'm',
				'/', 'a', 'p', 'p', '.', 'b', 'i', 'n',
			0x15, /* uint(suit-directive-fetch) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};
	struct zcbor_string exp_uri = {
		.value = "http://example.com/app.bin",
		.len = strlen("http://example.com/app.bin"),
	};
	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_fetch_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_uri, SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}

void test_seq_execution_fetch_internal_uri(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x15, /* uint(suit-parameter-uri) */
				0x78, 0x1A, /* text (26 characters) */
				'h', 't', 't', 'p', ':', '/', '/',
				'e', 'x', 'a', 'm', 'p', 'l', 'e', '.', 'c', 'o', 'm',
				'/', 'a', 'p', 'p', '.', 'b', 'i', 'n',
			0x15, /* uint(suit-directive-fetch) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};
	struct zcbor_string exp_uri = {
		.value = "http://example.com/app.bin",
		.len = strlen("http://example.com/app.bin"),
	};
	struct zcbor_string exp_payload = {
		.value = "My application",
		.len = sizeof("My application"),
	};
	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	state.manifest_stack[0].integrated_payloads_count = 1;
	state.manifest_stack[0].integrated_payloads[0].key = exp_uri;
	state.manifest_stack[0].integrated_payloads[0].payload = exp_payload;

	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_payload, SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}
