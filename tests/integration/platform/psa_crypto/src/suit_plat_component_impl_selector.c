#include <zcbor_decode.h>
#include <suit_platform_internal.h>

/** Forward declaration of the RAM-based component implementation constructor. */
struct suit_component_impl * suit_component_rambuf_impl(void);

struct suit_component_impl * suit_component_select_impl(struct zcbor_string *component_id,
		struct zcbor_string *key_ids[SUIT_MAX_NUM_SIGNERS], size_t num_key_ids,
		suit_component_t component_handle)
{
	ZCBOR_STATE_D(state, 1, component_id->value, component_id->len, SUIT_MAX_NUM_COMPONENT_ID_PARTS);
	struct zcbor_string component_type;
	bool res;

	res = zcbor_list_start_decode(state);
	res = res && zcbor_bstr_decode(state, &component_type);
	if ((!res) || (component_type.len != 1)) {
		return NULL;
	}

	/* Try loading the component using supported loaders. */
	switch(component_type.value[0]) {
		case 'M':
			return suit_component_rambuf_impl();

		default:
			break;
	}

	return NULL;
}
