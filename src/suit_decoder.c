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

/** Extract the major type, i.e. the first 3 bits of the header byte. */
#define MAJOR_TYPE(header_byte) ((zcbor_major_type_t)(((header_byte) >> 5) & 0x7))

/** Calculate the length of the CBOR byte string and array header
 */
static int header_len(size_t len, const uint8_t *value, zcbor_major_type_t major_type)
{
	int exp_len = zcbor_header_len(len);

	if (MAJOR_TYPE(*(value - exp_len)) != major_type) {
		return -1;
	}

	return exp_len;
}

static int verify_suit_digest(struct SUIT_Digest *digest, struct zcbor_string *data_bstr)
{
	/* Include CBOR header (type, length) in digest calculation */
	int offset = header_len(data_bstr->len, &data_bstr->value[0], ZCBOR_MAJOR_TYPE_BSTR);

	if (offset < 0) {
		return SUIT_ERR_DECODING;
	}

	if (digest->SUIT_Digest_suit_digest_algorithm_id.suit_cose_hash_algs_choice == suit_cose_hash_algs_cose_alg_sha_256_m_c) {
		/* The SHA256 algorithm is allowed by CDDL. Verify the digest length. */
		if (digest->SUIT_Digest_suit_digest_bytes.len != 32) {
			return SUIT_ERR_DECODING;
		}
	} else if (digest->SUIT_Digest_suit_digest_algorithm_id.suit_cose_hash_algs_choice == suit_cose_hash_algs_cose_alg_sha_512_m_c) {
		/* The SHA512 algorithm is allowed by CDDL. Verify the digest length. */
		if (digest->SUIT_Digest_suit_digest_bytes.len != 64) {
			return SUIT_ERR_DECODING;
		}
	} else {
		/* Other algorithms are not supported. */
		return SUIT_ERR_UNSUPPORTED_ALG;
	}

	struct zcbor_string data_bytes = {
		.value = data_bstr->value - offset,
		.len = data_bstr->len + offset,
	};

	return suit_plat_check_digest(
		digest->SUIT_Digest_suit_digest_algorithm_id.suit_cose_hash_algs_choice,
		&digest->SUIT_Digest_suit_digest_bytes,
		&data_bytes);
}

static int cose_verify_digest(struct zcbor_string *digest_bstr, struct zcbor_string *data_bstr)
{
	struct SUIT_Digest digest = {0};
	size_t bytes_processed = 0;

	int ret = cbor_decode_SUIT_Digest(digest_bstr->value, digest_bstr->len, &digest, &bytes_processed);

	if ((ret != 0) || (bytes_processed != digest_bstr->len)) {
		return SUIT_ERR_DECODING;
	}

	return verify_suit_digest(
		&digest,
		data_bstr
	);
}

static int cose_sign1_authenticate_digest(struct zcbor_string *manifest_component_id, struct zcbor_string *COSE_Sign1_bstr, struct zcbor_string *digest_bstr)
{
	uint8_t signed_data[SUIT_SUIT_SIG_STRUCTURE1_MAX_LENGTH];
	size_t signed_data_size = 0;

	/* Decode COSE_Sign1 structure */
	struct COSE_Sign1 cose_sign1_struct = {0};
	size_t cose_sign1_struct_size = 0;

	int ret = cbor_decode_COSE_Sign1_Tagged(
		COSE_Sign1_bstr->value,
		COSE_Sign1_bstr->len,
		&cose_sign1_struct,
		&cose_sign1_struct_size);
	if ((ret != ZCBOR_SUCCESS) || (cose_sign1_struct_size != COSE_Sign1_bstr->len)) {
		return SUIT_ERR_UNSUPPORTED_COSE;
	}

	/* Both ES256 and EdDSA25519 algorithms which are currently supported by CDDL
	   produce a signature of length 64. */
	if (cose_sign1_struct.COSE_Sign1_signature.len != 64) {
		return SUIT_ERR_UNSUPPORTED_ALG;
	}

	/* Construct Sig_structure1 structure */
	struct Sig_structure1 signature;

	memset(&signature, 0, sizeof(signature));
	signature.Sig_structure1_body_protected_cbor = cose_sign1_struct.COSE_Sign1_Headers_m.Headers_protected_cbor;
	signature.Sig_structure1_payload = *digest_bstr;

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

	/* Authenticate data using platform API */
	ret = suit_plat_authenticate_manifest(
		manifest_component_id,
		cose_sign1_struct.COSE_Sign1_Headers_m.Headers_protected_cbor.header_map_alg_id.supported_algs_choice,
		(cose_sign1_struct.COSE_Sign1_Headers_m.Headers_protected_cbor.header_map_key_id_present ?
			&cose_sign1_struct.COSE_Sign1_Headers_m.Headers_protected_cbor.header_map_key_id.header_map_key_id :
			(struct zcbor_string *)NULL),
		/* Pass signature, specific for the key */
		&cose_sign1_struct.COSE_Sign1_signature,
		/* Authenticate Signature1 structure, including both algorithm ID and digest bytes of the manifest */
		&signed_bstr);

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
	const struct zcbor_string *first_bstr = &component_id->SUIT_Component_Identifier_bstr[0];
	int first_bstr_len = header_len(first_bstr->len, &first_bstr->value[0], ZCBOR_MAJOR_TYPE_BSTR);

	if (first_bstr_len < 0) {
		return SUIT_ERR_DECODING;
	}

	int extra_header_len = header_len(component_id->SUIT_Component_Identifier_bstr_count, &(first_bstr->value[0]) - first_bstr_len, ZCBOR_MAJOR_TYPE_LIST);

	if (extra_header_len < 0) {
		return SUIT_ERR_DECODING;
	}

	/* Do checks on component ID parts. */
	for (int i = 0; i < component_id->SUIT_Component_Identifier_bstr_count; i++) {
		const struct zcbor_string *comp_bstr = &component_id->SUIT_Component_Identifier_bstr[i];

		if (header_len(comp_bstr->len, &comp_bstr->value[0], ZCBOR_MAJOR_TYPE_BSTR) < 0
				|| comp_bstr->len == 0) {
			return SUIT_ERR_DECODING;
		}
	}

	const struct zcbor_string *last_bstr = &component_id->SUIT_Component_Identifier_bstr[
					component_id->SUIT_Component_Identifier_bstr_count - 1];

	out_component_id->value = first_bstr->value - extra_header_len - first_bstr_len;
	out_component_id->len = last_bstr->value + last_bstr->len - out_component_id->value;

	return SUIT_SUCCESS;
}

static int check_dependency_indexes(struct SUIT_Common *common)
{
	const struct SUIT_Dependencies *dependencies = NULL;

	if (common->SUIT_Common_extensions_m_present) {
		const struct SUIT_Common_extensions_m *ext = &common->SUIT_Common_extensions_m;
		const struct SUIT_Common_extensions_suit_dependencies *dependencies_ext = &ext->SUIT_Common_extensions_m;

		dependencies = &dependencies_ext->SUIT_Common_extensions_suit_dependencies;
	}

	if (dependencies == NULL) {
		return SUIT_ERR_MISSING_COMPONENT;
	}

	if (!common->SUIT_Common_suit_components_present) {
		/* The presence of suit-common is already enforced by suit_decoder_authorize_manifest(..) API. */
		return SUIT_ERR_TAMP;
	}

	const struct SUIT_Components *components = &common->SUIT_Common_suit_components.SUIT_Common_suit_components;
	uint_fast32_t max_index = components->SUIT_Components_SUIT_Component_Identifier_m_count;

	for (size_t dep_i = 0; dep_i < dependencies->SUIT_Dependencies_SUIT_Dependency_Metadata_m_count; dep_i++) {
		const struct SUIT_Dependencies_SUIT_Dependency_Metadata_m *meta = &dependencies->SUIT_Dependencies_SUIT_Dependency_Metadata_m[dep_i];

		if (meta->SUIT_Dependencies_SUIT_Dependency_Metadata_m_key >= max_index) {
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

	if (common->SUIT_Common_extensions_m_present) {
		const struct SUIT_Common_extensions_m *ext = &common->SUIT_Common_extensions_m;
		const struct SUIT_Common_extensions_suit_dependencies *dependencies_ext = &ext->SUIT_Common_extensions_m;

		dependencies = &dependencies_ext->SUIT_Common_extensions_suit_dependencies;
	}

	if (dependencies == NULL) {
		return SUIT_ERR_MISSING_COMPONENT;
	}

	for (size_t dep_i = 0; dep_i < dependencies->SUIT_Dependencies_SUIT_Dependency_Metadata_m_count; dep_i++) {
		const struct SUIT_Dependencies_SUIT_Dependency_Metadata_m *meta = &dependencies->SUIT_Dependencies_SUIT_Dependency_Metadata_m[dep_i];

		if (index == meta->SUIT_Dependencies_SUIT_Dependency_Metadata_m_key) {
			const struct SUIT_Dependency_Metadata *prefix_cbor = &meta->SUIT_Dependencies_SUIT_Dependency_Metadata_m;

			if (prefix_cbor->SUIT_Dependency_Metadata_suit_dependency_prefix_present) {
				/* Zip list of strings into a single ZCBOR string */
				int ret = get_component_id_str(prefix,
					&prefix_cbor->SUIT_Dependency_Metadata_suit_dependency_prefix.SUIT_Dependency_Metadata_suit_dependency_prefix);
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

int suit_decoder_decode_envelope(struct suit_decoder_state *state, const uint8_t *envelope_str, size_t envelope_len)
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
		(uint8_t *)envelope_str, envelope_len, &state->envelope, &decoded_len);

	/* For development, condition on expected envelope size was modified.
	   Now envelope_len represents max allowed size of envelope */
	if ((ret != ZCBOR_SUCCESS) || (decoded_len > envelope_len)) {
		ret = ZCBOR_ERR_TO_SUIT_ERR((ret != ZCBOR_SUCCESS) ? ret : ZCBOR_ERR_UNKNOWN);
	}
	else
	{
		ret = SUIT_SUCCESS;
	}

	if (ret == SUIT_SUCCESS) {
		struct SUIT_Authentication *auth = &state->envelope.SUIT_Envelope_suit_authentication_wrapper_cbor;

		if ((auth->SUIT_Authentication_bstr_count > SUIT_MAX_NUM_SIGNERS) ||
		    (state->envelope.SUIT_Envelope_SUIT_Integrated_Payload_m_count > SUIT_MAX_NUM_INTEGRATED_PAYLOADS)) {
			ret = SUIT_ERR_DECODING;
		}

		if (ret == SUIT_SUCCESS) {
			/* Iterate through (key, signature) pairs */
			for (int i = 0; i < auth->SUIT_Authentication_bstr_count; i++) {
				state->authentication_bstr[i] = auth->SUIT_Authentication_bstr[i];
			}
			state->authentication_bstr_count = auth->SUIT_Authentication_bstr_count;
			state->manifest_digest_bytes = auth->SUIT_Authentication_SUIT_Digest_bstr;
		}

		/* Store pointers to the severable sequences for further verification and execution. */
		if (state->envelope.SUIT_Envelope_SUIT_Severable_Manifest_Members_m.SUIT_Severable_Manifest_Members_suit_text_present) {
			state->decoded_manifest->text = state->envelope.SUIT_Envelope_SUIT_Severable_Manifest_Members_m.SUIT_Severable_Manifest_Members_suit_text.SUIT_Severable_Manifest_Members_suit_text;
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

		if (state->envelope.SUIT_Envelope_SUIT_Severable_Manifest_Members_m.SUIT_Severable_Manifest_Members_suit_payload_fetch_present) {
			state->decoded_manifest->payload_fetch_seq = state->envelope.SUIT_Envelope_SUIT_Severable_Manifest_Members_m.SUIT_Severable_Manifest_Members_suit_payload_fetch.SUIT_Severable_Manifest_Members_suit_payload_fetch;
			if (state->decoded_manifest->payload_fetch_seq.len < 1) {
				ret = SUIT_ERR_DECODING;
				state->decoded_manifest->payload_fetch_seq.len = 0;
				state->decoded_manifest->payload_fetch_seq.value = NULL;
			} else {
				state->decoded_manifest->payload_fetch_seq_status = SEVERED;
			}
		}

		if (state->envelope.SUIT_Envelope_SUIT_Severable_Manifest_Members_m.SUIT_Severable_Manifest_Members_suit_install_present) {
			state->decoded_manifest->install_seq = state->envelope.SUIT_Envelope_SUIT_Severable_Manifest_Members_m.SUIT_Severable_Manifest_Members_suit_install.SUIT_Severable_Manifest_Members_suit_install;
			if (state->decoded_manifest->install_seq.len < 1) {
				ret = SUIT_ERR_DECODING;
				state->decoded_manifest->install_seq.len = 0;
				state->decoded_manifest->install_seq.value = NULL;
			} else {
				state->decoded_manifest->install_seq_status = SEVERED;
			}
		}

		const size_t ext_count = state->envelope.SUIT_Envelope_SUIT_Severable_Manifest_Members_m.SUIT_Severable_Manifest_Members_SUIT_severable_members_extensions_m_count;
		for (size_t ext_i = 0; ext_i < ext_count; ext_i++) {
			struct SUIT_severable_members_extensions_r *ext = &state->envelope.SUIT_Envelope_SUIT_Severable_Manifest_Members_m.SUIT_Severable_Manifest_Members_SUIT_severable_members_extensions_m[ext_i].SUIT_Severable_Manifest_Members_SUIT_severable_members_extensions_m;

			switch (ext->SUIT_severable_members_extensions_choice) {
				case SUIT_severable_members_extensions_suit_dependency_resolution_c:
					state->decoded_manifest->dependency_resolution_seq = ext->SUIT_severable_members_extensions_suit_dependency_resolution;
					if (state->decoded_manifest->dependency_resolution_seq.len < 1) {
						ret = SUIT_ERR_DECODING;
						state->decoded_manifest->dependency_resolution_seq.len = 0;
						state->decoded_manifest->dependency_resolution_seq.value = NULL;
					} else {
						state->decoded_manifest->dependency_resolution_seq_status = SEVERED;
					}

					break;

				case SUIT_severable_members_extensions_suit_candidate_verification_c:
					state->decoded_manifest->candidate_verification_seq = ext->SUIT_severable_members_extensions_suit_candidate_verification;
					if (state->decoded_manifest->candidate_verification_seq.len < 1) {
						ret = SUIT_ERR_DECODING;
						state->decoded_manifest->candidate_verification_seq.len = 0;
						state->decoded_manifest->candidate_verification_seq.value = NULL;
					} else {
						state->decoded_manifest->candidate_verification_seq_status = SEVERED;
					}

					break;

				default:
					ret = SUIT_ERR_DECODING;
					break;
			}

			if (ret != SUIT_SUCCESS) {
				break;
			}
		}

		/* Store pointers to the integrated payloads and their keys. */
		if (ret == SUIT_SUCCESS) {
			for (size_t i = 0; i < state->envelope.SUIT_Envelope_SUIT_Integrated_Payload_m_count; i++) {
				state->decoded_manifest->integrated_payloads[i].key = state->envelope.SUIT_Envelope_SUIT_Integrated_Payload_m[i].SUIT_Envelope_SUIT_Integrated_Payload_m.SUIT_Integrated_Payload_suit_integrated_payload_key_key;
				state->decoded_manifest->integrated_payloads[i].payload = state->envelope.SUIT_Envelope_SUIT_Integrated_Payload_m[i].SUIT_Envelope_SUIT_Integrated_Payload_m.SUIT_Integrated_Payload_suit_integrated_payload_key;
			}
			state->decoded_manifest->integrated_payloads_count = state->envelope.SUIT_Envelope_SUIT_Integrated_Payload_m_count;

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

	manifest_bstr = state->envelope.SUIT_Envelope_suit_manifest;
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

	manifest_bstr = state->envelope.SUIT_Envelope_suit_manifest;

	/* Verify manifest version - enforced by the CDDL and checked by the ZCBOR parser code */

	ret = cbor_decode_SUIT_Manifest(
		manifest_bstr.value,
		manifest_bstr.len,
		&state->manifest,
		&decoded_len);

	if ((ret != ZCBOR_SUCCESS) || decoded_len != manifest_bstr.len) {
		ret = ZCBOR_ERR_TO_SUIT_ERR((ret != ZCBOR_SUCCESS) ? ret : ZCBOR_ERR_UNKNOWN);
	}
	else
	{
		ret = SUIT_SUCCESS;
	}

	if (ret == SUIT_SUCCESS) {
		if (state->manifest.SUIT_Manifest_Extensions_m_present) {
			const struct SUIT_Manifest_Extensions_m *ext = &state->manifest.SUIT_Manifest_Extensions_m;
			const struct SUIT_Manifest_Extensions_suit_manifest_component_id *component_id_ext = &ext->SUIT_Manifest_Extensions_m;
			const struct SUIT_Component_Identifier *manifest_component = &component_id_ext->SUIT_Manifest_Extensions_suit_manifest_component_id;

			ret = get_component_id_str(&state->decoded_manifest->manifest_component_id, manifest_component);
		}
	}

	if (ret == SUIT_SUCCESS) {
		/* Cannot perform universal sequence number authorization.
		 * Skip this check in decoder logic and store the sequence number value inside the output structure.
		 */
		state->decoded_manifest->sequence_number = state->manifest.SUIT_Manifest_suit_manifest_sequence_number;
	}

	if (ret == SUIT_SUCCESS) {
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
		ret = suit_plat_authorize_unsigned_manifest(&state->decoded_manifest->manifest_component_id);
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

	if (ret == SUIT_SUCCESS) {
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

	/* Verify common sequence */
	common = &state->manifest.SUIT_Manifest_suit_common_cbor;

	/* Verify list of components */
	if (common->SUIT_Common_suit_components_present) {
		struct zcbor_string component_id;

		/* Verify the length of the list */
		if ((common->SUIT_Common_suit_components.SUIT_Common_suit_components.SUIT_Components_SUIT_Component_Identifier_m_count > SUIT_MAX_NUM_COMPONENTS) ||
		    (common->SUIT_Common_suit_components.SUIT_Common_suit_components.SUIT_Components_SUIT_Component_Identifier_m_count < 1)) {
			return SUIT_ERR_MANIFEST_VALIDATION;
		}

		/* Authorize component IDs */
		for (int i = 0; i < common->SUIT_Common_suit_components.SUIT_Common_suit_components.SUIT_Components_SUIT_Component_Identifier_m_count; i++) {
			/* Zip list of strings into a single ZCBOR string */
			ret = get_component_id_str(&component_id,
				&common->SUIT_Common_suit_components.SUIT_Common_suit_components.SUIT_Components_SUIT_Component_Identifier_m[i]);

			if (ret == SUIT_SUCCESS) {
				ret = suit_plat_authorize_component_id(&state->decoded_manifest->manifest_component_id, &component_id);
			}

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

	if (ret == SUIT_SUCCESS) {
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

#define UNSEVERABLE_SEQUENCE_DECODE(sequence) \
	if (state->manifest.SUIT_Manifest_SUIT_Unseverable_Members_m.SUIT_Unseverable_Members_suit_##sequence##_present) { \
		if (state->decoded_manifest->sequence##_seq_status == UNAVAILABLE) { \
			state->decoded_manifest->sequence##_seq = state->manifest.SUIT_Manifest_SUIT_Unseverable_Members_m.SUIT_Unseverable_Members_suit_##sequence.SUIT_Unseverable_Members_suit_##sequence; \
			state->decoded_manifest->sequence##_seq_status = AUTHENTICATED; \
		} else { \
			state->decoded_manifest->sequence##_seq_status = UNAVAILABLE; \
			ret = SUIT_ERR_MANIFEST_VALIDATION; \
		} \
	} else { \
		state->decoded_manifest->sequence##_seq_status = UNAVAILABLE; \
	}

#define SEVERABLE_SEQUENCE_DECODE(sequence) \
	if (state->manifest.SUIT_Manifest_SUIT_Severable_Members_Choice_m.SUIT_Severable_Members_Choice_suit_##sequence##_present) { \
		if (state->manifest.SUIT_Manifest_SUIT_Severable_Members_Choice_m.SUIT_Severable_Members_Choice_suit_##sequence.SUIT_Severable_Members_Choice_suit_##sequence##_choice \
			== SUIT_Severable_Members_Choice_suit_##sequence##_SUIT_Command_Sequence_bstr_c \
		    && state->decoded_manifest->sequence##_seq_status == UNAVAILABLE) { \
			state->decoded_manifest->sequence##_seq = state->manifest.SUIT_Manifest_SUIT_Severable_Members_Choice_m.SUIT_Severable_Members_Choice_suit_##sequence.SUIT_Severable_Members_Choice_suit_##sequence##_SUIT_Command_Sequence_bstr; \
			state->decoded_manifest->sequence##_seq_status = AUTHENTICATED; \
		} \
		else if (state->manifest.SUIT_Manifest_SUIT_Severable_Members_Choice_m.SUIT_Severable_Members_Choice_suit_##sequence.SUIT_Severable_Members_Choice_suit_##sequence##_choice \
			== SUIT_Severable_Members_Choice_suit_##sequence##_SUIT_Digest_m_c) { \
			if (state->decoded_manifest->sequence##_seq_status == SEVERED) { \
				int digest_ret = verify_suit_digest( \
					&state->manifest.SUIT_Manifest_SUIT_Severable_Members_Choice_m.SUIT_Severable_Members_Choice_suit_##sequence.SUIT_Severable_Members_Choice_suit_##sequence##_SUIT_Digest_m, \
					&state->decoded_manifest->sequence##_seq \
				); \
				if (digest_ret == SUIT_SUCCESS) { \
					state->decoded_manifest->sequence##_seq_status = AUTHENTICATED; \
				} else { \
					state->decoded_manifest->sequence##_seq_status = UNAVAILABLE; \
					ret = SUIT_ERR_MANIFEST_VALIDATION; \
				} \
			} else { \
				/* The sequence digest is present inside the manifest, but it cannot be verified, \
				 * because the sequence has been severed. \
				 * This can be a valid case, i.e., once minified envelope is transferred into the \
				 * SUIT storage partition. \
				 * However, if an attempt to run the sequence is made, an error should be issued, \
				 * instead of treating the sequence as unavailable. \
				 */ \
				state->decoded_manifest->sequence##_seq_status = SEVERED; \
			} \
		} else { \
			state->decoded_manifest->sequence##_seq_status = UNAVAILABLE; \
			ret = SUIT_ERR_MANIFEST_VALIDATION; \
		} \
	} else { \
		state->decoded_manifest->sequence##_seq_status = UNAVAILABLE; \
	}

#define SEVERABLE_EXTENSION_SEQUENCE_DECODE(sequence) \
	if (ext->severable_manifest_members_choice_extensions_suit_##sequence##_choice \
		== severable_manifest_members_choice_extensions_suit_##sequence##_SUIT_Command_Sequence_bstr_c \
	    && state->decoded_manifest->sequence##_seq_status == UNAVAILABLE) { \
		state->decoded_manifest->sequence##_seq = ext->severable_manifest_members_choice_extensions_suit_##sequence##_SUIT_Command_Sequence_bstr; \
		state->decoded_manifest->sequence##_seq_status = AUTHENTICATED; \
	} \
	else if (ext->severable_manifest_members_choice_extensions_suit_##sequence##_choice \
					== severable_manifest_members_choice_extensions_suit_##sequence##_SUIT_Digest_m_c) { \
		if (state->decoded_manifest->sequence##_seq_status == SEVERED) { \
			int digest_ret = verify_suit_digest( \
				&ext->severable_manifest_members_choice_extensions_suit_##sequence##_SUIT_Digest_m, \
				&state->decoded_manifest->sequence##_seq \
			); \
			if (digest_ret == SUIT_SUCCESS) { \
				state->decoded_manifest->sequence##_seq_status = AUTHENTICATED; \
			} else { \
				state->decoded_manifest->sequence##_seq_status = UNAVAILABLE; \
				ret = SUIT_ERR_MANIFEST_VALIDATION; \
			} \
		} else { \
			/* The sequence digest is present inside the manifest, but it cannot be verified, \
			 * because the sequence has been severed. \
			 * This can be a valid case, i.e., once minified envelope is transferred into the \
			 * SUIT storage partition. \
			 * However, if an attempt to run the sequence is made, an error should be issued, \
			 * instead of treating the sequence as unavailable. \
			 */ \
			state->decoded_manifest->sequence##_seq_status = SEVERED; \
		} \
	} else { \
		state->decoded_manifest->sequence##_seq_status = UNAVAILABLE; \
		ret = SUIT_ERR_MANIFEST_VALIDATION; \
	} \

int suit_decoder_decode_sequences(struct suit_decoder_state *state)
{
	int ret = SUIT_SUCCESS;

	if (state == NULL) {
		return SUIT_ERR_DECODING;
	}

	if (state->step != MANIFEST_AUTHORIZED) {
		return SUIT_ERR_ORDER;
	}

	if (state->manifest.SUIT_Manifest_suit_common_cbor.SUIT_Common_suit_shared_sequence_present) {
		if (state->decoded_manifest->shared_sequence_status == UNAVAILABLE) {
			state->decoded_manifest->shared_sequence = state->manifest.SUIT_Manifest_suit_common_cbor.SUIT_Common_suit_shared_sequence.SUIT_Common_suit_shared_sequence;
			state->decoded_manifest->shared_sequence_status = AUTHENTICATED;
		} else {
			state->decoded_manifest->shared_sequence_status = UNAVAILABLE;
			ret = SUIT_ERR_MANIFEST_VALIDATION;
		}
	} else {
		state->decoded_manifest->shared_sequence_status = UNAVAILABLE;
		ret = SUIT_ERR_MANIFEST_VALIDATION;
	}


	UNSEVERABLE_SEQUENCE_DECODE(validate);
	UNSEVERABLE_SEQUENCE_DECODE(load);
	UNSEVERABLE_SEQUENCE_DECODE(invoke);

	SEVERABLE_SEQUENCE_DECODE(payload_fetch)
	SEVERABLE_SEQUENCE_DECODE(install)

	/* The CDDL enforces to severe the text field from the manifest.
	 * Due to that fact, if the text field is present inside the manifest, it contains the digest of the severed text field.
	 * If the text field was found before (status SEVERED), verify its digest.
	 */
	if (state->manifest.SUIT_Manifest_SUIT_Severable_Members_Choice_m.SUIT_Severable_Members_Choice_suit_text_present) {
		if (state->decoded_manifest->text_status == SEVERED) {
			int digest_ret = verify_suit_digest(
				&state->manifest.SUIT_Manifest_SUIT_Severable_Members_Choice_m.SUIT_Severable_Members_Choice_suit_text.SUIT_Severable_Members_Choice_suit_text,
				&state->decoded_manifest->text
			);

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
	const size_t ext_count = state->manifest.SUIT_Manifest_SUIT_Severable_Members_Choice_m.SUIT_Severable_Members_Choice_severable_manifest_members_choice_extensions_m_count;
	for (size_t ext_i = 0; ext_i < ext_count; ext_i++) {
		struct severable_manifest_members_choice_extensions_r *ext = &state->manifest.SUIT_Manifest_SUIT_Severable_Members_Choice_m.SUIT_Severable_Members_Choice_severable_manifest_members_choice_extensions_m[ext_i].SUIT_Severable_Members_Choice_severable_manifest_members_choice_extensions_m;

		switch (ext->severable_manifest_members_choice_extensions_choice) {
			case severable_manifest_members_choice_extensions_suit_dependency_resolution_c:
				SEVERABLE_EXTENSION_SEQUENCE_DECODE(dependency_resolution)
				break;

			case severable_manifest_members_choice_extensions_suit_candidate_verification_c:
				SEVERABLE_EXTENSION_SEQUENCE_DECODE(candidate_verification)
				break;

			default:
				ret = SUIT_ERR_DECODING;
				break;
		}

		if (ret != SUIT_SUCCESS) {
			break;
		}
	}

	if ((state->decoded_manifest->dependency_resolution_seq_status != AUTHENTICATED) &&
	    (state->decoded_manifest->dependency_resolution_seq_status != UNAVAILABLE)) {
		state->decoded_manifest->dependency_resolution_seq_status = UNAVAILABLE;
		if (ret == SUIT_SUCCESS) {
			ret = SUIT_ERR_MANIFEST_VALIDATION;
		}
	}

	if ((state->decoded_manifest->candidate_verification_seq_status != AUTHENTICATED) &&
	    (state->decoded_manifest->candidate_verification_seq_status != UNAVAILABLE)) {
		state->decoded_manifest->candidate_verification_seq_status = UNAVAILABLE;
		if (ret == SUIT_SUCCESS) {
			ret = SUIT_ERR_MANIFEST_VALIDATION;
		}
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

	common = &state->manifest.SUIT_Manifest_suit_common_cbor;

	/* Verify list of components */
	if (common->SUIT_Common_suit_components_present) {
		/* Assign component handles */
		for (int i = 0; i < common->SUIT_Common_suit_components.SUIT_Common_suit_components.SUIT_Components_SUIT_Component_Identifier_m_count; i++) {
			/* Zip list of strings into a single ZCBOR string */
			ret = get_component_id_str(&component_id,
				&common->SUIT_Common_suit_components.SUIT_Common_suit_components.SUIT_Components_SUIT_Component_Identifier_m[i]);
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
