#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/ll_whitelist.h"
#include "../../proj/drivers/keyboard.h"
#include "../common/tl_audio.h"
#include "../common/blt_led.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/audio.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"

#if (__PROJECT_8267_BLE_RC_DEMO__)


#define 	ADV_IDLE_ENTER_DEEP_TIME		10  //60 s
#define 	CONN_IDLE_ENTER_DEEP_TIME		10  //60 s

MYFIFO_INIT(hci_tx_fifo, 72, 4);
////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////
//		handle 0x0e: consumper report
#if HID_MOUSE_ATT_ENABLE	//initial the define is closed

#define 		HID_HANDLE_MOUSE_REPORT				24
#define			HID_HANDLE_CONSUME_REPORT			28
#define			HID_HANDLE_KEYBOARD_REPORT			32
#define			AUDIO_HANDLE_MIC					50

#else 
#define			HID_HANDLE_CONSUME_REPORT			21
#define			HID_HANDLE_KEYBOARD_REPORT			25
#define			AUDIO_HANDLE_MIC					43
#endif 

u16 BattValue[10] = {0};

//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};

u8	tbl_advData[] = {
	 0x05, 0x09, 't', 'h', 'i', 'd',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
	 0x03, 0x19, 0x80, 0x01, 					// 384, Generic Remote Control, Generic category
	 0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,		// incomplete list of service class UUIDs (0x1812, 0x180F)
};

u8	tbl_scanRsp [] = {
		 0x08, 0x09, 't', 'R', 'e', 'm', 'o', 't', 'e',
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

const led_cfg_t led_cfg[] = {
	    {1000,    0,      1,      0x00,	 },    //power-on, 1s on
	    {100,	  0 ,	  0xff,	  0x02,  },    //audio on, long on
	    {0,	      100 ,   0xff,	  0x02,  },    //audio off, long off
	    {500,	  500 ,   2,	  0x04,	 },    //1Hz for 3 seconds
	    {250,	  250 ,   4,	  0x04,  },    //2Hz for 3 seconds
	    {125,	  125 ,   200,	  0x08,  },    //4Hz for 50 seconds
};



extern kb_data_t	kb_event;

u32		advertise_begin_tick;

u8		ui_mic_enable = 0;
int 	lowBattDet_enable = 0;

///////////////////// key mode //////////////////////
#define KEY_MODE_BLE	   		0    //ble key
#define KEY_MODE_IR        		1    //ir  key

u8 		user_key_mode = KEY_MODE_BLE;


//////////////////// key type ///////////////////////
#define IDLE_KEY	   			0
#define CONSUMER_KEY   	   		1
#define KEYBOARD_KEY   	   		2
#define IR_KEY   	   			3

u8 		key_type;

//////////////////// ir key /////////////////////////
//ir key
#define TYPE_IR_SEND			1
#define TYPE_IR_RELEASE			2




u8 		key_buf[8] = {0};

int 	key_not_released;

int 	ir_not_released;

u32 	latest_user_event_tick;

u8 		user_task_flg;
u8 		sendTerminate_before_enterDeep = 0;
u8 		ota_is_working = 0;
u8 		key_voice_press = 0;

u32 	key_voice_pressTick = 0;


#if (STUCK_KEY_PROCESS_ENABLE)
u32 	stuckKey_keyPressTime;
#endif

static const u8 kb_map_ble[49] = 	KB_MAP_BLE;  //7*7
static const u8 kb_map_ir[49] = 	KB_MAP_IR;   //7*7



signed char mouse_data_add[4]={0,20,20,0};
signed char mouse_data_minus[4]={0,-20,-20,0};



void		ui_enable_mic (u8 en)
{
	ui_mic_enable = en;

	//AMIC Bias output
	gpio_set_output_en (GPIO_AMIC_BIAS, en);
	gpio_write (GPIO_AMIC_BIAS, en);

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



void	task_audio (void)
{
	static u32 audioProcTick = 0;
	if(clock_time_exceed(audioProcTick, 5000)){
		audioProcTick = clock_time();
	}
	else{
		return;
	}

	//if (!ui_mic_enable || blt_busy)
	if (!ui_mic_enable)
	{
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
	bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);  //interval=10ms latency=99 timeout=4s

	latest_user_event_tick = clock_time();
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


	u8 key0 = kb_event.keycode[0];
	//u8 key1 = kb_event.keycode[1];

	u8 key_value;

	key_not_released = 1;
	if (kb_event.cnt == 2){  //two key press, do  not process

	}
	else if(kb_event.cnt == 1){
		if(key0 == RF_SWITCH){
			user_key_mode = !user_key_mode;
			device_led_setup(led_cfg[LED_SHINE_SLOW + user_key_mode]);
		}
		else if (key0 == VOICE)
		{
			if(ui_mic_enable){  //if voice on, voice off
				//adc_clk_powerdown();
				ui_enable_mic (0);
			}
			else{ //if voice not on, mark voice key press tick
				key_voice_press = 1;
				key_voice_pressTick = clock_time();
			}
		}
		else if(user_key_mode == KEY_MODE_BLE){
			key_value = kb_map_ble[key0];
#if (1)  //report Vol+ Vol- to  standard ble master(mobile phone/TV box)
			if(key_value == VK_VOL_DN || key_value == VK_VOL_UP){
				key_type = CONSUMER_KEY;
				key_buf[0] = key_value == VK_VOL_UP ? 0x01 : 0x02;
				bls_att_pushNotifyData (HID_HANDLE_CONSUME_REPORT, key_buf, 2);
			}
			else
#endif
			{
				key_type = KEYBOARD_KEY;
				key_buf[2] = key_value;
				bls_att_pushNotifyData (HID_HANDLE_KEYBOARD_REPORT, key_buf, 8);
			}

		}
		else if(user_key_mode == KEY_MODE_IR){  //IR mode
			key_value = kb_map_ir[key0];
			key_type = IR_KEY;
			if(!ir_not_released){
				//ir_dispatch(TYPE_IR_SEND, 0x88, key_value);
				ir_not_released = 1;
			}
		}
		else{
			key_type = IDLE_KEY;
		}
	}
	else{  //kb_event.cnt == 0,  key release
		key_not_released = 0;
		if(key_type == CONSUMER_KEY){
			key_buf[0] = 0;
			bls_att_pushNotifyData (HID_HANDLE_CONSUME_REPORT, key_buf, 2);  //release
		}
		else if(key_type == KEYBOARD_KEY){
			key_buf[2] = 0;
			bls_att_pushNotifyData (HID_HANDLE_KEYBOARD_REPORT, key_buf, 8); //release
		}
		else if(key_type == IR_KEY){
			if(ir_not_released){
				ir_not_released = 0;
				//ir_dispatch(TYPE_IR_RELEASE, 0, 0);  //release
			}
		}
	}


}



void proc_keyboard (u8 e, u8 *p, int n)
{

	static u32 keyScanTick = 0;
	if(e == BLT_EV_FLAG_GPIO_EARLY_WAKEUP || clock_time_exceed(keyScanTick, 8000)){
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
	

	 //long press voice 1 second
	if(key_voice_press && !ui_mic_enable && clock_time_exceed(key_voice_pressTick,1000000)){
		key_voice_press = 0;
		ui_enable_mic (1);
	}
}


extern u32	scan_pin_need;
void blt_pm_proc(void)
{
#if(BLE_REMOTE_PM_ENABLE)
	if(ota_is_working || ui_mic_enable){
		//bls_pm_setSuspendMask(SUSPEND_DISABLE);
		bls_pm_setSuspendMask (LOWPOWER_IDLE);
	}
	else{

		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);

		user_task_flg = scan_pin_need || key_not_released || DEVICE_LED_BUSY;

		if(user_task_flg){
			#if (LONG_PRESS_KEY_POWER_OPTIMIZE)
				extern int key_matrix_same_as_last_cnt;
				if(key_matrix_same_as_last_cnt > 5){  //key matrix stable can optize
					bls_pm_setManualLatency(4);
				}
				else{
					bls_pm_setManualLatency(0);  //latency off: 0
				}
			#else
				bls_pm_setManualLatency(0);
			#endif
		}

#if 0 //deepsleep
		if(sendTerminate_before_enterDeep == 1){ //sending Terminate and wait for ack before enter deepsleep
			if(user_task_flg){  //detect key Press again,  can not enter deep now
				sendTerminate_before_enterDeep = 0;
			}
		}
		else if(sendTerminate_before_enterDeep == 2){  //Terminate OK
			bls_pm_setSuspendMask (DEEPSLEEP_ADV); //when terminate, link layer change back to adc state
			bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio PAD wakeup deesleep
			analog_write(DEEP_ANA_REG0, CONN_DEEP_FLG);
			analog_write(DEEP_ANA_REG1, user_key_mode);
		}

		//adv 60s, deepsleep
		if( bls_ll_getCurrentState() == BLS_LINK_STATE_ADV && \
			clock_time_exceed(advertise_begin_tick , ADV_IDLE_ENTER_DEEP_TIME * 1000000)){

			bls_pm_setSuspendMask (DEEPSLEEP_ADV); //set deepsleep
			bls_pm_setWakeupSource(PM_WAKEUP_PAD);  //gpio PAD wakeup deesleep
			analog_write(DEEP_ANA_REG0, ADV_DEEP_FLG);
			analog_write(DEEP_ANA_REG1, user_key_mode);
		}
		//conn 60s no event(key/voice/led), enter deepsleep
		else if( bls_ll_getCurrentState() == BLS_LINK_STATE_CONN && !user_task_flg && \
				clock_time_exceed(latest_user_event_tick, CONN_IDLE_ENTER_DEEP_TIME * 1000000) ){

			bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN); //push terminate cmd into ble TX buffer
			sendTerminate_before_enterDeep = 1;
		}
#endif


	}

#endif  //END of  BLE_REMOTE_PM_ENABLE
}

void blt_system_power_optimize(void)  //to lower system power
{
	//disable_unnecessary_module_clock
	reg_rst_clk0 &= ~FLD_RST_SPI;  //spi not use
	reg_rst_clk0 &= ~FLD_RST_I2C;  //iic not use
#if(!MODULE_USB_ENABLE) //if usb not use
	reg_rst_clk0 &= ~(FLD_RST_USB | FLD_RST_USB_PHY);
#endif

#if(!BLE_REMOTE_UART_ENABLE) //if uart not use
	reg_clk_en1 &= ~(FLD_CLK_UART_EN);
#endif

}


//_attribute_ram_code_
void  ble_remote_set_sleep_wakeup (u8 e, u8 *p, int n)
{
	if( bls_ll_getCurrentState() == BLS_LINK_STATE_CONN && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 * CLOCK_SYS_CLOCK_1MS){  //suspend time > 30ms.add gpio wakeup
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
}


void user_init()
{

#if 0  //debug GPIO wakeup deep problem
	DBG_CHN3_HIGH;
	sleep_us(10000);
	//while(1);
	DBG_CHN3_LOW;
//	cpu_set_gpio_wakeup(GPIO_PA6, 1, 1);
//	cpu_set_gpio_wakeup(GPIO_PA7, 1, 1);
//	cpu_set_gpio_wakeup(GPIO_PB1, 1, 1);
//	cpu_set_gpio_wakeup(GPIO_PB4, 1, 1);
//	cpu_set_gpio_wakeup(GPIO_PB5, 1, 1);
	cpu_set_gpio_wakeup(GPIO_PB6, 1, 1);  //B6 ERR
//	cpu_set_gpio_wakeup(GPIO_PB7, 1, 1);

	while(1){

		sleep_us(10000);

		DBG_CHN3_HIGH;
		cpu_sleep_wakeup(0, PM_WAKEUP_TIMER, clock_time() + 10*CLOCK_SYS_CLOCK_1MS);
		DBG_CHN3_LOW;

		if(clock_time_exceed(0, 3000000)){
			DBG_CHN3_HIGH;
			sleep_us(10000);
			//while(1);
			DBG_CHN3_LOW;

			cpu_sleep_wakeup(1, PM_WAKEUP_PAD, 0);

		}
	}
#endif


	rf_customized_param_load();  //load customized freq_offset cap value and tp value

	/////////// USB init //////////////////
	//usb_log_init ();
	//usb_dp_pullup_en (1);  //open USB enum

	////////////////// BLE slave initialization ////////////////////////////////////
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff)
	{
	    memcpy (tbl_mac, pmac, 6);
	}
    else
    {
        //TODO : should write mac to flash after pair OK
        tbl_mac[0] = (u8)rand();
        flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
    }

	rf_set_power_level_index (RF_POWER_8dBm);

	bls_ll_init (tbl_mac);
	bls_ll_setAdvData( tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData(tbl_scanRsp, sizeof(tbl_scanRsp));

    //NOTE: my_att_init  must after bls_ll_init, and before bls_ll_setAdvParam
	extern void my_att_init ();
	my_att_init ();

	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER);

	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS, \
			 	 	 	 	 	     ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
			 	 	 	 	 	     0,  NULL,  BLT_ENABLE_ADV_37, ADV_FP_NONE);
	if(status != BLE_SUCCESS){
		//....
	}

	bls_ll_setAdvEnable(1);  //adv enable


	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);
	bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_keyboard);

	blc_l2cap_register_handler (blc_l2cap_packet_receive);



	////////////////// PM initialization ////////////////////////////////////
	// keyboard drive/scan line configuration
	u32 pin[] = KB_DRIVE_PINS;
	for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
	{
		gpio_set_wakeup(pin[i],1,1);  	   //drive pin core(gpio) high wakeup suspend
		cpu_set_gpio_wakeup (pin[i],1,1);  //drive pin pad high wakeup deepsleep

	}
	gpio_core_wakeup_enable_all(1);


#if(KEYSCAN_IRQ_TRIGGER_MODE)
	gpio_core_irq_enable_all(1);
	reg_irq_src = FLD_IRQ_GPIO_EN;
#endif


#if(BLE_REMOTE_PM_ENABLE)
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &ble_remote_set_sleep_wakeup);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif



	////////////////// Audio initialization ////////////////////////////////////
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

	adc_BatteryCheckInit(2);



	/////////////////////////////////////////////////////////////////

	device_led_init(GPIO_LED, 1);

	advertise_begin_tick = clock_time();

	user_key_mode = analog_read(DEEP_ANA_REG1);
}


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
u32 tick_loop;
unsigned short battValue[20];

extern u8	blt_slave_main_loop (void);
void main_loop ()
{
	tick_loop ++;

	////////////////////////////////////// BLE entry /////////////////////////////////

	blt_slave_main_loop ();

	////////////////////////////////////// UI entry /////////////////////////////////

	//if(bls_pm_isRxTimingAligned() )

	task_audio();

	proc_keyboard (0,0, 0);

	device_led_process();

	blt_pm_proc();
}




#endif  //end of __PROJECT_8267_BLE_RC_DEMO__
