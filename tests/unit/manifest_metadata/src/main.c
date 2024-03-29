/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include <stdint.h>
#include <suit.h>
#include <suit_processor.h>
#include "suit_decoder/cmock_suit_decoder.h"

static struct suit_processor_state state;

static const uint8_t valid_digest_bstr_cbor[] = {
	0x82, /* array (2 elements) */
	0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
	0x58, 0x20, /* suit-digest-bytes: bytes(32) */
	0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
	0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
	0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
	0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
};

static const struct zcbor_string exp_manifest_digest = {
	.value = &valid_digest_bstr_cbor[4],
	.len = sizeof(valid_digest_bstr_cbor) - 4,
};

static const uint8_t valid_digest_bstr_cbor_sha512[] = {
	0x82, /* array (2 elements) */
	0x38, 0x2B, /* suit-digest-algorithm-id: cose-alg-sha-512 */
	0x58, 0x40, /* suit-digest-bytes: bytes(64) */
	0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
	0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
	0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
	0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
	0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
	0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
	0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
	0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
};

static const struct zcbor_string exp_manifest_digest_sha512 = {
	.value = &valid_digest_bstr_cbor_sha512[5],
	.len = sizeof(valid_digest_bstr_cbor_sha512) - 5,
};

static const uint8_t manifest_component_id_cbor[] = {
	0x81, /* array (1 element) */
		0x41, /* bytes(1) */
		0x0D, /* 12 */
};

static const struct zcbor_string exp_manifest_component_id = {
	.value = manifest_component_id_cbor,
	.len = sizeof(manifest_component_id_cbor),
};

static const uint32_t exp_seq_num = 1234;


static int mock_manifest_invalid_digest_bstr(struct suit_decoder_state* decoder_state, int cmock_num_calls)
{
	static uint8_t invalid_digest_bstr_cbor[] = {
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
		0xff, 0xff,
	};

	decoder_state->manifest_digest_bytes.value = invalid_digest_bstr_cbor;
	decoder_state->manifest_digest_bytes.len = sizeof(invalid_digest_bstr_cbor);

	return SUIT_SUCCESS;
}

static int mock_manifest_invalid_digest_length(struct suit_decoder_state* decoder_state, int cmock_num_calls)
{
	static uint8_t invalid_digest_bstr_cbor[] = {
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x22, /* suit-digest-bytes: bytes(34) */
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
		0xff, 0xff,
	};

	decoder_state->manifest_digest_bytes.value = invalid_digest_bstr_cbor;
	decoder_state->manifest_digest_bytes.len = sizeof(invalid_digest_bstr_cbor);

	return SUIT_SUCCESS;
}

static int mock_manifest_invalid_digest_length_sha512(struct suit_decoder_state* decoder_state, int cmock_num_calls)
{
	static uint8_t invalid_digest_bstr_cbor[] = {
		0x82, /* array (2 elements) */
		0x38, 0x2B, /* suit-digest-algorithm-id: cose-alg-sha-512 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
	};

	decoder_state->manifest_digest_bytes.value = invalid_digest_bstr_cbor;
	decoder_state->manifest_digest_bytes.len = sizeof(invalid_digest_bstr_cbor);

	return SUIT_SUCCESS;
}

static int mock_valid_manifest(struct suit_decoder_state* decoder_state, int cmock_num_calls)
{
	decoder_state->manifest_digest_bytes.value = valid_digest_bstr_cbor;
	decoder_state->manifest_digest_bytes.len = sizeof(valid_digest_bstr_cbor);

	decoder_state->decoded_manifest = &state.manifest_stack[0];
	decoder_state->decoded_manifest->sequence_number = exp_seq_num;
	decoder_state->decoded_manifest->manifest_component_id = exp_manifest_component_id;

	return SUIT_SUCCESS;
}

static int mock_valid_manifest_sha512(struct suit_decoder_state* decoder_state, int cmock_num_calls)
{
	decoder_state->manifest_digest_bytes.value = valid_digest_bstr_cbor_sha512;
	decoder_state->manifest_digest_bytes.len = sizeof(valid_digest_bstr_cbor_sha512);

	decoder_state->decoded_manifest = &state.manifest_stack[0];
	decoder_state->decoded_manifest->sequence_number = exp_seq_num;
	decoder_state->decoded_manifest->manifest_component_id = exp_manifest_component_id;

	return SUIT_SUCCESS;
}


void setUp(void)
{
	memset(&state, 0, sizeof(state));

	int ret = suit_processor_override_state(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to initialize SUIT processor with external state");

	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder has a valid state before the test starts");
}

void test_invalid_input(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	int ret = suit_processor_override_state(NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_CRASH, ret, "Setting SUIT processor state to NULL did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");

	ret = suit_processor_get_manifest_metadata(NULL, envelope_len, false, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "Envelope was set to NULL and was decoded");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");

	ret = suit_processor_get_manifest_metadata(&envelope_str[0], 0, false, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "Envelope length was set to zero and was decoded");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_invalid_decoder_state(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	enum suit_decoder_step decoder_states[] = {
		INVALID,
		INITIALIZED,
		ENVELOPE_DECODED,
		MANIFEST_DIGEST_VERIFIED,
		MANIFEST_DECODED,
		MANIFEST_AUTHENTICATED,
		MANIFEST_AUTHORIZED,
		SEQUENCES_DECODED,
		COMPONENTS_CREATED,
		LAST_STEP,
	};

	for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(decoder_states); i++) {
		state.decoder_state.step = decoder_states[i];

		if ((decoder_states[i] == INVALID) ||
		    (decoder_states[i] == COMPONENTS_CREATED) ||
		    (decoder_states[i] == LAST_STEP)) {
			__cmock_suit_decoder_init_ExpectAndReturn(
				&state.decoder_state,
				&state.manifest_stack[0],
				SUIT_ERR_UNSUPPORTED_COMPONENT_ID);

			int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, NULL, NULL);
			TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "Envelope decoder was not busy, but the metadata API failed");
			TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
		} else {
			int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, NULL, NULL);
			TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_WAIT, ret, "Envelope decoder was busy, but it was overwritten by the metadata API");
			TEST_ASSERT_EQUAL_MESSAGE(decoder_states[i], state.decoder_state.step, "SUIT decoder state was busy and has been reset");
		}
	}
}

void test_decoder_init_failed(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_ERR_UNSUPPORTED_COMPONENT_ID);
	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "Envelope decoder was not busy, but the metadata API failed");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_decode_envelope_failed(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_ERR_UNSUPPORTED_COMPONENT_ID);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "Envelope decoding failed, but error code was not returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_check_manifest_digest_failed(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_ERR_UNSUPPORTED_COMPONENT_ID);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "Manifest digest check failed, but error code was not returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_decode_manifest_failed(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_ERR_UNSUPPORTED_COMPONENT_ID);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "Manifest decoding failed, but error code was not returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_authenticate_manifest_failed(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_authenticate_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_ERR_UNSUPPORTED_COMPONENT_ID);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, true, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "Manifest authentication failed, but error code was not returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_authorize_manifest_failed(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_authenticate_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_authorize_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_ERR_UNSUPPORTED_COMPONENT_ID);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, true, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_UNSUPPORTED_COMPONENT_ID, ret, "Manifest authorization failed, but error code was not returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_invalid_decoded_digest_bstr(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_manifest_invalid_digest_bstr);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "Invalid manifest digest decoded, but error code was not returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_invalid_decoded_digest_length(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_manifest_invalid_digest_length);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "Invalid manifest digest length decoded, but error code was not returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_invalid_decoded_digest_length_sha512(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_manifest_invalid_digest_length_sha512);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "Invalid manifest digest (SHA-512) length decoded, but error code was not returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_metadata_digest_no_alg(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);
	struct zcbor_string digest;

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, &digest, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "Algorithm ID was set to NULL and digest was returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_metadata_digest_no_digest_bstr(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);
	enum suit_cose_alg alg;

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, &alg, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "Digest bstr was set to NULL and algorithm ID was returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_no_metadata_returned(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Manifest decoded, but error code was returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_no_metadata_returned_auth(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);
	__cmock_suit_decoder_authenticate_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_authorize_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, true, NULL, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Manifest decoded with authentication, but error code was returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_metadata_digest(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);
	struct zcbor_string digest;
	enum suit_cose_alg alg;

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, &digest, &alg, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Manifest decoded, but error code was returned");
	TEST_ASSERT_EQUAL_MESSAGE(suit_cose_sha256, alg, "Invalid manifest digest algorithm ID returned");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(exp_manifest_digest.value, digest.value, "Invalid manifest digest returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_manifest_digest.len, digest.len, "Invalid manifest digest returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_metadata_digest_sha512(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);
	struct zcbor_string digest;
	enum suit_cose_alg alg;

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest_sha512);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, &digest, &alg, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Manifest decoded, but error code was returned");
	TEST_ASSERT_EQUAL_MESSAGE(suit_cose_sha512, alg, "Invalid manifest digest algorithm ID returned");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(exp_manifest_digest_sha512.value, digest.value, "Invalid manifest digest (SHA-512) returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_manifest_digest_sha512.len, digest.len, "Invalid manifest digest (SHA-512) returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_metadata_digest_auth(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);
	struct zcbor_string digest;
	enum suit_cose_alg alg;

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);
	__cmock_suit_decoder_authenticate_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_authorize_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, true, NULL, &digest, &alg, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Manifest decoded with authentication, but error code was returned");
	TEST_ASSERT_EQUAL_MESSAGE(suit_cose_sha256, alg, "Invalid manifest digest algorithm ID returned");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(exp_manifest_digest.value, digest.value, "Invalid manifest digest returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_manifest_digest.len, digest.len, "Invalid manifest digest returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_metadata_seq_num(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);
	unsigned int seq_num;

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, NULL, NULL, NULL, &seq_num);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Manifest decoded, but error code was returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_seq_num, seq_num, "Invalid manifest sequence number returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_metadata_seq_num_auth(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);
	unsigned int seq_num;

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);
	__cmock_suit_decoder_authenticate_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_authorize_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, true, NULL, NULL, NULL, &seq_num);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Manifest decoded with authentication, but error code was returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_seq_num, seq_num, "Invalid manifest sequence number returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_metadata_manifest_component_id(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);
	struct zcbor_string manifest_component_id;

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, &manifest_component_id, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Manifest decoded, but error code was returned");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(exp_manifest_component_id.value, manifest_component_id.value, "Invalid manifest component ID returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_manifest_component_id.len, manifest_component_id.len, "Invalid manifest component ID returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_metadata_manifest_component_id_auth(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);
	struct zcbor_string manifest_component_id;

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);
	__cmock_suit_decoder_authenticate_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_authorize_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, true, &manifest_component_id, NULL, NULL, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Manifest decoded with authentication, but error code was returned");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(exp_manifest_component_id.value, manifest_component_id.value, "Invalid manifest component ID returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_manifest_component_id.len, manifest_component_id.len, "Invalid manifest component ID returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_metadata_all(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);
	struct zcbor_string manifest_component_id;
	struct zcbor_string digest;
	enum suit_cose_alg alg;
	unsigned int seq_num;

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, false, &manifest_component_id, &digest, &alg, &seq_num);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Manifest decoded, but error code was returned");
	TEST_ASSERT_EQUAL_MESSAGE(suit_cose_sha256, alg, "Invalid manifest digest algorithm ID returned");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(exp_manifest_digest.value, digest.value, "Invalid manifest digest returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_manifest_digest.len, digest.len, "Invalid manifest digest returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_seq_num, seq_num, "Invalid manifest sequence number returned");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(exp_manifest_component_id.value, manifest_component_id.value, "Invalid manifest component ID returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_manifest_component_id.len, manifest_component_id.len, "Invalid manifest component ID returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
}

void test_metadata_all_auth(void)
{
	uint8_t envelope_str[] = {
		0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
		0xa0, /* map (0 elements) */
	};
	size_t envelope_len = sizeof(envelope_str);
	struct zcbor_string manifest_component_id;
	struct zcbor_string digest;
	enum suit_cose_alg alg;
	unsigned int seq_num;

	__cmock_suit_decoder_init_ExpectAndReturn(
		&state.decoder_state,
		&state.manifest_stack[0],
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_envelope_ExpectAndReturn(
		&state.decoder_state,
		&envelope_str[0],
		envelope_len,
		SUIT_SUCCESS);
	__cmock_suit_decoder_check_manifest_digest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_decode_manifest_StubWithCallback(mock_valid_manifest);
	__cmock_suit_decoder_authenticate_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);
	__cmock_suit_decoder_authorize_manifest_ExpectAndReturn(
		&state.decoder_state,
		SUIT_SUCCESS);

	int ret = suit_processor_get_manifest_metadata(&envelope_str[0], envelope_len, true, &manifest_component_id, &digest, &alg, &seq_num);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Manifest decoded, but error code was returned");
	TEST_ASSERT_EQUAL_MESSAGE(suit_cose_sha256, alg, "Invalid manifest digest algorithm ID returned");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(exp_manifest_digest.value, digest.value, "Invalid manifest digest returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_manifest_digest.len, digest.len, "Invalid manifest digest returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_seq_num, seq_num, "Invalid manifest sequence number returned");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(exp_manifest_component_id.value, manifest_component_id.value, "Invalid manifest component ID returned");
	TEST_ASSERT_EQUAL_MESSAGE(exp_manifest_component_id.len, manifest_component_id.len, "Invalid manifest component ID returned");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.decoder_state.step, "SUIT decoder state not reset after decoding");
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
