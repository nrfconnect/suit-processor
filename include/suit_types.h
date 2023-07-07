/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_TYPES_H__
#define SUIT_TYPES_H__

#include <stdint.h>
#include <string.h>
#include "zcbor_common.h"
#include "manifest_types.h"


#define SUIT_MAX_NUM_SIGNERS 2  ///! The maximum number of signers.
#define SUIT_MAX_NUM_COMPONENT_ID_PARTS 5  ///! The maximum number of bytestrings in a component ID.
#define SUIT_MAX_NUM_COMPONENTS 4  ///! The maximum number of components referenced in the manifest.
#define SUIT_MAX_NUM_COMPONENT_PARAMS (SUIT_MAX_NUM_COMPONENTS * SUIT_MAX_MANIFEST_DEPTH) ///! The maximum number of active components during processing dependency manifests.
#define SUIT_MAX_NUM_INTEGRATED_PAYLOADS 5  ///! The maximum number of integrated payloads in a single manifest.
#define SUIT_MAX_COMMAND_ARGS 3  ///! The maximum number of arguments consumed by a single command.
#define SUIT_SUIT_SIG_STRUCTURE1_MAX_LENGTH 61  ///! The maximum length of the Sig_structure1 structure. Current value allows to store only 256-bit long digests with key id.
#define SUIT_MAX_SEQ_DEPTH 5  ///! The maximum number of command sequences that may be encapsulated.
#define SUIT_SEQ_EXEC_DEFAULT_STATE 0  ///! The default value of the cmd_exec_state.
#define SUIT_MAX_MANIFEST_DEPTH 3 ///! The maximum nesting level of hierarchical manifests.

/** Errors from the suit API
 *
 * See also https://www.ietf.org/archive/id/draft-ietf-suit-manifest-17.html#name-manifest-processor-setup
 * for more error conditions.
 */
#define SUIT_SUCCESS                      0
#define SUIT_FAIL_CONDITION               1 // Test failed (e.g. Vendor ID/Class ID).
#define SUIT_ERR_TAMP                     3 // Tampering detected in processing.
#define SUIT_ERR_ORDER                    4 // API called in an invalid order.
#define SUIT_ERR_WAIT                     5 // Platform operation should be retried later.
#define SUIT_ERR_DECODING                 6 // Failed to decode the payload.
#define SUIT_ERR_AUTHENTICATION           7 // Envelope authentication failed.
#define SUIT_ERR_MANIFEST_VERIFICATION    8 // Manifest (cryptographic) verification failed.
#define SUIT_ERR_MANIFEST_VALIDATION      9 // Manifest validation failed (rule broken).
#define SUIT_ERR_PAYLOAD_VERIFICATION     10 // Payload verification failed.
#define SUIT_ERR_UNAVAILABLE_PAYLOAD      11 // Payload not available.
#define SUIT_ERR_UNAVAILABLE_COMMAND_SEQ  12 // Command sequence not available. (FATAL?)
#define SUIT_ERR_UNAVAILABLE_PARAMETER    13 // Required parameter not supplied.
#define SUIT_ERR_UNSUPPORTED_COMMAND      14 // Unsupported command encountered.
#define SUIT_ERR_UNSUPPORTED_PARAMETER    15 // Unsupported parameter encountered.
#define SUIT_ERR_UNSUPPORTED_COMPONENT_ID 16 // Unsupported Component Identifier encountered.
#define SUIT_ERR_MISSING_COMPONENT        17 // Missing required component from a Component Set.
#define SUIT_ERR_CRASH                    18 // Application crashed when executed.
#define SUIT_ERR_TIMEOUT                  19 // Watchdog timeout occurred.
#define SUIT_ERR_AGAIN                    100 // The execution has not yet finished. Call the API again.
#define SUIT_ERR_OVERFLOW                 101 // The execution context is too small to handle the command sequence.
#define SUIT_FAIL_SOFT_CONDITION          102 // Test failed (e.g. Vendor ID/Class ID) and soft-failure parameter was set to true.


#define SUIT_ZCBOR_ERR_OFFSET 128
#define ZCBOR_ERR_TO_SUIT_ERR(zcbor_err) ((zcbor_err) + SUIT_ZCBOR_ERR_OFFSET)

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
typedef intptr_t suit_component_t; ///! Handle to more easily refer to a component.

enum suit_bool {
	suit_bool_false = 0x2a17644c, ///! 10 1010 0001 0111 0110 0100 0100 1100
	suit_bool_true = 0x713cf9c6, ///! 111 0001 0011 1100 1111 1001 1100 0110
};

#define SUIT_NUM_STEPS 6

enum suit_command_sequence {
	SUIT_SEQ_INVALID,
	SUIT_SEQ_PARSE,
	SUIT_SEQ_SHARED,
	SUIT_SEQ_DEP_RESOLUTION,
	SUIT_SEQ_PAYLOAD_FETCH,
	SUIT_SEQ_INSTALL,
	SUIT_SEQ_VALIDATE,
	SUIT_SEQ_LOAD,
	SUIT_SEQ_INVOKE,
	SUIT_SEQ_MAX,
};

enum suit_manifest_step {
	SUIT_NO_STEP,
	SUIT_DEP_RESOLUTION,
	SUIT_PAYLOAD_FETCH,
	SUIT_INSTALL,
	SUIT_VALIDATE,
	SUIT_LOAD,
	SUIT_INVOKE,
	SUIT_LAST_STEP,
};

struct suit_manifest_params {
	suit_component_t component_handle;
	struct zcbor_string component_id;
	uint_fast32_t ref_count;

	struct zcbor_string vid;
	struct zcbor_string cid;
	struct zcbor_string image_digest;
	size_t image_size;
	unsigned int component_slot;
	struct zcbor_string uri;
	unsigned int source_component;
	struct zcbor_string invoke_args;
	struct zcbor_string did;

	bool vid_set;
	bool cid_set;
	bool image_digest_set;
	bool image_size_set;
	bool component_slot_set;
	bool uri_set;
	bool source_component_set;
	bool invoke_args_set;
	bool did_set;

	enum suit_bool is_dependency;
	bool integrity_checked;
};

static inline void suit_reset_params(struct suit_manifest_params *params)
{
	suit_component_t bak = params->component_handle;

	memset(params, 0, sizeof(*params));
	params->component_handle = bak;
}

/** @brief Structure describing manifest processor execution state.
 *
 * @note The cmd_exec_state should be used by the command implementation to
 *       distinguish between the first an subsequent calls.
 *       Currently it is used by the run-sequence directive to prevent it
 *       from scheduling the sequence more than once as well as the try-each
 *       directive to select the correct command block.
 */
struct suit_seq_exec_state {
	struct zcbor_string cmd_seq_str; ///! Structure describing the currently executed command sequence.
	size_t n_commands; ///! The number of commands expected inside the currently executed command sequence.
	size_t current_command; ///! The index of currently executed command within the executed command sequence.
	int cmd_exec_state; ///! Optional current command execution state.
	enum suit_bool soft_failure; ///! suit-parameter-soft-failure
	int retval; ///! Value returned by the nested command sequence execution.
	const uint8_t *exec_ptr; ///! The pointer within the currently executed command sequence, pointing to the current command in the sequence.
	size_t current_component_idx; ///! In case of nested command execution - the currently selected component from the component list.
	bool current_components[SUIT_MAX_NUM_COMPONENTS];
	bool current_components_backup[SUIT_MAX_NUM_COMPONENTS]; //! List of components, selected before the execution of command sequences.
};

struct suit_processor_state {
	struct zcbor_string envelope_str;
	enum suit_bool envelope_decoded;
	suit_manifest_envelope_t envelope;
	enum suit_bool envelope_validated;
	struct zcbor_string *key_ids[SUIT_MAX_NUM_SIGNERS];
	size_t num_key_ids;
	enum suit_bool manifest_decoded;
	enum suit_bool manifest_authenticated;
	suit_manifest_t manifest;
	enum suit_bool manifest_validated;
	enum suit_manifest_step current_step;
#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	enum suit_bool dry_run;
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */
	size_t num_components;
	struct suit_manifest_params components[SUIT_MAX_NUM_COMPONENTS];
	size_t seq_stack_height;
	struct suit_seq_exec_state seq_stack[SUIT_MAX_SEQ_DEPTH];
};

/** https://www.iana.org/assignments/cose/cose.xhtml */
enum suit_cose_alg {
	suit_cose_sha256 = -16,
	suit_cose_es256 = -7,
};

struct suit_arg {
	union{struct zcbor_string *bstr; unsigned int uint; } arg;
	enum{bstr, uint} arg_type;
};

struct suit_report {
	int result;
	suit_component_t component_handle;
	unsigned int command; ///! The identifier of the condition or directive
	struct suit_arg argv[SUIT_MAX_COMMAND_ARGS];
	size_t nargs;
	struct zcbor_string *addititional_info;
};


static inline bool suit_compare_zcbor_strings(const struct zcbor_string *str1, const struct zcbor_string *str2)
{
	return (str1 != NULL) && (str2 != NULL) && (str1->len == str2->len) && (memcmp(str1->value, str2->value, str1->len) == 0);
}


#endif /* SUIT_TYPES_H__ */
