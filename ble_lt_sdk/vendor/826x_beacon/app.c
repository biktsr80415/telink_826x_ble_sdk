#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/hci/hci.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj/drivers/uart.h"
#include "app_att.h"


//#define     MY_APP_ADV_CHANNEL					BLT_ENABLE_ADV_39
#define     MY_APP_ADV_CHANNEL					BLT_ENABLE_ADV_ALL
#define 	MY_DIRECT_ADV_TMIE_INIT				10000000

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);

#if 1

u32			tick_led_config;
const u8	tbl_scanRsp [] = {
		 0x08, 0x09, 'T', 'E', 'S', 'T', 'A', 'D', 'V',	//scan name
	};
#endif

void    app_update_adv ()
{
	u16 interval = telink_beacon_config.beacon_period * 8 / 5;
	bls_ll_setAdvData( (u8 *)&ibeacon_tbl_adv, sizeof(ibeacon_tbl_adv) );
	bls_ll_setAdvParam( interval, interval, \
						ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
						0,  NULL,  \
						MY_APP_ADV_CHANNEL, \
						ADV_FP_NONE);

}

void 	app_switch_to_nonconnectable_adv(u8 e, u8 *p, int n)
{

	tick_led_config = 0;
	gpio_write (GPIO_LED, 0);
	app_update_adv ();

	bls_ll_setAdvDuration(MY_DIRECT_ADV_TMIE_INIT, 0);
	bls_ll_setAdvEnable(1);  //must: set adv enable
}

void	task_connect (u8 e, u8 *p, int n)
{
	tick_led_config = 0;
	gpio_write (GPIO_LED, 1);
}

void	task_terminate (u8 e, u8 *p, int n)
{
	tick_led_config = 0;
	gpio_write (GPIO_LED, 0);
	irq_disable ();
	//save to flash
	u8 *padv = (u8 *) FLASH_TELINK_BEACON_CONFIG;
	if (memcmp (padv, &ibeacon_tbl_adv, sizeof (ibeacon_tbl_adv)) ||
		memcmp (padv + 64, &telink_beacon_config, sizeof (telink_beacon_config))	)
	{
		flash_erase_sector(FLASH_TELINK_BEACON_CONFIG);
		flash_write_page (FLASH_TELINK_BEACON_CONFIG, sizeof (ibeacon_tbl_adv), &ibeacon_tbl_adv);
		flash_write_page (FLASH_TELINK_BEACON_CONFIG + 64, sizeof (telink_beacon_config), &telink_beacon_config);
	}
	//reboot
	//REG_ADDR8 (0x6f) = 0x20;
	cpu_sleep_wakeup (1, PM_WAKEUP_TIMER, clock_time () + 10000 * sys_tick_per_us );
}


int AA_dbg_suspend;
void  func_suspend_enter (u8 e, u8 *p, int n)
{
	AA_dbg_suspend ++;
}

void  func_suspend_exit (u8 e, u8 *p, int n)
{

}

int setTxPowerMode(void* p){
    rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
    if((pm->value <= TX_POWERMODE_MAX) && (pm->value >= 0)){
        telink_beacon_config.txpower_mode = pm->value;
        rf_set_power_level_index(telink_beacon_config.txpower_mode);
    }
}

int setBeaconPeriod(void* p){
    u32 advInterval;
    rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
    u16 temV = 0;
    temV = pm->value + (*(&pm->value + 1)<<8);
    if((temV>=100) && (temV<=BEACON_PERIOD_MAX)&&(telink_beacon_config.beacon_period != temV)){
        telink_beacon_config.beacon_period = temV;   //  unit of ms
        u16 interval = temV * 8 / 5;
        bls_ll_setAdvParam( interval, interval,\
        						ADV_TYPE_NONCONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
        						0,  NULL,  \
        						MY_APP_ADV_CHANNEL, \
        						ADV_FP_NONE);
    }
}

//////////////////////////////////////////////////////////
// event call back
//////////////////////////////////////////////////////////

#if ( FEATURE_TEST_MODE == TEST_SCANNING_IN_ADV_AND_CONN_SLAVE_ROLE )

#define DBG_ADV_REPORT_ON_RAM 				1
#if (DBG_ADV_REPORT_ON_RAM)  //debug adv report on ram
	#define  RAM_ADV_MAX		64
	u8 AA_advRpt[RAM_ADV_MAX][48];
	u8 AA_advRpt_index = 0;
#endif

int app_event_callback (u32 h, u8 *p, int n)
{

	static u32 event_cb_num;
	event_cb_num++;

	if (h &HCI_FLAG_EVENT_BT_STD)		//ble controller hci event
	{
		u8 evtCode = h & 0xff;

		if(evtCode == HCI_EVT_LE_META)
		{
			u8 subEvt_code = p[0];
			if (subEvt_code == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
			{
				//after controller is set to scan state, it will report all the adv packet it received by this event

				event_adv_report_t *pa = (event_adv_report_t *)p;
				s8 rssi = pa->data[pa->len];

				#if (PRINT_DEBUG_INFO)
					printf("LE advertising report:\n");foreach(i, pa->len + 11){PrintHex(p[i]);}printf("\n");
				#endif

				#if (DBG_ADV_REPORT_ON_RAM)
					if(pa->len > 31){
						pa->len = 31;
					}
					memcpy( (u8 *)AA_advRpt[AA_advRpt_index++],  p, pa->len + 11);
					if(AA_advRpt_index >= RAM_ADV_MAX){
						AA_advRpt_index = 0;
					}
				#endif

				DBG_CHN3_TOGGLE;

			}
		}
	}

}
#endif

///////////////////////////////////
u8 ui_ota_is_working;
void entry_ota_mode(void)
{
	ui_ota_is_working = 1;

	bls_ota_setTimeout(100 * 1000000); //set OTA timeout  100 S
}

void show_ota_result(int result)
{
#if 1
	for(int i=0; i< 8;i++)
	{
		if (result == OTA_SUCCESS)
		{	//4Hz shine for  4 second
			gpio_write(GPIO_LED, i & 1);
		}
		else
		{
			gpio_write(GPIO_LED, i & 4);
		}
		cpu_sleep_wakeup (0, PM_WAKEUP_TIMER, clock_time () + 125000 * sys_tick_per_us );
	}
	gpio_write(GPIO_LED, 0);
#endif
}

void user_init()
{
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

#if (USB_ADV_REPORT_TO_PC_ENABLE)
	REG_ADDR8(0x74) = 0x53;
	REG_ADDR16(0x7e) = 0x08d1;
	REG_ADDR8(0x74) = 0x00;
	usb_log_init ();
	usb_dp_pullup_en (1);  //open USB enum
#endif


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
	//tbl_mac[4] = 0x55;
	//tbl_mac[5] = 0xaa;

	pmac = (u32 *) FLASH_TELINK_BEACON_CONFIG;
	if (*pmac != 0xffffffff)
	{
		memcpy (&ibeacon_tbl_adv, pmac, sizeof (ibeacon_tbl_adv));
		memcpy (&telink_beacon_config, ((u8 *)pmac) + 64, sizeof (telink_beacon_config));
	}
	//ibeacon_tbl_adv.flag = 7;

	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory

	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,


	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization
	//bls_smp_enableParing (SMP_PARING_CONN_TRRIGER ); 	//smp initialization
	//HID_service_on_android7p0_init();  //hid device on android 7.0

///////////////////// USER application initialization ///////////////////
	//bls_ll_setAdvData( (u8 *)&ibeacon_tbl_adv, sizeof(ibeacon_tbl_adv) );
	app_update_adv ();

	bls_ll_setScanRspData(0, 0);
	//rf_set_power_level_index (RF_POWER_0dBm);
	rf_set_power_level_index (telink_beacon_config.txpower_mode);
	bls_ll_setAdvParam( ADV_INTERVAL_100MS, ADV_INTERVAL_100MS, \
							ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
							0,  NULL,  \
							MY_APP_ADV_CHANNEL, \
							ADV_FP_NONE);
	bls_ll_setAdvEnable(1);  //adv enable
	tick_led_config = clock_time () | 1;

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &task_terminate);
	bls_ll_setAdvDuration(MY_DIRECT_ADV_TMIE_INIT, 1);
	bls_app_registerEventCallback (BLT_EV_FLAG_ADV_DURATION_TIMEOUT, &app_switch_to_nonconnectable_adv);


#if (BLE_PM_ENABLE)
	blc_ll_initPowerManagement_module();
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	//bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &func_suspend_enter);
	//bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &func_suspend_exit);
#endif

	// OTA init
	bls_ota_clearNewFwDataArea(); //must
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);


	////// Host Initialization  //////////
#if 0
	// 1.  controller ACL data to host
	blc_l2cap_register_handler (blc_hci_sendACLData2Host);  	//l2cap initialization

	// 2.  controller event data to hci tx fifo
	blc_hci_registerControllerEventHandler(blc_hci_send_data);

	// 3.  host cmd to controller / hci tx fifo data to host
#endif



	//scan test
//	blc_hci_le_setEventMask_cmd(HCI_LE_EVT_MASK_ADVERTISING_REPORT);
//
//	u8 filter_mac[6] = {0x77, 0x77,0x77,0x77,0x77,0x77};
//	ll_whiteList_add(BLE_ADDR_PUBLIC, filter_mac);
//	bls_ll_setScanParameter(SCAN_TYPE_ACTIVE, 144, 144, OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_WL);
//	blc_ll_setScanEnable(1, 1);


}


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
u32 tick_wakeup;
void main_loop (void)
{
	static u32 tick_loop, led = 0;
	static u32 tick_ui;

	tick_loop ++;

	blt_sdk_main_loop();


	if (tick_led_config && clock_time_exceed (tick_led_config, 80000))
	{
		tick_led_config = clock_time () | 1;
		gpio_write (GPIO_LED, led);
		led = !led;
	}

	if (gpio_read (GPIO_BUTTON))
	{
		tick_ui = clock_time () | 1;
	}
	else if (clock_time_exceed (tick_ui, 3000000))		//reboot if button pressed for 3 seconds
	{
		cpu_sleep_wakeup (1, PM_WAKEUP_TIMER, clock_time () + 10000 * sys_tick_per_us );
	}
}
