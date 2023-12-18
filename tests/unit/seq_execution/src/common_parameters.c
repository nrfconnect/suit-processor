/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <common_parameters.h>
#include <string.h>

struct parameter_value {
	enum parameter_values parameter;
	uint8_t *cmd;
	size_t cmd_size;
};

static uint8_t vendor_identifier_cmd[] = {
	0x01, /* uint(suit-parameter-vendor-identifier) */
	0x50, /* bytes (16) */
	/* RFC4122 uuid5(uuid.NAMESPACE_DNS, 'nordicsemi.com') */
	0x76, 0x17, 0xda, 0xa5, 0x71, 0xfd, 0x5a, 0x85,
	0x8f, 0x94, 0xe2, 0x8d, 0x73, 0x5c, 0xe9, 0xf4,
};

struct zcbor_string exp_vid = {
	.value = &vendor_identifier_cmd[2],
	.len = 16,
};

static uint8_t class_identifier_cmd[] = {
	0x02, /* uint(suit-parameter-class-identifier) */
	0x50, /* bytes (16) */
	/* RFC4122 uuid5(nordic_vid, 'nRF54H20_sample_app') */
	0x08, 0xc1, 0xb5, 0x99, 0x55, 0xe8, 0x5f, 0xbc,
	0x9e, 0x76, 0x7b, 0xc2, 0x9c, 0xe1, 0xb0, 0x4d,
};

struct zcbor_string exp_cid = {
	.value = &class_identifier_cmd[2],
	.len = 16,
};

static uint8_t image_digest_cmd[] = {
	0x03, /* uint(suit-parameter-image-digest) */
	0x58, 0x24, /* bytes(36) */
	0x82, /* array (2 elements) */
		0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
		0x58, 0x20, /* suit-digest-bytes: bytes(32) */
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
};

struct zcbor_string exp_digest = {
	.value = &image_digest_cmd[3],
	.len = 32 + 4,
};

static uint8_t image_digest_cmd_sha512[] = {
	0x03, /* uint(suit-parameter-image-digest) */
	0x58, 0x45, /* bytes(69) */
	0x82, /* array (2 elements) */
		0x38, 0x2B, /* suit-digest-algorithm-id: cose-alg-sha-512 */
		0x58, 0x40, /* suit-digest-bytes: bytes(64) */
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
		0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
		0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
		0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
		0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
};

struct zcbor_string exp_digest_sha512 = {
	.value = &image_digest_cmd_sha512[3],
	.len = 64 + 5,
};

static uint8_t component_slot_cmd[] = {
	0x05, /* uint(suit-parameter-component-slot) */
	0x03, /* uint (3) */
};

uint32_t exp_slot = 3;

static uint8_t soft_failure_cmd[] = {
	0x0d, /* uint(suit-parameter-soft-failure) */
	0xf5, /* True */
};

static uint8_t image_size_cmd[] = {
	0x0e, /* uint(suit-parameter-image-size) */
	0x1a, /* uint (32 bit) */
	0x01, 0x02, 0x03, 0x04,
};

size_t exp_image_size = 0x01020304;

static uint8_t content_cmd[] = {
	0x12, /* uint(suit-parameter-content) */
	0x48, /* bytes (8) */
		0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78, 0x89
};

struct zcbor_string exp_content = {
	.value = &content_cmd[2],
	.len = 8,
};

static uint8_t uri_cmd[] = {
	0x15, /* uint(suit-parameter-uri) */
	0x68, /* text (8 characters) */
		'#', 'a', 'p', 'p', '.', 'b', 'i', 'n',
};

struct zcbor_string exp_uri = {
	.value = "#app.bin",
	.len = sizeof("#app.bin") - 1,
};

static uint8_t source_component_cmd[] = {
	0x16, /* uint(suit-parameter-source-component) */
	0x04, /* uint (4) */
};

size_t exp_source_component = 4;

static uint8_t invoke_args_cmd[] = {
	0x17, /* uint(suit-parameter-invoke-args) */
	0x44, /* bytes (4) */
		0x1e, 0x05, 0x40, 0x00,
};

struct zcbor_string exp_args = {
	.value = &invoke_args_cmd[2],
	.len = 4,
};

static uint8_t device_identifier_cmd[] = {
	0x18, 0x18, /* uint(suit-parameter-device-identifier) */
	0x50, /* bytes (16) */
	/* RFC4122 uuid5(nordic_vid, 'unspecified_class') */
	0xca, 0xd8, 0x52, 0x3a, 0xf8, 0x29, 0x5a, 0x9a,
	0xba, 0x85, 0x2e, 0xa0, 0xb2, 0xf5, 0x77, 0xc9,
};

struct zcbor_string exp_did = {
	.value = &device_identifier_cmd[3],
	.len = 16,
};

static struct parameter_value *find_param(enum parameter_values param)
{
	static struct parameter_value parameter_values[] = {
		{VENDOR_ID, vendor_identifier_cmd, sizeof(vendor_identifier_cmd)},
		{CLASS_ID, class_identifier_cmd, sizeof(class_identifier_cmd)},
		{IMAGE_DIGEST, image_digest_cmd, sizeof(image_digest_cmd)},
		{COMPONENT_SLOT, component_slot_cmd, sizeof(component_slot_cmd)},
		{SOFT_FAILURE, soft_failure_cmd, sizeof(soft_failure_cmd)},
		{IMAGE_SIZE, image_size_cmd, sizeof(image_size_cmd)},
		{CONTENT, content_cmd, sizeof(content_cmd)},
		{URI, uri_cmd, sizeof(uri_cmd)},
		{SOURCE_COMPONENT, source_component_cmd, sizeof(source_component_cmd)},
		{INVOKE_ARGS, invoke_args_cmd, sizeof(invoke_args_cmd)},
		{DEVICE_ID, device_identifier_cmd, sizeof(device_identifier_cmd)},
	};

	for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(parameter_values); i++) {
		if (parameter_values[i].parameter == param) {
			return &parameter_values[i];
		}
	}

	return NULL;
}

size_t bootstrap_parameters(enum parameter_values *parameters, size_t n_parameters, uint8_t *buf, size_t buf_size)
{
	size_t buf_used = 0;

	for (size_t i = 0; i < n_parameters; i++) {
		struct parameter_value *param = find_param(parameters[i]);

		if ((param != NULL) && (buf_used + param->cmd_size <= buf_size)) {
			memcpy(&buf[buf_used], param->cmd, param->cmd_size);
			buf_used += param->cmd_size;
		} else {
			return 0;
		}
	}

	return buf_used;
}
