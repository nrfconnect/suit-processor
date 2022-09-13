/*
 * Generated using zcbor version 0.5.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef MANIFEST_TYPES_H__
#define MANIFEST_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 3

enum suit_reporting_bits {
	_suit_reporting_bits_suit_send_record_success = 0,
	_suit_reporting_bits_suit_send_record_failure = 1,
	_suit_reporting_bits_suit_send_sysinfo_success = 2,
	_suit_reporting_bits_suit_send_sysinfo_failure = 3,
};

struct SUIT_Condition_ {
	union {
		struct {
			uint32_t _SUIT_Condition___suit_condition_vendor_identifier__SUIT_Rep_Policy;
		};
		struct {
			uint32_t _SUIT_Condition___suit_condition_class_identifier__SUIT_Rep_Policy;
		};
		struct {
			uint32_t _SUIT_Condition___suit_condition_device_identifier__SUIT_Rep_Policy;
		};
		struct {
			uint32_t _SUIT_Condition___suit_condition_image_match__SUIT_Rep_Policy;
		};
		struct {
			uint32_t _SUIT_Condition___suit_condition_component_slot__SUIT_Rep_Policy;
		};
		struct {
			uint32_t _SUIT_Condition___suit_condition_abort__SUIT_Rep_Policy;
		};
	};
	enum {
		_SUIT_Condition___suit_condition_vendor_identifier = 1,
		_SUIT_Condition___suit_condition_class_identifier = 2,
		_SUIT_Condition___suit_condition_device_identifier = 24,
		_SUIT_Condition___suit_condition_image_match = 3,
		_SUIT_Condition___suit_condition_component_slot = 5,
		_SUIT_Condition___suit_condition_abort = 14,
	} _SUIT_Condition_choice;
};

struct SUIT_Component_Identifier {
	struct zcbor_string _SUIT_Component_Identifier_bstr[3];
	uint_fast32_t _SUIT_Component_Identifier_bstr_count;
};

struct SUIT_Components {
	struct SUIT_Component_Identifier _SUIT_Components__SUIT_Component_Identifier[4];
	uint_fast32_t _SUIT_Components__SUIT_Component_Identifier_count;
};

struct SUIT_Common_suit_components {
	struct SUIT_Components _SUIT_Common_suit_components;
};

struct SUIT_Common_suit_common_sequence {
	struct zcbor_string _SUIT_Common_suit_common_sequence;
};

struct SUIT_Common {
	struct SUIT_Common_suit_components _SUIT_Common_suit_components;
	uint_fast32_t _SUIT_Common_suit_components_present;
	struct SUIT_Common_suit_common_sequence _SUIT_Common_suit_common_sequence;
	uint_fast32_t _SUIT_Common_suit_common_sequence_present;
};

struct SUIT_Manifest_suit_reference_uri {
	struct zcbor_string _SUIT_Manifest_suit_reference_uri;
};

struct SUIT_Unseverable_Members_suit_validate {
	struct zcbor_string _SUIT_Unseverable_Members_suit_validate;
};

struct SUIT_Unseverable_Members_suit_load {
	struct zcbor_string _SUIT_Unseverable_Members_suit_load;
};

struct SUIT_Unseverable_Members_suit_run {
	struct zcbor_string _SUIT_Unseverable_Members_suit_run;
};

struct SUIT_Unseverable_Members_ {
	struct SUIT_Unseverable_Members_suit_validate _SUIT_Unseverable_Members_suit_validate;
	uint_fast32_t _SUIT_Unseverable_Members_suit_validate_present;
	struct SUIT_Unseverable_Members_suit_load _SUIT_Unseverable_Members_suit_load;
	uint_fast32_t _SUIT_Unseverable_Members_suit_load_present;
	struct SUIT_Unseverable_Members_suit_run _SUIT_Unseverable_Members_suit_run;
	uint_fast32_t _SUIT_Unseverable_Members_suit_run_present;
};

struct SUIT_Severable_Members_Choice_suit_payload_fetch {
	struct zcbor_string _SUIT_Severable_Members_Choice_suit_payload_fetch;
};

struct SUIT_Severable_Members_Choice_suit_install {
	struct zcbor_string _SUIT_Severable_Members_Choice_suit_install;
};

struct SUIT_Digest {
	struct zcbor_string _SUIT_Digest_suit_digest_bytes;
};

struct SUIT_Severable_Members_Choice_suit_text {
	struct SUIT_Digest _SUIT_Severable_Members_Choice_suit_text;
};

struct SUIT_Severable_Members_Choice_ {
	struct SUIT_Severable_Members_Choice_suit_payload_fetch _SUIT_Severable_Members_Choice_suit_payload_fetch;
	uint_fast32_t _SUIT_Severable_Members_Choice_suit_payload_fetch_present;
	struct SUIT_Severable_Members_Choice_suit_install _SUIT_Severable_Members_Choice_suit_install;
	uint_fast32_t _SUIT_Severable_Members_Choice_suit_install_present;
	struct SUIT_Severable_Members_Choice_suit_text _SUIT_Severable_Members_Choice_suit_text;
	uint_fast32_t _SUIT_Severable_Members_Choice_suit_text_present;
};

struct SUIT_Manifest {
	uint32_t _SUIT_Manifest_suit_manifest_sequence_number;
	struct zcbor_string _SUIT_Manifest_suit_common;
	struct SUIT_Common _SUIT_Manifest_suit_common_cbor;
	struct SUIT_Manifest_suit_reference_uri _SUIT_Manifest_suit_reference_uri;
	uint_fast32_t _SUIT_Manifest_suit_reference_uri_present;
	struct SUIT_Unseverable_Members_ _SUIT_Manifest__SUIT_Unseverable_Members;
	struct SUIT_Severable_Members_Choice_ _SUIT_Manifest__SUIT_Severable_Members_Choice;
};

struct IndexArg_ {
	union {
		uint32_t _IndexArg_uint;
		bool _IndexArg_bool;
		struct {
			uint32_t _IndexArg__uint_uint[3];
			uint_fast32_t _IndexArg__uint_uint_count;
		};
	};
	enum {
		_IndexArg_uint,
		_IndexArg_bool,
		_IndexArg__uint,
	} _IndexArg_choice;
};

struct SUIT_Directive_Try_Each_Argument {
	struct zcbor_string _SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr[3];
	uint_fast32_t _SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr_count;
	uint_fast32_t _SUIT_Directive_Try_Each_Argument_nil_present;
};

struct SUIT_Parameters_ {
	union {
		struct {
			struct zcbor_string _SUIT_Parameters_suit_parameter_vendor_identifier;
		};
		struct {
			struct zcbor_string _SUIT_Parameters_suit_parameter_class_identifier;
		};
		struct {
			struct zcbor_string _SUIT_Parameters_suit_parameter_image_digest;
			struct SUIT_Digest _SUIT_Parameters_suit_parameter_image_digest_cbor;
		};
		struct {
			uint32_t _SUIT_Parameters_suit_parameter_image_size;
		};
		struct {
			uint32_t _SUIT_Parameters_suit_parameter_component_slot;
		};
		struct {
			struct zcbor_string _SUIT_Parameters_suit_parameter_uri;
		};
		struct {
			uint32_t _SUIT_Parameters_suit_parameter_source_component;
		};
		struct {
			struct zcbor_string _SUIT_Parameters_suit_parameter_run_args;
		};
		struct {
			struct zcbor_string _SUIT_Parameters_suit_parameter_device_identifier;
		};
		struct {
			bool _SUIT_Parameters_suit_parameter_soft_failure;
		};
	};
	enum {
		_SUIT_Parameters_suit_parameter_vendor_identifier = 1,
		_SUIT_Parameters_suit_parameter_class_identifier = 2,
		_SUIT_Parameters_suit_parameter_image_digest = 3,
		_SUIT_Parameters_suit_parameter_image_size = 14,
		_SUIT_Parameters_suit_parameter_component_slot = 5,
		_SUIT_Parameters_suit_parameter_uri = 21,
		_SUIT_Parameters_suit_parameter_source_component = 22,
		_SUIT_Parameters_suit_parameter_run_args = 23,
		_SUIT_Parameters_suit_parameter_device_identifier = 24,
		_SUIT_Parameters_suit_parameter_soft_failure = 13,
	} _SUIT_Parameters_choice;
};

struct __suit_directive_override_parameters_map__SUIT_Parameters {
	struct SUIT_Parameters_ ___suit_directive_override_parameters_map__SUIT_Parameters;
};

struct SUIT_Directive_ {
	union {
		struct {
			struct IndexArg_ _SUIT_Directive___suit_directive_set_component_index__IndexArg;
		};
		struct {
			struct SUIT_Directive_Try_Each_Argument _SUIT_Directive___suit_directive_try_each__SUIT_Directive_Try_Each_Argument;
		};
		struct {
			struct __suit_directive_override_parameters_map__SUIT_Parameters ___suit_directive_override_parameters_map__SUIT_Parameters[6];
			uint_fast32_t ___suit_directive_override_parameters_map__SUIT_Parameters_count;
		};
		struct {
			uint32_t _SUIT_Directive___suit_directive_fetch__SUIT_Rep_Policy;
		};
		struct {
			uint32_t _SUIT_Directive___suit_directive_copy__SUIT_Rep_Policy;
		};
		struct {
			uint32_t _SUIT_Directive___suit_directive_run__SUIT_Rep_Policy;
		};
	};
	enum {
		_SUIT_Directive___suit_directive_set_component_index = 12,
		_SUIT_Directive___suit_directive_try_each = 15,
		_SUIT_Directive___suit_directive_override_parameters = 20,
		_SUIT_Directive___suit_directive_fetch = 21,
		_SUIT_Directive___suit_directive_copy = 22,
		_SUIT_Directive___suit_directive_run = 23,
	} _SUIT_Directive_choice;
};

struct SUIT_Directive_Try_Each_Argument_Common {
	struct zcbor_string _SUIT_Directive_Try_Each_Argument_Common_SUIT_Common_Sequence_bstr[3];
	uint_fast32_t _SUIT_Directive_Try_Each_Argument_Common_SUIT_Common_Sequence_bstr_count;
	uint_fast32_t _SUIT_Directive_Try_Each_Argument_Common_nil_present;
};

struct SUIT_Common_Commands_ {
	union {
		struct {
			struct IndexArg_ _SUIT_Common_Commands___suit_directive_set_component_index__IndexArg;
		};
		struct {
			struct SUIT_Directive_Try_Each_Argument_Common _SUIT_Common_Commands___suit_directive_try_each__SUIT_Directive_Try_Each_Argument_Common;
		};
		struct {
			struct __suit_directive_override_parameters_map__SUIT_Parameters ___suit_directive_override_parameters_map__SUIT_Parameters[6];
			uint_fast32_t ___suit_directive_override_parameters_map__SUIT_Parameters_count;
		};
	};
	enum {
		_SUIT_Common_Commands___suit_directive_set_component_index = 12,
		_SUIT_Common_Commands___suit_directive_try_each = 15,
		_SUIT_Common_Commands___suit_directive_override_parameters = 20,
	} _SUIT_Common_Commands_choice;
};

struct SUIT_Common_Sequence_union_ {
	union {
		struct SUIT_Condition_ _SUIT_Common_Sequence_union__SUIT_Condition;
		struct SUIT_Common_Commands_ _SUIT_Common_Sequence_union__SUIT_Common_Commands;
	};
	enum {
		_SUIT_Common_Sequence_union__SUIT_Condition,
		_SUIT_Common_Sequence_union__SUIT_Common_Commands,
	} _SUIT_Common_Sequence_union_choice;
};

struct SUIT_Common_Sequence {
	struct SUIT_Common_Sequence_union_ _SUIT_Common_Sequence_union[3];
	uint_fast32_t _SUIT_Common_Sequence_union_count;
};

struct SUIT_Command_Sequence_union_ {
	union {
		struct SUIT_Condition_ _SUIT_Command_Sequence_union__SUIT_Condition;
		struct SUIT_Directive_ _SUIT_Command_Sequence_union__SUIT_Directive;
	};
	enum {
		_SUIT_Command_Sequence_union__SUIT_Condition,
		_SUIT_Command_Sequence_union__SUIT_Directive,
	} _SUIT_Command_Sequence_union_choice;
};

struct SUIT_Command_Sequence {
	struct SUIT_Command_Sequence_union_ _SUIT_Command_Sequence_union[3];
	uint_fast32_t _SUIT_Command_Sequence_union_count;
};

struct header_map_key_id {
	struct zcbor_string _header_map_key_id;
};

struct header_map {
	struct header_map_key_id _header_map_key_id;
	uint_fast32_t _header_map_key_id_present;
};

struct Headers {
	struct zcbor_string _Headers_protected;
	struct header_map _Headers_protected_cbor;
};

struct COSE_Sign1 {
	struct Headers _COSE_Sign1__Headers;
	struct zcbor_string _COSE_Sign1_signature;
};

struct SUIT_Authentication_Block_bstr {
	struct zcbor_string _SUIT_Authentication_Block_bstr;
	struct COSE_Sign1 _SUIT_Authentication_Block_bstr_cbor;
};

struct SUIT_Authentication {
	struct zcbor_string _SUIT_Authentication_SUIT_Digest_bstr;
	struct SUIT_Digest _SUIT_Authentication_SUIT_Digest_bstr_cbor;
	struct SUIT_Authentication_Block_bstr _SUIT_Authentication_Block_bstr[2];
	uint_fast32_t _SUIT_Authentication_Block_bstr_count;
};

struct SUIT_Text_Component_Keys_suit_text_vendor_name {
	struct zcbor_string _SUIT_Text_Component_Keys_suit_text_vendor_name;
};

struct SUIT_Text_Component_Keys_suit_text_model_name {
	struct zcbor_string _SUIT_Text_Component_Keys_suit_text_model_name;
};

struct SUIT_Text_Component_Keys_suit_text_vendor_domain {
	struct zcbor_string _SUIT_Text_Component_Keys_suit_text_vendor_domain;
};

struct SUIT_Text_Component_Keys_suit_text_model_info {
	struct zcbor_string _SUIT_Text_Component_Keys_suit_text_model_info;
};

struct SUIT_Text_Component_Keys_suit_text_component_description {
	struct zcbor_string _SUIT_Text_Component_Keys_suit_text_component_description;
};

struct SUIT_Text_Component_Keys_suit_text_component_version {
	struct zcbor_string _SUIT_Text_Component_Keys_suit_text_component_version;
};

struct SUIT_Text_Component_Keys_ {
	struct SUIT_Text_Component_Keys_suit_text_vendor_name _SUIT_Text_Component_Keys_suit_text_vendor_name;
	uint_fast32_t _SUIT_Text_Component_Keys_suit_text_vendor_name_present;
	struct SUIT_Text_Component_Keys_suit_text_model_name _SUIT_Text_Component_Keys_suit_text_model_name;
	uint_fast32_t _SUIT_Text_Component_Keys_suit_text_model_name_present;
	struct SUIT_Text_Component_Keys_suit_text_vendor_domain _SUIT_Text_Component_Keys_suit_text_vendor_domain;
	uint_fast32_t _SUIT_Text_Component_Keys_suit_text_vendor_domain_present;
	struct SUIT_Text_Component_Keys_suit_text_model_info _SUIT_Text_Component_Keys_suit_text_model_info;
	uint_fast32_t _SUIT_Text_Component_Keys_suit_text_model_info_present;
	struct SUIT_Text_Component_Keys_suit_text_component_description _SUIT_Text_Component_Keys_suit_text_component_description;
	uint_fast32_t _SUIT_Text_Component_Keys_suit_text_component_description_present;
	struct SUIT_Text_Component_Keys_suit_text_component_version _SUIT_Text_Component_Keys_suit_text_component_version;
	uint_fast32_t _SUIT_Text_Component_Keys_suit_text_component_version_present;
};

struct SUIT_Text_Map_SUIT_Component_Identifier_ {
	struct SUIT_Component_Identifier _SUIT_Text_Map_SUIT_Component_Identifier_key;
	struct SUIT_Text_Component_Keys_ _SUIT_Text_Map_SUIT_Component_Identifier__SUIT_Text_Component_Keys;
};

struct SUIT_Text_Map {
	struct SUIT_Text_Map_SUIT_Component_Identifier_ _SUIT_Text_Map_SUIT_Component_Identifier[3];
	uint_fast32_t _SUIT_Text_Map_SUIT_Component_Identifier_count;
};

struct SUIT_Severable_Manifest_Members_suit_text {
	struct zcbor_string _SUIT_Severable_Manifest_Members_suit_text;
	struct SUIT_Text_Map _SUIT_Severable_Manifest_Members_suit_text_cbor;
};

struct SUIT_Severable_Manifest_Members_suit_text_ {
	struct SUIT_Severable_Manifest_Members_suit_text _SUIT_Severable_Manifest_Members_suit_text;
	uint_fast32_t _SUIT_Severable_Manifest_Members_suit_text_present;
};

struct SUIT_Integrated_Payload_suit_integrated_payload_key {
	struct zcbor_string _SUIT_Integrated_Payload_suit_integrated_payload_key_key;
	struct zcbor_string _SUIT_Integrated_Payload_suit_integrated_payload_key;
};

struct SUIT_Envelope__SUIT_Integrated_Payload {
	struct SUIT_Integrated_Payload_suit_integrated_payload_key _SUIT_Envelope__SUIT_Integrated_Payload;
};

struct SUIT_Envelope {
	struct zcbor_string _SUIT_Envelope_suit_authentication_wrapper;
	struct SUIT_Authentication _SUIT_Envelope_suit_authentication_wrapper_cbor;
	struct zcbor_string _SUIT_Envelope_suit_manifest;
	struct SUIT_Severable_Manifest_Members_suit_text_ _SUIT_Envelope__SUIT_Severable_Manifest_Members;
	struct SUIT_Envelope__SUIT_Integrated_Payload _SUIT_Envelope__SUIT_Integrated_Payload[5];
	uint_fast32_t _SUIT_Envelope__SUIT_Integrated_Payload_count;
};


#endif /* MANIFEST_TYPES_H__ */
