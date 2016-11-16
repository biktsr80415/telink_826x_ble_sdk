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
#define 	SPP_CMD_CONN_PARAM_UPDATE				0xFF15
#define 	SPP_CMD_SEND_NOTIFY_DATA                0xFF1C

#define 	SPP_EVENT_CONNECT						0x0730
#define 	SPP_EVENT_DISCONNECT                	0x0731

typedef struct {
	u16 	   cmd;
	u16		   cmdLen;
	u8         data[32];
} mcu_cmd_t;

mcu_cmd_t * mcu_p;

typedef struct {
	u16 	   cmd;
	u16		   cmdLen;
	u16 	   handle;
	u8         data[30];
} module_notify_t;


u16 cur_mcu_cmd;


typedef struct {
	u8 	   token;
	u8	   dataLen;  //cmd + dat
	u16    event;
	u8 	   dat[32];
} module_event_t;


int module_evt_err = 0;

int module_evt_unmatch = 0;

u16 module_evt_rcv;
u16 moduel_evt_expect;

int	mcu_uart_data_flg;
int	mcu_cmd_no_ack;
u32 mcu_wakeup_module_tick;


#define SET_DBG_FLG(x)  do{ write_reg8(0x8000, x);while(1);}while(0)


void push_uart_data_to_fifo (u8 *p, u8 n)
{

#if (REMOTE_PM_ENABLE)
/*
	mcu 往module发UART数据时，将GPIO_WAKEUP_MODULE管脚拉高，通知module有数据了（同时也可以将module从低功耗唤醒）
	并且记录当前时间点mcu_wakeup_module_tick，硬件uart发数据时，要在此时间点之后2500us，因为有可能拉高时module正好
	处于suspend，唤醒后扔需要2500us时间来回复稳定，保证硬件uart接收到正确数据
*/

	if(!mcu_uart_data_flg){ //UART上空闲，新的数据发送
		GPIO_WAKEUP_MODULE_HIGH;
		mcu_wakeup_module_tick = clock_time() | 1; //保证非0
		mcu_uart_data_flg = 1;
	}
	else{ //UART上 之前的数据收发还在进行
		mcu_wakeup_module_tick = 0;
	}
	mcu_cmd_no_ack = 1;
#endif


	if(my_fifo_push(&uart_tx_fifo, p, n)){
		SET_DBG_FLG(0x11);  //debug
	}
}







#define			HID_HANDLE_CONSUME_REPORT			21
#define			HID_HANDLE_KEYBOARD_REPORT			25
#define			AUDIO_HANDLE_MIC					43
/////////////////////////// led management /////////////////////
enum{
	LED_POWER_ON = 0,
	LED_SHINE_0S5,
	LED_SHINE_CONNECT,
	LED_SHINE_DISCONNECT, //3

	LED_AUDIO_ON,	//1
	LED_AUDIO_OFF,	//2
	LED_SHINE_SLOW, //3
	LED_SHINE_FAST, //4
	LED_SHINE_OTA, //5

};

const led_cfg_t led_cfg[] = {
	    {1000,    0,      1,      0x00,	 },    //power-on, 1s on
	    {125,	  125 ,   2,	  0x08,  },    //4Hz shine
	    {1500,	  0 ,  	  1,	  0x04,  },    //connect
	    {500,	  500 ,   2,	  0x04,  },    //disconenct

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

module_notify_t consumer_notify_cmd = {
		SPP_CMD_SEND_NOTIFY_DATA,
		4,
		HID_HANDLE_CONSUME_REPORT,
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
				consumer_notify_cmd.data[0] = (key0 == VK_VOL_UP ? 0x01 : 0x02);
				cur_mcu_cmd = SPP_CMD_SEND_NOTIFY_DATA;

				push_uart_data_to_fifo(&consumer_notify_cmd, 8);

			}
			else if(key0 == RED_KEY){
				static u8 adv_en;
				adv_en = !adv_en;

				//enable/disable advertising: 0a ff 01 00  01
				u8 adv_en_cmd[5];
				mcu_cmd_t *pCmd = (mcu_cmd_t *)adv_en_cmd;

				pCmd->cmd = cur_mcu_cmd = SPP_CMD_SET_ADV_ENABLE;
				pCmd->cmdLen = 1;
				pCmd->data[0] = adv_en;

				push_uart_data_to_fifo(adv_en_cmd, 5);

			}
			else
			{
				if( key0 >= VK_1 && key0 <= VK_0){
					key_type = KEYBOARD_KEY;

					kb_notify_cmd.data[2] = key0;
					cur_mcu_cmd = SPP_CMD_SEND_NOTIFY_DATA;

					push_uart_data_to_fifo(&kb_notify_cmd, 14);
				}
			}
		}
	}
	else{  //kb_event.cnt == 0,  key release
		key_not_released = 0;
		if(key_type == CONSUMER_KEY){
			consumer_notify_cmd.data[0] = 0;
			cur_mcu_cmd = SPP_CMD_SEND_NOTIFY_DATA;

			push_uart_data_to_fifo(&consumer_notify_cmd, 8);
		}
		else if(key_type == KEYBOARD_KEY){
			kb_notify_cmd.data[2] = 0;
			cur_mcu_cmd = SPP_CMD_SEND_NOTIFY_DATA;

			push_uart_data_to_fifo(&kb_notify_cmd, 14);
		}

		key_type = IDLE_KEY;
	}
}



void proc_keyboard (void)
{
#if 1
	static u32 keyScanTick = 0;
	if(clock_time_exceed(keyScanTick, 8000)){
		keyScanTick = clock_time();
	}
	else{
		return;
	}
#endif

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

int	module_uart_working;

int remote_suspend_enter ()
{
	 //module有uart数据时会在GPIO_WAKEUP_MCU脚上输出高电平
	module_uart_working = gpio_read(GPIO_WAKEUP_MCU);

	if (module_uart_working)
	{
		return 0;
	}
	return 1;
}



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


	gpio_set_wakeup		(GPIO_WAKEUP_MCU, 1, 1);  // core(gpio) high wakeup suspend
	cpu_set_gpio_wakeup (GPIO_WAKEUP_MCU, 1, 1);  // pad high wakeup deepsleep


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


#if (REMOTE_PM_ENABLE)
	bls_pm_registerFuncBeforeSuspend( &remote_suspend_enter );
#endif

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
		module_event_t *pEvt = (module_event_t *)my_rxdata_use.data;
		if(pEvt->token != 0xff){
			module_evt_err ++;
		}
		else{
			module_evt_rcv = pEvt->event;

#if (REMOTE_PM_ENABLE)
			if(mcu_cmd_no_ack){

				moduel_evt_expect = (cur_mcu_cmd & 0x3ff) | 0x400;
				if( module_evt_rcv != moduel_evt_expect){
					module_evt_unmatch ++;
				}
				else{
					mcu_cmd_no_ack = 0;  //cmd acked

					if(cur_mcu_cmd != SPP_CMD_CONN_PARAM_UPDATE){
						device_led_setup(led_cfg[LED_SHINE_0S5]);
					}
				}
			}
			else{
				if(module_evt_rcv == SPP_EVENT_CONNECT){
					device_led_setup(led_cfg[LED_SHINE_CONNECT]);

					cur_mcu_cmd = SPP_CMD_CONN_PARAM_UPDATE;
					u16 dat[6];
					dat[0] = SPP_CMD_CONN_PARAM_UPDATE;
					dat[1] = 8;  //cmd len
					dat[2] = 12; //interval min
					dat[3] = 12; //interval max
					dat[4] = 66; //latency
					dat[5] = 400; //timeout

					push_uart_data_to_fifo((u8 *)dat, 12);
				}
				else if(module_evt_rcv == SPP_EVENT_DISCONNECT){
					device_led_setup(led_cfg[LED_SHINE_DISCONNECT]);
				}
				else{

				}
			}
#endif
		}
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


#if (REMOTE_PM_ENABLE)
		if(mcu_wakeup_module_tick){
			while( !clock_time_exceed(mcu_wakeup_module_tick, 2500) );
		}
#endif


		//DEBUG_GPIO_TOGGLE;

		if (uart_Send((u8 *)(&my_txdata_buf)))
		{
			my_fifo_pop (&uart_tx_fifo);
		}
	}
	return 0;
}



void blt_pm_proc(void)
{
}


u32 tick_loop;

extern u8	blt_slave_main_loop (void);

int suspend_mode = 0;
int suspend_ms;
u32 loop_cnt = 0;
u32 loop_begin_tick;

#define UART_TX_BUSY    ( (uart_tx_fifo.rptr != uart_tx_fifo.wptr) || uart_tx_is_busy() )
#define UART_RX_BUSY	(my_rx_uart_w_index != my_rx_uart_r_index)



int	mcu_uart_working;
int mcu_task_busy;

void main_loop ()
{
	tick_loop ++;

	loop_begin_tick = clock_time();

	//UART entry
	app_packet_from_uart ();
	app_packet_to_uart ();


	//UI entry
	task_audio();

	proc_keyboard();

	device_led_process();


	// power management
#if (REMOTE_PM_ENABLE)
	extern u32	scan_pin_need;

	mcu_uart_working = UART_RX_BUSY || UART_TX_BUSY || mcu_cmd_no_ack;//mcu自己检查uart rx和tx是否都处理完毕


	//当mcu的uart数据发送完毕后，将GPIO_WAKEUP_MODULE拉低（输出对电平）
	if(mcu_uart_data_flg && !mcu_uart_working){
		mcu_uart_data_flg = 0;
		mcu_wakeup_module_tick = 0;
		GPIO_WAKEUP_MODULE_LOW;
	}

	mcu_task_busy = scan_pin_need || key_not_released || DEVICE_LED_BUSY;

	if(mcu_uart_working){
		loop_cnt = 0;
		sleep_us(10);
	}
	else{ //can enter suspend

		GPIO_WAKEUP_MCU_LOW;

		if(mcu_task_busy){
			suspend_mode = 0;
			loop_cnt = 0;
			suspend_ms = 15;

			u32 pin[] = KB_DRIVE_PINS;
			for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
			{
				gpio_set_wakeup(pin[i],1,0);
			}
		}
		else{
			loop_cnt ++;
			if(loop_cnt > 5){ //
				suspend_mode = 1;
				suspend_ms = 1000;  //1 s
			}

			u32 pin[] = KB_DRIVE_PINS;
			for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
			{
				gpio_set_wakeup(pin[i],1,1);
			}
		}



		//DEBUG_GPIO_LOW;
		cpu_sleep_wakeup(0, PM_WAKEUP_TIMER | PM_WAKEUP_CORE, loop_begin_tick + suspend_ms * CLOCK_SYS_CLOCK_1MS);
		//DEBUG_GPIO_HIGH;
		//mcu 工作时时，将GPIO_WAKEUP_MCU 上拉电阻开起来，确保module可以读到高电平
		GPIO_WAKEUP_MCU_HIGH;
	}
#endif
}




#endif  //end of __PROJECT_8267_BLE_RC_DEMO__
