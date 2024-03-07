/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <unity.h>
#include "suit.h"
#include "suit_platform/cmock_suit_platform.h"
#include "suit_platform_mock_ext.h"


extern uint8_t manifest_buf[];
extern const size_t manifest_len;

/* Definitions from root.c */
extern struct zcbor_string exp_root_manifest_digest;
extern struct zcbor_string exp_root_manifest_payload;
extern struct zcbor_string exp_root_manifest_id;
extern suit_component_t candidate_component_handle;
extern suit_component_t radio_component_handle;
extern suit_component_t app_component_handle;
void root_assert_envelope_authorization(void);
void root_assert_component_creation(void);
void root_assert_component_deletion(void);

/* Definitions from radio.c */
extern struct zcbor_string exp_radio_vid_uuid;
extern struct zcbor_string exp_radio_cid_uuid;
extern struct zcbor_string exp_radio_envelope_payload;
extern struct zcbor_string exp_radio_fw_payload;
extern struct zcbor_string exp_radio_image_digest;
extern struct zcbor_string exp_radio_manifest_digest;
extern struct zcbor_string exp_radio_manifest_payload;
extern struct zcbor_string exp_radio_manifest_id;
extern suit_component_t radio_fw_component_handle;
extern suit_component_t radio_fw_memptr_component_handle;
void radio_assert_envelope_integrity(bool installed);
void radio_assert_envelope_authorization(bool installed);
void radio_assert_component_creation(void);
void radio_assert_component_deletion(void);

/* Definitions from app.c */
extern struct zcbor_string exp_app_vid_uuid;
extern struct zcbor_string exp_app_cid_uuid;
extern struct zcbor_string exp_app_envelope_payload;
extern struct zcbor_string exp_app_fw_payload;
extern struct zcbor_string exp_app_image_digest;
extern struct zcbor_string exp_app_manifest_digest;
extern struct zcbor_string exp_app_manifest_payload;
extern struct zcbor_string exp_app_manifest_id;
extern suit_component_t app_fw_component_handle;
extern suit_component_t app_fw_memptr_component_handle;
void app_assert_envelope_integrity(bool installed);
void app_assert_envelope_authorization(bool installed);
void app_assert_component_creation(void);
void app_assert_component_deletion(void);


#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
static void root_assert_dry_run_execution(void)
{
	/* SUIT_PLATFORM_FETCH dry-run */
	/* Fetch the dependency manifest (radio) */
	__cmock_suit_plat_check_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_radio_envelope_payload, SUIT_SUCCESS);
	/* Fetch the dependency manifest (application) */
	__cmock_suit_plat_check_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_app_envelope_payload, SUIT_SUCCESS);

	/* SUIT_INSTALL dry-run */
	/* Fetch the dependency manifest (radio) */
	__cmock_suit_plat_check_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_radio_envelope_payload, SUIT_SUCCESS);
	/* Fetch the dependency manifest (application) */
	__cmock_suit_plat_check_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_app_envelope_payload, SUIT_SUCCESS);
}
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */


void setUp(void)
{
	int ret = suit_processor_init();
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to initialize SUIT processor");
}

void test_suit_decode_envelope(void)
{
	struct zcbor_string manifest_component_id;
	struct zcbor_string digest;
	enum suit_cose_alg alg;
	unsigned int seq_num;

	__cmock_suit_plat_check_digest_ExpectComplexArgsAndReturn(suit_cose_sha256, &exp_root_manifest_digest, &exp_root_manifest_payload, SUIT_SUCCESS);

	/* The envelope decoding is fully handled by the ZCBOR code and does not call platform APIs */
	int ret = suit_processor_get_manifest_metadata(
		manifest_buf, manifest_len, false,
		&manifest_component_id, &digest, &alg, &seq_num);

	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "Unable to decode input manifest");
	TEST_ASSERT_EQUAL_MESSAGE(1, seq_num, "Unexpected sequence number value");
	TEST_ASSERT_EQUAL_MESSAGE(suit_cose_sha256, alg, "Unexpected digest algorithm ID value");
	assert_zcbor_string(&exp_root_manifest_id, &manifest_component_id);
	assert_zcbor_string(&exp_root_manifest_digest, &digest);
}

void test_suit_process_seq_invald(void)
{
	/* SUIT_SEQ_INVALID is not a valid step - it serves as a boundary value of the enum. */
	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_INVALID);
	TEST_ASSERT_EQUAL(SUIT_ERR_UNAVAILABLE_COMMAND_SEQ, err);
}

void test_suit_process_seq_parse(void)
{
	root_assert_envelope_authorization();
	root_assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	root_assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PARSE, &exp_root_manifest_id, 1, SUIT_SUCCESS);
	root_assert_component_deletion();

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_PARSE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, err);
}

void test_suit_process_seq_dependency_resolution(void)
{
	/* SUIT_SEQ_DEP_RESOLUTION command sequence is not present in the sample manifest.
	 */
	root_assert_envelope_authorization();
	root_assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	root_assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_DEP_RESOLUTION, &exp_root_manifest_id, 1, SUIT_SUCCESS);

	/* SUIT_COMMMON sequence from the root manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_component_handle, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_component_handle, exp_app_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);

	/* Fetch the dependency manifest (radio) */
	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_radio_envelope_payload, SUIT_SUCCESS);

	/* Execute radio manifest */
	radio_assert_envelope_integrity(false);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_radio_manifest_id, SUIT_SEQ_DEP_RESOLUTION, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_DEP_RESOLUTION, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	radio_assert_envelope_authorization(false);
	radio_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_DEP_RESOLUTION, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the radio manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_memptr_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_DEP_RESOLUTION, &exp_radio_manifest_id, exp_radio_envelope_payload.value, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	radio_assert_component_deletion();

	/* Fetch the dependency manifest (application) */
	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_app_envelope_payload, SUIT_SUCCESS);

	/* Execute application manifest */
	app_assert_envelope_integrity(false);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_app_manifest_id, SUIT_SEQ_DEP_RESOLUTION, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_DEP_RESOLUTION, &exp_app_manifest_id, 1, SUIT_SUCCESS);
	app_assert_envelope_authorization(false);
	app_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_DEP_RESOLUTION, &exp_app_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the application manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_memptr_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_DEP_RESOLUTION, &exp_app_manifest_id, exp_app_envelope_payload.value, exp_app_envelope_payload.len, SUIT_SUCCESS);
	app_assert_component_deletion();

	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_DEP_RESOLUTION, &exp_root_manifest_id, manifest_buf, manifest_len, SUIT_SUCCESS);
	root_assert_component_deletion();

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_DEP_RESOLUTION);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, err);
}

void test_suit_process_seq_payload_fetch(void)
{
	root_assert_envelope_authorization();
	root_assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	root_assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &exp_root_manifest_id, 1, SUIT_SUCCESS);

	/* SUIT_COMMMON sequence from the root manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_component_handle, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_component_handle, exp_app_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);

	/* Fetch the dependency manifest (radio) */
	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_radio_envelope_payload, SUIT_SUCCESS);

	/* Execute radio manifest */
	radio_assert_envelope_integrity(false);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_radio_manifest_id, SUIT_SEQ_PAYLOAD_FETCH, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	radio_assert_envelope_authorization(false);
	radio_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the radio manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_memptr_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &exp_radio_manifest_id, exp_radio_envelope_payload.value, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	radio_assert_component_deletion();

	/* Fetch the dependency manifest (application) */
	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_app_envelope_payload, SUIT_SUCCESS);

	/* Execute application manifest */
	app_assert_envelope_integrity(false);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_app_manifest_id, SUIT_SEQ_PAYLOAD_FETCH, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &exp_app_manifest_id, 1, SUIT_SUCCESS);
	app_assert_envelope_authorization(false);
	app_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &exp_app_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the application manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_memptr_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &exp_app_manifest_id, exp_app_envelope_payload.value, exp_app_envelope_payload.len, SUIT_SUCCESS);
	app_assert_component_deletion();

	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_PAYLOAD_FETCH, &exp_root_manifest_id, manifest_buf, manifest_len, SUIT_SUCCESS);
	root_assert_component_deletion();

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_PAYLOAD_FETCH);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, err);
}

void test_suit_process_seq_candidate_verification(void)
{
	/* SUIT_SEQ_CAND_VERIFICATION command sequence is not present in the sample manifest.
	 */
	root_assert_envelope_authorization();
	root_assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	root_assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_CAND_VERIFICATION, &exp_root_manifest_id, 1, SUIT_SUCCESS);

	/* SUIT_COMMMON sequence from the root manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_component_handle, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_component_handle, exp_app_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);

	/* Fetch the dependency manifest (radio) */
	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_radio_envelope_payload, SUIT_SUCCESS);

	/* Execute radio manifest */
	radio_assert_envelope_integrity(false);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_radio_manifest_id, SUIT_SEQ_CAND_VERIFICATION, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_CAND_VERIFICATION, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	radio_assert_envelope_authorization(false);
	radio_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_CAND_VERIFICATION, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the radio manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_memptr_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_CAND_VERIFICATION, &exp_radio_manifest_id, exp_radio_envelope_payload.value, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	radio_assert_component_deletion();

	/* Fetch the dependency manifest (application) */
	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_app_envelope_payload, SUIT_SUCCESS);

	/* Execute application manifest */
	app_assert_envelope_integrity(false);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_app_manifest_id, SUIT_SEQ_CAND_VERIFICATION, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_CAND_VERIFICATION, &exp_app_manifest_id, 1, SUIT_SUCCESS);
	app_assert_envelope_authorization(false);
	app_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_CAND_VERIFICATION, &exp_app_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the application manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_memptr_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_CAND_VERIFICATION, &exp_app_manifest_id, exp_app_envelope_payload.value, exp_app_envelope_payload.len, SUIT_SUCCESS);
	app_assert_component_deletion();

	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_CAND_VERIFICATION, &exp_root_manifest_id, manifest_buf, manifest_len, SUIT_SUCCESS);
	root_assert_component_deletion();

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_CAND_VERIFICATION);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, err);
}

void test_suit_process_seq_install(void)
{
	root_assert_envelope_authorization();
	root_assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	root_assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_INSTALL, &exp_root_manifest_id, 1, SUIT_SUCCESS);

	/* SUIT_COMMMON sequence from the root manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_component_handle, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_component_handle, exp_app_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);

	/* Fetch the dependency manifest (radio) */
	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_radio_envelope_payload, SUIT_SUCCESS);

	/* Execute radio manifest */
	radio_assert_envelope_integrity(false);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_radio_manifest_id, SUIT_SEQ_INSTALL, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_INSTALL, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	radio_assert_envelope_authorization(false);
	radio_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_INSTALL, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the radio manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_memptr_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	/* SUIT_INSTALL sequence from the radio manifest */
	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(radio_fw_memptr_component_handle, &exp_radio_fw_payload, SUIT_SUCCESS);
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(radio_fw_memptr_component_handle, suit_cose_sha256, &exp_radio_image_digest, SUIT_SUCCESS);
	__cmock_suit_plat_copy_ExpectAndReturn(radio_fw_component_handle, radio_fw_memptr_component_handle, SUIT_SUCCESS);
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(radio_fw_component_handle, suit_cose_sha256, &exp_radio_image_digest, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_INSTALL, &exp_radio_manifest_id, exp_radio_envelope_payload.value, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	radio_assert_component_deletion();

	/* Fetch the dependency manifest (application) */
	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(candidate_component_handle, &exp_app_envelope_payload, SUIT_SUCCESS);

	/* Execute application manifest */
	app_assert_envelope_integrity(false);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_app_manifest_id, SUIT_SEQ_INSTALL, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_INSTALL, &exp_app_manifest_id, 1, SUIT_SUCCESS);
	app_assert_envelope_authorization(false);
	app_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_INSTALL, &exp_app_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the application manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_memptr_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);
	/* SUIT_INSTALL sequence from the application manifest */
	__cmock_suit_plat_fetch_integrated_ExpectComplexArgsAndReturn(app_fw_memptr_component_handle, &exp_app_fw_payload, SUIT_SUCCESS);
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(app_fw_memptr_component_handle, suit_cose_sha256, &exp_app_image_digest, SUIT_SUCCESS);
	__cmock_suit_plat_copy_ExpectAndReturn(app_fw_component_handle, app_fw_memptr_component_handle, SUIT_SUCCESS);
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(app_fw_component_handle, suit_cose_sha256, &exp_app_image_digest, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_INSTALL, &exp_app_manifest_id, exp_app_envelope_payload.value, exp_app_envelope_payload.len, SUIT_SUCCESS);
	app_assert_component_deletion();

	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_INSTALL, &exp_root_manifest_id, manifest_buf, manifest_len, SUIT_SUCCESS);
	root_assert_component_deletion();

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_INSTALL);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, err);
}

void test_suit_process_seq_validate(void)
{
	root_assert_envelope_authorization();
	root_assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	root_assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_VALIDATE, &exp_root_manifest_id, 1, SUIT_SUCCESS);

	/* SUIT_COMMMON sequence from the root manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_component_handle, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_component_handle, exp_app_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);
	/* SUIT_VALIDATE sequence from the root manifest */
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(radio_component_handle, suit_cose_sha256, &exp_radio_manifest_digest, SUIT_SUCCESS);
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(app_component_handle, suit_cose_sha256, &exp_app_manifest_digest, SUIT_SUCCESS);
	/* Execute dependency_integrity check on radio and application manifest */
	radio_assert_envelope_integrity(true);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_radio_manifest_id, SUIT_SEQ_VALIDATE, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_VALIDATE, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	app_assert_envelope_integrity(true);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_app_manifest_id, SUIT_SEQ_VALIDATE, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_VALIDATE, &exp_app_manifest_id, 1, SUIT_SUCCESS);

	/* Process radio manifest */
	radio_assert_envelope_authorization(true);
	radio_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_VALIDATE, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the radio manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_memptr_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	/* SUIT_VALIDATE sequence from the radio manifest */
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(radio_fw_component_handle, suit_cose_sha256, &exp_radio_image_digest, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_VALIDATE, &exp_radio_manifest_id, exp_radio_envelope_payload.value, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	radio_assert_component_deletion();

	/* Process application manifest */
	app_assert_envelope_authorization(true);
	app_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_VALIDATE, &exp_app_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the application manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_memptr_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);
	/* SUIT_VALIDATE sequence from the application manifest */
	__cmock_suit_plat_check_image_match_ExpectComplexArgsAndReturn(app_fw_component_handle, suit_cose_sha256, &exp_app_image_digest, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_VALIDATE, &exp_app_manifest_id, exp_app_envelope_payload.value, exp_app_envelope_payload.len, SUIT_SUCCESS);
	app_assert_component_deletion();

	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_VALIDATE, &exp_root_manifest_id, manifest_buf, manifest_len, SUIT_SUCCESS);
	root_assert_component_deletion();

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_VALIDATE);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, err);
}

void test_suit_process_seq_load(void)
{
	root_assert_envelope_authorization();
	root_assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	root_assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_LOAD, &exp_root_manifest_id, 1, SUIT_SUCCESS);

	/* SUIT_COMMMON sequence from the root manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_component_handle, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_component_handle, exp_app_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);

	/* Execute dependency_integrity check on radio and application manifest */
	radio_assert_envelope_integrity(true);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_radio_manifest_id, SUIT_SEQ_LOAD, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_LOAD, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	app_assert_envelope_integrity(true);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_app_manifest_id, SUIT_SEQ_LOAD, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_LOAD, &exp_app_manifest_id, 1, SUIT_SUCCESS);

	/* Process radio manifest */
	radio_assert_envelope_authorization(true);
	radio_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_LOAD, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the radio manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_memptr_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_LOAD, &exp_radio_manifest_id, exp_radio_envelope_payload.value, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	radio_assert_component_deletion();

	/* Process application manifest */
	app_assert_envelope_authorization(true);
	app_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_LOAD, &exp_app_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the application manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_memptr_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_LOAD, &exp_app_manifest_id, exp_app_envelope_payload.value, exp_app_envelope_payload.len, SUIT_SUCCESS);
	app_assert_component_deletion();

	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_LOAD, &exp_root_manifest_id, manifest_buf, manifest_len, SUIT_SUCCESS);
	root_assert_component_deletion();

	int err = suit_process_sequence(manifest_buf, manifest_len, SUIT_SEQ_LOAD);
	TEST_ASSERT_EQUAL(SUIT_SUCCESS, err);
}

void test_suit_process_seq_invoke(void)
{
	root_assert_envelope_authorization();
	root_assert_component_creation();
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	root_assert_dry_run_execution();
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_INVOKE, &exp_root_manifest_id, 1, SUIT_SUCCESS);

	/* SUIT_COMMMON sequence from the root manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_component_handle, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_component_handle, exp_app_envelope_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);

	/* Execute dependency_integrity check on radio and application manifest */
	radio_assert_envelope_integrity(true);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_radio_manifest_id, SUIT_SEQ_INVOKE, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_INVOKE, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	app_assert_envelope_integrity(true);
	__cmock_suit_plat_authorize_process_dependency_ExpectComplexArgsAndReturn(&exp_root_manifest_id, &exp_app_manifest_id, SUIT_SEQ_INVOKE, SUIT_SUCCESS);
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_INVOKE, &exp_app_manifest_id, 1, SUIT_SUCCESS);

	/* Process radio manifest */
	radio_assert_envelope_authorization(true);
	radio_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_INVOKE, &exp_radio_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the radio manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(radio_fw_memptr_component_handle, exp_radio_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(radio_fw_component_handle, &exp_radio_cid_uuid, SUIT_SUCCESS);
	/* SUIT_INVOKE sequence from the radio manifest */
	__cmock_suit_plat_invoke_ExpectAndReturn(radio_fw_component_handle, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_INVOKE, &exp_radio_manifest_id, exp_radio_envelope_payload.value, exp_radio_envelope_payload.len, SUIT_SUCCESS);
	radio_assert_component_deletion();

	/* Process application manifest */
	app_assert_envelope_authorization(true);
	app_assert_component_creation();
	__cmock_suit_plat_authorize_sequence_num_ExpectComplexArgsAndReturn(SUIT_SEQ_INVOKE, &exp_app_manifest_id, 1, SUIT_SUCCESS);
	/* SUIT_COMMON sequence from the application manifest */
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_override_image_size_ExpectAndReturn(app_fw_memptr_component_handle, exp_app_fw_payload.len, SUIT_SUCCESS);
	__cmock_suit_plat_check_vid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_vid_uuid, SUIT_SUCCESS);
	__cmock_suit_plat_check_cid_ExpectComplexArgsAndReturn(app_fw_component_handle, &exp_app_cid_uuid, SUIT_SUCCESS);
	/* SUIT_INVOKE sequence from the application manifest */
	__cmock_suit_plat_invoke_ExpectAndReturn(app_fw_component_handle, NULL, SUIT_SUCCESS);
	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_INVOKE, &exp_app_manifest_id, exp_app_envelope_payload.value, exp_app_envelope_payload.len, SUIT_SUCCESS);
	app_assert_component_deletion();

	__cmock_suit_plat_sequence_completed_ExpectComplexArgsAndReturn(SUIT_SEQ_INVOKE, &exp_root_manifest_id, manifest_buf, manifest_len, SUIT_SUCCESS);
	root_assert_component_deletion();

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
