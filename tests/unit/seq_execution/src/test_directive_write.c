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

void test_seq_execution_write_no_content(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x12, /* uint(suit-directive-write) */
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

void test_seq_execution_write_content(void)
{
	uint8_t seq_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */
			0x14, /* uint(suit-directive-override-parameters) */
			0xa1, /* map (1) */
				0x12, /* uint(suit-parameter-content) */
				0x49, /* bstr (9 bytes) */
				't', 'e', 's', 't', '_', 'd', 'a', 't', 'a',
			0x12, /* uint(suit-directive-write) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};
	struct zcbor_string exp_content = {
		.value = "test_data",
		.len = strlen("test_data"),
	};
	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_write_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_content, SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
}
