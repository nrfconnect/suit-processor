/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_TYPES_H__
#define SUIT_TYPES_H__

#include <stdint.h>
#include <string.h>
#include <zcbor_common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The maximum number of signers. */
#define SUIT_MAX_NUM_SIGNERS		    2
/** The maximum number of bytestrings in a component ID. */
#define SUIT_MAX_NUM_COMPONENT_ID_PARTS	    5
/** The maximum number of components referenced in the manifest. */
#define SUIT_MAX_NUM_COMPONENTS		    12
/** The maximum number of active components during processing dependency manifests. */
#define SUIT_MAX_NUM_COMPONENT_PARAMS	    (SUIT_MAX_NUM_COMPONENTS * SUIT_MANIFEST_STACK_MAX_ENTRIES)
/** The maximum number of integrated payloads in a single manifest. */
#define SUIT_MAX_NUM_INTEGRATED_PAYLOADS    6
/** The maximum number of arguments consumed by a single command. */
#define SUIT_MAX_COMMAND_ARGS		    3
/** The maximum length of the Sig_structure1 structure.
 *  Current value allows to store up to 512-bit long digests with 32-bit key id.
 */
#define SUIT_SUIT_SIG_STRUCTURE1_MAX_LENGTH 95
/** The maximum number of command sequences that may be encapsulated. */
#define SUIT_MAX_SEQ_DEPTH		    5
/** The default value of the cmd_exec_state. */
#define SUIT_SEQ_EXEC_DEFAULT_STATE	    0
/** The maximum nesting level of hierarchical manifests. */
#define SUIT_MAX_MANIFEST_DEPTH		    3
/** Maximum amount of manifests to be stored on stack.
 *  One entry for each manifest level + one for additional processing.
 */
#define SUIT_MANIFEST_STACK_MAX_ENTRIES	    (SUIT_MAX_MANIFEST_DEPTH + 1)

/** Errors from the suit API
 *
 * See also
 * https://www.ietf.org/archive/id/draft-ietf-suit-manifest-25.html#name-manifest-processor-setup
 * and https://www.ietf.org/archive/id/draft-ietf-suit-report-08.html#section-4-14
 * for more report reasons and error conditions.
 */
/** Execution successful. */
#define SUIT_SUCCESS			  0
/** Test failed (e.g. Vendor ID/Class ID). */
#define SUIT_FAIL_CONDITION		  1
/** Tampering detected in processing. */
#define SUIT_ERR_TAMP			  3
/** API called in an invalid order. */
#define SUIT_ERR_ORDER			  4
/** Platform operation should be retried later. */
#define SUIT_ERR_WAIT			  5
/** Failed to decode the payload. */
#define SUIT_ERR_DECODING		  6
/** Envelope authentication failed. */
#define SUIT_ERR_AUTHENTICATION		  7
/**  Manifest (cryptographic) verification failed. */
#define SUIT_ERR_MANIFEST_VERIFICATION	  8
/**  Manifest validation failed (rule broken). */
#define SUIT_ERR_MANIFEST_VALIDATION	  9
/** Payload verification failed. */
#define SUIT_ERR_PAYLOAD_VERIFICATION	  10
/** Payload not available. */
#define SUIT_ERR_UNAVAILABLE_PAYLOAD	  11
/** Command sequence not available. (FATAL?) */
#define SUIT_ERR_UNAVAILABLE_COMMAND_SEQ  12
/** Required parameter not supplied. */
#define SUIT_ERR_UNAVAILABLE_PARAMETER	  13
/** Unsupported command encountered. */
#define SUIT_ERR_UNSUPPORTED_COMMAND	  14
/** Unsupported parameter encountered. */
#define SUIT_ERR_UNSUPPORTED_PARAMETER	  15
/** Unsupported Component Identifier encountered. */
#define SUIT_ERR_UNSUPPORTED_COMPONENT_ID 16
/** Missing required component from a Component Set. */
#define SUIT_ERR_MISSING_COMPONENT	  17
/** Application crashed when executed. */
#define SUIT_ERR_CRASH			  18
/** Watchdog timeout occurred. */
#define SUIT_ERR_TIMEOUT		  19
/** Unsupported type of COSE structure encountered. */
#define SUIT_ERR_UNSUPPORTED_COSE	  20
/** Unsupported COSE algorithm encountered. */
#define SUIT_ERR_UNSUPPORTED_ALG	  21
/** Unauthorized component ID. */
#define SUIT_ERR_UNAUTHORIZED_COMPONENT	  22
/** Severed command sequence not authorized. */
#define SUIT_ERR_UNAUTHORIZED_COMMAND_SEQ 23
/** The execution has not yet finished. Call the API again. */
#define SUIT_ERR_AGAIN			  100
/** The execution context is too small to handle the command sequence. */
#define SUIT_ERR_OVERFLOW		  101
/** Test failed (e.g. Vendor ID/Class ID) and soft-failure parameter was set to true. */
#define SUIT_FAIL_SOFT_CONDITION	  102

/** Offset for all ZCBOR parsing error codes. */
#define SUIT_ZCBOR_ERR_OFFSET		 128
#define ZCBOR_ERR_TO_SUIT_ERR(zcbor_err) ((zcbor_err) + SUIT_ZCBOR_ERR_OFFSET)

/** Handle to more easily refer to a component. */
typedef intptr_t suit_component_t;

enum suit_command_sequence {
	SUIT_SEQ_INVALID,
	SUIT_SEQ_PARSE,
	SUIT_SEQ_SHARED,
	SUIT_SEQ_DEP_RESOLUTION,
	SUIT_SEQ_PAYLOAD_FETCH,
	SUIT_SEQ_CAND_VERIFICATION,
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
	suit_cose_VS_HashEdDSA = -65537,
	suit_cose_aes256_gcm = 3,
	suit_cose_aes256_kw = -5,
	suit_cose_direct = -6,
};

struct suit_arg {
	union {
		struct zcbor_string *bstr;
		unsigned int num;
	} arg;
	enum {
		bstr,
		num
	} arg_type;
};

struct suit_report {
	int result;
	suit_component_t component_handle;
	unsigned int command; ///! The identifier of the condition or directive
	struct suit_arg argv[SUIT_MAX_COMMAND_ARGS];
	size_t nargs;
	struct zcbor_string *addititional_info;
};

struct suit_aes_kw_data {
	/** @brief The key encryption key identifier. */
	struct zcbor_string key_id;
	/** @brief The encrypted encryption key. */
	struct zcbor_string ciphertext;
};

struct suit_direct_kw_data {
	/** @brief The content encryption key identifier. */
	struct zcbor_string key_id;
};

union suit_key_encryption_data {
	/** @brief AES Key Wrap. */
	struct suit_aes_kw_data aes;
	struct suit_direct_kw_data direct;
};

struct suit_encryption_info {
	/** @brief COSE algorithm ID for the content encryption method. */
	enum suit_cose_alg enc_alg_id;
	/** @brief The Initialization Vector (IV) value.
	 *
	 * For some symmetric encryption algorithms, this may be referred to as a nonce.
	 * The IV can be placed in the unprotected bucket, since for AE and AEAD algorithms,
	 * modifying the IV will cause the decryption to fail.
	 */
	struct zcbor_string IV;
	/** @brief A byte string to be used as the authenticated data structure. */
	struct zcbor_string aad;
	/** @brief COSE algorithm ID for the key wrap method. */
	enum suit_cose_alg kw_alg_id;
	/** @brief Wrapped key. */
	union suit_key_encryption_data kw_key;
};

static inline bool suit_compare_zcbor_strings(const struct zcbor_string *str1,
					      const struct zcbor_string *str2)
{
	return (str1 != NULL) && (str2 != NULL) && (str1->len == str2->len) &&
	       (memcmp(str1->value, str2->value, str1->len) == 0);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUIT_TYPES_H__ */
