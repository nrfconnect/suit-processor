/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <suit_platform_internal.h>


/** Platform component list, populated as a result of successful SUIT manifest validation. */
static struct suit_plat_component components[SUIT_MAX_NUM_COMPONENTS];


/** Resolve pointer to the component structure into component handle.
 */
static inline suit_component_t handle_from_component(const struct suit_plat_component *component)
{
	return (suit_component_t)component;
}

/** Resolve component handle into component structure.
 *
 * @details The component handle is considered as valid if its value
 *	    points to the component array element and the element has
 *	    an implementation assigned.
 */
static struct suit_plat_component * component_from_handle(suit_component_t handle)
{
	const suit_component_t first_component = (suit_component_t)(&components[0]);
	struct suit_plat_component *component = (struct suit_plat_component *)handle;

	if ((handle < (intptr_t)&components[0]) ||
	    (handle > (intptr_t)&components[SUIT_MAX_NUM_COMPONENTS - 1]) ||
	    ((handle - first_component) % (sizeof(struct suit_plat_component)) != 0) ||
	    (component->impl == NULL)) {
			return NULL;
	}

	return component;
}


void suit_plat_reset_components(void)
{
	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		const struct suit_plat_component *component = &components[i];
		const struct suit_component_impl *impl = component->impl;
		suit_component_t handle = handle_from_component(component);

		if ((impl != NULL) && (impl->release != NULL)) {
			impl->release(handle);
		}
	}

	memset(components, 0, sizeof(components));
}

int suit_plat_create_component_handle(struct zcbor_string *component_id,
		struct zcbor_string *key_ids[SUIT_MAX_NUM_SIGNERS], size_t num_key_ids,
		suit_component_t *component_handle)
{
	struct suit_plat_component *component = NULL;

	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		if (components[i].impl == NULL) {
			component = &components[i];
			break;
		}
	}

	if (component == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	*component_handle = (intptr_t)component;
	memset(component, 0, sizeof(struct suit_plat_component));

	/* Select component implementation, based on component ID. */
	component->impl = suit_component_select_impl(component_id, key_ids, num_key_ids, *component_handle);
	if (component->impl == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	/* Store component ID and keys, so the selected implementation can use it in the future. */
	component->component_id = component_id;
	component->key_ids = key_ids;
	component->num_key_ids = num_key_ids;

	/* Allow component implementation to extend the initialization procedure. */
	if (component->impl->init != NULL) {
		return component->impl->init(*component_handle);
	}

	return SUIT_SUCCESS;
}

const struct suit_component_impl * suit_plat_component_impl_get(suit_component_t handle)
{
	const struct suit_plat_component *component = component_from_handle(handle);

	if (component == NULL) {
		return NULL;
	}

	return component->impl;
}

int suit_plat_component_impl_data_set(suit_component_t handle, void *impl_data)
{
	struct suit_plat_component *component = component_from_handle(handle);

	if (component == NULL) {
			return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	component->impl_data = impl_data;
	return SUIT_SUCCESS;
}

int suit_plat_component_impl_data_get(suit_component_t handle, void **impl_data)
{
	const struct suit_plat_component *component = component_from_handle(handle);

	if (component == NULL) {
			return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	*impl_data = component->impl_data;
	return SUIT_SUCCESS;
}

int suit_plat_component_id_get(suit_component_t handle, struct zcbor_string **component_id)
{
	const struct suit_plat_component *component = component_from_handle(handle);

	if (component == NULL) {
			return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	*component_id = component->component_id;
	return SUIT_SUCCESS;
}

size_t suit_plat_component_keys_get(suit_component_t handle, struct zcbor_string ***key_ids)
{
	const struct suit_plat_component *component = component_from_handle(handle);

	if (component == NULL) {
			return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	*key_ids = component->key_ids;
	return component->num_key_ids;
}
