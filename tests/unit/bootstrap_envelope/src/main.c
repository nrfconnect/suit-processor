/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include <suit_manifest.h>
#include <suit_processor.h>
#include <suit_schedule_seq.h>
#include <bootstrap_envelope.h>
#include "suit_platform/cmock_suit_platform.h"


static struct suit_processor_state state;

static int process_sequence(struct suit_processor_state *state, enum suit_command_sequence seq_name)
{
	/* The bootstrap_envelope_* APIs create a single manifest on the stack. */
	struct suit_manifest_state *manifest = &state->manifest_stack[0];

	int ret = suit_schedule_execution(state, manifest, seq_name);
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
}

void test_bootstrap_empty_envelope(void)
{
	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 0);

	int retval = process_sequence(&state, SUIT_SEQ_VALIDATE);
	TEST_ASSERT_EQUAL(SUIT_ERR_UNAVAILABLE_COMMAND_SEQ, retval);
}

void test_bootstrap_empty_sequence(void)
{
	struct zcbor_string empty_seq = {
		.value = NULL,
		.len = 0,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_sequence(&state, SUIT_SEQ_VALIDATE, &empty_seq);
	/* If a manifest contains more than one component, it is required to start
	 * each sequence with set-component-index.
	 * Use special value (1) to skip this check.
	 */
	bootstrap_envelope_components(&state, 1);

	int retval = process_sequence(&state, SUIT_SEQ_VALIDATE);
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
	bootstrap_envelope_sequence(&state, SUIT_SEQ_INVOKE, &invoke_seq);
	bootstrap_envelope_components(&state, 0);

	int retval = process_sequence(&state, SUIT_SEQ_INVOKE);
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
	bootstrap_envelope_sequence(&state, SUIT_SEQ_INVOKE, &invoke_seq);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);

	int retval = process_sequence(&state, SUIT_SEQ_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, retval);
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
