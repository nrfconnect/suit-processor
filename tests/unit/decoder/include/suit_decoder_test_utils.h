/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef _SUIT_DECODER_TEST_UTILS_H
#define _SUIT_DECODER_TEST_UTILS_H

#include <unity.h>
#include <suit_decoder.h>

/** @brief Helper structure for constructing series of negative test cases
 */
struct input_envelope {
	uint8_t *envelope;
	size_t envelope_size;
	int exp_ret;
};


/** @brief Manifest decoder state declaration, defined inside the main.c file.
 */
extern struct suit_decoder_state state;

/** @brief Manifest state declaration, defined inside the main.c file.
 */
extern struct suit_manifest_state manifest;


/** @brief Initializes the manifest decoder state with the static memory, defined inside main.c file.
 */
void init_static_mem_input(void);

/** @brief Initializes the manifest decoder state with the static memory, defined inside main.c file.
 *         Additionally, it decodes the envelope using the suit_decoder_decode_envelope(..) API.
 */
void init_decode_envelope(uint8_t *envelope, size_t envelope_size);

/** @brief Initializes the manifest decoder state with the static memory, defined inside main.c file.
 *         Additionally, it decodes the envelope using the suit_decoder_decode_envelope(..) API and allows for
 *         severable text field in the input envelope.
 */
void init_decode_envelope_with_text(uint8_t *envelope, size_t envelope_size);

/** @brief Initializes the manifest decoder state with the static memory, defined inside main.c file.
 *         Additionally, it decodes the envelope using the suit_decoder_decode_envelope(..) API and allows for
 *         authentication blocks in the input envelope.
 */
void init_decode_signed_envelope(uint8_t *envelope, size_t envelope_size, size_t auth_count);

/** @brief Initializes the manifest decoder state with the static memory, defined inside main.c file.
 *         Additionally, it decodes the envelope using the suit_decoder_decode_envelope(..) API and
 *         decodes the manifest contents using the suit_decoder_decode_manifest(..) API.
 */
void init_decode_manifest(uint8_t *envelope, size_t envelope_size);

/** @brief Initializes the manifest decoder state with the static memory, defined inside main.c file.
 *         Additionally, it decodes the envelope using the suit_decoder_decode_envelope(..) API and
 *         decodes the manifest contents using the suit_decoder_decode_manifest(..) API.
 *         This variant allows for severable text field in the input envelope.
 */
void init_decode_manifest_with_text(uint8_t *envelope, size_t envelope_size);

#endif /* _SUIT_DECODER_TEST_UTILS_H */
