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

static uint8_t minimal_with_shared_and_candidate_verification[] = {
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
	0x12, /* suit-candidate-verification */
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
	0x58, 0x44, /* bytes(68) */
	0xab, /* map (11 elements) */
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
	0x12, /* suit-candidate-verification */
		0x41, /* bytes(1) */
			'C',

	0x17, /* suit-text (severed - value) */
		0x41, /* bytes(1) */
			'T',
};

static uint8_t envelope_with_severed_sequences[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa7, /* map (7 elements) */

	0x02, /* suit-authentication-wrapper */
		0x42, /* bytes(2) */
		0x81, /* array (1 element) */
			0x40, /* bytes(0) */

	0x03, /* suit-manifest */
	0x58, 0xCC, /* bytes(204) */
	0xab, /* map (11 elements) */
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
	0x10, /* suit-payload-fetch (severed - digest) */
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0x84, 0xbd, 0xcd, 0x15, 0x66, 0x03, 0x93, 0x5b,
		0x71, 0x1c, 0xcc, 0xd5, 0xa7, 0xc4, 0x4b, 0x59,
		0xa4, 0x75, 0xe4, 0x44, 0x29, 0xab, 0x0f, 0x8a,
		0x27, 0xec, 0x18, 0xe1, 0x75, 0x81, 0x74, 0x36,
	0x11, /* suit-install (severed - digest) */
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0xa4, 0xaa, 0xb2, 0x74, 0xa5, 0x7a, 0x49, 0x58,
		0xeb, 0x0c, 0x35, 0x9a, 0xf6, 0x32, 0xe5, 0x50,
		0xaa, 0xed, 0xf5, 0x42, 0xe1, 0xa6, 0x23, 0xa6,
		0x7d, 0xb8, 0x34, 0x76, 0xd4, 0xaf, 0xce, 0x0b,
	0x17, /* suit-text (severed - digest) */
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
	0x0f, /* suit-dependency-resolution (severed - digest) */
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0xfb, 0xc1, 0xf6, 0x89, 0x8b, 0x3f, 0xd1, 0xd2,
		0xd8, 0x06, 0xfc, 0xb9, 0x44, 0xfe, 0x53, 0x5f,
		0xf5, 0xf4, 0xa1, 0xd9, 0x73, 0xd8, 0xef, 0x21,
		0x85, 0x58, 0xdd, 0x2e, 0x9a, 0xe5, 0x26, 0xa3,
	0x12, /* suit-candidate-verification (severed - digest) */
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0xab, 0x86, 0xfe, 0x44, 0xde, 0xa1, 0x5e, 0x7a,
		0x6b, 0x43, 0x8f, 0x74, 0x1a, 0xa2, 0x08, 0xf0,
		0x79, 0xd5, 0x5f, 0x62, 0x6f, 0x5c, 0x47, 0x1c,
		0x99, 0x9b, 0x2e, 0xd7, 0x8b, 0x37, 0xbd, 0x6a,

	0x10, /* suit-payload-fetch (severed - value) */
		0x41, /* bytes(1) */
			'P',

	0x11, /* suit-install (severed - value) */
		0x41, /* bytes(1) */
			'I',

	0x17, /* suit-text (severed - value) */
		0x41, /* bytes(1) */
			'T',

	0x0f, /* suit-dependency-resolution (severed - value) */
		0x41, /* bytes(1) */
			'D',

	0x12, /* suit-candidate-verification (severed - value) */
		0x41, /* bytes(1) */
			'C',
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
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest sequences decoding failure");
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
		{
			.envelope = minimal_with_shared_and_candidate_verification,
			.envelope_size = sizeof(minimal_with_shared_and_candidate_verification),
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
		&manifest.candidate_verification_seq_status,
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
		TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest sequences decoding failure");
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
		TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed aftermanifest sequences decoding failure");
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
		{
			.envelope = minimal_with_shared_and_candidate_verification,
			.envelope_size = sizeof(minimal_with_shared_and_candidate_verification),
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
		&manifest.candidate_verification_seq_status,
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

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest severed text field decoding failed");
	TEST_ASSERT_EQUAL_MESSAGE(SEQUENCES_DECODED, state.step, "Invalid state transition after manifest sequence decoding");
	TEST_ASSERT_EQUAL_MESSAGE(UNAVAILABLE, manifest.text_status, "The text field status is still valid, but the payload is not present");
}

void test_decode_sequences_text_without_digest(void)
{
	init_decode_manifest_with_text(minimal_with_shared_and_text_without_digest, sizeof(minimal_with_shared_and_text_without_digest));
	state.step = MANIFEST_AUTHORIZED;
	manifest.text_status = AUTHENTICATED;

	int ret = suit_decoder_decode_sequences(&state);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "The manifest text field decoding did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed text field status check");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after text field status check failure");
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
	uint8_t text_payload[] = {
		0x41, /* bytes(1) */
		'D',
	};
	struct zcbor_string exp_text_payload = {
		.value = text_payload,
		.len = sizeof(text_payload),
	};

	init_decode_manifest_with_text(minimal_with_shared_and_text, sizeof(minimal_with_shared_and_text));
	state.step = MANIFEST_AUTHORIZED;

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_digest, &exp_text_payload, SUIT_SUCCESS);

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
	uint8_t text_payload[] = {
		0x41, /* bytes(1) */
		'D',
	};
	struct zcbor_string exp_text_payload = {
		.value = text_payload,
		.len = sizeof(text_payload),
	};

	init_decode_manifest_with_text(minimal_with_shared_and_text, sizeof(minimal_with_shared_and_text));
	state.step = MANIFEST_AUTHORIZED;

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_digest, &exp_text_payload, SUIT_ERR_MANIFEST_VERIFICATION);

	ret = suit_decoder_decode_sequences(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "The manifest text field decoding did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed text field digest check");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after text field digest check failure");
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
	uint8_t text_payload[] = {
		0x41, /* bytes(1) */
		'T',
	};
	struct zcbor_string exp_text_payload = {
		.value = text_payload,
		.len = sizeof(text_payload),
	};


	init_decode_manifest_with_text(envelope_with_all_sequences, sizeof(envelope_with_all_sequences));
	state.step = MANIFEST_AUTHORIZED;

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_digest, &exp_text_payload, SUIT_SUCCESS);

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

	TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, manifest.candidate_verification_seq_status, "Candidate-verification sequence decoded but not marked as authenticated");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.candidate_verification_seq.len, "Candidate-verification sequence length does not match");
	TEST_ASSERT_EQUAL_MESSAGE('C', manifest.candidate_verification_seq.value[0], "Candidate-verification sequence value does not match");
}

void test_decode_sequences_all_severed(void)
{
	int ret = SUIT_SUCCESS;

	uint8_t text_digest[] = {
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
	};
	struct zcbor_string exp_text_digest = {
		.value = &text_digest[4],
		.len = 32,
	};
	uint8_t text_payload[] = {
		0x41, /* bytes(1) */
		'T',
	};
	struct zcbor_string exp_text_payload = {
		.value = text_payload,
		.len = sizeof(text_payload),
	};

	uint8_t payload_fetch_digest[] = {
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0x84, 0xbd, 0xcd, 0x15, 0x66, 0x03, 0x93, 0x5b,
		0x71, 0x1c, 0xcc, 0xd5, 0xa7, 0xc4, 0x4b, 0x59,
		0xa4, 0x75, 0xe4, 0x44, 0x29, 0xab, 0x0f, 0x8a,
		0x27, 0xec, 0x18, 0xe1, 0x75, 0x81, 0x74, 0x36,
	};
	struct zcbor_string exp_payload_fetch_digest = {
		.value = &payload_fetch_digest[4],
		.len = 32,
	};
	uint8_t payload_fetch_payload[] = {
		0x41, /* bytes(1) */
		'P',
	};
	struct zcbor_string exp_payload_fetch_payload = {
		.value = payload_fetch_payload,
		.len = sizeof(payload_fetch_payload),
	};

	uint8_t install_digest[] = {
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0xa4, 0xaa, 0xb2, 0x74, 0xa5, 0x7a, 0x49, 0x58,
		0xeb, 0x0c, 0x35, 0x9a, 0xf6, 0x32, 0xe5, 0x50,
		0xaa, 0xed, 0xf5, 0x42, 0xe1, 0xa6, 0x23, 0xa6,
		0x7d, 0xb8, 0x34, 0x76, 0xd4, 0xaf, 0xce, 0x0b,
	};
	struct zcbor_string exp_install_digest = {
		.value = &install_digest[4],
		.len = 32,
	};
	uint8_t install_payload[] = {
		0x41, /* bytes(1) */
		'I',
	};
	struct zcbor_string exp_install_payload = {
		.value = install_payload,
		.len = sizeof(install_payload),
	};

	uint8_t dependency_resolution_digest[] = {
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0xfb, 0xc1, 0xf6, 0x89, 0x8b, 0x3f, 0xd1, 0xd2,
		0xd8, 0x06, 0xfc, 0xb9, 0x44, 0xfe, 0x53, 0x5f,
		0xf5, 0xf4, 0xa1, 0xd9, 0x73, 0xd8, 0xef, 0x21,
		0x85, 0x58, 0xdd, 0x2e, 0x9a, 0xe5, 0x26, 0xa3,
	};
	struct zcbor_string exp_dependency_resolution_digest = {
		.value = &dependency_resolution_digest[4],
		.len = 32,
	};
	uint8_t dependency_resolution_payload[] = {
		0x41, /* bytes(1) */
		'D',
	};
	struct zcbor_string exp_dependency_resolution_payload = {
		.value = dependency_resolution_payload,
		.len = sizeof(dependency_resolution_payload),
	};

	uint8_t candidate_verification_digest[] = {
		0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0xab, 0x86, 0xfe, 0x44, 0xde, 0xa1, 0x5e, 0x7a,
		0x6b, 0x43, 0x8f, 0x74, 0x1a, 0xa2, 0x08, 0xf0,
		0x79, 0xd5, 0x5f, 0x62, 0x6f, 0x5c, 0x47, 0x1c,
		0x99, 0x9b, 0x2e, 0xd7, 0x8b, 0x37, 0xbd, 0x6a,
	};
	struct zcbor_string exp_candidate_verification_digest = {
		.value = &candidate_verification_digest[4],
		.len = 32,
	};
	uint8_t candidate_verification_payload[] = {
		0x41, /* bytes(1) */
		'C',
	};
	struct zcbor_string exp_candidate_verification_payload = {
		.value = candidate_verification_payload,
		.len = sizeof(candidate_verification_payload),
	};

	init_decode_manifest_with_text(envelope_with_severed_sequences, sizeof(envelope_with_severed_sequences));
	state.step = MANIFEST_AUTHORIZED;

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_payload_fetch_digest, &exp_payload_fetch_payload, SUIT_SUCCESS);
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_install_digest, &exp_install_payload, SUIT_SUCCESS);
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_text_digest, &exp_text_payload, SUIT_SUCCESS);
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_dependency_resolution_digest, &exp_dependency_resolution_payload, SUIT_SUCCESS);
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_candidate_verification_digest, &exp_candidate_verification_payload, SUIT_SUCCESS);

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

	TEST_ASSERT_EQUAL_MESSAGE(AUTHENTICATED, manifest.candidate_verification_seq_status, "Candidate-verification sequence decoded but not marked as authenticated");
	TEST_ASSERT_EQUAL_MESSAGE(1, manifest.candidate_verification_seq.len, "Candidate-verification sequence length does not match");
	TEST_ASSERT_EQUAL_MESSAGE('C', manifest.candidate_verification_seq.value[0], "Candidate-verification sequence value does not match");
}