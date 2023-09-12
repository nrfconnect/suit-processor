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
void test_seq_execution_condition_image_match_no_image_size(void);
void test_seq_execution_condition_image_match_unsupported_algorithm(void);
void test_seq_execution_condition_image_match_invalid_digest_length(void);
void test_seq_execution_condition_image_match_failed(void);
void test_seq_execution_condition_image_match(void);

/* suit-condition-component-slot tests */
void test_seq_execution_condition_component_slot_no_slot(void);
void test_seq_execution_condition_component_slot(void);

/* suit-condition-abort tests */
void test_seq_execution_condition_abort(void);

/* suit-directive-override-parameters tests */
void test_seq_execution_override_parameter_single_component_3params(void);
void test_seq_execution_override_parameter_single_component_6params(void);
void test_seq_execution_override_parameter_single_component_7params(void);
void test_seq_execution_override_parameter_multiple_components_3params(void);
void test_seq_execution_override_parameter_soft_failure(void);
void test_seq_execution_override_parameter_soft_failure_nested(void);

/* suit-directive-set-parameters tests */
void test_seq_execution_set_parameter_single_component_3params(void);
void test_seq_execution_set_parameter_single_component_6params(void);
void test_seq_execution_set_parameter_single_component_7params(void);
void test_seq_execution_set_parameter_multiple_components_3params(void);
void test_seq_execution_set_parameter_soft_failure(void);

/* suit-directive-fetch tests */
void test_seq_execution_fetch_no_uri(void);
void test_seq_execution_fetch_external_uri(void);
void test_seq_execution_fetch_internal_uri(void);

/* suit-directive-copy tests */
void test_seq_execution_copy_no_src(void);
void test_seq_execution_copy_src_invalid(void);
void test_seq_execution_copy_current_not_set(void);
void test_seq_execution_copy_ok(void);

/* suit-directive-swap tests */
void test_seq_execution_swap(void);

/* suit-directive-invoke tests */
void test_seq_execution_invoke_no_args(void);
void test_seq_execution_invoke_with_args(void);


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
