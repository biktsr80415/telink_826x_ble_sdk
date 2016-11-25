#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll_whitelist.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"

#if (HCI_ACCESS==HCI_USE_UART)
#include "../../proj/drivers/uart.h"
#endif


MYFIFO_INIT(hci_tx_fifo, 72, 8);

MYFIFO_INIT(blt_rxfifo, 64, 8);

//MYFIFO_INIT(blt_txfifo, 40, 16);
MYFIFO_INIT(blt_txfifo, 80, 8);
//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};

u8	tbl_advData[] = {
	 0x05, 0x09, 't', 'M', 'o', 'd',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
};

u8	tbl_scanRsp [] = {
		 0x07, 0x09, 't', 'M', 'o', 'd', 'u', 'l',	//scan name " tmodul"
	};


u8 	ui_ota_is_working = 0;
u8  ui_task_flg;
u32	ui_advertise_begin_tick;

void entry_ota_mode(void)
{
	ui_ota_is_working = 1;

	bls_ota_setTimeout(30 * 1000000); //set OTA timeout  30 S

	//gpio_write(GPIO_LED, 1);  //LED on for indicate OTA mode
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


void	task_connect (void)
{
	//bls_l2cap_requestConnParamUpdate (12, 32, 0, 400);
}


void rf_customized_param_load(void)
{
	  // customize freq_offset adjust cap value, if not customized, default ana_81 is 0xd0
	 if( (*(unsigned char*) CUST_CAP_INFO_ADDR) != 0xff ){
		 //ana_81<4:0> is cap value(0x00 - 0x1f)
		 analog_write(0x81, (analog_read(0x81)&0xe0) | ((*(unsigned char*) CUST_CAP_INFO_ADDR)&0x1f) );
	 }

	 //customize TP0, flash 0x77041 customize TP1
	 if( ((*(unsigned char*) (CUST_TP_INFO_ADDR)) != 0xff) && ((*(unsigned char*) (CUST_TP_INFO_ADDR+1)) != 0xff) ){
		 rf_update_tp_value(*(unsigned char*) (CUST_TP_INFO_ADDR), *(unsigned char*) (CUST_TP_INFO_ADDR+1));
	 }


	 // customize 32k RC cap value, if not customized, default ana_32 is 0x80
	 if( (*(unsigned char*) CUST_RC32K_CAP_INFO_ADDR) != 0xff ){
		 //ana_81<4:0> is cap value(0x00 - 0x1f)
		 analog_write(0x32, *(unsigned char*) CUST_RC32K_CAP_INFO_ADDR );
	 }
}

u32 tick_wakeup;
int	mcu_uart_working;
int	module_uart_working;
int module_task_busy;


int	module_uart_data_flg;
u32 module_wakeup_module_tick;

#define UART_TX_BUSY			( (hci_tx_fifo.rptr != hci_tx_fifo.wptr) || uart_tx_is_busy() )
#define UART_RX_BUSY			( rx_uart_w_index != rx_uart_r_index )

int app_module_busy ()
{
	mcu_uart_working = gpio_read(GPIO_WAKEUP_MODULE);  //mcu用GPIO_WAKEUP_MODULE指示 是否处于uart数据收发状态
	module_uart_working = UART_TX_BUSY || UART_RX_BUSY; //module自己检查uart rx和tx是否都处理完毕
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


#if (BLE_MODULE_INDICATE_DATA_TO_MCU)
	module_uart_working = UART_TX_BUSY || UART_RX_BUSY;


	//当module的uart数据发送完毕后，将GPIO_WAKEUP_MCU拉低或悬浮(取决于user怎么设计)
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
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);  //需要被 GPIO_WAKEUP_MODULE 唤醒
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
	rf_customized_param_load();  //load customized freq_offset cap value and tp value

	REG_ADDR8(0x74) = 0x53;
	REG_ADDR16(0x7e) = 0x08d1;
	REG_ADDR8(0x74) = 0x00;
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
	extern void my_att_init ();
	my_att_init ();

	//l2cap initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);

	//smp initialization
	//bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );


	///////////////////// USER application initialization ///////////////////

	bls_ll_setAdvData( tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData(tbl_scanRsp, sizeof(tbl_scanRsp));


	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_30MS, ADV_INTERVAL_30MS, \
			 	 	 	 	 	     ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
			 	 	 	 	 	     0,  NULL,  BLT_ENABLE_ADV_37, ADV_FP_NONE);

	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}

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
		rx_uart_r_index = 0;
		rx_uart_w_index = 0;
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
		uart_BuffInit((u8 *)(&T_rxdata_buf), sizeof(T_rxdata_buf), (u8 *)(&T_txdata_buf));
//		blc_register_hci_handler (blc_rx_from_uart, blc_hci_tx_to_uart);		//default handler
		extern int rx_from_uart_cb (void);
		extern int tx_to_uart_cb (void);
		blc_register_hci_handler(rx_from_uart_cb,tx_to_uart_cb);				//customized uart handler
	#endif
	extern void event_handler(u32 h, u8 *para, int n);
	bls_register_event_data_callback(event_handler);		//register event callback
	bls_hci_mod_setEventMask_cmd(0xffff);			//enable all 15 events,event list see ble_ll.h

	// OTA init
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);



#if (BLE_MODULE_PM_ENABLE)
	//mcu 可以通过拉高GPIO_WAKEUP_MODULE将 module从低低功耗唤醒
	gpio_set_wakeup		(GPIO_WAKEUP_MODULE, 1, 1);  // core(gpio) high wakeup suspend
	cpu_set_gpio_wakeup (GPIO_WAKEUP_MODULE, 1, 1);  // pad high wakeup deepsleep

	GPIO_WAKEUP_MODULE_LOW;

	bls_pm_registerFuncBeforeSuspend( &app_suspend_enter );
#endif

	ui_advertise_begin_tick = clock_time();
}


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
void main_loop ()
{
	static u32 tick_loop;

	tick_loop ++;

	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_slave_main_loop ();


	////////////////////////////////////// UI entry /////////////////////////////////
	//  add spp UI task
	app_power_management ();

}
