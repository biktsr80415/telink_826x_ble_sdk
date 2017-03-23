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
#include "../../proj_lib/ble/blt_config.h"
#include "../common/blt_soft_timer.h"
#include "../../proj_lib/ble/ble_smp.h"

#include "../../proj/drivers/uart.h"

//#include "mouse_button.h"


/*******  Add mouse module ********/


#include "mouse.h"
#include "mouse_button.h"
#include "mouse_wheel.h"
#include "mouse_sensor_pix.h"
#include "mouse_sensor.h"

#include "mouse_custom.h"


#if ( __PROJECT_8261_MULTI_MOUSE__ )


#define  USER_TEST_BLT_SOFT_TIMER					0  //test soft timer


MYFIFO_INIT(hci_rx_fifo, 72, 2);
MYFIFO_INIT(hci_tx_fifo, 72, 4);

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);

/**    define variable param    **/


u16 ble_swith_time_thresh;
u16 switch_mode_start_flg;
//int SysMode = RF_1M_BLE_MODE; 			//default mode is ble mode
int SysMode = RF_2M_2P4G_MODE; 			//default mode is ble mode
extern mouse_status_t mouse_status;
extern mouse_sleep_t mouse_sleep;

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////

#if(HID_MOUSE_ATT_ENABLE)

#define			HID_HANDLE_MOUSE_REPORT				24
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

//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xe1, 0xb1, 0xb2, 0xe8, 0xa3, 0xc7};

u8	tbl_advData[] = {
	 0x05, 0x09, 't', 'h', 'i', 'k',
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
	LED_AUDIO_ON,	//1
	LED_AUDIO_OFF,	//2
	LED_SHINE_SLOW, //3
	LED_SHINE_FAST, //4
	LED_SHINE_OTA, //5
};

const led_cfg_t led_cpi[] = {
		{500, 	500, 	1, 		0x04},
		{500, 	500, 	2, 		0x04},
		{500, 	500, 	3, 		0x04},
};

const led_cfg_t led_cfg[] = {
	    {1000,    0,      1,      0x00,	 },    //power-on, 1s on
	    {100,	  0 ,	  0xff,	  0x02,  },    //audio on, long on
	    {0,	      100 ,   0xff,	  0x02,  },    //audio off, long off
	    {500,	  500 ,   3,	  0x04,	 },    //1Hz for 3 seconds
	    {250,	  250 ,   6,	  0x04,  },    //2Hz for 3 seconds
	    {125,	  125 ,   200,	  0x08,  },    //4Hz for 50 seconds
};


/**
 * @brief     config the irq of uart tx and rx
 * @param[in] rx_irq_en - 1:enable rx irq. 0:disable rx irq
 * @param[in] tx_irq_en - 1:enable tx irq. 0:disable tx irq
 * @return    none
 */
void UART_NDmaIrqInit(unsigned char rx_irq_en,unsigned char tx_irq_en)
{
	if(rx_irq_en){
		REG_ADDR8(0x96) |= FLD_UART_RX_IRQ_EN;
	}else{
		REG_ADDR8(0x96) &= (~FLD_UART_RX_IRQ_EN);
	}

	if(tx_irq_en){
		REG_ADDR8(0x96)|= FLD_UART_TX_IRQ_EN;
	}else{
		REG_ADDR8(0x96) &= (~FLD_UART_TX_IRQ_EN);
	}
	//REG_IRQ_MASK |= FLD_IRQ_UART_EN;
}

/**
 * @brief     config the number level setting the irq bit of status register 0x9d
 *            ie 0x9d[3].
 *            If the cnt register value(0x9c[0,3]) larger or equal than the value of 0x99[0,3]
 *            or the cnt register value(0x9c[4,7]) less or equal than the value of 0x99[4,7],
 *            it will set the irq bit of status register 0x9d, ie 0x9d[3]
 * @param[in] rx_level - receive level value. ie 0x99[0,3]
 * @param[in] tx_level - transmit level value.ie 0x99[4,7]
 * @return    none
 */
void UART_NDmaIrqTrigLevel(unsigned char rx_level, unsigned char tx_level)
{
	REG_ADDR8(0x99) = rx_level | (tx_level<<4);
}


u32	advertise_begin_tick;

u8	ui_mic_enable = 0;


int lowBattDet_enable = 0;
void		ui_enable_mic (u8 en)
{
	ui_mic_enable = en;

	gpio_set_output_en (GPIO_PC3, en);		//AMIC Bias output
	gpio_write (GPIO_PC3, en);

	device_led_setup(led_cfg[en ? LED_AUDIO_ON : LED_AUDIO_OFF]);


	if(en){  //audio on
		lowBattDet_enable = 1;
		battery2audio();////switch auto mode
	}
	else{  //audio off
		audio2battery();////switch manual mode
		lowBattDet_enable = 0;
	}
}


extern kb_data_t	kb_event;


u8 key_buf[8] = {0};
u8 key_type;
#define CONSUMER_KEY   0
#define KEYBOARD_KEY   1


u8 sendTerminate_before_enterDeep = 0;

int key_not_released;
u32 latest_user_event_tick;
u8  user_task_flg;

static u8 key_voice_press = 0;
static u8 ota_is_working = 0;
static u32 key_voice_pressTick = 0;

#if (STUCK_KEY_PROCESS_ENABLE)
u32 stuckKey_keyPressTime;
#endif




#if (BLE_AUDIO_ENABLE)
void	task_audio (void)
{
	static u32 audioProcTick = 0;
	if(clock_time_exceed(audioProcTick, 5000)){
		audioProcTick = clock_time();
	}
	else{
		return;
	}

	///////////////////////////////////////////////////////////////
	log_event(TR_T_audioTask);
	proc_mic_encoder ();		//about 1.2 ms @16Mhz clock

	//////////////////////////////////////////////////////////////////
	if (bls_ll_getTxFifoNumber() < 8)
	{
		int *p = mic_encoder_data_buffer ();
		if (p)					//around 3.2 ms @16MHz clock
		{
			log_event (TR_T_audioData);
			bls_att_pushNotifyData (AUDIO_HANDLE_MIC, (u8*)p, ADPCM_PACKET_LEN);
		}
	}
}
#endif



#if (USER_TEST_BLT_SOFT_TIMER)
int gpio_test0(void)
{
	//gpio 0 toggle to see the effect
	//DBG_CHN0_TOGGLE;

	return 0;
}



int gpio_test1(void)
{
	//gpio 1 toggle to see the effect
	//DBG_CHN1_TOGGLE;

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
	//DBG_CHN2_TOGGLE;

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

	if(ui_mic_enable){
		ui_enable_mic (0);
	}

	advertise_begin_tick = clock_time();

}

void	task_connect (u8 e, u8 *p, int n)
{

	bls_l2cap_requestConnParamUpdate (6, 6, 99, 400);   //10ms *(99+1) = 1000 ms

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

void deepback_post_proc(void)
{
	//manual key release
	if(deepback_key_state == DEEPBACK_KEY_WAIT_RELEASE && clock_time_exceed(deepback_key_tick,150000)){
		key_not_released = 0;

		key_buf[2] = 0;
		bls_att_pushNotifyData (HID_HANDLE_KEYBOARD_REPORT, key_buf, 8); //release
		deepback_key_state = DEEPBACK_KEY_IDLE;
	}
}



void key_change_proc(void)
{

	latest_user_event_tick = clock_time();  //record latest key change time

	if(key_voice_press){  //clear voice key press flg
		key_voice_press = 0;
	}

	u8 key = kb_event.keycode[0];

	if ( (key & 0xf0) == 0xf0)			//key in consumer report
	{
		key_not_released = 1;
		key_type = CONSUMER_KEY;

		u16 media_key;
		if(key == CR_VOL_UP){
			media_key= 0x0001;  //vol+
		}
		else if(key == CR_VOL_DN){
			media_key = 0x0002; //vol-
		}
		else{
			media_key = 1 << (key & 0x0f);
		}

		bls_att_pushNotifyData (HID_HANDLE_CONSUME_REPORT, (u8 *)&media_key, 2);
	}
	else if (key)			// key in standard reprot
	{
		key_not_released = 1;
#if (BLE_AUDIO_ENABLE)
		if (key == VK_M)
		{
			if(ui_mic_enable){
				//adc_clk_powerdown();
				ui_enable_mic (0);
			}
			else{ //if voice not on, mark voice key press tick
				key_voice_press = 1;
				key_voice_pressTick = clock_time();
			}
		}
		else
#endif
		{
			key_type = KEYBOARD_KEY;
			key_buf[2] = key;
			bls_att_pushNotifyData (HID_HANDLE_KEYBOARD_REPORT, key_buf, 8);
		}
	}
	else {
		key_not_released = 0;
		if(key_type == CONSUMER_KEY){
			u16 media_key = 0;
			bls_att_pushNotifyData (HID_HANDLE_CONSUME_REPORT, (u8 *)&media_key, 2);  //release
		}
		else{
			key_buf[2] = 0;
			bls_att_pushNotifyData (HID_HANDLE_KEYBOARD_REPORT, key_buf, 8); //release
		}
	}
}



void proc_mouse(u8 e, u8 *p, int n)
{

	/***************  °´¼ü¼ì²â      *****************/

	//clear x,y

	ble_swith_time_thresh = (blc_ll_getCurrentState() == BLS_LINK_STATE_ADV) ? BLE_ADV_MODE_SWITCH_CNT  : BLE_CON_MODE_SWITCH_CNT;

	gpio_write(mouse_status.hw_define->sensor_int, 1);
	static u32 ValueChange = 0;

	ValueChange = mouse_button_detect(&mouse_status, MOUSE_BTN_LOW);

	//sensor¼ì²â

#if(MOUSE_WHEEL_MODULE_EN)
	u32 wheel_prepare_tick = mouse_wheel_prepare_tick();
#endif

#if(MOUSE_SENSOR_MODULE_EN)
	mouse_status.data->x = 0;
	mouse_status.data->y = 0;
	mouse_sensor_data( &mouse_status );
#endif

#if(MOUSE_WHEEL_MODULE_EN)
	mouse_wheel_process(&mouse_status, wheel_prepare_tick);
#endif


#if(MOUSE_BTN_MODULE_EN)
	mouse_button_process(&mouse_status);
#endif

	//start to switch mode to 2.4G, then release button
	if(switch_mode_start_flg ){

		bls_att_pushNotifyData (HID_HANDLE_MOUSE_REPORT, mouse_relsease_buff, 4);
	}
	else{
		if(*(u32 *)(mouse_status.data)){
			latest_user_event_tick = clock_time();
			bls_att_pushNotifyData (HID_HANDLE_MOUSE_REPORT, mouse_status.data, 4);
		}
		else if(ValueChange){
			bls_att_pushNotifyData (HID_HANDLE_MOUSE_REPORT, mouse_relsease_buff, 4);
		}
	}




	//¹öÂÖ¼ì²â

}

void proc_keyboard (u8 e, u8 *p, int n)
{

	static u32 keyScanTick = 0;
	if(e == BLT_EV_FLAG_GPIO_EARLY_WAKEUP || clock_time_exceed(keyScanTick, 10000)){
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
	


#if (BLE_AUDIO_ENABLE)
	 //long press voice 1 second
	if(key_voice_press && !ui_mic_enable && clock_time_exceed(key_voice_pressTick,1000000)){
		key_voice_press = 0;
		ui_enable_mic (1);
	}
#endif
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



extern u32	scan_pin_need;
void blt_pm_proc(void)
{
#if(BLE_REMOTE_PM_ENABLE)
	if(ota_is_working || ui_mic_enable){
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
	}
	else{

		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);

		user_task_flg = scan_pin_need || key_not_released || DEVICE_LED_BUSY;

		if(user_task_flg){
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
			analog_write(DEEP_ANA_REG0, CONN_DEEP_FLG);
			//device_info_save(mouse_status, 0);
		}

		//adv 60s, deepsleep
		if( blc_ll_getCurrentState() == BLS_LINK_STATE_ADV && \
			clock_time_exceed(advertise_begin_tick , 60 * 1000000)){
			bls_pm_setSuspendMask (DEEPSLEEP_ADV); //set deepsleep
			bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio PAD wakeup deesleep
			analog_write(DEEP_ANA_REG0, ADV_DEEP_FLG);
			//device_info_save(mouse_status, 0);

			mouse_sleep.sensor_sleep = 1;
			mouse_status.mouse_sensor = SENSOR_MODE_WORKING;
		    mouse_sensor_sleep_wakeup( &mouse_status.mouse_sensor, &mouse_sleep.sensor_sleep, 0 );
		}
		//conn 60s no event(key/voice/led), enter deepsleep
		else if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && !user_task_flg && \
				clock_time_exceed(latest_user_event_tick, 60 * 1000000) ){

			bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //push terminate cmd into ble TX buffer
			sendTerminate_before_enterDeep = 1;

			mouse_sleep.sensor_sleep = 1;

			//device_info_save(mouse_status, 0);
			mouse_status.mouse_sensor = SENSOR_MODE_WORKING;
		    mouse_sensor_sleep_wakeup( &mouse_status.mouse_sensor, &mouse_sleep.sensor_sleep, 0 );
		}
#endif



	}

#endif
}


_attribute_ram_code_ void  ble_remote_set_sleep_wakeup (u8 e, u8 *p, int n)
{
	if( blc_ll_getCurrentState() == BLS_LINK_STATE_CONN && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 * CLOCK_SYS_CLOCK_1MS){  //suspend time > 30ms.add gpio wakeup
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);  //gpio CORE wakeup suspend
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



signed char rx_buff[16];
int bls_uart_handler(u8 *p, u8 n ){

	if(n > 4){
		return 0;
	}
	else{

		mouse_status.data->btn = p[0];
		mouse_status.data->x = p[1];
		mouse_status.data->y = p[2];
		mouse_status.data->wheel = p[3];

	}

}

int rx_from_uart_cb (void)//UART data send to Master,we will handler the data as CMD or DATA
{
	if(my_fifo_get(&hci_rx_fifo) == 0)
	{
		return 0;
	}

	u8* p = my_fifo_get(&hci_rx_fifo);
	u32 rx_len = p[0]; //usually <= 255 so 1 byte should be sufficient

	if (rx_len)
	{
		bls_uart_handler(&p[4], rx_len);
		my_fifo_pop(&hci_rx_fifo);
	}

	return 1;
}

int tx_to_uart_cb()
{

}


/** BLe mode **/
void ble_user_init()
{
	rf_customized_param_load();  //load customized freq_offset cap value and tp value

	//for USB debug
	//usb_log_init ();
	//usb_dp_pullup_en (1);  //open USB enum


	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff){
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



	///////////////////// USER application initialization ///////////////////
	bls_ll_setAdvData( tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData(tbl_scanRsp, sizeof(tbl_scanRsp));

	u8 status = bls_ll_setAdvParam( 32, ADV_INTERVAL_30MS, \
									 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
									 0,  NULL,  BLT_ENABLE_ADV_37, ADV_FP_NONE);
	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}
	bls_ll_setAdvEnable(1);  //adv enable
	rf_set_power_level_index (RF_POWER_8dBm);

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);
	bls_app_registerEventCallback (BLT_EV_FLAG_CHN_MAP_UPDATE, &update_done);
	bls_app_registerEventCallback (BLT_EV_FLAG_CONN_PARA_UPDATE, &update_done);


	///////////////////// keyboard matrix initialization, Ble mouse don't use the key///////////////////
//	u32 pin[] = KB_DRIVE_PINS;
//	for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
//	{
//		gpio_set_wakeup(pin[i],1,1);  	   //drive pin core(gpio) high wakeup suspend
//		cpu_set_gpio_wakeup (pin[i],1,1);  //drive pin pad high wakeup deepsleep
//	}
	gpio_set_wakeup(mouse_status.hw_define->sensor_int, 0, 1);
	cpu_set_gpio_wakeup(mouse_status.hw_define->sensor_int, 0, 1);  //PAD wakeup enable when wakeup sensor

#if(KEYSCAN_IRQ_TRIGGER_MODE)
	reg_irq_src = FLD_IRQ_GPIO_EN;
#endif

	//bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_keyboard);
	bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_mouse);


	///////////////////// AUDIO initialization///////////////////
#if (BLE_AUDIO_ENABLE)
	//buffer_mic set must before audio_init !!!
	config_mic_buffer ((u32)buffer_mic, TL_MIC_BUFFER_SIZE);

	#if (BLE_DMIC_ENABLE)  //Dmic config
		/////////////// DMIC: PA0-data, PA1-clk, PA3-power ctl
		gpio_set_func(GPIO_PA0, AS_DMIC);
		*(volatile unsigned char  *)0x8005b0 |= 0x01;  //PA0 as DMIC_DI
		gpio_set_func(GPIO_PA1, AS_DMIC);

		gpio_set_func(GPIO_PA3, AS_GPIO);
		gpio_set_input_en(GPIO_PA3, 1);
		gpio_set_output_en(GPIO_PA3, 1);
		gpio_write(GPIO_PA3, 0);

		Audio_Init(1, 0, DMIC, 26, 4, R64|0x10);  //16K
		Audio_InputSet(1);
	#else  //Amic config
		//////////////// AMIC: PC3 - bias; PC4/PC5 - input
		#if TL_MIC_32K_FIR_16K
			#if (CLOCK_SYS_CLOCK_HZ == 16000000)
				Audio_Init( 1, 0, AMIC, 47, 4, R2|0x10);
			#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
				Audio_Init( 1,0, AMIC,30,16,R2|0x10);
			#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
				Audio_Init( 1, 0, AMIC, 65, 15, R3|0x10);
			#endif
		#else
			#if (CLOCK_SYS_CLOCK_HZ == 16000000)
				Audio_Init( 1,0, AMIC,18,8,R5|0x10);
			#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
				Audio_Init( 1,0, AMIC,65,15,R3|0x10);
			#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
				Audio_Init( 1,0, AMIC,65,15,R6|0x10);
			#endif
		#endif
	#endif

	Audio_FineTuneSampleRate(3);//reg0x30[1:0] 2 bits for fine tuning, divider for slow down sample rate
	Audio_InputSet(1);//audio input set, ignore the input parameter
#endif
	adc_BatteryCheckInit(2);///add by Q.W


	///////////////////// Power Management initialization///////////////////
#if(BLE_REMOTE_PM_ENABLE)
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif


	////////////////LED initialization /////////////////////////
	device_led_init(GPIO_LED, 1);


	//////////////// TEST  /////////////////////////
#if (USER_TEST_BLT_SOFT_TIMER)
	blt_soft_timer_init();
	blt_soft_timer_add(&gpio_test0, 23000);
	blt_soft_timer_add(&gpio_test1, 7000);
	blt_soft_timer_add(&gpio_test2, 13000);
	blt_soft_timer_add(&gpio_test3, 27000);

#endif


	//mark adv begin time
	advertise_begin_tick = clock_time();
}

void user_init(){


	mouse_hw_init();


	if(SysMode == RF_2M_2P4G_MODE){
		normal_user_init();
	}
	else{
		ble_user_init();
	}

#if(UART_INIT_EN)

	gpio_set_input_en(GPIO_PC2, 1);
	gpio_set_input_en(GPIO_PC3, 1);
	gpio_setup_up_down_resistor(GPIO_PC2, PM_PIN_PULLUP_1M);
	gpio_setup_up_down_resistor(GPIO_PC3, PM_PIN_PULLUP_1M);
	uart_io_init(UART_GPIO_8267_PC2_PC3);

	//reg_dma_rx_rdy0 = FLD_DMA_UART_RX | FLD_DMA_UART_TX; //clear uart rx/tx status
	//CLK16M_UART115200;
	CLK16M_UART9600;
	uart_BuffInit(hci_rx_fifo_b, hci_rx_fifo.size, hci_tx_fifo_b);
//		blc_register_hci_handler (blc_rx_from_uart, blc_hci_tx_to_uart);		//default handler

	blc_register_hci_handler(rx_from_uart_cb,tx_to_uart_cb);				//customized uart handler


#endif
}


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
u32 tick_loop;
unsigned short battValue[20];


void main_loop ()
{
	tick_loop ++;

	if(SysMode == RF_2M_2P4G_MODE ){
		mouse_main_loop();
	}
	else{
		////////////////////////////////////// BLE entry /////////////////////////////////
		#if (BLT_SOFTWARE_TIMER_ENABLE)
			blt_soft_timer_process(MAINLOOP_ENTRY);
		#endif

		blt_sdk_main_loop ();


		////////////////////////////////////// UI entry /////////////////////////////////
		#if (BLE_AUDIO_ENABLE)
			if(ui_mic_enable){  //audio
				task_audio();
			}
		#endif

		//proc_keyboard (0,0,0);  //key scan
		proc_mouse(0,0,0);

		device_led_process();  //led management

		blt_pm_proc();  //power management
	}

}







#endif  //end of  (__PROJECT_8261_MULTI_MOUSE__)
