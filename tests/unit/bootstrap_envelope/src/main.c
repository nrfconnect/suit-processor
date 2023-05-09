/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include <suit.h>
#include <bootstrap_envelope.h>
#include "suit_platform/cmock_suit_platform.h"


static struct suit_processor_state state;


void test_bootstrap_reset_state(void)
{
	suit_reset_state(&state);
}

void test_bootstrap_empty_envelope(void)
{
	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 0);

	int retval = suit_process_manifest(&state, SUIT_VALIDATE);
	TEST_ASSERT_EQUAL(SUIT_ERR_UNAVAILABLE_COMMAND_SEQ, retval);
}

void test_bootstrap_empty_sequence(void)
{
	struct zcbor_string empty_seq = {
		.value = NULL,
		.len = 0,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_sequence(&state, SUIT_VALIDATE, &empty_seq);
	/* If a manifest contains more than one component, it is required to start
	 * each sequence with set-component-index.
	 * Use special value (1) to skip this check.
	 */
	bootstrap_envelope_components(&state, 1);

	int retval = suit_process_manifest(&state, SUIT_VALIDATE);
	TEST_ASSERT_EQUAL(ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_NO_PAYLOAD), retval);
}

void test_bootstrap_invoke_no_components(void)
{
	uint8_t invoke_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
		0x17, /* uint(suit-directive-invoke) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string invoke_seq = {
		.value = invoke_cmd,
		.len = sizeof(invoke_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
	bootstrap_envelope_components(&state, 0);

	int retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_ERR_MANIFEST_VALIDATION, retval);
}

void test_bootstrap_invoke_single_component(void)
{
	uint8_t invoke_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x17, /* uint(suit-directive-invoke) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string invoke_seq = {
		.value = invoke_cmd,
		.len = sizeof(invoke_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_sequence(&state, SUIT_INVOKE, &invoke_seq);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_INVOKE, NULL, NULL, 0, SUIT_SUCCESS);

	int retval = suit_process_manifest(&state, SUIT_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
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
