/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_DECODER_H__
#define SUIT_DECODER_H__

#include <suit_processor.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief Initialize the decoder context.
 *
 * @details The decoder output will be incrementally stored inside the provided manifest context structure.
 *
 * @param[in] state     Manifest decoder state to use.
 * @param[in] manifest  Manifest structure to use during decoder operation.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_decoder_init(struct suit_decoder_state *state, struct suit_manifest_state *manifest);

/** @brief Decode the SUIT envelope.
 *
 * @details In this step only the top level of the envelope is decoded.
 *          All nested types (i.e. the manifest) will be decoded in the further decoder steps.
 *
 * @note This function does not authenticate the input data.
 *
 * @param[in] state         Manifest decoder state to use.
 * @param[in] envelope_str  Reference to the SUIT envelope.
 * @param[in] envelope_len  Length of the SUIT envelope.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_decoder_decode_envelope(struct suit_decoder_state *state, const uint8_t *envelope_str, size_t envelope_len);

/** @brief Verify the SUIT manifest digest.
 *
 * @details In this step the manifest payload will be checked against the digest, stored inside the authentication wrapper.
 *          This step does not decode the manifest payload contents.
 *
 * @note This function checks the integrity of the manifest structure.
 *       It does not authenticate the manifest.
 *       The manifest authentication is postponed, since it requires the manifest component ID to be decoded.
 *
 * @param[in] state  Manifest decoder state to use.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_decoder_check_manifest_digest(struct suit_decoder_state *state);

/** @brief Decode the SUIT manifest.
 *
 * @details In this step the manifest structure, fetched from the decoded envelope will be parsed.
 *          The internal ZCBOR structure will be filled with pointers and the input data will be validated
 *          against the CDDL by the ZCBOR library.
 *          Due to the nested character of the sequences, only the first level of nesting is checked by this function.
 *
 * @note This function does not authenticate the manifest.
 *
 * @param[in] state  Manifest decoder state to use.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_decoder_decode_manifest(struct suit_decoder_state *state);

/** @brief Authenticate the SUIT manifest.
 *
 * @details In this step the manifest digest will be checked against all of the signatures present
 *          inside the authentication wrappers.
 *          If the envelope does not contain signatures, the @p suit_plat_authorize_unsigned_manifest
 *          platform API will be used to authorize the manifest.
 *
 * @note Delegation chains are not supported.
 *
 * @param[in] state  Manifest decoder state to use.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_decoder_authenticate_manifest(struct suit_decoder_state *state);

/** @brief Authorize the SUIT manifest components.
 *
 * @details In this step the platform API will be used to authorize the list of components.
 *          The platform should use the manifest component ID to identify the component policy,
 *          associated with the manifest.
 *
 * @note This function checks the dependency indexes.
 *       It does not authorize them nor the dependency prefixes through the platform API.
 *
 * @param[in] state  Manifest decoder state to use.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_decoder_authorize_manifest(struct suit_decoder_state *state);

/** @brief Decode the command sequences.
 *
 * @details In this step the pointers to the command sequences inside the output
 *          manifest structure are populated.
 *          All of the severed sequences are verified through their digests.
 *          If there is a severed sequence without a digest, this function will fail.
 *
 * @note This function does not check the command sequences contents.
 *
 * @param[in] state  Manifest decoder state to use.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_decoder_decode_sequences(struct suit_decoder_state *state);

/** @brief Create manifest components handles through patform API.
 *
 * @details In this step the component handles will be created for all components
 *          inside the manifest.
 *          Apart from that, all dependency manifest components will be marked
 *          inside the output manifest.
 *
 * @note The component handles can be released through the @p suit_manifest_release API
 *       on the output manifest.
 *       Such API cannot be linked with the decoder logic since the manifest structure
 *       lifetime is longer than the decoder context.
 *
 * @param[in] state  Manifest decoder state to use.
 *
 * @returns SUIT_SUCCESS if the operation succeeds, error code otherwise.
 */
int suit_decoder_create_components(struct suit_decoder_state *state);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUIT_DECODER_H__ */
