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
#include "../../proj/drivers/uart.h"


#if (__PROJECT_8267_MCU_RC__)

MYFIFO_INIT(uart_tx_fifo, 72, 4);

#define 	SPP_CMD_SET_ADV_ENABLE					0xFF0A
#define 	SPP_CMD_SEND_NOTIFY_DATA                0xFF0B

typedef struct {
	u16 	   cmd;
	u16		   cmdLen;
	u8         data[32];
} module_cmd_t;

module_cmd_t * module_p;

typedef struct {
	u16 	   cmd;
	u16		   cmdLen;
	u16 	   handle;
	u8         data[30];
} module_notify_t;




#define SET_DBG_FLG(x)  do{ write_reg8(0x8000, x);while(1);}while(0)



#define			HID_HANDLE_CONSUME_REPORT			21
#define			HID_HANDLE_KEYBOARD_REPORT			25
#define			AUDIO_HANDLE_MIC					43
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

u8		ui_mic_enable = 0;


//////////////////// key type ///////////////////////
#define IDLE_KEY	   			0
#define CONSUMER_KEY   	   		1
#define KEYBOARD_KEY   	   		2
#define IR_KEY   	   			3

u8 		key_type;

u8 		key_buf[8] = {0};
int 	key_not_released;
u8 		user_task_flg;
u8 		ota_is_working = 0;
u8 		key_voice_press = 0;
u32 	key_voice_pressTick = 0;



int 	lowBattDet_enable;
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
			//bls_att_pushNotifyData (AUDIO_HANDLE_MIC, (u8*)p, ADPCM_PACKET_LEN);
		}
	}
}



void 	ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{

	if(ui_mic_enable){
		ui_enable_mic (0);
	}

}



module_notify_t kb_notify_cmd = {
		SPP_CMD_SEND_NOTIFY_DATA,
		10,
		HID_HANDLE_KEYBOARD_REPORT,
		0,
};

void key_change_proc(void)
{

	if(key_voice_press){  //clear voice key press flg
		key_voice_press = 0;
	}


	u8 key0 = kb_event.keycode[0];
	//u8 key1 = kb_event.keycode[1];

	key_not_released = 1;
	if (kb_event.cnt == 2){  //two key press, do  not process

	}
	else if(kb_event.cnt == 1){

		if (key0 == VOICE)
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
		else{
			if(key0 == VK_VOL_DN || key0 == VK_VOL_UP){
				key_type = CONSUMER_KEY;
				key_buf[0] == VK_VOL_UP ? 0x01 : 0x02;
				//bls_att_pushNotifyData (HID_HANDLE_CONSUME_REPORT, key_buf, 2);
			}
			else if(key0 == RED_KEY){
				static u8 adv_en;
				adv_en = !adv_en;

				//enable/disable advertising: 0a ff 01 00  01
				u8 adv_en_cmd[5];
				module_cmd_t *pCmd = (module_cmd_t *)adv_en_cmd;

				pCmd->cmd = SPP_CMD_SET_ADV_ENABLE;
				pCmd->cmdLen = 2;
				pCmd->data[0] = adv_en;

				if( my_fifo_push(&uart_tx_fifo, adv_en_cmd, 5) ){ // 0 is OK
					SET_DBG_FLG(0x11);
				}

				device_led_setup(led_cfg[LED_SHINE_FAST]);
			}
			else
			{
				if( key0 >= VK_1 && key0 <= VK_0){
					key_type = KEYBOARD_KEY;

					kb_notify_cmd.data[2] = key0;
					if( my_fifo_push(&uart_tx_fifo, &kb_notify_cmd, 14) ){ // 0 is OK
						SET_DBG_FLG(0x11);
					}
				}
			}
		}
	}
	else{  //kb_event.cnt == 0,  key release
		key_not_released = 0;
		if(key_type == CONSUMER_KEY){
			key_buf[0] = 0;
			//bls_att_pushNotifyData (HID_HANDLE_CONSUME_REPORT, key_buf, 2);  //release
		}
		else if(key_type == KEYBOARD_KEY){
			kb_notify_cmd.data[2] = 0;
			if( my_fifo_push(&uart_tx_fifo, &kb_notify_cmd, 14) ){ // 0 is OK
				SET_DBG_FLG(0x11);
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



unsigned char my_rx_uart_r_index = 0;
unsigned char my_rx_uart_w_index = 0;
uart_data_t my_txdata_buf = {0,};
uart_data_t my_rxdata_use = {0,};
uart_data_t my_rxdata_buf[2] = {{0,}};   // data max 252, user must copy rxdata to other Ram,but not use directly


void user_init()
{
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



	////////////////// PM initialization ////////////////////////////////////
	// keyboard drive/scan line configuration
	u32 pin[] = KB_DRIVE_PINS;
	for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
	{
		gpio_set_wakeup(pin[i],1,1);  	   //drive pin core(gpio) high wakeup suspend
		cpu_set_gpio_wakeup (pin[i],1,1);  //drive pin pad high wakeup deepsleep

	}
	gpio_core_wakeup_enable_all(1);


	device_led_init(GPIO_LED, 1);
	device_led_setup( led_cfg[LED_POWER_ON]);



	// UART init
	my_rx_uart_r_index = 0;
	my_rx_uart_w_index = 0;

	gpio_set_func(GPIO_UTX, AS_UART);
	gpio_set_func(GPIO_URX, AS_UART);
	gpio_set_input_en(GPIO_UTX, 1);
	gpio_set_input_en(GPIO_URX, 1);
	gpio_write (GPIO_UTX, 1);			//pull-high RX to avoid mis-trig by floating signal
	gpio_write (GPIO_URX, 1);			//pull-high RX to avoid mis-trig by floating signal

	UART_GPIO_CFG_PB2_PB3();

	CLK16M_UART115200;			//todo:change to certain configuration according to lib
	uart_BuffInit((u8 *)(&my_rxdata_buf), sizeof(my_rxdata_buf), (u8 *)(&my_txdata_buf));

	reg_dma_rx_rdy0 = FLD_DMA_UART_RX | FLD_DMA_UART_TX;//CLR irq source
}


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
int app_packet_from_uart (void)
{
	if(my_rx_uart_w_index == my_rx_uart_r_index)  //rx buff empty
		return 0;


	u8	rcvLen = my_rxdata_buf[my_rx_uart_r_index].len + 4;
	u32 rx_len = rcvLen > sizeof(my_rxdata_use) ? sizeof(my_rxdata_use) : rcvLen;
	memcpy(&my_rxdata_use, &my_rxdata_buf[my_rx_uart_r_index], rx_len);


	if (rx_len > 4)
	{
		u8 *p = my_rxdata_use.data;


	}


	my_rx_uart_r_index = (my_rx_uart_r_index + 1)&0x01;
	return 0;
}


int app_packet_to_uart ()
{
	u8 *p = my_fifo_get (&uart_tx_fifo);
	if (p && !uart_tx_is_busy ())
	{
		memcpy(&my_txdata_buf.data, p + 2, p[0]+p[1]*256);
		my_txdata_buf.len = p[0]+p[1]*256 ;

		if (uart_Send((u8 *)(&my_txdata_buf)))
		{
			my_fifo_pop (&uart_tx_fifo);
		}
	}
	return 0;
}



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


u32 tick_loop;

extern u8	blt_slave_main_loop (void);
void main_loop ()
{
	tick_loop ++;

	//UART entry
	app_packet_from_uart ();
	app_packet_to_uart ();


	//UI entry
	task_audio();

	proc_keyboard (0,0, 0);

	device_led_process();

	blt_pm_proc();
}




#endif  //end of __PROJECT_8267_BLE_RC_DEMO__
