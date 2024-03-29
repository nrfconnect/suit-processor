/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_PROCESSOR_H__
#define SUIT_PROCESSOR_H__

#include <suit_types.h>
#include "manifest_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Forward declaration of the SUIT processor state, used inside the command processor callback. */
struct suit_processor_state;

/** @brief Type of the command to be handled. */
enum command_type {
	SUIT_COMMAND_INVALID,
	SUIT_COMMAND_CONDITION,
	SUIT_COMMAND_DIRECTIVE,
};

/** @brief Single SUIT command structure. */
typedef struct {
	enum command_type type;
	union {
		struct SUIT_Condition_r condition;
		struct SUIT_Directive_r directive;
	};
} suit_command_t;

/** @brief Single command handler function prototype.
 *
 * @param[in] state    The SUIT processor state.
 * @param[in] command  The pointer to the command structure to execute.
 */
typedef int (*seq_exec_processor_t)(struct suit_processor_state *state, suit_command_t *command);

enum suit_bool {
	suit_bool_false = 0x2a17644c, ///! 10 1010 0001 0111 0110 0100 0100 1100
	suit_bool_true = 0x713cf9c6, ///! 111 0001 0011 1100 1111 1001 1100 0110
};

struct suit_manifest_params {
	suit_component_t component_handle;
	struct zcbor_string component_id;
	uint_fast32_t ref_count;

	struct zcbor_string vid;
	struct zcbor_string cid;
	struct zcbor_string image_digest;
	size_t image_size;
	struct zcbor_string content;
	unsigned int component_slot;
	struct zcbor_string uri;
	unsigned int source_component;
	struct zcbor_string invoke_args;
	struct zcbor_string did;

	bool vid_set;
	bool cid_set;
	bool image_digest_set;
	bool image_size_set;
	bool content_set;
	bool component_slot_set;
	bool uri_set;
	bool source_component_set;
	bool invoke_args_set;
	bool did_set;

	enum suit_bool is_dependency;
	bool integrity_checked;
};

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

	struct zcbor_string candidate_verification_seq;
	enum suit_seq_status candidate_verification_seq_status;

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
	seq_exec_processor_t cmd_processor; ///! The command processor to use to process the sequence.
	struct suit_manifest_state *manifest; ///! The reference to the current manifest structure.
	const uint8_t *exec_ptr; ///! The pointer within the currently executed command sequence, pointing to the current command in the sequence.
	size_t current_component_idx; ///! In case of nested command execution - the currently selected component from the component list.
	bool current_components[SUIT_MAX_NUM_COMPONENTS];
	bool current_components_backup[SUIT_MAX_NUM_COMPONENTS]; //! List of components, selected before the execution of command sequences.
};

struct suit_processor_state {
	struct suit_decoder_state decoder_state;
	enum suit_command_sequence current_seq;

#ifdef SUIT_PLATFORM_DRY_RUN_SUPPORT
	enum suit_bool dry_run;
#endif /* SUIT_PLATFORM_DRY_RUN_SUPPORT */

	struct suit_manifest_params components[SUIT_MAX_NUM_COMPONENT_PARAMS];

	size_t manifest_stack_height;
	struct suit_manifest_state manifest_stack[SUIT_MANIFEST_STACK_MAX_ENTRIES];

	size_t seq_stack_height;
	struct suit_seq_exec_state seq_stack[SUIT_MAX_SEQ_DEPTH];
};


/** @brief Populate the manifest stack by loading a new envelope.
 *
 * @details This API will:
 *           - Reset the decoder state.
 *           - Parse the input CBOR envelope.
 *           - Check integrity of the manifest.
 *           - Parse the CBOR manifest from the input envelope.
 *           - Verify the signature of the manifest digest.
 *           - Authorize the manifest class ID as well as the list of components.
 *           - Decode the command sequences.
 *           - Authorize the manifest sequence number.
 *           - Populate component handles using platform API.
 *           - Commit the manifest stack by increasing the height variable.
 *
 * @param[in]  state         The SUIT processor state to be modified.
 * @param[in]  envelope_str  Reference to the input envelope to be loaded.
 * @param[in]  envelope_len  Length of the input envelope.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_processor_load_envelope(struct suit_processor_state *state, const uint8_t *envelope_str, size_t envelope_len);

#ifdef CONFIG_UNITY
/** @brief Override the internal state variable with a pointer to the external memory.
 *
 * @details This API is meant to be used inside unit tests, so the test runner
 *          is able to control and assert on the internal state of the module.
 *
 * @param[in]  new_state  The pointer to the SUIT processor state to use.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_processor_override_state(struct suit_processor_state *new_state);
#endif /* CONFIG_UNITY */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUIT_PROCESSOR_H__ */
