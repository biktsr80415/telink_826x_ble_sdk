/********************************************************************************************************
 * @file     app.c
 *
 * @brief    for TLSR chips
 *
 * @author	 BLE Group
 * @date     May. 12, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/

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
#include "../../proj/drivers/battery.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj_lib/ble/ble_common.h"


#if (HCI_ACCESS==HCI_USE_UART)
#include "../../proj/drivers/uart.h"
#endif


MYFIFO_INIT(hci_rx_fifo, 72, 2);
MYFIFO_INIT(hci_tx_fifo, 72, 8);

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);
//////////////////////////////////////////////////////////////////////////////
//	Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
const u8	tbl_advData[] = {
	 0x05, 0x09, 't', 'M', 'o', 'd',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
};

const u8	tbl_scanRsp [] = {
		 0x07, 0x09, 't', 'M', 'o', 'd', 'u', 'l',	//scan name " tmodul"
	};


u8 	ui_ota_is_working = 0;


#if SIG_PROC_ENABLE
/*------------------------------------------------------------------- l2cap data pkt(SIG) ---------------------------------------------------*
 | stamp_time(4B) |llid nesn sn md |  pdu-len   | l2cap_len(2B)| chanId(2B)| Code(1B)|Id(1B)|Data-Len(2B) |           Result(2B)             |
 |                |   type(1B)     | rf_len(1B) |       L2CAP header       |          SIG pkt Header      |  SIG_Connection_param_Update_Rsp |
 |                |                |            |     0x0006   |    0x05   |   0x13  | 0x01 |  0x0002     |             0x0000               |
 |                |          data_headr         |                                                       payload                              |
 *-------------------------------------------------------------------------------------------------------------------------------------------*/
int att_sig_proc_handler (u16 connHandle, u8 * p)
{
	rf_pkt_l2cap_sig_connParaUpRsp_t* pp = (rf_pkt_l2cap_sig_connParaUpRsp_t*)p;
	static u8 conn_update_cnt;
	u8 sig_conn_param_update_rsp[9] = { 0x0A, 0x06, 0x00, 0x05, 0x00, 0x13, 0x01, 0x02, 0x00 };
	if(!memcmp(sig_conn_param_update_rsp, &pp->rf_len, 9) && ((pp->type&0b11) == 2)){//l2cap data pkt, start pkt
		if(pp->result == 0x0000){
			printf("SIG: the LE master Host has accepted the connection parameters.\n");
			conn_update_cnt = 0;
		}
		else if(pp->result == 0x0001)
		{
			printf("SIG: the LE master Host has rejected the connection parameters..\n");
			printf("Current Connection interval:%dus.\n", bls_ll_getConnectionInterval() * 1250 );
			conn_update_cnt++;
            if(conn_update_cnt < 4){
            	printf("Slave sent update connPara req!\n");
            }
			if(conn_update_cnt == 1){
				bls_l2cap_requestConnParamUpdate (8, 16, 0, 400);//18.75ms iOS
			}
			else if(conn_update_cnt == 2){
				bls_l2cap_requestConnParamUpdate (16,32, 0, 400);
			}
			else if(conn_update_cnt == 3){
				bls_l2cap_requestConnParamUpdate (32,60, 0, 400);
			}
			else{
				conn_update_cnt = 0;
				printf("Slave Connection Parameters Update table all tested and failed!\n");
			}
		}
	}

}
#endif

void entry_ota_mode(void)
{
	ui_ota_is_working = 1;

	bls_ota_setTimeout(100 * 1000000); //set OTA timeout  100 S

	//gpio_write(GPIO_LED, 1);  //LED on for indicate OTA mode
}

void show_ota_result(int result)
{
#if 0
	if(result == OTA_SUCCESS){
		for(int i=0; i< 8;i++){  //4Hz shine for  4 second
			gpio_write(BLUE_LED, 0);
			sleep_us(125000);
			gpio_write(BLUE_LED, 1);
			sleep_us(125000);
		}
	}
	else{
		for(int i=0; i< 8;i++){  //1Hz shine for  4 second
			gpio_write(BLUE_LED, 0);
			sleep_us(500000);
			gpio_write(BLUE_LED, 1);
			sleep_us(500000);
		}

		//write_reg8(0x8000,result); ;while(1);  //debug which err lead to OTA fail
	}


	gpio_write(BLUE_LED, 0);
#endif
}


#define MAX_INTERVAL_VAL		16
void	task_connect (void)
{
	//bls_l2cap_requestConnParamUpdate (12, 32, 0, 400);
	bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);
#if 0
	//update connParam
	if(bls_ll_getConnectionInterval() > MAX_INTERVAL_VAL)//
	{
	    printf("ConnInterval > 20ms.\nSlave sent update connPara req!\n");
	    bls_l2cap_requestConnParamUpdate(MAX_INTERVAL_VAL, MAX_INTERVAL_VAL, 0, 400);
	}
	else
	{
		printf("ConnInterval < 20ms.\nSlave NOT need sent update connPara req!\n");
		printf("Connection interval:%dus.\n", bls_ll_getConnectionInterval() * 1250 );
	}
#endif
	

//	gpio_write(GREEN_LED,ON);
}


void led_init(void)
{
	gpio_set_func(GREEN_LED, AS_GPIO);
	gpio_set_input_en(GREEN_LED,0);
	gpio_set_output_en(GREEN_LED,1);
	gpio_write(GREEN_LED,OFF);
}

u32 tick_wakeup;
int	mcu_uart_working;
int	module_uart_working;
int module_task_busy;


int	module_uart_data_flg;
u32 module_wakeup_module_tick;

#define UART_TX_BUSY			( (hci_tx_fifo.rptr != hci_tx_fifo.wptr) || uart_tx_is_busy() )
#define UART_RX_BUSY			(hci_rx_fifo.rptr != hci_rx_fifo.wptr)

int app_module_busy ()
{
	mcu_uart_working = gpio_read(GPIO_WAKEUP_MODULE);  //mcu use GPIO_WAKEUP_MODULE to indicate the UART data transmission or receiving state
	module_uart_working = UART_TX_BUSY || UART_RX_BUSY; //module checks to see if UART rx and tX are all processed
	module_task_busy = mcu_uart_working || module_uart_working;
	return module_task_busy;
}

void app_suspend_exit ()
{
	GPIO_WAKEUP_MODULE_HIGH;  //module enter working state
	bls_pm_setSuspendMask(SUSPEND_DISABLE);
	tick_wakeup = clock_time () | 1;
}

int app_suspend_enter ()
{
	if (app_module_busy ())
	{
		app_suspend_exit ();
		return 0;
	}
	return 1;
}

void app_power_management ()
{
#if __PROJECT_8266_MODULE__
	uart_ErrorCLR();
#endif

#if (BLE_MODULE_INDICATE_DATA_TO_MCU)
	module_uart_working = UART_TX_BUSY || UART_RX_BUSY;

	if(ui_ota_is_working){
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
		bls_pm_setManualLatency(0);
		return;
	}
	//When module's UART data is sent, the GPIO_WAKEUP_MCU is lowered or suspended (depending on how the user is designed)
	if(module_uart_data_flg && !module_uart_working){
		module_uart_data_flg = 0;
		module_wakeup_module_tick = 0;
		GPIO_WAKEUP_MCU_LOW;
	}
#endif


	// pullup GPIO_WAKEUP_MODULE: exit from suspend
	// pulldown GPIO_WAKEUP_MODULE: enter suspend

#if (BLE_MODULE_PM_ENABLE)

	if (!app_module_busy() && !tick_wakeup)
	{
		bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);  // GPIO_WAKEUP_MODULE needs to be wakened
	}

	if (tick_wakeup && clock_time_exceed (tick_wakeup, 500))
	{
		GPIO_WAKEUP_MODULE_LOW;
		tick_wakeup = 0;
	}

#endif
}


void user_init()
{
	REG_ADDR8(0x74) = 0x53;
	REG_ADDR16(0x7e) = 0x08d1;
	REG_ADDR8(0x74) = 0x00;
	usb_log_init ();
	usb_dp_pullup_en (1);  //open USB enum

	led_init();

	/***********************************************************************************
	 * These section must be before battery_power_check.
	 * Because when low battery,chip will entry deep.if placed after battery_power_check,
	 * it is possible that can not wake up chip.
	 * mcu can wake up module from suspend or deepsleep by pulling up GPIO_WAKEUP_MODULE
	 *  *******************************************************************************/
	#if (BLE_MODULE_PM_ENABLE)
		gpio_set_wakeup		(GPIO_WAKEUP_MODULE, 1, 1);  // core(gpio) high wakeup suspend
		cpu_set_gpio_wakeup (GPIO_WAKEUP_MODULE, 1, 1);  // pad high wakeup deepsleep

		GPIO_WAKEUP_MODULE_LOW;
		bls_pm_registerFuncBeforeSuspend( &app_suspend_enter );
	#endif

	/*****************************************************************************************
	 Note: battery check must do before any flash write/erase operation, cause flash write/erase
		   under a low or unstable power supply will lead to error flash operation

		   Some module initialization may involve flash write/erase, include: OTA initialization,
				SMP initialization, ..
				So these initialization must be done after  battery check
	*****************************************************************************************/
	#if (BATT_CHECK_ENABLE)  //battery check must do before OTA relative operation
		if(analog_read(DEEP_ANA_REG2) == BATTERY_VOL_LOW){
			battery_power_check(VBAT_ALARM_THRES_MV + 200);//2.2V
		}
		else{
			battery_power_check(VBAT_ALARM_THRES_MV);//2.0 V
		}
	#endif

	/////////////////////////////////////////////////////////////
	u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};
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
///////////// BLE stack Initialization ////////////////
	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory

	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
	blc_ll_initPowerManagement_module();        //pm module:      	 optional


	//ATT initialization
//	blc_att_setRxMtuSize(160); //If not set RX MTU size, default is: 23 bytes.

	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization

 	//// smp initialization ////
#if (BLE_MODULE_SECURITY_ENABLE)
	blc_smp_param_setBondingDeviceMaxNumber(4);  	//default is SMP_BONDING_DEVICE_MAX_NUM, can not bigger that this value
													//and this func must call before bls_smp_enableParing
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );

	//blc_smp_enableScFlag(1);
	//blc_smp_setEcdhDebugMode(1);//debug
#else
	bls_smp_enableParing (SMP_PARING_DISABLE_TRRIGER );
#endif

	//HID_service_on_android7p0_init();  //hid device on android 7.0/7.1

	///////////////////// USER application initialization ///////////////////
	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));



	////////////////// config adv packet /////////////////////
	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS + 16,
								 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
								 0,  NULL,
								 MY_APP_ADV_CHANNEL,
								 ADV_FP_NONE);

	if(status != BLE_SUCCESS)
	{
		write_reg8(0x8000, 0x11);
		while(1);
	}  //debug: adv setting err


	bls_ll_setAdvEnable(1);  //adv enable
	rf_set_power_level_index (RF_POWER_8dBm);

	bls_pm_setSuspendMask (SUSPEND_DISABLE);//(SUSPEND_ADV | SUSPEND_CONN)


	////////////////// SPP initialization ///////////////////////////////////
	#if (HCI_ACCESS==HCI_USE_USB)
		blt_set_bluetooth_version (BLUETOOTH_VER_4_2);
		bls_ll_setAdvChannelMap (BLT_ENABLE_ADV_37);
		usb_bulk_drv_init (0);
		blc_register_hci_handler (blc_hci_rx_from_usb, blc_hci_tx_to_usb);
		bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );
	#else	//uart
		//one gpio should be configured to act as the wakeup pin if in power saving mode; pending
		//todo:uart init here
		#if __PROJECT_8266_MODULE__
				gpio_set_func(GPIO_UTX, AS_UART);
				gpio_set_func(GPIO_URX, AS_UART);
				gpio_set_input_en(GPIO_UTX, 1);
				gpio_set_input_en(GPIO_URX, 1);
				gpio_write (GPIO_UTX, 1);			//pull-high RX to avoid mis-trig by floating signal
				gpio_write (GPIO_URX, 1);			//pull-high RX to avoid mis-trig by floating signal
		#else
				gpio_set_input_en(GPIO_PB2, 1);
				gpio_set_input_en(GPIO_PB3, 1);
				gpio_setup_up_down_resistor(GPIO_PB2, PM_PIN_PULLUP_1M);
				gpio_setup_up_down_resistor(GPIO_PB3, PM_PIN_PULLUP_1M);
				uart_io_init(UART_GPIO_8267_PB2_PB3);
		#endif
		reg_dma_rx_rdy0 = FLD_DMA_UART_RX | FLD_DMA_UART_TX; //clear uart rx/tx status
		CLK16M_UART115200;
		uart_BuffInit(hci_rx_fifo_b, hci_rx_fifo.size, hci_tx_fifo_b);
		extern int rx_from_uart_cb (void);
		extern int tx_to_uart_cb (void);
		blc_register_hci_handler(rx_from_uart_cb,tx_to_uart_cb);				//customized uart handler
	#endif

	extern int event_handler(u32 h, u8 *para, int n);
	blc_hci_registerControllerEventHandler(event_handler);		//register event callback
	bls_hci_mod_setEventMask_cmd(0xfffff);			//enable all 18 events,event list see ble_ll.h

	// OTA init
	bls_ota_clearNewFwDataArea(); //must
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);

}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
u32 lowBattDet_tick = 0;

void main_loop ()
{
	static u32 tick_loop;
	tick_loop ++;

	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop();

	////////////////////////////////////// UI entry /////////////////////////////////
#if (BATT_CHECK_ENABLE)
	if(clock_time_exceed(lowBattDet_tick, 500*1000)){
		lowBattDet_tick = clock_time();
		battery_power_check(VBAT_ALARM_THRES_MV);
	}
#endif
	//  add spp UI task
	app_power_management ();

}
