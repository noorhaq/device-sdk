#ifndef SPOTFLOW_CONFIG_H
#define SPOTFLOW_CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void spotflow_config_init();
int spotflow_config_init_session();
void spotflow_config_desired_message(data, data_len);

#ifdef __cplusplus
}
#endif

#endif /* SPOTFLOW_CONFIG_H */