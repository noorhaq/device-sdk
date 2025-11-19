#ifndef SPOTFLOW_MQTT_H
#define SPOTFLOW_MQTT_H

#include "stdatomic.h"
#include "mqtt_client.h"

#ifdef __cplusplus
extern "C" {
#endif

extern esp_mqtt_client_handle_t spotflow_client;
extern atomic_bool spotflow_mqtt_connected;

void spotflow_mqtt_app_start(void);
void spotflow_mqtt_publish(void* pvParameters);
int spotflow_mqtt_subscribe(esp_mqtt_client_handle_t client, const char *topic, int qos);
void spotflow_mqtt_handle_data(esp_mqtt_event_handle_t event);
void spotflow_mqtt_on_message(const char *topic, int topic_len,
                              const uint8_t *data, int data_len);
#ifdef __cplusplus
}
#endif

#endif