/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SUIT_CONDITION_H__
#define SUIT_CONDITION_H__

#include "suit_types.h"


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

/** Unconditionally fail. */
int suit_condition_abort(struct suit_processor_state *state,
		struct suit_manifest_params *component_params);


#endif /* SUIT_CONDITION_H__ */
