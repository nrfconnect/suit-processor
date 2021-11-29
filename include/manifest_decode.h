/*
 * Generated using zcbor version 0.5.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef MANIFEST_DECODE_H__
#define MANIFEST_DECODE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"
#include "manifest_types.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


int cbor_decode_SUIT_Envelope_Tagged(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Envelope *result,
		size_t *payload_len_out);


int cbor_decode_SUIT_Manifest(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Manifest *result,
		size_t *payload_len_out);


int cbor_decode_SUIT_Common_Sequence(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Common_Sequence *result,
		size_t *payload_len_out);


int cbor_decode_SUIT_Command_Sequence(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Command_Sequence *result,
		size_t *payload_len_out);


int cbor_decode_SUIT_Condition(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Condition_ *result,
		size_t *payload_len_out);


int cbor_decode_SUIT_Directive(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Directive_ *result,
		size_t *payload_len_out);


int cbor_decode_SUIT_Common_Commands(
		const uint8_t *payload, size_t payload_len,
		struct SUIT_Common_Commands_ *result,
		size_t *payload_len_out);


#endif /* MANIFEST_DECODE_H__ */
