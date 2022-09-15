/*
 * Generated using zcbor version 0.5.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef COSE_DECODE_H__
#define COSE_DECODE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"
#include "cose_types.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


int cbor_decode_COSE_Sign1_Tagged(
		const uint8_t *payload, size_t payload_len,
		struct COSE_Sign1 *result,
		size_t *payload_len_out);


int cbor_decode_Sig_structure1(
		const uint8_t *payload, size_t payload_len,
		struct Sig_structure1 *result,
		size_t *payload_len_out);


#endif /* COSE_DECODE_H__ */
