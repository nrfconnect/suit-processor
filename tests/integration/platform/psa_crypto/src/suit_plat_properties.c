/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <zcbor_decode.h>
#include <suit_platform_internal.h>


struct suit_component_properties component_properties[SUIT_MAX_NUM_COMPONENTS];

int suit_component_rambuf_load(struct zcbor_string *component_id,
		struct zcbor_string *key_ids[SUIT_MAX_NUM_SIGNERS], size_t num_key_ids,
		struct suit_component_properties *properties);


/** Return the properties of the given component. */
int suit_plat_get_component_properties(suit_component_t component_handle,
		struct suit_component_properties *properties)
{
	for (size_t i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		if ((component_properties[i].component_handle == component_handle) &&
		    (component_properties[i].driver != NULL)) {
			if (properties != NULL) {
				memcpy(properties, &component_properties[i], sizeof(struct suit_component_properties));
			}
			return SUIT_SUCCESS;
		}
	}

	return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
}

/** Reset all loaded component properties and handles assigned to them. */
void suit_plat_reset_components(void)
{
	memset(component_properties, 0, sizeof(component_properties));
}

/** Check that the given component ID exists, is valid, and is authorized.
 *
 *  If so, return a component handle for it.
 *
 *  @param[in]  component_id  The CBOR-encoded component identifier.
 *  @param[in]  key_ids  The keys the current manifest was authenticated
 *			 against. This is to check that the manifest is
 *			 authorized to handle the given component ID.
 *  @param[in]  num_key_ids  The number of members in the @p key_ids list.
 *  @param[out] component_handle  A reference for use with other functions in
 *				  this API, instead of always passing the
 *				  @p parts.
 */
int suit_plat_get_component_handle(struct zcbor_string *component_id,
		struct zcbor_string *key_ids[SUIT_MAX_NUM_SIGNERS], size_t num_key_ids,
		suit_component_t *component_handle)
{
	ZCBOR_STATE_D(state, 1, component_id->value, component_id->len, SUIT_MAX_NUM_COMPONENT_ID_PARTS);
	struct zcbor_string component_type;
	bool res;
	struct suit_component_properties *properties = NULL;
	size_t i;
	int ret;

	res = zcbor_list_start_decode(state);
	res = res && zcbor_bstr_decode(state, &component_type);
	(void)zcbor_list_end_decode(state);
	if ((!res) || (component_type.len != 1)) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	for (i = 0; i < SUIT_MAX_NUM_COMPONENTS; i++) {
		if (component_properties[i].driver == NULL) {
			properties = &component_properties[i];
			break;
		}
	}

	if (properties == NULL) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	*component_handle = (i + 1);
	properties->component_handle = *component_handle;

	/* Try loading the component using supported loaders. */
	switch(component_type.value[0]) {
		case 'M':
			ret = suit_component_rambuf_load(
				component_id,
				key_ids, num_key_ids,
				properties);
			break;

		default:
			ret = SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
			break;
	}

	/* Currently only components with direct read functionality are supported. */
	if (properties->driver->read_address == NULL) {
		ret = SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	return ret;
}
