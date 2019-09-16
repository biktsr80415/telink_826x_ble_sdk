#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj/drivers/keyboard.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj_lib/ble/phy/phy.h"

#if (FEATURE_TEST_MODE == INTERNAL_TEST)

#define     RC_DEEP_SLEEP_EN					0
#define 	ADV_IDLE_ENTER_DEEP_TIME			60  //60 s
#define 	CONN_IDLE_ENTER_DEEP_TIME			60  //60 s

#define 	MY_DIRECT_ADV_TMIE					2000000


#define     MY_APP_ADV_CHANNEL					BLT_ENABLE_ADV_ALL

#define 	MY_ADV_INTERVAL_MIN					ADV_INTERVAL_30MS
#define 	MY_ADV_INTERVAL_MAX					ADV_INTERVAL_35MS

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);

//////////////////////////////////////////////////////////////////////////////
//	 Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
const u8	tbl_advData[] = {
	 0x05, 0x09, 't', 'H', 'I', 'D',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
	 0x03, 0x19, 0x80, 0x01, 					// 384, Generic Remote Control, Generic category
	 0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,		// incomplete list of service class UUIDs (0x1812, 0x180F)
};

const u8	tbl_scanRsp [] = {
		 0x05, 0x09, 't', 'H', 'I', 'D',
	};
static u8 test_data[128]={
	0x00, 0x00, 0x00, 0x00, 0x00, 0xD8, 0x12, 0x89, 0x40, 0x83, 0x51, 0xBB, 0xB9, 0xA0, 0x1E, 0x39,
	0x70, 0xA2, 0xB3, 0x21, 0x1B, 0x12, 0xCF, 0x21, 0x92, 0x9A, 0x02, 0xF8, 0xB1, 0x9B, 0xB4, 0x28,
	0xB8, 0x06, 0x11, 0xB9, 0x8A, 0x52, 0x81, 0x85, 0x01, 0x0C, 0x10, 0xA9, 0x35, 0x29, 0xA9, 0xA6,
	0x90, 0x1B, 0x9B, 0x81, 0x1B, 0x70, 0x3A, 0xF2, 0x98, 0x82, 0x80, 0x91, 0x93, 0xB3, 0x99, 0xA4,
	0x26, 0x38, 0x63, 0x90, 0xF3, 0x0B, 0xB0, 0x02, 0x28, 0x29, 0xCB, 0xA2, 0x86, 0x9B, 0x1B, 0x51,
	0xB8, 0x16, 0x09, 0xE0, 0x84, 0x21, 0x90, 0x89, 0x89, 0x8F, 0xB1, 0x1C, 0x89, 0x02, 0x19, 0x11,
	0x10, 0x24, 0xD0, 0xAB, 0xF2, 0x3B, 0x02, 0x22, 0x49, 0xE9, 0x01, 0x0B, 0x91, 0x16, 0xB3, 0x93,
	0x1D, 0x00, 0x15, 0x81, 0x1B, 0x22, 0x12, 0x10, 0xBE, 0x0B, 0xBD, 0x13, 0x10, 0xC8, 0x01, 0xEE
};

static u8  send_pkt_enable = 0;
static u8  is_2m_md_test_enter = false;
static u32 phy_update_tick = 0;
static u32 seqNoSend = 0;
static u32 send_pkt_tick = 0;

u32 	   latest_user_event_tick;
int 	   device_in_connection_state = 0;
u32		   advertise_begin_tick;
u8 		   sendTerminate_before_enterDeep = 0;
u8 		   user_task_flg;


void 	app_switch_to_indirect_adv(u8 e, u8 *p, int n)
{

	bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
						ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
						0,  NULL,
						MY_APP_ADV_CHANNEL,
						ADV_FP_NONE);

	bls_ll_setAdvEnable(1);  //must: set adv enable
}


void 	ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{
	device_in_connection_state = 0;
	is_2m_md_test_enter = false;
	send_pkt_tick = 0;
	phy_update_tick = 0;

	if(*p == HCI_ERR_CONN_TIMEOUT){

	}
	else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}
	else if(*p == HCI_ERR_CONN_TERM_MIC_FAILURE){

	}
	else{

	}
	advertise_begin_tick = clock_time();
}

void	task_connect (u8 e, u8 *p, int n)
{
	bls_l2cap_requestConnParamUpdate(8,8,99,400);

	latest_user_event_tick = clock_time();

	device_in_connection_state = 1;

	phy_update_tick = clock_time()|1;
}

void phy_2m_update_complete_cb(u8 e, u8* p, int len)
{
	is_2m_md_test_enter = true;
}
/////////////////////////////////////////////////////////////////////
#if (RC_BTN_ENABLE)
//////////////////// key type ///////////////////////
#define IDLE_KEY	   			0
#define CONSUMER_KEY   	   		1
#define KEYBOARD_KEY   	   		2
#define IR_KEY   	   			3

u8 		key_type;
u8 		user_key_mode;
u8 		key_buf[8] = {0};
int 	key_not_released;


static u16 vk_consumer_map[16] = {
		MKEY_VOL_UP,
		MKEY_VOL_DN,
		MKEY_MUTE,
		MKEY_CHN_UP,

		MKEY_CHN_DN,
		MKEY_POWER,
		MKEY_AC_SEARCH,
		MKEY_RECORD,

		MKEY_PLAY,
		MKEY_PAUSE,
		MKEY_STOP,
		MKEY_FAST_FORWARD,  //can not find fast_backword in <<HID Usage Tables>>

		MKEY_FAST_FORWARD,
		MKEY_AC_HOME,
		MKEY_AC_BACK,
		MKEY_MENU,
};

void key_change_proc(void)
{

	latest_user_event_tick = clock_time();  //record latest key change time

	u8 key0 = kb_event.keycode[0];
	u8 key_value;

	key_not_released = 1;
	if (kb_event.cnt == 2)   //two key press, do  not process
	{

	}
	else if(kb_event.cnt == 1)
	{
		if(key0 == KEY_MODE_SWITCH){
			send_pkt_enable ^= 1;

			seqNoSend = 0;
			*(u32*)test_data = seqNoSend;

			if(send_pkt_enable){
				send_pkt_tick = clock_time()|1;
				test_data[4] = 1;
			}
			else{
				test_data[4] = 0;
				send_pkt_tick = 0;
				bls_att_pushNotifyData (SPP_SERVER_TO_CLIENT_DP_H, (u8*)test_data, 5);
			}
		}
		else{
			key_value = key0;
			if(key_value >= 0xf0 ){
				key_type = CONSUMER_KEY;
				u16 consumer_key = vk_consumer_map[key_value & 0x0f];
				bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
			}
			else
			{
				key_type = KEYBOARD_KEY;
				key_buf[2] = key_value;
				bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);
			}
		}
	}
	else   //kb_event.cnt == 0,  key release
	{
		key_not_released = 0;
		if(key_type == CONSUMER_KEY)
		{
			u16 consumer_key = 0;
			bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
		}
		else if(key_type == KEYBOARD_KEY)
		{
			key_buf[2] = 0;
			bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8); //release
		}
	}

}

#define GPIO_WAKEUP_KEYPROC_CNT				3

void proc_keyboard (u8 e, u8 *p, int n)
{
	static int gpioWakeup_keyProc_cnt = 0;
	static u32 keyScanTick = 0;

	//when key press gpio wakeup suspend, proc keyscan at least GPIO_WAKEUP_KEYPROC_CNT times
	//regardless of 8000 us interval
	if(e == BLT_EV_FLAG_GPIO_EARLY_WAKEUP){
		gpioWakeup_keyProc_cnt = GPIO_WAKEUP_KEYPROC_CNT;
	}
	else if(gpioWakeup_keyProc_cnt){
		gpioWakeup_keyProc_cnt --;
	}


	if(gpioWakeup_keyProc_cnt || clock_time_exceed(keyScanTick, 8000)){
		keyScanTick = clock_time();
	}
	else{
		return;
	}

	kb_event.keycode[0] = 0;
	int det_key = kb_scan_key (0, 1);

	if (det_key){
		key_change_proc();
	}


}

#endif

//extern u32	scan_pin_need;

//_attribute_ram_code_
void blt_pm_proc(void)
{
#if (BLE_PM_ENABLE)

		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);

		user_task_flg = scan_pin_need || key_not_released;

		if(user_task_flg){
			#if (LONG_PRESS_KEY_POWER_OPTIMIZE)
				extern int key_matrix_same_as_last_cnt;
				if( key_matrix_same_as_last_cnt > 5){  //key matrix stable can optize
					bls_pm_setManualLatency(3);
				}
				else{
					bls_pm_setManualLatency(0);  //latency off: 0
				}
			#else
				bls_pm_setManualLatency(0);
			#endif
		}


	#if (RC_DEEP_SLEEP_EN) //deepsleep
		if(sendTerminate_before_enterDeep == 1){ //sending Terminate and wait for ack before enter deepsleep
			if(user_task_flg){  //detect key Press again,  can not enter deep now
				sendTerminate_before_enterDeep = 0;
				bls_ll_setAdvEnable(1);   //enable adv again
			}
		}
		else if(sendTerminate_before_enterDeep == 2){  //Terminate OK
			analog_write(DEEP_ANA_REG0, CONN_DEEP_FLG);
			cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
		}

		//adv 60s, deepsleep
		if( blc_ll_getCurrentState() == BLS_LINK_STATE_ADV && !sendTerminate_before_enterDeep && \
			clock_time_exceed(advertise_begin_tick , ADV_IDLE_ENTER_DEEP_TIME * 1000000))
		{
			cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
		}
		//conn 60s no event(key/voice/led), enter deepsleep
		else if( device_in_connection_state && !user_task_flg && \
				clock_time_exceed(latest_user_event_tick, CONN_IDLE_ENTER_DEEP_TIME * 1000000) )
		{

			bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //push terminate cmd into ble TX buffer
			bls_ll_setAdvEnable(0);   //disable adv
			sendTerminate_before_enterDeep = 1;
		}
	#endif

#endif  //END of  BLE_REMOTE_PM_ENABLE
}


_attribute_ram_code_ void  ble_remote_set_sleep_wakeup (u8 e, u8 *p, int n)
{
	if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 * CLOCK_SYS_CLOCK_1MS){  //suspend time > 30ms.add gpio wakeup
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);  //gpio CORE wakeup suspend
	}
}
int module_onReceiveData(rf_packet_att_write_t *p)
{

}

void test_2m_md_init()
{
	/***********************************************************************************
	 * Keyboard matrix initialization. These section must be before battery_power_check.
	 * Because when low battery,chip will entry deep.if placed after battery_power_check,
	 * it is possible that can not wake up chip.
	 *  *******************************************************************************/

	#if(RC_BTN_ENABLE)
		u32 pin[] = KB_DRIVE_PINS;
		for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
		{
			gpio_set_wakeup(pin[i],1,1);  	   //drive pin core(gpio) high wakeup suspend
			cpu_set_gpio_wakeup (pin[i],1,1);  //drive pin pad high wakeup deepsleep
		}
	#endif


////////////////// BLE stack initialization ////////////////////////////////////
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


 	//// smp initialization ////
	#if (BLE_REMOTE_SECURITY_ENABLE)
		blc_smp_param_setBondingDeviceMaxNumber(4);  	//default is SMP_BONDING_DEVICE_MAX_NUM, can not bigger that this value
														//and this func must call before bls_smp_enableParing
		bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );
	#else
		bls_smp_enableParing (SMP_PARING_DISABLE_TRRIGER );
	#endif

	//HID_service_on_android7p0_init();  //hid device on android 7.0/7.1



	///////////////////// USER application initialization ///////////////////
	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));


	////////////////// config adv packet /////////////////////
	#if (BLE_REMOTE_SECURITY_ENABLE)
		u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();  //get bonded device number
		smp_param_save_t  bondInfo;
		if(bond_number)   //at least 1 bonding device exist
		{
			blc_smp_param_loadByIndex( bond_number - 1, &bondInfo);  //get the latest bonding device (index: bond_number-1 )

		}

		if(bond_number)   //set direct adv
		{
			//set direct adv
			u8 status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
											ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, OWN_ADDRESS_PUBLIC,
											bondInfo.peer_addr_type,  bondInfo.peer_addr,
											MY_APP_ADV_CHANNEL,
											ADV_FP_NONE);
			if(status != BLE_SUCCESS) { write_reg8(0x8000, 0x11); 	while(1); }  //debug: adv setting err

			//it is recommended that direct adv only last for several seconds, then switch to indirect adv
			bls_ll_setAdvDuration(MY_DIRECT_ADV_TMIE, 1);
			bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &app_switch_to_indirect_adv);

		}
		else   //set indirect adv
	#endif
		{
			u8 status = bls_ll_setAdvParam(  MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
											 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
											 0,  NULL,
											 MY_APP_ADV_CHANNEL,
											 ADV_FP_NONE);
			if(status != BLE_SUCCESS) { write_reg8(0x8000, 0x11); 	while(1); }  //debug: adv setting err
		}

	bls_ll_setAdvEnable(1);  //adv enable
	rf_set_power_level_index (RF_POWER_8dBm);

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);

	///////////////////////////
	bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_keyboard);


		///////////////////// Power Management initialization///////////////////
	#if(BLE_PM_ENABLE)
		blc_ll_initPowerManagement_module();
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
		bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
	#else
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
	#endif
#if LL_FEATURE_ENABLE_LE_2M_PHY
	blc_ll_init2MPhy_feature();
	bls_app_registerEventCallback(BLT_EV_FLAG_PHY_UPDATE, &phy_2m_update_complete_cb);
#endif
}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
u32 tick_loop;


void test_2m_md_main_loop (void)
{

	/* UI entry --------------------------------------------------------------*/
	#if(RC_BTN_ENABLE)
		proc_keyboard (0,0, 0);
	#endif


	if(!is_2m_md_test_enter && phy_update_tick && clock_time_exceed(phy_update_tick, 10000000)){
		phy_update_tick = 0;
#if LL_FEATURE_ENABLE_LE_2M_PHY
		blc_ll_setPhy(BLS_CONN_HANDLE, PHY_TRX_PREFER, PHY_PREFER_2M, PHY_PREFER_2M);
#endif
	}

	if(is_2m_md_test_enter && send_pkt_tick && clock_time_exceed(send_pkt_tick, 1000000)){
		if(BLE_SUCCESS == bls_att_pushNotifyData(SPP_SERVER_TO_CLIENT_DP_H, (u8*)test_data, sizeof(test_data))){
			seqNoSend++;
			u32 *p = (u32*)test_data;
			*p = seqNoSend;
		}
	}


	blt_pm_proc();

}


#endif  //end of __PROJECT_826x_HID_SAMPLE
