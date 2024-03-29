/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "suit_decoder_test_utils.h"
#include "suit_manifest_mock_ext.h"


static uint8_t minimal_decodable_manifest[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
};

static uint8_t minimal_with_shared[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x4a, /* bytes(10) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x43, /* bytes(3) */
		0xA1, /* map (1 element) */
			0x04, /* suit-shared-sequence */
			0x40, /* bytes(0) */
};

static uint8_t minimal_with_single_dependency_component[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x51, /* bytes(17) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x4a, /* bytes(10) */
		0xA2, /* map (2 elements) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
			0x01, /* suit-dependencies */
				0xA1, /* map (1 element) */
				0x00, 0xA0, /* 0: {} */
};

static uint8_t minimal_with_single_component[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x4d, /* bytes(13) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x46, /* bytes(6) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
};

static uint8_t minimal_with_single_empty_component[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x4c, /* bytes(12) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x45, /* bytes(5) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x40, /* bytes(0) */
};

static uint8_t minimal_with_duplicated_component[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x50, /* bytes(16) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x49, /* bytes(9) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x82, /* array (2 elements) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
};

static uint8_t minimal_with_single_invalid_dependency[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x51, /* bytes(17) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x4a, /* bytes(10) */
		0xA2, /* map (2 elements) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
			0x01, /* suit-dependencies */
				0xA1, /* map (1 element) */
				0x01, 0xA0, /* 1: {} */
};

static uint8_t minimal_with_semidependency_component[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x54, /* bytes(20) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x4d, /* bytes(13) */
		0xA2, /* map (2 elements) */
			0x02, /* suit-components */
				0x82, /* array (2 elements) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
			0x01, /* suit-dependencies */
				0xA1, /* map (1 element) */
				0x00, 0xA0, /* 0: {} */
};

static uint8_t minimal_with_two_dependency_prefixes[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x1b, /* bytes(27) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x54, /* bytes(20) */
		0xA2, /* map (2 elements) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
			0x01, /* suit-dependencies */
				0xA2, /* map (2 elements) */
				0x00, 0xA1, /* 0: {1: [ bstr("1") ]} */
					0x01, 0x81, 0x41, '1',
				0x00, 0xA1, /* 0: {1: [ bstr("2") ]} */
					0x01, 0x81, 0x41, '2',
};

static uint8_t minimal_with_dependency_with_empty_bstr_as_prefix[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x54, /* bytes(20) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x4d, /* bytes(13) */
		0xA2, /* map (2 elements) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
			0x01, /* suit-dependencies */
				0xA1, /* map (1 element) */
				0x00, 0xA1, /* 0: {1: [ bstr() ]} */
					0x01, 0x81, 0x40,
};

static uint8_t minimal_with_component_len_23[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x24, /* bytes(36) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x58, 0x1c, /* bytes(28) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x57, /* bytes(23) */
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3',
};

static uint8_t minimal_with_component_len_24[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x26, /* bytes(38) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x58, 0x1e, /* bytes(30) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x58, 0x18, /* bytes(24) */
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4',
};

static uint8_t minimal_with_component_len_255[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x59, 0x01, 0x0e, /* bytes(38) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x59, 0x01, 0x05, /* bytes(30) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x58, 0xff, /* bytes(255) */
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '1',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '2',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '3',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '4',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '5',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '6',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '7',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '8',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '9',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '1',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '2',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '3',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '4',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '5',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '6',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '7',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '8',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '9',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '1',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '2',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '3',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '4',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '5',
					'1', '2', '3', '4', '5',
};

static uint8_t minimal_with_component_len_256[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x59, 0x01, 0x10, /* bytes(38) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x59, 0x01, 0x07, /* bytes(30) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x59, 0x01, 0x00, /* bytes(256) */
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '1',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '2',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '3',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '4',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '5',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '6',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '7',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '8',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '9',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '1',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '2',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '3',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '4',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '5',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '6',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '7',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '8',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '9',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '1',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '2',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '3',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '4',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '5',
					'1', '2', '3', '4', '5', '6',
};

void test_create_components_invalid_input(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_decodable_manifest, sizeof(minimal_decodable_manifest));

	ret = suit_decoder_create_components(NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The component creation did not fail on NULL context");
}

void test_create_components_invalid_state(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_decodable_manifest, sizeof(minimal_decodable_manifest));

	for (enum suit_decoder_step step = INVALID; step <= LAST_STEP; step++) {
		if (step == SEQUENCES_DECODED) {
			continue;
		}

		state.step = step;
		ret = suit_decoder_create_components(&state);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The component creation did not fail in incorrect state");
	}
}

void test_create_components_no_common_seq(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_decodable_manifest, sizeof(minimal_decodable_manifest));
	state.step = SEQUENCES_DECODED;

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The component creation failed");
	TEST_ASSERT_EQUAL_MESSAGE(COMPONENTS_CREATED, state.step, "Invalid state transition after component creation");
}

void test_create_components_no_components(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_with_shared, sizeof(minimal_with_shared));
	state.step = SEQUENCES_DECODED;

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The component creation failed");
	TEST_ASSERT_EQUAL_MESSAGE(COMPONENTS_CREATED, state.step, "Invalid state transition after component creation");
}

void test_create_components_manifest_append_dependency_failed(void)
{
	int ret = SUIT_SUCCESS;

	struct zcbor_string component_id = {
		.value = "\x81\x41M",
		.len = 3,
	};

	struct zcbor_string prefix = {
		.value = NULL,
		.len = 0,
	};

	init_decode_manifest(minimal_with_single_dependency_component, sizeof(minimal_with_single_dependency_component));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_append_dependency_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, &prefix, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
	__cmock_suit_manifest_release_ExpectAndReturn(state.decoded_manifest, SUIT_SUCCESS);

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "The dependency component creation did not fail on platform component creation failure");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed dependency component creation");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after dependency component creation failure");
}

void test_create_components_manifest_append_component_failed(void)
{
	int ret = SUIT_SUCCESS;

	struct zcbor_string component_id = {
		.value = "\x81\x41M",
		.len = 3,
	};

	init_decode_manifest(minimal_with_single_component, sizeof(minimal_with_single_component));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_append_component_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
	__cmock_suit_manifest_release_ExpectAndReturn(state.decoded_manifest, SUIT_SUCCESS);

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "The component creation did not fail on platform component creation failure");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed component creation");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after component creation failure");
}

void test_create_components_single_component(void)
{
	int ret = SUIT_SUCCESS;

	struct zcbor_string component_id = {
		.value = "\x81\x41M",
		.len = 3,
	};

	init_decode_manifest(minimal_with_single_component, sizeof(minimal_with_single_component));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_append_component_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, SUIT_SUCCESS);

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The component creation failed");
	TEST_ASSERT_EQUAL_MESSAGE(COMPONENTS_CREATED, state.step, "Invalid state transition after component creation");
}

void test_create_components_empty_component_id(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_with_single_empty_component, sizeof(minimal_with_single_empty_component));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_release_ExpectAndReturn(state.decoded_manifest, SUIT_SUCCESS);

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The component creation did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed component creation");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after component creation failure");
}

void test_create_components_duplicated_component(void)
{
	int ret = SUIT_SUCCESS;

	struct zcbor_string component_id = {
		.value = "\x81\x41M",
		.len = 3,
	};

	init_decode_manifest(minimal_with_duplicated_component, sizeof(minimal_with_duplicated_component));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_append_component_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, SUIT_SUCCESS);
	__cmock_suit_manifest_append_component_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, SUIT_SUCCESS);

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The component creation failed");
	TEST_ASSERT_EQUAL_MESSAGE(COMPONENTS_CREATED, state.step, "Invalid state transition after component creation");
}

void test_create_components_invalid_dependency_prefix(void)
{
	int ret = SUIT_SUCCESS;

	struct zcbor_string component_id = {
		.value = "\x81\x41M",
		.len = 3,
	};

	init_decode_manifest(minimal_with_single_invalid_dependency, sizeof(minimal_with_single_invalid_dependency));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_append_component_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, SUIT_SUCCESS);

	/* Components will be created.
	 * This state (index out of range) is invalidated by the suit_decoder_authorize_manifest(..).
	 */
	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The component creation failed");
	TEST_ASSERT_EQUAL_MESSAGE(COMPONENTS_CREATED, state.step, "Invalid state transition after component creation");
}

void test_create_components_duplicated_semidependency(void)
{
	int ret = SUIT_SUCCESS;

	struct zcbor_string component_id = {
		.value = "\x81\x41M",
		.len = 3,
	};

	struct zcbor_string prefix = {
		.value = NULL,
		.len = 0,
	};

	init_decode_manifest(minimal_with_semidependency_component, sizeof(minimal_with_semidependency_component));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_append_dependency_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, &prefix, SUIT_SUCCESS);
	__cmock_suit_manifest_append_component_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, SUIT_SUCCESS);

	/* Components will be created. This state is invalidated by the suit_decoder_authorize_manifest(..). */
	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The component creation failed");
	TEST_ASSERT_EQUAL_MESSAGE(COMPONENTS_CREATED, state.step, "Invalid state transition after component creation");
}

void test_create_components_duplicated_dependency_with_two_prefixes(void)
{
	int ret = SUIT_SUCCESS;

	struct zcbor_string component_id = {
		.value = "\x81\x41M",
		.len = 3,
	};

	struct zcbor_string prefix_0 = {
		.value = "\x81\x41\x31",
		.len = 3,
	};

	struct zcbor_string prefix_1 = {
		.value = "\x81\x41\x32",
		.len = 3,
	};

	init_decode_manifest(minimal_with_two_dependency_prefixes, sizeof(minimal_with_two_dependency_prefixes));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_append_dependency_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, &prefix_0, SUIT_SUCCESS);
	__cmock_suit_manifest_append_dependency_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, &prefix_1, SUIT_SUCCESS);

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The component creation failed");
	TEST_ASSERT_EQUAL_MESSAGE(COMPONENTS_CREATED, state.step, "Invalid state transition after component creation");
}

void test_create_components_dependency_with_empty_bstr_as_prefix(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_with_dependency_with_empty_bstr_as_prefix, sizeof(minimal_with_dependency_with_empty_bstr_as_prefix));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_release_ExpectAndReturn(state.decoded_manifest, SUIT_SUCCESS);

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The component creation did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed component creation");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after component creation failure");
}

void test_create_components_component_len_23(void)
{
	int ret = SUIT_SUCCESS;

	struct zcbor_string component_id = {
		.value = "\x81W12345678901234567890123",
		.len = 25,
	};

	init_decode_manifest(minimal_with_component_len_23, sizeof(minimal_with_component_len_23));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_append_component_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, SUIT_SUCCESS);

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The component creation failed");
	TEST_ASSERT_EQUAL_MESSAGE(COMPONENTS_CREATED, state.step, "Invalid state transition after component creation");
}

void test_create_components_component_len_24(void)
{
	int ret = SUIT_SUCCESS;

	struct zcbor_string component_id = {
		.value = "\x81\x58\x18""123456789012345678901234",
		.len = 27,
	};

	init_decode_manifest(minimal_with_component_len_24, sizeof(minimal_with_component_len_24));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_append_component_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, SUIT_SUCCESS);

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The component creation failed");
	TEST_ASSERT_EQUAL_MESSAGE(COMPONENTS_CREATED, state.step, "Invalid state transition after component creation");
}

void test_create_components_component_len_255(void)
{
	int ret = SUIT_SUCCESS;

	struct zcbor_string component_id = {
		.value = "\x81\x58\xff"
			"12345678911234567892"
			"12345678931234567894"
			"12345678951234567896"
			"12345678971234567898"
			"12345678991234567890"
			"12345678911234567892"
			"12345678931234567894"
			"12345678951234567896"
			"12345678971234567898"
			"12345678991234567890"
			"12345678911234567892"
			"12345678931234567894"
			"123456789512345",
		.len = 258,
	};

	init_decode_manifest(minimal_with_component_len_255, sizeof(minimal_with_component_len_255));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_append_component_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, SUIT_SUCCESS);

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The component creation failed");
	TEST_ASSERT_EQUAL_MESSAGE(COMPONENTS_CREATED, state.step, "Invalid state transition after component creation");
}

void test_create_components_component_len_256(void)
{
	int ret = SUIT_SUCCESS;

	struct zcbor_string component_id = {
		.value = "\x81\x59\x01\x00"
			"12345678911234567892"
			"12345678931234567894"
			"12345678951234567896"
			"12345678971234567898"
			"12345678991234567890"
			"12345678911234567892"
			"12345678931234567894"
			"12345678951234567896"
			"12345678971234567898"
			"12345678991234567890"
			"12345678911234567892"
			"12345678931234567894"
			"1234567895123456",
		.len = 260,
	};

	init_decode_manifest(minimal_with_component_len_256, sizeof(minimal_with_component_len_256));
	state.step = SEQUENCES_DECODED;

	__cmock_suit_manifest_append_component_ExpectComplexArgsAndReturn(state.decoded_manifest, &component_id, SUIT_SUCCESS);

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The component creation failed");
	TEST_ASSERT_EQUAL_MESSAGE(COMPONENTS_CREATED, state.step, "Invalid state transition after component creation");
}
