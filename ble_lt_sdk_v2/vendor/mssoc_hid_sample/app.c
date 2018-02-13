#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj/drivers/keyboard.h"
#include "../common/tl_audio.h"
#include "../common/blt_led.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/audio.h"
#include "../../proj/drivers/adc.h"
#include "../../proj/drivers/battery.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj/drivers/uart.h"

#if (__PROJECT_MSSOC_HID_SAMPLE__)

#define 	BLE_PM_ENABLE						0

#define     MY_APP_ADV_CHANNEL					BLT_ENABLE_ADV_ALL

#define 	MY_ADV_INTERVAL_MIN					48 //ADV_INTERVAL_30MS
#define 	MY_ADV_INTERVAL_MAX					56 //ADV_INTERVAL_30MS


#define 	ADV_IDLE_ENTER_DEEP_TIME			5  //60 s
#define 	CONN_IDLE_ENTER_DEEP_TIME			20  //60 s


#define 	BLE_PUSH_VOLUME_KEY 				0

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);


_attribute_data_retention_ u32			tick_led_config = 0x55;
//u32			tick_led_config = 0x55;
//////////////////////////////////////////////////////////////////////////////
//	 Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
const u8	tbl_advData[] = {
	 0x05, 0x09, 'c', 'R', 'e', 'm',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
	 0x03, 0x19, 0x80, 0x01, 					// 384, Generic Remote Control, Generic category
	 0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,		// incomplete list of service class UUIDs (0x1812, 0x180F)
};

const u8	tbl_scanRsp [] = {
		0x08, 0x09, 'c', 'R', 'e', 'm', 'o', 't', 'e',
	};


u32 interval_update_tick = 0;
int device_in_connection_state;


u32		advertise_begin_tick;



//////////////////// key type ///////////////////////
#define IDLE_KEY	   			0
#define CONSUMER_KEY   	   		1
#define KEYBOARD_KEY   	   		2
#define IR_KEY   	   			3

u8 		key_type;
u8 		user_key_mode;

u8 		key_buf[8] = {0};

int 	key_not_released;
int 	device_led_busy = 0;

int 	ir_not_released;

u32 	latest_user_event_tick;

u8 		user_task_flg;
u8 		sendTerminate_before_enterDeep = 0;
u8 		ota_is_working = 0;



void 	ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{

	if(*p == HCI_ERR_CONN_TIMEOUT){

	}
	else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}
	else if(*p == HCI_ERR_CONN_TERM_MIC_FAILURE){

	}
	else{

	}


#if (BLE_PM_ENABLE)
	 //user has push terminate pkt to ble TX buffer before deepsleep
	if(sendTerminate_before_enterDeep == 1){
		sendTerminate_before_enterDeep = 2;
	}
#endif

#if (LED_INDICATE_ENABLE)
	gpio_write(GPIO_LED2, LED_OFF_LEVAL); //red led off
#endif

	tick_led_config = clock_time() | 1;  //white led start to blink

	advertise_begin_tick = clock_time();
}

void	task_connect (u8 e, u8 *p, int n)
{

	tick_led_config = 0;

#if (LED_INDICATE_ENABLE)
	gpio_write(GPIO_LED1, LED_OFF_LEVAL);  //white led off
	gpio_write(GPIO_LED2, LED_ON_LEVAL);  //red led on
#endif

	bls_l2cap_requestConnParamUpdate (8, 8, 0, 400);

}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
#define MAX_BTN_SIZE			2
#define BTN_VALID_LEVEL			0

#define USER_BTN_1				0x01
#define USER_BTN_2				0x02

u32 ctrl_btn[] = {SW1_GPIO, SW2_GPIO};
u8 btn_map[MAX_BTN_SIZE] = {USER_BTN_1, USER_BTN_2};

typedef	struct{
	u8 	cnt;				//count button num
	u8 	btn_press;
	u8 	keycode[MAX_BTN_SIZE];			//6 btn
}vc_data_t;
vc_data_t vc_event;

typedef struct{
	u8  btn_history[4];		//vc history btn save
	u8  btn_filter_last;
	u8	btn_not_release;
	u8 	btn_new;					//new btn  flag
}btn_status_t;
btn_status_t 	btn_status;


u8 btn_debounce_filter(u8 *btn_v)
{
	u8 change = 0;

	for(int i=3; i>0; i--){
		btn_status.btn_history[i] = btn_status.btn_history[i-1];
	}
	btn_status.btn_history[0] = *btn_v;

	if(  btn_status.btn_history[0] == btn_status.btn_history[1] && btn_status.btn_history[1] == btn_status.btn_history[2] && \
		btn_status.btn_history[0] != btn_status.btn_filter_last ){
		change = 1;


		btn_status.btn_filter_last = btn_status.btn_history[0];
	}

	return change;
}


u8 vc_detect_button(int read_key)
{
	u8 btn_changed, i;
	memset(&vc_event,0,sizeof(vc_data_t));			//clear vc_event
//	vc_event.btn_press = 0;

	for(i=0; i<MAX_BTN_SIZE; i++){
		if(BTN_VALID_LEVEL != !gpio_read(ctrl_btn[i])){
			vc_event.btn_press |= BIT(i);
		}
	}


	btn_changed = btn_debounce_filter(&vc_event.btn_press);


	if(btn_changed && read_key){
		for(i=0; i<MAX_BTN_SIZE; i++){
			if(vc_event.btn_press & BIT(i)){
				vc_event.keycode[vc_event.cnt++] = btn_map[i];
			}
		}

		return 1;
	}

	return 0;
}



void proc_button (void)
{
	static u32 button_det_tick;
	if(clock_time_exceed(button_det_tick, 5000))
	{
		button_det_tick = clock_time();
	}
	else{
		return;
	}



//	static u32 button_history = 0;
//	static u32 last_singleKey_press_tick;

	static int button1_press_flag;
	static u32 button1_press_tick;
	static int button2_press_flag;
	static u32 button2_press_tick;


	int det_key = vc_detect_button (1);

	if (det_key)  //key change: press or release
	{

		u8 key0 = vc_event.keycode[0];
//		u8 key1 = vc_event.keycode[1];


		key_not_released = 1;

		if(vc_event.cnt == 2)  //two key press
		{

		}
		else if(vc_event.cnt == 1) //one key press
		{
			if(key0 == USER_BTN_1)
			{
				button1_press_flag = 1;
				button1_press_tick = clock_time();

#if (BLE_PUSH_VOLUME_KEY)
				u16 consumer_key = MKEY_VOL_UP;
				bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
				key_type = CONSUMER_KEY;
#else
				key_buf[2] = VK_1;
				bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);
				key_type = KEYBOARD_KEY;
#endif
			}
			else if(key0 == USER_BTN_2)
			{
				button2_press_flag = 1;
				button2_press_tick = clock_time();

#if (BLE_PUSH_VOLUME_KEY)
				u16 consumer_key = MKEY_VOL_DN;
				bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
				key_type = CONSUMER_KEY;
#else
				key_buf[2] = VK_2;
				bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);
				key_type = KEYBOARD_KEY;
#endif
			}
		}
		else{  //release

			button1_press_flag = 0;
			button2_press_flag = 0;

			key_not_released = 0;

			if(key_type == CONSUMER_KEY){
				u16 consumer_key = 0;
				bls_att_pushNotifyData (HID_CONSUME_REPORT_INPUT_DP_H, (u8 *)&consumer_key, 2);
			}
			else{
				key_buf[2] = 0;
				bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8); //release
			}
		}

	}


}



void blt_pm_proc(void)
{
#if (BLE_PM_ENABLE)

		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);

		user_task_flg = key_not_released || device_led_busy;

		if(user_task_flg){
			bls_pm_setManualLatency(0);
		}


#if 0 //deepsleep
		if(sendTerminate_before_enterDeep == 1){ //sending Terminate and wait for ack before enter deepsleep
			if(user_task_flg){  //detect key Press again,  can not enter deep now
				sendTerminate_before_enterDeep = 0;
			}
		}
		else if(sendTerminate_before_enterDeep == 2){  //Terminate OK
			cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
//			analog_write(DEEP_ANA_REG0, CONN_DEEP_FLG);
		}

		//adv 60s, deepsleep
		if( blc_ll_getCurrentState() == BLS_LINK_STATE_ADV && \
			clock_time_exceed(advertise_begin_tick , ADV_IDLE_ENTER_DEEP_TIME * 1000000)){
			cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
//			analog_write(DEEP_ANA_REG0, ADV_DEEP_FLG);
		}
		//conn 60s no event(key/voice/led), enter deepsleep
		else if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && !user_task_flg && \
				clock_time_exceed(latest_user_event_tick, CONN_IDLE_ENTER_DEEP_TIME * 1000000) ){

			bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //push terminate cmd into ble TX buffer
			sendTerminate_before_enterDeep = 1;
		}
#endif



#endif
}




rf_packet_adv_t	debug_pkt_adv = {
		sizeof (rf_packet_adv_t) - 4,		// dma_len
		{LL_TYPE_ADV_IND, 0, 0, 0},					// type
		sizeof (rf_packet_adv_t) - 6,		// rf_len
		{0x33, 0x33, 0x33, 0x33, 0x33, 0x33},	// advA
		// data
};

u8 debug_pkt_adv_2[30] = { 10,0,0,0,  0, 8, 0x33,0x33,0x33,0x33,0x33, 0x33,0x01};

void ble_tx_adv_test(void)
{
	//pm init
	write_reg8(0x60, 0x00);
	write_reg8(0x61, 0x00);
	write_reg8(0x62, 0x00);
	write_reg8(0x63, 0xff);
	write_reg8(0x64, 0xff);
	write_reg8(0x65, 0xff);
	analog_write(0x80, 0x22);
	analog_write(0x82, 0x24);
	analog_write(0x06, 0x00);
	analog_write(0x07, 0x19);
	analog_write(0x33, 0x77);
	analog_write(0x27, 0x00);
	analog_write(0x28, 0x00);
	analog_write(0x29, 0x00);
	analog_write(0x2a, 0x00);

	write_reg8(0x74f, 0x01);



	//clock init
	write_reg8(0x66, 0x43); //PLL 16M

	//tbl_rf_ini
	analog_write(0x34, 0x80);
	analog_write(0x00, 0x20);
	analog_write(0x80, 0x50);
	analog_write(0x81, 0xd5);  //fre_offset
	analog_write(0x82, 0x34);
	analog_write(0x86, 0xbb);
	analog_write(0x8c, 0xb0);
	analog_write(0x9a, 0x05);

	analog_write(0xa9, 0x2a);
	analog_write(0x08, 0x44);
	analog_write(0xa8, 0x73);
	analog_write(0xb6, 0x6a);
	analog_write(0xa7, 0xe3);
	analog_write(0xac, 0x67);
	analog_write(0x0b, 0x7c);
	analog_write(0xb3, 0x50);
	analog_write(0x08, 0x44);
	analog_write(0xb7, 0xb0);

	analog_write(0xa2, 0x25);

	write_reg8(0x433, 0x01);
	write_reg8(0xf04, 0x68);
	write_reg8(0xf06, 0x00);
	write_reg8(0xf0c, 0x50);
	write_reg8(0xf10, 0x00);
	write_reg8(0xc40, 0x04);
	write_reg8(0xc41, 0x04);
	write_reg8(0xc42, 0x04);
	write_reg8(0xc43, 0x04);
	write_reg8(0xc44, 0x04);
	write_reg8(0xc45, 0x04);
	write_reg8(0xc46, 0x04);
	write_reg8(0xc47, 0x04);




	//tbl_rf_1m
	analog_write(0xaa, 0xf6);
	analog_write(0xa3, 0xf0);
	analog_write(0x8f, 0x84);
	analog_write(0x9e, 0x56);
	write_reg8(0x400, 0x0f);
	write_reg8(0x401, 0x01);
	write_reg8(0x402, 0x26);
	write_reg8(0x404, 0xf5);
	write_reg8(0x405, 0x04);
	write_reg8(0x420, 0x5e);
	write_reg8(0x421, 0x01);
	write_reg8(0x422, 0x01);
	write_reg8(0x424, 0x91);
	write_reg8(0x428, 0x80);
	write_reg8(0x42b, 0x01);
	write_reg8(0x430, 0x0e);
	write_reg8(0x4db, 0x51);
	write_reg8(0xf03, 0x1e);


//	rf_set_power_level_index (RF_POWER_9m34PdBm);
	analog_write (0xa2, 0x25);
	analog_write (0x0a, 0x0b);
	analog_write (0xb6, 0x6a);
	analog_write (0x8b, 0xa3);

//	rf_set_ble_access_code_adv ();
	write_reg32 (0x800408, 0xd6be898e);

//	rf_set_ble_crc_adv ();
	write_reg32 (0x80042c, 0x555555);

	while(1){
		sleep_us(1000);

//		STOP_RF_STATE_MACHINE;						// stop SM
		REG_ADDR8(0xf00) = 0x80;


//		rf_set_ble_channel (37);
		write_reg8 (0x80040d, 37);
		write_reg8 (0x800f16, 0x29);
		write_reg8 (0x800428, RF_TRX_MODE);	// rx disable
		write_reg8 (0x800f02, RF_TRX_OFF);	// reset tx/rx state machine
		write_reg16 (0x8004e6, 2402);	//
		analog_write (0x93, 0x38);

		sleep_us(100);
		reg_rf_irq_status = 0xffff;

		debug_pkt_adv.data[0] ++;
		debug_pkt_adv_2[12]++;
//		rf_start_stx ((void *)&debug_pkt_adv, clock_time() + 100);
		write_reg32(0x800f18, REG_ADDR32(0x740) + 100 );
	    write_reg8(0x800f16, read_reg8(0x800f16) | 0x04);	// Enable cmd_schedule mode
		write_reg8 (0x800f00, 0x85);						// single TX
		write_reg16 (0x800c0c, (u16)((u32)debug_pkt_adv_2));
//		write_reg16 (0x800c0c, (u16)((u32)&debug_pkt_adv));

		sleep_us(1000);


		sleep_us(30000);
	}



}

void user_init_normal(void)
{

	sleep_us(2000000);

//		analog_write(0x05, analog_read(0x05) | BIT(5));

#if 0


#else
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value


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
	blc_ll_initBasicMCU();                      //mandatory
	blc_ll_initStandby_module(tbl_mac);				//mandatory
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,






	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization


 	//// smp initialization ////
#if (BLE_REMOTE_SECURITY_ENABLE)
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );
#else
	bls_smp_enableParing (SMP_PARING_DISABLE_TRRIGER );
#endif


///////////////////// USER application initialization ///////////////////
	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));

	u8 status = bls_ll_setAdvParam(  MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
									 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
									 0,  NULL,
									 MY_APP_ADV_CHANNEL,
									 ADV_FP_NONE);
	if(status != BLE_SUCCESS) { write_reg8(0x8000, 0x11); 	while(1); }  //debug: adv setting err

	bls_ll_setAdvEnable(1);  //adv enable
	rf_set_power_level_index (RF_POWER_9m34PdBm);

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);



	///////////////////// Power Management initialization///////////////////
#if (BLE_PM_ENABLE)
	blc_ll_initPowerManagement_module();        //pm module:      	 optional
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif



//	for (int i=0; i<(sizeof (ctrl_btn)/sizeof(*ctrl_btn)); i++)
//	{
//		gpio_set_wakeup(ctrl_btn[i], 0, 1);  	   //drive pin core(gpio) low wakeup suspend
//		cpu_set_gpio_wakeup (ctrl_btn[i], 0, 1);  //drive pin pad low wakeup deepsleep
//	}


	tick_led_config = clock_time () | 1;


	douber_cal();

#endif

}


_attribute_data_retention_  int deep_ret_cnt = 0;
_attribute_data_retention_  u32 deep_ret_tick = 0;

_attribute_ram_code_ void user_init_deepRetn(void)
{
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value
	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (RF_POWER_9m34PdBm);


	blc_ll_recoverDeepRetention();

	//test key
#if 0

	if(clock_time_exceed(deep_ret_tick, 1000000)){
		deep_ret_tick = clock_time();

		deep_ret_cnt ++;
		if(deep_ret_cnt & 1){
			key_buf[2] = VK_2;
		}
		else{
			key_buf[2] = 0;
		}
		bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);
	}


#elif 0
	deep_ret_cnt ++;
	if(deep_ret_cnt & 1){
		key_buf[2] = VK_2;
	}
	else{
		key_buf[2] = 0;
	}
	bls_att_pushNotifyData (HID_NORMAL_KB_REPORT_INPUT_DP_H, key_buf, 8);
#endif

	DBG_CHN4_HIGH;
}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
u32 tick_loop;

void main_loop (void)
{

	tick_loop ++;


	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop();

	////////////////////////////////////// UI entry /////////////////////////////////
	proc_button();


#if (LED_INDICATE_ENABLE)
	if (tick_led_config && clock_time_exceed (tick_led_config, 300000))
	{
		tick_led_config = clock_time () | 1;
		gpio_toggle (GPIO_LED1);
	}
#endif

	blt_pm_proc();  //power management
}


#endif  //end of __PROJECT_826x_HID_SAMPLE
