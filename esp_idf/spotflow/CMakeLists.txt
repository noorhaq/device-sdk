cmake_minimum_required(VERSION 3.16)
set(SRCS
    "src/net/spotflow_mqtt.c"
    "src/logging/spotflow_log_backend.c"
    "src/logging/spotflow_log_queue.c"
    "src/logging/spotflow_log_cbor.c"
    "src/coredump/spotflow_coredump.c"
 #   "src/buildid/spotflow_build_id.c"
    "spotflow.c"
)

idf_component_register( SRCS ${SRCS}   
                        PRIV_REQUIRES mqtt esp_partition nvs_flash esp_netif app_update espcoredump
                        INCLUDE_DIRS "include"
                        REQUIRES cbor
                        EMBED_TXTFILES x1_root.pem)

if(CONFIG_SPOTFLOW_GENERATE_BUILD_ID)
    get_filename_component(
        spotflow_patch_build_id_script
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/patch_build_id.py
        ABSOLUTE
    )
    get_property(
        PROPERTY_EXTRA_POST_BUILD_COMMANDS
        GLOBAL
        PROPERTY extra_post_build_commands
    )

    # Their existence is checked in the script
    set(POTENTIALLY_PATCHED_FILES
        ${CMAKE_BINARY_DIR}/zephyr/${CONFIG_KERNEL_BIN_NAME}.elf
        ${CMAKE_BINARY_DIR}/zephyr/${CONFIG_KERNEL_BIN_NAME}.hex
        ${CMAKE_BINARY_DIR}/zephyr/${CONFIG_KERNEL_BIN_NAME}.bin
        ${CMAKE_BINARY_DIR}/zephyr/${CONFIG_KERNEL_BIN_NAME}.strip
    )
    if(NOT CONFIG_NATIVE_LIBRARY)
        # The .exe file is just a copy of the .elf file unless targeting native simulator
        # (currently unsupported)
        list(
            APPEND
            POTENTIALLY_PATCHED_FILES
            ${CMAKE_BINARY_DIR}/zephyr/${CONFIG_KERNEL_BIN_NAME}.exe
        )
    endif()

    set_property(
        GLOBAL
        PROPERTY extra_post_build_commands
            COMMAND
                ${PYTHON_EXECUTABLE}
                ${spotflow_patch_build_id_script}
                ${POTENTIALLY_PATCHED_FILES}
            ${PROPERTY_EXTRA_POST_BUILD_COMMANDS}
    )
endif()