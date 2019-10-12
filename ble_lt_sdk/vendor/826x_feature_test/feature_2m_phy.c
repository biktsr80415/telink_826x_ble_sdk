/********************************************************************************************************
 * @file     feature_2m_phy.c
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
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/phy/phy.h"


#if(FEATURE_TEST_MODE == TEST_2M_PHY_CONNECTION)


MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);


int device_in_connection_state = 0;
u32 device_connection_tick = 0;

void	task_connect (u8 e, u8 *p, int n)
{
//	bls_l2cap_requestConnParamUpdate(8, 8, 19, 200);
	device_in_connection_state = 1;
	device_connection_tick = clock_time()|0x01;
}

void	task_terminate (u8 e, u8 *p, int n)
{
	device_in_connection_state = 0;
	device_connection_tick = 0;
}
void callback_phy_update_complete_event(u8 e,u8 *p, int n)
{
//	hci_le_phyUpdateCompleteEvt_t *pEvt = (hci_le_phyUpdateCompleteEvt_t *)p;

//	DBG_CHN0_TOGGLE;

	DBG_CHN5_TOGGLE;
}



void feature_2m_phy_conn_init(void){


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
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,

	blc_ll_init2MPhy_feature(); ///mandatory for 2M phy

	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER ); 	//smp initialization
	//HID_service_on_android7p0_init();  //hid device on android 7.0

    ///////////////////// USER application initialization ///////////////////
    u8 tbl_advData[] = {
        0x08,0x09,'t','e','s','t','D','L','E',
    };
    u8 tbl_scanRsp[] = {
        0x08,0x09,'t','e','s','t','D','L','E',
    };
	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));

	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS, \
									 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_37, ADV_FP_NONE);
	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}

	bls_ll_setAdvEnable(1);  //adv enable


	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &task_terminate);
	bls_app_registerEventCallback (BLT_EV_FLAG_PHY_UPDATE, &callback_phy_update_complete_event);

}


u32 phy_update_test_tick = 0;
u32 phy_update_test_seq  = 0;

void feature_2m_phy_conn_mainloop(void){

	if(device_connection_tick && clock_time_exceed(device_connection_tick, 2000000)){
		device_connection_tick = 0;
		phy_update_test_tick = clock_time() | 1;
		phy_update_test_seq = 0;  //reset
	}

	if(phy_update_test_tick && clock_time_exceed(phy_update_test_tick, 7000000)){
		phy_update_test_tick = clock_time() | 1;

		if((phy_update_test_seq++)%2 == 0){
			blc_ll_setPhy(BLS_CONN_HANDLE, PHY_TRX_PREFER, PHY_PREFER_2M, PHY_PREFER_2M);
		}
		else{
			blc_ll_setPhy(BLS_CONN_HANDLE, PHY_TRX_PREFER, PHY_PREFER_1M, PHY_PREFER_1M);
		}

		//phy_update_test_tick = 0;
		//blc_ll_setPhy(BLS_CONN_HANDLE, PHY_TRX_PREFER, PHY_PREFER_2M, PHY_PREFER_2M);
	}

	#if(BLE_PM_ENABLE)
		bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
	#endif
}







#endif ///end of #if(TEST_2M_PHY_CONNECTION)
