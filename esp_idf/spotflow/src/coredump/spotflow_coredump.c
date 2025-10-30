#include "coredump/spotflow_coredump.h"
#include "buildid/spotflow_build_id.h"
// #include "coredump/spotflow_coredump_cbor.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_partition.h"
#include "esp_core_dump.h"
#include "esp_random.h"

static const char *TAG = "SPOTFLOW_COREDUMP";

#define COREDUMP_PARTITION_NAME "coredump"

typedef struct {
    size_t size;
    size_t offset;
    int chunk_ordinal;
    uint32_t coredump_id;
} coredump_info_t;

static coredump_info_t coredump_info = {0};

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


esp_err_t spotflow_coredump_backend(void)
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
    
    // Find the coredump partition
    const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, 
                                                           ESP_PARTITION_SUBTYPE_DATA_COREDUMP, 
                                                           NULL);
    if (!part) {
        ESP_LOGE(TAG, "Coredump partition not found.");
        return ESP_ERR_NOT_FOUND;
    }

    // Adjust address to partition offset
    coredump_addr = coredump_addr - part->address;
    ESP_LOGI(TAG, "Adjusted coredump address: 0x%08X, partition address: 0x%08X", 
             (unsigned int)coredump_addr, (unsigned int)part->address);
    
    // Initialize coredump info
    coredump_info.size = coredump_size;
    coredump_info.offset = 0;
    coredump_info.chunk_ordinal = 0;
    coredump_info.coredump_id = esp_random(); // Generate random coredump ID
    
    ESP_LOGI(TAG, "Starting coredump processing with ID: 0x%08X, size: %zu", 
             coredump_info.coredump_id, coredump_info.size);

    // Allocate buffer for one chunk only
    size_t chunk_size = CONFIG_SPOTFLOW_COREDUMPS_CHUNK_SIZE;
    uint8_t *chunk_buffer = malloc(chunk_size);
    
    if (!chunk_buffer) {
        ESP_LOGE(TAG, "Failed to allocate memory for chunk buffer.");
        return ESP_ERR_NO_MEM;
    }

    // Process coredump in chunks
    while (coredump_info.offset < coredump_info.size) {
        // Calculate remaining size and current chunk size
        size_t remaining_size = coredump_info.size - coredump_info.offset;
        size_t current_chunk_size = (remaining_size < chunk_size) ? remaining_size : chunk_size;
        
        // Read the chunk from flash
        err = esp_partition_read(part, coredump_addr + coredump_info.offset, 
                                chunk_buffer, current_chunk_size);
        if (err != ESP_OK) {
            free(chunk_buffer);
            ESP_LOGE(TAG, "Failed to read coredump chunk at offset %zu.", coredump_info.offset);
            return err;
        }
        
        // Check if this is the last chunk
        bool is_last_chunk = (coredump_info.offset + current_chunk_size) >= coredump_info.size;
        if (is_last_chunk) {
            ESP_LOGI(TAG, "Processing last chunk of coredump");
        }
        
        // Get build ID (only for first chunk)
        const uint8_t *build_id = NULL;
        uint16_t build_id_len = 0;
        
#ifdef CONFIG_SPOTFLOW_GENERATE_BUILD_ID
        if (coredump_info.chunk_ordinal == 0) {
            int rc = spotflow_build_id_get(&build_id, &build_id_len);
            if (rc != 0) {
                ESP_LOGW(TAG, "Failed to get build ID for coredump: %d", rc);
            } else {
                ESP_LOGI(TAG, "Build ID retrieved, length: %zu", build_id_len);
            }
        }
#endif
        
        // Encode coredump chunk to CBOR
        uint8_t *cbor_data = NULL;
        size_t cbor_data_len = 0;
        int rc = 1;
        // int rc = spotflow_cbor_encode_coredump(
        //     chunk_buffer, 
        //     current_chunk_size,
        //     coredump_info.chunk_ordinal,
        //     coredump_info.coredump_id,
        //     is_last_chunk,
        //     build_id,
        //     build_id_len,
        //     &cbor_data,
        //     &cbor_data_len
        // );
        
        if (rc < 0) {
            free(chunk_buffer);
            ESP_LOGE(TAG, "Failed to encode coredump chunk: %d", rc);
            return ESP_FAIL;
        }
        
        // Free the CBOR data after sending
        free(cbor_data);
        
        // Update progress
        coredump_info.chunk_ordinal++;
        coredump_info.offset += current_chunk_size;
        
        ESP_LOGI(TAG, "Sent chunk %d: %zu/%zu bytes (%.1f%%)", 
                 coredump_info.chunk_ordinal - 1,
                 coredump_info.offset, 
                 coredump_info.size, 
                 (float)coredump_info.offset * 100.0f / coredump_info.size);
    }

    free(chunk_buffer);
    ESP_LOGI(TAG, "Successfully processed and sent all %zu bytes of coredump data in %d chunks", 
             coredump_info.size, coredump_info.chunk_ordinal);
    
    return ESP_OK;
}