/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <suit_types.h>
#include <unity.h>
#include "suit_platform_mock_ext.h"

#define ASSIGNED_COMPONENT_HANDLE 0x2E054000

extern uint8_t manifest_buf[];
extern const size_t manifest_len;

extern suit_component_t candidate_component_handle;
extern suit_component_t radio_component_handle;
suit_component_t radio_fw_component_handle = ASSIGNED_COMPONENT_HANDLE;
suit_component_t radio_fw_memptr_component_handle = ASSIGNED_COMPONENT_HANDLE + 1;

static uint8_t cid_uuid[] = {
	0x81, 0x6A, 0xA0, 0xA0, 0xAF, 0x11, 0x5E, 0xF2,
	0x85, 0x8A, 0xFE, 0xB6, 0x68, 0xB2, 0xE9, 0xC9,
};
struct zcbor_string exp_radio_cid_uuid = {
	.value = cid_uuid,
	.len = sizeof(cid_uuid),
};

static uint8_t vid_uuid[] = {
	0x76, 0x17, 0xDA, 0xA5, 0x71, 0xFD, 0x5A, 0x85,
	0x8F, 0x94, 0xE2, 0x8D, 0x73, 0x5C, 0xE9, 0xF4,
};
struct zcbor_string exp_radio_vid_uuid = {
	.value = vid_uuid,
	.len = sizeof(vid_uuid),
};

static struct zcbor_string signature = {
	.value = &(manifest_buf[687]),
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
			0xE6, 0x34, 0x39, 0x3D, 0x82, 0x64, 0x93, 0xF4,
			0x2C, 0x42, 0xCA, 0x81, 0x08, 0x82, 0x7E, 0xF6,
			0x10, 0x31, 0x47, 0x85, 0xB0, 0xA0, 0xAF, 0xAE,
			0xF4, 0x85, 0xA5, 0x05, 0x14, 0xE8, 0x38, 0x56,
};
static struct zcbor_string exp_signature = {
	.value = signature1_cbor,
	.len = sizeof(signature1_cbor),
};

static uint8_t manifest_digest[] = {
	0xE6, 0x34, 0x39, 0x3D, 0x82, 0x64, 0x93, 0xF4,
	0x2C, 0x42, 0xCA, 0x81, 0x08, 0x82, 0x7E, 0xF6,
	0x10, 0x31, 0x47, 0x85, 0xB0, 0xA0, 0xAF, 0xAE,
	0xF4, 0x85, 0xA5, 0x05, 0x14, 0xE8, 0x38, 0x56,
};
struct zcbor_string exp_radio_manifest_digest = {
	.value = manifest_digest,
	.len = sizeof(manifest_digest),
};

struct zcbor_string exp_radio_envelope_payload = {
	.value = &manifest_buf[630],
	.len = 568,
};
struct zcbor_string exp_radio_manifest_payload = {
	.value = &manifest_buf[752],
	.len = 177,
};
struct zcbor_string exp_radio_fw_payload = {
	.value = &manifest_buf[942],
	.len = 256,
};

struct zcbor_string exp_radio_manifest_id = {
	.value = NULL,
	.len = 0,
};

static uint8_t image_digest[] = {
	0x5F, 0xC3, 0x54, 0xBF, 0x8E, 0x8C, 0x50, 0xFB,
	0x4F, 0xBC, 0x2C, 0xFA, 0xEB, 0x04, 0x53, 0x41,
	0xC9, 0x80, 0x6D, 0xEA, 0xBD, 0xCB, 0x41, 0x54,
	0xFB, 0x79, 0xCC, 0xA4, 0xF0, 0xC9, 0x8C, 0x12,
};
struct zcbor_string exp_radio_image_digest = {
	.value = image_digest,
	.len = sizeof(image_digest),
};

static uint8_t radio_fw_id[] = {
	0x84, // SUIT_Component_Identifier: array(4)
		0x41, // bstr: bytes(1)
			'M',
		0x41, // bstr: bytes(1)
			0x03, // RADIO core ID
		0x45, // bstr: bytes(5)
			0x1A, 0x0E, 0x05, 0x40, 0x00, // uint32: start address
		0x45, // bstr: bytes(5)
			0x1A, 0x00, 0x05, 0x58, 0x00, // uint32: size
};
static struct zcbor_string exp_radio_fw_id = {
	.value = radio_fw_id,
	.len = sizeof(radio_fw_id),
};

static uint8_t fw_memptr_id[] = {
	0x82, // SUIT_Component_Identifier: array(2)
		0x41, // bstr: bytes(1)
			'D',
		0x41, // bstr: bytes(1)
			0x00,
};
static struct zcbor_string exp_fw_memptr_id = {
	.value = fw_memptr_id,
	.len = sizeof(fw_memptr_id),
};


void radio_assert_envelope_integrity(bool installed)
{
	suit_component_t checked_component = installed ? radio_component_handle : candidate_component_handle;

	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&(signature1_cbor[23]), manifest_digest, sizeof(manifest_digest), "Please fix the test: radio manifest digest inside signature structure is incorrect");

	/* read manifest from the candidate component data */
	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(checked_component, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_radio_envelope_payload.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_radio_envelope_payload.len);

	/* authorization */
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_radio_manifest_digest, &exp_radio_manifest_payload, SUIT_SUCCESS);
	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&exp_radio_manifest_id, suit_cose_es256, NULL, &signature, &exp_signature, SUIT_SUCCESS);

	/* component creation */
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_radio_manifest_id, &exp_radio_fw_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_radio_fw_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&radio_fw_component_handle);

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_radio_manifest_id, &exp_fw_memptr_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_fw_memptr_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&radio_fw_memptr_component_handle);

	/* component deletion */
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(radio_fw_component_handle, SUIT_SUCCESS);
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(radio_fw_memptr_component_handle, SUIT_SUCCESS);
}

void radio_assert_envelope_authorization(bool installed)
{
	suit_component_t checked_component = installed ? radio_component_handle : candidate_component_handle;

	/* read manifest from the candidate component data */
	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(checked_component, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_radio_envelope_payload.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_radio_envelope_payload.len);

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_radio_manifest_digest, &exp_radio_manifest_payload, SUIT_SUCCESS);
	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&exp_radio_manifest_id, suit_cose_es256, NULL, &signature, &exp_signature, SUIT_SUCCESS);
}

void radio_assert_component_creation(void)
{
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_radio_manifest_id, &exp_radio_fw_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_radio_fw_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&radio_fw_component_handle);

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_radio_manifest_id, &exp_fw_memptr_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_fw_memptr_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&radio_fw_memptr_component_handle);
}

void radio_assert_component_deletion(void)
{
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(radio_fw_component_handle, SUIT_SUCCESS);
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(radio_fw_memptr_component_handle, SUIT_SUCCESS);
}
