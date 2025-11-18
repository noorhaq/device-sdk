#include <stdbool.h>
#include <stdint.h>

#include "logging/spotflow_log_backend.h"
#include "config/spotflow_config.h"
#include "config/spotflow_config_cbor.h"


void spotflow_config_init()
{
	struct spotflow_config_persisted_settings persisted_settings;

	spotflow_config_persistence_try_init();
	spotflow_config_persistence_try_load(&persisted_settings);

	if (persisted_settings.contains_sent_log_level) {
		spotflow_config_init_sent_log_level(persisted_settings.sent_log_level);
	} else {
		spotflow_config_init_sent_log_level_default();
	}
}