#include "suit_types.h"
#include <suit_platform_internal.h>
#include <zcbor_decode.h>


static uint8_t membuf[256];
static size_t read_size;  ///! The size of the current contents in the current slot.
static size_t dry_read_size;  ///! The size of the current contents in the current slot while dry running commands.
static uint8_t * run_address = NULL;


static int read(suit_component_t handle, size_t offset, uint8_t *buf, size_t *len)
{
	size_t read_len = (buf != NULL) ? read_size : dry_read_size;
	*len = MIN(read_len, *len); // To prevent overflow in the comparison below

	if (offset > read_len - *len ) {
		if (offset > read_len) {
			return SUIT_ERR_UNAVAILABLE_PAYLOAD;
		} else {
			*len = read_len - offset;
		}
	}

	if (buf != NULL) {
		memcpy(buf, &membuf[offset], *len);
	}

	return SUIT_SUCCESS;
}

static int write(suit_component_t handle, size_t offset, uint8_t *buf, size_t len)
{
	if ((offset > sizeof(membuf)) ||
	    (len > (sizeof(membuf) - offset))) {
		return SUIT_ERR_CRASH;
	}

	if (buf != NULL) {
		memcpy(&membuf[offset], buf, len);
		if (offset + len > read_size) {
			read_size = offset + len;
		}
	} else {
		if (offset + len > dry_read_size) {
			dry_read_size = offset + len;
		}
	}

	return SUIT_SUCCESS;
}

static int run(suit_component_t handle, struct zcbor_string *run_args)
{
	return SUIT_SUCCESS;
}

static size_t read_address(suit_component_t handle, uint8_t **read_address)
{
	if (read_address != NULL) {
		*read_address = membuf;
	}

	return (read_address == NULL ? dry_read_size : read_size);
}

static struct suit_component_driver driver = {
	.read = read,
	.write = write,
	.run = run,
	.read_address = read_address,
};

static uint8_t vid_uuid_raw[] = {
	0x2B, 0xDC, 0x1C, 0x07, 0xE0, 0xD1, 0x54, 0x84,
	0xBE, 0x50, 0x63, 0x17, 0x4D, 0x5A, 0x74, 0xC3,
};

struct zcbor_string vid_uuid = {
	.value = vid_uuid_raw,
	.len = sizeof(vid_uuid_raw),
};

static uint8_t cid_uuid_raw[] = {
	0x85, 0x20, 0xEA, 0x9C, 0x51, 0x5E, 0x57, 0x79,
	0x8B, 0x5F, 0xBD, 0xAD, 0x67, 0xDE, 0xC7, 0xD9,
};

struct zcbor_string cid_uuid = {
	.value = cid_uuid_raw,
	.len = sizeof(cid_uuid_raw),
};


int suit_component_rambuf_load(struct zcbor_string *component_id,
		struct zcbor_string *key_ids[SUIT_MAX_NUM_SIGNERS], size_t num_key_ids,
		struct suit_component_properties *properties)
{
	ZCBOR_STATE_D(state, 2, component_id->value, component_id->len, SUIT_MAX_NUM_COMPONENT_ID_PARTS);
	struct zcbor_string component_type;
	size_t partition_size;
	uint8_t cpu_id;
	bool res;

	res = zcbor_list_start_decode(state);
	res = res && zcbor_bstr_decode(state, &component_type);
	res = res && zcbor_bstr_start_decode(state, NULL);
	res = res && zcbor_int_decode(state, &cpu_id, 1);
	res = res && zcbor_bstr_end_decode(state);
	res = res && zcbor_bstr_start_decode(state, NULL);
	res = res && zcbor_size_decode(state, (size_t *)&run_address);
	res = res && zcbor_bstr_end_decode(state);
	res = res && zcbor_bstr_start_decode(state, NULL);
	res = res && zcbor_size_decode(state, (size_t *)&partition_size);
	res = res && zcbor_bstr_end_decode(state);
	res = res && zcbor_list_end_decode(state);
	if (!res) {
		return SUIT_ERR_UNSUPPORTED_COMPONENT_ID;
	}

	properties->vid_uuid = &vid_uuid;
	properties->cid_uuid = &cid_uuid;
	properties->did_uuid = NULL; // Not supported
	properties->mode = (suit_comp_readable | suit_comp_writable | suit_comp_runnable); // RWX
	properties->write_size = sizeof(membuf); // Area size
	properties->slot = 0;
	properties->driver = &driver;

	read_size = 0;
	dry_read_size = 0;

	return SUIT_SUCCESS;
}
