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
#include "suit_err.h"

#define SUIT_MAX_NUM_SIGNERS 2  ///! The maximum number of signers.
#define SUIT_MAX_NUM_COMPONENT_ID_PARTS 5  ///! The maximum number of bytestrings in a component ID.
#define SUIT_MAX_NUM_COMPONENTS 4  ///! The maximum number of components referenced in the manifest.
#define SUIT_MAX_NUM_COMPONENT_PARAMS (SUIT_MAX_NUM_COMPONENTS * SUIT_MAX_MANIFEST_DEPTH) ///! The maximum number of active components during processing dependency manifests.
#define SUIT_MAX_NUM_INTEGRATED_PAYLOADS 5  ///! The maximum number of integrated payloads in a single manifest.
#define SUIT_MAX_COMMAND_ARGS 3  ///! The maximum number of arguments consumed by a single command.
#define SUIT_SUIT_SIG_STRUCTURE1_MAX_LENGTH 62  ///! The maximum length of the Sig_structure1 structure. Current value allows to store only 256-bit long digests with key id.
#define SUIT_MAX_SEQ_DEPTH 5  ///! The maximum number of command sequences that may be encapsulated.
#define SUIT_SEQ_EXEC_DEFAULT_STATE 0  ///! The default value of the cmd_exec_state.
#define SUIT_MAX_MANIFEST_DEPTH 3 ///! The maximum nesting level of hierarchical manifests.

typedef intptr_t suit_component_t; ///! Handle to more easily refer to a component.

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

/** https://www.iana.org/assignments/cose/cose.xhtml */
enum suit_cose_alg {
	suit_cose_sha512 = -44,
	suit_cose_sha256 = -16,
	suit_cose_es256 = -7,
	suit_cose_EdDSA = -8,
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
