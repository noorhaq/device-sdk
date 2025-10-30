#ifndef SPOTFLOW_COREDUMP_H
#define SPOTFLOW_COREDUMP_H

#include "stdbool.h"
#include "esp_err.h"

bool is_coredump_available(void);
esp_err_t spotflow_coredump_backend(void);

#endif