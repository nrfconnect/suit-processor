/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_ERR_H__
#define SUIT_ERR_H__

/** Errors from the suit API
 *
 * See also https://www.ietf.org/archive/id/draft-ietf-suit-manifest-17.html#name-manifest-processor-setup
 * for more error conditions.
 */

typedef int suit_err_t;

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

#define SUIT_PROCESSOR_ERR_MAX (SUIT_ZCBOR_ERR_OFFSET + 128)

#endif /* SUIT_TYPES_H__ */
