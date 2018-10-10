#include "app.h"
#include <stack/ble/ble.h>
#include "tl_common.h"
#include "drivers.h"
#include "app_config.h"
#include "vendor/common/blt_led.h"
#include "application/keyboard/keyboard.h"
#include "vendor/common/tl_audio.h"
#include "vendor/common/blt_soft_timer.h"



#if (FEATURE_TEST_MODE == TEST_WHITELIST)



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






int  app_whilteList_enable;



void	task_connect (u8 e, u8 *p, int n)
{

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
		if(*p == SMP_STANDARD_PAIR){  //first paring

		}
		else if(*p == SMP_FAST_CONNECT){  //auto connect

		}
}




int AA_dbg_suspend;
void  func_suspend_enter (u8 e, u8 *p, int n)
{
	AA_dbg_suspend ++;
}

#define		MY_RF_POWER_INDEX					RF_POWER_P3p01dBm

_attribute_ram_code_ void  func_suspend_exit (u8 e, u8 *p, int n)
{
	rf_set_power_level_index (MY_RF_POWER_INDEX);
}









void feature_whitelist_test_init_normal(void)
{

	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value

	random_generator_init();  //this is must

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

	rf_set_power_level_index (MY_RF_POWER_INDEX);

	////// Controller Initialization  //////////
	blc_ll_initBasicMCU();   //mandatory
	blc_ll_initStandby_module(tbl_mac);				//mandatory



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


_attribute_ram_code_ void feature_whitelist_test_init_deepRetn(void)
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
