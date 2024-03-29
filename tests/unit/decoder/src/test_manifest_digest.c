/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "suit_decoder_test_utils.h"
#include "suit_platform_mock_ext.h"

static uint8_t cbor_digest_unavailable[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t cbor_unsupported_digest_algorithm[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x28, /* bytes(40) */
		0x81, /* array (1 element) */
			0x58, 0x25, /* bytes(37) */
			0x82, /* array (2 elements) */
			0x38, 0x2c, /* suit-digest-algorithm-id: cose-alg-shake256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t cbor_invalid_digest_length[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x47, /* bytes(71) */
		0x81, /* array (1 element) */
			0x58, 0x44, /* bytes(68) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x40, /* suit-digest-bytes: bytes(64) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t cbor_invalid_digest_length_sha512[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x28, /* bytes(40) */
		0x81, /* array (1 element) */
			0x58, 0x25, /* bytes(37) */
			0x82, /* array (2 elements) */
			0x38, 0x2B, /* suit-digest-algorithm-id: cose-alg-sha-512 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t cbor_invalid_digest_length_encoding[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x28, /* bytes(40) */
		0x81, /* array (1 element) */
			0x58, 0x25, /* bytes(37) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x59, 0x00, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t minimal_checkable_envelope[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x27, /* bytes(39) */
		0x81, /* array (1 element) */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t minimal_checkable_envelope_sha512[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x48, /* bytes(72) */
		0x81, /* array (1 element) */
			0x58, 0x45, /* bytes(69) */
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

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};


void test_manifest_digest_invalid_input(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_envelope(minimal_checkable_envelope, sizeof(minimal_checkable_envelope));

	ret = suit_decoder_check_manifest_digest(NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The manifest digest check did not fail on NULL context");
}

void test_manifest_digest_invalid_state(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_envelope(minimal_checkable_envelope, sizeof(minimal_checkable_envelope));

	for (enum suit_decoder_step step = INVALID; step <= LAST_STEP; step++) {
		if (step == ENVELOPE_DECODED) {
			continue;
		}

		state.step = step;
		ret = suit_decoder_check_manifest_digest(&state);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The manifest digest check did not fail in incorrect state");
	}
}

void test_manifest_digest_minimal(void)
{
	int ret = SUIT_SUCCESS;
	uint8_t digest[] = {
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
	};
	struct zcbor_string exp_manifest_digest = {
		.value = &digest[4],
		.len = 32,
	};
	struct zcbor_string exp_manifest_payload = {
		.value = "HManifest", /* 'H' is the bstr tag and length. */
		.len = 9,
	};

	init_decode_envelope(minimal_checkable_envelope, sizeof(minimal_checkable_envelope));
	TEST_ASSERT_EQUAL_MESSAGE(32 + 4, state.manifest_digest_bytes.len, "Decoding the manifest digest length failed");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(digest, state.manifest_digest_bytes.value, sizeof(digest), "Decoding the manifest digest value failed");

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_manifest_digest, &exp_manifest_payload, SUIT_SUCCESS);
	ret = suit_decoder_check_manifest_digest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest digest check failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_DIGEST_VERIFIED, state.step, "Invalid state transition after manifest digest check");
}

void test_manifest_digest_minimal_sha512(void)
{
	int ret = SUIT_SUCCESS;
	uint8_t digest[] = {
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
	struct zcbor_string exp_manifest_digest = {
		.value = &digest[5],
		.len = 64,
	};
	struct zcbor_string exp_manifest_payload = {
		.value = "HManifest", /* 'H' is the bstr tag and length. */
		.len = 9,
	};

	init_decode_envelope(minimal_checkable_envelope_sha512, sizeof(minimal_checkable_envelope_sha512));
	TEST_ASSERT_EQUAL_MESSAGE(64 + 5, state.manifest_digest_bytes.len, "Decoding the manifest digest length failed");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(digest, state.manifest_digest_bytes.value, sizeof(digest), "Decoding the manifest digest value failed");

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha512, &exp_manifest_digest, &exp_manifest_payload, SUIT_SUCCESS);
	ret = suit_decoder_check_manifest_digest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest digest check failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_DIGEST_VERIFIED, state.step, "Invalid state transition after manifest digest check");
}

void test_manifest_digest_invalid_input_bytes(void)
{
	int ret = SUIT_SUCCESS;

	struct input_envelope envelopes[] = {
		{
			.envelope = cbor_digest_unavailable,
			.envelope_size = sizeof(cbor_digest_unavailable),
			.exp_ret = SUIT_ERR_DECODING,
		},
		{
			.envelope = cbor_unsupported_digest_algorithm,
			.envelope_size = sizeof(cbor_unsupported_digest_algorithm),
			.exp_ret = SUIT_ERR_DECODING,
		},
		{
			.envelope = cbor_invalid_digest_length,
			.envelope_size = sizeof(cbor_invalid_digest_length),
			.exp_ret = SUIT_ERR_DECODING,
		},
		{
			.envelope = cbor_invalid_digest_length_sha512,
			.envelope_size = sizeof(cbor_invalid_digest_length_sha512),
			.exp_ret = SUIT_ERR_DECODING,
		},
		{
			.envelope = cbor_invalid_digest_length_encoding,
			.envelope_size = sizeof(cbor_invalid_digest_length_encoding),
			.exp_ret = SUIT_ERR_DECODING,
		},
	};

	for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(envelopes); i++) {
		/* Reset state. */
		memset(&state, 0, sizeof(state));
		memset(&manifest, 0, sizeof(manifest));
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "SUIT decoder has a valid state before the test starts");
		init_decode_envelope(envelopes[i].envelope, envelopes[i].envelope_size);

		ret = suit_decoder_check_manifest_digest(&state);
		TEST_ASSERT_EQUAL_MESSAGE(envelopes[i].exp_ret, ret, "The manifest digest check failed");
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest digest check");
		TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest digest check failure");
	}
}

void test_manifest_digest_minimal_platform_fail(void)
{
	int ret = SUIT_SUCCESS;
	uint8_t digest[] = {
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
	};
	struct zcbor_string exp_manifest_digest = {
		.value = &digest[4],
		.len = 32,
	};
	struct zcbor_string exp_manifest_payload = {
		.value = "HManifest", /* 'H' is the bstr tag and length. */
		.len = 9,
	};

	init_decode_envelope(minimal_checkable_envelope, sizeof(minimal_checkable_envelope));
	TEST_ASSERT_EQUAL_MESSAGE(32 + 4, state.manifest_digest_bytes.len, "Decoding the manifest digest length failed");
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(digest, state.manifest_digest_bytes.value, sizeof(digest), "Decoding the manifest digest value failed");

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_manifest_digest, &exp_manifest_payload, SUIT_ERR_MANIFEST_VERIFICATION);
	ret = suit_decoder_check_manifest_digest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VERIFICATION, ret, "The manifest digest check failed");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest digest check");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest digest check failure");
}
