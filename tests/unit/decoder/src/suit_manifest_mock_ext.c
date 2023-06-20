/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include "suit_manifest_mock_ext.h"


COMPLEX_ARG_Q_DEFINE(__append_dependency_callback_queue);
int __append_dependency_callback(struct suit_manifest_state *manifest, struct zcbor_string *component_id, struct zcbor_string *prefix, int cmock_num_calls)
{
	(void)assert_complex_arg(&__append_dependency_callback_queue, component_id);
	(void)assert_complex_arg(&__append_dependency_callback_queue, prefix);
	return assert_complex_arg(&__append_dependency_callback_queue, NULL);
}

COMPLEX_ARG_Q_DEFINE(__append_component_callback_queue);
int __append_component_callback(struct suit_manifest_state *manifest, struct zcbor_string *component_id, int cmock_num_calls)
{
	(void)assert_complex_arg(&__append_component_callback_queue, component_id);
	return assert_complex_arg(&__append_component_callback_queue, NULL);
}
