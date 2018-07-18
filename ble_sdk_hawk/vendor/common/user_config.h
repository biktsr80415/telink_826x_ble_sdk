
#pragma once


#if(__PROJECT_5316_BLE_REMOTE__)
	#include "../5316_ble_remote/app_config.h"
#elif(__PROJECT_5317_BLE_REMOTE__)
	#include "vendor/5317_ble_remote/app_config.h"
#elif(__PROJECT_5317_FEATURE_TEST__)
	#include "vendor/531x_feature_test/app_config.h"
#else
	#include "../common/default_config.h"
#endif

