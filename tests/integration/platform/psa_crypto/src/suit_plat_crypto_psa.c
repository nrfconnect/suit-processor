/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <psa/crypto.h>
#include <suit_platform.h>
#include <suit_platform_internal.h>


#if defined(CONFIG_SOC_NRF52840)
#define DEFAULT_PSA_KEY_ID 0x7fffffe0
#elif defined(CONFIG_SOC_NRF5420)
#define DEFAULT_PSA_KEY_ID 0x7fffffe0
#else
#define DEFAULT_PSA_KEY_ID 0
#endif


int suit_plat_check_digest(enum suit_cose_alg alg_id,
		struct zcbor_string *digest,
		struct zcbor_string *payload)
{
	size_t expected_hash_len = 0;
	psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
	psa_algorithm_t psa_alg;

	/* Find the PSA hash ID */
	switch (alg_id) {
		case suit_cose_sha256:
			psa_alg = PSA_ALG_SHA_256;
			break;
		default:
			return SUIT_ERR_DECODING;
	}

	/* Verify digest length */
	expected_hash_len = PSA_HASH_LENGTH(psa_alg);
	if (digest->len != expected_hash_len) {
		return SUIT_ERR_DECODING;
	}

	/* Calculate digest over input */
	if (psa_hash_setup(&operation, psa_alg) != PSA_SUCCESS) {
		return SUIT_ERR_DECODING;
	}

	if (psa_hash_update(&operation, payload->value, payload->len) != PSA_SUCCESS) {
		/* Clean up hash operation context */
		psa_hash_abort(&operation);
		return SUIT_ERR_DECODING;
	}

	/* Verify digest */
	if (psa_hash_verify(&operation, digest->value, digest->len) == PSA_SUCCESS) {
		return SUIT_SUCCESS;
	}

	/* Clean up hash operation context */
	psa_hash_abort(&operation);

	return SUIT_ERR_AUTHENTICATION;
}

int suit_plat_authenticate(enum suit_cose_alg alg_id, struct zcbor_string *key_id,
		struct zcbor_string *signature, struct zcbor_string *data)
{
	psa_algorithm_t psa_alg;
	psa_key_id_t public_key_id = DEFAULT_PSA_KEY_ID;

	switch (alg_id) {
		case suit_cose_es256:
			psa_alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
			break;
		default:
			return SUIT_ERR_DECODING;
	}

	if (psa_verify_message(
		public_key_id,
		psa_alg,
		data->value,
		data->len,
		signature->value,
		signature->len) == PSA_SUCCESS) {
		return SUIT_SUCCESS;
	}

	return SUIT_ERR_AUTHENTICATION;
}

int suit_plat_check_image_match(enum suit_cose_alg alg_id,
		struct zcbor_string *digest, size_t image_size,
		suit_component_t image_handle)
{
	const struct suit_component_impl *impl = suit_plat_component_impl_get(image_handle);
	uint8_t * read_address = NULL;
	size_t read_size = 0;
	struct zcbor_string payload;

	/* Check the component implementation. */
	if (impl == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	/* Check if the driver supports direct read mode. */
	if (impl->read_address == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	/* Get the pointer to the readable data. */
	read_size = impl->read_address(image_handle, &read_address);
	if (read_size < image_size) {
		return SUIT_ERR_UNAVAILABLE_PAYLOAD;
	}

	payload.value = read_address;
	payload.len = image_size;

	return suit_plat_check_digest(alg_id, digest, &payload);
}
