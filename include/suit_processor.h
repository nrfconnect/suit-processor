/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_PROCESSOR_H__
#define SUIT_PROCESSOR_H__

#include <suit_types.h>
#include "manifest_types.h"


enum suit_decoder_step {
	INVALID,
	INITIALIZED,
	ENVELOPE_DECODED,
	MANIFEST_DIGEST_VERIFIED,
	MANIFEST_DECODED,
	MANIFEST_AUTHENTICATED,
	MANIFEST_AUTHORIZED,
	SEQUENCES_DECODED,
	COMPONENTS_CREATED,
	LAST_STEP,
};

struct suit_decoder_state {
	struct suit_manifest_state *decoded_manifest;
	enum suit_decoder_step step;

	struct zcbor_string manifest_digest_bytes;
	struct zcbor_string authentication_bstr[2];
	uint_fast32_t authentication_bstr_count;

	union {
		suit_manifest_envelope_t envelope;
		suit_manifest_t manifest;
	};
};

enum suit_seq_status {
	UNAVAILABLE,
	SEVERED,
	AUTHENTICATED,
};

struct suit_integrated_payload {
	struct zcbor_string key;
	struct zcbor_string payload;
};

struct suit_manifest_state {
	struct zcbor_string envelope_str;
	struct zcbor_string manifest_component_id;
	uint32_t sequence_number;

	struct suit_integrated_payload integrated_payloads[SUIT_MAX_NUM_INTEGRATED_PAYLOADS];
	size_t integrated_payloads_count;

	size_t component_map[SUIT_MAX_NUM_COMPONENTS];
	size_t components_count;

	struct zcbor_string shared_sequence;
	enum suit_seq_status shared_sequence_status;

	struct zcbor_string dependency_resolution_seq;
	enum suit_seq_status dependency_resolution_seq_status;

	struct zcbor_string payload_fetch_seq;
	enum suit_seq_status payload_fetch_seq_status;

	struct zcbor_string install_seq;
	enum suit_seq_status install_seq_status;

	struct zcbor_string validate_seq;
	enum suit_seq_status validate_seq_status;

	struct zcbor_string load_seq;
	enum suit_seq_status load_seq_status;

	struct zcbor_string invoke_seq;
	enum suit_seq_status invoke_seq_status;

	struct zcbor_string text;
	enum suit_seq_status text_status;
};

#endif /* SUIT_PROCESSOR_H__ */
