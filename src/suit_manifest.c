/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <suit_manifest.h>
#include <suit_processor.h>
#include <suit_platform.h>

static struct suit_manifest_params *components;
static size_t components_count;


static int assign_component_index(struct zcbor_string *component_id, size_t *assigned_index)
{
	int ret = SUIT_ERR_OVERFLOW;

	for (size_t i = 0; i < components_count; i++) {
		if ((components[i].ref_count != 0) &&
		    (component_id->len == components[i].component_id.len) &&
		    (memcmp(components[i].component_id.value, component_id->value, component_id->len) == 0)) {
			SUIT_DBG("Found an existing component at index: %d\r\n", i);
			*assigned_index = i;
			components[i].ref_count++;
			return SUIT_SUCCESS;
		}
	}

	for (size_t i = 0; i < components_count; i++) {
		if (components[i].ref_count == 0) {
			SUIT_DBG("Creating a new component at index %d\r\n", i);
			memset(&components[i], 0, sizeof(struct suit_manifest_params));
			components[i].component_id = *component_id;
			*assigned_index = i;

#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
			ret = suit_plat_create_component_handle(component_id, NULL, 0, &components[i].component_handle);
#else /* SUIT_PLATFORM_LEGACY_API_SUPPORT */
			ret = suit_plat_create_component_handle(component_id, &components[i].component_handle);
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */

			if (ret == SUIT_SUCCESS) {
				components[*assigned_index].ref_count++;
			}
			break;
		}
	}

	return ret;
}

static int release_component_index(size_t assigned_index)
{
	int ret = SUIT_SUCCESS;

	if ((assigned_index >= components_count) ||
	    (components[assigned_index].ref_count == 0)) {
		return SUIT_ERR_MISSING_COMPONENT;
	}

	if (components[assigned_index].ref_count == 1) {
#ifndef SUIT_PLATFORM_LEGACY_API_SUPPORT
		ret = suit_plat_release_component_handle(components[assigned_index].component_handle);
		if (ret == SUIT_SUCCESS)
#endif /* !SUIT_PLATFORM_LEGACY_API_SUPPORT */
		{
			components[assigned_index].is_dependency = 0;
		}
	}

	if (ret == SUIT_SUCCESS) {
		components[assigned_index].ref_count--;
	}

	return ret;
}


int suit_manifest_params_init(struct suit_manifest_params *params, size_t count)
{
	if ((params == NULL) || (count < 1)) {
		SUIT_ERR("Invalid input parameters.\r\n");
		return SUIT_ERR_CRASH;
	}

	if (components != NULL) {
		SUIT_ERR("Module already initialized.\r\n");
		return SUIT_ERR_ORDER;
	}

	components = params;
	components_count = count;

	memset(components, 0, sizeof(struct suit_manifest_params) * components_count);

	return SUIT_SUCCESS;
}

int suit_manifest_append_dependency(struct suit_manifest_state *manifest, struct zcbor_string *component_id, struct zcbor_string *prefix)
{
	size_t index;

	if ((components == NULL) || (components_count < 1)) {
		SUIT_ERR("Module not initialized.\r\n");
		return SUIT_ERR_ORDER;
	}

	if ((manifest == NULL) || (component_id == NULL) || (prefix == NULL)) {
		SUIT_ERR("Invalid input parameters.\r\n");
		return SUIT_ERR_CRASH;
	}

	if ((prefix->value != NULL) || (prefix->len != 0)) {
		SUIT_ERR("Unsupported prefix value.\r\n");
		return SUIT_ERR_MANIFEST_VALIDATION;
	}

	if (manifest->components_count >= ZCBOR_ARRAY_SIZE(manifest->component_map)) {
		SUIT_ERR("Manifest reached the maximum number of components.\r\n");
		return SUIT_ERR_MANIFEST_VALIDATION;
	}

	int ret = assign_component_index(component_id, &index);

	if (ret == SUIT_SUCCESS) {
		if (components[index].is_dependency == suit_bool_false) {
			SUIT_ERR("The components is marked as dependency component, but a regular component was requested.\r\n");
			ret = SUIT_ERR_MANIFEST_VALIDATION;
			(void)release_component_index(index);
		} else if (components[index].is_dependency != suit_bool_true) {
			components[index].is_dependency = suit_bool_true;
		}
	}

	if (ret == SUIT_SUCCESS) {
		SUIT_DBG("Assigned index: %d for manifest component %d\r\n", index, manifest->components_count);
		manifest->component_map[manifest->components_count] = index;
		/* Increase the number of valid component indexes / handles */
		manifest->components_count++;
	} else {
		SUIT_DBG("Failed to assign component index (%d)\r\n", ret);
	}

	return ret;
}

int suit_manifest_append_component(struct suit_manifest_state *manifest, struct zcbor_string *component_id)
{
	size_t index;

	if ((components == NULL) || (components_count < 1)) {
		SUIT_ERR("Module not initialized.\r\n");
		return SUIT_ERR_ORDER;
	}

	if ((manifest == NULL) || (component_id == NULL)) {
		SUIT_ERR("Invalid input parameters.\r\n");
		return SUIT_ERR_CRASH;
	}

	if (manifest->components_count >= ZCBOR_ARRAY_SIZE(manifest->component_map)) {
		SUIT_ERR("Manifest reached the maximum number of components.\r\n");
		return SUIT_ERR_MANIFEST_VALIDATION;
	}

	int ret = assign_component_index(component_id, &index);

	if (ret == SUIT_SUCCESS) {
		if (components[index].is_dependency == suit_bool_true) {
			SUIT_ERR("The components is marked as dependency component, but a regular component was requested.\r\n");
			ret = SUIT_ERR_MANIFEST_VALIDATION;
			(void)release_component_index(index);
		} else if (components[index].is_dependency != suit_bool_false) {
			components[index].is_dependency = suit_bool_false;
		}
	}

	if (ret == SUIT_SUCCESS) {
		SUIT_DBG("Assigned index: %d for manifest component %d\r\n", index, manifest->components_count);
		manifest->component_map[manifest->components_count] = index;
		/* Increase the number of valid component indexes / handles */
		manifest->components_count++;
	} else {
		SUIT_DBG("Failed to assign component index (%d)\r\n", ret);
	}

	return ret;
}

int suit_manifest_release(struct suit_manifest_state *manifest)
{
	if ((components == NULL) || (components_count < 1)) {
		SUIT_ERR("Module not initialized.\r\n");
		return SUIT_ERR_ORDER;
	}

	if (manifest == NULL) {
		SUIT_ERR("Invalid input parameters.\r\n");
		return SUIT_ERR_CRASH;
	}

	for (int i = 0; i < manifest->components_count; i++) {
		int ret = release_component_index(manifest->component_map[i]);

		if (ret != SUIT_SUCCESS) {
			return ret;
		}
	}

#ifdef SUIT_PLATFORM_LEGACY_API_SUPPORT
	suit_plat_reset_components();
#endif /* SUIT_PLATFORM_LEGACY_API_SUPPORT */

	memset(manifest, 0, sizeof(*manifest));

	return SUIT_SUCCESS;
}

int suit_manifest_get_component_params(struct suit_manifest_state *manifest, size_t component_idx, struct suit_manifest_params **params)
{
	if ((components == NULL) || (components_count < 1)) {
		SUIT_ERR("Module not initialized.\r\n");
		return SUIT_ERR_ORDER;
	}

	if ((manifest == NULL) || (params == NULL)) {
		SUIT_ERR("Invalid input parameters.\r\n");
		return SUIT_ERR_CRASH;
	}

	if (component_idx >= manifest->components_count) {
		return SUIT_ERR_MISSING_COMPONENT;
	}

	if (manifest->component_map[component_idx] >= components_count) {
		return SUIT_ERR_MISSING_COMPONENT;
	}

	if (components[manifest->component_map[component_idx]].ref_count < 1) {
		return SUIT_ERR_MISSING_COMPONENT;
	}

	*params = &components[manifest->component_map[component_idx]];

	return SUIT_SUCCESS;
}

struct zcbor_string *suit_manifest_get_command_seq(struct suit_manifest_state *manifest, enum suit_command_sequence seq_name)
{
	if (manifest == NULL) {
		return NULL;
	}

	switch (seq_name) {
	case SUIT_SEQ_SHARED:
		if (manifest->shared_sequence_status == AUTHENTICATED) {
			return &manifest->shared_sequence;
		}
		return NULL;
	case SUIT_SEQ_PAYLOAD_FETCH:
		if (manifest->payload_fetch_seq_status == AUTHENTICATED) {
			return &manifest->payload_fetch_seq;
		}
		return NULL;
	case SUIT_SEQ_INSTALL:
		if (manifest->install_seq_status == AUTHENTICATED) {
			return &manifest->install_seq;
		}
		return NULL;
	case SUIT_SEQ_VALIDATE:
		if (manifest->validate_seq_status == AUTHENTICATED) {
			return &manifest->validate_seq;
		}
		return NULL;
	case SUIT_SEQ_LOAD:
		if (manifest->load_seq_status == AUTHENTICATED) {
			return &manifest->load_seq;
		}
		return NULL;
	case SUIT_SEQ_INVOKE:
		if (manifest->invoke_seq_status == AUTHENTICATED) {
			return &manifest->invoke_seq;
		}
		return NULL;
	default:
		return NULL;
	}

	return NULL;
}

int suit_manifest_get_integrated_payload(struct suit_manifest_state *manifest, struct zcbor_string *uri, struct zcbor_string *payload)
{
	if ((uri == NULL) || (manifest == NULL) || (payload == NULL)) {
		SUIT_ERR("Invalid input parameters.\r\n");
		return SUIT_ERR_CRASH;
	}

	for (int i = 0; i < manifest->integrated_payloads_count; i++) {
		if (suit_compare_zcbor_strings(&manifest->integrated_payloads[i].key, uri)) {
			*payload = manifest->integrated_payloads[i].payload;
			return SUIT_SUCCESS;
		}
	}

	return SUIT_ERR_UNAVAILABLE_PAYLOAD;
}
