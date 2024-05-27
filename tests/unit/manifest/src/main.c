/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include <stdint.h>
#include <suit_processor.h>
#include <suit_manifest.h>
#include "suit_platform/cmock_suit_platform.h"

static struct suit_manifest_params components[SUIT_MAX_NUM_COMPONENT_PARAMS];

void uninitialized_append_component(void);
void uninitialized_append_dependency(void);
void uninitialized_release(void);
void uninitialized_get_component_params(void);

void setUp(void)
{
	int ret = SUIT_SUCCESS;
	static bool initialized = false;

	if (!initialized) {
		uninitialized_append_component();
		uninitialized_append_dependency();
		uninitialized_release();
		uninitialized_get_component_params();

		ret = suit_manifest_params_init((struct suit_manifest_params *)&components, ZCBOR_ARRAY_SIZE(components));
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to initialize SUIT manifest parameters");

		initialized = true;
	} else {
		ret = suit_manifest_params_init((struct suit_manifest_params *)&components, ZCBOR_ARRAY_SIZE(components));
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "Unable to initialize SUIT manifest parameters");
		memset(components, 0, sizeof(components));
	}

	/* Just to be sure that the macro returns the correct value. */
	TEST_ASSERT_EQUAL_MESSAGE(ZCBOR_ARRAY_SIZE(components), SUIT_MAX_NUM_COMPONENT_PARAMS, "The number of component parameters entries does not match");
}

void test_params_init_invalid_input(void)
{
	int ret = SUIT_SUCCESS;

	ret = suit_manifest_params_init(NULL, SUIT_MAX_NUM_COMPONENTS);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "Initialization with NULL pointer returned unexpected value");

	ret = suit_manifest_params_init((struct suit_manifest_params *)&components, 0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "Initialization with too small memory returned unexpected value");

	ret = suit_manifest_params_init((struct suit_manifest_params *)&components, SUIT_MAX_NUM_COMPONENTS);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "Initialization of already initialized module returned unexpected value");
}

void uninitialized_append_component(void)
{
	struct suit_manifest_state manifest;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;
	static struct zcbor_string sample_component_0 = {
		.value = "TEST_COMPONENT_0",
		.len = sizeof("TEST_COMPONENT_0"),
	};

	int ret = suit_manifest_append_component(&manifest, &sample_component_0);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "A component was appended on uninitialized module");
}

void test_append_component_invalid_input(void)
{
	struct suit_manifest_state manifest;
	int ret = SUIT_SUCCESS;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;
	static struct zcbor_string sample_component_0 = {
		.value = "TEST_COMPONENT_0",
		.len = sizeof("TEST_COMPONENT_0"),
	};

	ret = suit_manifest_append_component(NULL, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "A component was appended to NULL manifest");
	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");

	ret = suit_manifest_append_component(&manifest, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "A NULL component was appended to manifest");
	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");
}

void test_append_component_invalid_state(void)
{
	struct suit_manifest_state manifest;
	int ret = SUIT_SUCCESS;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;
	static struct zcbor_string sample_component_0 = {
		.value = "TEST_COMPONENT_0",
		.len = sizeof("TEST_COMPONENT_0"),
	};

	/* Create an invalid component inside the database. */
	components[0].component_handle = 0;
	components[0].is_dependency = suit_bool_true;
	components[0].ref_count = 1;
	components[0].component_id = sample_component_0;

	ret = suit_manifest_append_component(&manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "A component with invalid dependency flag was added");

	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");
	TEST_ASSERT_EQUAL_MESSAGE(1, components[0].ref_count, "Unexpected value of the reference counter");
	TEST_ASSERT_EQUAL_MESSAGE(suit_bool_true, components[0].is_dependency, "Unexpected value of component dependency flag");
}

void test_append_component_unsupported_id(void)
{
	struct suit_manifest_state manifest;
	int ret = SUIT_SUCCESS;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;
	static struct zcbor_string sample_component_0 = {
		.value = "TEST_COMPONENT_0",
		.len = sizeof("TEST_COMPONENT_0"),
	};

	__cmock_suit_plat_create_component_handle_ExpectAndReturn(&sample_component_0, NULL, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();

	ret = suit_manifest_append_component(&manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "An unsupported component was added");

	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");
	TEST_ASSERT_EQUAL_MESSAGE(0, components[0].ref_count, "Unexpected value of the reference counter");
}

void test_append_component_fill_array(void)
{
	suit_component_t component_handle;
	struct suit_manifest_state manifest;
	struct suit_manifest_state sub_manifest;
	int ret = SUIT_SUCCESS;

	static struct zcbor_string prefix = {
		.value = NULL,
		.len = 0,
	};

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;

	/* Initialize the sub_manifest structure. */
	memset(&sub_manifest, 0, sizeof(sub_manifest));
	sub_manifest.components_count = 0;

	/* Initialize the sample component ID.
	 * The remaining component IDs will be created by changing the length of the static string.
	 */
	static struct zcbor_string sample_component_0 = {
		.value = "TEST_COMPONENT_0123456789abcdef67890abcd123456789abcdef67890abcd0123",
		.len = sizeof("TEST_COMPONENT_0"),
	};

	/* Verify that the test boundaries can be reached. */
	TEST_ASSERT_LESS_THAN_MESSAGE(
		sizeof("TEST_COMPONENT_0123456789abcdef67890abcd123456789abcdef67890abcd0123") - sizeof("TEST_COMPONENT_0"),
		SUIT_MAX_NUM_COMPONENTS,
		"Unable to reach maximum number of components. Please extend the component ID generator"
	);

	TEST_ASSERT_LESS_THAN_MESSAGE(
		sizeof("TEST_COMPONENT_0123456789abcdef67890abcd123456789abcdef67890abcd0123") - sizeof("TEST_COMPONENT_0"),
		SUIT_MAX_NUM_COMPONENT_PARAMS,
		"Unable to reach maximum number of components. Please extend the component ID generator"
	);

	/* Fill the component array. */
	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		component_handle = i;
		sample_component_0.len = strlen("TEST_COMPONENT_0") + i;

		__cmock_suit_plat_create_component_handle_ExpectAndReturn(&sample_component_0, NULL, SUIT_SUCCESS);
		__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
		__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&component_handle);

		ret = suit_manifest_append_component(&manifest, &sample_component_0);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to append a new component");

		/* Verify the component mapping. */
		TEST_ASSERT_EQUAL_MESSAGE(i + 1, manifest.components_count, "The manifest component counter was not increased");
		TEST_ASSERT_EQUAL_MESSAGE(i, manifest.component_map[i], "Unexpected value of the component mapping");

		/* Verify the contents of the created component. */
		TEST_ASSERT_EQUAL_MESSAGE(component_handle, components[i].component_handle, "Unexpected value of the component handle");
		TEST_ASSERT_EQUAL_MESSAGE(1, components[i].ref_count, "Unexpected value of the reference counter");
		TEST_ASSERT_EQUAL_PTR_MESSAGE(sample_component_0.value, components[i].component_id.value, "Unexpected value of component ID value");
		TEST_ASSERT_EQUAL_MESSAGE(strlen("TEST_COMPONENT_0") + i, components[i].component_id.len, "Unexpected value of component ID length");
		TEST_ASSERT_EQUAL_MESSAGE(suit_bool_false, components[i].is_dependency, "Unexpected value of component dependency flag");
		TEST_ASSERT_EQUAL_MESSAGE(false, components[i].integrity_checked, "Invalid initial value of the integrity flag");
	}

	/* Verify that the manifest component array is full. */
	component_handle = SUIT_MAX_NUM_COMPONENTS;
	sample_component_0.len = strlen("TEST_COMPONENT_0") + SUIT_MAX_NUM_COMPONENTS;

	ret = suit_manifest_append_component(&manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "Manifest capacity reached, but the new component was added");

	ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "Manifest capacity reached, but the new component was added");

	component_handle = 0;
	sample_component_0.len = strlen("TEST_COMPONENT_0") + 0;

	ret = suit_manifest_append_component(&manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "Manifest capacity reached, but the existing component was added");
	TEST_ASSERT_EQUAL_MESSAGE(1, components[0].ref_count, "Unexpected value of the reference counter after failed component addition");

	ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "Manifest capacity reached, but the existing component was added");
	TEST_ASSERT_EQUAL_MESSAGE(1, components[0].ref_count, "Unexpected value of the reference counter after failed component addition");

	/* Fill half of the array with the same IDs, using the second manifest,
	 * so that should result in the increased reference counters.
	 */
	for (size_t i = 0; i < (SUIT_MAX_NUM_COMPONENTS / 2); i++) {
		component_handle = i;
		sample_component_0.len = strlen("TEST_COMPONENT_0") + i;

		ret = suit_manifest_append_component(&sub_manifest, &sample_component_0);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to append an existing component to submanifest");

		/* Verify the component mapping. */
		TEST_ASSERT_EQUAL_MESSAGE(i + 1, sub_manifest.components_count, "The manifest component counter was not increased");
		TEST_ASSERT_EQUAL_MESSAGE(i, sub_manifest.component_map[i], "Unexpected value of the component mapping");

		/* Verify the contents of the created component. */
		TEST_ASSERT_EQUAL_MESSAGE(component_handle, components[i].component_handle, "Unexpected value of the component handle");
		TEST_ASSERT_EQUAL_MESSAGE(2, components[i].ref_count, "Unexpected value of the reference counter");
		TEST_ASSERT_EQUAL_PTR_MESSAGE(sample_component_0.value, components[i].component_id.value, "Unexpected value of component ID value");
		TEST_ASSERT_EQUAL_MESSAGE(strlen("TEST_COMPONENT_0") + i, components[i].component_id.len, "Unexpected value of component ID length");
		TEST_ASSERT_EQUAL_MESSAGE(suit_bool_false, components[i].is_dependency, "Unexpected value of component dependency flag");
		TEST_ASSERT_EQUAL_MESSAGE(false, components[i].integrity_checked, "Invalid initial value of the integrity flag");
	}

	/* Reach the boundaries of component params storage using single, flakey slot of the submanifest. */
	for (size_t i = SUIT_MAX_NUM_COMPONENTS; i < SUIT_MAX_NUM_COMPONENT_PARAMS; i++) {
		component_handle = i;
		sample_component_0.len = strlen("TEST_COMPONENT_0") + i;

		__cmock_suit_plat_create_component_handle_ExpectAndReturn(&sample_component_0, NULL, SUIT_SUCCESS);
		__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
		__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&component_handle);

		ret = suit_manifest_append_component(&sub_manifest, &sample_component_0);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to append a new component to submanifest");

		/* Verify the component mapping. */
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_MAX_NUM_COMPONENTS / 2 + 1, sub_manifest.components_count, "The manifest component counter was not increased");
		TEST_ASSERT_EQUAL_MESSAGE(i, sub_manifest.component_map[sub_manifest.components_count - 1], "Unexpected value of the component mapping");

		/* Verify the contents of the created component. */
		TEST_ASSERT_EQUAL_MESSAGE(component_handle, components[i].component_handle, "Unexpected value of the component handle");
		TEST_ASSERT_EQUAL_MESSAGE(1, components[i].ref_count, "Unexpected value of the reference counter");
		TEST_ASSERT_EQUAL_PTR_MESSAGE(sample_component_0.value, components[i].component_id.value, "Unexpected value of component ID value");
		TEST_ASSERT_EQUAL_MESSAGE(strlen("TEST_COMPONENT_0") + i, components[i].component_id.len, "Unexpected value of component ID length");
		TEST_ASSERT_EQUAL_MESSAGE(suit_bool_false, components[i].is_dependency, "Unexpected value of component dependency flag");
		TEST_ASSERT_EQUAL_MESSAGE(false, components[i].integrity_checked, "Invalid initial value of the integrity flag");

		/* Hack the last entry of manifest mapping to use it once again. */
		sub_manifest.components_count--;
	}

	/* Verify that the component parameters array is full. */
	component_handle = SUIT_MAX_NUM_COMPONENT_PARAMS;
	sample_component_0.len = strlen("TEST_COMPONENT_0") + SUIT_MAX_NUM_COMPONENT_PARAMS;

	ret = suit_manifest_append_component(&sub_manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_OVERFLOW, ret, "Parameters capacity reached, but a new component was added");

	ret = suit_manifest_append_dependency(&sub_manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_OVERFLOW, ret, "Parameters capacity reached, but a new component was added");

	/* Verify that it is still possible to assign existing components to manifests. */
	for (size_t i = (SUIT_MAX_NUM_COMPONENTS / 2); i < SUIT_MAX_NUM_COMPONENTS; i++) {
		component_handle = i;
		sample_component_0.len = strlen("TEST_COMPONENT_0") + i;

		TEST_ASSERT_EQUAL_MESSAGE(1, components[i].ref_count, "Unexpected value of the reference counter");

		ret = suit_manifest_append_component(&sub_manifest, &sample_component_0);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to append an existing component to submanifest");

		/* Verify the component mapping. */
		TEST_ASSERT_EQUAL_MESSAGE(i + 1, sub_manifest.components_count, "The manifest component counter was not increased");
		TEST_ASSERT_EQUAL_MESSAGE(i, sub_manifest.component_map[i], "Unexpected value of the component mapping");

		/* Verify the contents of the created component. */
		TEST_ASSERT_EQUAL_MESSAGE(component_handle, components[i].component_handle, "Unexpected value of the component handle");
		TEST_ASSERT_EQUAL_MESSAGE(2, components[i].ref_count, "Unexpected value of the reference counter");
		TEST_ASSERT_EQUAL_PTR_MESSAGE(sample_component_0.value, components[i].component_id.value, "Unexpected value of component ID value");
		TEST_ASSERT_EQUAL_MESSAGE(strlen("TEST_COMPONENT_0") + i, components[i].component_id.len, "Unexpected value of component ID length");
		TEST_ASSERT_EQUAL_MESSAGE(suit_bool_false, components[i].is_dependency, "Unexpected value of component dependency flag");
		TEST_ASSERT_EQUAL_MESSAGE(false, components[i].integrity_checked, "Invalid initial value of the integrity flag");
	}

	/* Verify that the both arrays are full. */
	component_handle = SUIT_MAX_NUM_COMPONENTS;
	sample_component_0.len = strlen("TEST_COMPONENT_0") + SUIT_MAX_NUM_COMPONENTS;

	ret = suit_manifest_append_component(&sub_manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "Manifest capacity reached, but the new component was added");

	component_handle = 0;
	sample_component_0.len = strlen("TEST_COMPONENT_0") + 0;

	ret = suit_manifest_append_component(&sub_manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "Manifest capacity reached, but the existing component was added");
	TEST_ASSERT_EQUAL_MESSAGE(2, components[0].ref_count, "Unexpected value of the reference counter after failed component addition");
}

void uninitialized_append_dependency(void)
{
	struct suit_manifest_state manifest;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;
	static struct zcbor_string sample_component_0 = {
		.value = "TEST_COMPONENT_0",
		.len = sizeof("TEST_COMPONENT_0"),
	};
	static struct zcbor_string prefix = {
		.value = NULL,
		.len = 0,
	};

	int ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &prefix);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "A dependency was appended on uninitialized module");
}

void test_append_dependency_invalid_input(void)
{
	struct suit_manifest_state manifest;
	int ret = SUIT_SUCCESS;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;
	static struct zcbor_string sample_component_0 = {
		.value = "TEST_COMPONENT_0",
		.len = sizeof("TEST_COMPONENT_0"),
	};
	static struct zcbor_string prefix = {
		.value = NULL,
		.len = 0,
	};

	ret = suit_manifest_append_dependency(NULL, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "A dependency was appended to NULL manifest");
	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");

	ret = suit_manifest_append_dependency(&manifest, NULL, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "A NULL component was appended to manifest");
	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");

	ret = suit_manifest_append_dependency(&manifest, &sample_component_0, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "A NULL prefix was appended to manifest");
	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");

	ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "A dependency with non-empty prefix was appended to manifest");
	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");

	prefix.value = sample_component_0.value;
	ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "A dependency with non-empty prefix was appended to manifest");
	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");

	prefix.value = NULL;
	prefix.len = sample_component_0.len;
	ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "A dependency with non-empty prefix was appended to manifest");
	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");
}

void test_append_dependency_invalid_state(void)
{
	struct suit_manifest_state manifest;
	int ret = SUIT_SUCCESS;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;
	static struct zcbor_string sample_component_0 = {
		.value = "TEST_COMPONENT_0",
		.len = sizeof("TEST_COMPONENT_0"),
	};
	static struct zcbor_string prefix = {
		.value = NULL,
		.len = 0,
	};

	/* Create an invalid component inside the database. */
	components[0].component_handle = 0;
	components[0].is_dependency = suit_bool_false;
	components[0].ref_count = 1;
	components[0].component_id = sample_component_0;

	ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "A component with invalid dependency flag was added");

	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");
	TEST_ASSERT_EQUAL_MESSAGE(1, components[0].ref_count, "Unexpected value of the reference counter");
	TEST_ASSERT_EQUAL_MESSAGE(suit_bool_false, components[0].is_dependency, "Unexpected value of component dependency flag");
}

void test_append_dependency_unsupported_id(void)
{
	struct suit_manifest_state manifest;
	int ret = SUIT_SUCCESS;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;
	static struct zcbor_string sample_component_0 = {
		.value = "TEST_COMPONENT_0",
		.len = sizeof("TEST_COMPONENT_0"),
	};
	static struct zcbor_string prefix = {
		.value = NULL,
		.len = 0,
	};

	__cmock_suit_plat_create_component_handle_ExpectAndReturn(&sample_component_0, NULL, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();

	ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "An unsupported component was added");

	TEST_ASSERT_EQUAL_MESSAGE(0, manifest.components_count, "The manifest component counter was increased");
	TEST_ASSERT_EQUAL_MESSAGE(0, components[0].ref_count, "Unexpected value of the reference counter");
}

void test_append_dependency_fill_array(void)
{
	suit_component_t component_handle;
	struct suit_manifest_state manifest;
	int ret = SUIT_SUCCESS;

	static struct zcbor_string prefix = {
		.value = NULL,
		.len = 0,
	};

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;

	/* Initialize the sample component ID.
	 * The remaining component IDs will be created by changing the length of the static string.
	 */
	static struct zcbor_string sample_component_0 = {
		.value = "TEST_COMPONENT_0123456789abcdef67890abcd123456789abcdef67890abcd0123",
		.len = sizeof("TEST_COMPONENT_0"),
	};

	/* Verify that the test boundaries can be reached. */
	TEST_ASSERT_LESS_THAN_MESSAGE(
		sizeof("TEST_COMPONENT_0123456789abcdef67890abcd123456789abcdef67890abcd0123") - sizeof("TEST_COMPONENT_0"),
		SUIT_MAX_NUM_COMPONENTS,
		"Unable to reach maximum number of components. Please extend the component ID generator"
	);

	TEST_ASSERT_LESS_THAN_MESSAGE(
		sizeof("TEST_COMPONENT_0123456789abcdef67890abcd123456789abcdef67890abcd0123") - sizeof("TEST_COMPONENT_0"),
		SUIT_MAX_NUM_COMPONENT_PARAMS,
		"Unable to reach maximum number of components. Please extend the component ID generator"
	);

	/* Fill the component array. */
	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		component_handle = i;
		sample_component_0.len = strlen("TEST_COMPONENT_0") + i;

		__cmock_suit_plat_create_component_handle_ExpectAndReturn(&sample_component_0, NULL, SUIT_SUCCESS);
		__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
		__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&component_handle);

		ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &prefix);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to append a new component");

		/* Verify the component mapping. */
		TEST_ASSERT_EQUAL_MESSAGE(i + 1, manifest.components_count, "The manifest component counter was not increased");
		TEST_ASSERT_EQUAL_MESSAGE(i, manifest.component_map[i], "Unexpected value of the component mapping");

		/* Verify the contents of the created component. */
		TEST_ASSERT_EQUAL_MESSAGE(component_handle, components[i].component_handle, "Unexpected value of the component handle");
		TEST_ASSERT_EQUAL_MESSAGE(1, components[i].ref_count, "Unexpected value of the reference counter");
		TEST_ASSERT_EQUAL_PTR_MESSAGE(sample_component_0.value, components[i].component_id.value, "Unexpected value of component ID value");
		TEST_ASSERT_EQUAL_MESSAGE(strlen("TEST_COMPONENT_0") + i, components[i].component_id.len, "Unexpected value of component ID length");
		TEST_ASSERT_EQUAL_MESSAGE(suit_bool_true, components[i].is_dependency, "Unexpected value of component dependency flag");
		TEST_ASSERT_EQUAL_MESSAGE(false, components[i].integrity_checked, "Invalid initial value of the integrity flag");
	}

	/* Verify that the manifest component array is full. */
	component_handle = SUIT_MAX_NUM_COMPONENTS;
	sample_component_0.len = strlen("TEST_COMPONENT_0") + SUIT_MAX_NUM_COMPONENTS;

	ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "Manifest capacity reached, but the new component was added");

	ret = suit_manifest_append_component(&manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "Manifest capacity reached, but the new component was added");

	component_handle = 0;
	sample_component_0.len = strlen("TEST_COMPONENT_0") + 0;

	ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "Manifest capacity reached, but the existing component was added");
	TEST_ASSERT_EQUAL_MESSAGE(1, components[0].ref_count, "Unexpected value of the reference counter after failed component addition");

	ret = suit_manifest_append_component(&manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "Manifest capacity reached, but the existing component was added");
	TEST_ASSERT_EQUAL_MESSAGE(1, components[0].ref_count, "Unexpected value of the reference counter after failed component addition");
}

void uninitialized_release(void)
{
	struct suit_manifest_state manifest;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;

	int ret = suit_manifest_release(&manifest);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "A manifest was released on uninitialized module");
}

void test_release_invalid_input(void)
{
	struct suit_manifest_state manifest;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;

	int ret = suit_manifest_release(NULL);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "A NULL manifest was released");
}

void test_release_invalid_state(void)
{
	int ret = SUIT_SUCCESS;
	struct suit_manifest_state manifest;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));

	/* Map a component with cleared reference counter. */
	manifest.components_count = 1;
	manifest.component_map[0] = 0;

	ret = suit_manifest_release(&manifest);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MISSING_COMPONENT, ret, "An uninitialized component was released");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.components_count, "The manifest component counter was decreased");

	/* Map a component outside of the parameters boundaries. */
	manifest.components_count = 1;
	manifest.component_map[0] = SUIT_MAX_NUM_COMPONENT_PARAMS;

	ret = suit_manifest_release(&manifest);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MISSING_COMPONENT, ret, "An out of range component was released");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.components_count, "The manifest component counter was decreased");

	/* Fail to release the component on the platform level. */
	components[0].ref_count = 1;
	components[0].component_handle = 123;
	components[0].is_dependency = suit_bool_false;
	manifest.components_count = 1;
	manifest.component_map[0] = 0;

	__cmock_suit_plat_release_component_handle_ExpectAndReturn(123, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);

	ret = suit_manifest_release(&manifest);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "The platfrom-level failure was not propagated");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.components_count, "The manifest component counter was decreased");
	TEST_ASSERT_EQUAL_MESSAGE(1, components[0].ref_count, "The component reference counter was decreased");
	TEST_ASSERT_EQUAL_MESSAGE(suit_bool_false, components[0].is_dependency, "The dependency flag was altered");
}

void test_release_appended_components(void)
{
	suit_component_t component_handle;
	struct suit_manifest_state manifest;
	struct suit_manifest_state sub_manifest;
	int ret = SUIT_SUCCESS;

	static struct zcbor_string prefix = {
		.value = NULL,
		.len = 0,
	};

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 0;

	/* Initialize the sub_manifest structure. */
	memset(&sub_manifest, 0, sizeof(sub_manifest));
	sub_manifest.components_count = 0;

	/* Initialize the sample component ID.
	 * The remaining component IDs will be created by changing the length of the static string.
	 */
	static struct zcbor_string sample_component_0 = {
		.value = "TEST_COMPONENT_0123",
		.len = sizeof("TEST_COMPONENT_0"),
	};

	/* Create components */
	component_handle = 0;
	sample_component_0.len = strlen("TEST_COMPONENT_0") + 0;

	__cmock_suit_plat_create_component_handle_ExpectAndReturn(&sample_component_0, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&component_handle);

	ret = suit_manifest_append_component(&manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to append a new component");

	component_handle = 1;
	sample_component_0.len = strlen("TEST_COMPONENT_0") + 1;

	__cmock_suit_plat_create_component_handle_ExpectAndReturn(&sample_component_0, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&component_handle);

	ret = suit_manifest_append_component(&manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to append a new component");

	ret = suit_manifest_append_component(&sub_manifest, &sample_component_0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to append a new component");

	component_handle = 2;
	sample_component_0.len = strlen("TEST_COMPONENT_0") + 2;

	__cmock_suit_plat_create_component_handle_ExpectAndReturn(&sample_component_0, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&component_handle);

	ret = suit_manifest_append_dependency(&sub_manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to append a new component");

	ret = suit_manifest_append_dependency(&manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to append a new component");

	component_handle = 3;
	sample_component_0.len = strlen("TEST_COMPONENT_0") + 3;

	__cmock_suit_plat_create_component_handle_ExpectAndReturn(&sample_component_0, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&component_handle);

	ret = suit_manifest_append_dependency(&sub_manifest, &sample_component_0, &prefix);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to append a new component");

	TEST_ASSERT_EQUAL_MESSAGE(1, components[0].ref_count, "Unexpected value of the reference counter");
	TEST_ASSERT_EQUAL_MESSAGE(2, components[1].ref_count, "Unexpected value of the reference counter");
	TEST_ASSERT_EQUAL_MESSAGE(2, components[2].ref_count, "Unexpected value of the reference counter");
	TEST_ASSERT_EQUAL_MESSAGE(1, components[3].ref_count, "Unexpected value of the reference counter");

	/* Release the first manifest. */
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(0, SUIT_SUCCESS);

	ret = suit_manifest_release(&manifest);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to release manifest");

	TEST_ASSERT_EQUAL_MESSAGE(0, components[0].ref_count, "Unexpected value of the reference counter");
	TEST_ASSERT_EQUAL_MESSAGE(1, components[1].ref_count, "Unexpected value of the reference counter");
	TEST_ASSERT_EQUAL_MESSAGE(1, components[2].ref_count, "Unexpected value of the reference counter");
	TEST_ASSERT_EQUAL_MESSAGE(1, components[3].ref_count, "Unexpected value of the reference counter");

	/* Release the second manifest. */
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(1, SUIT_SUCCESS);
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(2, SUIT_SUCCESS);
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(3, SUIT_SUCCESS);

	ret = suit_manifest_release(&sub_manifest);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to release manifest");

	TEST_ASSERT_EQUAL_MESSAGE(0, components[0].ref_count, "Unexpected value of the reference counter");
	TEST_ASSERT_EQUAL_MESSAGE(0, components[1].ref_count, "Unexpected value of the reference counter");
	TEST_ASSERT_EQUAL_MESSAGE(0, components[2].ref_count, "Unexpected value of the reference counter");
	TEST_ASSERT_EQUAL_MESSAGE(0, components[3].ref_count, "Unexpected value of the reference counter");
}

void uninitialized_get_component_params(void)
{
	struct suit_manifest_params *params;
	struct suit_manifest_state manifest;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 1;
	manifest.component_map[0] = 0;
	components[0].ref_count = 1;
	components[0].component_handle = 123;

	int ret = suit_manifest_get_component_params(&manifest, 0, &params);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "Parameters returned on uninitialized module");
}

void test_get_component_params(void)
{
	int ret = SUIT_SUCCESS;
	struct suit_manifest_params *params;
	struct suit_manifest_state manifest;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.components_count = 1;
	manifest.component_map[0] = 0;
	components[0].ref_count = 1;
	components[0].component_handle = 123;

	ret = suit_manifest_get_component_params(NULL, 0, &params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "Parameters returned for NULL manifest");

	ret = suit_manifest_get_component_params(&manifest, 0, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "Parameters returned to NULL pointer");

	ret = suit_manifest_get_component_params(&manifest, SUIT_MAX_NUM_COMPONENTS, &params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MISSING_COMPONENT, ret, "Parameters returned for out-of-range component");

	manifest.component_map[0] = SUIT_MAX_NUM_COMPONENT_PARAMS;
	ret = suit_manifest_get_component_params(&manifest, 0, &params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MISSING_COMPONENT, ret, "Parameters returned for out-of-range component");

	manifest.component_map[0] = 1;
	ret = suit_manifest_get_component_params(&manifest, 0, &params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MISSING_COMPONENT, ret, "Parameters returned for non-referenced component");

	manifest.component_map[0] = 0;
	ret = suit_manifest_get_component_params(&manifest, 0, &params);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to get component parameters");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(params, &components[0], "Invalid reference to component parameters returned");
}

void test_get_command_seq_invalid_input(void)
{
	int ret;
	struct zcbor_string *seq;

	enum suit_command_sequence seq_name;

	for (seq_name = SUIT_SEQ_INVALID; seq_name <= SUIT_SEQ_MAX; seq_name++) {
		ret = suit_manifest_get_command_seq(NULL, seq_name, &seq);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "Sequence did not return SUIT_ERR_CRASH for a NULL manifest");
	}
}

void test_get_command_seq_empty_manifest(void)
{
	int ret;
	struct suit_manifest_state manifest;
	struct zcbor_string *seq;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));

	enum suit_command_sequence seq_name;

	for (seq_name = SUIT_SEQ_INVALID; seq_name <= SUIT_SEQ_MAX; seq_name++) {
		ret = suit_manifest_get_command_seq(&manifest, seq_name, &seq);
		TEST_ASSERT_NOT_EQUAL_MESSAGE(SUIT_SUCCESS, ret,
			"Sequence returned incorrect error code for an empty manifest");
	}
}

void test_get_command_seq_unavailable(void)
{
	int ret;
	struct suit_manifest_state manifest;
	struct zcbor_string *seq;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.shared_sequence_status = UNAVAILABLE;
	manifest.payload_fetch_seq_status = UNAVAILABLE;
	manifest.install_seq_status = UNAVAILABLE;
	manifest.validate_seq_status = UNAVAILABLE;
	manifest.load_seq_status = UNAVAILABLE;
	manifest.invoke_seq_status = UNAVAILABLE;

	enum suit_command_sequence seq_name;

	for (seq_name = SUIT_SEQ_INVALID; seq_name <= SUIT_SEQ_MAX; seq_name++) {
		ret = suit_manifest_get_command_seq(&manifest, seq_name, &seq);
		TEST_ASSERT_NOT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Getting unavailable sequence succeeded");
	}
}

void test_get_command_seq_severed(void)
{
	int ret;
	struct suit_manifest_state manifest;
	struct zcbor_string *seq;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.shared_sequence_status = SEVERED;
	manifest.payload_fetch_seq_status = SEVERED;
	manifest.install_seq_status = SEVERED;
	manifest.validate_seq_status = SEVERED;
	manifest.load_seq_status = SEVERED;
	manifest.invoke_seq_status = SEVERED;
	manifest.dependency_resolution_seq_status = SEVERED;
	manifest.candidate_verification_seq_status = SEVERED;

	enum suit_command_sequence seq_name;

	for (seq_name = SUIT_SEQ_INVALID; seq_name <= SUIT_SEQ_MAX; seq_name++) {
		ret = suit_manifest_get_command_seq(&manifest, seq_name, &seq);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "Incorrect error code returned");
	}
}

void test_get_command_seq_authenticated(void)
{
	int ret;
	struct suit_manifest_state manifest;
	struct zcbor_string *seq;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.shared_sequence_status = AUTHENTICATED;
	manifest.payload_fetch_seq_status = AUTHENTICATED;
	manifest.install_seq_status = AUTHENTICATED;
	manifest.validate_seq_status = AUTHENTICATED;
	manifest.load_seq_status = AUTHENTICATED;
	manifest.invoke_seq_status = AUTHENTICATED;

	enum suit_command_sequence seq_name;

	for (seq_name = SUIT_SEQ_INVALID; seq_name <= SUIT_SEQ_MAX; seq_name++) {
		struct zcbor_string *exp_seq = NULL;
		seq = NULL;

		switch (seq_name) {
		case SUIT_SEQ_SHARED:
			exp_seq = &manifest.shared_sequence;
			break;
		case SUIT_SEQ_PAYLOAD_FETCH:
			exp_seq = &manifest.payload_fetch_seq;
			break;
		case SUIT_SEQ_INSTALL:
			exp_seq = &manifest.install_seq;
			break;
		case SUIT_SEQ_VALIDATE:
			exp_seq = &manifest.validate_seq;
			break;
		case SUIT_SEQ_LOAD:
			exp_seq = &manifest.load_seq;
			break;
		case SUIT_SEQ_INVOKE:
			exp_seq = &manifest.invoke_seq;
			break;
		default:
			break;
		}

		ret = suit_manifest_get_command_seq(&manifest, seq_name, &seq);

		if (exp_seq != NULL) {
			TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Getting an available sequence failed");
			TEST_ASSERT_EQUAL_PTR_MESSAGE(exp_seq, seq, "Invalid sequence returned");
		} else {
			TEST_ASSERT_NOT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Getting unavailable sequence succeeded");
			TEST_ASSERT_NULL_MESSAGE(seq, "Invalid sequence returned");
		}
	}
}

void test_get_integrated_payload_invalid_input(void)
{
	struct suit_manifest_state manifest;
	struct zcbor_string uri = {
		.value = "http://example.com",
		.len = strlen("http://example.com"),
	};
	struct zcbor_string payload;
	int ret = SUIT_SUCCESS;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.integrated_payloads_count = 0;

	ret = suit_manifest_get_integrated_payload(NULL, &uri, &payload);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "Payload returned from NULL manifest");

	ret = suit_manifest_get_integrated_payload(&manifest, NULL, &payload);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "Payload returned from NULL URI");

	ret = suit_manifest_get_integrated_payload(&manifest, &uri, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "Payload returned from NULL payload");

	ret = suit_manifest_get_integrated_payload(&manifest, &uri, &payload);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNAVAILABLE_PAYLOAD, ret, "Payload returned from empty manifest");
}

void test_get_integrated_payload_nonempty_uri(void)
{
	struct suit_manifest_state manifest;
	struct zcbor_string uri_0 = {
		.value = "http://example.com",
		.len = strlen("http://example.com"),
	};
	struct zcbor_string uri_1 = {
		.value = "http://sample.com",
		.len = strlen("http://sample.com"),
	};
	struct zcbor_string uri_2 = {
		.value = "#app.bin",
		.len = strlen("#app.bin"),
	};
	struct zcbor_string payload_0 = {
		.value = "CAFECAFE",
		.len = sizeof("CAFECAFE"),
	};
	struct zcbor_string payload_1 = {
		.value = "ABCD",
		.len = sizeof("ABCD"),
	};
	struct zcbor_string payload_2 = {
		.value = "This is a sample FW file",
		.len = sizeof("This is a sample FW file"),
	};
	struct zcbor_string payload;
	int ret = SUIT_SUCCESS;

	/* Initialize the manifest structure. */
	memset(&manifest, 0, sizeof(manifest));
	manifest.integrated_payloads[0].key = uri_0;
	manifest.integrated_payloads[1].key = uri_1;
	manifest.integrated_payloads[2].key = uri_2;
	manifest.integrated_payloads[0].payload = payload_0;
	manifest.integrated_payloads[1].payload = payload_1;
	manifest.integrated_payloads[2].payload = payload_2;
	manifest.integrated_payloads_count = 2;

	ret = suit_manifest_get_integrated_payload(&manifest, &uri_0, &payload);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to get integrated payload");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(payload_0.value, payload.value, "Unexpected value of payload contents");
	TEST_ASSERT_EQUAL_MESSAGE(payload_0.len, payload.len, "Unexpected value of payload length");

	ret = suit_manifest_get_integrated_payload(&manifest, &uri_2, &payload);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNAVAILABLE_PAYLOAD, ret, "Out-of-range payload returned");

	ret = suit_manifest_get_integrated_payload(&manifest, &uri_1, &payload);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to get integrated payload");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(payload_1.value, payload.value, "Unexpected value of payload contents");
	TEST_ASSERT_EQUAL_MESSAGE(payload_1.len, payload.len, "Unexpected value of payload length");
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
