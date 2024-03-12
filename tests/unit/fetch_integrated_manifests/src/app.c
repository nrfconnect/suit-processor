/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <suit_types.h>
#include <unity.h>
#include "suit_platform_mock_ext.h"

#define ASSIGNED_COMPONENT_HANDLE 0x3E054000

extern uint8_t manifest_buf[];
extern const size_t manifest_len;

extern suit_component_t candidate_component_handle;
extern suit_component_t app_component_handle;
suit_component_t app_fw_component_handle = ASSIGNED_COMPONENT_HANDLE;
suit_component_t app_fw_memptr_component_handle = ASSIGNED_COMPONENT_HANDLE + 1;

static uint8_t cid_uuid[] = {
	0x08, 0xC1, 0xB5, 0x99, 0x55, 0xE8, 0x5F, 0xBC,
	0x9E, 0x76, 0x7B, 0xC2, 0x9C, 0xE1, 0xB0, 0x4D,
};
struct zcbor_string exp_app_cid_uuid = {
	.value = cid_uuid,
	.len = sizeof(cid_uuid),
};

static uint8_t vid_uuid[] = {
	0x76, 0x17, 0xDA, 0xA5, 0x71, 0xFD, 0x5A, 0x85,
	0x8F, 0x94, 0xE2, 0x8D, 0x73, 0x5C, 0xE9, 0xF4,
};
struct zcbor_string exp_app_vid_uuid = {
	.value = vid_uuid,
	.len = sizeof(vid_uuid),
};

static struct zcbor_string signature = {
	.value = &(manifest_buf[1268]),
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
			0x3E, 0xC1, 0x59, 0x55, 0x7F, 0x09, 0x37, 0xFE,
			0x12, 0x6E, 0x02, 0x8A, 0x99, 0xFA, 0x51, 0x20,
			0x60, 0x2B, 0xDD, 0xC5, 0x2A, 0xBE, 0x32, 0x70,
			0x57, 0x37, 0xF9, 0x26, 0x36, 0x6A, 0xA6, 0x05,
};
static struct zcbor_string exp_signature = {
	.value = signature1_cbor,
	.len = sizeof(signature1_cbor),
};

static uint8_t manifest_digest[] = {
	0x3E, 0xC1, 0x59, 0x55, 0x7F, 0x09, 0x37, 0xFE,
	0x12, 0x6E, 0x02, 0x8A, 0x99, 0xFA, 0x51, 0x20,
	0x60, 0x2B, 0xDD, 0xC5, 0x2A, 0xBE, 0x32, 0x70,
	0x57, 0x37, 0xF9, 0x26, 0x36, 0x6A, 0xA6, 0x05,
};
struct zcbor_string exp_app_manifest_digest = {
	.value = manifest_digest,
	.len = sizeof(manifest_digest),
};

struct zcbor_string exp_app_envelope_payload = {
	.value = &manifest_buf[1211],
	.len = 568,
};
struct zcbor_string exp_app_manifest_payload = {
	.value = &manifest_buf[1333],
	.len = 177,
};
struct zcbor_string exp_app_fw_payload = {
	.value = &manifest_buf[1523],
	.len = 256,
};

struct zcbor_string exp_app_manifest_id = {
	.value = NULL,
	.len = 0,
};

static uint8_t image_digest[] = {
	0x5F, 0xC3, 0x54, 0xBF, 0x8E, 0x8C, 0x50, 0xFB,
	0x4F, 0xBC, 0x2C, 0xFA, 0xEB, 0x04, 0x53, 0x41,
	0xC9, 0x80, 0x6D, 0xEA, 0xBD, 0xCB, 0x41, 0x54,
	0xFB, 0x79, 0xCC, 0xA4, 0xF0, 0xC9, 0x8C, 0x12,
};
struct zcbor_string exp_app_image_digest = {
	.value = image_digest,
	.len = sizeof(image_digest),
};

static uint8_t app_fw_id[] = {
	0x84, // SUIT_Component_Identifier: array(4)
		0x41, // bstr: bytes(1)
			'M',
		0x41, // bstr: bytes(1)
			0x02, // APPLICATION core ID
		0x45, // bstr: bytes(5)
			0x1A, 0x0E, 0x0A, 0xA0, 0x00, // uint32: start address
		0x45, // bstr: bytes(5)
			0x1A, 0x00, 0x07, 0xF8, 0x00, // uint32: size
};
static struct zcbor_string exp_app_fw_id = {
	.value = app_fw_id,
	.len = sizeof(app_fw_id),
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


void app_assert_envelope_integrity(bool installed)
{
	suit_component_t checked_component = installed ? app_component_handle : candidate_component_handle;

	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&(signature1_cbor[23]), manifest_digest, sizeof(manifest_digest), "Please fix the test: application manifest digest inside signature structure is incorrect");

	/* read manifest from the candidate component data */
	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(checked_component, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_app_envelope_payload.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_app_envelope_payload.len);

	/* authorization */
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_app_manifest_digest, &exp_app_manifest_payload, SUIT_SUCCESS);
	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&exp_app_manifest_id, suit_cose_es256, NULL, &signature, &exp_signature, SUIT_SUCCESS);

	/* component creation */
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_app_manifest_id, &exp_app_fw_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_app_fw_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&app_fw_component_handle);

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_app_manifest_id, &exp_fw_memptr_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_fw_memptr_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&app_fw_memptr_component_handle);

	/* component deletion */
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(app_fw_component_handle, SUIT_SUCCESS);
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(app_fw_memptr_component_handle, SUIT_SUCCESS);
}

void app_assert_envelope_authorization(bool installed)
{
	suit_component_t checked_component = installed ? app_component_handle : candidate_component_handle;

	/* read manifest from the candidate component data */
	__cmock_suit_plat_retrieve_manifest_ExpectAndReturn(checked_component, NULL, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_str();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_str(&exp_app_envelope_payload.value);
	__cmock_suit_plat_retrieve_manifest_IgnoreArg_envelope_len();
	__cmock_suit_plat_retrieve_manifest_ReturnThruPtr_envelope_len(&exp_app_envelope_payload.len);

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_app_manifest_digest, &exp_app_manifest_payload, SUIT_SUCCESS);
	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&exp_app_manifest_id, suit_cose_es256, NULL, &signature, &exp_signature, SUIT_SUCCESS);
}

void app_assert_component_creation(void)
{
	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_app_manifest_id, &exp_app_fw_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_app_fw_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&app_fw_component_handle);

	__cmock_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_app_manifest_id, &exp_fw_memptr_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_fw_memptr_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&app_fw_memptr_component_handle);
}

void app_assert_component_deletion(void)
{
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(app_fw_component_handle, SUIT_SUCCESS);
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(app_fw_memptr_component_handle, SUIT_SUCCESS);
}
