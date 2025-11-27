#include "coredump/spotflow_coredump_net.h"
#include "net/spotflow_mqtt.h"
#include "logging/spotflow_log_backend.h"
#include "coredump/spotflow_coredump_queue.h"

queue_msg_t msg;

int spotflow_coredump_send_message(void)
{
	if (spotflow_queue_coredump_read(&msg)) {
		int msg_id =
		    spotflow_mqtt_publish_messgae(SPOTFLOW_MQTT_COREDUMP_TOPIC, msg.ptr, msg.len,
						  SPOTFLOW_MQTT_COREDUMP_QOS // QoS
		    );

		if (msg_id < 0) {
			SPOTFLOW_LOG("Error %d occurred sending MQTT (coredump). Retrying", msg_id);
			return msg_id;
		} else {
			SPOTFLOW_LOG("Coredump message sent successfully. Freeing queue entry.");
			spotflow_queue_coredump_free(&msg);
			return msg_id;
		}
	}
	return 0;
}