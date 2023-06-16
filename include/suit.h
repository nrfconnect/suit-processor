/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_H__
#define SUIT_H__

#include <stdint.h>
#include <suit_types.h>
#include <suit_legacy.h>


/** @brief Initialize SUIT processor module
 *
 * @details This API initializes internal states of all SUIT core modules.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_processor_init(void);

/** @brief Process a sequence of the SUIT manifest.
 *
 * @details This API will decode, authenticate and validate the input manifest data structure.
 *
 * @param[in]  envelope_str  Reference to the input envelope to be parsed.
 * @param[in]  envelope_len  Length of the input envelope.
 * @param[in]  seq_name      Name of the sequence to process.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_process_sequence(uint8_t *envelope_str, size_t envelope_len, enum suit_command_sequence seq_name);

/** Extract metadata from the given envelope.
 *
 * @details This API will decode and (optionally) authenticate the input manifest data structure.
 *
 * @note The output structures will be set to point to the correct places within the input envelope.
 *
 * @param[in]   envelope_str           Reference to the input envelope to be parsed.
 * @param[in]   envelope_len           Length of the input envelope.
 * @param[in]   authenticate           Boolean flag, indicating if the input manifest should be authenticated.
 * @param[out]  manifest_component_id  Pointer to the structure in which the manifest component ID value will be stored.
 * @param[out]  digest                 Pointer to the structure in which the manifest digest value will be stored.
 * @param[out]  seq_num                Pointer to the structure in which the manifest sequence number will be stored.
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_processor_get_manifest_metadata(uint8_t *envelope_str, size_t envelope_len, bool authenticate, struct zcbor_string *manifest_component_id, struct zcbor_string *digest, unsigned int *seq_num);

#endif /* SUIT_H__ */
