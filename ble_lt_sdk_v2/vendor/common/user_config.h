
#pragma once


#if (__PROJECT_MSSOC_MODULE__)
	#include "../826x_module/app_config.h"
#elif (__PROJECT_MSSOC_HID_SAMPLE__)
	#include "../mssoc_hid_sample/app_config.h"
#elif (__PROJECT_MSSOC_FEATURE_TEST__)
	#include "../mssoc_feature_test/app_config.h"
#elif (__PROJECT_MSSOC_BLE_REMOTE__)
	#include "../mssoc_ble_remote/app_config.h"
#elif (__PROJECT_MSSOC_DRIVER_TEST__)
	#include "../mssoc_driver_test/app_config.h"
#else
	#include "../common/default_config.h"
#endif

