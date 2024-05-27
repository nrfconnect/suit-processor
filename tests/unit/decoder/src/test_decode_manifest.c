/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "suit_decoder_test_utils.h"
#include "suit_platform_mock_ext.h"

static uint8_t cbor_envelope_empty_manifest[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_no_common[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x45, /* bytes(5) */
	0xa2, /* map (2 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x00,
};

static uint8_t cbor_envelope_no_manifest_version[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x46, /* bytes(6) */
	0xa2, /* map (2 elements) */
	0x02, /* suit-manifest-sequence-number */ 0x00,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
};

static uint8_t cbor_envelope_no_sequence_number[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x46, /* bytes(6) */
	0xa2, /* map (2 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
};

static uint8_t cbor_envelope_lower_version[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x00,
	0x02, /* suit-manifest-sequence-number */ 0x00,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
};

static uint8_t cbor_envelope_higher_version[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x02,
	0x02, /* suit-manifest-sequence-number */ 0x00,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
};

static uint8_t cbor_envelope_invalid_component_id_no_bstr[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x4a, /* bytes(10) */
	0xa4, /* map (4 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x00,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
	0x05, /* suit-manifest-component-id */
		0x80, /* array (0 elements) */
};

static uint8_t cbor_envelope_invalid_component_id_empty_bstr[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x4b, /* bytes(11) */
	0xa4, /* map (4 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x00,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
	0x05, /* suit-manifest-component-id */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_invalid_component_id_empty_second_bstr[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x4f, /* bytes(15) */
	0xa4, /* map (4 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x00,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
	0x05, /* suit-manifest-component-id */
		0x83, /* array (3 elements) */
			0x41, /* bytes(1) */
			0x0D, /* 12 */
			0x40, /* bytes(0) */
			0x41, /* bytes(1) */
			0x0E, /* 13 */
};

static uint8_t cbor_envelope_single_component_invalid_length[] = {
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
				0x98, 0x01, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
};

static uint8_t cbor_envelope_single_component_invalid_second_chunk_length[] = {
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
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x82, /* array (2 elements) */
					0x41, /* bytes(1) */
					'M',
					0x58, 0x02, /* bytes(2) */
					'M',
					'2',
};

static uint8_t cbor_envelope_component_len_20_invalid[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x22, /* bytes(34) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x58, 0x1a, /* bytes(26) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x58, 0x14, /* bytes(24) */
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
};

static uint8_t cbor_envelope_component_len_64_invalid[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x4f, /* bytes(79) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x58, 0x47, /* bytes(71) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x59, 0x00, 0x40, /* bytes(64) */
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
					'1', '2', '3', '4',
};

static uint8_t cbor_envelope_two_component_ids[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x50, /* bytes(16) */
	0xa5, /* map (5 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x00,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
	0x05, /* suit-manifest-component-id */
		0x81, /* array (1 element) */
			0x41, /* bytes(1) */
			0x0C, /* 12 */
	0x05, /* suit-manifest-component-id */
		0x81, /* array (1 element) */
			0x41, /* bytes(1) */
			0x0D, /* 12 */
};

static uint8_t cbor_envelope_without_components[] = {
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
			0x02, /* suit-components */
				0x80, /* array (0 elements) */
};

static uint8_t cbor_envelope_with_single_null_component[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x4b, /* bytes(11) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x44, /* bytes(4) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x80, /* array (0 elements) */
};

static uint8_t cbor_envelope_with_12_components[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x2f, /* bytes(47) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x58, 0x27, /* bytes(39) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x8c, /* array (12 elements) */
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

static uint8_t cbor_envelope_with_13_components[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x32, /* bytes(50) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x58, 0x2a, /* bytes(42) */
		0xA1, /* map (1 element) */
			0x02, /* suit-components */
				0x8d, /* array (13 elements) */
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
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
};

static uint8_t cbor_envelope_with_single_dependency_with_empty_prefix[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x53, /* bytes(19) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x4c, /* bytes(12) */
		0xA2, /* map (2 elements) */
			0x02, /* suit-components */
				0x81, /* array (1 element) */
				0x81, /* array (1 element) */
					0x41, /* bytes(1) */
					'M',
			0x01, /* suit-dependencies */
				0xA1, /* map (1 element) */
				0x00, 0xA1, /* 0: {1: [ ]} */
					0x01, 0x80,
};

static uint8_t cbor_envelope_with_8_dependencies[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x3b, /* bytes(59) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x58, 0x33, /* bytes(51) */
		0xA2, /* map (2 elements) */
			0x02, /* suit-components */
				0x87, /* array (7 elements) */
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
				0xA8, /* map (8 elements) */
				0x06, 0xA0, /* 4: {} */
				0x04, 0xA0, /* 4: {} */
				0x05, 0xA0, /* 5: {} */
				0x03, 0xA0, /* 3: {} */
				0x01, 0xA0, /* 1: {} */
				0x02, 0xA0, /* 2: {} */
				0x00, 0xA0, /* 0: {} */
				0x00, 0xA1, /* 0: {1: [ bstr("prefix") ]} */
					0x01, 0x81, 0x46,
					'p', 'r', 'e', 'f', 'i', 'x',
};

static uint8_t minimal_with_shared_and_text[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x4c, /* bytes(12) */
	0xa4, /* map (4 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x43, /* bytes(3) */
		0xA1, /* map (1 element) */
			0x04, /* suit-shared-sequence */
			0x40, /* bytes(0) */
	0x17, /* suit-text */
		0x40, /* bytes(0) */
};

static uint8_t minimal_decodable_manifest[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
	0xa3, /* map (3 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x11,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
};

static uint8_t manifest_with_component_id[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(36) */

	0x03, /* suit-manifest */
	0x4c, /* bytes(12) */
	0xa4, /* map (4 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x12,
	0x03, /* suit-common */
		0x41, /* bytes(1) */
		0xA0, /* map (0 elements) */
	0x05, /* suit-manifest-component-id */
		0x81, /* array (1 element) */
			0x41, /* bytes(1) */
			0x0C, /* 12 */
};


void test_decode_manifest_invalid_input(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_envelope(minimal_decodable_manifest, sizeof(minimal_decodable_manifest));
	state.step = MANIFEST_DIGEST_VERIFIED;

	ret = suit_decoder_decode_manifest(NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The manifest decoding did not fail on NULL context");
}

void test_decode_manifest_invalid_state(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_envelope(minimal_decodable_manifest, sizeof(minimal_decodable_manifest));

	for (enum suit_decoder_step step = INVALID; step <= LAST_STEP; step++) {
		if (step == MANIFEST_DIGEST_VERIFIED) {
			continue;
		}

		state.step = step;
		ret = suit_decoder_decode_manifest(&state);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The manifest decoding did not fail in incorrect state");
	}
}

void test_decode_manifest_minimal(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_envelope(minimal_decodable_manifest, sizeof(minimal_decodable_manifest));
	state.step = MANIFEST_DIGEST_VERIFIED;

	ret = suit_decoder_decode_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest decoding failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_DECODED, state.step, "Invalid state transition after manifest decoding");
	TEST_ASSERT_EQUAL_MESSAGE(0, state.decoded_manifest->manifest_component_id.len, "Invalid length of the manifest component ID");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest->manifest_component_id.value, "Invalid value of the manifest component ID");
	TEST_ASSERT_EQUAL_MESSAGE(0x11, state.decoded_manifest->sequence_number, "Incorrect manifest sequence number value");
}

void test_decode_manifest_max_components(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_envelope(cbor_envelope_with_12_components, sizeof(cbor_envelope_with_12_components));
	state.step = MANIFEST_DIGEST_VERIFIED;

	ret = suit_decoder_decode_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest decoding failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_DECODED, state.step, "Invalid state transition after manifest decoding");
	TEST_ASSERT_EQUAL_MESSAGE(0, state.decoded_manifest->manifest_component_id.len, "Invalid length of the manifest component ID");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest->manifest_component_id.value, "Invalid value of the manifest component ID");
	TEST_ASSERT_EQUAL_MESSAGE(0x10, state.decoded_manifest->sequence_number, "Incorrect manifest sequence number value");
}

void test_decode_manifest_invalid_input_bytes(void)
{
	int ret = SUIT_SUCCESS;

	struct input_envelope envelopes[] = {
		{
			.envelope = cbor_envelope_empty_manifest,
			.envelope_size = sizeof(cbor_envelope_empty_manifest),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_NO_PAYLOAD),
		},
		{
			.envelope = cbor_envelope_no_manifest_version,
			.envelope_size = sizeof(cbor_envelope_no_manifest_version),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_WRONG_VALUE),
		},
		{
			.envelope = cbor_envelope_no_sequence_number,
			.envelope_size = sizeof(cbor_envelope_no_sequence_number),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_WRONG_VALUE),
		},
		{
			.envelope = cbor_envelope_no_common,
			.envelope_size = sizeof(cbor_envelope_no_common),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_NO_PAYLOAD),
		},
		{
			.envelope = cbor_envelope_lower_version,
			.envelope_size = sizeof(cbor_envelope_lower_version),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_WRONG_VALUE),
		},
		{
			.envelope = cbor_envelope_higher_version,
			.envelope_size = sizeof(cbor_envelope_higher_version),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_WRONG_VALUE),
		},
		{
			.envelope = cbor_envelope_invalid_component_id_no_bstr,
			.envelope_size = sizeof(cbor_envelope_invalid_component_id_no_bstr),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_HIGH_ELEM_COUNT),
		},
		{
			.envelope = cbor_envelope_invalid_component_id_empty_bstr,
			.envelope_size = sizeof(cbor_envelope_invalid_component_id_empty_bstr),
			.exp_ret = SUIT_ERR_DECODING,
		},
		{
			.envelope = cbor_envelope_invalid_component_id_empty_second_bstr,
			.envelope_size = sizeof(cbor_envelope_invalid_component_id_empty_second_bstr),
			.exp_ret = SUIT_ERR_DECODING,
		},
		{
			.envelope = cbor_envelope_two_component_ids,
			.envelope_size = sizeof(cbor_envelope_two_component_ids),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_HIGH_ELEM_COUNT),
		},
		{
			.envelope = cbor_envelope_without_components,
			.envelope_size = sizeof(cbor_envelope_without_components),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_PAYLOAD_NOT_CONSUMED),
		},
		{
			.envelope = cbor_envelope_with_single_null_component,
			.envelope_size = sizeof(cbor_envelope_with_single_null_component),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_PAYLOAD_NOT_CONSUMED),
		},
		{
			.envelope = cbor_envelope_with_13_components,
			.envelope_size = sizeof(cbor_envelope_with_13_components),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_PAYLOAD_NOT_CONSUMED),
		},
		{
			.envelope = cbor_envelope_with_single_dependency_with_empty_prefix,
			.envelope_size = sizeof(cbor_envelope_with_single_dependency_with_empty_prefix),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_PAYLOAD_NOT_CONSUMED),
		},
		{
			.envelope = cbor_envelope_with_8_dependencies,
			.envelope_size = sizeof(cbor_envelope_with_8_dependencies),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_PAYLOAD_NOT_CONSUMED),
		},
		{
			.envelope = minimal_with_shared_and_text,
			.envelope_size = sizeof(minimal_with_shared_and_text),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_HIGH_ELEM_COUNT),
		},
		{
			.envelope = cbor_envelope_single_component_invalid_length,
			.envelope_size = sizeof(cbor_envelope_single_component_invalid_length),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_PAYLOAD_NOT_CONSUMED),
		},
		{
			.envelope = cbor_envelope_single_component_invalid_second_chunk_length,
			.envelope_size = sizeof(cbor_envelope_single_component_invalid_second_chunk_length),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_PAYLOAD_NOT_CONSUMED),
		},
		{
			.envelope = cbor_envelope_component_len_20_invalid,
			.envelope_size = sizeof(cbor_envelope_component_len_20_invalid),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_PAYLOAD_NOT_CONSUMED),
		},
		{
			.envelope = cbor_envelope_component_len_64_invalid,
			.envelope_size = sizeof(cbor_envelope_component_len_64_invalid),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_PAYLOAD_NOT_CONSUMED),
		},
	};

	for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(envelopes); i++) {
		/* Reset state. */
		memset(&state, 0, sizeof(state));
		memset(&manifest, 0, sizeof(manifest));
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "SUIT decoder has a valid state before the test starts");
		init_decode_envelope(envelopes[i].envelope, envelopes[i].envelope_size);
		state.step = MANIFEST_DIGEST_VERIFIED;

		ret = suit_decoder_decode_manifest(&state);
		TEST_ASSERT_EQUAL_MESSAGE(envelopes[i].exp_ret, ret, "The manifest decoding did not fail");
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest decoding");
		TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest decoding failure");
	}
}

void test_decode_manifest_with_component_id(void)
{
	int ret = SUIT_SUCCESS;
	uint8_t manifest_component_id[] = {
		0x81, 0x41, 0x0C,
	};

	init_decode_envelope(manifest_with_component_id, sizeof(manifest_with_component_id));
	state.step = MANIFEST_DIGEST_VERIFIED;

	ret = suit_decoder_decode_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest decoding failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_DECODED, state.step, "Invalid state transition after manifest decoding");

	TEST_ASSERT_EQUAL_MESSAGE(3, state.decoded_manifest->manifest_component_id.len, "Invalid length of the manifest component ID");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(manifest_component_id, state.decoded_manifest->manifest_component_id.value, sizeof(manifest_component_id), "Invalid value of the manifest component ID");
	TEST_ASSERT_EQUAL_MESSAGE(0x12, state.decoded_manifest->sequence_number, "Incorrect manifest sequence number value");
}
