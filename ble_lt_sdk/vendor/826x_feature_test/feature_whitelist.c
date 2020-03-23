/********************************************************************************************************
 * @file     feature_whiltelist.c
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

#if (FEATURE_TEST_MODE == TEST_WHITELIST)


MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);

int  app_whilteList_enable;



void	task_connect (u8 e, u8 *p, int n)
{
	bls_l2cap_requestConnParamUpdate(8, 8, 19, 200);
}

void	task_terminate (u8 e, u8 *p, int n)
{

}

void	task_paring_begin (u8 e, u8 *p, int n)
{

}



u8 paring_result = 0xff;
void	task_paring_end (u8 e, u8 *p, int n)
{
	paring_result = *p;

	if(paring_result == BLE_SUCCESS){

	}
	else{
		// paring_result is fail reason
	}

}

void	task_encryption_done (u8 e, u8 *p, int n)
{

}


void feature_whitelist_test_init(void){


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

	//////////////////////////////////////////
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,

	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER ); 	//smp initialization




///////////////////// USER application initialization ///////////////////
	u8 tbl_advData[] = {
		 0x05, 0x09, 't', 'e', 's', 't',
		 0x02, 0x01, 0x05,
		};
	u8	tbl_scanRsp [] = {
			 0x05, 0x09, 't', 'e', 's', 't',
		};
	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));



	smp_param_save_t  bondInfo;
	u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
	if(bond_number)   //get latest device info
	{
		blc_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )
	}


	ll_whiteList_reset(); 	  //clear whitelist
	ll_resolvingList_reset(); //clear resolving list


	if(bond_number)  //use whitelist to filter master device
	{
		app_whilteList_enable = 1;

		//if master device use RPA(resolvable private address), must add irk to resolving list
		if( IS_RESOLVABLE_PRIVATE_ADDR(bondInfo.peer_addr_type, bondInfo.peer_addr) ){
			//resolvable private address, should add peer irk to resolving list
			ll_resolvingList_add(bondInfo.peer_id_adrType, bondInfo.peer_id_addr, bondInfo.peer_irk, NULL);  //no local IRK
			ll_resolvingList_setAddrResolutionEnable(1);
		}
		else{
			//if not resolvable random address, add peer address to whitelist
			ll_whiteList_add(bondInfo.peer_addr_type, bondInfo.peer_addr);
		}


		bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS, \
							ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
							0,  NULL, BLT_ENABLE_ADV_37, ADV_FP_ALLOW_SCAN_WL_ALLOW_CONN_WL);

	}
	else{

		bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS,
							ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
							0,  NULL, BLT_ENABLE_ADV_37, ADV_FP_NONE);
	}



	bls_ll_setAdvEnable(1);  //adv enable



	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &task_terminate);

	bls_app_registerEventCallback (BLT_EV_FLAG_PAIRING_BEGIN, &task_paring_begin);
	bls_app_registerEventCallback (BLT_EV_FLAG_PAIRING_END, &task_paring_end);
	bls_app_registerEventCallback (BLT_EV_FLAG_ENCRYPTION_CONN_DONE, &task_encryption_done);

}





#endif ///endif of #if (TEST_WHITELIST)
