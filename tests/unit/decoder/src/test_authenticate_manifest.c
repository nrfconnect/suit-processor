/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "suit_decoder_test_utils.h"
#include "suit_platform_mock_ext.h"

static uint8_t cbor_envelope_signed_no_signature[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x4c, /* bytes(12) */
		0x82, /* array (2 elements) */
			0x41, /* bytes(1) */
			0x09,

			0x48, /* bytes(8): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x83, /* array (3 elements) */
			0x43, /* protected: bytes(3) / serialized map */
				0xa1, /* header_map (1 element) */
				0x01, /* alg_id */ 0x26, /* ES256 */
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t cbor_envelope_signed_empty_signature[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x4d, /* bytes(13) */
		0x82, /* array (2 elements) */
			0x41, /* bytes(1) */
			0x09,

			0x49, /* bytes(9): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x43, /* protected: bytes(3) / serialized map */
				0xa1, /* header_map (1 element) */
				0x01, /* alg_id */ 0x26, /* ES256 */
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x40, /* bytes(0) : signature */

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t cbor_envelope_signed_no_embedded_payload[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x4e, /* bytes(78) */
		0x82, /* array (2 elements) */
			0x41, /* bytes(1) */
			0x09,

			0x58, 0x49, /* bytes(73): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x83, /* array (3 elements) */
			0x43, /* protected: bytes(3) / serialized map */
				0xa1, /* header_map (1 element) */
				0x01, /* alg_id */ 0x26, /* ES256 */
			0xa0, /* unprotected: header_map (0 elements) */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t cbor_envelope_signed_no_unprotected[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x4e, /* bytes(78) */
		0x82, /* array (2 elements) */
			0x41, /* bytes(1) */
			0x09,

			0x58, 0x49, /* bytes(73): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x83, /* array (3 elements) */
			0x43, /* protected: bytes(3) / serialized map */
				0xa1, /* header_map (1 element) */
				0x01, /* alg_id */ 0x26, /* ES256 */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t cbor_envelope_signed_no_alg_id[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x4d, /* bytes(77) */
		0x82, /* array (2 elements) */
			0x41, /* bytes(1) */
			0x09,

			0x58, 0x48, /* bytes(72): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x43, /* protected: bytes(3) / serialized map */
				0xa0, /* empty header_map  */
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t cbor_envelope_signed_empty_header_map[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x4c, /* bytes(76) */
		0x82, /* array (2 elements) */
			0x41, /* bytes(1) */
			0x09,

			0x58, 0x47, /* bytes(71): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x40, /* protected: bytes(0) / serialized map */
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t cbor_envelope_signed_no_protected[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x4b, /* bytes(75) */
		0x82, /* array (2 elements) */
			0x41, /* bytes(1) */
			0x09,

			0x58, 0x46, /* bytes(70): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x83, /* array (3 elements) */
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t cbor_envelope_signed_invalid_alg_id[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x50, /* bytes(80) */
		0x82, /* array (2 elements) */
			0x41, /* bytes(1) */
			0x09,

			0x58, 0x4b, /* bytes(75): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x44, /* protected: bytes(4) / serialized map */
				0xa1, /* header_map (1 element) */
				0x01, /* alg_id */ 0x38, 0x23, /* ES512 */
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t unsigned_envelope[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x27, /* bytes(39) */
		0x81, /* array (1 element) */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t signed_envelope[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x73, /* bytes(115) */
		0x82, /* array (2 elements) */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

			0x58, 0x4a, /* bytes(74): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x43, /* protected: bytes(3) / serialized map */
				0xa1, /* header_map (1 element) */
				0x01, /* alg_id */ 0x26, /* ES256 */
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t signed_envelope_with_key[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x76, /* bytes(118) */
		0x82, /* array (2 elements) */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

			0x58, 0x4d, /* bytes(77): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x46, /* protected: bytes(6) / serialized map */
				0xa2, /* header_map (2 elements) */
				0x01, /* alg_id */ 0x26, /* ES256 */
				0x04, /* key_id */
					0x41, /* bytes(1) */
					0xAA,
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t signed_envelope_with_32bit_key[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x7A, /* bytes(122) */
		0x82, /* array (2 elements) */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

			0x58, 0x51, /* bytes(81): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x4A, /* protected: bytes(10) / serialized map */
				0xa2, /* header_map (2 elements) */
				0x01, /* alg_id */ 0x26, /* ES256 */
				0x04, /* key_id */
					0x45, /* bytes(5) */
					0x1A, /* 32-bit unsigned integer */
					0x7F, 0xFF, 0xFF, 0xE0,
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t signed_envelope_with_32bit_key_sha512[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x9b, /* bytes(155) */
		0x82, /* array (2 elements) */
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

			0x58, 0x51, /* bytes(81): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x4A, /* protected: bytes(10) / serialized map */
				0xa2, /* header_map (2 elements) */
				0x01, /* alg_id */ 0x26, /* ES256 */
				0x04, /* key_id */
					0x45, /* bytes(5) */
					0x1A, /* 32-bit unsigned integer */
					0x7F, 0xFF, 0xFF, 0xE0,
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t signed_envelope_with_2keys[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0xc5, /* bytes(118) */
		0x83, /* array (3 elements) */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

			0x58, 0x4d, /* bytes(77): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x46, /* protected: bytes(6) / serialized map */
				0xa2, /* header_map (2 elements) */
				0x01, /* alg_id */ 0x26, /* ES256 */
				0x04, /* key_id */
					0x41, /* bytes(1) */
					0xAA,
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

			0x58, 0x4d, /* bytes(77): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x46, /* protected: bytes(6) / serialized map */
				0xa2, /* header_map (2 elements) */
				0x01, /* alg_id */ 0x26, /* ES256 */
				0x04, /* key_id */
					0x41, /* bytes(1) */
					0xBB,
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,
	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t signature[] = {
	0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
	0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
	0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
	0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
	0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
	0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
	0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
	0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,
};

static struct zcbor_string exp_signature = {
	.value = signature,
	.len = sizeof(signature),
};

static uint8_t signature1_cbor[] = {
	0x84, /* Sig_structure1: array(4) */
		0x6A, /* context: text(10) */
			'S', 'i', 'g', 'n', 'a', 't', 'u', 'r', 'e', '1',
		0x43, /* protected: bytes(3) / serialized map */
			0xa1, /* header_map (1 element) */
			0x01, /* alg_id */ 0x26, /* ES256 */
		0x40, /* external_aad: bytes(0) */
		0x58, 0x24, /* payload: bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
};

static uint8_t signed_envelope_eddsa[] = {
	0xd8, 0x6b, /* tag(107) : SUIT_Envelope */
	0xa2, /* map (2 elements) */

	0x02, /* suit-authentication-wrapper */
		0x58, 0x73, /* bytes(115) */
		0x82, /* array (2 elements) */
			0x58, 0x24, /* bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,

			0x58, 0x4a, /* bytes(74): SUIT_Authentication_Block */
			0xd2, /* tag(18) : COSE_Sign1 */
			0x84, /* array (4 elements) */
			0x43, /* protected: bytes(3) / serialized map */
				0xa1, /* header_map (1 element) */
				0x01, /* alg_id */ 0x27, /* EdDSA */
			0xa0, /* unprotected: header_map (0 elements) */
			0xf6, /* payload: nil */
			0x58, 0x40, /* bytes(64) : signature */
			0xe3, 0x50, 0x5f, 0x7a, 0xb7, 0x0b, 0xd3, 0xa0,
			0xe0, 0x49, 0x16, 0xf3, 0x7b, 0x0d, 0x72, 0x51,
			0xaa, 0x6f, 0x52, 0xca, 0x12, 0xc7, 0xed, 0xaa,
			0x88, 0x6a, 0x41, 0x29, 0xa2, 0x98, 0xca, 0x6a,
			0x1e, 0xcc, 0x2a, 0x57, 0x95, 0x5c, 0x6b, 0xf4,
			0xcc, 0xb9, 0xf0, 0x1d, 0x68, 0x4d, 0x5d, 0x1c,
			0x47, 0x74, 0xdf, 0xfb, 0xe5, 0x08, 0xa0, 0x34,
			0x43, 0x1f, 0xea, 0xfa, 0x60, 0x84, 0x8a, 0x2c,

	0x03, /* suit-manifest */
	0x48, /* bytes(8) */
		'M', 'a', 'n', 'i', 'f', 'e', 's', 't',
};

static uint8_t signature1_cbor_eddsa[] = {
	0x84, /* Sig_structure1: array(4) */
		0x6A, /* context: text(10) */
			'S', 'i', 'g', 'n', 'a', 't', 'u', 'r', 'e', '1',
		0x43, /* protected: bytes(3) / serialized map */
			0xa1, /* header_map (1 element) */
			0x01, /* alg_id */ 0x27, /* ES256 */
		0x40, /* external_aad: bytes(0) */
		0x58, 0x24, /* payload: bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
};

static struct zcbor_string exp_data_eddsa = {
	.value = signature1_cbor_eddsa,
	.len = sizeof(signature1_cbor_eddsa),
};

static struct zcbor_string exp_data = {
	.value = signature1_cbor,
	.len = sizeof(signature1_cbor),
};

static uint8_t key_id_cbor[] = {
	0xAA,
};

static struct zcbor_string exp_key = {
	.value = key_id_cbor,
	.len = 1,
};

static uint8_t key_id_32bit_cbor[] = {
	0x1A,
	0x7F, 0xFF, 0xFF, 0xE0,
};

static struct zcbor_string exp_32bit_key = {
	.value = key_id_32bit_cbor,
	.len = sizeof(key_id_32bit_cbor),
};

static uint8_t key2_id_cbor[] = {
	0xBB,
};

static struct zcbor_string exp_key2 = {
	.value = key2_id_cbor,
	.len = 1,
};

static uint8_t signature1_wkey_cbor[] = {
	0x84, /* Sig_structure1: array(4) */
		0x6A, /* context: text(10) */
			'S', 'i', 'g', 'n', 'a', 't', 'u', 'r', 'e', '1',
		0x46, /* protected: bytes(6) / serialized map */
			0xa2, /* header_map (2 elements) */
			0x01, /* alg_id */ 0x26, /* ES256 */
			0x04, /* key_id */
				0x41, /* bytes(1) */
				0xAA,
		0x40, /* external_aad: bytes(0) */
		0x58, 0x24, /* payload: bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
};

static struct zcbor_string exp_data_wkey = {
	.value = signature1_wkey_cbor,
	.len = sizeof(signature1_wkey_cbor),
};

static uint8_t signature1_w32bitkey_cbor[] = {
	0x84, /* Sig_structure1: array(4) */
		0x6A, /* context: text(10) */
			'S', 'i', 'g', 'n', 'a', 't', 'u', 'r', 'e', '1',
		0x4A, /* protected: bytes(10) / serialized map */
			0xa2, /* header_map (2 elements) */
			0x01, /* alg_id */ 0x26, /* ES256 */
			0x04, /* key_id */
				0x45, /* bytes(5) */
				0x1A, /* 32-bit unsigned integer */
				0x7F, 0xFF, 0xFF, 0xE0,
		0x40, /* external_aad: bytes(0) */
		0x58, 0x24, /* payload: bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
};

static struct zcbor_string exp_data_w32bitkey = {
	.value = signature1_w32bitkey_cbor,
	.len = sizeof(signature1_w32bitkey_cbor),
};

static uint8_t signature1_w32bitkey_sha512_cbor[] = {
	0x84, /* Sig_structure1: array(4) */
		0x6A, /* context: text(10) */
			'S', 'i', 'g', 'n', 'a', 't', 'u', 'r', 'e', '1',
		0x4A, /* protected: bytes(10) / serialized map */
			0xa2, /* header_map (2 elements) */
			0x01, /* alg_id */ 0x26, /* ES256 */
			0x04, /* key_id */
				0x45, /* bytes(5) */
				0x1A, /* 32-bit unsigned integer */
				0x7F, 0xFF, 0xFF, 0xE0,
		0x40, /* external_aad: bytes(0) */
		0x58, 0x45, /* payload: bytes(69) */
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

static struct zcbor_string exp_data_w32bitkey_sha512 = {
	.value = signature1_w32bitkey_sha512_cbor,
	.len = sizeof(signature1_w32bitkey_sha512_cbor),
};

static uint8_t signature1_wkey2_cbor[] = {
	0x84, /* Sig_structure1: array(4) */
		0x6A, /* context: text(10) */
			'S', 'i', 'g', 'n', 'a', 't', 'u', 'r', 'e', '1',
		0x46, /* protected: bytes(6) / serialized map */
			0xa2, /* header_map (2 elements) */
			0x01, /* alg_id */ 0x26, /* ES256 */
			0x04, /* key_id */
				0x41, /* bytes(1) */
				0xBB,
		0x40, /* external_aad: bytes(0) */
		0x58, 0x24, /* payload: bytes(36) */
			0x82, /* array (2 elements) */
			0x2f, /* suit-digest-algorithm-id: cose-alg-sha-256 */
			0x58, 0x20, /* suit-digest-bytes: bytes(32) */
			0x66, 0x58, 0xea, 0x56, 0x02, 0x62, 0x69, 0x6d,
			0xd1, 0xf1, 0x3b, 0x78, 0x22, 0x39, 0xa0, 0x64,
			0xda, 0x7c, 0x6c, 0x5c, 0xba, 0xf5, 0x2f, 0xde,
			0xd4, 0x28, 0xa6, 0xfc, 0x83, 0xc7, 0xe5, 0xaf,
};

static struct zcbor_string exp_data_wkey2 = {
	.value = signature1_wkey2_cbor,
	.len = sizeof(signature1_wkey2_cbor),
};

void test_authenticate_manifest_invalid_input(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_envelope(unsigned_envelope, sizeof(unsigned_envelope));
	state.step = MANIFEST_DECODED;

	ret = suit_decoder_authenticate_manifest(NULL);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_DECODING, ret, "The manifest authentication did not fail on NULL context");
}

void test_authenticate_manifest_invalid_state(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_envelope(unsigned_envelope, sizeof(unsigned_envelope));

	for (enum suit_decoder_step step = INVALID; step <= LAST_STEP; step++) {
		if (step == MANIFEST_DECODED) {
			continue;
		}

		state.step = step;
		ret = suit_decoder_authenticate_manifest(&state);
		TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_ORDER, ret, "The manifest authentication check did not fail in incorrect state");
	}
}

void test_authenticate_unsigned_manifest(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_envelope(unsigned_envelope, sizeof(unsigned_envelope));
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authorize_unsigned_manifest_ExpectAndReturn(&state.decoded_manifest->manifest_component_id, SUIT_SUCCESS);

	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The unsigned manifest authentication failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_AUTHENTICATED, state.step, "Invalid state transition after manifest authentication");
}

void test_authenticate_unsigned_manifest_platform_fail(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_envelope(unsigned_envelope, sizeof(unsigned_envelope));
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authorize_unsigned_manifest_ExpectAndReturn(&state.decoded_manifest->manifest_component_id, SUIT_ERR_MANIFEST_VALIDATION);

	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_MANIFEST_VALIDATION, ret, "The unsigned manifest authentication did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed unsigned manifest authentication");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after unsigned manifest authentication failure");
}

void test_authenticate_signed_manifest(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_signed_envelope(signed_envelope, sizeof(signed_envelope), 1);
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, NULL, &exp_signature, &exp_data, SUIT_SUCCESS);
	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The signed manifest authentication failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_AUTHENTICATED, state.step, "Invalid state transition after manifest authentication");
}

void test_authenticate_signed_manifest_eddsa(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_signed_envelope(signed_envelope_eddsa, sizeof(signed_envelope_eddsa), 1);
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_EdDSA, NULL, &exp_signature, &exp_data_eddsa, SUIT_SUCCESS);
	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The signed manifest authentication failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_AUTHENTICATED, state.step, "Invalid state transition after manifest authentication");
}

void test_authenticate_signed_manifest_platform_fail(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_signed_envelope(signed_envelope, sizeof(signed_envelope), 1);
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, NULL, &exp_signature, &exp_data, SUIT_ERR_MANIFEST_VALIDATION);
	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_AUTHENTICATION, ret, "The signed manifest authentication did not fail");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authentication");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authentication failure");
}

void test_authenticate_signed_manifest_invalid_input_bytes(void)
{
	int ret = SUIT_SUCCESS;

	struct input_envelope envelopes[] = {
		{
			.envelope = cbor_envelope_signed_no_signature,
			.envelope_size = sizeof(cbor_envelope_signed_no_signature),
			.exp_ret = SUIT_ERR_AUTHENTICATION,
		},
		{
			.envelope = cbor_envelope_signed_empty_signature,
			.envelope_size = sizeof(cbor_envelope_signed_empty_signature),
			.exp_ret = SUIT_ERR_AUTHENTICATION,
		},
		{
			.envelope = cbor_envelope_signed_no_embedded_payload,
			.envelope_size = sizeof(cbor_envelope_signed_no_embedded_payload),
			.exp_ret = SUIT_ERR_AUTHENTICATION,
		},
		{
			.envelope = cbor_envelope_signed_no_unprotected,
			.envelope_size = sizeof(cbor_envelope_signed_no_unprotected),
			.exp_ret = SUIT_ERR_AUTHENTICATION,
		},
		{
			.envelope = cbor_envelope_signed_no_alg_id,
			.envelope_size = sizeof(cbor_envelope_signed_no_alg_id),
			.exp_ret = SUIT_ERR_AUTHENTICATION,
		},
		{
			.envelope = cbor_envelope_signed_empty_header_map,
			.envelope_size = sizeof(cbor_envelope_signed_empty_header_map),
			.exp_ret = SUIT_ERR_AUTHENTICATION,
		},
		{
			.envelope = cbor_envelope_signed_no_protected,
			.envelope_size = sizeof(cbor_envelope_signed_no_protected),
			.exp_ret = SUIT_ERR_AUTHENTICATION,
		},
		{
			.envelope = cbor_envelope_signed_invalid_alg_id,
			.envelope_size = sizeof(cbor_envelope_signed_invalid_alg_id),
			.exp_ret = SUIT_ERR_AUTHENTICATION,
		},
	};

	for (size_t i = 0; i < ZCBOR_ARRAY_SIZE(envelopes); i++) {
		/* Reset state. */
		memset(&state, 0, sizeof(state));
		memset(&manifest, 0, sizeof(manifest));

		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "SUIT decoder has a valid state before the test starts");
		init_decode_signed_envelope(envelopes[i].envelope, envelopes[i].envelope_size, 1);
		state.step = MANIFEST_DECODED;

		ret = suit_decoder_authenticate_manifest(&state);
		TEST_ASSERT_EQUAL_MESSAGE(envelopes[i].exp_ret, ret, "The manifest authentication did not fail");
		TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authentication");
		TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authentication failure");
	}
}

void test_authenticate_signed_manifest_with_key(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_signed_envelope(signed_envelope_with_key, sizeof(signed_envelope_with_key), 1);
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, &exp_key, &exp_signature, &exp_data_wkey, SUIT_SUCCESS);
	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The signed manifest with key authentication failed");
}

void test_authenticate_signed_manifest_with_32bit_key(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_signed_envelope(signed_envelope_with_32bit_key, sizeof(signed_envelope_with_32bit_key), 1);
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, &exp_32bit_key, &exp_signature, &exp_data_w32bitkey, SUIT_SUCCESS);
	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The signed manifest with key authentication failed");
}

void test_authenticate_signed_manifest_with_32bit_key_sha512(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_signed_envelope(signed_envelope_with_32bit_key_sha512, sizeof(signed_envelope_with_32bit_key_sha512), 1);
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, &exp_32bit_key, &exp_signature, &exp_data_w32bitkey_sha512, SUIT_SUCCESS);
	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The signed manifest with key authentication failed");
}

void test_authenticate_signed_manifest_with_2keys(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_signed_envelope(signed_envelope_with_2keys, sizeof(signed_envelope_with_2keys), 2);
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, &exp_key, &exp_signature, &exp_data_wkey, SUIT_SUCCESS);
	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, &exp_key2, &exp_signature, &exp_data_wkey2, SUIT_SUCCESS);

	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_SUCCESS, ret, "The signed manifest with key authentication failed");
	TEST_ASSERT_EQUAL_MESSAGE(MANIFEST_AUTHENTICATED, state.step, "Invalid state transition after manifest authentication");
}

void test_authenticate_signed_manifest_with_2keys_first_platform_fail(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_signed_envelope(signed_envelope_with_2keys, sizeof(signed_envelope_with_2keys), 2);
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, &exp_key, &exp_signature, &exp_data_wkey, SUIT_ERR_MANIFEST_VERIFICATION);
	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, &exp_key2, &exp_signature, &exp_data_wkey2, SUIT_SUCCESS);

	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_AUTHENTICATION, ret, "The signed manifest with key authentication did not fail due to platform error");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authentication");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authentication failure");
}

void test_authenticate_signed_manifest_with_2keys_second_platform_fail(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_signed_envelope(signed_envelope_with_2keys, sizeof(signed_envelope_with_2keys), 2);
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, &exp_key, &exp_signature, &exp_data_wkey, SUIT_SUCCESS);
	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, &exp_key2, &exp_signature, &exp_data_wkey2, SUIT_ERR_MANIFEST_VERIFICATION);

	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_AUTHENTICATION, ret, "The signed manifest with key authentication did not fail due to platform error");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authentication");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authentication failure");
}

void test_authenticate_signed_manifest_with_2keys_platform_fail(void)
{
	int ret = SUIT_SUCCESS;

	init_decode_signed_envelope(signed_envelope_with_2keys, sizeof(signed_envelope_with_2keys), 2);
	state.step = MANIFEST_DECODED;

	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, &exp_key, &exp_signature, &exp_data_wkey, SUIT_ERR_MANIFEST_VERIFICATION);
	__cmock_suit_plat_authenticate_manifest_ExpectComplexArgsAndReturn(&state.decoded_manifest->manifest_component_id, suit_cose_es256, &exp_key2, &exp_signature, &exp_data_wkey2, SUIT_ERR_MANIFEST_VERIFICATION);

	ret = suit_decoder_authenticate_manifest(&state);
	TEST_ASSERT_EQUAL_MESSAGE(SUIT_ERR_AUTHENTICATION, ret, "The signed manifest with key authentication did not fail due to platform error");
	TEST_ASSERT_EQUAL_MESSAGE(INVALID, state.step, "Invalid state transition after failed manifest authentication");
	TEST_ASSERT_NULL_MESSAGE(state.decoded_manifest, "Manifest structure not freed after manifest authentication failure");
}
