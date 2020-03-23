/********************************************************************************************************
 * @file     feature_soft_timer.c
 *
 * @brief    for TLSR chips
 *
 * @author	 BLE Group
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

#include "../../proj/tl_common.h"
#include "../../proj_lib/ble/ble_common.h"
#include "../../proj_lib/ble/hci/hci_const.h"
#include "../../proj_lib/ble/ll/ll_scan.h"
#include "../../proj_lib/ble/ll/ll_whitelist.h"
#include "../../proj_lib/ble/ll/ll_pm.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj_lib/pm.h"
#include "../common/blt_soft_timer.h"

#if (FEATURE_TEST_MODE == TEST_USER_BLT_SOFT_TIMER)

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);

void	task_connect (u8 e, u8 *p, int n)
{
	bls_l2cap_requestConnParamUpdate(8, 8, 99, 1000);
}

void	task_terminate (u8 e, u8 *p, int n)
{

}

int gpio_test0(void)
{
	//gpio 0 toggle to see the effect
	DBG_CHN0_TOGGLE;

	return 0;
}


int gpio_test1(void)
{
	//gpio 1 toggle to see the effect
	DBG_CHN1_TOGGLE;

	static u8 flg = 0;
	flg = !flg;
	if(flg){
		return 7000;
	}
	else{
		return 17000;
	}

}

int gpio_test2(void)
{
	//gpio 2 toggle to see the effect
	DBG_CHN2_TOGGLE;

	//timer last for 5 second
	if(clock_time_exceed(0, 5000000)){
		//return -1;
		//blt_soft_timer_delete(&gpio_test2);
	}
	else{

	}

	return 0;
}

int gpio_test3(void)
{
	//gpio 3 toggle to see the effect
	DBG_CHN3_TOGGLE;

	return 0;
}

///////////////////////////////////////

void feature_soft_timer_init(void){


////////////////// BLE stack initialization ////////////////////////////////////
	rf_set_power_level_index (RF_POWER_8dBm);

	u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff)
	{
		memcpy (tbl_mac, pmac, 6);
	}
	else{
		tbl_mac[0] = (u8)rand();
		flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
	}

	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory

	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,

	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER ); 	//smp initialization


///////////////////// USER application initialization ///////////////////
	u8 tbl_advData[] = {
		 0x08, 0x09, 't', 'e', 's', 't', 'T', 'I', 'M',
		 0x02, 0x01, 0x05,
		};
	u8	tbl_scanRsp [] = {
			 0x08, 0x09, 't', 'e', 's', 't', 'T', 'I', 'M',
		};
	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));

	bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS, \
									 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_37, ADV_FP_NONE);

	bls_ll_setAdvEnable(1);  //adv enable

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &task_terminate);


	//////////////// TEST  /////////////////////////
	//common/blt_soft_timer.h   #define		BLT_SOFTWARE_TIMER_ENABLE				1
	blt_soft_timer_init();
	blt_soft_timer_add(&gpio_test0, 23000);
	blt_soft_timer_add(&gpio_test1, 7000);
	blt_soft_timer_add(&gpio_test2, 13000);
	blt_soft_timer_add(&gpio_test3, 27000);
}



#endif ////endif of #if (FEATURE_TEST_MODE == TEST_USER_BLT_SOFT_TIMER)
