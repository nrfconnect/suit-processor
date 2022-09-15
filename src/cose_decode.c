/*
 * Generated using zcbor version 0.5.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"
#include "cose_decode.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif

static bool decode_repeated_header_map_key_id(zcbor_state_t *state, struct header_map_key_id *result);
static bool decode_header_map(zcbor_state_t *state, struct header_map *result);
static bool decode_Headers(zcbor_state_t *state, struct Headers *result);
static bool decode_COSE_Sign1(zcbor_state_t *state, struct COSE_Sign1 *result);
static bool decode_Sig_structure1(zcbor_state_t *state, struct Sig_structure1 *result);
static bool decode_COSE_Sign1_Tagged(zcbor_state_t *state, struct COSE_Sign1 *result);


static bool decode_repeated_header_map_key_id(
		zcbor_state_t *state, struct header_map_key_id *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (4))))
	&& (zcbor_bstr_decode(state, (&(*result)._header_map_key_id)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_header_map(
		zcbor_state_t *state, struct header_map *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (1))))
	&& (zcbor_int32_expect(state, (-7))))
	&& zcbor_present_decode(&((*result)._header_map_key_id_present), (zcbor_decoder_t *)decode_repeated_header_map_key_id, state, (&(*result)._header_map_key_id))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_Headers(
		zcbor_state_t *state, struct Headers *result)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((((zcbor_bstr_start_decode(state, &(*result)._Headers_protected)
	&& (int_res = (((decode_header_map(state, (&(*result)._Headers_protected_cbor))))), zcbor_bstr_end_decode(state), int_res)))
	&& ((zcbor_map_start_decode(state) && zcbor_map_end_decode(state))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_COSE_Sign1(
		zcbor_state_t *state, struct COSE_Sign1 *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_list_start_decode(state) && ((((decode_Headers(state, (&(*result)._COSE_Sign1__Headers))))
	&& ((zcbor_nil_expect(state, NULL)))
	&& ((zcbor_bstr_decode(state, (&(*result)._COSE_Sign1_signature))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_Sig_structure1(
		zcbor_state_t *state, struct Sig_structure1 *result)
{
	zcbor_print("%s\r\n", __func__);
	struct zcbor_string tmp_str;
	bool int_res;

	bool tmp_result = (((zcbor_list_start_decode(state) && ((((zcbor_tstr_expect(state, ((tmp_str.value = (uint8_t *)"Signature1", tmp_str.len = sizeof("Signature1") - 1, &tmp_str)))))
	&& ((zcbor_bstr_start_decode(state, &(*result)._Sig_structure1_body_protected)
	&& (int_res = (((decode_header_map(state, (&(*result)._Sig_structure1_body_protected_cbor))))), zcbor_bstr_end_decode(state), int_res)))
	&& ((zcbor_bstr_expect(state, ((tmp_str.value = (uint8_t *)"", tmp_str.len = sizeof("") - 1, &tmp_str)))))
	&& ((zcbor_bstr_decode(state, (&(*result)._Sig_structure1_payload))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_COSE_Sign1_Tagged(
		zcbor_state_t *state, struct COSE_Sign1 *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((zcbor_tag_expect(state, 18)
	&& (decode_COSE_Sign1(state, (&(*result))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



int cbor_decode_COSE_Sign1_Tagged(
		const uint8_t *payload, size_t payload_len,
		struct COSE_Sign1 *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[5];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_COSE_Sign1_Tagged(states, result);

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


int cbor_decode_Sig_structure1(
		const uint8_t *payload, size_t payload_len,
		struct Sig_structure1 *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[5];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_Sig_structure1(states, result);

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
