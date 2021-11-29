/*
 * Generated using cddl_gen version 0.3.0
 * https://github.com/NordicSemiconductor/cddl-gen
 * Generated with a default_max_qty of 3
 */

#ifndef MANIFEST_DECODE_TYPES_H__
#define MANIFEST_DECODE_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "cbor_decode.h"

#define DEFAULT_MAX_QTY 3

struct SUIT_Component_Identifier {
 	cbor_string_type_t _SUIT_Component_Identifier_bstr[3];
	uint32_t _SUIT_Component_Identifier_bstr_count;
};

struct SUIT_Components {
 	struct SUIT_Component_Identifier _SUIT_Components__SUIT_Component_Identifier[3];
	uint32_t _SUIT_Components__SUIT_Component_Identifier_count;
};

struct SUIT_Common_suit_components {
 	struct SUIT_Components _SUIT_Common_suit_components;
};

struct SUIT_Common_suit_common_sequence {
 	cbor_string_type_t _SUIT_Common_suit_common_sequence;
};

struct SUIT_Common {
 	struct SUIT_Common_suit_components _SUIT_Common_suit_components;
	uint32_t _SUIT_Common_suit_components_present;
	struct SUIT_Common_suit_common_sequence _SUIT_Common_suit_common_sequence;
	uint32_t _SUIT_Common_suit_common_sequence_present;
};

struct SUIT_Manifest_suit_reference_uri {
 	cbor_string_type_t _SUIT_Manifest_suit_reference_uri;
};

struct SUIT_Severable_Members_Choice_suit_payload_fetch {
 	cbor_string_type_t _SUIT_Severable_Members_Choice_suit_payload_fetch;
};

struct SUIT_Severable_Members_Choice_suit_install {
 	cbor_string_type_t _SUIT_Severable_Members_Choice_suit_install;
};

struct SUIT_Digest {
 	cbor_string_type_t _SUIT_Digest_suit_digest_bytes;
};

struct SUIT_Severable_Members_Choice_suit_text {
 	struct SUIT_Digest _SUIT_Severable_Members_Choice_suit_text;
};

struct SUIT_Severable_Members_Choice_ {
 	struct SUIT_Severable_Members_Choice_suit_payload_fetch _SUIT_Severable_Members_Choice_suit_payload_fetch;
	uint32_t _SUIT_Severable_Members_Choice_suit_payload_fetch_present;
	struct SUIT_Severable_Members_Choice_suit_install _SUIT_Severable_Members_Choice_suit_install;
	uint32_t _SUIT_Severable_Members_Choice_suit_install_present;
	struct SUIT_Severable_Members_Choice_suit_text _SUIT_Severable_Members_Choice_suit_text;
	uint32_t _SUIT_Severable_Members_Choice_suit_text_present;
};

struct SUIT_Unseverable_Members_suit_validate {
 	cbor_string_type_t _SUIT_Unseverable_Members_suit_validate;
};

struct SUIT_Unseverable_Members_suit_load {
 	cbor_string_type_t _SUIT_Unseverable_Members_suit_load;
};

struct SUIT_Unseverable_Members_suit_run {
 	cbor_string_type_t _SUIT_Unseverable_Members_suit_run;
};

struct SUIT_Unseverable_Members_ {
 	struct SUIT_Unseverable_Members_suit_validate _SUIT_Unseverable_Members_suit_validate;
	uint32_t _SUIT_Unseverable_Members_suit_validate_present;
	struct SUIT_Unseverable_Members_suit_load _SUIT_Unseverable_Members_suit_load;
	uint32_t _SUIT_Unseverable_Members_suit_load_present;
	struct SUIT_Unseverable_Members_suit_run _SUIT_Unseverable_Members_suit_run;
	uint32_t _SUIT_Unseverable_Members_suit_run_present;
};

struct SUIT_Manifest {
 	uint32_t _SUIT_Manifest_suit_manifest_sequence_number;
	cbor_string_type_t _SUIT_Manifest_suit_common;
	struct SUIT_Common _SUIT_Manifest_suit_common_cbor;
	struct SUIT_Manifest_suit_reference_uri _SUIT_Manifest_suit_reference_uri;
	uint32_t _SUIT_Manifest_suit_reference_uri_present;
	struct SUIT_Severable_Members_Choice_ _SUIT_Manifest__SUIT_Severable_Members_Choice;
	struct SUIT_Unseverable_Members_ _SUIT_Manifest__SUIT_Unseverable_Members;
};

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

struct IndexArg_ {
 	union {
		uint32_t _IndexArg_uint;
		bool _IndexArg_bool;
		struct {
			uint32_t _IndexArg__uint_uint[3];
			uint32_t _IndexArg__uint_uint_count;
		};
	};
	enum {
		_IndexArg_uint,
		_IndexArg_bool,
		_IndexArg__uint,
	} _IndexArg_choice;
};

struct SUIT_Parameters_ {
 	union {
		struct {
			cbor_string_type_t _SUIT_Parameters_suit_parameter_vendor_identifier;
		};
		struct {
			cbor_string_type_t _SUIT_Parameters_suit_parameter_class_identifier;
		};
		struct {
			cbor_string_type_t _SUIT_Parameters_suit_parameter_image_digest;
			struct SUIT_Digest _SUIT_Parameters_suit_parameter_image_digest_cbor;
		};
		struct {
			uint32_t _SUIT_Parameters_suit_parameter_image_size;
		};
		struct {
			uint32_t _SUIT_Parameters_suit_parameter_component_slot;
		};
		struct {
			cbor_string_type_t _SUIT_Parameters_suit_parameter_uri;
		};
		struct {
			uint32_t _SUIT_Parameters_suit_parameter_source_component;
		};
		struct {
			cbor_string_type_t _SUIT_Parameters_suit_parameter_device_identifier;
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
		_SUIT_Parameters_suit_parameter_device_identifier = 24,
	} _SUIT_Parameters_choice;
};

struct __suit_directive_override_parameters_map__SUIT_Parameters {
 	struct SUIT_Parameters_ ___suit_directive_override_parameters_map__SUIT_Parameters;
};

struct SUIT_Common_Commands_ {
 	union {
		struct {
			struct IndexArg_ _SUIT_Common_Commands___suit_directive_set_component_index__IndexArg;
		};
		struct {
			struct __suit_directive_override_parameters_map__SUIT_Parameters ___suit_directive_override_parameters_map__SUIT_Parameters[3];
			uint32_t ___suit_directive_override_parameters_map__SUIT_Parameters_count;
		};
	};
	enum {
		_SUIT_Common_Commands___suit_directive_set_component_index = 12,
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
	uint32_t _SUIT_Common_Sequence_union_count;
};

struct SUIT_Directive_Try_Each_Argument {
 	cbor_string_type_t _SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr[3];
	uint32_t _SUIT_Directive_Try_Each_Argument_SUIT_Command_Sequence_bstr_count;
	uint32_t _SUIT_Directive_Try_Each_Argument_nil_present;
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
			struct __suit_directive_override_parameters_map__SUIT_Parameters ___suit_directive_override_parameters_map__SUIT_Parameters[3];
			uint32_t ___suit_directive_override_parameters_map__SUIT_Parameters_count;
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
	uint32_t _SUIT_Command_Sequence_union_count;
};

struct empty_Headers {
 };

struct header_map_key_id {
 	cbor_string_type_t _header_map_key_id;
};

struct header_map {
 	struct header_map_key_id _header_map_key_id;
	uint32_t _header_map_key_id_present;
};

struct Headers {
 	cbor_string_type_t _Headers_protected;
	struct header_map _Headers_protected_cbor;
};

struct COSE_Signature {
 	struct Headers _COSE_Signature__Headers;
	cbor_string_type_t _COSE_Signature_signature;
};

struct COSE_Sign {
 	struct empty_Headers _COSE_Sign__empty_Headers;
	struct COSE_Signature _COSE_Sign_signatures__COSE_Signature[3];
	uint32_t _COSE_Sign_signatures__COSE_Signature_count;
};

struct COSE_Sign1 {
 	struct Headers _COSE_Sign1__Headers;
	cbor_string_type_t _COSE_Sign1_signature;
};

struct SUIT_Authentication_Block_ {
 	union {
		struct COSE_Sign _SUIT_Authentication_Block__COSE_Sign_Tagged;
		struct COSE_Sign1 _SUIT_Authentication_Block__COSE_Sign1_Tagged;
	};
	enum {
		_SUIT_Authentication_Block__COSE_Sign_Tagged,
		_SUIT_Authentication_Block__COSE_Sign1_Tagged,
	} _SUIT_Authentication_Block_choice;
};

struct SUIT_Authentication_Block_bstr {
 	cbor_string_type_t _SUIT_Authentication_Block_bstr;
	struct SUIT_Authentication_Block_ _SUIT_Authentication_Block_bstr_cbor;
};

struct SUIT_Authentication {
 	cbor_string_type_t _SUIT_Authentication_SUIT_Digest_bstr;
	struct SUIT_Digest _SUIT_Authentication_SUIT_Digest_bstr_cbor;
	struct SUIT_Authentication_Block_bstr _SUIT_Authentication_Block_bstr[3];
	uint32_t _SUIT_Authentication_Block_bstr_count;
};

struct SUIT_Text_Map_uinttstr {
 	uint32_t _SUIT_Text_Map_uinttstr_key;
	cbor_string_type_t _SUIT_Text_Map_uinttstr;
};

struct SUIT_Component_Identifier_uinttstr {
 	uint32_t _SUIT_Component_Identifier_uinttstr_key;
	cbor_string_type_t _SUIT_Component_Identifier_uinttstr;
};

struct SUIT_Text_Map_SUIT_Component_Identifier_ {
 	struct SUIT_Component_Identifier _SUIT_Text_Map_SUIT_Component_Identifier_key;
	struct SUIT_Component_Identifier_uinttstr _SUIT_Component_Identifier_uinttstr[3];
	uint32_t _SUIT_Component_Identifier_uinttstr_count;
};

struct SUIT_Text_Map {
 	struct SUIT_Text_Map_uinttstr _SUIT_Text_Map_uinttstr[3];
	uint32_t _SUIT_Text_Map_uinttstr_count;
	struct SUIT_Text_Map_SUIT_Component_Identifier_ _SUIT_Text_Map_SUIT_Component_Identifier[3];
	uint32_t _SUIT_Text_Map_SUIT_Component_Identifier_count;
};

struct SUIT_Severable_Manifest_Members_suit_text {
 	cbor_string_type_t _SUIT_Severable_Manifest_Members_suit_text;
	struct SUIT_Text_Map _SUIT_Severable_Manifest_Members_suit_text_cbor;
};

struct SUIT_Severable_Manifest_Members_suit_text_ {
 	struct SUIT_Severable_Manifest_Members_suit_text _SUIT_Severable_Manifest_Members_suit_text;
	uint32_t _SUIT_Severable_Manifest_Members_suit_text_present;
};

struct SUIT_Integrated_Payload_suit_integrated_payload_key {
 	cbor_string_type_t _SUIT_Integrated_Payload_suit_integrated_payload_key_key;
	cbor_string_type_t _SUIT_Integrated_Payload_suit_integrated_payload_key;
};

struct SUIT_Envelope__SUIT_Integrated_Payload {
 	struct SUIT_Integrated_Payload_suit_integrated_payload_key _SUIT_Envelope__SUIT_Integrated_Payload;
};

struct SUIT_Envelope {
 	cbor_string_type_t _SUIT_Envelope_suit_authentication_wrapper;
	struct SUIT_Authentication _SUIT_Envelope_suit_authentication_wrapper_cbor;
	cbor_string_type_t _SUIT_Envelope_suit_manifest;
	struct SUIT_Severable_Manifest_Members_suit_text_ _SUIT_Envelope__SUIT_Severable_Manifest_Members;
	struct SUIT_Envelope__SUIT_Integrated_Payload _SUIT_Envelope__SUIT_Integrated_Payload[3];
	uint32_t _SUIT_Envelope__SUIT_Integrated_Payload_count;
};


#endif /* MANIFEST_DECODE_TYPES_H__ */
