/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <suit_decoder.h>
#include <suit_platform.h>
#include <manifest_decode.h>
#include <cose_encode.h>
#include <cose_decode.h>
#include <suit_manifest.h>

/** Verify that the header matches the expected length.
 */
static bool header_check(const uint8_t *cbstr, size_t exp_len)
{
	uint8_t byte = *cbstr;

	/* Verify the expected length by decoding the ZCBOR header manually.
	 *
	 * The first byte indicates the type of the data:
	 *  - 0x40:0x58 is a byte string with the length encoded as a sum with 0x40.
	 *  - 0x58 is a byte string with the length encoded on the next byte.
	 *  - 0x59 is a byte string with the length encoded on the next two bytes.
	 *  - 0x80:0x88 is an array with the length encoded as a sum with 0x80.
	 *  - 0x98 is an array with the length encoded on the next byte.
	 *  - 0x99 is an array with the length encoded on the next two bytes.
	 *
	 * Indefinite length structures, as well as those with length encoded on
	 * 4 bytes (0x5A and 0x9A) are not supported.
	 * For more details, see the description of major types in RFC 7049.
	 *
	 * It is expected, that the decoding will be covered by the ZCBOR library
	 * in the near future, but currently the implementation of the
	 * zcbor_header_len(..) is not robust enough to pass the unit tests.
	 */
	if ((byte >= 0x40) && (byte < 0x58)) {
		return (byte == (0x40 + exp_len));
	} else if (byte == 0x58) {
		return (cbstr[1] == exp_len);
	} else if (byte == 0x59) {
		return ((cbstr[1] * 256 + cbstr[2]) == exp_len);
	} else if ((byte >= 0x80) && (byte < 0x88)) {
		return (byte == (0x80 + exp_len));
	} else if (byte == 0x98) {
		return (cbstr[1] == exp_len);
	} else if (byte == 0x99) {
		return ((cbstr[1] * 256 + cbstr[2]) == exp_len);
	}

	return false;
}


/** Calculate the length of the CBOR byte string and array header
 */
static int header_len(size_t len, const uint8_t *value)
{
	int exp_len = -1;
	bool header_ok = false;

	if (len <= 23) {
		exp_len = 1;
	} else if (len <= 255) {
		exp_len = 2;
	} else if (len <= 0xFFFF) {
		exp_len = 3;
	}

	if (exp_len > 0) {
		header_ok = header_check(value - exp_len, len);
	}

	if (!header_ok) {
		return -1;
	}

	return exp_len;
}

static int cose_verify_digest(struct zcbor_string *digest_bstr, struct zcbor_string *data_bstr)
{
	struct SUIT_Digest digest;
	size_t bytes_processed;

	int ret = cbor_decode_SUIT_Digest(digest_bstr->value, digest_bstr->len, &digest, &bytes_processed);

	if ((ret != 0) || (bytes_processed != digest_bstr->len)) {
		return SUIT_ERR_DECODING;
	}

	/* The SHA256 algorithm is enforced by CDDL, so the digest length is known. */
	if (digest._SUIT_Digest_suit_digest_bytes.len != 32) {
		return SUIT_ERR_DECODING;
	}

	/* Include CBOR header (type, length) in digest calculation */
	int offset = header_len(data_bstr->len, &data_bstr->value[0]);

	if (offset < 0) {
		return SUIT_ERR_DECODING;
	}
	struct zcbor_string data_bytes = {
		.value = data_bstr->value - offset,
		.len = data_bstr->len + offset,
	};

	return suit_plat_check_digest(
		/* Value enforced by the input CDDL (manifest.cddl, suit-cose-hash-algs /= cose-alg-sha-256) */
		suit_cose_sha256,
		&digest._SUIT_Digest_suit_digest_bytes,
		&data_bytes);
}

static int cose_sign1_authenticate_digest(struct zcbor_string *manifest_component_id, struct zcbor_string *COSE_Sign1_bstr, struct zcbor_string *digest_bstr)
{
	uint8_t signed_data[SUIT_SUIT_SIG_STRUCTURE1_MAX_LENGTH];
	size_t signed_data_size = 0;

	/* Decode COSE_Sign1 structure */
	struct COSE_Sign1 cose_sign1_struct;
	size_t cose_sign1_struct_size = 0;

	int ret = cbor_decode_COSE_Sign1_Tagged(
		COSE_Sign1_bstr->value,
		COSE_Sign1_bstr->len,
		&cose_sign1_struct,
		&cose_sign1_struct_size);
	if ((ret != ZCBOR_SUCCESS) || (cose_sign1_struct_size != COSE_Sign1_bstr->len)) {
		return SUIT_ERR_DECODING;
	}

	/* The ES256 algorithm is enforced by CDDL, so the signature length is known. */
	if (cose_sign1_struct._COSE_Sign1_signature.len != 64) {
		return SUIT_ERR_DECODING;
	}

	/* Construct Sig_structure1 structure */
	struct Sig_structure1 signature;

	memset(&signature, 0, sizeof(signature));
	signature._Sig_structure1_body_protected_cbor = cose_sign1_struct._COSE_Sign1__Headers._Headers_protected_cbor;
	signature._Sig_structure1_payload = *digest_bstr;

	/* Encode Sig_structure1 structure as byte string */
	memset(signed_data, 0, sizeof(signed_data));
	ret = cbor_encode_Sig_structure1(
		signed_data, sizeof(signed_data),
		&signature,
		&signed_data_size);
	if (ret != ZCBOR_SUCCESS) {
		return SUIT_ERR_DECODING;
	}

	struct zcbor_string signed_bstr = {
		.value = signed_data,
		.len = signed_data_size,
	};

#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
	/* Authenticate data using platform API */
	ret = suit_plat_authenticate(
		/* Value enforced by the input CDDL (cose_sign.cddl, supported_algs //= (ES256: -7)) */
		suit_cose_es256,
		(cose_sign1_struct._COSE_Sign1__Headers._Headers_protected_cbor._header_map_key_id_present ?
			&cose_sign1_struct._COSE_Sign1__Headers._Headers_protected_cbor._header_map_key_id._header_map_key_id :
			(struct zcbor_string *)NULL),
		/* Pass signature, specific for the key */
		&cose_sign1_struct._COSE_Sign1_signature,
		/* Authenticate Signature1 structure, including both algorithm ID and digest bytes of the manifest */
		&signed_bstr);
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
	/* Authenticate data using platform API */
	ret = suit_plat_authenticate_manifest(
		manifest_component_id,
		/* Value enforced by the input CDDL (cose_sign.cddl, supported_algs //= (ES256: -7)) */
		suit_cose_es256,
		(cose_sign1_struct._COSE_Sign1__Headers._Headers_protected_cbor._header_map_key_id_present ?
			&cose_sign1_struct._COSE_Sign1__Headers._Headers_protected_cbor._header_map_key_id._header_map_key_id :
			(struct zcbor_string *)NULL),
		/* Pass signature, specific for the key */
		&cose_sign1_struct._COSE_Sign1_signature,
		/* Authenticate Signature1 structure, including both algorithm ID and digest bytes of the manifest */
		&signed_bstr);
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */

	return ret;
}

static void suit_decoder_reset_state(struct suit_decoder_state *state)
{
	memset(state, 0, sizeof(*state));
	state->step = INVALID;
}

static int get_component_id_str(struct zcbor_string *out_component_id,
	const struct SUIT_Component_Identifier *component_id)
{
	const struct zcbor_string *first_bstr = &component_id->_SUIT_Component_Identifier_bstr[0];
	size_t payload_len = 0;

	int first_bstr_len = header_len(first_bstr->len, &first_bstr->value[0]);

	if (first_bstr_len < 0) {
		return SUIT_ERR_DECODING;
	}

	int extra_header_len = header_len(component_id->_SUIT_Component_Identifier_bstr_count, &(first_bstr->value[0]) - first_bstr_len);

	if (extra_header_len < 0) {
		return SUIT_ERR_DECODING;
	}

	/* Do not allow empty lists as components ID values (already verified by CDDL). */

	for (size_t i = 0; i < component_id->_SUIT_Component_Identifier_bstr_count; i++) {
		/* Do not allow empty byte strings as component ID values. */
		if (component_id->_SUIT_Component_Identifier_bstr[i].len == 0) {
			return SUIT_ERR_DECODING;
		}

		int chunk_len = header_len(component_id->_SUIT_Component_Identifier_bstr[i].len, &component_id->_SUIT_Component_Identifier_bstr[i].value[0]);

		if (chunk_len < 0) {
			return SUIT_ERR_DECODING;
		}
		payload_len += chunk_len;
		payload_len += component_id->_SUIT_Component_Identifier_bstr[i].len;
	}

	out_component_id->value = first_bstr->value - extra_header_len - first_bstr_len;
	out_component_id->len = extra_header_len + payload_len;

	return SUIT_SUCCESS;
}

static int check_dependency_indexes(struct SUIT_Common *common)
{
	const struct SUIT_Dependencies *dependencies = NULL;

	if (common->__SUIT_Common_extensions_present) {
		const struct _SUIT_Common_extensions *ext = &common->__SUIT_Common_extensions;
		const struct SUIT_Common_extensions_suit_dependencies_ *dependencies_ext = &ext->__SUIT_Common_extensions;

		if (dependencies_ext->_SUIT_Common_extensions_suit_dependencies_present) {
			dependencies = &dependencies_ext->_SUIT_Common_extensions_suit_dependencies._SUIT_Common_extensions_suit_dependencies;
		}
	}

	if (dependencies == NULL) {
		return SUIT_ERR_MISSING_COMPONENT;
	}

	if (!common->_SUIT_Common_suit_components_present) {
		/* The presence of suit-common is already enforced by suit_decoder_authorize_manifest(..) API. */
		return SUIT_ERR_TAMP;
	}

	const struct SUIT_Components *components = &common->_SUIT_Common_suit_components._SUIT_Common_suit_components;
	uint_fast32_t max_index = components->_SUIT_Components__SUIT_Component_Identifier_count;

	for (size_t dep_i = 0; dep_i < dependencies->_SUIT_Dependencies__SUIT_Dependency_Metadata_count; dep_i++) {
		const struct SUIT_Dependencies__SUIT_Dependency_Metadata *meta = &dependencies->_SUIT_Dependencies__SUIT_Dependency_Metadata[dep_i];

		if (meta->_SUIT_Dependencies__SUIT_Dependency_Metadata_key >= max_index) {
			return SUIT_ERR_MANIFEST_VALIDATION;
		}
	}

	return SUIT_SUCCESS;
}

static int find_dependency_manifest_prefix(struct SUIT_Common *common, size_t index, struct zcbor_string *prefix)
{
	const struct SUIT_Dependencies *dependencies = NULL;

	if (prefix == NULL) {
		return SUIT_ERR_CRASH;
	}

	if (common->__SUIT_Common_extensions_present) {
		const struct _SUIT_Common_extensions *ext = &common->__SUIT_Common_extensions;
		const struct SUIT_Common_extensions_suit_dependencies_ *dependencies_ext = &ext->__SUIT_Common_extensions;

		if (dependencies_ext->_SUIT_Common_extensions_suit_dependencies_present) {
			dependencies = &dependencies_ext->_SUIT_Common_extensions_suit_dependencies._SUIT_Common_extensions_suit_dependencies;
		}
	}

	if (dependencies == NULL) {
		return SUIT_ERR_MISSING_COMPONENT;
	}

	for (size_t dep_i = 0; dep_i < dependencies->_SUIT_Dependencies__SUIT_Dependency_Metadata_count; dep_i++) {
		const struct SUIT_Dependencies__SUIT_Dependency_Metadata *meta = &dependencies->_SUIT_Dependencies__SUIT_Dependency_Metadata[dep_i];

		if (index == meta->_SUIT_Dependencies__SUIT_Dependency_Metadata_key) {
			const struct SUIT_Dependency_Metadata *prefix_cbor = &meta->_SUIT_Dependencies__SUIT_Dependency_Metadata;

			if (prefix_cbor->_SUIT_Dependency_Metadata_suit_dependency_prefix_present) {
				/* Zip list of strings into a single ZCBOR string */
				int ret = get_component_id_str(prefix,
					&prefix_cbor->_SUIT_Dependency_Metadata_suit_dependency_prefix._SUIT_Dependency_Metadata_suit_dependency_prefix);
				if (ret != SUIT_SUCCESS) {
					return ret;
				}
			} else {
				prefix->value = NULL;
				prefix->len = 0;
			}

			return SUIT_SUCCESS;
		}
	}

	return SUIT_ERR_MISSING_COMPONENT;
}


int suit_decoder_init(struct suit_decoder_state *state, struct suit_manifest_state *manifest)
{
	if ((state == NULL) || (manifest == NULL)) {
		return SUIT_ERR_DECODING;
	}

	suit_decoder_reset_state(state);
	state->decoded_manifest = manifest;
	state->step = INITIALIZED;

	return SUIT_SUCCESS;
}

int suit_decoder_decode_envelope(struct suit_decoder_state *state, uint8_t *envelope_str, size_t envelope_len)
{
	size_t decoded_len = 0;
	int ret = SUIT_SUCCESS;

	if ((state == NULL) || (envelope_str == NULL) || (envelope_len == 0)) {
		return SUIT_ERR_DECODING;
	}

	if (state->step != INITIALIZED) {
		return SUIT_ERR_ORDER;
	}

	ret = cbor_decode_SUIT_Envelope_Tagged(
		envelope_str, envelope_len, &state->envelope, &decoded_len);

	/* For development, condition on expected envelope size was modified.
	   Now envelope_len represents max allowed size of envelope */
	if ((ret != ZCBOR_SUCCESS) || (decoded_len > envelope_len)) {
		ret = ZCBOR_ERR_TO_SUIT_ERR(ret);
	}

	if (ret == ZCBOR_SUCCESS) {
		struct SUIT_Authentication *auth = &state->envelope._SUIT_Envelope_suit_authentication_wrapper_cbor;

		if ((auth->_SUIT_Authentication_bstr_count > SUIT_MAX_NUM_SIGNERS) ||
		    (state->envelope._SUIT_Envelope__SUIT_Integrated_Payload_count > SUIT_MAX_NUM_INTEGRATED_PAYLOADS)) {
			ret = SUIT_ERR_DECODING;
		}

		if (ret == ZCBOR_SUCCESS) {
			/* Iterate through (key, signature) pairs */
			for (int i = 0; i < auth->_SUIT_Authentication_bstr_count; i++) {
				state->authentication_bstr[i] = auth->_SUIT_Authentication_bstr[i];
			}
			state->authentication_bstr_count = auth->_SUIT_Authentication_bstr_count;
			state->manifest_digest_bytes = auth->_SUIT_Authentication_SUIT_Digest_bstr;
		}

		/* Store pointers to the severable sequences for further verification and execution. */
		if (state->envelope._SUIT_Envelope__SUIT_Severable_Manifest_Members._SUIT_Severable_Manifest_Members_suit_text_present) {
			state->decoded_manifest->text = state->envelope._SUIT_Envelope__SUIT_Severable_Manifest_Members._SUIT_Severable_Manifest_Members_suit_text._SUIT_Severable_Manifest_Members_suit_text;
			/* If the length of the sequence is zero, it is impossible to calculate the digest of it.
			 * In such case, invalidate envelope to avoid possible random memory accesses.
			 */
			if (state->decoded_manifest->text.len < 1) {
				ret = SUIT_ERR_DECODING;
				state->decoded_manifest->text.len = 0;
				state->decoded_manifest->text.value = NULL;
			} else {
				state->decoded_manifest->text_status = SEVERED;
			}
		}

		/* Store pointers to the integrated payloads and their keys. */
		if (ret == ZCBOR_SUCCESS) {
			for (size_t i = 0; i < state->envelope._SUIT_Envelope__SUIT_Integrated_Payload_count; i++) {
				state->decoded_manifest->integrated_payloads[i].key = state->envelope._SUIT_Envelope__SUIT_Integrated_Payload[i]._SUIT_Envelope__SUIT_Integrated_Payload._SUIT_Integrated_Payload_suit_integrated_payload_key_key;
				state->decoded_manifest->integrated_payloads[i].payload = state->envelope._SUIT_Envelope__SUIT_Integrated_Payload[i]._SUIT_Envelope__SUIT_Integrated_Payload._SUIT_Integrated_Payload_suit_integrated_payload_key;
			}
			state->decoded_manifest->integrated_payloads_count = state->envelope._SUIT_Envelope__SUIT_Integrated_Payload_count;

			state->decoded_manifest->envelope_str.value = envelope_str;
			state->decoded_manifest->envelope_str.len = decoded_len;
			state->step = ENVELOPE_DECODED;
			return SUIT_SUCCESS;
		}
	}

	suit_decoder_reset_state(state);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	return SUIT_ERR_TAMP;
}

int suit_decoder_check_manifest_digest(struct suit_decoder_state *state)
{
	int ret = SUIT_SUCCESS;
	struct zcbor_string manifest_bstr;

	if (state == NULL) {
		return SUIT_ERR_DECODING;
	}

	if (state->step != ENVELOPE_DECODED) {
		SUIT_ERR("Invalid decoder state. Expected: %d, current state: %d\r\n", ENVELOPE_DECODED, state->step);
		return SUIT_ERR_ORDER;
	}

	manifest_bstr = state->envelope._SUIT_Envelope_suit_manifest;
	ret = cose_verify_digest(&state->manifest_digest_bytes, &manifest_bstr);

	if (ret == SUIT_SUCCESS) {
		state->step = MANIFEST_DIGEST_VERIFIED;
		return SUIT_SUCCESS;
	}

	suit_decoder_reset_state(state);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	return SUIT_ERR_TAMP;
}

int suit_decoder_decode_manifest(struct suit_decoder_state *state)
{
	size_t decoded_len = 0;
	int ret = SUIT_SUCCESS;
	struct zcbor_string manifest_bstr;

	if (state == NULL) {
		return SUIT_ERR_DECODING;
	}

	if (state->step != MANIFEST_DIGEST_VERIFIED) {
		return SUIT_ERR_ORDER;
	}

	manifest_bstr = state->envelope._SUIT_Envelope_suit_manifest;

	ret = cbor_decode_SUIT_Manifest(
		manifest_bstr.value,
		manifest_bstr.len,
		&state->manifest,
		&decoded_len);

	if ((ret != ZCBOR_SUCCESS) || (decoded_len != manifest_bstr.len)) {
		ret = ZCBOR_ERR_TO_SUIT_ERR(ret);
	}

	if (ret == ZCBOR_SUCCESS) {
		if (state->manifest.__SUIT_Manifest_Extensions_present) {
			const struct _SUIT_Manifest_Extensions *ext = &state->manifest.__SUIT_Manifest_Extensions;
			const struct SUIT_Manifest_Extensions_suit_manifest_component_id *component_id_ext = &ext->__SUIT_Manifest_Extensions;
			const struct SUIT_Component_Identifier *manifest_component = &component_id_ext->_SUIT_Manifest_Extensions_suit_manifest_component_id;

			ret = get_component_id_str(&state->decoded_manifest->manifest_component_id, manifest_component);
		}
	}

	if (ret == ZCBOR_SUCCESS) {
		state->step = MANIFEST_DECODED;
		SUIT_DBG("Manifest decoded\r\n");
		return SUIT_SUCCESS;
	}

	SUIT_DBG("Failed to decode manifest (%d)\r\n", ret);

	suit_decoder_reset_state(state);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	return SUIT_ERR_TAMP;
}

int suit_decoder_authenticate_manifest(struct suit_decoder_state *state)
{
	int ret = SUIT_SUCCESS;

	if (state == NULL) {
		return SUIT_ERR_DECODING;
	}

	if (state->step != MANIFEST_DECODED) {
		return SUIT_ERR_ORDER;
	}

	if (state->authentication_bstr_count == 0) {
#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
		ret = SUIT_ERR_AUTHENTICATION;
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
		ret = suit_plat_authorize_unsigned_manifest(&state->decoded_manifest->manifest_component_id);
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
	} else {
		volatile enum suit_bool results[SUIT_MAX_NUM_SIGNERS * 2]; /* Use every other entry as canary */
		volatile int num_ok = 0;
		size_t i = 0;

		for (i = 0; i < SUIT_MAX_NUM_SIGNERS * 2; i++) {
			results[i] = suit_bool_false;
		}

		/* Iterate through (key, signature) pairs */
		for (i = 0; i < state->authentication_bstr_count; i++) {
			ret = cose_sign1_authenticate_digest(
				&state->decoded_manifest->manifest_component_id,
				&state->authentication_bstr[i],
				&state->manifest_digest_bytes);

			/* If authentication for any key fails, drop the envelope */
			if (ret != SUIT_SUCCESS) {
				results[i * 2] = suit_bool_false;
			}

			if (ret == SUIT_SUCCESS) {
				results[i * 2] = suit_bool_true;
			}
		}

		for (i = 0; i < state->authentication_bstr_count; i++) {
			if (results[i * 2 + 1] != suit_bool_false) {
				ret = SUIT_ERR_TAMP;
			}

			if (results[i * 2] == suit_bool_true) {
				if (num_ok == i) {
					num_ok++;
				}
			}
		}

		if (num_ok == state->authentication_bstr_count) {
			ret = SUIT_SUCCESS;
		} else if (ret != SUIT_ERR_TAMP) {
			ret = SUIT_ERR_AUTHENTICATION;
		}
	}

	if (ret == ZCBOR_SUCCESS) {
		state->step = MANIFEST_AUTHENTICATED;
		return SUIT_SUCCESS;
	}

	suit_decoder_reset_state(state);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	return SUIT_ERR_TAMP;
}

int suit_decoder_authorize_manifest(struct suit_decoder_state *state)
{
	struct SUIT_Common *common = NULL;
	int ret = SUIT_SUCCESS;

	if (state == NULL) {
		return SUIT_ERR_DECODING;
	}

	if (state->step != MANIFEST_AUTHENTICATED) {
		return SUIT_ERR_ORDER;
	}

	/* Verify manifest version - enforced by the CDDL and checked by the ZCBOR parser code */

	/* Cannot perform universal sequence number authorization.
	 * Skip this check in decoder logic and store the sequence number value inside the output structure.
	 */
	state->decoded_manifest->sequence_number = state->manifest._SUIT_Manifest_suit_manifest_sequence_number;

	/* Verify common sequence */
	common = &state->manifest._SUIT_Manifest_suit_common_cbor;

	/* Verify list of components */
	if (common->_SUIT_Common_suit_components_present) {
		struct zcbor_string component_id;

		/* Verify the length of the list */
		if ((common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier_count > SUIT_MAX_NUM_COMPONENTS) ||
		    (common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier_count < 1)) {
			return SUIT_ERR_MANIFEST_VALIDATION;
		}

		/* Authorize component IDs */
		for (int i = 0; i < common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier_count; i++) {
			/* Zip list of strings into a single ZCBOR string */
			ret = get_component_id_str(&component_id,
				&common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier[i]);

#ifndef SUIT_PLATFORM_LEGACY_API_SUPPORT
			if (ret == SUIT_SUCCESS) {
				ret = suit_plat_authorize_component_id(&state->decoded_manifest->manifest_component_id, &component_id);
			}
#endif /* !SUIT_PLATFORM_LEGACY_API_SUPPORT */

			if (ret == SUIT_SUCCESS) {
				continue;
			}

			break;
		}

		/* Verify the depenecy indexes. */
		if (ret == SUIT_SUCCESS) {
			ret = check_dependency_indexes(common);
			/* Mask the error code, because it is allowed to have a manifest without dependencies. */
			if (ret == SUIT_ERR_MISSING_COMPONENT) {
				ret = SUIT_SUCCESS;
			}
		}
	} else {
		ret = SUIT_ERR_MANIFEST_VALIDATION;
	}

	if (ret == ZCBOR_SUCCESS) {
		SUIT_DBG("Manifest component IDs authorized\r\n");
		state->step = MANIFEST_AUTHORIZED;
		return SUIT_SUCCESS;
	}

	suit_decoder_reset_state(state);
	if (ret != SUIT_SUCCESS) {
		return ret;
	}

	return SUIT_ERR_TAMP;
}

int suit_decoder_decode_sequences(struct suit_decoder_state *state)
{
	int ret = SUIT_SUCCESS;

	if (state == NULL) {
		return SUIT_ERR_DECODING;
	}

	if (state->step != MANIFEST_AUTHORIZED) {
		return SUIT_ERR_ORDER;
	}

	if (state->manifest._SUIT_Manifest_suit_common_cbor._SUIT_Common_suit_shared_sequence_present) {
		if (state->decoded_manifest->shared_sequence_status == UNAVAILABLE) {
			state->decoded_manifest->shared_sequence = state->manifest._SUIT_Manifest_suit_common_cbor._SUIT_Common_suit_shared_sequence._SUIT_Common_suit_shared_sequence;
			state->decoded_manifest->shared_sequence_status = AUTHENTICATED;
		} else {
			state->decoded_manifest->shared_sequence_status = UNAVAILABLE;
			ret = SUIT_ERR_MANIFEST_VALIDATION;
		}
	} else {
		state->decoded_manifest->shared_sequence_status = UNAVAILABLE;
		ret = SUIT_ERR_MANIFEST_VALIDATION;
	}

	if (state->manifest._SUIT_Manifest__SUIT_Unseverable_Members._SUIT_Unseverable_Members_suit_validate_present) {
		if (state->decoded_manifest->validate_seq_status == UNAVAILABLE) {
			state->decoded_manifest->validate_seq = state->manifest._SUIT_Manifest__SUIT_Unseverable_Members._SUIT_Unseverable_Members_suit_validate._SUIT_Unseverable_Members_suit_validate;
			state->decoded_manifest->validate_seq_status = AUTHENTICATED;
		} else {
			state->decoded_manifest->validate_seq_status = UNAVAILABLE;
			ret = SUIT_ERR_MANIFEST_VALIDATION;
		}
	} else {
		state->decoded_manifest->validate_seq_status = UNAVAILABLE;
	}

	if (state->manifest._SUIT_Manifest__SUIT_Unseverable_Members._SUIT_Unseverable_Members_suit_load_present) {
		if (state->decoded_manifest->load_seq_status == UNAVAILABLE) {
			state->decoded_manifest->load_seq = state->manifest._SUIT_Manifest__SUIT_Unseverable_Members._SUIT_Unseverable_Members_suit_load._SUIT_Unseverable_Members_suit_load;
			state->decoded_manifest->load_seq_status = AUTHENTICATED;
		} else {
			state->decoded_manifest->load_seq_status = UNAVAILABLE;
			ret = SUIT_ERR_MANIFEST_VALIDATION;
		}
	} else {
		state->decoded_manifest->load_seq_status = UNAVAILABLE;
	}

	if (state->manifest._SUIT_Manifest__SUIT_Unseverable_Members._SUIT_Unseverable_Members_suit_invoke_present) {
		if (state->decoded_manifest->invoke_seq_status == UNAVAILABLE) {
			state->decoded_manifest->invoke_seq = state->manifest._SUIT_Manifest__SUIT_Unseverable_Members._SUIT_Unseverable_Members_suit_invoke._SUIT_Unseverable_Members_suit_invoke;
			state->decoded_manifest->invoke_seq_status = AUTHENTICATED;
		} else {
			state->decoded_manifest->invoke_seq_status = UNAVAILABLE;
			ret = SUIT_ERR_MANIFEST_VALIDATION;
		}
	} else {
		state->decoded_manifest->invoke_seq_status = UNAVAILABLE;
	}

	if (state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice._SUIT_Severable_Members_Choice_suit_payload_fetch_present) {
		if (state->decoded_manifest->payload_fetch_seq_status == UNAVAILABLE) {
			state->decoded_manifest->payload_fetch_seq = state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice._SUIT_Severable_Members_Choice_suit_payload_fetch._SUIT_Severable_Members_Choice_suit_payload_fetch;
			state->decoded_manifest->payload_fetch_seq_status = AUTHENTICATED;
		} else {
			state->decoded_manifest->payload_fetch_seq_status = UNAVAILABLE;
			ret = SUIT_ERR_MANIFEST_VALIDATION;
		}
	} else {
		state->decoded_manifest->payload_fetch_seq_status = UNAVAILABLE;
	}

	if (state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice._SUIT_Severable_Members_Choice_suit_install_present) {
		if (state->decoded_manifest->install_seq_status == UNAVAILABLE) {
			state->decoded_manifest->install_seq = state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice._SUIT_Severable_Members_Choice_suit_install._SUIT_Severable_Members_Choice_suit_install;
			state->decoded_manifest->install_seq_status = AUTHENTICATED;
		} else {
			state->decoded_manifest->install_seq_status = UNAVAILABLE;
			ret = SUIT_ERR_MANIFEST_VALIDATION;
		}
	} else {
		state->decoded_manifest->install_seq_status = UNAVAILABLE;
	}

	/* The CDDL enforces to severe the text field from the manifest.
	 * Due to that fact, if the text field is present inside the manifest, it contains the digest of the severed text field.
	 * If the text field was found before (status SEVERED), verify its digest.
	 */
	if (state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice._SUIT_Severable_Members_Choice_suit_text_present) {
		if (state->decoded_manifest->text_status == SEVERED) {
			int digest_ret = suit_plat_check_digest(suit_cose_sha256,
				&state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice._SUIT_Severable_Members_Choice_suit_text._SUIT_Severable_Members_Choice_suit_text._SUIT_Digest_suit_digest_bytes,
				&state->decoded_manifest->text);
			if (digest_ret == SUIT_SUCCESS) {
				state->decoded_manifest->text_status = AUTHENTICATED;
			} else {
				state->decoded_manifest->text_status = UNAVAILABLE;
				ret = SUIT_ERR_MANIFEST_VALIDATION;
			}
		} else {
			/* The text digest is present inside the manifest, but it cannot be verified,
			 * because the text payload has been severed.
			 * This is a valid case, ie once minified envelope is transferred into the
			 * SUIT storage partition.
			 */
			state->decoded_manifest->text_status = UNAVAILABLE;
		}
	} else {
		if (state->decoded_manifest->text_status != UNAVAILABLE) {
			ret = SUIT_ERR_MANIFEST_VALIDATION;
		}
		state->decoded_manifest->text_status = UNAVAILABLE;
	}

	/* Parse severable manifest members extensions. */
	if (state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice._SUIT_Severable_Members_Choice__severable_manifest_members_choice_extensions_present) {
		const struct SUIT_Severable_Members_Choice__severable_manifest_members_choice_extensions *ext = &state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice._SUIT_Severable_Members_Choice__severable_manifest_members_choice_extensions;
		const struct severable_manifest_members_choice_extensions_suit_dependency_resolution *dependency_resolution_ext = &ext->_SUIT_Severable_Members_Choice__severable_manifest_members_choice_extensions;
		const struct zcbor_string *dependency_resolution_seq = &dependency_resolution_ext->_severable_manifest_members_choice_extensions_suit_dependency_resolution;

		if (state->decoded_manifest->dependency_resolution_seq_status == UNAVAILABLE) {
			state->decoded_manifest->dependency_resolution_seq = *dependency_resolution_seq;
			state->decoded_manifest->dependency_resolution_seq_status = AUTHENTICATED;
		} else {
			state->decoded_manifest->dependency_resolution_seq_status = UNAVAILABLE;
			ret = SUIT_ERR_MANIFEST_VALIDATION;
		}
	} else {
		state->decoded_manifest->dependency_resolution_seq_status = UNAVAILABLE;
	}

	if (ret == SUIT_SUCCESS) {
		state->step = SEQUENCES_DECODED;
	} else  {
		suit_decoder_reset_state(state);
	}

	return ret;
}

int suit_decoder_create_components(struct suit_decoder_state *state)
{
	struct SUIT_Common *common = NULL;
	int ret = SUIT_SUCCESS;
	struct zcbor_string component_id;
	struct zcbor_string component_prefix;

	if (state == NULL) {
		return SUIT_ERR_DECODING;
	}

	if (state->step != SEQUENCES_DECODED) {
		return SUIT_ERR_ORDER;
	}

	common = &state->manifest._SUIT_Manifest_suit_common_cbor;

	/* Verify list of components */
	if (common->_SUIT_Common_suit_components_present) {
		/* Assign component handles */
		for (int i = 0; i < common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier_count; i++) {
			/* Zip list of strings into a single ZCBOR string */
			ret = get_component_id_str(&component_id,
				&common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier[i]);
			if (ret != SUIT_SUCCESS) {
				break;
			}

			/* Check if the component is on the list of dependencies */
			int dependency = find_dependency_manifest_prefix(common, i, &component_prefix);

			if (dependency == SUIT_SUCCESS) {
				SUIT_DBG("Append dependency manifest component (%d)\r\n", i);
				ret = suit_manifest_append_dependency(state->decoded_manifest, &component_id, &component_prefix);
			} else if (dependency == SUIT_ERR_MISSING_COMPONENT) {
				SUIT_DBG("Append regular component (%d)\r\n", i);
				ret = suit_manifest_append_component(state->decoded_manifest, &component_id);
			} else {
				SUIT_ERR("Unable to find dependency info for component %d\r\n", i);
				ret = dependency;
			}

			if (ret != SUIT_SUCCESS) {
				break;
			}
		}
		SUIT_DBG("Manifest component handles created (%d)\r\n", ret);
	}

	if (ret == SUIT_SUCCESS) {
		state->step = COMPONENTS_CREATED;
	} else  {
		(void)suit_manifest_release(state->decoded_manifest);
		suit_decoder_reset_state(state);
	}

	return ret;
}
