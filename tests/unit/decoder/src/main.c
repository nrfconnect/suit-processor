/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include <stdint.h>
#include <suit_processor.h>
#include <suit_decoder.h>
#include "suit_platform/cmock_suit_platform.h"


struct suit_decoder_state state;
struct suit_manifest_state manifest;

uint8_t sample_signed_envelope[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x73, /* bytes(115) */
		0x82, /* array (2 elements) */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

			0x58, 0x4a, /* bytes(74): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x43, /* protected: bytes(3) / serialized map*/
			0xa1, /* header_map (1 element) */
			0x01, /* alg_id */ 0x26, /* ES256 */
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x58, 0x71, /* bytes(113) */
	0xa5, /* map (5 elements) */
	0x01, /* suit-manifest-version */ 0x01,
	0x02, /* suit-manifest-sequence-number */ 0x00,
	0x03, /* suit-common */
		0x58, 0x5f, /* bytes(95) */
		0xa2, /* map (2 elements) */
		0x02, /* suit-components */
		0x81, 0x81, 0x41, 0x00, /* [ [ bstr(0x00) ] ] */
		0x04, /* suit-shared-sequence */
		0x58, 0x56, /* bytes(86) */
		0x86, /* array (6 elements) */
		0x14, 0xa4, /* suit-directive-override-parameters (4) */
			0x01, /* suit-parameter-vendor-identifier */
			0x50, /* bytes(16) */
			0xfa, 0x6b, 0x4a, 0x53, 0xd5, 0xad, 0x5f, 0xdf,
			0xbe, 0x9d, 0xe6, 0x63, 0xe4, 0xd4, 0x1f, 0xfe,
			0x02, /* suit-parameter-class-identifier */
			0x50, /* bytes(16) */
			0x14, 0x92, 0xaf, 0x14, 0x25, 0x69, 0x5e, 0x48,
			0xbf, 0x42, 0x9b, 0x2d, 0x51, 0xf2, 0xab, 0x45,
			0x03, /* suit-parameter-image-digest */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* cose-alg-sha-256 */
			0x58, 0x20, /* bytes(32) */
			0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
			0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
			0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
			0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
			0x0e, 0x19, 0x87, 0xd0, /* suit-parameter-image-size : 34768 */
		0x01, 0x0f, /* suit-condition-vendor-identifier */
		0x02, 0x0f, /* suit-condition-class-identifier */

	0x07, /* suit-validate */
	0x43, /* bytes(3) */
		0x82, /* array (2 elements) */
		0x03, 0x0f, /* suit-condition-image-match */

	0x09, /* suit-invoke */
	0x43, /* bytes(3) */
		0x82, /* array (2 elements) */
		0x17, 0x02, /* suit-directive-invoke */
};


void setUp(void)
{
	memset(&state, 0, sizeof(state));
	memset(&manifest, 0, sizeof(manifest));

	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "SUIT decoder has a valid state before the test starts");
}

void test_init_invalid_input(void)
{
	int ret = SUIT_SUCCESS;

	ret = suit_decoder_init(NULL, &manifest);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "Initialization with NULL state pointer returned unexpected value");

	ret = suit_decoder_init(&state, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "Initialization with NULL manifest pointer returned unexpected value");
}

void test_init_static_mem_input(void)
{
	int ret = SUIT_SUCCESS;

	ret = suit_decoder_init(&state, &manifest);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to initialize SUIT manifest decoder");
	TEST_ASSERT_EQUAL_PTR_MESSAGE(&manifest, state.decoded_manifest, "SUIT manifest structure not initialized");
	TEST_ASSERT_EQUAL_MESSAGE(INITIALIZED, state.step, "SUIT decoder state not initialized");
}

void test_api_on_uninitialized_context(void)
{
	int ret = SUIT_SUCCESS;

	ret = suit_decoder_decode_envelope(&state, sample_signed_envelope, sizeof(sample_signed_envelope));
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The envelope decoding did not fail on uninitialized context");

	ret = suit_decoder_check_manifest_digest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The manifest digest check did not fail on uninitialized context");

	ret = suit_decoder_decode_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The manifest decoding did not fail on uninitialized context");

	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The manifest authentication did not fail on uninitialized context");

	ret = suit_decoder_authorize_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The manifest authorization did not fail on uninitialized context");

	ret = suit_decoder_decode_sequences(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The sequences decoding did not fail on uninitialized context");

	ret = suit_decoder_create_components(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The components creation did not fail on uninitialized context");
}

/* Decode envelope tests */
void test_decode_envelope_invalid_input(void);
void test_decode_envelope_invalid_state(void);
void test_decode_envelope_minimal(void);
void test_decode_envelope_minimal_incorrect_length(void);
void test_decode_envelope_invalid_input_bytes(void);
void test_deocde_envelope_several_auth_bstrs(void);
void test_deocde_envelope_severed_text_field(void);
void test_deocde_envelope_empty_text_field(void);
void test_deocde_envelope_integrated_payload(void);
void test_deocde_envelope_integrated_payloads(void);

/* Manifest digest check tests */
void test_manifest_digest_invalid_input(void);
void test_manifest_digest_invalid_state(void);
void test_manifest_digest_minimal(void);
void test_manifest_digest_minimal_sha512(void);
void test_manifest_digest_invalid_input_bytes(void);
void test_manifest_digest_minimal_platform_fail(void);

/* Decode manifest tests */
void test_decode_manifest_invalid_input(void);
void test_decode_manifest_invalid_state(void);
void test_decode_manifest_minimal(void);
void test_decode_manifest_max_components(void);
void test_decode_manifest_invalid_input_bytes(void);
void test_decode_manifest_with_component_id(void);

/* Authenticate manifest tests */
void test_authenticate_manifest_invalid_input(void);
void test_authenticate_manifest_invalid_state(void);
void test_authenticate_unsigned_manifest(void);
void test_authenticate_unsigned_manifest_platform_fail(void);
void test_authenticate_signed_manifest(void);
void test_authenticate_signed_manifest_eddsa(void);
void test_authenticate_signed_manifest_invalid_input_bytes(void);
void test_authenticate_signed_manifest_platform_fail(void);
void test_authenticate_signed_manifest_with_key(void);
void test_authenticate_signed_manifest_with_32bit_key(void);
void test_authenticate_signed_manifest_with_32bit_key_sha512(void);
void test_authenticate_signed_manifest_with_2keys(void);
void test_authenticate_signed_manifest_with_2keys_first_platform_fail(void);
void test_authenticate_signed_manifest_with_2keys_second_platform_fail(void);
void test_authenticate_signed_manifest_with_2keys_platform_fail(void);

/* Authenticate manifest tests */
void test_authorize_manifest_invalid_input(void);
void test_authorize_manifest_invalid_state(void);
void test_authorize_manifest_minimal(void);
void test_authorize_manifest_minimal_without_common(void);
void test_authorize_manifest_single_component(void);
void test_authorize_manifest_single_component_platform_fail(void);
void test_authorize_manifest_with_null_bstr_component(void);
void test_authorize_manifest_with_4_components(void);
void test_authorize_manifest_with_single_dependency_component(void);
void test_authorize_manifest_with_dependency_with_empty_bstr_as_prefix(void);
void test_authorize_manifest_with_dependency_with_prefix(void);
void test_authorize_manifest_with_invalid_dependency(void);
void test_authorize_manifest_with_4_dependencies(void);
void test_authorize_manifest_minimal_with_dependency(void);

/* Decode sequences tests */
void test_decode_sequences_invalid_input(void);
void test_decode_sequences_invalid_state(void);
void test_decode_sequences_no_sequences(void);
void test_decode_sequences_shared(void);
void test_decode_sequences_invalid_sequence_state(void);
void test_decode_sequences_shared_and_one_sequence(void);
void test_decode_sequences_text_invalid_state(void);
void test_decode_sequences_text_without_digest(void);
void test_decode_sequences_text_with_valid_digest(void);
void test_decode_sequences_text_with_digest_platform_fail(void);
void test_decode_sequences_all(void);
void test_decode_sequences_all_severed(void);

/* Components creation tests */
void test_create_components_invalid_input(void);
void test_create_components_invalid_state(void);
void test_create_components_no_common_seq(void);
void test_create_components_no_components(void);
void test_create_components_manifest_append_dependency_failed(void);
void test_create_components_manifest_append_component_failed(void);
void test_create_components_single_component(void);
void test_create_components_empty_component_id(void);
void test_create_components_duplicated_component(void);
void test_create_components_invalid_dependency_prefix(void);
void test_create_components_duplicated_semidependency(void);
void test_create_components_duplicated_dependency_with_two_prefixes(void);
void test_create_components_dependency_with_empty_bstr_as_prefix(void);
void test_create_components_component_len_23(void);
void test_create_components_component_len_24(void);
void test_create_components_component_len_255(void);
void test_create_components_component_len_256(void);

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
