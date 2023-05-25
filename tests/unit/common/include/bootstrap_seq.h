/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef BOOTSTRAP_SEQ_H__
#define BOOTSTRAP_SEQ_H__

#include <stdint.h>
#include <string.h>


/** @brief Sequence header bootstraping function prototype.
 *
 * @param  cmd_buf       Buffer to modify.
 * @param  buf_size      Size of the destination buffer.
 * @param  content_size  Size of the contents, already populated inside the buffer.
 *
 * @returns The new content size.
 */
typedef size_t (*bootsrap_seq_hdr_generator_t)(uint8_t * cmd_buf, size_t buf_size, size_t content_size);

/** @brief Bootstrap command sequence header with directive-run-sequence.
 *
 * @note The payload passed as argument is attached as the directive-run-sequence body.
 *
 * @param  cmd_buf       Buffer to modify.
 * @param  buf_size      Size of the destination buffer.
 * @param  content_size  Size of the contents, already populated inside the buffer.
 *
 * @returns The new content size.
 */
size_t bootsrap_seq_hdr_run_sequence(uint8_t * cmd_buf, size_t buf_size, size_t content_size);

/** @brief Bootstrap command sequence header with directive-run-sequence and set-component-index set to true.
 *
 * @note The payload passed as argument is attached as the directive-run-sequence body.
 *
 * @param  cmd_buf       Buffer to modify.
 * @param  buf_size      Size of the destination buffer.
 * @param  content_size  Size of the contents, already populated inside the buffer.
 *
 * @returns The new content size.
 */
size_t bootsrap_seq_hdr_run_sequence_on_all_components(uint8_t * cmd_buf, size_t buf_size, size_t content_size);

/** @brief Bootstrap command sequence header with directive-try-each.
 *
 * @note The payload passed as argument is attached as the directive-run-sequence body.
 *
 * @param  cmd_buf       Buffer to modify.
 * @param  buf_size      Size of the destination buffer.
 * @param  content_size  Size of the contents, already populated inside the buffer.
 *
 * @returns The new content size.
 */
size_t bootsrap_seq_hdr_try_each(uint8_t * cmd_buf, size_t buf_size, size_t content_size);

/** @brief Bootstrap command sequence header with directive-try-each and set-component-index set to true.
 *
 * @note The payload passed as argument is attached as the directive-run-sequence body.
 *
 * @param  cmd_buf       Buffer to modify.
 * @param  buf_size      Size of the destination buffer.
 * @param  content_size  Size of the contents, already populated inside the buffer.
 *
 * @returns The new content size.
 */
size_t bootsrap_seq_hdr_try_each_on_all_components(uint8_t * cmd_buf, size_t buf_size, size_t content_size);

#endif /* BOOTSTRAP_SEQ_H__ */
