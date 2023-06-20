/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "suit_decoder_test_utils.h"
#include "suit_platform_mock_ext.h"


static uint8_t minimal_without_sequences[] = {
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

static uint8_t minimal_with_shared_and_validate[] = {
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
	0x07, /* suit-validate */
		0x40, /* bytes(0) */
};

static uint8_t minimal_with_shared_and_load[] = {
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
	0x08, /* suit-load */
		0x40, /* bytes(0) */
};

static uint8_t minimal_with_shared_and_invoke[] = {
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
	0x09, /* suit-invoke */
		0x40, /* bytes(0) */
};

static uint8_t minimal_with_shared_and_payload_fetch[] = {
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
	0x10, /* suit-payload-fetch */
		0x40, /* bytes(0) */
};

static uint8_t minimal_with_shared_and_install[] = {
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
	0x11, /* suit-install */
		0x40, /* bytes(0) */
};

static uint8_t minimal_with_shared_and_dependency_resolution[] = {
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
	0x0f, /* suit-dependency-resolution */
		0x40, /* bytes(0) */
};

static uint8_t minimal_with_shared_and_text[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa3, /* map (3 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x2f, /* bytes(47) */
	0xa4, /* map (4 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x43, /* bytes(3) */
		0xA1, /* map (1 element) */
			0x04, /* suit-shared-sequence */
			0x40, /* bytes(0) */
	0x17, /* suit-text (severed - digest) */
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

	0x17, /* suit-text (severed - value) */
		0x41, /* bytes(1) */
			'D',
};

static uint8_t minimal_with_shared_and_text_without_digest[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa3, /* map (3 elements) */

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

	0x17, /* suit-text (severed - value) */
		0x41, /* bytes(1) */
			'D',
};

static uint8_t envelope_with_all_sequences[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa3, /* map (3 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0x41, /* bytes(65) */
	0xaa, /* map (10 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x10,
	0x03, /* suit-common */
		0x43, /* bytes(3) */
		0xA1, /* map (1 element) */
			0x04, /* suit-shared-sequence */
			0x40, /* bytes(0) */

	0x07, /* suit-validate */
		0x41, /* bytes(1) */
			'V',
	0x08, /* suit-load */
		0x41, /* bytes(1) */
			'L',
	0x09, /* suit-invoke */
		0x41, /* bytes(1) */
			'R',
	0x10, /* suit-payload-fetch */
		0x41, /* bytes(1) */
			'P',
	0x11, /* suit-install */
		0x41, /* bytes(1) */
			'I',
	0x17, /* suit-text (severed - digest) */
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
	0x0f, /* suit-dependency-resolution */
		0x41, /* bytes(1) */
			'D',

	0x17, /* suit-text (severed - value) */
		0x41, /* bytes(1) */
			'T',
};

void test_decode_sequences_invalid_input(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_with_shared, sizeof(minimal_with_shared));
	state.step = MANIFEST_AUTHORIZED;

	ret = suit_decoder_decode_sequences(NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The sequence decoding did not fail on NULL context");
}

void test_decode_sequences_invalid_state(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_with_shared, sizeof(minimal_with_shared));

	for (enum suit_decoder_step step = INVALID; step <= LAST_STEP; step++) {
		if (step == MANIFEST_AUTHORIZED) {
			continue;
		}

		state.step = step;
		ret = suit_decoder_decode_sequences(&state);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The sequence decoding did not fail in incorrect state");
	}
}

void test_decode_sequences_no_sequences(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_without_sequences, sizeof(minimal_without_sequences));
	state.step = MANIFEST_AUTHORIZED;

	ret = suit_decoder_decode_sequences(&state);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "The manifest without sequences decoding did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest sequences decoding");
	TEST_ASSERT_EQUAL_MESSAGE(NULL, state.decoded_manifest, "Manifest structure not freed after manifest sequences decoding failure");
}

void test_decode_sequences_shared(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_manifest(minimal_with_shared, sizeof(minimal_with_shared));
	state.step = MANIFEST_AUTHORIZED;

	ret = suit_decoder_decode_sequences(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest sequence decoding failed");
	TEST_ASSERT_EQUAL_MESSAGE(SEQUENCES_DECODED, state.step, "Invalid state transition after manifest sequence decoding");
}

void test_decode_sequences_invalid_sequence_state(void)
{
	int ret = SUIT_SUCCESS;

	struct input_envelope envelopes[] = {
		{
			.envelope = minimal_with_shared,
			.envelope_size = sizeof(minimal_with_shared),
			.exp_ret = SUIT_ERR_MANIFEST_VALIDATION,
		},
		{
			.envelope = minimal_with_shared_and_validate,
			.envelope_size = sizeof(minimal_with_shared_and_validate),
			.exp_ret = SUIT_ERR_MANIFEST_VALIDATION,
		},
		{
			.envelope = minimal_with_shared_and_load,
			.envelope_size = sizeof(minimal_with_shared_and_load),
			.exp_ret = SUIT_ERR_MANIFEST_VALIDATION,
		},
		{
			.envelope = minimal_with_shared_and_invoke,
			.envelope_size = sizeof(minimal_with_shared_and_invoke),
			.exp_ret = SUIT_ERR_MANIFEST_VALIDATION,
		},
		{
			.envelope = minimal_with_shared_and_payload_fetch,
			.envelope_size = sizeof(minimal_with_shared_and_payload_fetch),
			.exp_ret = SUIT_ERR_MANIFEST_VALIDATION,
		},
		{
			.envelope = minimal_with_shared_and_install,
			.envelope_size = sizeof(minimal_with_shared_and_install),
			.exp_ret = SUIT_ERR_MANIFEST_VALIDATION,
		},
		{
			.envelope = minimal_with_shared_and_dependency_resolution,
			.envelope_size = sizeof(minimal_with_shared_and_dependency_resolution),
			.exp_ret = SUIT_ERR_MANIFEST_VALIDATION,
		},
	};

	enum suit_seq_status *seq_status[] = {
		&manifest.shared_sequence_status,
		&manifest.validate_seq_status,
		&manifest.load_seq_status,
		&manifest.invoke_seq_status,
		&manifest.payload_fetch_seq_status,
		&manifest.install_seq_status,
		&manifest.dependency_resolution_seq_status,
	};

	for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(envelopes); i++) {
		/* Reset state. */
		memset(&state, 0, sizeof(state));
		memset(&manifest, 0, sizeof(manifest));
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "SUIT decoder has a valid state before the test starts");
		init_decode_manifest(envelopes[i].envelope, envelopes[i].envelope_size);
		state.step = MANIFEST_AUTHORIZED;
		*seq_status[i] = SEVERED;

		ret = suit_decoder_decode_sequences(&state);
		TEST_ASSERT_EQUAL_MESSAGE(envelopes[i].exp_ret, ret, "The manifest sequence decoding did not fail");
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest sequences decoding");
		TEST_ASSERT_EQUAL_MESSAGE(NULL, state.decoded_manifest, "Manifest structure not freed after manifest sequences decoding failure");
	}

	for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(envelopes); i++) {
		/* Reset state. */
		memset(&state, 0, sizeof(state));
		memset(&manifest, 0, sizeof(manifest));
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "SUIT decoder has a valid state before the test starts");
		init_decode_manifest(envelopes[i].envelope, envelopes[i].envelope_size);
		state.step = MANIFEST_AUTHORIZED;
		*seq_status[i] = AUTHENTICATED;

		ret = suit_decoder_decode_sequences(&state);
		TEST_ASSERT_EQUAL_MESSAGE(envelopes[i].exp_ret, ret, "The manifest sequences decoding did not fail");
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest sequences decoding");
		TEST_ASSERT_EQUAL_MESSAGE(NULL, state.decoded_manifest, "Manifest structure not freed aftermanifest sequences decoding failure");
	}
}

void test_decode_sequences_shared_and_one_sequence(void)
{
	int ret = SUIT_SUCCESS;

	struct input_envelope envelopes[] = {
		{
			.envelope = minimal_with_shared_and_validate,
			.envelope_size = sizeof(minimal_with_shared_and_validate),
			.exp_ret = SUIT_SUCCESS,
		},
		{
			.envelope = minimal_with_shared_and_load,
			.envelope_size = sizeof(minimal_with_shared_and_load),
			.exp_ret = SUIT_SUCCESS,
		},
		{
			.envelope = minimal_with_shared_and_invoke,
			.envelope_size = sizeof(minimal_with_shared_and_invoke),
			.exp_ret = SUIT_SUCCESS,
		},
		{
			.envelope = minimal_with_shared_and_payload_fetch,
			.envelope_size = sizeof(minimal_with_shared_and_payload_fetch),
			.exp_ret = SUIT_SUCCESS,
		},
		{
			.envelope = minimal_with_shared_and_install,
			.envelope_size = sizeof(minimal_with_shared_and_install),
			.exp_ret = SUIT_SUCCESS,
		},
		{
			.envelope = minimal_with_shared_and_dependency_resolution,
			.envelope_size = sizeof(minimal_with_shared_and_dependency_resolution),
			.exp_ret = SUIT_SUCCESS,
		},
	};

	enum suit_seq_status *seq_status[] = {
		&manifest.validate_seq_status,
		&manifest.load_seq_status,
		&manifest.invoke_seq_status,
		&manifest.payload_fetch_seq_status,
		&manifest.install_seq_status,
		&manifest.dependency_resolution_seq_status,
	};

	for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(envelopes); i++) {
		/* Reset state. */
		memset(&state, 0, sizeof(state));
		memset(&manifest, 0, sizeof(manifest));
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "SUIT decoder has a valid state before the test starts");
		init_decode_manifest(envelopes[i].envelope, envelopes[i].envelope_size);
		state.step = MANIFEST_AUTHORIZED;

		ret = suit_decoder_decode_sequences(&state);
		TEST_ASSERT_EQUAL_MESSAGE(envelopes[i].exp_ret, ret, "The manifest sequences decoding failed");
		TEST_ASSERT_EQUAL_MESSAGE(SEQUENCES_DECODED, state.step, "Invalid state transition after manifest sequence decoding");

		for (size_t seq_i = 0; seq_i < ZCBOR_ARRAY_SIZE(envelopes); seq_i++) {
			if (seq_i == i) {
				TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, *seq_status[seq_i], "Unsevered sequence decoded but not marked as authenticated");
				TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, manifest.shared_sequence_status, "Shared sequence decoded but not marked as authenticated");
			} else {
				TEST_ASSERT_EQUAL_MESSAGE(UNAVAILABLE, *seq_status[seq_i], "Unsevered sequence not decoded but not marked as unavailable");
			}
		}
	}
}

void test_decode_sequences_text_invalid_state(void)
{
	init_decode_manifest_with_text(minimal_with_shared_and_text, sizeof(minimal_with_shared_and_text));
	state.step = MANIFEST_AUTHORIZED;
	manifest.text_status = AUTHENTICATED;

	int ret = suit_decoder_decode_sequences(&state);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "The manifest text field decoding did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed text field status check");
	TEST_ASSERT_EQUAL_MESSAGE(NULL, state.decoded_manifest, "Manifest structure not freed after text field status check failure");
}

void test_decode_sequences_text_without_digest(void)
{
	init_decode_manifest_with_text(minimal_with_shared_and_text_without_digest, sizeof(minimal_with_shared_and_text_without_digest));
	state.step = MANIFEST_AUTHORIZED;
	manifest.text_status = AUTHENTICATED;

	int ret = suit_decoder_decode_sequences(&state);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "The manifest text field decoding did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed text field status check");
	TEST_ASSERT_EQUAL_MESSAGE(NULL, state.decoded_manifest, "Manifest structure not freed after text field status check failure");
}

void test_decode_sequences_text_with_valid_digest(void)
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
	struct zcbor_string exp_digest = {
		.value = &digest[4],
		.len = 32,
	};
	struct zcbor_string exp_payload = {
		.value = "D",
		.len = 1,
	};

	init_decode_manifest_with_text(minimal_with_shared_and_text, sizeof(minimal_with_shared_and_text));
	state.step = MANIFEST_AUTHORIZED;

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_digest, &exp_payload, SUIT_SUCCESS);

	ret = suit_decoder_decode_sequences(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest sequence decoding failed");
	TEST_ASSERT_EQUAL_MESSAGE(SEQUENCES_DECODED, state.step, "Invalid state transition after manifest sequence decoding");
}

void test_decode_sequences_text_with_digest_platform_fail(void)
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
	struct zcbor_string exp_digest = {
		.value = &digest[4],
		.len = 32,
	};
	struct zcbor_string exp_payload = {
		.value = "D",
		.len = 1,
	};

	init_decode_manifest_with_text(minimal_with_shared_and_text, sizeof(minimal_with_shared_and_text));
	state.step = MANIFEST_AUTHORIZED;

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_digest, &exp_payload, SUIT_ERR_MANIFEST_VERIFICATION);

	ret = suit_decoder_decode_sequences(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "The manifest text field decoding did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed text field digest check");
	TEST_ASSERT_EQUAL_MESSAGE(NULL, state.decoded_manifest, "Manifest structure not freed after text field digest check failure");
}

void test_decode_sequences_all(void)
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
	struct zcbor_string exp_digest = {
		.value = &digest[4],
		.len = 32,
	};
	struct zcbor_string exp_payload = {
		.value = "T",
		.len = 1,
	};

	init_decode_manifest_with_text(envelope_with_all_sequences, sizeof(envelope_with_all_sequences));
	state.step = MANIFEST_AUTHORIZED;

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_digest, &exp_payload, SUIT_SUCCESS);

	ret = suit_decoder_decode_sequences(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest sequence decoding failed");
	TEST_ASSERT_EQUAL_MESSAGE(SEQUENCES_DECODED, state.step, "Invalid state transition after manifest sequence decoding");

	TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, manifest.shared_sequence_status, "Shared sequence decoded but not marked as authenticated");

	TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, manifest.validate_seq_status, "Validate sequence decoded but not marked as authenticated");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.validate_seq.len, "Validate sequence length does not match");
	TEST_ASSERT_EQUAL_MESSAGE('V', manifest.validate_seq.value[0], "Validate sequence value does not match");

	TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, manifest.load_seq_status, "Load sequence decoded but not marked as authenticated");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.load_seq.len, "Load sequence length does not match");
	TEST_ASSERT_EQUAL_MESSAGE('L', manifest.load_seq.value[0], "Load sequence value does not match");

	TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, manifest.invoke_seq_status, "Invoke sequence decoded but not marked as authenticated");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.invoke_seq.len, "Invoke sequence length does not match");
	TEST_ASSERT_EQUAL_MESSAGE('R', manifest.invoke_seq.value[0], "Invoke sequence value does not match");

	TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, manifest.payload_fetch_seq_status, "Payload-fetch sequence decoded but not marked as authenticated");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.payload_fetch_seq.len, "Payload-fetch sequence length does not match");
	TEST_ASSERT_EQUAL_MESSAGE('P', manifest.payload_fetch_seq.value[0], "Payload-fetch sequence value does not match");

	TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, manifest.install_seq_status, "Install sequence decoded but not marked as authenticated");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.install_seq.len, "Install sequence length does not match");
	TEST_ASSERT_EQUAL_MESSAGE('I', manifest.install_seq.value[0], "Install sequence value does not match");

	TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, manifest.text_status, "Severed text sequence decoded but not marked as authenticated");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.text.len, "Severed text sequence length does not match");
	TEST_ASSERT_EQUAL_MESSAGE('T', manifest.text.value[0], "Severed text sequence value does not match");

	TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, manifest.dependency_resolution_seq_status, "Dependency-resolution sequence decoded but not marked as authenticated");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.dependency_resolution_seq.len, "Dependency-resolution sequence length does not match");
	TEST_ASSERT_EQUAL_MESSAGE('D', manifest.dependency_resolution_seq.value[0], "Dependency-resolution sequence value does not match");
}
