/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef SUIT_CONDITION_H__
#define SUIT_CONDITION_H__

#include "suit_processor.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Check a vendor ID based on the configured parameters. */
int suit_condition_vendor_identifier(struct suit_processor_state *state,
		struct suit_manifest_params *component_params);

/** Check a class ID based on the configured parameters. */
int suit_condition_class_identifier(struct suit_processor_state *state,
		struct suit_manifest_params *component_params);

/** Check a device ID based on the configured parameters. */
int suit_condition_device_identifier(struct suit_processor_state *state,
		struct suit_manifest_params *component_params);

/** Check an image digest/size based on the configured parameters. */
int suit_condition_image_match(struct suit_processor_state *state,
		struct suit_manifest_params *component_params);

/** Check a component slot based on the configured parameters. */
int suit_condition_component_slot(struct suit_processor_state *state,
		struct suit_manifest_params *component_params);

/** Check if the component content matches the configured parameter. */
int suit_condition_check_content(struct suit_processor_state *state,
		struct suit_manifest_params *component_params);

/** Unconditionally fail. */
int suit_condition_abort(struct suit_processor_state *state,
		struct suit_manifest_params *component_params);

/** Check a manifest component integrity and signature. */
int suit_condition_dependency_integrity(struct suit_processor_state *state,
		struct suit_manifest_params *component_params);

/** Check if component is a dependency manifest component. */
int suit_condition_is_dependency(struct suit_processor_state *state,
		struct suit_manifest_params *component_params);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SUIT_CONDITION_H__ */
