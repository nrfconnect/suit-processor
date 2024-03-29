/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include <stdint.h>
#include <suit_manifest.h>

struct suit_processor_state state;

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


/* suit-condition-vendor-identifier tests */
void test_seq_execution_condition_vendor_identifier_no_vid(void);
void test_seq_execution_condition_vendor_identifier(void);

/* suit-condition-class-identifier tests */
void test_seq_execution_condition_class_identifier_no_cid(void);
void test_seq_execution_condition_class_identifier(void);

/* suit-condition-device-identifier tests */
void test_seq_execution_condition_device_identifier_no_did(void);
void test_seq_execution_condition_device_identifier(void);

/* suit-condition-image-match tests */
void test_seq_execution_condition_image_match_no_digest(void);
void test_seq_execution_condition_image_match_unsupported_algorithm(void);
void test_seq_execution_condition_image_match_invalid_digest_length(void);
void test_seq_execution_condition_image_match_invalid_digest_length_sha256(void);
void test_seq_execution_condition_image_match_invalid_digest_length_sha512(void);
void test_seq_execution_condition_image_match_failed(void);
void test_seq_execution_condition_image_match(void);
void test_seq_execution_condition_image_match_sha512(void);

/* suit-condition-component-slot tests */
void test_seq_execution_condition_component_slot_no_slot(void);
void test_seq_execution_condition_component_slot(void);

/* suit-condition-check-content tests */
void test_seq_execution_condition_check_content_no_content(void);
void test_seq_execution_condition_check_content(void);

/* suit-condition-abort tests */
void test_seq_execution_condition_abort(void);

/* suit-condition-is-dependency tests */
void test_seq_execution_condition_dependency_fail(void);
void test_seq_execution_condition_dependency_success(void);
void test_seq_execution_condition_dependency_invalid(void);
void test_seq_execution_condition_dependency_mixed(void);
void test_seq_execution_condition_dependency_multiple_dependencies(void);
void test_seq_execution_condition_dependency_integrity_fail(void);
void test_seq_execution_condition_dependency_integrity_invalid(void);

/* suit-condition-dependency-integrity */
void test_seq_execution_condition_dependency_integrity_tampered_component(void);
void test_seq_execution_condition_dependency_integrity_regular_component(void);
void test_seq_execution_condition_dependency_integrity_manifest_not_found(void);
void test_seq_execution_condition_dependency_integrity_invalid_dependency_payload(void);
void test_seq_execution_condition_dependency_integrity_invalid_dependency_contents(void);
void test_seq_execution_condition_dependency_integrity_corrupted_component_stack(void);
void test_seq_execution_condition_dependency_integrity_lazy_component_release(void);
void test_seq_execution_condition_dependency_integrity_valid_dependency(void);
void test_seq_execution_condition_dependency_integrity_integrity_lost_fetch(void);
void test_seq_execution_condition_dependency_integrity_integrity_lost_fetch_integrated(void);
void test_seq_execution_condition_dependency_integrity_integrity_lost_copy(void);
void test_seq_execution_condition_dependency_integrity_integrity_lost_write(void);

/* suit-directive-process-dependency */
void test_seq_execution_process_dependency_tampered_component(void);
void test_seq_execution_process_dependency_regular_component(void);
void test_seq_execution_process_dependency_integrity_unknown(void);
void test_seq_execution_process_dependency_manifest_not_found(void);
void test_seq_execution_process_dependency_manifest_lazy_loader(void);
void test_seq_execution_process_dependency_invalid_dependency_payload(void);
void test_seq_execution_process_dependency_unauthorized_dependency(void);
void test_seq_execution_process_dependency_invalid_dependency_contents(void);
void test_seq_execution_process_dependency_processing_failed(void);
void test_seq_execution_process_dependency_seq_completion_failed(void);
void test_seq_execution_process_dependency_corrupted_component_stack(void);
void test_seq_execution_process_dependency_lazy_component_release(void);
void test_seq_execution_process_dependency_valid_dependency(void);

/* suit-directive-override-parameters tests */
void test_seq_execution_override_parameter_single_component_4params(void);
void test_seq_execution_override_parameter_single_component_6params(void);
void test_seq_execution_override_parameter_single_component_7params(void);
void test_seq_execution_override_parameter_multiple_components_3params(void);
void test_seq_execution_override_parameter_soft_failure(void);
void test_seq_execution_override_parameter_soft_failure_nested(void);

/* suit-directive-set-parameters tests */
void test_seq_execution_set_parameter_single_component_4params(void);
void test_seq_execution_set_parameter_single_component_6params(void);
void test_seq_execution_set_parameter_single_component_7params(void);
void test_seq_execution_set_parameter_multiple_components_3params(void);
void test_seq_execution_set_parameter_two_components_3params(void);
void test_seq_execution_set_parameter_soft_failure(void);
void test_seq_execution_set_parameter_multiple_components_image_size_failed(void);
void test_seq_execution_set_parameter_several_components_with_invalid_index(void);
void test_seq_execution_set_parameter_lazy_platform_image_size_set(void);

/* suit-directive-fetch tests */
void test_seq_execution_fetch_no_uri(void);
void test_seq_execution_fetch_external_uri(void);
void test_seq_execution_fetch_internal_uri(void);

/* suit-directive-copy tests */
void test_seq_execution_copy_no_src(void);
void test_seq_execution_copy_src_invalid(void);
void test_seq_execution_copy_current_not_set(void);
void test_seq_execution_copy_ok(void);

/* suit-directive-write tests */
void test_seq_execution_write_no_content(void);
void test_seq_execution_write_content(void);

/* suit-directive-swap tests */
void test_seq_execution_swap(void);

/* suit-directive-invoke tests */
void test_seq_execution_invoke_no_args(void);
void test_seq_execution_invoke_with_args(void);

/* NULL args in suit directives tests */
void test_set_current_components_null_args(void);
void test_try_each_null_args(void);
void test_run_sequence_null_args(void);
void test_override_parameters_null_args(void);
void test_set_parameters_null_args(void);
void test_process_dependency_null_args(void);
void test_fetch_null_args(void);
void test_copy_null_args(void);
void test_write_null_args(void);
void test_swap_null_args(void);
void test_invoke_null_args(void);


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
