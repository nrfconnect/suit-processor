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


typedef int (*seq_validation_api_t)(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str);

struct cmd_support_matrix {
	int exp_retval;
	uint8_t *test_cmd;
	size_t test_cmd_size;
	const uint8_t *cmd_name;
};


static struct suit_processor_state state;

static uint8_t condition_vendor_identifier_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x01, /* uint(suit-condition-vendor-identifier) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t condition_class_identifier_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x02, /* uint(suit-condition-class-identifier) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t condition_image_match_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x03, /* uint(suit-condition-image-match) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t condition_component_slot_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x05, /* uint(suit-condition-component-slot) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t condition_check_content_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x06, /* uint(suit-condition-check-content) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t condition_dependency_integrity_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x07, /* uint(suit-condition-dependency-integrity) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t condition_is_dependency_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x08, /* uint(suit-condition-is_dependency) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t condition_abort_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x0e, /* uint(suit-condition-abort) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t condition_device_identifier_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x18, 0x18, /* uint(suit-condition-device-identifier) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};

static uint8_t condition_unsupported_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0xfe, /* unsupported value */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};

static uint8_t directive_process_dependency_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x0b, /* uint(suit-directive-process-dependency) */
		0xf5, /* true */
};
static uint8_t directive_set_component_index_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x0c, /* uint(suit-directive-set-component-index) */
		0xf5, /* true */
};
static uint8_t directive_try_each_empty_sequences_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x0f, /* uint(suit-directive-try-each) */
		0x82, /* list (2 elements - sequences) */
			0x40, /* bytes (0) */
			0x40, /* bytes (0) */
};
static uint8_t directive_try_each_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x0f, /* uint(suit-directive-try-each) */
		0x82, /* list (2 elements - sequences) */
			0x43, /* bytes (3) */
				0x82, /* list (2 elements - 1 command) */
					0x0e, /* uint(suit-condition-abort) */
					0x00, /* uint(SUIT_Rep_Policy::None) */
			0x43, /* bytes (3) */
				0x82, /* list (2 elements - 1 command) */
					0x0e, /* uint(suit-condition-abort) */
					0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t directive_write_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x12, /* uint(suit-directive-write) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t directive_set_parameters_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x13, /* uint(suit-directive-set-parameters) */
		0xa1, /* map (1) */
			0x0d, /* uint(suit-parameter-soft-failure) */
			0xf5, /* True */
};
static uint8_t directive_override_parameters_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x14, /* uint(suit-directive-override-parameters) */
		0xa1, /* map (1) */
			0x0d, /* uint(suit-parameter-soft-failure) */
			0xf5, /* True */
};
static uint8_t directive_fetch_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x15, /* uint(suit-directive-fetch) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t directive_copy_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x16, /* uint(suit-directive-copy) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t directive_invoke_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x17, /* uint(suit-directive-invoke) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t directive_swap_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x1f, /* uint(suit-directive-swap) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t directive_run_empty_sequence_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x18, 0x20, /* uint(suit-directive-run-sequence) */
		0x40, /* bytes (0) */
};
static uint8_t directive_run_sequence_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x18, 0x20, /* uint(suit-directive-run-sequence) */
		0x43, /* bytes (3) */
			0x82, /* list (2 elements - 1 command) */
				0x0e, /* uint(suit-condition-abort) */
				0x00, /* uint(SUIT_Rep_Policy::None) */
};
static uint8_t directive_unlink_cmd[] = {
	0x82, /* list (2 elements - 1 command) */
		0x21, /* uint(suit-directive-unlink) */
		0x00, /* uint(SUIT_Rep_Policy::None) */
};

struct cmd_support_matrix shared_support_matrix[] = {
	{SUIT_SUCCESS, condition_vendor_identifier_cmd, sizeof(condition_vendor_identifier_cmd), "condition_vendor_identifier"},
	{SUIT_SUCCESS, condition_class_identifier_cmd, sizeof(condition_class_identifier_cmd), "condition_class_identifier"},
	{SUIT_SUCCESS, condition_image_match_cmd, sizeof(condition_image_match_cmd), "condition_image_match"},
	{SUIT_SUCCESS, condition_component_slot_cmd, sizeof(condition_component_slot_cmd), "condition_component_slot"},
	{SUIT_SUCCESS, condition_check_content_cmd, sizeof(condition_check_content_cmd), "condition_check_content"},
	{SUIT_SUCCESS, condition_dependency_integrity_cmd, sizeof(condition_dependency_integrity_cmd), "condition_dependency_integrity"},
	{SUIT_SUCCESS, condition_is_dependency_cmd, sizeof(condition_is_dependency_cmd), "condition_is_dependency"},
	{SUIT_SUCCESS, condition_abort_cmd, sizeof(condition_abort_cmd), "condition_abort"},
	{SUIT_SUCCESS, condition_device_identifier_cmd, sizeof(condition_device_identifier_cmd), "condition_device_identifier"},
	{SUIT_ERR_DECODING, condition_unsupported_cmd, sizeof(condition_unsupported_cmd), "Unsupported condition"},
	{SUIT_ERR_DECODING, directive_process_dependency_cmd, sizeof(directive_process_dependency_cmd), "directive_process_dependency"},
	{SUIT_SUCCESS, directive_set_component_index_cmd, sizeof(directive_set_component_index_cmd), "directive_set_component_index"},
	{ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_NO_PAYLOAD), directive_try_each_empty_sequences_cmd, sizeof(directive_try_each_empty_sequences_cmd), "directive_try_each without payload"},
	{SUIT_SUCCESS, directive_try_each_cmd, sizeof(directive_try_each_cmd), "directive_try_each"},
	{SUIT_ERR_MANIFEST_VALIDATION, directive_set_parameters_cmd, sizeof(directive_set_parameters_cmd), "directive_set_parameters"},
	{SUIT_SUCCESS, directive_override_parameters_cmd, sizeof(directive_override_parameters_cmd), "directive_override_parameters"},
	{SUIT_ERR_MANIFEST_VALIDATION, directive_fetch_cmd, sizeof(directive_fetch_cmd), "directive_fetch"},
	{SUIT_ERR_MANIFEST_VALIDATION, directive_copy_cmd, sizeof(directive_copy_cmd), "directive_copy"},
	{SUIT_ERR_MANIFEST_VALIDATION, directive_write_cmd, sizeof(directive_write_cmd), "directive_write"},
	{SUIT_ERR_MANIFEST_VALIDATION, directive_invoke_cmd, sizeof(directive_invoke_cmd), "directive_invoke"},
	{SUIT_ERR_DECODING, directive_swap_cmd, sizeof(directive_swap_cmd), "directive_swap"},
	{SUIT_ERR_DECODING, directive_run_empty_sequence_cmd, sizeof(directive_run_empty_sequence_cmd), "directive_run_sequence without payload"},
	{SUIT_SUCCESS, directive_run_sequence_cmd, sizeof(directive_run_sequence_cmd), "directive_run_sequence"},
	{SUIT_ERR_DECODING, directive_unlink_cmd, sizeof(directive_unlink_cmd), "directive_unlink"},
};

struct cmd_support_matrix command_support_matrix[] = {
	{SUIT_SUCCESS, condition_vendor_identifier_cmd, sizeof(condition_vendor_identifier_cmd), "condition_vendor_identifier"},
	{SUIT_SUCCESS, condition_class_identifier_cmd, sizeof(condition_class_identifier_cmd), "condition_class_identifier"},
	{SUIT_SUCCESS, condition_image_match_cmd, sizeof(condition_image_match_cmd), "condition_image_match"},
	{SUIT_SUCCESS, condition_component_slot_cmd, sizeof(condition_component_slot_cmd), "condition_component_slot"},
	{SUIT_SUCCESS, condition_check_content_cmd, sizeof(condition_check_content_cmd), "condition_check_content"},
	{SUIT_SUCCESS, condition_dependency_integrity_cmd, sizeof(condition_is_dependency_cmd), "condition_dependency_integrity"},
	{SUIT_SUCCESS, condition_is_dependency_cmd, sizeof(condition_is_dependency_cmd), "condition_is_dependency"},
	{SUIT_SUCCESS, condition_abort_cmd, sizeof(condition_abort_cmd), "condition_abort"},
	{SUIT_SUCCESS, condition_device_identifier_cmd, sizeof(condition_device_identifier_cmd), "condition_device_identifier"},
	{SUIT_ERR_DECODING, condition_unsupported_cmd, sizeof(condition_unsupported_cmd), "Unsupported condition"},
	{SUIT_ERR_DECODING, directive_process_dependency_cmd, sizeof(directive_process_dependency_cmd), "directive_process_dependency"},
	{SUIT_SUCCESS, directive_set_component_index_cmd, sizeof(directive_set_component_index_cmd), "directive_set_component_index"},
	{ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_NO_PAYLOAD), directive_try_each_empty_sequences_cmd, sizeof(directive_try_each_empty_sequences_cmd), "directive_try_each without payload"},
	{SUIT_SUCCESS, directive_try_each_cmd, sizeof(directive_try_each_cmd), "directive_try_each"},
	{SUIT_SUCCESS, directive_set_parameters_cmd, sizeof(directive_set_parameters_cmd), "directive_set_parameters"},
	{SUIT_SUCCESS, directive_override_parameters_cmd, sizeof(directive_override_parameters_cmd), "directive_override_parameters"},
	{SUIT_SUCCESS, directive_fetch_cmd, sizeof(directive_fetch_cmd), "directive_fetch"},
	{SUIT_SUCCESS, directive_copy_cmd, sizeof(directive_copy_cmd), "directive_copy"},
	{SUIT_SUCCESS, directive_write_cmd, sizeof(directive_write_cmd), "directive_write"},
	{SUIT_SUCCESS, directive_invoke_cmd, sizeof(directive_invoke_cmd), "directive_invoke"},
	{SUIT_ERR_DECODING, directive_swap_cmd, sizeof(directive_swap_cmd), "directive_swap"},
	{SUIT_ERR_DECODING, directive_run_empty_sequence_cmd, sizeof(directive_run_empty_sequence_cmd), "directive_run_sequence without payload"},
	{SUIT_SUCCESS, directive_run_sequence_cmd, sizeof(directive_run_sequence_cmd), "directive_run_sequence"},
	{SUIT_ERR_DECODING, directive_unlink_cmd, sizeof(directive_unlink_cmd), "directive_unlink"},
};


static void seq_validation_nested_test_template(seq_validation_api_t validate, bootsrap_seq_hdr_generator_t generator, struct cmd_support_matrix *support_matrix, size_t n_components, size_t depth)
{
	struct zcbor_string seq;
	uint8_t nested_cmd[256];
	size_t content_size = 0;
	char assert_msg[] = "Assetion failed at very-long-directive-name-with-comments with 5 nesting headers and 55 components";

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, n_components);

	/* Append command to test */
	memcpy(nested_cmd, support_matrix->test_cmd, support_matrix->test_cmd_size);
	content_size = support_matrix->test_cmd_size;

	/* Prepend nested commands. */
	for (size_t nesting_hdr_cnt = 0; nesting_hdr_cnt < depth; nesting_hdr_cnt++) {
		content_size = generator(nested_cmd, sizeof(nested_cmd), content_size);
	}

	/* Construct command sequence structure and validate. */
	seq.value = nested_cmd;
	seq.len = content_size;//ptr - nested_cmd;

	int retval = validate(&state, &seq);

	snprintf(assert_msg, sizeof(assert_msg),
		"Assetion failed at %s with %zu nesting headers and %zu components",
		support_matrix->cmd_name,
		depth,
		n_components);
	TEST_ASSERT_EQUAL_MESSAGE(
		support_matrix->exp_retval,
		retval,
		assert_msg);
	TEST_ASSERT_EQUAL(state.seq_stack_height, 0);
}

static int validate_command_sequence(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str)
{
	enum suit_command_sequence seq = SUIT_SEQ_PAYLOAD_FETCH;
	struct suit_manifest_state *manifest_state = &state->manifest_stack[0];

	bootstrap_envelope_sequence(state, seq, cmd_seq_str);

	int ret = suit_schedule_validation(state, manifest_state, seq);
	if (ret == SUIT_ERR_AGAIN) {
		ret = suit_process_scheduled(state);
	}

	return ret;
}

static int validate_shared_sequence(struct suit_processor_state *state, struct zcbor_string *cmd_seq_str)
{
	enum suit_command_sequence seq = SUIT_SEQ_SHARED;
	struct suit_manifest_state *manifest_state = &state->manifest_stack[0];

	bootstrap_envelope_sequence(state, seq, cmd_seq_str);

	int ret = suit_schedule_validation(state, manifest_state, seq);
	if (ret == SUIT_ERR_AGAIN) {
		ret = suit_process_scheduled(state);
	}

	return ret;
}


/* Generic note about the test sequences:
 *
 * If a manifest contains more than one component, it is required to start each sequence with set-component-index.
 * For all cases that use sequences without this command - use special value of component number (1) to skip this check.
 */

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


void test_seq_validation_shared_bstr_as_command_list(void)
{
	uint8_t seq_cmd[] = {
		0x42, /* bytes (2) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 3,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = validate_shared_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_WRONG_TYPE), retval);
}

void test_seq_validation_shared_infinite_length_command_list(void)
{
	uint8_t seq_cmd[] = {
		0x9f, /* list (infinite, non-canonical) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
		0xff, /* end of list */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 4,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = validate_shared_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_ADDITIONAL_INVAL), retval);
}

void test_seq_validation_shared_odd_number_of_commands(void)
{
	uint8_t seq_cmd[] = {
		0x83, /* list (3 elements - 1.5 commands) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 4,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = validate_shared_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_DECODING, retval);
}

void test_seq_validation_shared_payload_longer_than_expected(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 5,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = validate_shared_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_DECODING, retval);
}

void test_seq_validation_shared_no_components(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 3,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 0);

	int retval = validate_shared_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_MANIFEST_VALIDATION, retval);
}

void test_seq_validation_command_no_components(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 3,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 0);

	int retval = validate_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_MANIFEST_VALIDATION, retval);
}

void test_seq_validation_shared_set_invalid_index(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0c, /* uint(suit-directive-set-component-index) */
			0x02, /* uint(2) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 3,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = validate_shared_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_MANIFEST_VALIDATION, retval);
}

void test_seq_validation_command_set_invalid_index(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0c, /* uint(suit-directive-set-component-index) */
			0x02, /* uint(2) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 3,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = validate_command_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_MANIFEST_VALIDATION, retval);
}

void test_seq_validation_shared_condition_without_set_index(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x0e, /* uint(suit-condition-abort) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 3,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 2);

	int retval = validate_shared_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_MANIFEST_VALIDATION, retval);
}

void test_seq_validation_shared_directive_without_set_index(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
		0x14, /* uint(suit-directive-override-parameters) */
		0xa1, /* map (1) */
			0x0d, /* uint(suit-parameter-soft-failure) */
			0xf5, /* True */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 5,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 2);

	int retval = validate_shared_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_MANIFEST_VALIDATION, retval);
}

void test_seq_validation_shared_unknown_command(void)
{
	uint8_t seq_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
			0x18, 0x20, /* uint(suit-directive-run-sequence), but without payload */
	};
	struct zcbor_string seq = {
		.value = seq_cmd,
		.len = 3,
	};

	bootstrap_envelope_empty(&state);
	bootstrap_envelope_components(&state, 1);

	int retval = validate_shared_sequence(&state, &seq);

	TEST_ASSERT_EQUAL(SUIT_ERR_DECODING, retval);
}


void test_seq_validation_shared_nested_run_sequence(void)
{
	for (size_t depth = 0; depth < (SUIT_MAX_SEQ_DEPTH - 1); depth++) {
		for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(shared_support_matrix); i++) {
			seq_validation_nested_test_template(
				validate_shared_sequence,
				bootsrap_seq_hdr_run_sequence,
				&shared_support_matrix[i],
				1,
				depth);

			resetTest();
		}
	}
}

void test_seq_validation_command_nested_run_sequence(void)
{
	for (size_t depth = 0; depth < (SUIT_MAX_SEQ_DEPTH - 1); depth++) {
		for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(command_support_matrix); i++) {
			seq_validation_nested_test_template(
				validate_command_sequence,
				bootsrap_seq_hdr_run_sequence,
				&command_support_matrix[i],
				1,
				depth);

			resetTest();
		}
	}
}

void test_seq_validation_shared_nested_run_sequence_multiple_components(void)
{
	for (size_t depth = 1; depth < (SUIT_MAX_SEQ_DEPTH - 1); depth++) {
		for (size_t n_components = 2; n_components < SUIT_MAX_NUM_COMPONENTS; n_components++) {
			for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(shared_support_matrix); i++) {
				seq_validation_nested_test_template(
					validate_shared_sequence,
					bootsrap_seq_hdr_run_sequence_on_all_components,
					&shared_support_matrix[i],
					n_components,
					depth);

				resetTest();
			}
		}
	}
}

void test_seq_validation_command_nested_run_sequence_multiple_components(void)
{
	for (size_t depth = 1; depth < (SUIT_MAX_SEQ_DEPTH - 1); depth++) {
		for (size_t n_components = 2; n_components < SUIT_MAX_NUM_COMPONENTS; n_components++) {
			for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(command_support_matrix); i++) {
				seq_validation_nested_test_template(
					validate_command_sequence,
					bootsrap_seq_hdr_run_sequence_on_all_components,
					&command_support_matrix[i],
					n_components,
					depth);

				resetTest();
			}
		}
	}
}

void test_seq_validation_shared_nested_try_each(void)
{
	for (size_t depth = 0; depth < (SUIT_MAX_SEQ_DEPTH - 1); depth++) {
		for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(shared_support_matrix); i++) {
			seq_validation_nested_test_template(
				validate_shared_sequence,
				bootsrap_seq_hdr_try_each,
				&shared_support_matrix[i],
				1,
				depth);

			resetTest();
		}
	}
}

void test_seq_validation_command_nested_try_each(void)
{
	for (size_t depth = 0; depth < (SUIT_MAX_SEQ_DEPTH - 1); depth++) {
		for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(command_support_matrix); i++) {
			seq_validation_nested_test_template(
				validate_command_sequence,
				bootsrap_seq_hdr_try_each,
				&command_support_matrix[i],
				1,
				depth);

			resetTest();
		}
	}
}

void test_seq_validation_shared_nested_try_each_multiple_components(void)
{
	for (size_t depth = 1; depth < (SUIT_MAX_SEQ_DEPTH - 1); depth++) {
		for (size_t n_components = 2; n_components < SUIT_MAX_NUM_COMPONENTS; n_components++) {
			for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(shared_support_matrix); i++) {
				seq_validation_nested_test_template(
					validate_shared_sequence,
					bootsrap_seq_hdr_try_each_on_all_components,
					&shared_support_matrix[i],
					n_components,
					depth);

				resetTest();
			}
		}
	}
}

void test_seq_validation_command_nested_try_each_multiple_components(void)
{
	for (size_t depth = 1; depth < (SUIT_MAX_SEQ_DEPTH - 1); depth++) {
		for (size_t n_components = 2; n_components < SUIT_MAX_NUM_COMPONENTS; n_components++) {
			for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(command_support_matrix); i++) {
				seq_validation_nested_test_template(
					validate_command_sequence,
					bootsrap_seq_hdr_try_each_on_all_components,
					&command_support_matrix[i],
					n_components,
					depth);

				resetTest();
			}
		}
	}
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
