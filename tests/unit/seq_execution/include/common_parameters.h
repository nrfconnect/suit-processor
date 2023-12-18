/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zcbor_common.h>
#include <stddef.h>

#ifndef COMMON_PARAMETERS_H__
#define COMMON_PARAMETERS_H__

/** @brief Enumeration, listing parameters with their value, returned by
 *         bootstrap_parameters(..) function.
 */
enum parameter_values {
	VENDOR_ID,
	CLASS_ID,
	IMAGE_DIGEST,
	COMPONENT_SLOT,
	SOFT_FAILURE,
	IMAGE_SIZE,
	CONTENT,
	URI,
	SOURCE_COMPONENT,
	INVOKE_ARGS,
	DEVICE_ID,
};

/** @brief Expected vendor identifier, embedded inside VENDOR_ID sequence. */
extern struct zcbor_string exp_vid;

/** @brief Expected class identifier, embedded inside CLASS_ID sequence. */
extern struct zcbor_string exp_cid;

/** @brief Expected image digest (SHA-256), embedded inside IMAGE_DIGEST sequence. */
extern struct zcbor_string exp_digest;

/** @brief Expected image digest (SHA-512), embedded inside IMAGE_DIGEST sequence. */
extern struct zcbor_string exp_digest_sha512;

/** @brief Expected slot number, embedded inside COMPONENT_SLOT sequence. */
extern uint32_t exp_slot;

/** @brief Expected image size, embedded inside IMAGE_SIZE sequence. */
extern size_t exp_image_size;

/** @brief Expected content parameter, embedded inside CONTENT sequence. */
extern struct zcbor_string exp_content;

/** @brief Expected URI, embedded inside URI sequence. */
extern struct zcbor_string exp_uri;

/** @brief Expected source component index, embedded inside SOURCE_COMPONENT sequence. */
extern size_t exp_source_component;

/** @brief Expected invoke arguments, embedded inside INVOKE_ARGS sequence. */
extern struct zcbor_string exp_args;

/** @brief Expected device identifier, embedded inside DEVICE_ID sequence. */
extern struct zcbor_string exp_did;

/** @brief Generate sequence with parameter and a sample value.
 *
 * @param[in]   parameters    List of parameters to include inside output sequence.
 * @param[in]   n_parameters  Number of parameters inside the input list.
 * @param[out]  buf           Pointer to the buffer, to be filled with the sequence.
 *
 * @returns Size of the generated sequence, zero in case of error.
 */
size_t bootstrap_parameters(enum parameter_values *parameters, size_t n_parameters, uint8_t *buf, size_t buf_size);

#endif /* COMMON_PARAMETERS_H__ */
