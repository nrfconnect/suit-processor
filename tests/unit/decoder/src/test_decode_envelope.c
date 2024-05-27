/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "suit_decoder_test_utils.h"

static uint8_t cbor_envelope_tag[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
};

static uint8_t cbor_envelope_empty_map[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa0, /* empty map */
};

static uint8_t cbor_envelope_bare_manifest[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa1, /* map (1 element) */

	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_no_digest[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_no_manifest[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa1, /* map (1 element) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_incomplete_digest_no_array[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_incomplete_digest_empty_array[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */
	0x02, /* suit-authentication-wrapper */
		0x41, /* bytes(1) */
		0x80, /* empty array */

	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_auth_two_bstrs[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */
	0x02, /* suit-authentication-wrapper */
		0x43, /* bytes(3) */
		0x82, /* array (2 elements) */
			0x40, /* bytes(0) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_auth_three_bstrs[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */
	0x02, /* suit-authentication-wrapper */
		0x44, /* bytes(4) */
		0x83, /* array (3 elements) */
			0x40, /* bytes(0) */
			0x40, /* bytes(0) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_auth_four_bstrs[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */
	0x02, /* suit-authentication-wrapper */
		0x45, /* bytes(5) */
		0x84, /* array (4 elements) */
			0x40, /* bytes(0) */
			0x40, /* bytes(0) */
			0x40, /* bytes(0) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_with_auth_wrappers[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa3, /* map (3 elements) */
	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_with_two_manifests[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa3, /* map (3 elements) */
	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_with_two_severable_texts[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa4, /* map (4 elements) */
	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
	0x17, /* suit-text */
	0x41, /* bytes(1) */
		0x0A, /* 10 */
	0x17, /* suit-text */
	0x41, /* bytes(1) */
		0x0A, /* 10 */
};

static uint8_t minimal_decodable_envelope[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */
	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
};

static uint8_t cbor_envelope_with_invalid_manifest_length[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */
	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x58, 0x08, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t minimal_decodable_envelope_ext[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */
	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
	0x00,
};

static uint8_t envelope_with_severable_text[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa3, /* map (3 elements) */
	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
	0x17, /* suit-text */
	0x41, /* bytes(1) */
		0x0A, /* 10 */
};

static uint8_t envelope_with_empty_severable_text[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa3, /* map (3 elements) */
	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
	0x17, /* suit-text */
	0x40, /* bytes(0) */
};

static uint8_t envelope_with_integrated_payload[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa3, /* map (3 elements) */
	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */
	0x03, /* suit-manifest */
	0x40, /* bytes(0) */
	0x64, /* text field (4 bytes) */
		'#', 'a', 'p', 'p',
	0x41, /* bytes(1) */
		0x0B, /* 11 */
};


static size_t envelope_with_integrated_payload_gen(size_t n_payloads, uint8_t **envelope)
{
	/* Update this value when adding or removing payloads. */
	const size_t max_payloads = 7;

	static uint8_t envelope_with_integrated_payloads[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa8, /* map (8 elements) */
		0x02, /* suit-authentication-wrapper */
			0x42, /* bytes(2) */
			0x81, /* array (1 element) */
				0x40, /* bytes(0) */
		0x03, /* suit-manifest */
		0x40, /* bytes(0) */
		0x64, /* text field (4 bytes) */
			'#', 'F', 'W', '0',
		0x41, /* bytes(1) */
			0x00, /* 0 */
		0x64, /* text field (4 bytes) */
			'#', 'F', 'W', '1',
		0x41, /* bytes(1) */
			0x01, /* 1 */
		0x64, /* text field (4 bytes) */
			'#', 'F', 'W', '2',
		0x41, /* bytes(1) */
			0x02, /* 2 */
		0x64, /* text field (4 bytes) */
			'#', 'F', 'W', '3',
		0x41, /* bytes(1) */
			0x03, /* 3 */
		0x64, /* text field (4 bytes) */
			'#', 'F', 'W', '4',
		0x41, /* bytes(1) */
			0x04, /* 4 */
		0x64, /* text field (4 bytes) */
			'#', 'F', 'W', '5',
		0x41, /* bytes(1) */
			0x05, /* 5 */
		0x64, /* text field (4 bytes) */
			'#', 'F', 'W', '6',
		0x41, /* bytes(1) */
			0x05, /* 5 */
	};

	if (n_payloads > max_payloads) {
		*envelope = NULL;
		return 0;
	}

	envelope_with_integrated_payloads[2] = 0xa2 + n_payloads;
	*envelope = envelope_with_integrated_payloads;

	return sizeof(envelope_with_integrated_payloads) - 7 * (max_payloads - n_payloads);
}


void test_decode_envelope_invalid_input(void)
{
	int ret = SUIT_SUCCESS;

	init_static_mem_input();

	ret = suit_decoder_decode_envelope(NULL, minimal_decodable_envelope, sizeof(minimal_decodable_envelope));
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The envelope decoding did not fail on NULL context");

	ret = suit_decoder_decode_envelope(&state, NULL, sizeof(minimal_decodable_envelope));
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The envelope decoding did not fail on NULL envelope");

	ret = suit_decoder_decode_envelope(&state, minimal_decodable_envelope, 0);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The envelope decoding did not fail on empty envelope");
}

void test_decode_envelope_invalid_state(void)
{
	int ret = SUIT_SUCCESS;

	init_static_mem_input();

	for (enum suit_decoder_step step = INVALID; step <= LAST_STEP; step++) {
		if (step == INITIALIZED) {
			continue;
		}

		state.step = step;
		ret = suit_decoder_decode_envelope(&state, minimal_decodable_envelope, sizeof(minimal_decodable_envelope));
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The envelope decoding did not fail in incorrect state");
	}
}

void test_decode_envelope_minimal(void)
{
	int ret = SUIT_SUCCESS;

	init_static_mem_input();

	ret = suit_decoder_decode_envelope(&state, minimal_decodable_envelope, sizeof(minimal_decodable_envelope));
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Decoding of valid minimal envelope failed");

	TEST_ASSERT_EQUAL_MESSAGE(0, state.authentication_bstr_count, "Decoding authentication block count failed");

	TEST_ASSERT_EQUAL_MESSAGE(0, state.manifest_digest_bytes.len, "Decoding the manifest digest length failed");
	TEST_ASSERT_EQUAL_MESSAGE(UNAVAILABLE, state.decoded_manifest->text_status, "Decoding the severable text field failed");

	TEST_ASSERT_EQUAL_PTR_MESSAGE(minimal_decodable_envelope, state.decoded_manifest->envelope_str.value, "Invalid reference to the decoded envelope");
	TEST_ASSERT_EQUAL_MESSAGE(sizeof(minimal_decodable_envelope), state.decoded_manifest->envelope_str.len, "Invalid length of the decoded envelope");

	TEST_ASSERT_EQUAL_MESSAGE(0, state.decoded_manifest->integrated_payloads_count, "Decoding the number of integrated payloads failed");
}

void test_decode_envelope_minimal_incorrect_length(void)
{
	int ret = SUIT_SUCCESS;

	init_static_mem_input();

	ret = suit_decoder_decode_envelope(&state, minimal_decodable_envelope, sizeof(minimal_decodable_envelope));
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Decoding of valid minimal envelope failed");
	TEST_ASSERT_EQUAL_MESSAGE(ENVELOPE_DECODED, state.step, "Invalid state transition after envelope decoding");
	TEST_ASSERT_EQUAL_MESSAGE(sizeof(minimal_decodable_envelope), state.decoded_manifest->envelope_str.len, "Invalid length of the decoded envelope");
	TEST_ASSERT_EQUAL_MESSAGE(0, state.decoded_manifest->integrated_payloads_count, "Decoding the number of integrated payloads failed");

	state.step = INITIALIZED;
	ret = suit_decoder_decode_envelope(&state, minimal_decodable_envelope_ext, sizeof(minimal_decodable_envelope_ext));
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Decoding of valid minimal envelope failed");
	TEST_ASSERT_EQUAL_MESSAGE(ENVELOPE_DECODED, state.step, "Invalid state transition after envelope decoding");
	TEST_ASSERT_EQUAL_MESSAGE(sizeof(minimal_decodable_envelope), state.decoded_manifest->envelope_str.len, "Invalid length of the decoded envelope");
	TEST_ASSERT_EQUAL_MESSAGE(0, state.decoded_manifest->integrated_payloads_count, "Decoding the number of integrated payloads failed");

	state.step = INITIALIZED;
	ret = suit_decoder_decode_envelope(&state, minimal_decodable_envelope, sizeof(minimal_decodable_envelope) - 1);
	TEST_ASSERT_EQUAL_MESSAGE(ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_NO_PAYLOAD), ret, "Decoding of valid too short envelope did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed envelope decoding");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after envelope decoding failure");
}

void test_decode_envelope_invalid_input_bytes(void)
{
	int ret = SUIT_SUCCESS;
	uint8_t *cbor_envelope_with_too_many_payloads;
	size_t too_many_payloads_envelope_len = envelope_with_integrated_payload_gen(SUIT_MAX_NUM_INTEGRATED_PAYLOADS + 1, &cbor_envelope_with_too_many_payloads);

	struct input_envelope envelopes[] = {
		{
			.envelope = cbor_envelope_tag,
			.envelope_size = sizeof(cbor_envelope_tag),
			.exp_ret = ZCBOR_ERR_NO_PAYLOAD,
		},
		{
			.envelope = cbor_envelope_empty_map,
			.envelope_size = sizeof(cbor_envelope_empty_map),
			.exp_ret = ZCBOR_ERR_NO_PAYLOAD,
		},
		{
			.envelope = cbor_envelope_bare_manifest,
			.envelope_size = sizeof(cbor_envelope_bare_manifest),
			.exp_ret = ZCBOR_ERR_WRONG_VALUE,
		},
		{
			.envelope = cbor_envelope_no_digest,
			.envelope_size = sizeof(cbor_envelope_no_digest),
			.exp_ret = ZCBOR_ERR_NO_PAYLOAD,
		},
		{
			.envelope = cbor_envelope_incomplete_digest_no_array,
			.envelope_size = sizeof(cbor_envelope_incomplete_digest_no_array),
			.exp_ret = ZCBOR_ERR_NO_PAYLOAD,
		},
		{
			.envelope = cbor_envelope_incomplete_digest_empty_array,
			.envelope_size = sizeof(cbor_envelope_incomplete_digest_empty_array),
			.exp_ret = ZCBOR_ERR_NO_PAYLOAD,
		},
		{
			.envelope = cbor_envelope_no_manifest,
			.envelope_size = sizeof(cbor_envelope_no_manifest),
			.exp_ret = ZCBOR_ERR_NO_PAYLOAD,
		},
		{
			.envelope = cbor_envelope_with_auth_wrappers,
			.envelope_size = sizeof(cbor_envelope_with_auth_wrappers),
			.exp_ret = ZCBOR_ERR_WRONG_VALUE,
		},
		{
			.envelope = cbor_envelope_with_two_manifests,
			.envelope_size = sizeof(cbor_envelope_with_two_manifests),
			.exp_ret = ZCBOR_ERR_HIGH_ELEM_COUNT,
		},
		{
			.envelope = cbor_envelope_with_two_severable_texts,
			.envelope_size = sizeof(cbor_envelope_with_two_severable_texts),
			.exp_ret = ZCBOR_ERR_HIGH_ELEM_COUNT,
		},
		{
			.envelope = cbor_envelope_with_too_many_payloads,
			.envelope_size = too_many_payloads_envelope_len,
			.exp_ret = ZCBOR_ERR_HIGH_ELEM_COUNT,
		},
		{
			.envelope = cbor_envelope_with_invalid_manifest_length,
			.envelope_size = sizeof(cbor_envelope_with_invalid_manifest_length),
			.exp_ret = ZCBOR_ERR_INVALID_VALUE_ENCODING,
		}
	};

	for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(envelopes); i++) {
		/* Reset state. */
		memset(&state, 0, sizeof(state));
		memset(&manifest, 0, sizeof(manifest));
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "SUIT decoder has a valid state before the test starts");
		init_static_mem_input();

		ret = suit_decoder_decode_envelope(&state, envelopes[i].envelope, envelopes[i].envelope_size);
		TEST_ASSERT_EQUAL_MESSAGE(ZCBOR_ERR_TO_SUIT_ERR(envelopes[i].exp_ret), ret, "Decoding of invalid envelope must fail");
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed envelope decoding");
		TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after envelope decoding failure");
	}
}
void test_deocde_envelope_several_auth_bstrs(void)
{
	int ret = SUIT_SUCCESS;

	struct input_envelope envelopes[] = {
		{
			.envelope = cbor_envelope_auth_two_bstrs,
			.envelope_size = sizeof(cbor_envelope_auth_two_bstrs),
			.exp_ret = SUIT_SUCCESS,
		},
		{
			.envelope = cbor_envelope_auth_three_bstrs,
			.envelope_size = sizeof(cbor_envelope_auth_three_bstrs),
			.exp_ret = SUIT_SUCCESS,
		},
		{
			.envelope = cbor_envelope_auth_four_bstrs,
			.envelope_size = sizeof(cbor_envelope_auth_four_bstrs),
			.exp_ret = ZCBOR_ERR_TO_SUIT_ERR(ZCBOR_ERR_PAYLOAD_NOT_CONSUMED),
		},
	};

	for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(envelopes); i++) {
		/* Reset state. */
		memset(&state, 0, sizeof(state));
		memset(&manifest, 0, sizeof(manifest));
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "SUIT decoder has a valid state before the test starts");

		/* Use the other test as an additional setUp step. */
		init_static_mem_input();

		/* Pass invalid envelope. Use the bigger size as than input payload as it does not affect decoding. */
		ret = suit_decoder_decode_envelope(&state, envelopes[i].envelope, envelopes[i].envelope_size);
		TEST_ASSERT_EQUAL_MESSAGE(envelopes[i].exp_ret, ret, "Unexpected return code");

		if (ret == SUIT_SUCCESS) {
			/* Compare with the byte after the type & size tag. */
			TEST_ASSERT_EQUAL_PTR_MESSAGE(&envelopes[i].envelope[7], state.manifest_digest_bytes.value, "Invalid manifest digest value");
			TEST_ASSERT_EQUAL_MESSAGE(ENVELOPE_DECODED, state.step, "Invalid state transition after envelope decoding");
		} else {
			TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed envelope decoding");
			TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after envelope decoding failure");
		}
	}
}

void test_deocde_envelope_severed_text_field(void)
{
	int ret = SUIT_SUCCESS;

	/* Use the other test as an additional setUp step. */
	init_static_mem_input();

	/* Pass invalid envelope. Use the bigger size as than input payload as it does not affect decoding. */
	ret = suit_decoder_decode_envelope(&state, envelope_with_severable_text, sizeof(envelope_with_severable_text));
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Parsing envelope with severable text failed");
	TEST_ASSERT_EQUAL_MESSAGE(ENVELOPE_DECODED, state.step, "Invalid state transition after envelope decoding");

	TEST_ASSERT_EQUAL_MESSAGE(0, state.authentication_bstr_count, "Decoding authentication block count failed");
	TEST_ASSERT_EQUAL_MESSAGE(0, state.manifest_digest_bytes.len, "Decoding the manifest digest length failed");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(envelope_with_severable_text, state.decoded_manifest->envelope_str.value, "Invalid reference to the decoded envelope");
	TEST_ASSERT_EQUAL_MESSAGE(sizeof(envelope_with_severable_text), state.decoded_manifest->envelope_str.len, "Invalid length of the decoded envelope");
	TEST_ASSERT_EQUAL_MESSAGE(0, state.decoded_manifest->integrated_payloads_count, "Decoding the number of integrated payloads failed");

	TEST_ASSERT_EQUAL_MESSAGE(SEVERED, state.decoded_manifest->text_status, "Decoding the severable text field failed");
	TEST_ASSERT_EQUAL_MESSAGE(1, state.decoded_manifest->text.len, "Decoding the severable text field payload length failed");
	TEST_ASSERT_EQUAL_MESSAGE(0x0A, state.decoded_manifest->text.value[0], "Decoding the severable text field payload value failed");
}

void test_deocde_envelope_empty_text_field(void)
{
	int ret = SUIT_SUCCESS;

	/* Use the other test as an additional setUp step. */
	init_static_mem_input();

	/* Pass invalid envelope. Use the bigger size as than input payload as it does not affect decoding. */
	ret = suit_decoder_decode_envelope(&state, envelope_with_empty_severable_text, sizeof(envelope_with_empty_severable_text));
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "Parsing envelope with severable text failed");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed envelope decoding");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after envelope decoding failure");
}

void test_deocde_envelope_integrated_payload(void)
{
	int ret = SUIT_SUCCESS;
	char key[] = "#app";

	/* Use the other test as an additional setUp step. */
	init_static_mem_input();

	ret = suit_decoder_decode_envelope(&state, envelope_with_integrated_payload, sizeof(envelope_with_integrated_payload));
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Parsing envelope with integrated payload failed");
	TEST_ASSERT_EQUAL_MESSAGE(ENVELOPE_DECODED, state.step, "Invalid state transition after envelope decoding");

	TEST_ASSERT_EQUAL_MESSAGE(0, state.authentication_bstr_count, "Decoding authentication block count failed");
	TEST_ASSERT_EQUAL_MESSAGE(0, state.manifest_digest_bytes.len, "Decoding the manifest digest length failed");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(envelope_with_integrated_payload, state.decoded_manifest->envelope_str.value, "Invalid reference to the decoded envelope");
	TEST_ASSERT_EQUAL_MESSAGE(sizeof(envelope_with_integrated_payload), state.decoded_manifest->envelope_str.len, "Invalid length of the decoded envelope");
	TEST_ASSERT_EQUAL_MESSAGE(UNAVAILABLE, state.decoded_manifest->text_status, "Decoding the severable text field failed");

	TEST_ASSERT_EQUAL_MESSAGE(1, state.decoded_manifest->integrated_payloads_count, "Decoding the number of integrated payloads failed");
	TEST_ASSERT_EQUAL_MESSAGE(4, state.decoded_manifest->integrated_payloads[0].key.len, "Decoding the integrated payload key length failed");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(key, state.decoded_manifest->integrated_payloads[0].key.value, state.decoded_manifest->integrated_payloads[0].key.len, "Decoding the integrated payload key value failed");
	TEST_ASSERT_EQUAL_MESSAGE(1, state.decoded_manifest->integrated_payloads[0].payload.len, "Decoding the integrated payload length failed");
	TEST_ASSERT_EQUAL_MESSAGE(0x0B, state.decoded_manifest->integrated_payloads[0].payload.value[0], "Decoding the integrated payload value failed");
}

void test_deocde_envelope_integrated_payloads(void)
{
	int ret = SUIT_SUCCESS;
	uint8_t *envelope;
	size_t envelope_size;
	char key[] = "#FW0";

	for (size_t i = 1; i < SUIT_MAX_NUM_INTEGRATED_PAYLOADS; i++) {
		/* Reset state. */
		memset(&state, 0, sizeof(state));
		memset(&manifest, 0, sizeof(manifest));
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "SUIT decoder has a valid state before the test starts");

		/* Use the other test as an additional setUp step. */
		init_static_mem_input();

		envelope_size = envelope_with_integrated_payload_gen(i, &envelope);
		ret = suit_decoder_decode_envelope(&state, envelope, envelope_size);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Parsing envelope with integrated payload failed");
		TEST_ASSERT_EQUAL_MESSAGE(ENVELOPE_DECODED, state.step, "Invalid state transition after envelope decoding");

		TEST_ASSERT_EQUAL_MESSAGE(0, state.authentication_bstr_count, "Decoding authentication block count failed");
		TEST_ASSERT_EQUAL_MESSAGE(0, state.manifest_digest_bytes.len, "Decoding the manifest digest length failed");
		TEST_ASSERT_EQUAL_PTR_MESSAGE(envelope, state.decoded_manifest->envelope_str.value, "Invalid reference to the decoded envelope");
		TEST_ASSERT_EQUAL_MESSAGE(envelope_size, state.decoded_manifest->envelope_str.len, "Invalid length of the decoded envelope");
		TEST_ASSERT_EQUAL_MESSAGE(UNAVAILABLE, state.decoded_manifest->text_status, "Decoding the severable text field failed");

		TEST_ASSERT_EQUAL_MESSAGE(i, state.decoded_manifest->integrated_payloads_count, "Decoding the number of integrated payloads failed");
		for (size_t p_i = 0; p_i < i; p_i++) {
			key[3] = '0' + p_i;
			TEST_ASSERT_EQUAL_MESSAGE(4, state.decoded_manifest->integrated_payloads[p_i].key.len, "Decoding the integrated payload key length failed");
			TEST_ASSERT_EQUAL_MEMORY_MESSAGE(key, state.decoded_manifest->integrated_payloads[p_i].key.value, state.decoded_manifest->integrated_payloads[0].key.len, "Decoding the integrated payload key value failed");
			TEST_ASSERT_EQUAL_MESSAGE(1, state.decoded_manifest->integrated_payloads[p_i].payload.len, "Decoding the integrated payload length failed");
			TEST_ASSERT_EQUAL_MESSAGE(p_i, state.decoded_manifest->integrated_payloads[p_i].payload.value[0], "Decoding the integrated payload value failed");
		}
	}
}
