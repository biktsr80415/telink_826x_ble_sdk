
#pragma once

#if(__PROJECT_8261_MASTER_KMA_DONGLE__ || __PROJECT_8266_MASTER_KMA_DONGLE__ || __PROJECT_8267_MASTER_KMA_DONGLE__ || __PROJECT_8269_MASTER_KMA_DONGLE__ )
	#include "../826x_master_kma_dongle/app_config.h"
#elif (__PROJECT_8261_MASTER__ || __PROJECT_8266_MASTER__ || __PROJECT_8267_MASTER__ || __PROJECT_8269_MASTER__)
	#include "../826x_master/app_config.h"
#elif (__PROJECT_8261_BTUSB__ || __PROJECT_8266_BTUSB__ || __PROJECT_8267_BTUSB__ || __PROJECT_8269_BTUSB__)
	#include "../826x_btusb/app_config.h"
#elif (__PROJECT_8261_HCI__|__PROJECT_8267_HCI__|__PROJECT_8269_HCI__)
	#include "../826x_hci/app_config.h"
#elif (__PROJECT_8261_MODULE__ || __PROJECT_8267_MODULE__ || __PROJECT_8269_MODULE__ || __PROJECT_8266_MODULE__)
	#include "../826x_module/app_config.h"
#elif (__PROJECT_8261_BLE_REMOTE__ || __PROJECT_8267_BLE_REMOTE__ || __PROJECT_8269_BLE_REMOTE__)
	#include "../826x_ble_remote/app_config.h"
#elif (__PROJECT_8261_FEATURE_TEST__ || __PROJECT_8266_FEATURE_TEST__ || __PROJECT_8267_FEATURE_TEST__ || __PROJECT_8269_FEATURE_TEST__)
	#include "../826x_feature_test/app_config.h"
#elif (__PROJECT_8266_HID_SAMPLE__ || __PROJECT_8267_HID_SAMPLE__)
	#include "../826x_hid_sample/app_config.h"
#else
	#include "../common/default_config.h"
#endif

