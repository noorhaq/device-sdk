#include "coredump/spotflow_coredump.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_core_dump.h"

static const char *TAG = "SPOTFLOW_COREDUMP";

#define COREDUMP_PARTITION_NAME "coredump"


bool is_coredump_available(void)
{
    const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, NULL);
    
    if (part == NULL) {
        ESP_LOGI(TAG, "No coredump partition found.");
        return false;
    }

    // Get the actual size of the coredump partition
    size_t partition_size = part->size;
    if (partition_size == 0) {
        ESP_LOGE(TAG, "Coredump partition size is zero.");
        return false;
    }
    return true;
}


esp_err_t display_coredump(void)
{
    size_t coredump_addr = 0;
    size_t coredump_size = 0;

    // Retrieve the coredump image address and size
    esp_err_t err = esp_core_dump_image_get(&coredump_addr, &coredump_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get coredump image address/size: %s", esp_err_to_name(err));
        return err;
    }

    // If the coredump size is 0, there's nothing to display
    if (coredump_size == 0) {
        ESP_LOGE(TAG, "Coredump is empty.");
        return ESP_ERR_INVALID_STATE;
    }

    // Log the coredump information
    ESP_LOGI(TAG, "Coredump address: 0x%08X, size: 0x%08X bytes", (unsigned int)coredump_addr, (int)coredump_size);

    // Read the coredump from flash
    uint8_t *coredump_data = malloc(coredump_size);
    if (!coredump_data) {
        ESP_LOGE(TAG, "Failed to allocate memory for coredump.");
        return ESP_ERR_NO_MEM;
    }

    // Read the coredump data from flash
    const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, NULL);
    if (!part) {
        free(coredump_data);
        ESP_LOGE(TAG, "Coredump partition not found.");
        return ESP_ERR_NOT_FOUND;
    }

    coredump_addr = coredump_addr - part->address;
    ESP_LOGI(TAG, "New address Coredump_Addr 0x%08X and partition Addr 0x%08X", coredump_addr, part->address);
    err = esp_partition_read(part, coredump_addr, coredump_data, coredump_size);
    if (err != ESP_OK) {
        free(coredump_data);
        ESP_LOGE(TAG, "Failed to read coredump data from flash.");
        return err;
    }

    // Display the coredump data (raw hex output)
    ESP_LOGI(TAG, "Coredump Data (Hex Dump):");
    ESP_LOGI(TAG, "%s", coredump_data);
    for (size_t i = 0; i < coredump_size; i++) {
    if (i % 16 == 0) {
        // Print ASCII section for the previous 16 bytes
        if (i > 0) {
            printf(" | ");
            for (size_t j = i - 16; j < i; j++) {
                uint8_t c = coredump_data[j];
                printf("%c", (c >= 32 && c <= 126) ? c : '.');
            }
            printf("\n");
        }
        // Print address offset at the start of each line
        printf("0x%08X: ", (unsigned)(coredump_addr + i));
    }

    // Print hex byte
    printf("%02X ", coredump_data[i]);
}

    free(coredump_data);
    return ESP_OK;
}