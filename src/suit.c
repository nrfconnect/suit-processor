/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <manifest_decode.h>
#include <suit_types.h>
#include <suit_platform.h>
#include <suit_command_seq.h>
#include <suit_platform.h>


/** Decode the string into a manifest envelope and validate the data structure.
 */
int suit_decode_envelope(uint8_t *envelope_str, size_t envelope_len,
	struct suit_processor_state *state)
{
	state->envelope_decoded = suit_bool_false;
	state->envelope_validated = suit_bool_false;

	if (state->envelope_decoded != suit_bool_false
		|| state->envelope_validated != suit_bool_false) {
		return SUIT_ERR_TAMP;
	}

	size_t decoded_len;
	int ret = cbor_decode_SUIT_Envelope_Tagged(
		envelope_str, envelope_len, &state->envelope, &decoded_len);

	if (ret != ZCBOR_SUCCESS || decoded_len != envelope_len) {
		state->envelope_decoded = suit_bool_false;
		return ZCBOR_ERR_TO_SUIT_ERR(ret);
	}

	if (ret == ZCBOR_SUCCESS) {
		state->envelope_decoded = suit_bool_true;
		return SUIT_SUCCESS;
	}

	if (state->envelope_decoded != suit_bool_true) {
		return SUIT_ERR_TAMP;
	}

	return SUIT_ERR_TAMP;
}


int suit_validate_envelope(struct suit_processor_state *state)
{
	if (state->envelope_decoded != suit_bool_true
		|| state->envelope_validated != suit_bool_false) {
		return SUIT_ERR_ORDER;
	}

	struct SUIT_Authentication *auth = &state->envelope._SUIT_Envelope_suit_authentication_wrapper_cbor;
	enum suit_bool results[auth->_SUIT_Authentication_Block_bstr_count * 2]; /* Use every other entry as canary */
	// SUS: Variable-sized object cannot be initialized through assignment.
	for (size_t i = 0; i < auth->_SUIT_Authentication_Block_bstr_count * 2; i++) {
		results[i] = suit_bool_false;
	}
	unsigned int num_ok = 0;

	if (auth->_SUIT_Authentication_Block_bstr_count == 0) {
		return SUIT_ERR_MANIFEST_VERIFICATION;
	}

	for (int i = 0; i < auth->_SUIT_Authentication_Block_bstr_count; i++) {
		struct COSE_Sign1 *block = &auth->_SUIT_Authentication_Block_bstr[i]._SUIT_Authentication_Block_bstr_cbor;

		int ret = suit_plat_authenticate(
			suit_cose_es256,
			(block->_COSE_Sign1__Headers._Headers_protected_cbor._header_map_key_id_present ?
				&block->_COSE_Sign1__Headers._Headers_protected_cbor._header_map_key_id._header_map_key_id:
				(struct zcbor_string *)NULL
			),
			&block->_COSE_Sign1_signature,
			&auth->_SUIT_Authentication_SUIT_Digest_bstr);

		if (ret != SUIT_SUCCESS) {
			results[i * 2] = suit_bool_false;
			// SUS: Information leak, about the authentication block # that failed.
			//return ret;
		}

		if (ret == SUIT_SUCCESS) {
			results[i * 2] = suit_bool_true;
		}
	}

	for (int i = 0; i < auth->_SUIT_Authentication_Block_bstr_count; i++) {
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

	if (num_ok && (num_ok == auth->_SUIT_Authentication_Block_bstr_count)) {
		int ret = suit_plat_check_digest(suit_cose_sha256,
			&auth->_SUIT_Authentication_SUIT_Digest_bstr_cbor._SUIT_Digest_suit_digest_bytes,
			&state->envelope._SUIT_Envelope_suit_manifest);

		if (ret != SUIT_SUCCESS) {
			state->envelope_validated = suit_bool_false;
			return ret;
		}

		if (ret == SUIT_SUCCESS) {
			// SUS: The index (i * 2) is outside the array with results.
			//results[i * 2] = suit_bool_true;
			state->envelope_validated = suit_bool_true;
			return SUIT_SUCCESS;
		}
		goto tamp;
	}

tamp:
	state->envelope_validated = suit_bool_false;
	return SUIT_ERR_TAMP;
}


int suit_decode_manifest(struct suit_processor_state *state)
{
	if (state->envelope_decoded != suit_bool_true
		|| state->envelope_validated != suit_bool_true) {
		return SUIT_ERR_ORDER;
	}

	state->manifest_decoded = suit_bool_false;
	state->manifest_validated = suit_bool_false;

	if (state->manifest_decoded != suit_bool_false
		|| state->manifest_validated != suit_bool_false) {
		return SUIT_ERR_TAMP;
	}

	size_t decoded_len;
	int ret = cbor_decode_SUIT_Manifest(
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

	if (state->manifest_decoded != suit_bool_true) {
		return SUIT_ERR_TAMP;
	}

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


static void get_component_id_str(struct zcbor_string *out_component_id,
	const struct zcbor_string *first_bstr, uint_fast32_t num_bstrs)
{
	size_t extra_header_len = header_len(first_bstr->len) + header_len(num_bstrs);
	out_component_id->value = first_bstr->value - extra_header_len;
	out_component_id->len = first_bstr->len + extra_header_len;
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
		case SUIT_RUN:
			if (unseverable->_SUIT_Unseverable_Members_suit_run_present) {
				return &unseverable->_SUIT_Unseverable_Members_suit_run._SUIT_Unseverable_Members_suit_run;
			}
			return NULL;
		default:
			return NULL;
	}
	return NULL;
}


int suit_validate_manifest(struct suit_processor_state *state)
{
	if (state->envelope_decoded != suit_bool_true
		|| state->envelope_validated != suit_bool_true
		|| state->envelope_decoded != suit_bool_true) {
		return SUIT_ERR_ORDER;
	}

	state->manifest_validated = suit_bool_false;

	if (state->manifest_validated != suit_bool_false) {
		return SUIT_ERR_TAMP;
	}

	do {
		int ret = suit_plat_check_sequence_num(state->manifest._SUIT_Manifest_suit_manifest_sequence_number);

		CHECK_RET(ret, break);
	} while (0);

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

	struct SUIT_Common *common = &state->manifest._SUIT_Manifest_suit_common_cbor;

	while (common->_SUIT_Common_suit_common_sequence_present){
		int ret = suit_validate_common_sequence(&common->_SUIT_Common_suit_common_sequence._SUIT_Common_suit_common_sequence);

		CHECK_RET(ret, break);
	}

	if (common->_SUIT_Common_suit_components_present){
		struct zcbor_string component_id;
		for (int i = 0; i < common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier_count; i++) {
			get_component_id_str(&component_id,
				&common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier[i]._SUIT_Component_Identifier_bstr[0],
				common->_SUIT_Common_suit_components._SUIT_Common_suit_components._SUIT_Components__SUIT_Component_Identifier[i]._SUIT_Component_Identifier_bstr_count);
			int ret = suit_plat_get_component_handle(&component_id, &state->key_ids, state->num_key_ids, &state->components[i].component_handle);

			CHECK_RET(ret, continue);
		}
	}

	if (common != &state->manifest._SUIT_Manifest_suit_common_cbor) {
		goto tamp;
	}

	do {
		int ret = suit_plat_check_sequence_num(state->manifest._SUIT_Manifest_suit_manifest_sequence_number);

		CHECK_RET(ret, break);
	} while (0);

	for (enum suit_manifest_step i = SUIT_NO_STEP + 1; i < SUIT_LAST_STEP; i++) {
		struct zcbor_string *step_seq = get_command_seq(state, i);

		if (step_seq != NULL) {
			int ret = suit_validate_command_sequence(step_seq);

			CHECK_RET(ret, break);
		}
	}

	state->dry_run = suit_bool_true;

	unsigned int num_dry_run_steps = 0;

	for (enum suit_manifest_step step = SUIT_NO_STEP + 1; step < SUIT_LAST_STEP; step++) {
		struct zcbor_string *step_seq = get_command_seq(state, step);
		int ret = SUIT_ERR_TAMP;

		if (step_seq == NULL) {
			/* The lack of a command sequence (i.e. fetch step) does not invalidate the whole manifest */
			ret = SUIT_SUCCESS;
		}

		if (step_seq != NULL) {
			ret = suit_run_command_sequence(state, step_seq);
		}

		if (ret != SUIT_SUCCESS) {
			return ret;
		} else if (ret == SUIT_SUCCESS) {
			num_dry_run_steps++;
		}
	}

	if (num_dry_run_steps != SUIT_NUM_STEPS) {
		goto tamp;
	} else if (num_dry_run_steps == SUIT_NUM_STEPS) {
		state->manifest_validated = suit_bool_true;
		state->dry_run = suit_bool_false;
		return SUIT_SUCCESS;
	}

tamp:
	state->manifest_validated = suit_bool_false;
	return SUIT_ERR_TAMP;
}


int suit_process_manifest(struct suit_processor_state *state,
	enum suit_manifest_step step)
{
	if (state->envelope_decoded != suit_bool_true
		|| state->envelope_validated != suit_bool_true
		|| state->envelope_decoded != suit_bool_true
		|| state->envelope_validated != suit_bool_true) {
		return SUIT_ERR_ORDER;
	}

	struct zcbor_string *step_seq = get_command_seq(state, step);

	if (step_seq == NULL) {
		return SUIT_ERR_UNAVAILABLE_COMMAND_SEQ;
	}

	if (step_seq != NULL) {
		return suit_run_command_sequence(state, step_seq);
	}

tamp:
	return SUIT_ERR_TAMP;
}
