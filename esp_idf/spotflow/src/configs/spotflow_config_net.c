#include "configs/spotflow_config_net.h"
#include "net/spotflow_mqtt.h"
#include "logging/spotflow_log_backend.h"

static volatile bool is_message_pending = false;
static uint8_t pending_message_buffer[SPOTFLOW_CONFIG_RESPONSE_MAX_LENGTH];
static size_t pending_message_length = 0;

int spotflow_config_prepare_pending_message(struct spotflow_config_reported_msg* reported_msg)
{
	int rc = spotflow_config_cbor_encode_reported(reported_msg, pending_message_buffer,
						      sizeof(pending_message_buffer),
						      &pending_message_length);
	is_message_pending = (rc == 0);

	return rc;
}

int spotflow_config_send_pending_message(void) {
    if (!is_message_pending) {
		return 0;
	}
    int rc = spotflow_mqtt_publish_messgae(SPOTFLOW_MQTT_CONFIG_CBOR_D2C_TOPIC, pending_message_buffer, pending_message_length, SPOTFLOW_MQTT_CONFIG_CBOR_D2C_TOPIC_QOS);

    if(rc < 0)
    {
        SPOTFLOW_LOG("Error occured while sending message on topic %s. Error Code %d\n", SPOTFLOW_MQTT_CONFIG_CBOR_D2C_TOPIC, rc);
        return rc;
    }
    SPOTFLOW_LOG("Successfuly sent message on topic %s.\n", SPOTFLOW_MQTT_CONFIG_CBOR_D2C_TOPIC);
    is_message_pending = false;
    return 0;
}