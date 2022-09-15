/*
 * Generated using zcbor version 0.5.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "cose_encode.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif

static bool encode_repeated_header_map_key_id(zcbor_state_t *state, const struct header_map_key_id *input);
static bool encode_header_map(zcbor_state_t *state, const struct header_map *input);
static bool encode_Headers(zcbor_state_t *state, const struct Headers *input);
static bool encode_COSE_Sign1(zcbor_state_t *state, const struct COSE_Sign1 *input);
static bool encode_Sig_structure1(zcbor_state_t *state, const struct Sig_structure1 *input);
static bool encode_COSE_Sign1_Tagged(zcbor_state_t *state, const struct COSE_Sign1 *input);


static bool encode_repeated_header_map_key_id(
		zcbor_state_t *state, const struct header_map_key_id *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_put(state, (4))))
	&& (zcbor_bstr_encode(state, (&(*input)._header_map_key_id)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_header_map(
		zcbor_state_t *state, const struct header_map *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_encode(state, 2) && (((((zcbor_uint32_put(state, (1))))
	&& (zcbor_int32_put(state, (-7))))
	&& zcbor_present_encode(&((*input)._header_map_key_id_present), (zcbor_encoder_t *)encode_repeated_header_map_key_id, state, (&(*input)._header_map_key_id))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_map_end_encode(state, 2))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_Headers(
		zcbor_state_t *state, const struct Headers *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = ((((((*input)._Headers_protected.value ? (memcpy(&tmp_str, &(*input)._Headers_protected, sizeof(tmp_str)), (zcbor_bstr_encode(state, (&(*input)._Headers_protected)))) : ((zcbor_bstr_start_encode(state)
	&& (int_res = (((encode_header_map(state, (&(*input)._Headers_protected_cbor))))), zcbor_bstr_end_encode(state, &tmp_str), int_res)))))
	&& ((zcbor_map_start_encode(state, 0) && zcbor_map_end_encode(state, 0))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_COSE_Sign1(
		zcbor_state_t *state, const struct COSE_Sign1 *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_list_start_encode(state, 4) && ((((encode_Headers(state, (&(*input)._COSE_Sign1__Headers))))
	&& ((zcbor_nil_put(state, NULL)))
	&& ((zcbor_bstr_encode(state, (&(*input)._COSE_Sign1_signature))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 4))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_Sig_structure1(
		zcbor_state_t *state, const struct Sig_structure1 *input)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_list_start_encode(state, 4) && ((((zcbor_tstr_encode(state, ((tmp_str.value = (uint8_t *)"Signature1", tmp_str.len = sizeof("Signature1") - 1, &tmp_str)))))
	&& (((*input)._Sig_structure1_body_protected.value ? (memcpy(&tmp_str, &(*input)._Sig_structure1_body_protected, sizeof(tmp_str)), (zcbor_bstr_encode(state, (&(*input)._Sig_structure1_body_protected)))) : ((zcbor_bstr_start_encode(state)
	&& (int_res = (((encode_header_map(state, (&(*input)._Sig_structure1_body_protected_cbor))))), zcbor_bstr_end_encode(state, &tmp_str), int_res)))))
	&& ((zcbor_bstr_encode(state, ((tmp_str.value = (uint8_t *)"", tmp_str.len = sizeof("") - 1, &tmp_str)))))
	&& ((zcbor_bstr_encode(state, (&(*input)._Sig_structure1_payload))))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 4))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool encode_COSE_Sign1_Tagged(
		zcbor_state_t *state, const struct COSE_Sign1 *input)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((zcbor_tag_encode(state, 18)
	&& (encode_COSE_Sign1(state, (&(*input))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



int cbor_encode_COSE_Sign1_Tagged(
		uint8_t *payload, size_t payload_len,
		const struct COSE_Sign1 *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[5];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_COSE_Sign1_Tagged(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}


int cbor_encode_Sig_structure1(
		uint8_t *payload, size_t payload_len,
		const struct Sig_structure1 *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[5];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = encode_Sig_structure1(states, input);

	if (ret && (payload_len_out != NULL)) {
		*payload_len_out = MIN(payload_len,
				(size_t)states[0].payload - (size_t)payload);
	}

	if (!ret) {
		int err = zcbor_pop_error(states);

		zcbor_print("Return error: %d\r\n", err);
		return (err == ZCBOR_SUCCESS) ? ZCBOR_ERR_UNKNOWN : err;
	}
	return ZCBOR_SUCCESS;
}
