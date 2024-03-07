/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <suit_types.h>
#include <unity.h>
#include "suit_platform_mock_ext.h"

#define ASSIGNED_COMPONENT_HANDLE 0x1E054000

extern uint8_t manifest_buf[];
extern const size_t manifest_len;

suit_component_t candidate_component_handle = ASSIGNED_COMPONENT_HANDLE;
suit_component_t radio_component_handle = ASSIGNED_COMPONENT_HANDLE + 1;
suit_component_t app_component_handle = ASSIGNED_COMPONENT_HANDLE + 2;

static struct zcbor_string signature = {
	.value = &(manifest_buf[57]),
	.len = 64,
};
static uint8_t signature1_cbor[] = {
	0x84, // Sig_structure1: array(4)
		0x6A, // context: text(10)
			'S', 'i', 'g', 'n', 'a', 't', 'u', 'r', 'e', '1',
		0x43, // body_protected: bytes(3)
			0xA1, // header_map: map(1)
				0x01, // alg_id: 1
					0x26, // ES256: -7
		0x40, // external_aad: bytes(0)
		0x58, // payload: bytes(36)
			0x24, 0x82, 0x2F, 0x58, 0x20,
			0xD6, 0x8D, 0x7E, 0x78, 0xE5, 0xD6, 0xC2, 0xE2,
			0x46, 0x80, 0x84, 0x29, 0xB1, 0xA2, 0xC7, 0x9D,
			0xA7, 0x52, 0xB3, 0x5C, 0x3E, 0x84, 0x1F, 0xBE,
			0x47, 0x60, 0xA5, 0xC2, 0x7D, 0xE2, 0xE1, 0xD7,
};
static struct zcbor_string exp_signature = {
	.value = signature1_cbor,
	.len = sizeof(signature1_cbor),
};

static uint8_t manifest_digest[] = {
	0xD6, 0x8D, 0x7E, 0x78, 0xE5, 0xD6, 0xC2, 0xE2,
	0x46, 0x80, 0x84, 0x29, 0xB1, 0xA2, 0xC7, 0x9D,
	0xA7, 0x52, 0xB3, 0x5C, 0x3E, 0x84, 0x1F, 0xBE,
	0x47, 0x60, 0xA5, 0xC2, 0x7D, 0xE2, 0xE1, 0xD7,
};
struct zcbor_string exp_root_manifest_digest = {
	.value = manifest_digest,
	.len = sizeof(manifest_digest),
};

struct zcbor_string exp_root_manifest_payload = {
	.value = &(manifest_buf[122]),
	.len = 493,
};

static uint8_t root_component_uuid[] = {
	0x82, 0x41, 0x49, 0x50,
	0x3F, 0x6A, 0x3A, 0x4D, 0xCD, 0xFA, 0x58, 0xC5,
	0xAC, 0xCE, 0xF9, 0xF5, 0x84, 0xC4, 0x11, 0x24,
};
struct zcbor_string exp_root_manifest_id = {
	.value = root_component_uuid,
	.len = sizeof(root_component_uuid),
};


void root_assert_envelope_authorization(void)
{
	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&(signature1_cbor[23]), manifest_digest, sizeof(manifest_digest), "Please fix the test: root manifest digest inside signature structure is incorrect");

	/* The envelope authorization should:
	 * - Verify that the manifest digest matches with the manifest contents
	 * - Verify the manifest signature
	 * - Verify the severable fields digest
	 */
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_root_manifest_digest, &exp_root_manifest_payload, SUIT_SUCCESS);
	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&exp_root_manifest_id, suit_cose_es256, NULL, &signature, &exp_signature, SUIT_SUCCESS);
}

void root_assert_component_creation(void)
{
	static uint8_t manifest_candidate_id[] = {
		0x82, // SUIT_Component_Identifier: array(2)
			0x41, // bstr: bytes(1)
				'C',
			0x41, // bstr: bytes(1)
				0x00,
	};
	static struct zcbor_string exp_manifest_candidate_id = {
		.value = manifest_candidate_id,
		.len = sizeof(manifest_candidate_id),
	};

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_manifest_candidate_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_manifest_candidate_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&candidate_component_handle);

	static uint8_t radio_manifest_id[] = {
		0x82, // SUIT_Component_Identifier: array(2)
			0x41, // bstr: bytes(1)
				'I',
			0x50, // bstr: bytes(16)
				0x81, 0x6A, 0xA0, 0xA0, 0xAF, 0x11, 0x5E, 0xF2,
				0x85, 0x8A, 0xFE, 0xB6, 0x68, 0xB2, 0xE9, 0xC9,
	};
	static struct zcbor_string exp_radio_manifest_id = {
		.value = radio_manifest_id,
		.len = sizeof(radio_manifest_id),
	};

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_radio_manifest_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_radio_manifest_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&radio_component_handle);

	static uint8_t app_manifest_id[] = {
		0x82, // SUIT_Component_Identifier: array(2)
			0x41, // bstr: bytes(1)
				'I',
			0x50, // bstr: bytes(16)
				0x08, 0xC1, 0xB5, 0x99, 0x55, 0xE8, 0x5F, 0xBC,
				0x9E, 0x76, 0x7B, 0xC2, 0x9C, 0xE1, 0xB0, 0x4D,
	};
	static struct zcbor_string exp_app_manifest_id = {
		.value = app_manifest_id,
		.len = sizeof(app_manifest_id),
	};

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_app_manifest_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_app_manifest_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&app_component_handle);
}

void root_assert_component_deletion(void)
{
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(candidate_component_handle, SUIT_SUCCESS);
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(radio_component_handle, SUIT_SUCCESS);
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(app_component_handle, SUIT_SUCCESS);
}
