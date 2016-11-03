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

MYFIFO_INIT(hci_tx_fifo, 72, 8);
//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};

#if 0 //(TELIK_SPP_SERVICE_ENABLE)
u8	tbl_advData[] = {
	 0x05, 0x09, 't', 'M', 'o', 'd',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
};
#else
u8	tbl_advData[] = {
	 0x05, 0x09, 't', 'M', 'o', 'd',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
	 0x03, 0x19, 0x80, 0x01, 					// 384, Generic Remote Control, Generic category
	 0x05, 0x02, 0x12, 0x18, 0x0F, 0x18,		// incomplete list of service class UUIDs (0x1812, 0x180F)
};
#endif

u8	tbl_scanRsp [] = {
		 0x07, 0x09, 't', 'M', 'o', 'd', 'u', 'l',	//scan name " tmodul"
	};



extern kb_data_t	kb_event;

u8 key_buf[8] = {0};
u8 key_type;
#define CONSUMER_KEY   0
#define KEYBOARD_KEY   1

int key_not_released;
u8 	ui_ota_is_working = 0;
u8  ui_task_flg;
u32	ui_advertise_begin_tick;

void entry_ota_mode(void)
{
	ui_ota_is_working = 1;

	bls_ota_setTimeout(30 * 1000000); //set OTA timeout  30 S

	gpio_write(GPIO_LED, 1);  //LED on for indicate OTA mode
}

void show_ota_result(int result)
{
#if 0
	if(result == OTA_SUCCESS){
		for(int i=0; i< 8;i++){  //4Hz shine for  4 second
			gpio_write(GPIO_LED, 0);
			sleep_us(125000);
			gpio_write(GPIO_LED, 1);
			sleep_us(125000);
		}
	}
	else{
		for(int i=0; i< 8;i++){  //1Hz shine for  4 second
			gpio_write(GPIO_LED, 0);
			sleep_us(500000);
			gpio_write(GPIO_LED, 1);
			sleep_us(500000);
		}

		//write_reg8(0x8000,result); ;while(1);  //debug which err lead to OTA fail
	}


	gpio_write(GPIO_LED, 0);
#endif
}



void 	ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{
	if(*p == HCI_ERR_CONN_TIMEOUT){

	}
	else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}
	else{

	}


	ui_advertise_begin_tick = clock_time();

}

void	task_connect (u8 e, u8 *p, int n)
{
#if (TELIK_SPP_SERVICE_ENABLE)
	bls_l2cap_requestConnParamUpdate (12, 32, 99, 400);  //interval=10ms latency=99 timeout=4s
#else
	bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);
#endif  //remote control
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

		u8 key = kb_event.keycode[0];

		if ( (key & 0xf0) == 0xf0)			//key in consumer report
		{
			key_not_released = 1;
			key_type = CONSUMER_KEY;
			u16 media_key = 1 << (key & 0xf);
			bls_att_pushNotifyData (HID_HANDLE_CONSUME_REPORT, (u8 *)&media_key, 2);
		}
		else if (key)			// key in standard reprot
		{
			key_not_released = 1;

			key_type = KEYBOARD_KEY;
			key_buf[2] = key;
			bls_att_pushNotifyData (HID_HANDLE_KEYBOARD_REPORT, key_buf, 8);

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
}


_attribute_ram_code_ void  ble_remote_set_sleep_wakeup (u8 e, u8 *p, int n)
{
	if( bls_ll_getCurrentState() == BLS_LINK_STATE_CONN && ((u32)(bls_pm_getSystemWakeupTick() - clock_time())) > 80 * CLOCK_SYS_CLOCK_1MS){  //suspend time > 30ms.add gpio wakeup
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);  //gpio CORE wakeup suspend
	}
}


extern u32	scan_pin_need;
void remote_control_pm_proc(void)
{
#if 1 //(BLE_REMOTE_PM_ENABLE)
	if(ui_ota_is_working){
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
	}
	else{
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);

		ui_task_flg = scan_pin_need || key_not_released || DEVICE_LED_BUSY;

		if(ui_task_flg){
#if 1
			extern int key_matrix_same_as_last_cnt;
			if(key_matrix_same_as_last_cnt > 5){  //key matrix stable can optize
				bls_pm_setManualLatency( 4 );
			}
			else{
				bls_pm_setManualLatency(0);  //latency off: 0
			}
#else
			bls_pm_setManualLatency(0);
#endif
		}

	}
#endif  //END of  BLE_REMOTE_PM_ENABLE
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
		gpio_set_func(GPIO_UTX, AS_UART);
		gpio_set_func(GPIO_URX, AS_UART);
		gpio_set_input_en(GPIO_UTX, 1);
		gpio_set_input_en(GPIO_URX, 1);
		gpio_write (GPIO_UTX, 1);			//pull-high RX to avoid mis-trig by floating signal
		gpio_write (GPIO_URX, 1);			//pull-high RX to avoid mis-trig by floating signal
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


	// OTA init
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);


	ui_advertise_begin_tick = clock_time();
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
#if(TELIK_SPP_SERVICE_ENABLE)

	//  add spp UI task

#else
	proc_keyboard (0,0, 0);
	remote_control_pm_proc();
#endif
}
