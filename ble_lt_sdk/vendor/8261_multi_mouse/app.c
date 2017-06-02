#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/ll/ll_whitelist.h"
#include "../../proj/drivers/keyboard.h"
#include "../common/tl_audio.h"
#include "../common/blt_led.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/audio.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../common/blt_soft_timer.h"
#include "../../proj_lib/ble/ble_smp.h"

#include "../../proj/drivers/uart.h"
#include "../../proj/mcu/putchar.h"
#include "../../proj/common/printf.h"



//#include "mouse_button.h"


/*******  Add mouse module ********/


#include "mouse.h"
#include "mouse_button.h"
#include "mouse_wheel.h"
#include "mouse_sensor_pix.h"
#include "mouse_sensor.h"

#include "mouse_custom.h"
#include "mouse_emi.h"
#include "drv_mouse_pmw3610.h"


#if ( __PROJECT_8261_MULTI_MOUSE__ )


#define  USER_TEST_BLT_SOFT_TIMER					1 //test soft timer


//MYFIFO_INIT(hci_rx_fifo, 72, 2);
//MYFIFO_INIT(hci_tx_fifo, 72, 4);

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);

/**    define variable param    **/

u16 BLE_MODE_SWITCH_THRESH;
u16 switch_mode_start_flg;

u8 adv_type_switch;
u8 adv_type_det;
u8 adv_direct_flg = 0;


u8 SysMode = RF_2M_2P4G_MODE; 			//default mode is ble mode

extern mouse_status_t mouse_status;
extern mouse_sleep_t  mouse_sleep;
extern mouse_hw_t     mouse_hw;

u8 mouse_get_pre_info_from_master;		//0 : successful,  1: fail
addr_info_t master_per_info;

extern st_ll_conn_slave_t		bltc;



////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

#if(HID_MOUSE_ATT_ENABLE)

#if(MI_MOUSE_EN || 1)
#define			HID_HANDLE_MOUSE_REPORT				15     //24
#else
#define			HID_HANDLE_MOUSE_REPORT				24     //24
#endif
#define			HID_HANDLE_CONSUME_REPORT			28
#define			HID_HANDLE_KEYBOARD_REPORT			32
#define			AUDIO_HANDLE_MIC					50

#else
//		handle 0x0e: consumper report
#define			HID_HANDLE_CONSUME_REPORT			21
#define			HID_HANDLE_KEYBOARD_REPORT			25
#define			AUDIO_HANDLE_MIC					43

#endif


u8 mouse_relsease_buff[4] = {0};
unsigned short battValue[10] = {2376, 2897, 2678,2579, 2533, 2899, 2756, 2734, 2890, 2609};

//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xe1, 0xb1, 0xb2, 0xe8, 0xa3, 0xc7};

u8	tbl_advData[] = {
	 0x07, 0x09, 't', 'm', 'o', 'u','s','e',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
	 0x03, 0x19, 0x80, 0x01, 					// 384, Generic Remote Control, Generic category
	 0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,		// incomplete list of service class UUIDs (0x1812, 0x180F)
};


u8	tbl_scanRsp [] = {
		 0x08, 0x09, 't', 'M', 'o', 'u', 's', 'e',
	};

/////////////////////////// led management /////////////////////
enum{
	LED_POWER_ON = 0,

	LED_SHINE_SLOW, //3
	LED_SHINE_FAST, //4
	LED_SHINE_OTA, //5
};

/*
const led_cfg_t led_cpi[] = {
		{250, 	250, 	1, 		0x04},
		{250, 	250, 	2, 		0x04},
		{250, 	250, 	3, 		0x04},
};
*/
const led_cfg_t led_cfg[] = {

		{250,	250, 	1,	0x02,  },	// CPI 0
		{250, 	250, 	2, 	0x02,  },	// CPI 1
		{250, 	250, 	3, 	0x02,  },	// CPI 2

#if (MOUSE_EMI_TEST_WITH_LED)
		{500,   500,    1,  0x02,  },	//EMI Carry
		{500,   500,    2,  0x02,  },	//EMI CD
		{500,   500,    3,  0x02,  },	//EMI TX
		{500,   500,    4,  0x02,  },	//EMI RX
#endif
	    {500,   0,   	1,  0x00,  },    // 1Hz, switch mode

	    {500,	500 ,   2,	0x02,  },    // 1Hz for 2 seconds, back to non-direct adv
	    {125,	125 ,   3,	0x08,  },    // 4Hz for 3 seconds

};


u32	advertise_begin_tick;
u32 latest_user_event_tick;
u8  user_task_flg;
u8  sendTerminate_before_enterDeep = 0;

static u8 ota_is_working = 0;

#if (STUCK_KEY_PROCESS_ENABLE)
u32 stuckKey_keyPressTime;
#endif


u8 conn_para_updata_success = 0;
u32 conn_para_tick;
_attribute_ram_code_ u8 conn_para_updata_retry(void){

	if(conn_para_tick != 0 && !conn_para_updata_success && !adv_direct_flg){
		if(clock_time_exceed(conn_para_tick, 5000000)){
			if( bltc.conn_interval_next == 9 && bltc.conn_latency_next == 99){
				conn_para_updata_success = 1;
			}
			else{
				conn_para_tick = clock_time();
				bls_l2cap_requestConnParamUpdate (9, 9, 99, 400);   //10ms *(99+1) = 1000 ms
			}
		}
	}
}



#if(MOUSE_BATT_MOUDULE_EN)
/***********************************	希尔排序           ***********************************
 *
 * 算法实现：将需要排序的数组按增量d(n/2, n/4, n/8)进行分组插入排序，直到增量d为1，相当于执行一次直接插入排序
 *
**************************************************************************************/
inline void ShellInsertSort(u16 *p, u8 len, int dt){
	for(int i= dt; i<len; i++){
		if(p[i] < p[i-dt]){
			int j = i-dt;
			int watcher = p[i];		//设置哨兵为第i个元素
			while(watcher < p[j] && (j >= 0)){	//进行比较，找寻合适的插入位置
				p[j+dt] =  p[j];
				j -= dt;
			}
			p[j+dt] = watcher;		//插入到正确的位置
		}
	}
}
inline void ShellSort(u16 *p, u8 len){
	u8 dk = len >> 1;
		while(dk >= 1){
			ShellInsertSort(p, len, dk);
			dk = dk >> 1;
		}
 }


_attribute_ram_code_ void mouse_battery_check(unsigned short *batt, u8 len, unsigned short alarm_thresh)
{
	static u32 battCheckTick = 0;
	if(!clock_time_exceed(battCheckTick, 1000000)){
		return;
	}
	else{
		battCheckTick = clock_time();
	}

	for(int i=0; i<len;i++){
		batt[i] = adc_SampleValueGet();
	}
	ShellSort(batt, len);//希尔排序
	u8 midd = len >> 1;  //1/3分压，batt[midd-1] + batt[midd] + batt[midd-1]抵消
	batt[0] =  (1428*((batt[midd] + batt[midd - 1] + batt[midd + 1])-128)/(16383-256));    //2^14 - 1 = 16383;

	if(batt[0] < alarm_thresh){
		device_led_setup(led_cfg[LED_BATTRY_ALARM]);			//4Hz, 3 seconds
	}

}


#endif

_attribute_ram_code_ void proc_mouse(u8 e, u8 *p, int n);

#if (USER_TEST_BLT_SOFT_TIMER)

_attribute_ram_code_ int mouse_update_suspend_proc(void)
{
	//gpio 0 toggle to see the effect

	proc_mouse(0,0,0);
	mouse_battery_check(battValue, sizeof(battValue)/sizeof(battValue[0]), 2200);

	device_led_process();  //led management
	if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN ){
		conn_para_updata_retry();
		if(adv_type_det){
			adv_type_det = 0;
			u8 analog_r4 = analog_read(DEEP_ANA_REG4)& (~BIT(3));
			analog_write(DEEP_ANA_REG4, analog_r4);
		}
		BLE_MODE_SWITCH_THRESH =  BLE_CON_MODE_SWITCH_CNT;// : BLE_CON_MODE_SWITCH_CNT;
		if(!clock_time_exceed(latest_user_event_tick, 3000000)){
			return 11350;
		}
		else if(!clock_time_exceed(latest_user_event_tick, 61000000)){

			return 105000;
		}

	}
	else{
		return 0;
//		if(!clock_time_exceed(latest_user_event_tick, 62 * 1000000)){
//			BleSuspendMode = BLE_SUSPEND_1;
//			return 105000;
//		}
	}
}

int gpio_test1(void)
{
	//gpio 1 toggle to see the effect
	//DBG_CHN1_TOGGLE;

#if 0
	static u8 flg = 0;
	flg = !flg;
	if(flg){
		return 7000;
	}
	else{
		return 17000;
	}
#endif
}

int gpio_test2(void)
{
	//gpio 2 toggle to see the effect
	//DBG_CHN2_TOGGLE;

#if 0
	//timer last for 5 second
	if(clock_time_exceed(0, 5000000)){
		//return -1;
		//blt_soft_timer_delete(&gpio_test2);
	}
	else{

	}

	return 0;
#endif
}

int gpio_test3(void)
{
	//gpio 3 toggle to see the effect
	//DBG_CHN3_TOGGLE;

	return 0;
}



#endif



void 	ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{
	if(*p == HCI_ERR_CONN_TIMEOUT){

	}
	else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}
	else{

	}

	 //user has push terminate pkt to ble TX buffer before deepsleep
	if(sendTerminate_before_enterDeep == 1){
		sendTerminate_before_enterDeep = 2;
	}

	advertise_begin_tick = clock_time();

}

/*
u8 conn_para_up = 0;
u8 conn_para_uu[12];
void ble_para_updata(u8 e, u8 *p, int n ){

//	if( bltc.conn_interval_next != 9 ){
//		conn_para_tick = clock_time();
//		bls_l2cap_requestConnParamUpdate (9, 9, 0, 400);   //10ms *(99+1) = 1000 ms
//	}
	//memcpy(conn_para_uu + conn_para_up * 6, p, 6);
	//conn_para_up++;
}
*/

void	task_connect (u8 e, u8 *p, int n)
{
	conn_para_tick = clock_time();
	if(adv_direct_flg){				//发定向广播包时，无需sever discover,直接更新参数
		bls_l2cap_requestConnParamUpdate (9, 9, 99, 400);   //10ms *(99+1) = 1000 ms
	}
	else{							//非定向时，latency先为0,后继续更新
		bls_l2cap_requestConnParamUpdate (9, 9, 0, 400);   //10ms *(99+1) = 1000 ms
	}
}

void	update_done (u8 e, u8 *p, int n)
{



}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


//This function process ...
void deep_wakeup_proc(void)
{
#if(DEEPBACK_FAST_KEYSCAN_ENABLE)
	//if deepsleep wakeup is wakeup by GPIO(key press), we must quickly scan this
	//press, hold this data to the cache, when connection established OK, send to master
	//deepsleep_wakeup_fast_keyscan
	if(analog_read(DEEP_ANA_REG0) == CONN_DEEP_FLG){
		if(kb_scan_key (KB_NUMLOCK_STATUS_POWERON,1) && kb_event.cnt){
			deepback_key_state = DEEPBACK_KEY_CACHE;
			key_not_released = 1;
			memcpy(&kb_event_cache,&kb_event,sizeof(kb_event));
		}
	}
#endif
}




void deepback_pre_proc(int *det_key)
{
#if 0
	// to handle deepback key cache
	extern u32 blt_conn_start_tick; //ble connect establish time
	if(!(*det_key) && deepback_key_state == DEEPBACK_KEY_CACHE && blt_state == BLT_LINK_STATE_CONN \
			&& clock_time_exceed(blt_conn_start_tick,25000)){

		memcpy(&kb_event,&kb_event_cache,sizeof(kb_event));
		*det_key = 1;

		if(key_not_released || kb_event_cache.keycode[0] == VK_M){  //no need manual release
			deepback_key_state = DEEPBACK_KEY_IDLE;
		}
		else{  //need manual release
			deepback_key_tick = clock_time();
			deepback_key_state = DEEPBACK_KEY_WAIT_RELEASE;
		}
	}
#endif
}

_attribute_ram_code_ void proc_mouse(u8 e, u8 *p, int n)
{

	/***************  按键检测      *****************/
	//clear x,y
	//static u8 last_btn_value;
	u8 cur_btn_value;
	*(u32 *)(mouse_status.data) = 0;
	//gpio_write(mouse_hw.sensor_int, 1);

	cur_btn_value = mouse_button_detect(&mouse_status, MOUSE_BTN_LOW); // 0:release, 1:pressed, 2:hold on


#if(MOUSE_WHEEL_MODULE_EN)
	u32 wheel_prepare_tick = mouse_wheel_prepare_tick();
#endif

#if(TELINK_MOUSE_DEMO)
	mouse_sensor_data( &mouse_status );
#else
	drv_mouse_motion_report(&mouse_data.x, 0);
#endif

#if(MOUSE_WHEEL_MODULE_EN)
	mouse_wheel_process(&mouse_status, wheel_prepare_tick);
#endif

#if(MOUSE_BTN_MODULE_EN)
	mouse_button_process(&mouse_status);
#endif

	//start to switch mode to 2.4G, then release button
	if( switch_mode_start_flg){
		bls_att_pushNotifyData (HID_HANDLE_MOUSE_REPORT, mouse_relsease_buff, 4);
	}
	else{
		if(*(u32 *)(mouse_status.data)){				//when switch mode start, do not need to send data
			latest_user_event_tick = clock_time();
			bls_att_pushNotifyData (HID_HANDLE_MOUSE_REPORT, (u8 *)mouse_status.data, 4);
			//Suspend_back_flag = (BleSuspendMode == BLE_SUSPEND_1) ? 1 : 0;
		}
		else if(cur_btn_value){//(last_btn_value && !cur_btn_value){
			bls_att_pushNotifyData (HID_HANDLE_MOUSE_REPORT, mouse_relsease_buff, 4);
		}
	}

	//last_btn_value = cur_btn_value;
}


u16 ui_manual_latency_when_key_press(void)
{
	u16 cur_interval = bls_ll_getConnectionInterval();

	if(cur_interval > 40){  //50 ms = 40*1.25ms
		return 0;
	}
	else{
		return   (40/cur_interval) - 1;        //50ms
	}
}


void blt_pm_proc(void)
{
#if(BLE_MOUSE_PM_ENABLE)
	if(ota_is_working){
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
	}
	else{
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
		user_task_flg = DEVICE_LED_BUSY;
		if(user_task_flg ){
#if (LONG_PRESS_KEY_POWER_OPTIMIZE)
			extern int key_matrix_same_as_last_cnt;
			if(key_matrix_same_as_last_cnt > 5){  //key matrix stable can optize
				bls_pm_setManualLatency( ui_manual_latency_when_key_press() );
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
			}
		}
		else if(sendTerminate_before_enterDeep == 2){  //Terminate OK
			bls_pm_setSuspendMask (DEEPSLEEP_ADV); //when terminate, link layer change back to adc state
			bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio PAD wakeup deesleep
			//analog_write(DEEP_ANA_REG0, CONN_DEEP_FLG);
		}

		//adv 60s, deepsleep
		if( blc_ll_getCurrentState() == BLS_LINK_STATE_ADV && \
			clock_time_exceed(advertise_begin_tick , 60 * 1000000)){
			bls_pm_setSuspendMask (DEEPSLEEP_ADV); //set deepsleep
			bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio PAD wakeup deesleep
			//analog_write(DEEP_ANA_REG0, ADV_DEEP_FLG);

			mouse_sleep.sensor_sleep = 1;
			mouse_status.mouse_sensor = SENSOR_MODE_WORKING;
		    gpio_setup_up_down_resistor(mouse_status.hw_define->sensor_data, PM_PIN_PULLUP_1M );
		    mouse_sensor_sleep_wakeup( &mouse_status.mouse_sensor, &mouse_sleep.sensor_sleep, 0 );

		}
		//conn 60s no event(key/voice/led), enter deepsleep
		else if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && !user_task_flg && \
				clock_time_exceed(latest_user_event_tick, 60 * 1000000) ){

			bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //push terminate cmd into ble TX buffer
			sendTerminate_before_enterDeep = 1;
			//u32 wheel0_wakeup_level = !gpio_read(mouse_status.hw_define->wheel[0]);
			//cpu_set_gpio_wakeup(mouse_status.hw_define->wheel[0], wheel0_wakeup_level, 1);  //PAD wakeup

			mouse_sleep.sensor_sleep = 1;
			mouse_status.mouse_sensor = SENSOR_MODE_WORKING;
		    gpio_setup_up_down_resistor(mouse_status.hw_define->sensor_data, PM_PIN_PULLUP_1M );
		    mouse_sensor_sleep_wakeup( &mouse_status.mouse_sensor, &mouse_sleep.sensor_sleep, 0 );
		}
#endif


	}

#endif
}


void  ble_remote_set_sleep_wakeup (u8 e, u8 *p, int n)
{
	if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 * CLOCK_SYS_CLOCK_1MS){  //suspend time > 30ms.add gpio wakeup
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);  //gpio CORE wakeup suspend

		//u32 debug_wheel0_wakeup_level = !gpio_read(mouse_status.hw_define->wheel[0]);
		//gpio_set_wakeup(mouse_status.hw_define->wheel[0], debug_wheel0_wakeup_level, 1);	//CORE wakeup
	}
}



void rf_customized_param_load(void)
{
	  //flash 0x77000 customize freq_offset adjust cap value, if not customized, default ana_81 is 0xd0
	 if( (*(unsigned char*) CUST_CAP_INFO_ADDR) != 0xff ){
		 //ana_81<4:0> is cap value(0x00 - 0x1f)
		 analog_write(0x81, (analog_read(0x81)&0xe0) | ((*(unsigned char*) CUST_CAP_INFO_ADDR)&0x1f) );
	 }

	 //flash 0x77040 customize TP0, flash 0x77041 customize TP1
	 if( ((*(unsigned char*) (CUST_TP_INFO_ADDR)) != 0xff) && ((*(unsigned char*) (CUST_TP_INFO_ADDR+1)) != 0xff) ){
		 rf_update_tp_value(*(unsigned char*) (CUST_TP_INFO_ADDR), *(unsigned char*) (CUST_TP_INFO_ADDR+1));
	 }

	 // customize 32k RC cap value, if not customized, default ana_32 is 0x80
	 if( (*(unsigned char*) CUST_RC32K_CAP_INFO_ADDR) != 0xff ){
		 //ana_81<4:0> is cap value(0x00 - 0x1f)
		 analog_write(0x32, *(unsigned char*) CUST_RC32K_CAP_INFO_ADDR );
	 }
}


/** BLe mode **/
void ble_user_init()
{
	u8 status;
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

////////////////// BLE stack initialization ////////////////////////////////////
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff)
	{
		memcpy (tbl_mac, pmac, 6);
	}
	else{
		tbl_mac[0] = (u8)rand();
		flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
	}


///////////// BLE stack Initialization ////////////////
////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory

	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
	blc_ll_initPowerManagement_module();        //pm module:      	 optional



////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER ); 	//smp initialization

//HID_service_on_android7p0_init();  //hid device on android 7.0

///////////////////// USER application initialization ///////////////////
	bls_ll_setScanRspData(tbl_scanRsp, sizeof(tbl_scanRsp));
	BLE_MODE_SWITCH_THRESH = BLE_CON_MODE_SWITCH_CNT;
	mouse_get_pre_info_from_master = bls_smp_getPeerAddrInfo (&master_per_info);		//0:success, 1:fail

	if(!mouse_get_pre_info_from_master && !adv_type_det){			//direct adv
		adv_direct_flg = 1;
		status = bls_ll_setAdvParam( ADV_INTERVAL_10MS, ADV_INTERVAL_10MS, \
										ADV_TYPE_CONNECTABLE_DIRECTED_LOW_DUTY, OWN_ADDRESS_PUBLIC, \
									 	 0,  master_per_info.addr_mac,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);		//directed adv packet, all channel, interval = 30ms, duration 60s
		bls_ll_setAdvDuration(61000000, 1);
	}
	else{
		status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS, \
									 	 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 	 0,  NULL,  BLT_ENABLE_ADV_37, ADV_FP_NONE);
		bls_ll_setAdvData( tbl_advData, sizeof(tbl_advData) );
	}

	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}

	bls_ll_setAdvEnable(1);  //adv enable
	rf_set_power_level_index (RF_POWER_8dBm);

//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);
//	bls_app_registerEventCallback (BLT_EV_FLAG_ENCRYPTION_CONN_DONE, &ble_para_updata);


#if(KEYSCAN_IRQ_TRIGGER_MODE)
	reg_irq_src = FLD_IRQ_GPIO_EN;
#endif

	//bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_mouse);

	//adc_BatteryCheckInit(2);///add by Q.W

	///////////////////// Power Management initialization///////////////////
#if(BLE_MOUSE_PM_ENABLE)
	blc_ll_initPowerManagement_module();
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	//bls_app_registerEventCallback (BLT_EV_FLAG_CONN_PARA_REQ, &ble_mouse_para_req_conf);
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif


////////////////LED initialization /////////////////////////

#if(TELINK_MOUSE_DEMO)
	device_led_init(GPIO_LED, 1);
	//gpio_set_output_en(GPIO_LED, 1);
#else
	device_led_init(GPIO_LED_R, 0);
#endif

#if (BLE_REMOTE_OTA_ENABLE)
////////////////// OTA relative ////////////////////////
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(LED_show_ota_result);
#endif



//////////////// TEST  /////////////////////////
#if (USER_TEST_BLT_SOFT_TIMER)
	blt_soft_timer_init();
	blt_soft_timer_add(&mouse_update_suspend_proc, 11350);
#endif


	advertise_begin_tick = clock_time();

}


void user_init(){

#if(SIMULATE_UART_FUNC_EN)
	gpio_set_output_en(UART_TX_PIN_SIM, 1);
	gpio_set_input_en(UART_TX_PIN_SIM, 1);

	gpio_write(UART_TX_PIN_SIM, 1);
#else

	mouse_hw_init();
#if(MOUSE_BATT_MOUDULE_EN)
	adc_BatteryCheckInit(ADC_CLK_4M, 1, Battery_Chn_B7, 0, SINGLEEND, RV_1P428, RES14, S_3);
#endif
	if(SysMode == RF_2M_2P4G_MODE){
		normal_user_init();
	}
	else{
		ble_user_init();
	}


#endif
}


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////

void main_loop ()
{

	if(SysMode == RF_1M_BLE_MODE ){
		////////////////////////////////////// BLE entry /////////////////////////////////
#if (BLT_SOFTWARE_TIMER_ENABLE)
		blt_soft_timer_process(MAINLOOP_ENTRY);
#endif

		blt_sdk_main_loop();
		////////////////////////////////////// UI entry /////////////////////////////////

		//proc_mouse(0,0,0);

		blt_pm_proc();						//power management
	}
	else{
		mouse_main_loop();
	}

}

//ble_remote_set_sleep_wakeup



#endif  //end of  (__PROJECT_8261_MULTI_MOUSE__)
