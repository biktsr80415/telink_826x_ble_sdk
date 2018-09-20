
#pragma once

#if (__PROJECT_8258_DRIVER_TEST__)
	#include "../8258_driver_test/app_config.h"
#elif (__PROJECT_8258_BLE_REMOTE__)
	#include "../8258_ble_remote/app_config.h"
#elif (__PROJECT_8258_BLE_BEACON__)
	#include "../8258_ble_beacon/app_config.h"
#elif (__PROJECT_8258_BLE_SAMPLE__)
	#include "../8258_ble_sample/app_config.h"
#elif (__PROJECT_8258_MODULE__)
	#include "../8258_module/app_config.h"
#elif (__PROJECT_8258_HCI__)
	#include "../8258_hci/app_config.h"
#elif (__PROJECT_8258_FEATURE_TEST__)
	#include "../8258_feature_test/app_config.h"
#elif(__PROJECT_8258_MASTER_KMA_DONGLE__ )
	#include "../8258_master_kma_dongle/app_config.h"
#else
	#include "../common/default_config.h"
#endif

