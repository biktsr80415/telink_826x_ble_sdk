#include "app.h"
#include <stack/ble/ble.h>
#include "tl_common.h"
#include "drivers.h"
#include "../common/blt_led.h"
#include "../common/keyboard.h"

#include "rc_ir.h"


#if (__PROJECT_5316_BLE_REMOTE__ )

#define ADV_IDLE_ENTER_DEEP_TIME	60  //60 s
#define CONN_IDLE_ENTER_DEEP_TIME	60  //60 s

#define MY_DIRECT_ADV_TMIE			2000000

#define MY_APP_ADV_CHANNEL			BLT_ENABLE_ADV_ALL

#define MY_ADV_INTERVAL_MIN			ADV_INTERVAL_30MS
#define MY_ADV_INTERVAL_MAX			ADV_INTERVAL_35MS


MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);


/* ADV Packet, SCAN Response Packet define */
const u8 tbl_advData[] = {
	 0x05, 0x09, 'G', 'h', 'i', 'd',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
	 0x03, 0x19, 0x80, 0x01, 					// 384, Generic Remote Control, Generic category
	 0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,		// incomplete list of service class UUIDs (0x1812, 0x180F)
};

const u8 tbl_scanRsp [] = {
	0x08, 0x09, 'G', 'R', 'e', 'm', 'o', 't', 'e',
};

u32 interval_update_tick = 0;
int device_in_connection_state;

/* LED Management define */
enum{
	LED_POWER_ON = 0,
	LED_AUDIO_ON,	//1
	LED_AUDIO_OFF,	//2
	LED_SHINE_SLOW, //3
	LED_SHINE_FAST, //4
	LED_SHINE_OTA,  //5
};

const led_cfg_t led_cfg[] = {
    {1000,    0,      1,      0x00,	 },    //power-on, 1s on
	{100,	  0 ,	  0xff,	  0x02,  },    //audio on, long on
	{0,	      100 ,   0xff,	  0x02,  },    //audio off, long off
	{500,	  500 ,   2,	  0x04,	 },    //1Hz for 3 seconds
	{250,	  250 ,   4,	  0x04,  },    //2Hz for 3 seconds
	{250,	  250 ,   200,	  0x08,  },    //2Hz for 50 seconds
};

u32	advertise_begin_tick;

u8	ui_mic_enable = 0;
u8 	key_voice_press = 0;

int lowBattDet_enable = 0;
int	lowBatt_alarmFlag = 0;

int ui_mtu_size_exchange_req = 0;

/* Key type Macro */
#define IDLE_KEY	   			0
#define CONSUMER_KEY   	   		1
#define KEYBOARD_KEY   	   		2
#define IR_KEY   	   			3

u8 	key_type;
u8 	user_key_mode;

u8 	key_buf[8] = {0};

int key_not_released;

int ir_not_released;

u32 latest_user_event_tick;

u8 	user_task_flg;
u8 	sendTerminate_before_enterDeep = 0;
u8 	ota_is_working = 0;

/* User Consumer Key Map */
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

#if (STUCK_KEY_PROCESS_ENABLE)
	u32 stuckKey_keyPressTime;
#endif

/*----------------------------------------------------------------------------*/
/*------------- IR  Function                                  ----------------*/
/*----------------------------------------------------------------------------*/
#if (REMOTE_IR_ENABLE)
//ir key
#define TYPE_IR_SEND			1
#define TYPE_IR_RELEASE			2

///////////////////// key mode //////////////////////
#define KEY_MODE_BLE	   		0    //ble key
#define KEY_MODE_IR        		1    //ir  key

static const u8 kb_map_ble[] = KB_MAP_BLE;  //5*6
static const u8 kb_map_ir[]  = KB_MAP_IR;   //5*6

void ir_dispatch(u8 type, u8 syscode ,u8 ircode){
	if(type == TYPE_IR_SEND){
		IR_SendNec(syscode,~(syscode),ircode);
	}
	else if(type == TYPE_IR_RELEASE){
		IR_Stop();
	}
}
#endif


/*----------------------------------------------------------------------------*/
/*------------- OTA  Function                                 ----------------*/
/*----------------------------------------------------------------------------*/
#if (BLE_REMOTE_OTA_ENABLE)
void entry_ota_mode(void)
{
	ota_is_working = 1;
	device_led_setup(led_cfg[LED_SHINE_OTA]);
	bls_ota_setTimeout(15 * 1000 * 1000); //set OTA timeout  15 seconds
}

void LED_show_ota_result(int result)
{
	#if 0
		irq_disable();
		WATCHDOG_DISABLE;

		gpio_set_output_en(GPIO_LED, 1);

		if(result == OTA_SUCCESS){  //OTA success
			gpio_write(GPIO_LED, 1);
			sleep_us(2000000);  //led on for 2 second
			gpio_write(GPIO_LED, 0);
		}
		else{  //OTA fail

		}

		gpio_set_output_en(GPIO_LED, 0);
	#endif
}
#endif




/*----------------------------------------------------------------------------*/
/*------------- CallBack function of BLE                      ----------------*/
/*----------------------------------------------------------------------------*/
void app_switch_to_indirect_adv(u8 e, u8 *p, int n)
{

	bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
						ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
						0,  NULL,
						MY_APP_ADV_CHANNEL,
						ADV_FP_NONE);

	bls_ll_setAdvEnable(1);  //must: set adv enable
}

void ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{
	device_in_connection_state = 0;

	if(*p == HCI_ERR_CONN_TIMEOUT){

	}else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}else if(*p == HCI_ERR_CONN_TERM_MIC_FAILURE){

	}else{

	}

#if (BLE_REMOTE_PM_ENABLE)
	 //user has push terminate pkt to ble TX buffer before deepsleep
	if(sendTerminate_before_enterDeep == 1){
		sendTerminate_before_enterDeep = 2;
	}
#endif


	advertise_begin_tick = clock_time();
}

void task_connect (u8 e, u8 *p, int n)
{
	bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);  //interval=10ms latency=99 timeout=4s

	latest_user_event_tick = clock_time();

	ui_mtu_size_exchange_req = 1;

	device_in_connection_state = 1;//

	interval_update_tick = clock_time() | 1; //none zero
}

/*----------------------------------------------------------------------------*/
/*------------- Key Function                                  ----------------*/
/*----------------------------------------------------------------------------*/
#if(RC_BTN_ENABLE)
void deep_wakeup_proc(void)
{
#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
	//if deepsleep wakeup is wakeup by GPIO(key press), we must quickly scan this
	//press, hold this data to the cache, when connection established OK, send to master
	//deepsleep_wakeup_fast_keyscan
	if(analog_read(DEEP_ANA_REG0) == CONN_DEEP_FLG){
		if(kb_scan_key (KB_NUMLOCK_STATUS_POWERON, 1) && kb_event.cnt){
			deepback_key_state = DEEPBACK_KEY_CACHE;
			key_not_released = 1;
			memcpy(&kb_event_cache,&kb_event,sizeof(kb_event));
		}

		analog_write(DEEP_ANA_REG0, 0);
	}
#endif
}

void deepback_pre_proc(int *det_key)
{
#if (DEEPBACK_FAST_KEYSCAN_ENABLE)
	// to handle deepback key cache
	if(!(*det_key) && deepback_key_state == DEEPBACK_KEY_CACHE
			&& blc_ll_getCurrentState() == BLS_LINK_STATE_CONN
			&& clock_time_exceed(bls_ll_getConnectionCreateTime(), 25000))
	{
		memcpy(&kb_event,&kb_event_cache,sizeof(kb_event));
		*det_key = 1;

		if(key_not_released || kb_event_cache.keycode[0] == VOICE){  //no need manual release
			deepback_key_state = DEEPBACK_KEY_IDLE;
		}
		else{  //need manual release
			deepback_key_tick = clock_time();
			deepback_key_state = DEEPBACK_KEY_WAIT_RELEASE;
		}
	}
#endif
}

void deepback_post_proc(void)
{
#if (DEEPBACK_FAST_KEYSCAN_ENABLE)
	//manual key release
	if(deepback_key_state == DEEPBACK_KEY_WAIT_RELEASE && clock_time_exceed(deepback_key_tick,150000)){
		key_not_released = 0;

		key_buf[2] = 0;
		bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8); //release
		deepback_key_state = DEEPBACK_KEY_IDLE;
	}
#endif
}

void key_change_proc(void)
{
	latest_user_event_tick = clock_time();  //record latest key change time

	if(key_voice_press){  //clear voice key press flg
		key_voice_press = 0;
	}

	u8 key0 = kb_event.keycode[0];
	//u8 key1 = kb_event.keycode[1];
	u8 key_value;

	key_not_released = 1;
	if (kb_event.cnt == 2)   //two key press, do  not process
	{
#if (BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE)  //"enter + back" trigger PhyTest
		//notice that if IR enable, trigger keys must be defined in key map
		if ( (key0 == VK_ENTER && key1 == CR_BACK) || (key0 == CR_BACK && key1 == VK_ENTER))
		{
			extern void app_trigger_phytest_mode(void);
			app_trigger_phytest_mode();
		}
#endif
	}
	else if(kb_event.cnt == 1)
	{
		if(key0 == KEY_MODE_SWITCH)
		{
			user_key_mode = !user_key_mode;
			device_led_setup(led_cfg[LED_SHINE_SLOW + user_key_mode]);
		}

#if (REMOTE_IR_ENABLE)
		else if(user_key_mode == KEY_MODE_BLE)
		{
			key_value = kb_map_ble[key0];
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
		else if(user_key_mode == KEY_MODE_IR)
		{  //IR mode
			key_value = kb_map_ir[key0];
			key_type = IR_KEY;
			if(!ir_not_released){
				ir_dispatch(TYPE_IR_SEND, 0x88, key_value);
				ir_not_released = 1;
			}
		}
		else
		{
			key_type = IDLE_KEY;
		}
#else
		else
		{
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
#endif
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
#if (REMOTE_IR_ENABLE)
		else if(key_type == IR_KEY)
		{
			if(ir_not_released){
				ir_not_released = 0;
				ir_dispatch(TYPE_IR_RELEASE, 0, 0);  //release
			}
		}
#endif
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

#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
	if(deepback_key_state != DEEPBACK_KEY_IDLE){
		deepback_pre_proc(&det_key);
	}
#endif

	if (det_key){
		key_change_proc();
	}
	

#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
	if(deepback_key_state != DEEPBACK_KEY_IDLE){
		deepback_post_proc();
	}
#endif
}
#endif

extern u32	scan_pin_need;
//_attribute_ram_code_
void blt_pm_proc(void)
{
#if(BLE_REMOTE_PM_ENABLE)
	if(ui_mic_enable)
	{
		bls_pm_setSuspendMask (MCU_STALL);
	}
#if(REMOTE_IR_ENABLE)
	#if(MCU_CORE_TYPE == MCU_CORE_5316)
	if(IR_GetIrState() == IR_STATE_SENDING){
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
	}
	#else
	else if( ir_send_ctrl.is_sending || ir_send_ctrl.repeat_timer_enable){
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
	}
	#endif
#endif

#if (BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE)
	else if( blc_phy_isPhyTestEnable() )
	{
		bls_pm_setSuspendMask(SUSPEND_DISABLE);  //phy test can not enter suspend
	}
#endif

	else
	{
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);

		user_task_flg = ota_is_working || scan_pin_need || key_not_released || DEVICE_LED_BUSY;

		if(user_task_flg){
			#if (LONG_PRESS_KEY_POWER_OPTIMIZE)
				extern int key_matrix_same_as_last_cnt;
				if(!ota_is_working && key_matrix_same_as_last_cnt > 5){  //key matrix stable can optize
					bls_pm_setManualLatency(3);
					DBG_CHN1_TOGGLE;
				}
				else{
					bls_pm_setManualLatency(0);  //latency off: 0
				}
			#else
				bls_pm_setManualLatency(0);
			#endif
		}


	#if 1 //deepsleep
		if(sendTerminate_before_enterDeep == 1){ //sending Terminate and wait for ack before enter deepsleep
			if(user_task_flg){  //detect key Press again,  can not enter deep now
				sendTerminate_before_enterDeep = 0;
				bls_ll_setAdvEnable(1);   //enable adv again
			}
		}
		else if(sendTerminate_before_enterDeep == 2){  //Terminate OK
			analog_write(DEEP_ANA_REG0, CONN_DEEP_FLG);

			#if (REMOTE_IR_ENABLE)
				analog_write(DEEP_ANA_REG1, user_key_mode);
			#endif
			cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //deepsleep
		}

		//adv 60s, deepsleep
		if( blc_ll_getCurrentState() == BLS_LINK_STATE_ADV && !sendTerminate_before_enterDeep && \
			clock_time_exceed(advertise_begin_tick , ADV_IDLE_ENTER_DEEP_TIME * 1000000))
		{

			#if (REMOTE_IR_ENABLE)
				analog_write(DEEP_ANA_REG1, user_key_mode);
			#endif
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
	}
#endif  //END of  BLE_REMOTE_PM_ENABLE
}


void  ble_remote_set_sleep_wakeup(u8 e, u8 *p, int n)
{                                                                                                                //3995*16     sys_tick_per_us
	if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 *CLOCK_16M_SYS_TIMER_CLK_1MS ){  //suspend time > 30ms.add gpio wakeup
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);  //gpio CORE wakeup suspend
	}
}


/* 5316 Driver test. ---------------------------------------------------------*/
#define PM_TEST  	            PM_GPIO_WAKEUP_SUSPEND

#define PM_NONE                 0xff
#define PM_GPIO_WAKEUP_SUSPEND  0
#define PM_PAD_WAKEUP_SUSPEND   1
#define PM_32K_WAKEUP_SUSPEND   2
#define PM_PAD_WAKEUP_DEEP      3
#define PM_32K_WAKEUP_DEEP      4
/* End of 5316 Driver test. --------------------------------------------------*/


//u16 ADC_SampleValue[16];
void user_init()
{
	/* load customized freq_offset CAP value and TP value.*/
	blc_app_loadCustomizedParameters();

	/*-- BLE stack initialization --------------------------------------------*/
	u8  tbl_mac[] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};
	u32 *pmac = (u32 *)CFG_ADR_MAC;
	if(*pmac != 0xffffffff)
	{
		memcpy (tbl_mac, pmac, 6);
	}
	else{
		tbl_mac[0] = (u8)rand();
		flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
	}

	/*-- BLE Controller initialization ---------------------------------------*/
	blc_ll_initBasicMCU(tbl_mac);//mandatory
	blc_ll_initAdvertising_module(tbl_mac);//adv module: mandatory for BLE slave,
	blc_ll_initSlaveRole_module();//slave module: mandatory for BLE slave,

	/*-- BLE Host initialization ---------------------------------------------*/
	extern void my_att_init(void);
	//GATT initialization
	my_att_init();
	//L2CAP initialization
	blc_l2cap_register_handler(blc_l2cap_packet_receive);

	/*-- BLE SMP initialization ----------------------------------------------*/
#if (BLE_REMOTE_SECURITY_ENABLE)
	blc_smp_param_setBondingDeviceMaxNumber(4);  	//default is SMP_BONDING_DEVICE_MAX_NUM, can not bigger that this value
													//and this func must call before bls_smp_enableParing
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );
#else
	bls_smp_enableParing (SMP_PARING_DISABLE_TRRIGER );
#endif

	//HID_service_on_android7p0_init();  //hid device on android 7.0/7.1

	/*-- USER application initialization -------------------------------------*/
	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));


	/* Configure ADV packet */
#if(BLE_REMOTE_SECURITY_ENABLE)
	//get bonded device number
	u8 bond_number = blc_smp_param_getCurrentBondingDeviceNumber();
	smp_param_save_t  bondInfo;
	if(bond_number)   //at least 1 bonding device exist
	{
		//get the latest bonding device (index: bond_number-1 )
		blc_smp_param_loadByIndex( bond_number - 1, &bondInfo);
	}

	if(bond_number)//set direct adv
	{
		//set direct adv
		u8 status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
										ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, OWN_ADDRESS_PUBLIC,
										bondInfo.peer_addr_type,  bondInfo.peer_addr,
										MY_APP_ADV_CHANNEL,
										ADV_FP_NONE);
		//debug: ADV setting err
		if(status != BLE_SUCCESS) { write_reg8(0x8000, 0x11); 	while(1); }

		//it is recommended that direct adv only last for several seconds, then switch to indirect adv
		bls_ll_setAdvDuration(MY_DIRECT_ADV_TMIE, 1);
		bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &app_switch_to_indirect_adv);
	}
	else//set indirect ADV
#endif
	{
		u8 status = bls_ll_setAdvParam(  MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
										 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
										 0,  NULL,
										 MY_APP_ADV_CHANNEL,
										 ADV_FP_NONE);
		//debug: ADV setting err
		if(status != BLE_SUCCESS) { write_reg8(0x8000, 0x11); 	while(1); }
	}

	bls_ll_setAdvEnable(1);  //adv enable
	rf_set_power_level_index (RF_POWER_7P9dBm);//OK

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);

	/* Keyboard matrix initialization */
#if(RC_BTN_ENABLE)
	u32 pin[] = KB_DRIVE_PINS;
	for(int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
	{
		gpio_set_wakeup(pin[i],1,1);  	   //drive pin core(gpio) high wakeup suspend
		cpu_set_gpio_wakeup (pin[i],1,1);  //drive pin pad high wakeup deepsleep
	}

	bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_keyboard);
#endif

	/* Power Management initialization */
#if(BLE_REMOTE_PM_ENABLE)
	blc_ll_initPowerManagement_module();        //pm module:      	 optional
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif


	/* Battery Check Function Initialization */
#if(BATT_CHECK_ENABLE)
	ADC_BatteryCheckInit(ADC_Channel_PA7);
	lowBattDet_enable = 1;
#endif


	/* IR Function initialization */
#if(REMOTE_IR_ENABLE)
	user_key_mode = analog_read(DEEP_ANA_REG1);
	analog_write(DEEP_ANA_REG1, 0x00);
	IR_Init(IR_Pin_PA0);
#endif

	/* OTA Function Initialization  */
#if(BLE_REMOTE_OTA_ENABLE)
	bls_ota_clearNewFwDataArea(); //must
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(LED_show_ota_result);
#endif


	/* LED Indicator Initialization */
#if (BLT_APP_LED_ENABLE)
	device_led_init(GPIO_LED, 1);
#endif

	advertise_begin_tick = clock_time();

//	gpio_set_func(GPIO_PB1, AS_GPIO);
//	gpio_set_output_en(GPIO_PB1,1);
//	gpio_write(GPIO_PB1,0);
}



/*----------------------------------------------------------------------------*/
/*--------- Main Loop                                             ------------*/
/*----------------------------------------------------------------------------*/
u32 tick_loop;

void main_loop (void)
{
	tick_loop ++;
//	DBG_CHN0_TOGGLE;

	/* BLE entry -------------------------------------------------------------*/
	blt_sdk_main_loop();

	/* UI entry --------------------------------------------------------------*/
	#if (BATT_CHECK_ENABLE)
		if(lowBattDet_enable){
		  battery_power_check();
		}
	#endif

	#if(RC_BTN_ENABLE)
		proc_keyboard(0, 0, 0);
	#endif

	#if (BLT_APP_LED_ENABLE)
		device_led_process();
	#endif


	/*-- Power Management  -------------------------------------------------------*/
	blt_pm_proc();
}
#endif  //end of__PROJECT_5316_BLE_REMOTE__

