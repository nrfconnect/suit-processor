/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */


#include <manifest_decode.h>
#include <suit_types.h>
#include <suit_platform.h>
#include <suit_command_seq.h>
#include <suit_platform.h>
#include <cose_encode.h>
#include <cose_decode.h>
#include <suit_directive.h>

/** Calculate the length of the CBOR byte string header
 */
static size_t header_len(size_t num_elems)
{
	if (num_elems <= 23) {
		return 1;
	} else if (num_elems <= 255) {
		return 2;
	} else if (num_elems <= 0xFFFF) {
		return 3;
	} else if (num_elems <= 0xFFFFFFFF) {
		return 5;
	} else {
		return 9;
	}
}


/** Reset the iternal state of the SUIT manifest processor.
 */
void suit_reset_state(struct suit_processor_state *state)
{
	memset(state, 0, sizeof(*state));
	state->envelope_decoded = suit_bool_false;
	state->envelope_validated = suit_bool_false;
	state->manifest_authenticated = suit_bool_false;
	state->manifest_decoded = suit_bool_false;
	state->manifest_validated = suit_bool_false;
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	state->dry_run = suit_bool_true;
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
	suit_plat_reset_components();
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		if (state->components[i].component_handle) {
			(void)suit_plat_release_component_handle(state->components[i].component_handle);
		}
	}
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
}

/** Decode the string into a manifest envelope and validate the data structure.
 */
int suit_decode_envelope(uint8_t *envelope_str, size_t envelope_len,
	struct suit_processor_state *state)
{
	size_t decoded_len = 0;
	int ret;

	state->envelope_decoded = suit_bool_false;
	state->envelope_validated = suit_bool_false;

	if (state->envelope_decoded != suit_bool_false
		|| state->envelope_validated != suit_bool_false) {
		goto tamp;
	}

	ret = cbor_decode_SUIT_Envelope_Tagged(
		envelope_str, envelope_len, &state->envelope, &decoded_len);

	/* For development, condition on expected envelope size was modified. 
	   Now envelope_len represents max allowed size of envelope */
	if ((ret != ZCBOR_SUCCESS) || (decoded_len > envelope_len)) {
		state->envelope_decoded = suit_bool_false;
		return ZCBOR_ERR_TO_SUIT_ERR(ret);
	}

	if (ret == ZCBOR_SUCCESS) {
		state->envelope_decoded = suit_bool_true;
		state->envelope_str.value = envelope_str;
		state->envelope_str.len = envelope_len;
		return SUIT_SUCCESS;
	}

tamp:
	state->envelope_decoded = suit_bool_false;
	state->envelope_validated = suit_bool_false;
	return SUIT_ERR_TAMP;
}


static int cose_verify_digest(struct zcbor_string *digest_bstr, struct zcbor_string *data_bstr)
{
	struct SUIT_Digest digest;
	size_t bytes_processed;

	int ret = cbor_decode_SUIT_Digest(digest_bstr->value, digest_bstr->len, &digest, &bytes_processed);
	if ((ret != 0) || (bytes_processed != digest_bstr->len)) {
		return SUIT_ERR_DECODING;
	}

	/* Include CBOR header (type, length) in digest calculation */
	size_t offset = header_len(data_bstr->len);
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


static int cose_authenticate_digest(struct suit_processor_state *state, struct zcbor_string * COSE_Sign1_bstr, struct zcbor_string * digest_bstr)
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

	/* Construct Sig_structure1 structure */
	struct Sig_structure1 signature;
	signature._Sig_structure1_body_protected = cose_sign1_struct._COSE_Sign1__Headers._Headers_protected;
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
			&cose_sign1_struct._COSE_Sign1__Headers._Headers_protected_cbor._header_map_key_id._header_map_key_id:
			(struct zcbor_string *)NULL),
		/* Pass signature, specific for the key */
		&cose_sign1_struct._COSE_Sign1_signature,
		/* Authenticate Signature1 structure, including both algorithm ID and digest bytes of the manifest */
		&signed_bstr);
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
	/* Authenticate data using platform API */
	ret = suit_plat_authenticate_manifest(
		NULL, /* manifest_component_id */
		/* Value enforced by the input CDDL (cose_sign.cddl, supported_algs //= (ES256: -7)) */
		suit_cose_es256,
		(cose_sign1_struct._COSE_Sign1__Headers._Headers_protected_cbor._header_map_key_id_present ?
			&cose_sign1_struct._COSE_Sign1__Headers._Headers_protected_cbor._header_map_key_id._header_map_key_id:
			(struct zcbor_string *)NULL),
		/* Pass signature, specific for the key */
		&cose_sign1_struct._COSE_Sign1_signature,
		/* Authenticate Signature1 structure, including both algorithm ID and digest bytes of the manifest */
		&signed_bstr);
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */

	/* Store the key IDs that authenticated the digest */
	if (ret == SUIT_SUCCESS) {
		if (cose_sign1_struct._COSE_Sign1__Headers._Headers_protected_cbor._header_map_key_id_present) {
			state->key_ids[state->num_key_ids++] = &cose_sign1_struct._COSE_Sign1__Headers._Headers_protected_cbor._header_map_key_id._header_map_key_id;
		}
	}

	return ret;
}


int suit_validate_envelope(struct suit_processor_state *state)
{
	state->envelope_validated = suit_bool_false;

	if (state->envelope_decoded != suit_bool_true
		|| state->envelope_validated != suit_bool_false) {
		return SUIT_ERR_ORDER;
	}

	struct SUIT_Authentication *auth = &state->envelope._SUIT_Envelope_suit_authentication_wrapper_cbor;
	volatile int num_ok = 0;
	volatile enum suit_bool results[SUIT_MAX_NUM_SIGNERS * 2]; /* Use every other entry as canary */
	size_t i = 0;

	for (i = 0; i < SUIT_MAX_NUM_SIGNERS * 2; i++) {
		results[i] = suit_bool_false;
	}

	if (i != SUIT_MAX_NUM_SIGNERS * 2) {
		num_ok -= 4;
	} else {
		num_ok--;
	}

	if (auth->_SUIT_Authentication_bstr_count > SUIT_MAX_NUM_SIGNERS) {
		num_ok -= 4;
	} else {
		num_ok--;
	}

	if (auth->_SUIT_Authentication_bstr_count == 0) {
		num_ok -= 4;
	} else {
		num_ok--;
	}

	if (num_ok != -3) {
		return SUIT_ERR_MANIFEST_VERIFICATION;
	}
	if (num_ok == -3) {
		num_ok = 0;
	}

	/* Iterate through (key, signature) pairs */
	for (int i = 0; (num_ok == 0) && (i < auth->_SUIT_Authentication_bstr_count); i++) {
		int ret = cose_authenticate_digest(
			state,
			&auth->_SUIT_Authentication_bstr[i],
			&auth->_SUIT_Authentication_SUIT_Digest_bstr);

		/* If authentication for any key fails, drop the envelope */
		if (ret != SUIT_SUCCESS) {
			results[i * 2] = suit_bool_false;
		}

		if (ret == SUIT_SUCCESS) {
			results[i * 2] = suit_bool_true;
		}
	}

	for (int i = 0; i < auth->_SUIT_Authentication_bstr_count; i++) {
		if (results[i * 2] != suit_bool_true) {
			goto tamp;
		}
		if (results[i * 2 + 1] != suit_bool_false) {
			goto tamp;
		}
		if (results[i * 2] == suit_bool_true) {
			if (num_ok == i) {
				num_ok++;
				continue;
			}
			goto tamp;
		}
		goto tamp;
	}

	if (num_ok && (num_ok == auth->_SUIT_Authentication_bstr_count)) {
		/* Check the manifest digest against authenticated value */
		int ret = cose_verify_digest(
			&auth->_SUIT_Authentication_SUIT_Digest_bstr,
			&state->envelope._SUIT_Envelope_suit_manifest);

		if (ret != SUIT_SUCCESS) {
			state->envelope_validated = suit_bool_false;
			return ret;
		}

		if (ret == SUIT_SUCCESS) {
			state->envelope_validated = suit_bool_true;
			return ret;
		}
		goto tamp;
	}

tamp:
	state->envelope_validated = suit_bool_false;
	return SUIT_ERR_TAMP;
}


int suit_decode_manifest(struct suit_processor_state *state)
{
	size_t decoded_len = 0;
	int ret;

	if (state->envelope_decoded != suit_bool_true
		|| state->envelope_validated != suit_bool_true) {
		return SUIT_ERR_ORDER;
	}

	state->manifest_decoded = suit_bool_false;
	state->manifest_validated = suit_bool_false;

	if (state->manifest_decoded != suit_bool_false
		|| state->manifest_validated != suit_bool_false) {
		goto tamp;
	}

	ret = cbor_decode_SUIT_Manifest(
		state->envelope._SUIT_Envelope_suit_manifest.value,
		state->envelope._SUIT_Envelope_suit_manifest.len,
		&state->manifest, &decoded_len);

	if (ret != ZCBOR_SUCCESS
		|| decoded_len
			!= state->envelope._SUIT_Envelope_suit_manifest.len) {
		state->manifest_decoded = suit_bool_false;
		return ZCBOR_ERR_TO_SUIT_ERR(ret);
	}

	if (ret == ZCBOR_SUCCESS) {
		state->manifest_decoded = suit_bool_true;
		return SUIT_SUCCESS;
	}

tamp:
	state->manifest_decoded = suit_bool_false;
	state->manifest_validated = suit_bool_false;
	return SUIT_ERR_TAMP;
}


#define CHECK_RET(ret, on_success) \
	if (ret != SUIT_SUCCESS) { \
		return ret; \
	} \
 \
	if (ret == SUIT_SUCCESS) { \
		on_success; \
	} \
 \
	goto tamp;


static void get_component_id_str(struct zcbor_string *out_component_id,
	const struct SUIT_Component_Identifier * component_id)
{
	const struct zcbor_string * first_bstr = &component_id->_SUIT_Component_Identifier_bstr[0];
	size_t extra_header_len = header_len(component_id->_SUIT_Component_Identifier_bstr_count);
	size_t payload_len = 0;

	for (size_t i = 0; i < component_id->_SUIT_Component_Identifier_bstr_count; i++) {
		payload_len += header_len(component_id->_SUIT_Component_Identifier_bstr[i].len);
		payload_len += component_id->_SUIT_Component_Identifier_bstr[i].len;
	}

	out_component_id->value = first_bstr->value - extra_header_len - header_len(first_bstr->len);
	out_component_id->len = extra_header_len + payload_len;
}


static struct zcbor_string *get_command_seq(struct suit_processor_state *state,
	enum suit_manifest_step step)
{
	struct SUIT_Severable_Members_Choice_ *severable = &state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice;
	struct SUIT_Unseverable_Members_ *unseverable = &state->manifest._SUIT_Manifest__SUIT_Unseverable_Members;

	switch (step) {
		case SUIT_PAYLOAD_FETCH:
			if (severable->_SUIT_Severable_Members_Choice_suit_payload_fetch_present) {
				return &severable->_SUIT_Severable_Members_Choice_suit_payload_fetch._SUIT_Severable_Members_Choice_suit_payload_fetch;
			}
			return NULL;
		case SUIT_INSTALL:
			if (severable->_SUIT_Severable_Members_Choice_suit_install_present) {
				return &severable->_SUIT_Severable_Members_Choice_suit_install._SUIT_Severable_Members_Choice_suit_install;
			}
			return NULL;
		case SUIT_VALIDATE:
			if (unseverable->_SUIT_Unseverable_Members_suit_validate_present) {
				return &unseverable->_SUIT_Unseverable_Members_suit_validate._SUIT_Unseverable_Members_suit_validate;
			}
			return NULL;
		case SUIT_LOAD:
			if (unseverable->_SUIT_Unseverable_Members_suit_load_present) {
				return &unseverable->_SUIT_Unseverable_Members_suit_load._SUIT_Unseverable_Members_suit_load;
			}
			return NULL;
		case SUIT_INVOKE:
			if (unseverable->_SUIT_Unseverable_Members_suit_invoke_present) {
				return &unseverable->_SUIT_Unseverable_Members_suit_invoke._SUIT_Unseverable_Members_suit_invoke;
			}
			return NULL;
		default:
			return NULL;
	}
	return NULL;
}

/** @brief Reset all component's parameter as well as the list of active components.
 *
 * @param  state  Manifest processor state to be modified.
*/
static int prepare_params(struct suit_processor_state *state)
{
	int retval = SUIT_SUCCESS;

	/* Reset SUIT parameters */
	for (int i = 0; i < state->num_components; i++) {
		suit_reset_params(&state->components[i]);
	}

	return retval;
}


int suit_validate_manifest(struct suit_processor_state *state)
{
	struct SUIT_Common *common = NULL;
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	unsigned int num_dry_run_steps = 0;
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	if (state->envelope_decoded != suit_bool_true
		|| state->envelope_validated != suit_bool_true
		|| state->manifest_decoded != suit_bool_true) {
		SUIT_ERR("Manifest validation failed: Invalid order\r\n");
		return SUIT_ERR_ORDER;
	}

	state->manifest_validated = suit_bool_false;

	if (state->manifest_validated != suit_bool_false) {
		goto tamp;
	}

	/* Verify manifest version - enforced by the CDDL and checked by the ZCBOR parser code */

	/* Verify the manifest sequence number */
	do {
#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
		int ret = suit_plat_check_sequence_num(state->manifest._SUIT_Manifest_suit_manifest_sequence_number);
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
		int ret = suit_plat_authorize_sequence_num(
			state->current_step,
			NULL, /* manifest_component_id */
			state->manifest._SUIT_Manifest_suit_manifest_sequence_number);
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */

		CHECK_RET(ret, break);
	} while (0);
	SUIT_DBG("Manifest sequence number authorized\r\n");

	/* If the text field inside the envelope is present and the text field is severed from the manifest, verify the digest */
	while (state->envelope._SUIT_Envelope__SUIT_Severable_Manifest_Members._SUIT_Severable_Manifest_Members_suit_text_present
		&& state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice._SUIT_Severable_Members_Choice_suit_text_present) {

		int ret = suit_plat_check_digest(suit_cose_sha256,
			&state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice._SUIT_Severable_Members_Choice_suit_text._SUIT_Severable_Members_Choice_suit_text._SUIT_Digest_suit_digest_bytes,
			&state->envelope._SUIT_Envelope__SUIT_Severable_Manifest_Members._SUIT_Severable_Manifest_Members_suit_text._SUIT_Severable_Manifest_Members_suit_text);
		CHECK_RET(ret, break);
	}

	/* If the text field inside the envelope is present and the text field is not present inside the manifest, invalidate the envelope */
	if (state->envelope._SUIT_Envelope__SUIT_Severable_Manifest_Members._SUIT_Severable_Manifest_Members_suit_text_present
		&& (!state->manifest._SUIT_Manifest__SUIT_Severable_Members_Choice._SUIT_Severable_Members_Choice_suit_text_present)) {
		/* Text appears in envelope but not in manifest, so it is not signed */
		return SUIT_ERR_MANIFEST_VALIDATION;
	}
	SUIT_DBG("Manifest severable elements verified against digests\r\n");

	/* Verify common sequence */
	common = &state->manifest._SUIT_Manifest_suit_common_cbor;

	/* Verify list of components */
	if (common->_SUIT_Common_suit_components_present){
		struct zcbor_string component_id;

		/* Verify the length of the list */
		if (common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier_count > SUIT_MAX_NUM_COMPONENTS) {
			return SUIT_ERR_MANIFEST_VALIDATION;
		}

#ifndef SUIT_PLATFORM_LEGACY_API_SUPPORT
		/* Authorize component IDs */
		for (int i = 0; i < common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier_count; i++) {
			/* Zip list of strings into a single ZCBOR string */
			get_component_id_str(&component_id,
				&common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier[i]);
			int ret = suit_plat_authorize_component_id(NULL /* manifest_component_id */, &component_id);

			CHECK_RET(ret, continue);
		}
		SUIT_DBG("Manifest component handles authorized\r\n");
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */

		/* Assign component handles */
		for (int i = 0; i < common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier_count; i++) {
			/* Zip list of strings into a single ZCBOR string */
			get_component_id_str(&component_id,
				&common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier[i]);
#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
			int ret = suit_plat_create_component_handle(&component_id, state->key_ids, state->num_key_ids, &state->components[i].component_handle);
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
			int ret = suit_plat_create_component_handle(&component_id, &state->components[i].component_handle);
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */

			/* Increase the number of valid component indexes / handles */
			state->num_components++;

			CHECK_RET(ret, continue);
		}
		SUIT_DBG("Manifest component handles created\r\n");
	}


	/* Verify shared command sequence */
	while (common->_SUIT_Common_suit_shared_sequence_present){
		int ret = prepare_params(state);

		CHECK_RET(ret, break);

		ret = suit_validate_shared_sequence(state, &common->_SUIT_Common_suit_shared_sequence._SUIT_Common_suit_shared_sequence);

		CHECK_RET(ret, break);
	}
	SUIT_DBG("Manifest shared sequence validated\r\n");

	if (common != &state->manifest._SUIT_Manifest_suit_common_cbor) {
		goto tamp;
	}

	/* Verify manifest members */
	for (enum suit_manifest_step step = SUIT_NO_STEP + 1; step < SUIT_LAST_STEP; step++) {
		struct zcbor_string *step_seq = get_command_seq(state, step);

		int ret = prepare_params(state);
		if (ret != SUIT_SUCCESS) {
			return ret;
		}
		while (step_seq != NULL) {
			ret = suit_validate_command_sequence(state, step_seq);

			CHECK_RET(ret, break);
		}
		SUIT_DBG("Manifest sequence %d validated\r\n", step);
	}

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	/* Execute dry run over all manifest members */
	state->dry_run = suit_bool_true;

	for (enum suit_manifest_step step = SUIT_NO_STEP + 1; step < SUIT_LAST_STEP; step++) {
		struct zcbor_string *step_seq = get_command_seq(state, step);
		int ret = SUIT_ERR_TAMP;

		if (step_seq == NULL) {
			/* The lack of a command sequence (i.e. fetch step) does not invalidate the whole manifest */
			ret = SUIT_SUCCESS;
		}

		if (step_seq != NULL) {
			ret = prepare_params(state);
			if (ret != SUIT_SUCCESS) {
				return ret;
			}

			/* Execute shared command sequence */
			while (common->_SUIT_Common_suit_shared_sequence_present){
				ret = suit_run_command_sequence(state, &common->_SUIT_Common_suit_shared_sequence._SUIT_Common_suit_shared_sequence);

				CHECK_RET(ret, break);
			}

			ret = suit_run_command_sequence(state, step_seq);
			SUIT_DBG("Manifest sequence %d dru-run completed\r\n", step);
		}

		if (ret != SUIT_SUCCESS) {
			SUIT_ERR("Manifest sequence %d dry-run failed\r\n", step);
			return ret;
		} else if (ret == SUIT_SUCCESS) {
			num_dry_run_steps++;
		}
	}

	if (num_dry_run_steps != SUIT_NUM_STEPS) {
		SUIT_DBG("Manifest validation failed\r\n");
		goto tamp;
	} else if (num_dry_run_steps == SUIT_NUM_STEPS) {
		SUIT_DBG("Manifest validation finished\r\n");
		state->manifest_validated = suit_bool_true;
		state->dry_run = suit_bool_false;
		return SUIT_SUCCESS;
	}
#else /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
	SUIT_DBG("Manifest validation finished\r\n");
	state->manifest_validated = suit_bool_true;
	return SUIT_SUCCESS;
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

tamp:
	state->manifest_validated = suit_bool_false;
	return SUIT_ERR_TAMP;
}


int suit_process_manifest(struct suit_processor_state *state,
	enum suit_manifest_step step)
{
	int ret = SUIT_ERR_TAMP;

	if (state->envelope_decoded != suit_bool_true
		|| state->envelope_validated != suit_bool_true
		|| state->manifest_decoded != suit_bool_true
		|| state->manifest_validated != suit_bool_true) {
		return SUIT_ERR_ORDER;
	}

	state->current_step = step;

	struct zcbor_string *step_seq = get_command_seq(state, step);

	if (step_seq == NULL) {
		return SUIT_ERR_UNAVAILABLE_COMMAND_SEQ;
	}

	if (step_seq != NULL) {
		ret = prepare_params(state);
		if (ret != SUIT_SUCCESS) {
			return ret;
		}

		/* Execute shared command sequence */
		struct SUIT_Common *common = &state->manifest._SUIT_Manifest_suit_common_cbor;
		while (common->_SUIT_Common_suit_shared_sequence_present){
			ret = suit_run_command_sequence(state, &common->_SUIT_Common_suit_shared_sequence._SUIT_Common_suit_shared_sequence);

			CHECK_RET(ret, break);
		}

		ret = suit_run_command_sequence(state, step_seq);

#ifndef SUIT_PLATFORM_LEGACY_API_SUPPORT
		if (ret == SUIT_SUCCESS) {
			ret = suit_plat_sequence_completed(state->current_step,
				NULL  /* manifest_component_id */,
				state->envelope_str.value,
				state->envelope_str.len);
		}
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */

		return ret;
	}

tamp:
	return SUIT_ERR_TAMP;
}
