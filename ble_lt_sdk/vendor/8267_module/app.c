#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll_whitelist.h"
#include "../../proj/drivers/keyboard.h"
#include "../common/blt_led.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"

#if (HCI_ACCESS==HCI_USE_UART)
#include "../../proj/drivers/uart.h"
#endif


#define			HID_HANDLE_CONSUME_REPORT			21
#define			HID_HANDLE_KEYBOARD_REPORT			25


//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};

u8	tbl_advData[] = {
	 0x05, 0x09, 't', 'M', 'o', 'd',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
	 0x03, 0x19, 0x80, 0x01, 					// 384, Generic Remote Control, Generic category
	 0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,		// incomplete list of service class UUIDs (0x1812, 0x180F)
};

u8	tbl_scanRsp [] = {
		 0x07, 0x09, 't', 'M', 'o', 'd', 'u', 'l',	//scan name " tmodul"
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
	    {500,	  500 ,   3,	  0x04,	 },    //1Hz for 3 seconds
	    {250,	  250 ,   6,	  0x04,  },    //2Hz for 3 seconds
	    {125,	  125 ,   200,	  0x08,  },    //4Hz for 50 seconds
};


u32	advertise_begin_tick;


extern kb_data_t	kb_event;


u8 key_buf[8] = {0};
u8 key_type;
u8 pm_mask_temp;
#define CONSUMER_KEY   0
#define KEYBOARD_KEY   1


u8 sendTerminate_before_enterDeep = 0;

int key_not_released;
int ir_not_released;

u32 latest_user_event_tick;
u8  user_task_ongoing_flg;

#if (STUCK_KEY_PROCESS_ENABLE)
u32 stuckKey_keyPressTime;
#endif


void	conn_para_update_req_proc (u8 e, u8 *p)
{
	//p[1]p[0]:offset; p[3]p[2]:interval; p[5]p[4]:latency; p[7]p[6]:timeout; p[9]p[8]:inst;

	u16 *ps = (u16 *)p;
	int interval = ps[1];

}


void 	ble_remote_terminate(u8 e,u8 *p) //*p is terminate reason
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

void	task_connect (u8 e, u8 *p)
{
	bls_l2cap_requestConnParamUpdate (12, 32, 99, 400);  //interval=10ms latency=99 timeout=4s
	//latest_user_event_tick = clock_time();
}





static u8 key_voice_press = 0;
static u8 ota_is_working = 0;
static u32 key_voice_pressTick = 0;





u32		dbg_key;
void key_change_proc(void)
{

	latest_user_event_tick = clock_time();  //record latest key change time

	if(key_voice_press){  //clear voice key press flg
		key_voice_press = 0;
	}

	u8 key = kb_event.keycode[0];

	if ( (key & 0xf0) == 0xf0)			//key in consumer report
	{
		dbg_key++;
		key_not_released = 1;
		key_type = CONSUMER_KEY;
		u16 media_key = 1 << (key & 0xf);
		bls_att_pushNotifyData (HID_HANDLE_CONSUME_REPORT, (u8 *)&media_key, 2);
	}
	else if (key)			// key in standard reprot
	{
		key_not_released = 1;
		if (key == VK_M)
		{
			{ //if voice not on, mark voice key press tick
				key_voice_press = 1;
				key_voice_pressTick = clock_time();
			}
		}
		else{
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



void proc_keyboard (u8 e, u8 *p)
{
	static u32 keyDetectTick = 0;
	if(clock_time_exceed(keyDetectTick, 15000)){
		keyDetectTick = clock_time();
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
	if(key_voice_press && clock_time_exceed(key_voice_pressTick,1000000)){
		key_voice_press = 0;
	}
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
	rf_customized_param_load();  //load customized freq_offset cap value and tp value

	usb_log_init ();
	usb_dp_pullup_en (1);  //open USB enum

	/////////////////// keyboard drive/scan line configuration /////////
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

	////////////////// BLE stack initialization ////////////////////////////////////
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

	//link layer initialization
	bls_ll_init (tbl_mac);

	//gatt initialization
    //NOTE: my_att_init  must after bls_ll_init, and before bls_ll_setAdvParam
	extern void my_att_init ();
	my_att_init ();

	//l2cap initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);

	//smp initialization
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );



	///////////////////// USER application initialization ///////////////////

	bls_ll_setAdvData( tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData(tbl_scanRsp, sizeof(tbl_scanRsp));


	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS, \
			 	 	 	 	 	     ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
			 	 	 	 	 	     0,  NULL,  BLT_ENABLE_ADV_37, ADV_FP_NONE);

	bls_ll_setAdvEnable(1);  //adv enable

	rf_set_power_level_index (RF_POWER_8dBm);

	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);


#if(BLE_REMOTE_PM_ENABLE)
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif


	/////////////////////////////////////////////////////////////////
#if (TELIK_SPP_SERVICE_ENABLE)
	////////////////// SPP initialization ///////////////////////////////////
	#if (HCI_ACCESS==HCI_USE_USB)
		//usb_bulk_drv_init (0);
		//blc_register_hci_handler (blc_hci_rx_from_usb, blc_hci_tx_to_usb);
	#else	//uart
		//one gpio should be configured to act as the wakeup pin if in power saving mode; pending
		//todo:uart init here
		rx_uart_r_index = 0;
		rx_uart_w_index = 0;
		uart_io_init(UART_GPIO_8267_PB2_PB3);
		CLK16M_UART115200;
		uart_BuffInit((u8 *)(&T_rxdata_buf), sizeof(T_rxdata_buf), (u8 *)(&T_txdata_buf));
		blc_register_hci_handler (blc_rx_from_uart, blc_hci_tx_to_uart);		//default handler,set your own
															//in spp.c like rx_from_uart_cb & tx_to_uart_cb
	#endif
//	extern void event_handler(u32 h, u8 *para, int n);
//	bls_register_event_data_callback(event_handler);		//register event callback
#else  //remote control
	/////////////////// keyboard drive/scan line configuration /////////
	u32 pin[] = KB_DRIVE_PINS;
	for (int i=0; i<(sizeof (pin)/sizeof(*pin)); i++)
	{
		gpio_set_wakeup(pin[i],1,1);  	   //drive pin core(gpio) high wakeup suspend
		cpu_set_gpio_wakeup (pin[i],1,1);  //drive pin pad high wakeup deepsleep
	}
	gpio_core_wakeup_enable_all(1);

	bls_app_registerEventCallback (BLT_EV_FLAG_GPIO_EARLY_WAKEUP, &proc_keyboard);
	bls_app_registerEventCallback (BLT_EV_FLAG_SET_WAKEUP_SOURCE, &ble_remote_set_sleep_wakeup);

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

	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_slave_main_loop ();


	////////////////////////////////////// UI entry /////////////////////////////////
	proc_keyboard (0,0);


	device_led_process();
}
