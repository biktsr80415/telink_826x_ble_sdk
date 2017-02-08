
#pragma once

#if (__PROJECT_8266_MASTER_KMA_DONGLE__)
	#include "../8267_master_kma_dongle/app_config.h"
#elif (__PROJECT_8266_HOST_DONGLE__)
	#include "../8266_host_dongle/app_config.h"
#elif (__PROJECT_8267_MASTER_KMA_DONGLE__)
	#include "../8267_master_kma_dongle/app_config.h"
#elif (__PROJECT_8267_MASTER__)
	#include "../8267_master/app_config.h"
#elif (__PROJECT_8267_BTUSB__)
	#include "../8267_btusb/app_config.h"
#elif (__PROJECT_8267_SMP_TEST__)
	#include "../8267_smp_test/app_config.h"
#elif (__PROJECT_8267_HCI__)
	#include "../826x_hci/8267_app_config.h"
#elif (__PROJECT_8269_HCI__)
	#include "../826x_hci/8269_app_config.h"
#elif (__PROJECT_8267_MODULE__)
	#include "../826x_module/8267_app_config.h"
#elif (__PROJECT_826X_SPI_MODULE__)
	#include "../826x_spi_module/8267_app_config.h"
#elif (__PROJECT_8267_SPI_MASTER__)
    #include "../8267_spi_master/app_config.h"
#elif (__PROJECT_8266_MODULE__)
	#include "../826x_module/8266_app_config.h"
#elif (__PROJECT_8261_MODULE__)
	#include "../826x_module/8261_app_config.h"
#elif (__PROJECT_8266_FLYCO_MODULE__)
    #include "../826x_flyco_module/8266_app_config.h"
#elif (__PROJECT_DRIVER_TEST__)
	#include "../8267_driver_test/app_config.h"
#elif (__PROJECT_8267_BLE_REMOTE__ || __PROJECT_8261_BLE_REMOTE__)
	#include "../826x_ble_remote/app_config.h"
#elif (__PROJECT_8267_BLE_RC_DEMO__)
	#include "../8267_ble_rc_demo/app_config.h"
#elif (__PROJECT_8266_OTA_BOOT__ || __PROJECT_8261_OTA_BOOT__)
	#include "../826x_ota_boot/app_config.h"
#elif (__PROJECT_8267_MCU_RC__)
	#include "../8267_mcu_rc/app_config.h"
#elif (__PROJECT_8261_BLE_WEIXIN__)
	#include "../8261_ble_weixin/app_config.h"
#else
	#include "../common/default_config.h"
#endif

