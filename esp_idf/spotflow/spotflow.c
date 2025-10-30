#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "net/spotflow_mqtt.h"
#include "logging/spotflow_log_backend.h"
#include "logging/spotflow_log_queue.h"
#include "coredump/spotflow_coredump.h"
#include "spotflow.h"
#include "buildid/spotflow_build_id.h"
vprintf_like_t original_vprintf = NULL;

/**
 * @brief For Unused Configurations. 
 * 
 */
void Spotflow_Todo(void)
{
    #ifdef SPOTFLOW_GENERATE_BUILD_ID
    SPOTFLOW_LOG("Not implemented yet.\n");
    #endif
}
/**
 * @brief 
 * @details To utilize the esp_log_set_vprintf function to expose the logs
 */
void spotflow_init(void)
{
    // original_vprintf = esp_log_set_vprintf(spotflow_log_backend);

    Spotflow_Todo(); //Checking for unused set Configs.
    if(is_coredump_available())
    {
        display_coredump();
    }
    // mqtt_app_start(); // Calling the mqtt_start from the init function.
    // queue_init(); //Initilize the queue
}