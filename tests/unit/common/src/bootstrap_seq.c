/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <bootstrap_seq.h>

size_t bootsrap_seq_hdr_run_sequence(uint8_t * cmd_buf, size_t buf_size, size_t content_size)
{
	size_t hdr_len;
	uint8_t nesting_hdr_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
		0x18, 0x20, /* uint(suit-directive-run-sequence) */
		0x40, /* bytes(0) */
		0x00, /* additional byte reserved for longer bytes types */
	};

	/* The size of bstr is stored on a single byte for up to 23-byte long payloads.
	 * For longer payloads, the 2-byte type signature is used.
	 */
	if (content_size < 24) {
		nesting_hdr_cmd[3] = 0x40 + content_size;
		hdr_len = 4;
	}
	else if (content_size < 256) {
		nesting_hdr_cmd[3] = 0x58;
		nesting_hdr_cmd[4] = content_size;
		hdr_len = 5;
	} else {
		hdr_len = 0;
	}
	if (buf_size < (content_size + hdr_len)) {
		hdr_len = 0;
	}

	/* Construct a nested command. */
	memmove(&cmd_buf[hdr_len], cmd_buf, content_size);
	memcpy(cmd_buf, nesting_hdr_cmd, hdr_len);

	return content_size + hdr_len;
}

size_t bootsrap_seq_hdr_run_sequence_on_all_components(uint8_t * cmd_buf, size_t buf_size, size_t content_size)
{
	size_t hdr_len;
	uint8_t nesting_hdr_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */

		0x0c, /* uint(suit-directive-set-component-index) */
		0xf5, /* true */

		0x18, 0x20, /* uint(suit-directive-run-sequence) */
		0x40, /* bytes(0) */
		0x00, /* additional byte reserved for longer bytes types */
	};

	/* The size of bstr is stored on a single byte for up to 23-byte long payloads.
	 * For longer payloads, the 2-byte type signature is used.
	 */
	if (content_size < 24) {
		nesting_hdr_cmd[5] = 0x40 + content_size;
		hdr_len = 6;
	}
	else if (content_size < 256) {
		nesting_hdr_cmd[5] = 0x58;
		nesting_hdr_cmd[6] = content_size;
		hdr_len = 7;
	} else {
		hdr_len = 0;
	}
	if (buf_size < (content_size + hdr_len)) {
		hdr_len = 0;
	}

	/* Construct a nested command. */
	memmove(&cmd_buf[hdr_len], cmd_buf, content_size);
	memcpy(cmd_buf, nesting_hdr_cmd, hdr_len);

	return content_size + hdr_len;
}

size_t bootsrap_seq_hdr_try_each(uint8_t * cmd_buf, size_t buf_size, size_t content_size)
{
	size_t hdr_len;
	uint8_t nesting_hdr_cmd[] = {
		0x82, /* list (2 elements - 1 command) */
		0x0f, /* uint(suit-directive-try-each) */
		0x82, /* list (2 elements - sequences) */
			0x43, /* bytes (2) */
				0x82, /* list (2 elements - 1 command) */
					0x0e, /* uint(suit-condition-abort) */
					0x00, /* uint(SUIT_Rep_Policy::None) */
			0x40, /* bytes(0) */
			0x00, /* additional byte reserved for longer bytes types */
	};

	/* The size of bstr is stored on a single byte for up to 23-byte long payloads.
	 * For longer payloads, the 2-byte type signature is used.
	 */
	if (content_size < 24) {
		nesting_hdr_cmd[7] = 0x40 + content_size;
		hdr_len = 8;
	}
	else if (content_size < 256) {
		nesting_hdr_cmd[7] = 0x58;
		nesting_hdr_cmd[8] = content_size;
		hdr_len = 9;
	} else {
		hdr_len = 0;
	}
	if (buf_size < (content_size + hdr_len)) {
		hdr_len = 0;
	}

	/* Construct a nested command. */
	memmove(&cmd_buf[hdr_len], cmd_buf, content_size);
	memcpy(cmd_buf, nesting_hdr_cmd, hdr_len);

	return content_size + hdr_len;
}

size_t bootsrap_seq_hdr_try_each_on_all_components(uint8_t * cmd_buf, size_t buf_size, size_t content_size)
{
	size_t hdr_len;
	uint8_t nesting_hdr_cmd[] = {
		0x84, /* list (4 elements - 2 commands) */

		0x0c, /* uint(suit-directive-set-component-index) */
		0xf5, /* true */

		0x0f, /* uint(suit-directive-try-each) */
		0x82, /* list (2 elements - sequences) */
			0x43, /* bytes (2) */
				0x82, /* list (2 elements - 1 command) */
					0x0e, /* uint(suit-condition-abort) */
					0x00, /* uint(SUIT_Rep_Policy::None) */
			0x40, /* bytes(0) */
			0x00, /* additional byte reserved for longer bytes types */
	};

	/* The size of bstr is stored on a single byte for up to 23-byte long payloads.
	 * For longer payloads, the 2-byte type signature is used.
	 */
	if (content_size < 24) {
		nesting_hdr_cmd[9] = 0x40 + content_size;
		hdr_len = 10;
	}
	else if (content_size < 256) {
		nesting_hdr_cmd[9] = 0x58;
		nesting_hdr_cmd[10] = content_size;
		hdr_len = 11;
	} else {
		hdr_len = 0;
	}
	if (buf_size < (content_size + hdr_len)) {
		hdr_len = 0;
	}

	/* Construct a nested command. */
	memmove(&cmd_buf[hdr_len], cmd_buf, content_size);
	memcpy(cmd_buf, nesting_hdr_cmd, hdr_len);

	return content_size + hdr_len;
}
