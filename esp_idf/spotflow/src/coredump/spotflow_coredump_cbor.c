#include "spotflow.h"
#include "net/spotflow_mqtt.h"
#include "coredump/spotflow_coredump_cbor.h"
#include "cbor.h"

static const char *TAG = "SPOTFLOW_COREDUMP";

#define MAX_KEY_COUNT 8

#define KEY_MESSAGE_TYPE 0x00
#define KEY_COREDUMP_ID 0x09
#define KEY_CHUNK_ORDINAL 0x0A
#define KEY_CONTENT 0x0B
#define KEY_IS_LAST_CHUNK 0x0C
#define KEY_BUILD_ID 0x0E
#define KEY_OS 0x0F
/*not used in current version*/
#define KEY_OS_VERSION 0x10

#define CORE_DUMP_CHUNK_MESSAGE_TYPE 2
#define ZEPHYR_OS_VALUE 1

/* Should be approximately 47 bytes (including build ID), putting 64 to be safe */
#define COREDUMPS_OVERHEAD 64

uint8_t buffer[CONFIG_SPOTFLOW_COREDUMPS_CHUNK_SIZE + COREDUMPS_OVERHEAD];

int spotflow_cbor_encode_coredump(const uint8_t* coredump_data, size_t coredump_data_len,
				  int chunk_ordinal, uint32_t core_dump_id, bool last_chunk,
				  const uint8_t* build_id_data, size_t build_id_data_len,
				  uint8_t** cbor_data, size_t* cbor_data_len)
{
    CborEncoder array_encoder;

	uint8_t *buf = malloc(CONFIG_SPOTFLOW_COREDUMPS_CHUNK_SIZE + COREDUMPS_OVERHEAD);
    if (!buf) {
        SPOTFLOW_LOG("Failed to allocate CBOR buffer");
        return NULL;
    }
    cbor_encoder_init(&array_encoder, buf, CONFIG_SPOTFLOW_COREDUMPS_CHUNK_SIZE + COREDUMPS_OVERHEAD, 0);
	cbor_encoder_create_map(&array_encoder, &map_encoder, 7); // {
	
	/* start outer map */

	cbor_encode_uint(&map_encoder, KEY_MESSAGE_TYPE);
	cbor_encode_uint(&map_encoder, CORE_DUMP_CHUNK_MESSAGE_TYPE);

	cbor_encode_uint(&map_encoder, KEY_COREDUMP_ID);
	cbor_encode_uint(&map_encoder, core_dump_id);

	cbor_encode_uint(&map_encoder, KEY_CHUNK_ORDINAL);
	cbor_encode_uint(&map_encoder, chunk_ordinal);

	cbor_encode_uint(&map_encoder, KEY_CONTENT);
	cbor_encode_text_string(&map_encoder, coredump_data, coredump_data_len);

	cbor_encode_uint(&map_encoder, KEY_IS_LAST_CHUNK);
	cbor_encode_boolean(&map_encoder, last_chunk);

	if (build_id_data != NULL) {
		cbor_encode_uint(&map_encoder, KEY_BUILD_ID);
		cbor_encode_text_string(&map_encoder, build_id_data, build_id_data_len);
	}

	cbor_encode_uint(&map_encoder, KEY_OS);
	cbor_encode_uint(&map_encoder, ZEPHYR_OS_VALUE);

	/* finish cbor */
    cbor_encoder_close_container(&array_encoder, &map_encoder); // }

	*out_len = cbor_encoder_get_buffer_size(&array_encoder, buf);

	*cbor_data = data;
	return 0;
}