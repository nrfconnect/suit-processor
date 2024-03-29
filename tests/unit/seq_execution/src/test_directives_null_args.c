/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include <suit_directive.h>
#include <suit_manifest.h>

extern struct suit_processor_state state;

void test_set_current_components_null_args(void)
{
	struct IndexArg_r index_arg = {
		.IndexArg_choice = IndexArg_bool_c,
	};

	int retval = suit_directive_set_current_components(NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL) were not detected");

	retval = suit_directive_set_current_components(&state, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL) were not detected");

	retval = suit_directive_set_current_components(NULL, &index_arg);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (NULL, _) were not detected");

	retval = suit_directive_set_current_components(&state, &index_arg);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, retval, "Invalid execution state was not detected");
}

void test_try_each_null_args(void)
{
	uint8_t cmd_seq[] = {
		0x82, /* list (2 elements - 1 command) */
			0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string sample_seq = {
		.value = cmd_seq,
		.len = sizeof(cmd_seq),
	};
	struct SUIT_Directive_Try_Each_Argument try_each_arg = {
		.SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr = {{
			.value = NULL,
			.len = 0,
		}},
		.SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr_count = 0,
		.SUIT_Directive_Try_Each_Argument_nil_present = 1,
	};

	int retval = suit_directive_try_each(NULL, NULL, false);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL, false) were not detected");

	retval = suit_directive_try_each(&state, NULL, false);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL, false) were not detected");

	retval = suit_directive_try_each(NULL, &try_each_arg, false);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (NULL, _, false) were not detected");

	retval = suit_directive_try_each(&state, &try_each_arg, false);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL sequence was not detected");

	try_each_arg.SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr[0] = sample_seq;
	try_each_arg.SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr[1] = sample_seq;
	try_each_arg.SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr[2] = sample_seq;
	try_each_arg.SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr_count = 3;

	retval = suit_directive_try_each(&state, &try_each_arg, false);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, retval, "Invalid execution state was not detected");
}

void test_run_sequence_null_args(void)
{
	uint8_t cmd_seq[] = {
		0x82, /* list (2 elements - 1 command) */
			0x01, /* uint(suit-condition-vendor-identifier) */
			0x00, /* uint(SUIT_Rep_Policy::None) */
	};
	struct zcbor_string command_sequence = {
		.value = NULL,
		.len = 0,
	};

	int retval = suit_directive_run_sequence(NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL) were not detected");

	retval = suit_directive_run_sequence(&state, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL) were not detected");

	retval = suit_directive_run_sequence(NULL, &command_sequence);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (NULL, _) were not detected");

	retval = suit_directive_run_sequence(&state, &command_sequence);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL sequence was not detected");

	command_sequence.value = cmd_seq;
	command_sequence.len = sizeof(cmd_seq);

	retval = suit_directive_run_sequence(&state, &command_sequence);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, retval, "Invalid execution state was not detected");
}

void test_override_parameters_null_args(void)
{
	struct suit_directive_override_parameters_m_l_map_SUIT_Parameters_m params = {
		.suit_directive_override_parameters_m_l_map_SUIT_Parameters_m = {
			.SUIT_Parameters_choice = SUIT_Parameters_suit_parameter_image_size_c,
			.SUIT_Parameters_suit_parameter_image_size = 0x01020304,
		},
	};

	int retval = suit_directive_override_parameters(NULL, NULL, 1);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL,  1) were not detected");

	retval = suit_directive_override_parameters(&state, NULL, 1);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL, 1) were not detected");

	retval = suit_directive_override_parameters(NULL, &params, 1);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, _,  1) were not detected");

	retval = suit_directive_override_parameters(&state, &params, 1);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, retval, "Invalid execution state was not detected");
}

void test_set_parameters_null_args(void)
{
	struct suit_manifest_params component_params;
	struct suit_directive_set_parameters_m_l_map_SUIT_Parameters_m params = {
		.suit_directive_set_parameters_m_l_map_SUIT_Parameters_m = {
			.SUIT_Parameters_choice = SUIT_Parameters_suit_parameter_image_size_c,
			.SUIT_Parameters_suit_parameter_image_size = 0x01020304,
		},
	};

	memset(&component_params, 0, sizeof(component_params));

	int retval = suit_directive_set_parameters(NULL, NULL, 1, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL,  1, NULL) were not detected");

	retval = suit_directive_set_parameters(&state, NULL, 1, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL, 1, NULL) were not detected");

	retval = suit_directive_set_parameters(NULL, &params, 1, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, _,  1, NULL) were not detected");

	retval = suit_directive_set_parameters(&state, &params, 1, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, _, 1, NULL) were not detected");

	retval = suit_directive_set_parameters(NULL, NULL, 1, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL,  1, _) were not detected");

	retval = suit_directive_set_parameters(&state, NULL, 1, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL, 1, _) were not detected");

	retval = suit_directive_set_parameters(NULL, &params, 1, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, _,  1, _) were not detected");
}

void test_process_dependency_null_args(void)
{
	struct suit_manifest_params component_params;

	memset(&component_params, 0, sizeof(component_params));
	component_params.is_dependency = suit_bool_false;
	component_params.integrity_checked = false;

	int retval = suit_directive_process_dependency(NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL) were not detected");

	retval = suit_directive_process_dependency(&state, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL) were not detected");

	retval = suit_directive_process_dependency(NULL, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (NULL, _) were not detected");

	retval = suit_directive_process_dependency(&state, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, retval, "Invalid component type was not detected");

	component_params.is_dependency = suit_bool_true;

	retval = suit_directive_process_dependency(&state, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, retval, "Lack of integrity verification was not detected");

	component_params.integrity_checked = true;

	retval = suit_directive_process_dependency(&state, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, retval, "Invalid execution state was not detected");

	component_params.is_dependency = true;

	retval = suit_directive_process_dependency(&state, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_TAMP, retval, "Tampered component type was not detected");
}

void test_fetch_null_args(void)
{
	struct suit_manifest_params component_params;

	memset(&component_params, 0, sizeof(component_params));

	component_params.uri.value = "http://example.com/file.bin";
	component_params.uri.len = sizeof("http://example.com/file.bin");
	component_params.uri_set = true;

	int retval = suit_directive_fetch(NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL) were not detected");

	retval = suit_directive_fetch(&state, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL) were not detected");

	retval = suit_directive_fetch(NULL, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (NULL, _) were not detected");

	retval = suit_directive_fetch(&state, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, retval, "Invalid execution state was not detected");
}

void test_copy_null_args(void)
{
	struct suit_manifest_params component_params;

	memset(&component_params, 0, sizeof(component_params));

	int retval = suit_directive_copy(NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL) were not detected");

	retval = suit_directive_copy(&state, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL) were not detected");

	retval = suit_directive_copy(NULL, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (NULL, _) were not detected");

	retval = suit_directive_copy(&state, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, retval, "Invalid execution state was not detected");
}

void test_write_null_args(void)
{
	struct suit_manifest_params component_params;

	memset(&component_params, 0, sizeof(component_params));

	component_params.content.value = "test_data";
	component_params.content.len = strlen("test_data");
	component_params.content_set = true;

	int retval = suit_directive_write(NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL) were not detected");

	retval = suit_directive_write(&state, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL) were not detected");

	retval = suit_directive_write(NULL, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (NULL, _) were not detected");
}

void test_swap_null_args(void)
{
	struct suit_manifest_params component_params;

	memset(&component_params, 0, sizeof(component_params));

	int retval = suit_directive_swap(NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL) were not detected");

	retval = suit_directive_swap(&state, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL) were not detected");

	retval = suit_directive_swap(NULL, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (NULL, _) were not detected");

	retval = suit_directive_swap(&state, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMMAND, retval, "Unexpected return code from unsupported suit-directive-swap command");
}

void test_invoke_null_args(void)
{
	struct suit_manifest_params component_params;

	memset(&component_params, 0, sizeof(component_params));

	int retval = suit_directive_invoke(NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "Invalid arguments (NULL, NULL) were not detected");

	retval = suit_directive_invoke(&state, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (_, NULL) were not detected");

	retval = suit_directive_invoke(NULL, &component_params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, retval, "NULL arguments (NULL, _) were not detected");
}
