#include "configs/spotflow_config_net.h"


int spotflow_mqtt_subscribe(esp_mqtt_client_handle_t client,
                            const char *topic,
                            int qos)
{
    if (client == NULL || topic == NULL) {
        return ESP_FAIL;
    }

    return esp_mqtt_client_subscribe(client, topic, qos);
}