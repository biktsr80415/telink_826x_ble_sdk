
#pragma once

#if (__PROJECT_8255_BLE_REMOTE__)
	#include "../8255_ble_remote/app_config.h"
#elif (__PROJECT_8255_DRIVER_TEST__)
	#include "../8255_driver_test/app_config.h"
#elif (__PROJECT_8258_BLE_REMOTE__)
	#include "../8258_ble_remote/app_config.h"
#elif (__PROJECT_8258_BLE_SAMPLE__)
	#include "../8258_ble_sample/app_config.h"
#else
	#include "../common/default_config.h"
#endif

