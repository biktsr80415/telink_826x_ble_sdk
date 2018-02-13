
#pragma once

#if (__PROJECT_8266_MASTER_KMA_DONGLE__ || __PROJECT_8267_MASTER_KMA_DONGLE__)
	#include "../8267_master_kma_dongle/app_config.h"
#elif (__PROJECT_8267_MASTER__)
	#include "../8267_master/app_config.h"
#elif (__PROJECT_8267_BTUSB__)
	#include "../8267_btusb/app_config.h"
#elif (__PROJECT_8267_SMP_TEST__)
	#include "../8267_smp_test/app_config.h"
#elif (__PROJECT_8267_HCI__)
	#include "../826x_hci/8267_app_config.h"
#elif (__PROJECT_MSSOC_MODULE__)
	#include "../826x_module/app_config.h"
#elif (__PROJECT_MSSOC_HID_SAMPLE__)
	#include "../mssoc_hid_sample/app_config.h"
#elif (__PROJECT_MSSOC_FEATURE_TEST__)
	#include "../mssoc_feature_test/app_config.h"
#elif (__PROJECT_MSSOC_BLE_REMOTE__)
	#include "../mssoc_ble_remote/app_config.h"
#elif (__PROJECT_8266_MODULE__)
	#include "../826x_module/8266_app_config.h"
#elif (__PROJECT_8261_MODULE__)
	#include "../826x_module/8261_app_config.h"
#elif (__PROJECT_8267_MESH__)
	#include "../826x_mesh/app_config.h"
#elif (__PROJECT_8266_MESH__)
	#include "../826x_mesh/app_config.h"
#elif (__PROJECT_8267_BLE_REMOTE__ || __PROJECT_8261_BLE_REMOTE__)
	#include "../826x_ble_remote/app_config.h"
#elif (__PROJECT_8267_BLE_RC_DEMO__)
	#include "../8267_ble_rc_demo/app_config.h"
#elif (__PROJECT_8266_BLE_REMOTE__)
	#include "../8266_ble_remote/app_config.h"
#elif (__PROJECT_8266_BLE_SNIFFER__ || __PROJECT_8267_BLE_SNIFFER__)
	#include "../826x_sniffer/app_config.h"
#elif (__PROJECT_8267_CERT_SLAVE__)
	#include "../826x_cert_slave/app_config.h"
#else
	#include "../common/default_config.h"
#endif

