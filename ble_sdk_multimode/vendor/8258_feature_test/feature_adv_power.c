/********************************************************************************************************
 * @file     feature_adv_power.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     May. 12, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#include "app.h"
#include <stack/ble/ble.h>
#include "tl_common.h"
#include "drivers.h"
#include "app_config.h"
#include "vendor/common/blt_led.h"
#include "application/keyboard/keyboard.h"
#include "vendor/common/tl_audio.h"
#include "vendor/common/blt_soft_timer.h"
#include "vendor/common/blt_common.h"

#if (FEATURE_TEST_MODE == TEST_POWER_ADV)




#define RX_FIFO_SIZE	64
#define RX_FIFO_NUM		8

#define TX_FIFO_SIZE	40
#define TX_FIFO_NUM		16






_attribute_data_retention_  u8 		 	blt_rxfifo_b[RX_FIFO_SIZE * RX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_rxfifo = {
												RX_FIFO_SIZE,
												RX_FIFO_NUM,
												0,
												0,
												blt_rxfifo_b,};


_attribute_data_retention_  u8 		 	blt_txfifo_b[TX_FIFO_SIZE * TX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_txfifo = {
												TX_FIFO_SIZE,
												TX_FIFO_NUM,
												0,
												0,
												blt_txfifo_b,};








int AA_dbg_suspend;
void  func_suspend_enter (u8 e, u8 *p, int n)
{
	AA_dbg_suspend ++;
}

#define		MY_RF_POWER_INDEX					RF_POWER_P0p04dBm

_attribute_ram_code_ void  func_suspend_exit (u8 e, u8 *p, int n)
{
	rf_set_power_level_index (MY_RF_POWER_INDEX);
}









void feature_adv_power_test_init_normal(void)
{
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value

	random_generator_init();  //this is must



	u8  tbl_mac [6];  //BLE public address
	blc_initMacAddress(CFG_ADR_MAC, tbl_mac, NULL);


	////// Controller Initialization  //////////
	blc_ll_initBasicMCU();   //mandatory
	blc_ll_initStandby_module(tbl_mac);				//mandatory




	sleep_us(2000000);




	blc_ll_initAdvertising_module(tbl_mac);

/******************************************************************************************************
 * Here are just some ADV power example
 * The actual measured power is affected by several ADV parameters, such as:
 * 1. ADV data length: long ADV data means bigger power
 *
 * 2. ADV type:   non_connectable undirected: ADV power is small, cause only data sending involved, no
 *                                           need receiving any packet from master
 *                connectable ADV: must try to receive scan_req/scan_conn from master after sending adv
 *                                           data, so power is bigger.
 *                                               And if needing send scan_rsp to master's scan_req,
 *                                           power will increase. Here we can use whiteList to disable scan_rsp.
 *											     With connectable ADV, user should test power under a clean
 *											 and shielded environment to avoid receiving scan_req/conn_req
 *
 * 3. ADV power index: We use 0dBm in examples, higher power index will cause poser to increase
 *
 * 4. ADV interval: Bigger adv interval lead to smaller power, cause more timing for suspend/deepSleep retention
 *
 * 5. ADV channel: Power with 3 channel is bigger than power with 1 or 2 channel
 *
 *
 * If you want test ADV power with different ADV parameters from our examples, you should modify these
 *      parameters in code, and re_test by yourself.
 *****************************************************************************************************/

	//set to special ADV channel can avoid master's scan_req to get a very clean power,
	// but remember that special channel ADV packet can not be scanned by BLE master and captured by BLE sniffer
//	blc_ll_setAdvCustomedChannel(33,34,35);

#if 0   // connectable undirected ADV
	//ADV data length: 12 byte
	u8 tbl_advData[12] = {
		 0x08, 0x09, 't', 'e', 's', 't', 'a', 'd', 'v',
		 0x02, 0x01, 0x05,
		};
	u8	tbl_scanRsp [] = {
			 0x08, 0x09, 'T', 'E', 'S', 'T', 'A', 'D', 'V',	//scan name
		};

	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));


	// ADV data length:	12 byte
	// ADV type: 		connectable undirected ADV
	// ADV power index: 0 dBm
	// ADV interval: 	1S
	// ADV channel: 	1 channel
	// test result: 	9 uA
//	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_1S, ADV_INTERVAL_1S, \
									ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_37, ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL);  //no scan, no connect

	// ADV data length:	12 byte
	// ADV type: 		connectable undirected ADV
	// ADV power index: 0 dBm
	// ADV interval: 	1S
	// ADV channel: 	3 channel
	// test result: 	15 uA
	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_1S, ADV_INTERVAL_1S, \
									ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL);  //no scan, no connect

	// ADV data length:	12 byte
	// ADV type: 		connectable undirected ADV
	// ADV power index: 0 dBm
	// ADV interval: 	500 mS
	// ADV channel: 	3 channel
	// test result: 	30 uA
//	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_500MS, ADV_INTERVAL_500MS, \
									ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL);  //no scan, no connect

	// ADV data length:	12 byte
	// ADV type: 		connectable undirected ADV
	// ADV power index: 0 dBm
	// ADV interval: 	30 mS
	// ADV channel: 	3 channel
	// test result: 	430 uA
//	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS, \
									ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL);  //no scan, no connect


#else  // non_connectable undirected adv
	//ADV data length: 16 byte
	u8 tbl_advData[16] = {
		 0x0C, 0x09, 't', 'e', 's', 't', 'a', 'd', 'v', 'T', 'E', 'S', 'T',
		 0x02, 0x01, 0x05,
		};

	u8	tbl_scanRsp [] = {
			 0x08, 0x09, 'T', 'E', 'S', 'T', 'A', 'D', 'V',	//scan name
		};

	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));

	// ADV data length: 16 byte
	// ADV type: non_connectable undirected ADV
	// ADV power index: 0 dBm
	// ADV interval: 1S
	// ADV channel: 3 channel
	// test result: 11 uA
	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_1S, ADV_INTERVAL_1S, \
									ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);


	// ADV data length: 16 byte
	// ADV type: non_connectable undirected ADV
	// ADV power index: 0 dBm
	// ADV interval: 2S
	// ADV channel: 3 channel
	// test result: 8 uA
//	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_1S5, ADV_INTERVAL_1S5, \
									ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);


	// ADV data length: 16 byte
	// ADV type: non_connectable undirected ADV
	// ADV power index: 0 dBm
	// ADV interval: 2S
	// ADV channel: 3 channel
	// test result: 6 uA
//	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_2S, ADV_INTERVAL_2S, \
									ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
#endif

	if(status != BLE_SUCCESS){  //ADV setting err
		write_reg8(0x40000, 0x11);  //debug
		while(1);
	}

	bls_ll_setAdvEnable(1);  //ADV enable


	rf_set_power_level_index (MY_RF_POWER_INDEX);





#if(BLE_PM_ENABLE)
	blc_ll_initPowerManagement_module();

	#if (PM_DEEPSLEEP_RETENTION_ENABLE)
		bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
		blc_pm_setDeepsleepRetentionThreshold(50, 50);
		blc_pm_setDeepsleepRetentionEarlyWakeupTiming(200);
	#else
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	#endif

	//bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &func_suspend_enter);
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &func_suspend_exit);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif


}


_attribute_ram_code_ void feature_adv_power_test_init_deepRetn(void)
{
#if (PM_DEEPSLEEP_RETENTION_ENABLE)
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value

	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (MY_RF_POWER_INDEX);

	blc_ll_recoverDeepRetention();

	irq_enable();

	DBG_CHN0_HIGH;    //debug
#endif
}




#endif

