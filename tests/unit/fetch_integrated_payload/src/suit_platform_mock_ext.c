/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "suit_platform_mock_ext.h"


void assert_zcbor_string(void *p1, void *p2)
{
	struct zcbor_string *a = (struct zcbor_string *)p1;
	struct zcbor_string *b = (struct zcbor_string *)p2;

	/* Do not assert if both pointers are set to NULL */
	if ((a != NULL) || (b != NULL)) {
		TEST_ASSERT_NOT_NULL_MESSAGE((a), "Expected ZCBOR string pointer was not passed");
		TEST_ASSERT_NOT_NULL_MESSAGE((b), "ZCBOR string pointer was not passed");

		TEST_ASSERT_EQUAL_MESSAGE((a)->len, (b)->len, "ZCBOR string length does not match");
		if ((a)->len != 0) {
			TEST_ASSERT_NOT_NULL_MESSAGE((a)->value, "expected ZCBOR string value is not set");
			TEST_ASSERT_NOT_NULL_MESSAGE((b)->value, "ZCBOR string value is not set");
			TEST_ASSERT_EQUAL_MEMORY_MESSAGE((a)->value, (b)->value, (a)->len, "ZCBOR string value does not match");
		}
	}
}

COMPLEX_ARG_Q_DEFINE(__check_digest_callback_queue);
int __check_digest_callback(enum suit_cose_alg alg_id, struct zcbor_string* digest, struct zcbor_string* payload, int cmock_num_calls)
{
	(void)assert_complex_arg(&__check_digest_callback_queue, digest);
	(void)assert_complex_arg(&__check_digest_callback_queue, payload);
	return assert_complex_arg(&__check_digest_callback_queue, NULL);
}

COMPLEX_ARG_Q_DEFINE(__authenticate_callback_queue);
int __authenticate_callback(enum suit_cose_alg alg_id, struct zcbor_string* key_id, struct zcbor_string* signature, struct zcbor_string* data, int cmock_num_calls)
{
	(void)assert_complex_arg(&__authenticate_callback_queue, key_id);
	(void)assert_complex_arg(&__authenticate_callback_queue, signature);
	(void)assert_complex_arg(&__authenticate_callback_queue, data);
	return assert_complex_arg(&__authenticate_callback_queue, NULL);
}

COMPLEX_ARG_Q_DEFINE(__get_component_handle_callback_queue);
int __get_component_handle_callback(struct zcbor_string* component_id, struct zcbor_string** key_ids, size_t num_key_ids, suit_component_t* component_handle, int cmock_num_calls)
{
	(void)assert_complex_arg(&__get_component_handle_callback_queue, component_id);
	return assert_complex_arg(&__get_component_handle_callback_queue, NULL);
}

COMPLEX_ARG_Q_DEFINE(__check_image_match_callback_queue);
int __check_image_match_callback(enum suit_cose_alg alg_id, struct zcbor_string* digest, size_t image_size, suit_component_t image_handle, int cmock_num_calls)
{
	(void)assert_complex_arg(&__check_image_match_callback_queue, digest);
	return assert_complex_arg(&__check_image_match_callback_queue, NULL);
}

COMPLEX_ARG_Q_DEFINE(__check_vid_callback_queue);
int __check_vid_callback(struct zcbor_string* vid_uuid, suit_component_t component_handle, int cmock_num_calls)
{
	(void)assert_complex_arg(&__check_vid_callback_queue, vid_uuid);
	return assert_complex_arg(&__check_vid_callback_queue, NULL);
}

COMPLEX_ARG_Q_DEFINE(__check_cid_callback_queue);
int __check_cid_callback(struct zcbor_string* cid_uuid, suit_component_t component_handle, int cmock_num_calls)
{
	(void)assert_complex_arg(&__check_cid_callback_queue, cid_uuid);
	return assert_complex_arg(&__check_cid_callback_queue, NULL);
}

COMPLEX_ARG_Q_DEFINE(__check_did_callback_queue);
int __check_did_callback(struct zcbor_string* did_uuid, suit_component_t component_handle, int cmock_num_calls)
{
	(void)assert_complex_arg(&__check_did_callback_queue, did_uuid);
	return assert_complex_arg(&__check_did_callback_queue, NULL);
}

COMPLEX_ARG_Q_DEFINE(__check_fetch_integrated_callback_queue);
int __check_fetch_integrated_callback(suit_component_t dst_handle, struct zcbor_string* payload, int cmock_num_calls)
{
	(void)assert_complex_arg(&__check_fetch_integrated_callback_queue, payload);
	return assert_complex_arg(&__check_fetch_integrated_callback_queue, NULL);
}

COMPLEX_ARG_Q_DEFINE(__fetch_integrated_callback_queue);
int __fetch_integrated_callback(suit_component_t dst_handle, struct zcbor_string* payload, int cmock_num_calls)
{
	(void)assert_complex_arg(&__fetch_integrated_callback_queue, payload);
	return assert_complex_arg(&__fetch_integrated_callback_queue, NULL);
}
