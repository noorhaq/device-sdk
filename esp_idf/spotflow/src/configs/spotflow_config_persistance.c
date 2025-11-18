#include <stdio.h>
#include <inttypes.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_err.h"

/**
 * @brief 
 * 
 */
void spotflow_config_persistence_try_init(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    err = nvs_open("spotflow", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        SPOTFLOW_LOG("Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }
}

/**
 * @brief 
 * 
 * @param settings 
 */
void spotflow_config_persistence_try_load(struct spotflow_config_persisted_settings* settings)
{
    *settings = (struct spotflow_config_persisted_settings){ 0 };
    int32_t stored_level = 0;
    esp_err_t err = nvs_get_i32(nvs_handle, "sent_log_level", &stored_level);
    if (err == ESP_OK) {
        settings->sent_log_level = (uint8_t)stored_level;
    } else {
        // Value not found, keep default 0
        SPOTFLOW_LOG("No persisted sent_log_level found, using default 0\n");
    }
}

/**
 * @brief 
 * 
 * @param settings 
 */
void spotflow_config_persistence_try_save(struct spotflow_config_persisted_settings* settings)
{
    if (!settings->contains_sent_log_level) {
        return; // Nothing to save
    }
    
    esp_err_t err = nvs_set_i32(nvs_handle, "sent_log_level", (int32_t)settings->sent_log_level);
    
    if (err != ESP_OK) {
        printf("Failed to persist sent_log_level: %d\n", err);
    } else {
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            SPOTFLOW_LOG("Failed to commit sent_log_level: %d\n", err);
        } else {
            SPOTFLOW_LOG("Sent log level persisted: %d\n", settings->sent_log_level);
        }
    }
}