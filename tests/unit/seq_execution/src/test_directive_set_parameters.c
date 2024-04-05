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

void test_seq_execution_set_parameter_single_component_4params(void)
{
	uint8_t seq_cmd[128];
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 0,
	};

	/* Common header */
	seq_cmd[0] = 0x82; /* list (2 elements - 1 command) */
	seq_cmd[1] = 0x13; /* uint(suit-directive-set-parameters) */
	seq_cmd[2] = 0xa4; /* map (4) */

	enum parameter_values params[] = {
		SOURCE_COMPONENT,
		INVOKE_ARGS,
		DEVICE_ID,
		CONTENT,
	};

	seq.len = bootstrap_parameters(params, ZCBOR_ARRAY_SIZE(params), &seq_cmd[3], sizeof(seq_cmd) - 3) + 3;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, retval, "Failed to set parameters");

	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].vid_set, "Vendor ID not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].cid_set, "Class ID not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].image_digest_set, "Image digest not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].component_slot_set, "Component slot not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].image_size_set, "Image size not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].uri_set, "URI not set, but flag was updated");

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].source_component_set, "Source component set, but flag is not updated");
	TEST_ASSERT_EQUAL_MESSAGE(exp_source_component, state.components[0].source_component, "Source component set with invalid value");

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].invoke_args_set, "Invoke args set, but flag is not updated");
	TEST_ASSERT_EQUAL_MESSAGE(exp_args.len, state.components[0].invoke_args.len, "Invoke args set with invalid length");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(exp_args.value, state.components[0].invoke_args.value, exp_args.len, "Invoke args set with invalid value");

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].did_set, "Device ID set, but flag is not updated");
	TEST_ASSERT_EQUAL_MESSAGE(exp_did.len, state.components[0].did.len, "Device ID set with invalid length");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(exp_did.value, state.components[0].did.value, exp_did.len, "Device ID set with invalid value");

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].content_set, "Content set, but flag is not updated");
	TEST_ASSERT_EQUAL_MESSAGE(exp_content.len, state.components[0].content.len, "Content set with invalid length");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(exp_content.value, state.components[0].content.value, exp_content.len, "Content set with invalid value");
}

void test_seq_execution_set_parameter_single_component_6params(void)
{
	uint8_t seq_cmd[128];
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 0,
	};

	/* Common header */
	seq_cmd[0] = 0x82; /* list (2 elements - 1 command) */
	seq_cmd[1] = 0x13; /* uint(suit-directive-set-parameters) */
	seq_cmd[2] = 0xa6; /* map (6) */

	enum parameter_values params[] = {
		VENDOR_ID,
		CLASS_ID,
		IMAGE_DIGEST,
		COMPONENT_SLOT,
		IMAGE_SIZE,
		URI,
	};

	seq.len = bootstrap_parameters(params, ZCBOR_ARRAY_SIZE(params), &seq_cmd[3], sizeof(seq_cmd) - 3) + 3;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	__cmock_suit_plat_override_image_size_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, exp_image_size, SUIT_SUCCESS);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, retval, "Failed to set parameters");

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].vid_set, "Vendor ID set, but flag is not updated");
	TEST_ASSERT_EQUAL_MESSAGE(exp_vid.len, state.components[0].vid.len, "Vendor ID set with invalid length");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(exp_vid.value, state.components[0].vid.value, exp_vid.len, "Vendor ID set with invalid value");

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].cid_set, "Class ID set, but flag is not updated");
	TEST_ASSERT_EQUAL_MESSAGE(exp_cid.len, state.components[0].cid.len, "Class ID set with invalid length");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(exp_cid.value, state.components[0].cid.value, exp_cid.len, "Class ID set with invalid value");

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].image_digest_set, "Image digest set, but flag is not updated");
	TEST_ASSERT_EQUAL_MESSAGE(exp_digest.len, state.components[0].image_digest.len, "Image digest set with invalid length");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(exp_digest.value, state.components[0].image_digest.value, exp_cid.len, "Image digest set with invalid value");

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].component_slot_set, "Component slot set, but flag is not updated");
	TEST_ASSERT_EQUAL_MESSAGE(exp_slot, state.components[0].component_slot, "Component slot set with invalid value");

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].image_size_set, "Image size set, but flag is not updated");
	TEST_ASSERT_EQUAL_MESSAGE(exp_image_size, state.components[0].image_size, "Image size set with invalid value");

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].uri_set, "URI set, but flag is not updated");
	TEST_ASSERT_EQUAL_MESSAGE(exp_uri.len, state.components[0].uri.len, "URI set with invalid length");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(exp_uri.value, state.components[0].uri.value, exp_uri.len, "URI set with invalid value");

	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].source_component_set, "Source component not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].invoke_args_set, "Invoke args not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].did_set, "Device ID set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].content_set, "Content not set, but flag was updated");
}

void test_seq_execution_set_parameter_single_component_7params(void)
{
	uint8_t seq_cmd[128];
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 0,
	};

	/* Common header */
	seq_cmd[0] = 0x82; /* list (2 elements - 1 command) */
	seq_cmd[1] = 0x13; /* uint(suit-directive-set-parameters) */
	seq_cmd[2] = 0xa7; /* map (7) */

	enum parameter_values params[] = {
		VENDOR_ID,
		CLASS_ID,
		IMAGE_DIGEST,
		COMPONENT_SLOT,
		IMAGE_SIZE,
		URI,
		SOURCE_COMPONENT,
	};

	seq.len = bootstrap_parameters(params, ZCBOR_ARRAY_SIZE(params), &seq_cmd[3], sizeof(seq_cmd) - 3) + 3;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Failed to set parameters");

	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].vid_set, "Vendor ID not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].cid_set, "Class ID not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].image_digest_set, "Image digest not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].component_slot_set, "Component slot not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].image_size_set, "Image size not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].content_set, "Content not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].uri_set, "URI not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].source_component_set, "Source component not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].invoke_args_set, "Invoke args not set, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].did_set, "Device ID not set, but flag was updated");
}

void test_seq_execution_set_parameter_multiple_components_3params(void)
{
	uint8_t seq_cmd[128];
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 0,
	};

	/* Common header */
	seq_cmd[0] = 0x84; /* list (4 elements - 2 commands) */
	seq_cmd[1] = 0x0c, /* uint(suit-directive-set-component-index) */
	seq_cmd[2] = 0xf5, /* True */
	seq_cmd[3] = 0x13; /* uint(suit-directive-set-parameters) */
	seq_cmd[4] = 0xa3; /* map (3) */

	enum parameter_values params[] = {
		SOURCE_COMPONENT,
		INVOKE_ARGS,
		DEVICE_ID,
	};

	seq.len = bootstrap_parameters(params, ZCBOR_ARRAY_SIZE(params), &seq_cmd[5], sizeof(seq_cmd) - 5) + 5;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 4);
	state.components[0].source_component_set = true;
	state.components[1].invoke_args_set = true;
	state.components[2].did_set = true;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, retval, "Failed to set parameters");

	for (size_t i = 0; i < 4; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].vid_set, "Vendor ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].cid_set, "Class ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].image_digest_set, "Image digest not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].component_slot_set, "Component slot not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].image_size_set, "Image size not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].content_set, "Content not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].uri_set, "URI not set, but flag was updated");

		TEST_ASSERT_EQUAL_MESSAGE(true, state.components[i].source_component_set, "Source component set, but flag is not updated");
		if (i != 0) {
			TEST_ASSERT_EQUAL_MESSAGE(exp_source_component, state.components[i].source_component, "Source component set with invalid value");
		} else {
			TEST_ASSERT_EQUAL_MESSAGE(0, state.components[i].source_component, "Source component overwritten, but was set before sequence execution");
		}

		TEST_ASSERT_EQUAL_MESSAGE(true, state.components[i].invoke_args_set, "Invoke args set, but flag is not updated");
		if (i != 1) {
			TEST_ASSERT_EQUAL_MESSAGE(exp_args.len, state.components[i].invoke_args.len, "Invoke args set with invalid length");
			TEST_ASSERT_EQUAL_MEMORY_MESSAGE(exp_args.value, state.components[i].invoke_args.value, exp_args.len, "Invoke args set with invalid value");
		} else {
			TEST_ASSERT_EQUAL_MESSAGE(0, state.components[i].invoke_args.len, "Invoke args length overwritten, but was set before sequence execution");
			TEST_ASSERT_NULL_MESSAGE(state.components[i].invoke_args.value, "Invoke args value overwritten, but was set before sequence execution");
		}

		TEST_ASSERT_EQUAL_MESSAGE(true, state.components[i].did_set, "Device ID set, but flag is not updated");
		if (i != 2) {
			TEST_ASSERT_EQUAL_MESSAGE(exp_did.len, state.components[i].did.len, "Device ID set with invalid length");
			TEST_ASSERT_EQUAL_MEMORY_MESSAGE(exp_did.value, state.components[i].did.value, exp_did.len, "Device ID set with invalid value");
		} else {
			TEST_ASSERT_EQUAL_MESSAGE(0, state.components[i].did.len, "Device ID length overwritten, but was set before sequence execution");
			TEST_ASSERT_NULL_MESSAGE(state.components[i].did.value, "Device ID value overwritten, but was set before sequence execution");
		}
	}
}

void test_seq_execution_set_parameter_two_components_3params(void)
{
	uint8_t seq_cmd[128];
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 0,
	};

	/* Common header */
	seq_cmd[0] = 0x84; /* list (4 elements - 2 commands) */
	seq_cmd[1] = 0x0c, /* uint(suit-directive-set-component-index) */
		seq_cmd[2] = 0x82, /* list (2 elements) */
		seq_cmd[3] = 0x01; /* uint(2nd component) */
		seq_cmd[4] = 0x02; /* uint(3rd component) */
	seq_cmd[5] = 0x13; /* uint(suit-directive-set-parameters) */
	seq_cmd[6] = 0xa3; /* map (3) */

	enum parameter_values params[] = {
		SOURCE_COMPONENT,
		INVOKE_ARGS,
		DEVICE_ID,
	};

	seq.len = bootstrap_parameters(params, ZCBOR_ARRAY_SIZE(params), &seq_cmd[7], sizeof(seq_cmd) - 7) + 7;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 4);
	state.components[0].source_component_set = true;
	state.components[1].invoke_args_set = true;
	state.components[2].did_set = true;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, retval, "Failed to set parameters");

	for (size_t i = 0; i < 4; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].vid_set, "Vendor ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].cid_set, "Class ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].image_digest_set, "Image digest not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].component_slot_set, "Component slot not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].image_size_set, "Image size not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].content_set, "Content not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].uri_set, "URI not set, but flag was updated");

		if (i == 3) {
			TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].source_component_set, "Source component set for unselected component");
		} else if (i == 0) {
			TEST_ASSERT_EQUAL_MESSAGE(true, state.components[i].source_component_set, "Source component set, but flag is not updated");
			TEST_ASSERT_EQUAL_MESSAGE(0, state.components[i].source_component, "Source component overwritten, but was set before sequence execution");
		} else {
			TEST_ASSERT_EQUAL_MESSAGE(true, state.components[i].source_component_set, "Source component set, but flag is not updated");
			TEST_ASSERT_EQUAL_MESSAGE(exp_source_component, state.components[i].source_component, "Source component set with invalid value");
		}

		if (i == 1) {
			TEST_ASSERT_EQUAL_MESSAGE(true, state.components[i].invoke_args_set, "Invoke args set, but flag is not updated");
			TEST_ASSERT_EQUAL_MESSAGE(0, state.components[i].invoke_args.len, "Invoke args length overwritten, but was set before sequence execution");
			TEST_ASSERT_NULL_MESSAGE(state.components[i].invoke_args.value, "Invoke args value overwritten, but was set before sequence execution");
		} else if (i == 2) {
			TEST_ASSERT_EQUAL_MESSAGE(true, state.components[i].invoke_args_set, "Invoke args set, but flag is not updated");
			TEST_ASSERT_EQUAL_MESSAGE(exp_args.len, state.components[i].invoke_args.len, "Invoke args set with invalid length");
			TEST_ASSERT_EQUAL_MEMORY_MESSAGE(exp_args.value, state.components[i].invoke_args.value, exp_args.len, "Invoke args set with invalid value");
		} else {
			TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].invoke_args_set, "Invoke args set for unselected component");
		}

		if (i == 2) {
			TEST_ASSERT_EQUAL_MESSAGE(true, state.components[i].did_set, "Device ID set, but flag is not updated");
			TEST_ASSERT_EQUAL_MESSAGE(0, state.components[i].did.len, "Device ID length overwritten, but was set before sequence execution");
			TEST_ASSERT_NULL_MESSAGE(state.components[i].did.value, "Device ID value overwritten, but was set before sequence execution");
		} else if (i == 1) {
			TEST_ASSERT_EQUAL_MESSAGE(true, state.components[i].did_set, "Device ID set, but flag is not updated");
			TEST_ASSERT_EQUAL_MESSAGE(exp_did.len, state.components[i].did.len, "Device ID set with invalid length");
			TEST_ASSERT_EQUAL_MEMORY_MESSAGE(exp_did.value, state.components[i].did.value, exp_did.len, "Device ID set with invalid value");
		} else {
			TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].did_set, "Device ID set for unselected component");
		}
	}
}

void test_seq_execution_set_parameter_soft_failure(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
		0x18, 0x20, /* uint(suit-directive-run-sequence) */
		0x45, /* bytes(5) */
			0x82, /* list (2 elements - 1 command) */
			0x13, /* uint(suit-directive-set-parameters) */
			0xa1, /* map (1 element) */
				0x0d, /* uint(suit-parameter-soft-failure) */
				0xf5, /* True */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = sizeof(seq_cmd),
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_PARAMETER, retval, "Soft failure should not be settable through suit-directive-set-parameters");
}

void test_seq_execution_set_parameter_multiple_components_image_size_failed(void)
{
	uint8_t seq_cmd[128];
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 0,
	};

	/* Common header */
	seq_cmd[0] = 0x84; /* list (4 elements - 2 commands) */
	seq_cmd[1] = 0x0c, /* uint(suit-directive-set-component-index) */
	seq_cmd[2] = 0xf5, /* True */
	seq_cmd[3] = 0x13; /* uint(suit-directive-set-parameters) */
	seq_cmd[4] = 0xa1; /* map (1) */

	enum parameter_values params[] = {
		IMAGE_SIZE,
	};

	seq.len = bootstrap_parameters(params, ZCBOR_ARRAY_SIZE(params), &seq_cmd[5], sizeof(seq_cmd) - 5) + 5;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 4);
	state.components[0].image_size_set = true;
	state.components[1].image_size_set = false;
	state.components[2].image_size_set = true;
	state.components[3].image_size_set = false;

	__cmock_suit_plat_override_image_size_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + 1, exp_image_size, SUIT_ERR_CRASH);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, retval, "Image size set failed, but processing succeeded");

	for (size_t i = 0; i < 4; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].vid_set, "Vendor ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].cid_set, "Class ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].image_digest_set, "Image digest not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].component_slot_set, "Component slot not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].uri_set, "URI not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].source_component_set, "Source component not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].invoke_args_set, "Invoke args not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].did_set, "Device ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].content_set, "Content not set, but flag was updated");
	}

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].image_size_set, "Image size set before command execution, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[1].image_size_set, "Image size set failed, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[2].image_size_set, "Image size set before command execution, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[3].image_size_set, "Image size set failed on previous component, but the flag was updated");
}

void test_seq_execution_set_parameter_several_components_with_invalid_index(void)
{
	uint8_t seq_cmd[128];
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 0,
	};

	/* Common header */
	seq_cmd[0] = 0x84; /* list (4 elements - 2 commands) */
	seq_cmd[1] = 0x0c, /* uint(suit-directive-set-component-index) */
		seq_cmd[2] = 0x82, /* list (2 elements) */
		seq_cmd[3] = 0x01; /* uint(2nd component) */
		seq_cmd[4] = 0x05; /* uint(5th component) */
	seq_cmd[5] = 0x13; /* uint(suit-directive-set-parameters) */
	seq_cmd[6] = 0xa3; /* map (3) */

	enum parameter_values params[] = {
		IMAGE_SIZE,
	};

	seq.len = bootstrap_parameters(params, ZCBOR_ARRAY_SIZE(params), &seq_cmd[7], sizeof(seq_cmd) - 7) + 7;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 4);
	state.components[0].image_size_set = true;
	state.components[1].image_size_set = false;
	state.components[2].image_size_set = true;
	state.components[3].image_size_set = false;

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MISSING_COMPONENT, retval, "Invalid component index not detected");

	for (size_t i = 0; i < 4; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].vid_set, "Vendor ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].cid_set, "Class ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].image_digest_set, "Image digest not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].component_slot_set, "Component slot not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].uri_set, "URI not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].source_component_set, "Source component not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].invoke_args_set, "Invoke args not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].did_set, "Device ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].content_set, "Content not set, but flag was updated");
	}

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].image_size_set, "Image size set before command execution, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[1].image_size_set, "Image size set failed, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[2].image_size_set, "Image size set before command execution, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[3].image_size_set, "Image size set failed, but the flag was updated");
}

void test_seq_execution_set_parameter_lazy_platform_image_size_set(void)
{
	uint8_t seq_cmd[128];
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 0,
	};

	/* Common header */
	seq_cmd[0] = 0x84; /* list (4 elements - 2 commands) */
	seq_cmd[1] = 0x0c, /* uint(suit-directive-set-component-index) */
	seq_cmd[2] = 0xf5, /* True */
	seq_cmd[3] = 0x13; /* uint(suit-directive-set-parameters) */
	seq_cmd[4] = 0xa1; /* map (1) */

	enum parameter_values params[] = {
		IMAGE_SIZE,
	};

	seq.len = bootstrap_parameters(params, ZCBOR_ARRAY_SIZE(params), &seq_cmd[5], sizeof(seq_cmd) - 5) + 5;

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 4);
	state.components[0].image_size_set = true;
	state.components[1].image_size_set = false;
	state.components[2].image_size_set = true;
	state.components[3].image_size_set = false;

	__cmock_suit_plat_override_image_size_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE + 1, exp_image_size, SUIT_ERR_AGAIN);

	int retval = execute_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_TAMP, retval, "Image size set is not allowed to take more than one iteration");

	for (size_t i = 0; i < 4; i++) {
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].vid_set, "Vendor ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].cid_set, "Class ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].image_digest_set, "Image digest not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].component_slot_set, "Component slot not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].uri_set, "URI not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].source_component_set, "Source component not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].invoke_args_set, "Invoke args not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[i].did_set, "Device ID not set, but flag was updated");
		TEST_ASSERT_EQUAL_MESSAGE(false, state.components[0].content_set, "Content not set, but flag was updated");
	}

	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[0].image_size_set, "Image size set before command execution, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[1].image_size_set, "Image size set failed, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(true, state.components[2].image_size_set, "Image size set before command execution, but flag was updated");
	TEST_ASSERT_EQUAL_MESSAGE(false, state.components[3].image_size_set, "Image size set failed on previous component, but the flag was updated");
}
