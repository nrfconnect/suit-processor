/*
 * Generated using zcbor version 0.5.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef COSE_TYPES_H__
#define COSE_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 3

struct header_map_key_id {
	struct zcbor_string _header_map_key_id;
};

struct header_map {
	struct header_map_key_id _header_map_key_id;
	uint_fast32_t _header_map_key_id_present;
};

struct Sig_structure1 {
	struct zcbor_string _Sig_structure1_body_protected;
	struct header_map _Sig_structure1_body_protected_cbor;
	struct zcbor_string _Sig_structure1_payload;
};

struct Headers {
	struct zcbor_string _Headers_protected;
	struct header_map _Headers_protected_cbor;
};

struct COSE_Sign1 {
	struct Headers _COSE_Sign1__Headers;
	struct zcbor_string _COSE_Sign1_signature;
};


#endif /* COSE_TYPES_H__ */
