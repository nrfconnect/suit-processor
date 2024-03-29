/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "suit_decoder_test_utils.h"
#include "suit_platform_mock_ext.h"


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

static uint8_t cbor_envelope_with_single_component_with_null_bstr[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x4e, /* bytes(14) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x47, /* bytes(7) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x82, /* array (2 elements) */
					0x41, /* bytes(1) */
					'M',
					0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_with_4_components[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x56, /* bytes(22) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x4f, /* bytes(15) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x84, /* array (4 elements) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
};

static uint8_t cbor_envelope_with_single_dependency_component[] = {
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

static uint8_t cbor_envelope_with_single_dependency_with_empty_bstr_as_prefix[] = {
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

static uint8_t cbor_envelope_with_single_dependency_with_prefix[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x1a, /* bytes(26) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x53, /* bytes(19) */
		0xA2, /* map (2 elements) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
			0x01, /* suit-dependencies */
				0xA1, /* map (1 element) */
				0x00, 0xA1, /* 0: {1: [ bstr("prefix") ]} */
					0x01, 0x81, 0x46,
					'p', 'r', 'e', 'f', 'i', 'x',
};

static uint8_t cbor_envelope_with_invalid_dependency[] = {
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


static uint8_t cbor_envelope_with_4_dependencies[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x21, /* bytes(33) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x58, 0x19, /* bytes(25) */
		0xA2, /* map (2 elements) */
			0x02, /* suit-components */
				0x84, /* array (4 elements) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
			0x01, /* suit-dependencies */
				0xA4, /* map (4 elements) */
				0x03, 0xA0, /* 3: {} */
				0x01, 0xA0, /* 1: {} */
				0x02, 0xA0, /* 2: {} */
				0x00, 0xA0, /* 0: {} */
};

static uint8_t minimal_decodable_manifest_with_dependency[] = {
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
			0x01, /* suit-dependencies */
				0xA1, /* map (1 element) */
				0x00, 0xA0, /* 0: {} */
};

static uint8_t component_id[] = {
	0x81, 0x41, 'M',
};

static struct zcbor_string exp_component_id = {
	.value = component_id,
	.len = sizeof(component_id),
};


void test_authorize_manifest_invalid_input(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_with_single_component, sizeof(minimal_with_single_component));
	state.step = MANIFEST_DECODED;

	ret = suit_decoder_authorize_manifest(NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The manifest authentication did not fail on NULL context");
}

void test_authorize_manifest_invalid_state(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_with_single_component, sizeof(minimal_with_single_component));

	for (enum suit_decoder_step step = INVALID; step <= LAST_STEP; step++) {
		if (step == MANIFEST_AUTHENTICATED) {
			continue;
		}

		state.step = step;
		ret = suit_decoder_authorize_manifest(&state);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The manifest authentication check did not fail in incorrect state");
	}
}

void test_authorize_manifest_minimal(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_decodable_manifest, sizeof(minimal_decodable_manifest));
	state.step = MANIFEST_AUTHENTICATED;

	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "The manifest without components authorization did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authorization");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authorization failure");
}

void test_authorize_manifest_minimal_without_common(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_decodable_manifest, sizeof(minimal_decodable_manifest));
	state.step = MANIFEST_AUTHENTICATED;
	state.manifest.SUIT_Manifest_suit_common_cbor.SUIT_Common_suit_components_present = false;

	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "The manifest without components authorization did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authorization");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authorization failure");
}

void test_authorize_manifest_single_component(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_with_single_component, sizeof(minimal_with_single_component));
	state.step = MANIFEST_AUTHENTICATED;

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, &exp_component_id, SUIT_SUCCESS);

	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest with single component authorization failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_AUTHORIZED, state.step, "Invalid state transition after manifest authorization");
	TEST_ASSERT_EQUAL_MESSAGE(0x10, state.decoded_manifest->sequence_number, "Incorrect manifest sequence number value");
}

void test_authorize_manifest_single_component_platform_fail(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_with_single_component, sizeof(minimal_with_single_component));
	state.step = MANIFEST_AUTHENTICATED;

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, &exp_component_id, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);

	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "The manifest without components authorization did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authorization");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authorization failure");
}


void test_authorize_manifest_with_null_bstr_component(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(cbor_envelope_with_single_component_with_null_bstr, sizeof(cbor_envelope_with_single_component_with_null_bstr));
	state.step = MANIFEST_AUTHENTICATED;

	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The manifest without components authorization did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authorization");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authorization failure");
}

void test_authorize_manifest_with_4_components(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(cbor_envelope_with_4_components, sizeof(cbor_envelope_with_4_components));
	state.step = MANIFEST_AUTHENTICATED;

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, &exp_component_id, SUIT_ERR_UNSUPPORTED_COMPONENT_ID);

	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "The manifest without components authorization did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authorization");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authorization failure");
}

void test_authorize_manifest_with_single_dependency_component(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(cbor_envelope_with_single_dependency_component, sizeof(cbor_envelope_with_single_dependency_component));
	state.step = MANIFEST_AUTHENTICATED;

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, &exp_component_id, SUIT_SUCCESS);

	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest with single dependency component authorization failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_AUTHORIZED, state.step, "Invalid state transition after manifest authorization");
	TEST_ASSERT_EQUAL_MESSAGE(0x10, state.decoded_manifest->sequence_number, "Incorrect manifest sequence number value");
}

void test_authorize_manifest_with_dependency_with_empty_bstr_as_prefix(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(cbor_envelope_with_single_dependency_with_empty_bstr_as_prefix, sizeof(cbor_envelope_with_single_dependency_with_empty_bstr_as_prefix));
	state.step = MANIFEST_AUTHENTICATED;

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, &exp_component_id, SUIT_SUCCESS);

	/* Will fail on component creation. */
	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest with single dependency component authorization failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_AUTHORIZED, state.step, "Invalid state transition after manifest authorization");
	TEST_ASSERT_EQUAL_MESSAGE(0x10, state.decoded_manifest->sequence_number, "Incorrect manifest sequence number value");
}

void test_authorize_manifest_with_dependency_with_prefix(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(cbor_envelope_with_single_dependency_with_prefix, sizeof(cbor_envelope_with_single_dependency_with_prefix));
	state.step = MANIFEST_AUTHENTICATED;

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, &exp_component_id, SUIT_SUCCESS);

	/* Will fail on component creation. */
	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest with single dependency component authorization failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_AUTHORIZED, state.step, "Invalid state transition after manifest authorization");
	TEST_ASSERT_EQUAL_MESSAGE(0x10, state.decoded_manifest->sequence_number, "Incorrect manifest sequence number value");
}

void test_authorize_manifest_with_invalid_dependency(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(cbor_envelope_with_invalid_dependency, sizeof(cbor_envelope_with_invalid_dependency));
	state.step = MANIFEST_AUTHENTICATED;

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, &exp_component_id, SUIT_SUCCESS);

	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "The manifest with invalid dependency component authorization did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authorization");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authorization failure");
}

void test_authorize_manifest_with_4_dependencies(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(cbor_envelope_with_4_dependencies, sizeof(cbor_envelope_with_4_dependencies));
	state.step = MANIFEST_AUTHENTICATED;

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, &exp_component_id, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, &exp_component_id, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, &exp_component_id, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, &exp_component_id, SUIT_SUCCESS);

	/* Will fail on component creation. */
	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest with single dependency component authorization failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_AUTHORIZED, state.step, "Invalid state transition after manifest authorization");
	TEST_ASSERT_EQUAL_MESSAGE(0x10, state.decoded_manifest->sequence_number, "Incorrect manifest sequence number value");
}

void test_authorize_manifest_minimal_with_dependency(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_decodable_manifest_with_dependency, sizeof(minimal_decodable_manifest_with_dependency));
	state.step = MANIFEST_AUTHENTICATED;

	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "The manifest without components authorization did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authorization");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authorization failure");
}
