#include <stdbool.h>
#include <stdint.h>

#include "logging/spotflow_log_backend.h"
#include "configs/spotflow_config.h"
#include "configs/spotflow_config_cbor.h"
#include "configs/spotflow_config_persistance.h"

void spotflow_config_init()
{
	// struct spotflow_config_persisted_settings persisted_settings;

	// spotflow_config_persistence_try_init();
	// spotflow_config_persistence_try_load(&persisted_settings);

	// if ((persisted_settings.flags & SPOTFLOW_REPORTED_FLAG_MINIMAL_LOG_SEVERITY) != 0) {
	// 	spotflow_config_init_sent_log_level(persisted_settings.sent_log_level);
	// } else {
	// 	spotflow_config_init_sent_log_level_default();
	// }
}

void spotflow_config_desired_message(const uint8_t* payload, int len)
{
	struct spotflow_config_desired_msg desired_msg;
	SPOTFLOW_LOG("Deconding Payload\n");
	int rc = spotflow_config_cbor_decode_desired(payload, len, &desired_msg);
	if (rc < 0) {
		SPOTFLOW_LOG("Failed to decode received desired configuration message: %d\n", rc);
		return;
	}
	else {
		SPOTFLOW_LOG("decode successful\n");
	}

	SPOTFLOW_LOG("Minimal log severity %d, desired config version %d \n\n", desired_msg.minimal_log_severity, desired_msg.minimal_log_severity);
	// struct spotflow_config_reported_msg reported_msg = {
	// 	.contains_acked_desired_config_version = true,
	// 	.acked_desired_config_version = desired_msg.desired_config_version,
	// };

	// struct spotflow_config_persisted_settings settings_to_persist = { 0 };

	// if (desired_msg.flags & SPOTFLOW_REPORTED_FLAG_MINIMAL_LOG_SEVERITY) {
	// 	uint8_t new_sent_log_level =
	// 	    spotflow_cbor_convert_severity_to_log_level(desired_msg.minimal_log_severity);

	// 	spotflow_config_set_sent_log_level(new_sent_log_level);

	// 	add_log_severity_to_reported_msg(&reported_msg);

	// 	settings_to_persist.contains_sent_log_level = true;
	// 	settings_to_persist.sent_log_level = new_sent_log_level;
	// }

	// spotflow_config_persistence_try_save(&settings_to_persist);

	// rc = spotflow_config_prepare_pending_message(&reported_msg);
	// if (rc < 0) {
	// 	LOG_ERR("Failed to prepare reported configuration response message: %d", rc);
	// 	return;
	// }
}