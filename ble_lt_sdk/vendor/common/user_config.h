
#pragma once

#if (__PROJECT_8261_BLE_REMOTE__ || __PROJECT_8266_BLE_REMOTE__ || __PROJECT_8267_BLE_REMOTE__ || __PROJECT_8269_BLE_REMOTE__)
	#include "../826x_ble_remote/app_config.h"
#elif(__PROJECT_8261_MASTER_KMA_DONGLE__ || __PROJECT_8266_MASTER_KMA_DONGLE__ || __PROJECT_8267_MASTER_KMA_DONGLE__ || __PROJECT_8269_MASTER_KMA_DONGLE__ )
	#include "../826x_master_kma_dongle/app_config.h"
#elif (__PROJECT_8266_OTA_BOOT__ || __PROJECT_8261_OTA_BOOT__)
	#include "../826x_ota_boot/app_config.h"
#else
	#include "../common/default_config.h"
#endif

