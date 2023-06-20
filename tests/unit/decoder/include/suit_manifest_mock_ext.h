/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "suit_manifest/cmock_suit_manifest.h"
#include <complex_arg.h>

#ifndef _SUIT_MANIFEST_MOCK_EXT_H
#define _SUIT_MANIFEST_MOCK_EXT_H

/** @file suit_manifest_mock_ext.h
 * @brief Provides additional CMock-like functions that assert on the zcbor string contents.
 *
 * The CMock provides two main prototypes for checking the set of arguments, passed to the mocked APIs:
 *  - __cmock_<API>_ExpectArgsAndReturn(args..): that compare directly the values with the expected arguments
 *  - __cmock_<API>_ExpectWithArrayAndReturn(args.., *args..): that compare pointer arguments against an expected, pointed memory contents
 *
 * In case of pointers to ZCBOR strings, defined as the following structure:
 * struct zcbor_string {
 *     const uint8_t *value;
 *     size_t len;
 * };
 *
 * both of them are insufficient:
 *  - __cmock_<API>_ExpectArgsAndReturn(args..): will assert on the pointer to the structure,
 *    which usually points to the local variable, thus the address is unpredictable.
 *  - __cmock_<API>_ExpectWithArrayAndReturn(args.., *args..): will correctly verify the `len` field,
 *    but instead of checking value, pointed by the `value` field, it will assert on the pointed address.
 *    In most cases, it will be predictable, because the SUIT module passes pointers to the manifest buffer,
 *    but this is not true for checking temporary buffers, for example the Signature1 authentication block.
 *
 * To overcome this limitation, a complex_arg queues are used. They allow to:
 *  - Store the expected argument value with a custom assertion function on the queue through push_complex_arg(..) API
 *  - Store the expected return value on the queue through push_retval_arg API
 *  - Pop the queue head and execute the assertion using the assert_complex_arg(..) API
 *
 * To mix complex and non-complex argument types, the regular __cmock_<API>_ExpectArgsAndReturn(args..)
 * functions are used, but for each complex argument:
 *  - The queue is populated
 *  - The __cmock_<API>_IgnoreArg_<complex_arg_name> API is called
 * In addition to that:
 *  - A callback for running queue-based assertions is registered
 *  - The return value, expected to be returned by callback is added to the queue
 *
 * The implementation for those extensions should:
 * - Define queues for storing complex arguments, one for each mock using COMPLEX_ARG_Q_DEFINE(..) API.
 * - Implement the registered callbacks.
 *
 * Note: The generation of the CMock functions is requested by calling cmock_handle(<module.h> <module_subdir>)
 *       CMake function and the auto-generated functions are placed inside the build/mocks/<module_subdir> directory.
 */


/** @brief assert if length and values of two ZCBOR string differ.
 *
 * @param p1  Pointer to the first ZCBOR string.
 * @param p2  Pointer to the second ZCBOR string.
 */
void assert_zcbor_string(void *p1, void *p2);

#define __cmock_suit_manifest_append_dependency_ExpectComplexArgsAndReturn(manifest, component_id, prefix, cmock_retval) { \
	extern complex_arg_q_t __append_dependency_callback_queue; \
	push_complex_arg(component_id, assert_zcbor_string, __append_dependency_callback_queue); \
	push_complex_arg(prefix, assert_zcbor_string, __append_dependency_callback_queue); \
	push_retval_arg(cmock_retval, __append_dependency_callback_queue); \
	__cmock_suit_manifest_append_dependency_AddCallback(__append_dependency_callback); \
	__cmock_suit_manifest_append_dependency_ExpectAndReturn(manifest, component_id, prefix, cmock_retval); \
	__cmock_suit_manifest_append_dependency_IgnoreArg_component_id(); \
	__cmock_suit_manifest_append_dependency_IgnoreArg_prefix(); \
}
int __append_dependency_callback(struct suit_manifest_state *manifest, struct zcbor_string *component_id, struct zcbor_string *prefix, int cmock_num_calls);

#define __cmock_suit_manifest_append_component_ExpectComplexArgsAndReturn(manifest, component_id, cmock_retval) { \
	extern complex_arg_q_t __append_component_callback_queue; \
	push_complex_arg(component_id, assert_zcbor_string, __append_component_callback_queue); \
	push_retval_arg(cmock_retval, __append_component_callback_queue); \
	__cmock_suit_manifest_append_component_AddCallback(__append_component_callback); \
	__cmock_suit_manifest_append_component_ExpectAndReturn(manifest, component_id, cmock_retval); \
	__cmock_suit_manifest_append_component_IgnoreArg_component_id(); \
}
int __append_component_callback(struct suit_manifest_state *manifest, struct zcbor_string *component_id, int cmock_num_calls);

#endif /* _SUIT_MANIFEST_MOCK_EXT_H */
