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


#if (HCI_ACCESS==HCI_USE_UART)
#include "../../proj/drivers/uart.h"
#endif

////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};

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

//////////////////////////////////////////
// Service Discovery
//////////////////////////////////////////
typedef void (*main_service_t) (void);

main_service_t		main_service = 0;

u16	conn_handle;					//	handle of  connection

void app_register_service (void *p)
{
	main_service = p;
}

int app_l2cap_handler (u16 conn, u8 *raw_pkt)
{
	u8 *p = raw_pkt + 12;
	blc_hci_send_data (conn | BLM_CONN_HANDLE | HCI_FLAG_ACL_BT_STD, p, p[1]);	//can be removed, debug purpose

	return 0;
}

//////////////////////////////////////////////////////////
// event call back
//////////////////////////////////////////////////////////
int app_event_callback (u32 h, u8 *p, int n)
{
	static u32 event_cb_num;
	event_cb_num++;

	blc_hci_send_data (h, p, n);
	return 0;
}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
int main_idle_loop ()
{
	static u32 tick_loop;
	tick_loop ++;

	// bcopy
	////////////////////////////////////// BLE entry /////////////////////////////////
	blm_main_loop ();

	extern int blc_hci_proc ();
	blc_hci_proc ();
	////////////////////////////////////// UI entry /////////////////////////////////
	device_led_process();

	////////////////////////////////////// USB /////////////////////////////////
	btusb_handle_irq ();
	return 0;
}

void main_loop ()
{
	main_idle_loop ();

	if (main_service)
	{
		main_service ();
		main_service = 0;
	}
}

///////////////////////////////////////////
void user_init()
{
	rf_customized_param_load();  //load customized freq_offset cap value and tp value

	usb_log_init ();
	btusb_init ();
	usb_dp_pullup_en (1);  //open USB enum

	/////////////////// keyboard drive/scan line configuration /////////

	//blc_l2cap_register_handler (blc_l2cap_packet_receive);
	blc_enable_hci_master_handler ();
	blm_register_event_data_callback (app_event_callback, app_l2cap_handler);
	att_register_idle_func (main_idle_loop);

#if (HCI_ACCESS==HCI_USE_USB)
	usb_bulk_drv_init (0);
	blc_register_hci_handler (blc_acl_from_btusb, blc_hci_tx_to_btusb);
#else	//uart
	//one gpio should be configured to act as the wakeup pin if in power saving mode; pending
	//todo:uart init here
	uart_io_init(UART_GPIO_8267_PB2_PB3);
	CLK32M_UART115200;
	uart_BuffInit((u8 *)(&T_rxdata_buf), sizeof(T_rxdata_buf), (u8 *)(&T_txdata_buf));
	blc_register_hci_handler (blc_rx_from_uart, blc_hci_tx_to_uart);
#endif

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

	ble_master_init (pmac);
	blm_ll_setScanEnable (0, 0);

	/////////////////////////////////////////////////////////////////
	//ll_whiteList_reset();

	//device_led_init(GPIO_LED, 1);
}
