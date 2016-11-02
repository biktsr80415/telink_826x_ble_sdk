
#pragma once

#if (__PROJECT_8266_MASTER_KMA_DONGLE__)
	#include "../8267_master_kma_dongle/app_config.h"
#elif (__PROJECT_8267_MASTER_KMA_DONGLE__)
	#include "../8267_master_kma_dongle/app_config.h"
#elif (__PROJECT_8267_MASTER__)
	#include "../8267_master/app_config.h"
#elif (__PROJECT_8267_BTUSB__)
	#include "../8267_btusb/app_config.h"
#elif (__PROJECT_8267_SMP_TEST__)
	#include "../8267_smp_test/app_config.h"
#elif (__PROJECT_8267_MODULE__)
	#include "../8267_module/app_config.h"
#elif (__PROJECT_8266_MODULE__)
	#include "../8266_module/app_config.h"
#elif (__PROJECT_FLYCO_8266_MODULE__)
    #include "../flyco_8266_module/app_config.h"
#elif (__PROJECT_DRIVER_TEST__)
	#include "../8267_driver_test/app_config.h"
#elif (__PROJECT_8267_BLE_REMOTE__)
	#include "../8267_ble_remote/app_config.h"
#elif (__PROJECT_8267_BLE_RC_DEMO__)
	#include "../8267_ble_rc_demo/app_config.h"
#elif (__PROJECT_8266_OTA_BOOT__)
	#include "../8266_ota_boot/app_config.h"
#else
	#include "../common/default_config.h"
#endif

