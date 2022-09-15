/*
 * Generated using zcbor version 0.5.1
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef COSE_ENCODE_H__
#define COSE_ENCODE_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "cose_types.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif


int cbor_encode_COSE_Sign1_Tagged(
		uint8_t *payload, size_t payload_len,
		const struct COSE_Sign1 *input,
		size_t *payload_len_out);


int cbor_encode_Sig_structure1(
		uint8_t *payload, size_t payload_len,
		const struct Sig_structure1 *input,
		size_t *payload_len_out);


#endif /* COSE_ENCODE_H__ */
