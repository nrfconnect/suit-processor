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
#include "manifest_decode.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif

static bool decode_SUIT_Component_Identifier(zcbor_state_t *state, struct SUIT_Component_Identifier *result);
static bool decode_SUIT_Components(zcbor_state_t *state, struct SUIT_Components *result);
static bool decode_repeated_SUIT_Common_suit_components(zcbor_state_t *state, struct SUIT_Common_suit_components *result);
static bool decode_repeated_SUIT_Common_suit_common_sequence(zcbor_state_t *state, struct SUIT_Common_suit_common_sequence *result);
static bool decode_SUIT_Common(zcbor_state_t *state, struct SUIT_Common *result);
static bool decode_repeated_SUIT_Manifest_suit_reference_uri(zcbor_state_t *state, struct SUIT_Manifest_suit_reference_uri *result);
static bool decode_repeated_SUIT_Unseverable_Members_suit_validate(zcbor_state_t *state, struct SUIT_Unseverable_Members_suit_validate *result);
static bool decode_repeated_SUIT_Unseverable_Members_suit_load(zcbor_state_t *state, struct SUIT_Unseverable_Members_suit_load *result);
static bool decode_repeated_SUIT_Unseverable_Members_suit_run(zcbor_state_t *state, struct SUIT_Unseverable_Members_suit_run *result);
static bool decode_SUIT_Unseverable_Members(zcbor_state_t *state, struct SUIT_Unseverable_Members_ *result);
static bool decode_repeated_SUIT_Severable_Members_Choice_suit_payload_fetch(zcbor_state_t *state, struct SUIT_Severable_Members_Choice_suit_payload_fetch *result);
static bool decode_repeated_SUIT_Severable_Members_Choice_suit_install(zcbor_state_t *state, struct SUIT_Severable_Members_Choice_suit_install *result);
static bool decode_SUIT_Digest(zcbor_state_t *state, struct SUIT_Digest *result);
static bool decode_repeated_SUIT_Severable_Members_Choice_suit_text(zcbor_state_t *state, struct SUIT_Severable_Members_Choice_suit_text *result);
static bool decode_SUIT_Severable_Members_Choice(zcbor_state_t *state, struct SUIT_Severable_Members_Choice_ *result);
static bool decode_IndexArg(zcbor_state_t *state, struct IndexArg_ *result);
static bool decode_SUIT_Directive_Try_Each_Argument(zcbor_state_t *state, struct SUIT_Directive_Try_Each_Argument *result);
static bool decode_SUIT_Parameters(zcbor_state_t *state, struct SUIT_Parameters_ *result);
static bool decode_repeated___suit_directive_override_parameters_map__SUIT_Parameters(zcbor_state_t *state, struct __suit_directive_override_parameters_map__SUIT_Parameters *result);
static bool decode_SUIT_Directive_Try_Each_Argument_Common(zcbor_state_t *state, struct SUIT_Directive_Try_Each_Argument_Common *result);
static bool decode_repeated_SUIT_Common_Sequence_union(zcbor_state_t *state, struct SUIT_Common_Sequence_union_ *result);
static bool decode_repeated_SUIT_Command_Sequence_union(zcbor_state_t *state, struct SUIT_Command_Sequence_union_ *result);
static bool decode_repeated_header_map_key_id(zcbor_state_t *state, struct header_map_key_id *result);
static bool decode_header_map(zcbor_state_t *state, struct header_map *result);
static bool decode_Headers(zcbor_state_t *state, struct Headers *result);
static bool decode_COSE_Sign1(zcbor_state_t *state, struct COSE_Sign1 *result);
static bool decode_COSE_Sign1_Tagged(zcbor_state_t *state, struct COSE_Sign1 *result);
static bool decode_repeated_SUIT_Authentication_Block_bstr(zcbor_state_t *state, struct SUIT_Authentication_Block_bstr *result);
static bool decode_SUIT_Authentication(zcbor_state_t *state, struct SUIT_Authentication *result);
static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_vendor_name(zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_vendor_name *result);
static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_model_name(zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_model_name *result);
static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_vendor_domain(zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_vendor_domain *result);
static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_model_info(zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_model_info *result);
static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_component_description(zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_component_description *result);
static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_component_version(zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_component_version *result);
static bool decode_SUIT_Text_Component_Keys(zcbor_state_t *state, struct SUIT_Text_Component_Keys_ *result);
static bool decode_repeated_SUIT_Text_Map_SUIT_Component_Identifier(zcbor_state_t *state, struct SUIT_Text_Map_SUIT_Component_Identifier_ *result);
static bool decode_SUIT_Text_Map(zcbor_state_t *state, struct SUIT_Text_Map *result);
static bool decode_repeated_SUIT_Severable_Manifest_Members_suit_text(zcbor_state_t *state, struct SUIT_Severable_Manifest_Members_suit_text *result);
static bool decode_SUIT_Severable_Manifest_Members_suit_text(zcbor_state_t *state, struct SUIT_Severable_Manifest_Members_suit_text_ *result);
static bool decode_SUIT_Integrated_Payload_suit_integrated_payload_key(zcbor_state_t *state, struct SUIT_Integrated_Payload_suit_integrated_payload_key *result);
static bool decode_repeated_SUIT_Envelope__SUIT_Integrated_Payload(zcbor_state_t *state, struct SUIT_Envelope__SUIT_Integrated_Payload *result);
static bool decode_SUIT_Envelope(zcbor_state_t *state, struct SUIT_Envelope *result);
static bool decode_SUIT_Common_Commands(zcbor_state_t *state, struct SUIT_Common_Commands_ *result);
static bool decode_SUIT_Directive(zcbor_state_t *state, struct SUIT_Directive_ *result);
static bool decode_SUIT_Condition(zcbor_state_t *state, struct SUIT_Condition_ *result);
static bool decode_SUIT_Command_Sequence(zcbor_state_t *state, struct SUIT_Command_Sequence *result);
static bool decode_SUIT_Common_Sequence(zcbor_state_t *state, struct SUIT_Common_Sequence *result);
static bool decode_SUIT_Manifest(zcbor_state_t *state, struct SUIT_Manifest *result);
static bool decode_SUIT_Envelope_Tagged(zcbor_state_t *state, struct SUIT_Envelope *result);


static bool decode_SUIT_Component_Identifier(
		zcbor_state_t *state, struct SUIT_Component_Identifier *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_list_start_decode(state) && ((zcbor_multi_decode(1, 3, &(*result)._SUIT_Component_Identifier_bstr_count, (zcbor_decoder_t *)zcbor_bstr_decode, state, (&(*result)._SUIT_Component_Identifier_bstr), sizeof(struct zcbor_string))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Components(
		zcbor_state_t *state, struct SUIT_Components *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_list_start_decode(state) && ((zcbor_multi_decode(1, 4, &(*result)._SUIT_Components__SUIT_Component_Identifier_count, (zcbor_decoder_t *)decode_SUIT_Component_Identifier, state, (&(*result)._SUIT_Components__SUIT_Component_Identifier), sizeof(struct SUIT_Component_Identifier))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Common_suit_components(
		zcbor_state_t *state, struct SUIT_Common_suit_components *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (2))))
	&& (decode_SUIT_Components(state, (&(*result)._SUIT_Common_suit_components)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Common_suit_common_sequence(
		zcbor_state_t *state, struct SUIT_Common_suit_common_sequence *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (4))))
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Common_suit_common_sequence)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Common(
		zcbor_state_t *state, struct SUIT_Common *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_decode(state) && ((zcbor_present_decode(&((*result)._SUIT_Common_suit_components_present), (zcbor_decoder_t *)decode_repeated_SUIT_Common_suit_components, state, (&(*result)._SUIT_Common_suit_components))
	&& zcbor_present_decode(&((*result)._SUIT_Common_suit_common_sequence_present), (zcbor_decoder_t *)decode_repeated_SUIT_Common_suit_common_sequence, state, (&(*result)._SUIT_Common_suit_common_sequence))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Manifest_suit_reference_uri(
		zcbor_state_t *state, struct SUIT_Manifest_suit_reference_uri *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (4))))
	&& (zcbor_tstr_decode(state, (&(*result)._SUIT_Manifest_suit_reference_uri)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Unseverable_Members_suit_validate(
		zcbor_state_t *state, struct SUIT_Unseverable_Members_suit_validate *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (7))))
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Unseverable_Members_suit_validate)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Unseverable_Members_suit_load(
		zcbor_state_t *state, struct SUIT_Unseverable_Members_suit_load *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (8))))
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Unseverable_Members_suit_load)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Unseverable_Members_suit_run(
		zcbor_state_t *state, struct SUIT_Unseverable_Members_suit_run *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (9))))
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Unseverable_Members_suit_run)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Unseverable_Members(
		zcbor_state_t *state, struct SUIT_Unseverable_Members_ *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_present_decode(&((*result)._SUIT_Unseverable_Members_suit_validate_present), (zcbor_decoder_t *)decode_repeated_SUIT_Unseverable_Members_suit_validate, state, (&(*result)._SUIT_Unseverable_Members_suit_validate))
	&& zcbor_present_decode(&((*result)._SUIT_Unseverable_Members_suit_load_present), (zcbor_decoder_t *)decode_repeated_SUIT_Unseverable_Members_suit_load, state, (&(*result)._SUIT_Unseverable_Members_suit_load))
	&& zcbor_present_decode(&((*result)._SUIT_Unseverable_Members_suit_run_present), (zcbor_decoder_t *)decode_repeated_SUIT_Unseverable_Members_suit_run, state, (&(*result)._SUIT_Unseverable_Members_suit_run)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Severable_Members_Choice_suit_payload_fetch(
		zcbor_state_t *state, struct SUIT_Severable_Members_Choice_suit_payload_fetch *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (16))))
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Severable_Members_Choice_suit_payload_fetch)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Severable_Members_Choice_suit_install(
		zcbor_state_t *state, struct SUIT_Severable_Members_Choice_suit_install *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (17))))
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Severable_Members_Choice_suit_install)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Digest(
		zcbor_state_t *state, struct SUIT_Digest *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_list_start_decode(state) && ((((zcbor_int32_expect(state, (-16))))
	&& ((zcbor_bstr_decode(state, (&(*result)._SUIT_Digest_suit_digest_bytes))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Severable_Members_Choice_suit_text(
		zcbor_state_t *state, struct SUIT_Severable_Members_Choice_suit_text *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (23))))
	&& (decode_SUIT_Digest(state, (&(*result)._SUIT_Severable_Members_Choice_suit_text)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Severable_Members_Choice(
		zcbor_state_t *state, struct SUIT_Severable_Members_Choice_ *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_present_decode(&((*result)._SUIT_Severable_Members_Choice_suit_payload_fetch_present), (zcbor_decoder_t *)decode_repeated_SUIT_Severable_Members_Choice_suit_payload_fetch, state, (&(*result)._SUIT_Severable_Members_Choice_suit_payload_fetch))
	&& zcbor_present_decode(&((*result)._SUIT_Severable_Members_Choice_suit_install_present), (zcbor_decoder_t *)decode_repeated_SUIT_Severable_Members_Choice_suit_install, state, (&(*result)._SUIT_Severable_Members_Choice_suit_install))
	&& zcbor_present_decode(&((*result)._SUIT_Severable_Members_Choice_suit_text_present), (zcbor_decoder_t *)decode_repeated_SUIT_Severable_Members_Choice_suit_text, state, (&(*result)._SUIT_Severable_Members_Choice_suit_text)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_IndexArg(
		zcbor_state_t *state, struct IndexArg_ *result)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_union_start_code(state) && (int_res = ((((zcbor_uint32_decode(state, (&(*result)._IndexArg_uint)))) && (((*result)._IndexArg_choice = _IndexArg_uint) || 1))
	|| (((zcbor_bool_decode(state, (&(*result)._IndexArg_bool)))) && (((*result)._IndexArg_choice = _IndexArg_bool) || 1))
	|| (zcbor_union_elem_code(state) && (((zcbor_list_start_decode(state) && ((zcbor_multi_decode(1, 3, &(*result)._IndexArg__uint_uint_count, (zcbor_decoder_t *)zcbor_uint32_decode, state, (&(*result)._IndexArg__uint_uint), sizeof(uint32_t))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))) && (((*result)._IndexArg_choice = _IndexArg__uint) || 1)))), zcbor_union_end_code(state), int_res))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Directive_Try_Each_Argument(
		zcbor_state_t *state, struct SUIT_Directive_Try_Each_Argument *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_list_start_decode(state) && ((zcbor_multi_decode(2, 3, &(*result)._SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr_count, (zcbor_decoder_t *)zcbor_bstr_decode, state, (&(*result)._SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr), sizeof(struct zcbor_string))
	&& zcbor_present_decode(&((*result)._SUIT_Directive_Try_Each_Argument_nil_present), (zcbor_decoder_t *)zcbor_nil_expect, state, NULL)) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Parameters(
		zcbor_state_t *state, struct SUIT_Parameters_ *result)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((((zcbor_int_decode(state, &(*result)._SUIT_Parameters_choice, sizeof((*result)._SUIT_Parameters_choice)))) && ((((((*result)._SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_vendor_identifier) && (((1)
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Parameters_suit_parameter_vendor_identifier)))
	&& ((((((*result)._SUIT_Parameters_suit_parameter_vendor_identifier.len >= 16)
	&& ((*result)._SUIT_Parameters_suit_parameter_vendor_identifier.len <= 16)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))
	|| (((*result)._SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_class_identifier) && (((1)
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Parameters_suit_parameter_class_identifier)))
	&& ((((((*result)._SUIT_Parameters_suit_parameter_class_identifier.len >= 16)
	&& ((*result)._SUIT_Parameters_suit_parameter_class_identifier.len <= 16)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))
	|| (((*result)._SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_image_digest) && (((1)
	&& (zcbor_bstr_start_decode(state, &(*result)._SUIT_Parameters_suit_parameter_image_digest)
	&& (int_res = (((decode_SUIT_Digest(state, (&(*result)._SUIT_Parameters_suit_parameter_image_digest_cbor))))), zcbor_bstr_end_decode(state), int_res)))))
	|| (((*result)._SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_image_size) && (((1)
	&& (zcbor_uint32_decode(state, (&(*result)._SUIT_Parameters_suit_parameter_image_size))))))
	|| (((*result)._SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_component_slot) && (((1)
	&& (zcbor_uint32_decode(state, (&(*result)._SUIT_Parameters_suit_parameter_component_slot))))))
	|| (((*result)._SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_uri) && (((1)
	&& (zcbor_tstr_decode(state, (&(*result)._SUIT_Parameters_suit_parameter_uri))))))
	|| (((*result)._SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_source_component) && (((1)
	&& (zcbor_uint32_decode(state, (&(*result)._SUIT_Parameters_suit_parameter_source_component))))))
	|| (((*result)._SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_run_args) && (((1)
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Parameters_suit_parameter_run_args))))))
	|| (((*result)._SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_device_identifier) && (((1)
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Parameters_suit_parameter_device_identifier)))
	&& ((((((*result)._SUIT_Parameters_suit_parameter_device_identifier.len >= 16)
	&& ((*result)._SUIT_Parameters_suit_parameter_device_identifier.len <= 16)) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))
	|| (((*result)._SUIT_Parameters_choice == _SUIT_Parameters_suit_parameter_soft_failure) && (((1)
	&& (zcbor_bool_decode(state, (&(*result)._SUIT_Parameters_suit_parameter_soft_failure))))))) || (zcbor_error(state, ZCBOR_ERR_WRONG_VALUE), false))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated___suit_directive_override_parameters_map__SUIT_Parameters(
		zcbor_state_t *state, struct __suit_directive_override_parameters_map__SUIT_Parameters *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((decode_SUIT_Parameters(state, (&(*result).___suit_directive_override_parameters_map__SUIT_Parameters)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Directive_Try_Each_Argument_Common(
		zcbor_state_t *state, struct SUIT_Directive_Try_Each_Argument_Common *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_list_start_decode(state) && ((zcbor_multi_decode(2, 3, &(*result)._SUIT_Directive_Try_Each_Argument_Common_SUIT_Common_Sequence_bstr_count, (zcbor_decoder_t *)zcbor_bstr_decode, state, (&(*result)._SUIT_Directive_Try_Each_Argument_Common_SUIT_Common_Sequence_bstr), sizeof(struct zcbor_string))
	&& zcbor_present_decode(&((*result)._SUIT_Directive_Try_Each_Argument_Common_nil_present), (zcbor_decoder_t *)zcbor_nil_expect, state, NULL)) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Common_Sequence_union(
		zcbor_state_t *state, struct SUIT_Common_Sequence_union_ *result)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_union_start_code(state) && (int_res = ((((decode_SUIT_Condition(state, (&(*result)._SUIT_Common_Sequence_union__SUIT_Condition)))) && (((*result)._SUIT_Common_Sequence_union_choice = _SUIT_Common_Sequence_union__SUIT_Condition) || 1))
	|| (zcbor_union_elem_code(state) && (((decode_SUIT_Common_Commands(state, (&(*result)._SUIT_Common_Sequence_union__SUIT_Common_Commands)))) && (((*result)._SUIT_Common_Sequence_union_choice = _SUIT_Common_Sequence_union__SUIT_Common_Commands) || 1)))), zcbor_union_end_code(state), int_res))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Command_Sequence_union(
		zcbor_state_t *state, struct SUIT_Command_Sequence_union_ *result)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_union_start_code(state) && (int_res = ((((decode_SUIT_Condition(state, (&(*result)._SUIT_Command_Sequence_union__SUIT_Condition)))) && (((*result)._SUIT_Command_Sequence_union_choice = _SUIT_Command_Sequence_union__SUIT_Condition) || 1))
	|| (zcbor_union_elem_code(state) && (((decode_SUIT_Directive(state, (&(*result)._SUIT_Command_Sequence_union__SUIT_Directive)))) && (((*result)._SUIT_Command_Sequence_union_choice = _SUIT_Command_Sequence_union__SUIT_Directive) || 1)))), zcbor_union_end_code(state), int_res))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

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

static bool decode_repeated_SUIT_Authentication_Block_bstr(
		zcbor_state_t *state, struct SUIT_Authentication_Block_bstr *result)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_bstr_start_decode(state, &(*result)._SUIT_Authentication_Block_bstr)
	&& (int_res = (((decode_COSE_Sign1_Tagged(state, (&(*result)._SUIT_Authentication_Block_bstr_cbor))))), zcbor_bstr_end_decode(state), int_res))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Authentication(
		zcbor_state_t *state, struct SUIT_Authentication *result)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_list_start_decode(state) && ((((zcbor_bstr_start_decode(state, &(*result)._SUIT_Authentication_SUIT_Digest_bstr)
	&& (int_res = (((decode_SUIT_Digest(state, (&(*result)._SUIT_Authentication_SUIT_Digest_bstr_cbor))))), zcbor_bstr_end_decode(state), int_res)))
	&& zcbor_multi_decode(1, 2, &(*result)._SUIT_Authentication_Block_bstr_count, (zcbor_decoder_t *)decode_repeated_SUIT_Authentication_Block_bstr, state, (&(*result)._SUIT_Authentication_Block_bstr), sizeof(struct SUIT_Authentication_Block_bstr))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_vendor_name(
		zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_vendor_name *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (1))))
	&& (zcbor_tstr_decode(state, (&(*result)._SUIT_Text_Component_Keys_suit_text_vendor_name)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_model_name(
		zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_model_name *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (2))))
	&& (zcbor_tstr_decode(state, (&(*result)._SUIT_Text_Component_Keys_suit_text_model_name)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_vendor_domain(
		zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_vendor_domain *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (3))))
	&& (zcbor_tstr_decode(state, (&(*result)._SUIT_Text_Component_Keys_suit_text_vendor_domain)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_model_info(
		zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_model_info *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (4))))
	&& (zcbor_tstr_decode(state, (&(*result)._SUIT_Text_Component_Keys_suit_text_model_info)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_component_description(
		zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_component_description *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (5))))
	&& (zcbor_tstr_decode(state, (&(*result)._SUIT_Text_Component_Keys_suit_text_component_description)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Text_Component_Keys_suit_text_component_version(
		zcbor_state_t *state, struct SUIT_Text_Component_Keys_suit_text_component_version *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_uint32_expect(state, (6))))
	&& (zcbor_tstr_decode(state, (&(*result)._SUIT_Text_Component_Keys_suit_text_component_version)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Text_Component_Keys(
		zcbor_state_t *state, struct SUIT_Text_Component_Keys_ *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_present_decode(&((*result)._SUIT_Text_Component_Keys_suit_text_vendor_name_present), (zcbor_decoder_t *)decode_repeated_SUIT_Text_Component_Keys_suit_text_vendor_name, state, (&(*result)._SUIT_Text_Component_Keys_suit_text_vendor_name))
	&& zcbor_present_decode(&((*result)._SUIT_Text_Component_Keys_suit_text_model_name_present), (zcbor_decoder_t *)decode_repeated_SUIT_Text_Component_Keys_suit_text_model_name, state, (&(*result)._SUIT_Text_Component_Keys_suit_text_model_name))
	&& zcbor_present_decode(&((*result)._SUIT_Text_Component_Keys_suit_text_vendor_domain_present), (zcbor_decoder_t *)decode_repeated_SUIT_Text_Component_Keys_suit_text_vendor_domain, state, (&(*result)._SUIT_Text_Component_Keys_suit_text_vendor_domain))
	&& zcbor_present_decode(&((*result)._SUIT_Text_Component_Keys_suit_text_model_info_present), (zcbor_decoder_t *)decode_repeated_SUIT_Text_Component_Keys_suit_text_model_info, state, (&(*result)._SUIT_Text_Component_Keys_suit_text_model_info))
	&& zcbor_present_decode(&((*result)._SUIT_Text_Component_Keys_suit_text_component_description_present), (zcbor_decoder_t *)decode_repeated_SUIT_Text_Component_Keys_suit_text_component_description, state, (&(*result)._SUIT_Text_Component_Keys_suit_text_component_description))
	&& zcbor_present_decode(&((*result)._SUIT_Text_Component_Keys_suit_text_component_version_present), (zcbor_decoder_t *)decode_repeated_SUIT_Text_Component_Keys_suit_text_component_version, state, (&(*result)._SUIT_Text_Component_Keys_suit_text_component_version)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Text_Map_SUIT_Component_Identifier(
		zcbor_state_t *state, struct SUIT_Text_Map_SUIT_Component_Identifier_ *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((decode_SUIT_Component_Identifier(state, (&(*result)._SUIT_Text_Map_SUIT_Component_Identifier_key))))
	&& (zcbor_map_start_decode(state) && ((((decode_SUIT_Text_Component_Keys(state, (&(*result)._SUIT_Text_Map_SUIT_Component_Identifier__SUIT_Text_Component_Keys))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Text_Map(
		zcbor_state_t *state, struct SUIT_Text_Map *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_map_start_decode(state) && ((zcbor_multi_decode(0, 3, &(*result)._SUIT_Text_Map_SUIT_Component_Identifier_count, (zcbor_decoder_t *)decode_repeated_SUIT_Text_Map_SUIT_Component_Identifier, state, (&(*result)._SUIT_Text_Map_SUIT_Component_Identifier), sizeof(struct SUIT_Text_Map_SUIT_Component_Identifier_))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Severable_Manifest_Members_suit_text(
		zcbor_state_t *state, struct SUIT_Severable_Manifest_Members_suit_text *result)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = ((((zcbor_uint32_expect(state, (23))))
	&& (zcbor_bstr_start_decode(state, &(*result)._SUIT_Severable_Manifest_Members_suit_text)
	&& (int_res = (((decode_SUIT_Text_Map(state, (&(*result)._SUIT_Severable_Manifest_Members_suit_text_cbor))))), zcbor_bstr_end_decode(state), int_res))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Severable_Manifest_Members_suit_text(
		zcbor_state_t *state, struct SUIT_Severable_Manifest_Members_suit_text_ *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (zcbor_present_decode(&((*result)._SUIT_Severable_Manifest_Members_suit_text_present), (zcbor_decoder_t *)decode_repeated_SUIT_Severable_Manifest_Members_suit_text, state, (&(*result)._SUIT_Severable_Manifest_Members_suit_text)));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Integrated_Payload_suit_integrated_payload_key(
		zcbor_state_t *state, struct SUIT_Integrated_Payload_suit_integrated_payload_key *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((((zcbor_tstr_decode(state, (&(*result)._SUIT_Integrated_Payload_suit_integrated_payload_key_key))))
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Integrated_Payload_suit_integrated_payload_key)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_repeated_SUIT_Envelope__SUIT_Integrated_Payload(
		zcbor_state_t *state, struct SUIT_Envelope__SUIT_Integrated_Payload *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((decode_SUIT_Integrated_Payload_suit_integrated_payload_key(state, (&(*result)._SUIT_Envelope__SUIT_Integrated_Payload)))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Envelope(
		zcbor_state_t *state, struct SUIT_Envelope *result)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (2))))
	&& (zcbor_bstr_start_decode(state, &(*result)._SUIT_Envelope_suit_authentication_wrapper)
	&& (int_res = (((decode_SUIT_Authentication(state, (&(*result)._SUIT_Envelope_suit_authentication_wrapper_cbor))))), zcbor_bstr_end_decode(state), int_res)))
	&& (((zcbor_uint32_expect(state, (3))))
	&& (zcbor_bstr_decode(state, (&(*result)._SUIT_Envelope_suit_manifest))))
	&& ((decode_SUIT_Severable_Manifest_Members_suit_text(state, (&(*result)._SUIT_Envelope__SUIT_Severable_Manifest_Members))))
	&& zcbor_multi_decode(1, 5, &(*result)._SUIT_Envelope__SUIT_Integrated_Payload_count, (zcbor_decoder_t *)decode_repeated_SUIT_Envelope__SUIT_Integrated_Payload, state, (&(*result)._SUIT_Envelope__SUIT_Integrated_Payload), sizeof(struct SUIT_Envelope__SUIT_Integrated_Payload))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Common_Commands(
		zcbor_state_t *state, struct SUIT_Common_Commands_ *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((((zcbor_int_decode(state, &(*result)._SUIT_Common_Commands_choice, sizeof((*result)._SUIT_Common_Commands_choice)))) && ((((((*result)._SUIT_Common_Commands_choice == _SUIT_Common_Commands___suit_directive_set_component_index) && ((((1)
	&& ((decode_IndexArg(state, (&(*result)._SUIT_Common_Commands___suit_directive_set_component_index__IndexArg))))))))
	|| (((*result)._SUIT_Common_Commands_choice == _SUIT_Common_Commands___suit_directive_try_each) && ((((1)
	&& ((decode_SUIT_Directive_Try_Each_Argument_Common(state, (&(*result)._SUIT_Common_Commands___suit_directive_try_each__SUIT_Directive_Try_Each_Argument_Common))))))))
	|| (((*result)._SUIT_Common_Commands_choice == _SUIT_Common_Commands___suit_directive_override_parameters) && ((((1)
	&& ((zcbor_map_start_decode(state) && ((zcbor_multi_decode(1, 6, &(*result).___suit_directive_override_parameters_map__SUIT_Parameters_count, (zcbor_decoder_t *)decode_repeated___suit_directive_override_parameters_map__SUIT_Parameters, state, (&(*result).___suit_directive_override_parameters_map__SUIT_Parameters), sizeof(struct __suit_directive_override_parameters_map__SUIT_Parameters))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state)))))))) || (zcbor_error(state, ZCBOR_ERR_WRONG_VALUE), false))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Directive(
		zcbor_state_t *state, struct SUIT_Directive_ *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((((zcbor_int_decode(state, &(*result)._SUIT_Directive_choice, sizeof((*result)._SUIT_Directive_choice)))) && ((((((*result)._SUIT_Directive_choice == _SUIT_Directive___suit_directive_set_component_index) && ((((1)
	&& ((decode_IndexArg(state, (&(*result)._SUIT_Directive___suit_directive_set_component_index__IndexArg))))))))
	|| (((*result)._SUIT_Directive_choice == _SUIT_Directive___suit_directive_try_each) && ((((1)
	&& ((decode_SUIT_Directive_Try_Each_Argument(state, (&(*result)._SUIT_Directive___suit_directive_try_each__SUIT_Directive_Try_Each_Argument))))))))
	|| (((*result)._SUIT_Directive_choice == _SUIT_Directive___suit_directive_override_parameters) && ((((1)
	&& ((zcbor_map_start_decode(state) && ((zcbor_multi_decode(1, 6, &(*result).___suit_directive_override_parameters_map__SUIT_Parameters_count, (zcbor_decoder_t *)decode_repeated___suit_directive_override_parameters_map__SUIT_Parameters, state, (&(*result).___suit_directive_override_parameters_map__SUIT_Parameters), sizeof(struct __suit_directive_override_parameters_map__SUIT_Parameters))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state)))))))
	|| (((*result)._SUIT_Directive_choice == _SUIT_Directive___suit_directive_fetch) && ((((1)
	&& ((zcbor_uint32_decode(state, (&(*result)._SUIT_Directive___suit_directive_fetch__SUIT_Rep_Policy)))
	&& ((((!((*result)._SUIT_Directive___suit_directive_fetch__SUIT_Rep_Policy & ~((1 << _suit_reporting_bits_suit_send_record_success) | (1 << _suit_reporting_bits_suit_send_record_failure) | (1 << _suit_reporting_bits_suit_send_sysinfo_success) | (1 << _suit_reporting_bits_suit_send_sysinfo_failure)))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))))
	|| (((*result)._SUIT_Directive_choice == _SUIT_Directive___suit_directive_copy) && ((((1)
	&& ((zcbor_uint32_decode(state, (&(*result)._SUIT_Directive___suit_directive_copy__SUIT_Rep_Policy)))
	&& ((((!((*result)._SUIT_Directive___suit_directive_copy__SUIT_Rep_Policy & ~((1 << _suit_reporting_bits_suit_send_record_success) | (1 << _suit_reporting_bits_suit_send_record_failure) | (1 << _suit_reporting_bits_suit_send_sysinfo_success) | (1 << _suit_reporting_bits_suit_send_sysinfo_failure)))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))))
	|| (((*result)._SUIT_Directive_choice == _SUIT_Directive___suit_directive_run) && ((((1)
	&& ((zcbor_uint32_decode(state, (&(*result)._SUIT_Directive___suit_directive_run__SUIT_Rep_Policy)))
	&& ((((!((*result)._SUIT_Directive___suit_directive_run__SUIT_Rep_Policy & ~((1 << _suit_reporting_bits_suit_send_record_success) | (1 << _suit_reporting_bits_suit_send_record_failure) | (1 << _suit_reporting_bits_suit_send_sysinfo_success) | (1 << _suit_reporting_bits_suit_send_sysinfo_failure)))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))))) || (zcbor_error(state, ZCBOR_ERR_WRONG_VALUE), false))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Condition(
		zcbor_state_t *state, struct SUIT_Condition_ *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((((zcbor_int_decode(state, &(*result)._SUIT_Condition_choice, sizeof((*result)._SUIT_Condition_choice)))) && ((((((*result)._SUIT_Condition_choice == _SUIT_Condition___suit_condition_vendor_identifier) && ((((1)
	&& ((zcbor_uint32_decode(state, (&(*result)._SUIT_Condition___suit_condition_vendor_identifier__SUIT_Rep_Policy)))
	&& ((((!((*result)._SUIT_Condition___suit_condition_vendor_identifier__SUIT_Rep_Policy & ~((1 << _suit_reporting_bits_suit_send_record_success) | (1 << _suit_reporting_bits_suit_send_record_failure) | (1 << _suit_reporting_bits_suit_send_sysinfo_success) | (1 << _suit_reporting_bits_suit_send_sysinfo_failure)))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))))
	|| (((*result)._SUIT_Condition_choice == _SUIT_Condition___suit_condition_class_identifier) && ((((1)
	&& ((zcbor_uint32_decode(state, (&(*result)._SUIT_Condition___suit_condition_class_identifier__SUIT_Rep_Policy)))
	&& ((((!((*result)._SUIT_Condition___suit_condition_class_identifier__SUIT_Rep_Policy & ~((1 << _suit_reporting_bits_suit_send_record_success) | (1 << _suit_reporting_bits_suit_send_record_failure) | (1 << _suit_reporting_bits_suit_send_sysinfo_success) | (1 << _suit_reporting_bits_suit_send_sysinfo_failure)))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))))
	|| (((*result)._SUIT_Condition_choice == _SUIT_Condition___suit_condition_device_identifier) && ((((1)
	&& ((zcbor_uint32_decode(state, (&(*result)._SUIT_Condition___suit_condition_device_identifier__SUIT_Rep_Policy)))
	&& ((((!((*result)._SUIT_Condition___suit_condition_device_identifier__SUIT_Rep_Policy & ~((1 << _suit_reporting_bits_suit_send_record_success) | (1 << _suit_reporting_bits_suit_send_record_failure) | (1 << _suit_reporting_bits_suit_send_sysinfo_success) | (1 << _suit_reporting_bits_suit_send_sysinfo_failure)))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))))
	|| (((*result)._SUIT_Condition_choice == _SUIT_Condition___suit_condition_image_match) && ((((1)
	&& ((zcbor_uint32_decode(state, (&(*result)._SUIT_Condition___suit_condition_image_match__SUIT_Rep_Policy)))
	&& ((((!((*result)._SUIT_Condition___suit_condition_image_match__SUIT_Rep_Policy & ~((1 << _suit_reporting_bits_suit_send_record_success) | (1 << _suit_reporting_bits_suit_send_record_failure) | (1 << _suit_reporting_bits_suit_send_sysinfo_success) | (1 << _suit_reporting_bits_suit_send_sysinfo_failure)))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))))
	|| (((*result)._SUIT_Condition_choice == _SUIT_Condition___suit_condition_component_slot) && ((((1)
	&& ((zcbor_uint32_decode(state, (&(*result)._SUIT_Condition___suit_condition_component_slot__SUIT_Rep_Policy)))
	&& ((((!((*result)._SUIT_Condition___suit_condition_component_slot__SUIT_Rep_Policy & ~((1 << _suit_reporting_bits_suit_send_record_success) | (1 << _suit_reporting_bits_suit_send_record_failure) | (1 << _suit_reporting_bits_suit_send_sysinfo_success) | (1 << _suit_reporting_bits_suit_send_sysinfo_failure)))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))))
	|| (((*result)._SUIT_Condition_choice == _SUIT_Condition___suit_condition_abort) && ((((1)
	&& ((zcbor_uint32_decode(state, (&(*result)._SUIT_Condition___suit_condition_abort__SUIT_Rep_Policy)))
	&& ((((!((*result)._SUIT_Condition___suit_condition_abort__SUIT_Rep_Policy & ~((1 << _suit_reporting_bits_suit_send_record_success) | (1 << _suit_reporting_bits_suit_send_record_failure) | (1 << _suit_reporting_bits_suit_send_sysinfo_success) | (1 << _suit_reporting_bits_suit_send_sysinfo_failure)))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false))) || (zcbor_error(state, ZCBOR_ERR_WRONG_RANGE), false)))))))) || (zcbor_error(state, ZCBOR_ERR_WRONG_VALUE), false))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Command_Sequence(
		zcbor_state_t *state, struct SUIT_Command_Sequence *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_list_start_decode(state) && ((zcbor_multi_decode(1, 3, &(*result)._SUIT_Command_Sequence_union_count, (zcbor_decoder_t *)decode_repeated_SUIT_Command_Sequence_union, state, (&(*result)._SUIT_Command_Sequence_union), sizeof(struct SUIT_Command_Sequence_union_))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Common_Sequence(
		zcbor_state_t *state, struct SUIT_Common_Sequence *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = (((zcbor_list_start_decode(state) && ((zcbor_multi_decode(1, 3, &(*result)._SUIT_Common_Sequence_union_count, (zcbor_decoder_t *)decode_repeated_SUIT_Common_Sequence_union, state, (&(*result)._SUIT_Common_Sequence_union), sizeof(struct SUIT_Common_Sequence_union_))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Manifest(
		zcbor_state_t *state, struct SUIT_Manifest *result)
{
	zcbor_print("%s\r\n", __func__);
	bool int_res;

	bool tmp_result = (((zcbor_map_start_decode(state) && (((((zcbor_uint32_expect(state, (1))))
	&& (zcbor_uint32_expect(state, (1))))
	&& (((zcbor_uint32_expect(state, (2))))
	&& (zcbor_uint32_decode(state, (&(*result)._SUIT_Manifest_suit_manifest_sequence_number))))
	&& (((zcbor_uint32_expect(state, (3))))
	&& (zcbor_bstr_start_decode(state, &(*result)._SUIT_Manifest_suit_common)
	&& (int_res = (((decode_SUIT_Common(state, (&(*result)._SUIT_Manifest_suit_common_cbor))))), zcbor_bstr_end_decode(state), int_res)))
	&& zcbor_present_decode(&((*result)._SUIT_Manifest_suit_reference_uri_present), (zcbor_decoder_t *)decode_repeated_SUIT_Manifest_suit_reference_uri, state, (&(*result)._SUIT_Manifest_suit_reference_uri))
	&& ((decode_SUIT_Unseverable_Members(state, (&(*result)._SUIT_Manifest__SUIT_Unseverable_Members))))
	&& ((decode_SUIT_Severable_Members_Choice(state, (&(*result)._SUIT_Manifest__SUIT_Severable_Members_Choice))))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_map_end_decode(state))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}

static bool decode_SUIT_Envelope_Tagged(
		zcbor_state_t *state, struct SUIT_Envelope *result)
{
	zcbor_print("%s\r\n", __func__);

	bool tmp_result = ((zcbor_tag_expect(state, 107)
	&& (decode_SUIT_Envelope(state, (&(*result))))));

	if (!tmp_result)
		zcbor_trace();

	return tmp_result;
}



int cbor_decode_SUIT_Envelope_Tagged(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Envelope *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[9];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_SUIT_Envelope_Tagged(states, result);

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


int cbor_decode_SUIT_Manifest(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Manifest *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[7];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_SUIT_Manifest(states, result);

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


int cbor_decode_SUIT_Common_Sequence(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Common_Sequence *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[9];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_SUIT_Common_Sequence(states, result);

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


int cbor_decode_SUIT_Command_Sequence(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Command_Sequence *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[9];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 1);

	bool ret = decode_SUIT_Command_Sequence(states, result);

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


int cbor_decode_SUIT_Condition(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Condition_ *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[3];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 2);

	bool ret = decode_SUIT_Condition(states, result);

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


int cbor_decode_SUIT_Directive(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Directive_ *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[7];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 2);

	bool ret = decode_SUIT_Directive(states, result);

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


int cbor_decode_SUIT_Common_Commands(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Common_Commands_ *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[7];

	zcbor_new_state(states, sizeof(states) / sizeof(zcbor_state_t), payload, payload_len, 2);

	bool ret = decode_SUIT_Common_Commands(states, result);

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
