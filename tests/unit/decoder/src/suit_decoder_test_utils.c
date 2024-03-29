/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "suit_decoder_test_utils.h"

void init_static_mem_input(void)
{
	int ret = SUIT_SUCCESS;

	memset(&state, 0, sizeof(state));
	memset(&manifest, 0, sizeof(manifest));

	ret = suit_decoder_init(&state, &manifest);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to initialize SUIT manifest decoder");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(&manifest, state.decoded_manifest, "SUIT manifest structure not initialized");
	TEST_ASSERT_EQUAL_MESSAGE(INITIALIZED, state.step, "SUIT decoder state not initialized");
}

static void init_decode_envelope_common(uint8_t *envelope, size_t envelope_size)
{
	/* Use the other test as an additional setUp step. */
	init_static_mem_input();

	int ret = suit_decoder_decode_envelope(&state, envelope, envelope_size);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Decoding of valid envelope failed");
	TEST_ASSERT_EQUAL_MESSAGE(ENVELOPE_DECODED, state.step, "Invalid state transition after envelope decoding");

	TEST_ASSERT_EQUAL_MESSAGE(0, state.authentication_bstr_count, "Decoding authentication block count failed");

	TEST_ASSERT_EQUAL_PTR_MESSAGE(envelope, state.decoded_manifest->envelope_str.value, "Invalid reference to the decoded envelope");
	TEST_ASSERT_EQUAL_MESSAGE(envelope_size, state.decoded_manifest->envelope_str.len, "Invalid length of the decoded envelope");
	TEST_ASSERT_EQUAL_MESSAGE(0, state.decoded_manifest->integrated_payloads_count, "Decoding the number of integrated payloads failed");
}

void init_decode_envelope_with_text(uint8_t *envelope, size_t envelope_size)
{
	init_decode_envelope_common(envelope, envelope_size);
	TEST_ASSERT_EQUAL_MESSAGE(SEVERED, state.decoded_manifest->text_status, "Decoding the severable text field failed (not present)");
}

void init_decode_envelope(uint8_t *envelope, size_t envelope_size)
{
	init_decode_envelope_common(envelope, envelope_size);
	TEST_ASSERT_EQUAL_MESSAGE(UNAVAILABLE, state.decoded_manifest->text_status, "Decoding the severable text field failed (not unavailable)");
}

void init_decode_signed_envelope(uint8_t *envelope, size_t envelope_size, size_t auth_count)
{
	/* Use the other test as an additional setUp step. */
	init_static_mem_input();

	int ret = suit_decoder_decode_envelope(&state, envelope, envelope_size);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Decoding of valid envelope failed");
	TEST_ASSERT_EQUAL_MESSAGE(ENVELOPE_DECODED, state.step, "Invalid state transition after envelope decoding");

	TEST_ASSERT_EQUAL_MESSAGE(auth_count, state.authentication_bstr_count, "Decoding authentication block count failed");
	TEST_ASSERT_EQUAL_MESSAGE(UNAVAILABLE, state.decoded_manifest->text_status, "Decoding the severable text field failed");

	TEST_ASSERT_EQUAL_PTR_MESSAGE(envelope, state.decoded_manifest->envelope_str.value, "Invalid reference to the decoded envelope");
	TEST_ASSERT_EQUAL_MESSAGE(envelope_size, state.decoded_manifest->envelope_str.len, "Invalid length of the decoded envelope");
	TEST_ASSERT_EQUAL_MESSAGE(0, state.decoded_manifest->integrated_payloads_count, "Decoding the number of integrated payloads failed");
}

void init_decode_manifest_with_text(uint8_t *envelope, size_t envelope_size)
{
	init_decode_envelope_with_text(envelope, envelope_size);
	state.step = MANIFEST_DIGEST_VERIFIED;

	int ret = suit_decoder_decode_manifest(&state);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest decoding failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_DECODED, state.step, "Invalid state transition after manifest decoding");
}

void init_decode_manifest(uint8_t *envelope, size_t envelope_size)
{
	init_decode_envelope(envelope, envelope_size);
	state.step = MANIFEST_DIGEST_VERIFIED;

	int ret = suit_decoder_decode_manifest(&state);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The manifest decoding failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_DECODED, state.step, "Invalid state transition after manifest decoding");
}
