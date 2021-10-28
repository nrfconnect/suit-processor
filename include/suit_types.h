/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUIT_TYPES_H__
#define SUIT_TYPES_H__

#include <stdint.h>
#include <string.h>
#include "cbor_common.h"
#include "manifest_decode_types.h"


#define SUIT_MAX_NUM_SIGNERS 3  ///! The maximum number of signers.
#define SUIT_MAX_NUM_COMPONENTS 16  ///! The maximum number of components referenced in the manifest.
#define SUIT_MAX_COMMAND_ARGS 3 ///! The maximum number of arguments consumed by a single command.

#define SUIT_ERR_SUCCESS 0
#define SUIT_ERR_WAIT 128
#define SUIT_ERR_DECODING 129
#define SUIT_ERR_AUTHENTICATION 130
#define SUIT_ERR_VERIFICATION 131

/** The envelope contains the manifest and its signature plus a few other things.
 *
 *  The envelope contains:
 *  - The SUIT manifest.
 *  - The signature(s) of the manifest. One or more signatures or MACs.
 *  - "severable" manifest members. These are parts of the manifest that can be
 *    discarded when not needed any more, like installation instructions after
 *    an update, or text descriptions that will not be needed. These members
 *    are referenced by digest in the manifest, so they are covered by the
 *    signature.
 *  - Pre-fetched or integrated payloads. When the manifest processor is
 *    fetching payloads, it can check the envelope to see if it's there.
 *    The payload might have been placed there by the manifest creator, or a
 *    previous handler has fetched the payload and placed it there.
 *  - Delegation chains if the manifest is signed by someone else than the owner
 *    of the manifest processor's root of trust key. The delegation chain is a
 *    chain of signatures (like a certificate) leading back to the root of trust
 *    key.
 */
typedef struct SUIT_Envelope suit_manifest_envelope_t;
typedef struct SUIT_Manifest suit_manifest_t;
typedef unsigned int suit_component_t; ///! Handle to more easily refer to a component.

enum suit_bool {
	suit_bool_false = 0x2a17644c, // 10 1010 0001 0111 0110 0100 0100 1100
	suit_bool_true = 0x713cf9c6, // 111 0001 0011 1100 1111 1001 1100 0110
};

enum suit_manifest_step {
	NO_STEP,
	SUIT_PAYLOAD_FETCH,
	SUIT_INSTALL,
	SUIT_VALIDATE,
	SUIT_LOAD,
	SUIT_RUN,
};

struct suit_manifest_params {
	unsigned int component_handle;

	struct suit_parameter_vendor_identifier *vid;
	struct suit_parameter_class_identifier *cid;
	struct SUIT_Digest *image_digest;
	unsigned int image_size;
	unsigned int component_slot;
	cbor_string_type_t *uri;
	unsigned int source_component;
	cbor_string_type_t *fetch_args;
	cbor_string_type_t *run_args;
	struct suit_parameter_device_identifier *did;

	bool vid_set;
	bool cid_set;
	bool image_digest_set;
	bool image_size_set;
	bool component_slot_set;
	bool uri_set;
	bool source_component_set;
	bool fetch_args_set;
	bool run_args_set;
	bool did_set;
};

static inline void suit_reset_params(struct suit_manifest_params *params)
{
	memset(params, sizeof(*params), 0);
}

struct suit_processor_state {
	enum suit_bool envelope_decoded;
	suit_manifest_envelope_t envelope;
	enum suit_bool envelope_validated;
	cbor_string_type_t *key_ids[SUIT_MAX_NUM_SIGNERS];
	size_t num_key_ids;
	enum suit_bool manifest_decoded;
	enum suit_bool manifest_authenticated;
	suit_manifest_t manifest;
	enum suit_bool manifest_validated;
	enum suit_manifest_step previous_step;
	unsigned int component_indices[SUIT_MAX_NUM_COMPONENTS]; // Current component(s)
	size_t num_component_indices;
	struct suit_manifest_params components[SUIT_MAX_NUM_COMPONENTS];
	bool soft_failure; // suit-parameter-soft-failure
};

static inline void suit_reset_state(struct suit_processor_state *state)
{
	memset(state, sizeof(*state), 0);
	state->envelope_decoded = suit_bool_false;
	state->envelope_validated = suit_bool_false;
	state->manifest_authenticated = suit_bool_false;
	state->manifest_decoded = suit_bool_false;
	state->manifest_validated = suit_bool_false;
}

enum suit_component_properties {
	suit_comp_writable = 1,
	suit_comp_readable = 2,
	suit_comp_runnable = 4,
};

/** https://www.iana.org/assignments/cose/cose.xhtml */
enum suit_cose_alg {
	suit_cose_sha256 = -16,
	suit_cose_es256 = -7,
};

struct suit_arg {
	union{cbor_string_type_t *bstr; unsigned int uint;} arg;
	enum{bstr, uint} arg_type;
};

struct suit_report {
	int result;
	suit_component_t component_handle;
	unsigned int command; ///! The identifier of the condition or directive
	struct suit_arg argv[SUIT_MAX_COMMAND_ARGS];
	size_t nargs;
	cbor_string_type_t *addititional_info;
};

#endif /* SUIT_TYPES_H__ */
