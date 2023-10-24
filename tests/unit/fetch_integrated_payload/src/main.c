/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include <stdint.h>
#include "suit.h"
#include "suit_platform/cmock_suit_platform.h"
#include "suit_platform_mock_ext.h"

#define ASSIGNED_COMPONENT_HANDLE 0x1E054000


extern uint8_t manifest_buf[];
extern const size_t manifest_len;


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
			0xAD, 0xD7, 0xDD, 0x3E, 0x37, 0x4D, 0x38, 0xF3,
			0x8A, 0x7E, 0x4F, 0xF2, 0x60, 0x12, 0x42, 0xAA,
			0x2D, 0xF2, 0x46, 0x3B, 0x8F, 0xEC, 0xA3, 0x60,
			0xEA, 0x37, 0x5F, 0x50, 0xEA, 0xB3, 0xBF, 0x7D,
};
static struct zcbor_string exp_signature = {
	.value = signature1_cbor,
	.len = sizeof(signature1_cbor),
};

static uint8_t manifest_digest[] = {
	0xAD, 0xD7, 0xDD, 0x3E, 0x37, 0x4D, 0x38, 0xF3,
	0x8A, 0x7E, 0x4F, 0xF2, 0x60, 0x12, 0x42, 0xAA,
	0x2D, 0xF2, 0x46, 0x3B, 0x8F, 0xEC, 0xA3, 0x60,
	0xEA, 0x37, 0x5F, 0x50, 0xEA, 0xB3, 0xBF, 0x7D,
};
static struct zcbor_string exp_manifest_digest = {
	.value = manifest_digest,
	.len = sizeof(manifest_digest),
};
static struct zcbor_string exp_manifest_payload = {
	.value = &(manifest_buf[122]),
	.len = 176,
};

static struct zcbor_string exp_manifest_id = {
	.value = NULL,
	.len = 0,
};

static uint8_t vid_uuid[] = {
	0x76, 0x17, 0xDA, 0xA5, 0x71, 0xFD, 0x5A, 0x85, /* RFC4122_UUID(nordicsemi.com) */
	0x8F, 0x94, 0xE2, 0x8D, 0x73, 0x5C, 0xE9, 0xF4,
};
static struct zcbor_string exp_vid_uuid = {
	.value = vid_uuid,
	.len = sizeof(vid_uuid),
};

static uint8_t cid_uuid[] = {
	0xD6, 0x22, 0xBA, 0xFD, 0x43, 0x37, 0x51, 0x85,
	0x90, 0xBC, 0x63, 0x68, 0xCD, 0xA7, 0xFB, 0xCA,
};
static struct zcbor_string exp_cid_uuid = {
	.value = cid_uuid,
	.len = sizeof(cid_uuid),
};

static uint8_t image_digest[] = {
	0x5F, 0xC3, 0x54, 0xBF, 0x8E, 0x8C, 0x50, 0xFB,
	0x4F, 0xBC, 0x2C, 0xFA, 0xEB, 0x04, 0x53, 0x41,
	0xC9, 0x80, 0x6D, 0xEA, 0xBD, 0xCB, 0x41, 0x54,
	0xFB, 0x79, 0xCC, 0xA4, 0xF0, 0xC9, 0x8C, 0x12,
};
static struct zcbor_string exp_image_digest = {
	.value = image_digest,
	.len = sizeof(image_digest),
};
static struct zcbor_string exp_image_payload = {
	.value = &manifest_buf[451],
	.len = 256,
};

static uint8_t text_digest[] = {
	0x4E, 0xDC, 0x09, 0xC1, 0x4D, 0x19, 0xF1, 0x56,
	0x0C, 0x9A, 0xCE, 0x62, 0x64, 0xA5, 0x3D, 0x86,
	0xF8, 0x90, 0x73, 0x70, 0x49, 0x94, 0x63, 0x48,
	0x77, 0x00, 0x7F, 0x1E, 0x04, 0x27, 0x2E, 0xE5,
};
static struct zcbor_string exp_text_digest = {
	.value = text_digest,
	.len = sizeof(text_digest),
};
static struct zcbor_string exp_text_payload = {
	.value = &(manifest_buf[299]),
	.len = 140,
};



static void assert_envelope_authorization(void)
{
	/* The envelope authorization should:
	 * - Verify that the manifest digest matches with the manifest contents
	 * - Verify the manifest signature
	 * - Verify the severable fields digest
	 */
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_manifest_digest, &exp_manifest_payload, SUIT_SUCCESS);
	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&exp_manifest_id, suit_cose_es256, NULL, &signature, &exp_signature, SUIT_SUCCESS);
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_text_digest, &exp_text_payload, SUIT_SUCCESS);
}

static void assert_component_creation(void)
{
	static suit_component_t component_handle = ASSIGNED_COMPONENT_HANDLE;
	static uint8_t app_id[] = {
		0x82, // SUIT_Component_Identifier: array(2)
			0x41, // bstr: bytes(1)
				'X',
			0x44, // bstr: bytes(4)
				0x1E, 0x05, 0x40, 0x00,
	};
	static struct zcbor_string exp_component_id = {
		.value = app_id,
		.len = sizeof(app_id),
	};

	__cmock_suit_plat_suit_plat_authorize_component_id_ExpectComplexArgsAndReturn(&exp_manifest_id, &exp_component_id, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_ExpectComplexArgsAndReturn(&exp_component_id, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_create_component_handle_IgnoreArg_handle();
	__cmock_suit_plat_create_component_handle_ReturnThruPtr_handle(&component_handle);

	/* clean-up */
	__cmock_suit_plat_release_component_handle_ExpectAndReturn(component_handle, SUIT_SUCCESS);
}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
static void assert_dry_run_execution(void)
{
	__cmock_suit_plat_check_fetch_integrated_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_image_payload, SUIT_SUCCESS);
	__cmock_suit_plat_check_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */


void setUp(void)
{
	int ret = suit_processor_init();
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to initialize SUIT processor");

	TEST_ASSERT_EQUAL_MEMORY_MESSAGE(&(signature1_cbor[23]), manifest_digest, sizeof(manifest_digest), "Please fix the test: manifest digest inside signature structure is incorrect");
}

void test_suit_decode_envelope(void)
{
	struct zcbor_string manifest_component_id;
	struct zcbor_string digest;
	enum suit_cose_alg alg;
	unsigned int seq_num;

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_manifest_digest, &exp_manifest_payload, SUIT_SUCCESS);

	/* The envelope decoding is fully handled by the ZCBOR code and does not call platform APIs */
	int ret = suit_processor_get_manifest_metadata(
		manifest_buf, manifest_len, false,
		&manifest_component_id, &digest, &alg, &seq_num);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to decode input manifest");
	TEST_ASSERT_EQUAL_MESSAGE(1, seq_num, "Unexpected sequence number value");
	TEST_ASSERT_EQUAL_MESSAGE(suit_cose_sha256, alg, "Unexpected digest algorithm ID value");
	assert_zcbor_string(&exp_manifest_id, &manifest_component_id);
	assert_zcbor_string(&exp_manifest_digest, &digest);
}

void test_suit_process_seq_invald(void)
{
	/* SUIT_SEQ_INVALID is not a valid step - it serves as a boundary value of the enum. */
	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_INVALID);
	TEST_ASSERT_EQUAL(SUIT_ERR_UNAVAILABLE_COMMAND_SEQ, err);
}

void test_suit_process_seq_parse(void)
{
	assert_envelope_authorization();
	assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_SEQ_PARSE, &exp_manifest_id, 1, SUIT_SUCCESS);

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_PARSE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, err);
}

void test_suit_process_seq_payload_fetch(void)
{
	/* SUIT_SEQ_PAYLOAD_FETCH command sequence is not present in the sample manifest.
	 * The internal payload is fetched to the executable slot by executing FETCH command
	 * from the INSTALL step.
	 */
	assert_envelope_authorization();
	assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &exp_manifest_id, 1, SUIT_SUCCESS);

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_PAYLOAD_FETCH);
	TEST_ASSERT_EQUAL(SUIT_ERR_UNAVAILABLE_COMMAND_SEQ, err);
}

static int authorize_sequence_num_callback(enum suit_command_sequence seq_name, struct zcbor_string *manifest_component_id, unsigned int seq_num, int cmock_num_calls)
{
	static uint8_t minimal_manifest[] = {
		0xd8, 0x6b, 0xa2, 0x02, 0x58, 0x7a, 0x82, 0x58, 0x24, 0x82, 0x2f, 0x58, 0x20, 0xfd, 0xf6,
		0x82, 0x8c, 0x9a, 0xd7, 0xf8, 0x57, 0xc5, 0x66, 0xaa, 0xd3, 0x4c, 0xc1, 0x7c, 0x28, 0xb4,
		0x5a, 0x07, 0x90, 0xf2, 0xa0, 0x06, 0x37, 0x67, 0x26, 0xf2, 0xf4, 0x50, 0x81, 0xb6, 0x37,
		0x58, 0x51, 0xd2, 0x84, 0x4a, 0xa2, 0x01, 0x26, 0x04, 0x45, 0x1a, 0x7f, 0xff, 0xff, 0xe0,
		0xa0, 0xf6, 0x58, 0x40, 0x34, 0x7b, 0x7c, 0xac, 0x27, 0x3e, 0x72, 0xb7, 0xc0, 0x0d, 0x7b,
		0x0d, 0x35, 0x33, 0xfa, 0xb4, 0x4b, 0xb4, 0x45, 0x8c, 0xe5, 0xd4, 0x51, 0xe0, 0xeb, 0x4b,
		0x27, 0xa7, 0xd2, 0x3f, 0x69, 0x63, 0x7f, 0xdc, 0x03, 0x52, 0xd2, 0x1f, 0x79, 0x0c, 0x61,
		0x9c, 0xa1, 0xb8, 0x18, 0x91, 0xc0, 0x97, 0x0f, 0xb5, 0x5c, 0xdf, 0x14, 0x92, 0xbd, 0x92,
		0x66, 0xba, 0xa4, 0xe0, 0x35, 0x84, 0x5c, 0xd0, 0x03, 0x58, 0x19, 0xa3, 0x01, 0x01, 0x02,
		0x0c, 0x03, 0x52, 0xa1, 0x02, 0x81, 0x84, 0x41, 0x4d, 0x41, 0x00, 0x45, 0x1a, 0x00, 0x08,
		0x00, 0x00, 0x43, 0x19, 0x10, 0x00
	};
	/* It must be possible to decode a different manifest from the sequence completion callback. */
	unsigned int seq_num_minimal;

	static uint8_t minimal_manifest_digest[] = {
		0xfd, 0xf6, 0x82, 0x8c, 0x9a, 0xd7, 0xf8, 0x57,
		0xc5, 0x66, 0xaa, 0xd3, 0x4c, 0xc1, 0x7c, 0x28,
		0xb4, 0x5a, 0x07, 0x90, 0xf2, 0xa0, 0x06, 0x37,
		0x67, 0x26, 0xf2, 0xf4, 0x50, 0x81, 0xb6, 0x37,
	};
	static struct zcbor_string exp_minimal_manifest_digest = {
		.value = minimal_manifest_digest,
		.len = sizeof(minimal_manifest_digest),
	};
	static struct zcbor_string exp_minimal_manifest_payload = {
		.value = &minimal_manifest[129],
		.len = 27,
	};
	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_minimal_manifest_digest, &exp_minimal_manifest_payload, SUIT_SUCCESS);

	/* The envelope decoding is fully handled by the ZCBOR code and does not call platform APIs */
	int ret = suit_processor_get_manifest_metadata(
		minimal_manifest, sizeof(minimal_manifest), false,
		NULL, NULL, NULL, &seq_num_minimal);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to decode input manifest");
	TEST_ASSERT_EQUAL_MESSAGE(12, seq_num_minimal, "Unexpected sequence number value");

	return SUIT_SUCCESS;
}


void test_suit_process_seq_install(void)
{
	/* SUIT_SEQ_INSTALL should:
	 * - execute the shared sequence (VID and CID checks),
	 * - copy the integrated payload into executable slot through FETCH (integrated) command
	 * - verify the image digest in the executable slot
	 */
	assert_envelope_authorization();
	assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_SEQ_INSTALL, &exp_manifest_id, 1, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_AddCallback(authorize_sequence_num_callback);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, 256, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_image_payload, SUIT_SUCCESS);
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, suit_cose_sha256, &exp_image_digest, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_SEQ_INSTALL, &exp_manifest_id, manifest_buf, manifest_len, SUIT_SUCCESS);

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_INSTALL);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, err);
}

void test_suit_process_seq_validate(void)
{
	/* SUIT_SEQ_VALIDATE should:
	 * - execute the shared sequence (VID and CID checks)
	 * - verify the image digest in the executable slot.
	 */
	assert_envelope_authorization();
	assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_SEQ_VALIDATE, &exp_manifest_id, 1, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, 256, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, suit_cose_sha256, &exp_image_digest, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_SEQ_VALIDATE, &exp_manifest_id, manifest_buf, manifest_len, SUIT_SUCCESS);

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_VALIDATE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, err);
}

void test_suit_process_seq_load(void)
{
	/* SUIT_SEQ_LOAD command sequence is not present in the sample manifest. */
	assert_envelope_authorization();
	assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_SEQ_LOAD, &exp_manifest_id, 1, SUIT_SUCCESS);

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_LOAD);
	TEST_ASSERT_EQUAL(SUIT_ERR_UNAVAILABLE_COMMAND_SEQ, err);
}

void test_suit_process_seq_invoke(void)
{
	/* SUIT_SEQ_INVOKE should:
	 * - execute the shared sequence (VID and CID checks)
	 * - execute the INVOKE command.
	 * The image validity is expected to be checked in the SUIT_VALIDATE step.
	 */
	assert_envelope_authorization();
	assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectAndReturn(SUIT_SEQ_INVOKE, &exp_manifest_id, 1, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, 256, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(ASSIGNED_COMPONENT_HANDLE, &exp_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_invoke_ExpectAndReturn(ASSIGNED_COMPONENT_HANDLE, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectAndReturn(SUIT_SEQ_INVOKE, &exp_manifest_id, manifest_buf, manifest_len, SUIT_SUCCESS);

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_INVOKE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, err);
}

void test_suit_process_seq_max(void)
{
	/* SUIT_SEQ_MAX is not a valid step - it serves as a boundary value of the enum. */
	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_MAX);
	TEST_ASSERT_EQUAL(SUIT_ERR_UNAVAILABLE_COMMAND_SEQ, err);
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
