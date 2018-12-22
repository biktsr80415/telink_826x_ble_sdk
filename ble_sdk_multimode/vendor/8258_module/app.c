/********************************************************************************************************
 * @file     app.c 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
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
#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/blt_common.h"

#include "spp.h"
#include "battery_check.h"

//module spp Tx / Rx fifo
//MYFIFO_INIT(spp_rx_fifo, 72, 2);
//MYFIFO_INIT(spp_tx_fifo, 72, 8);


#define SPP_RXFIFO_SIZE		72
#define SPP_RXFIFO_NUM		2

#define SPP_TXFIFO_SIZE		72
#define SPP_TXFIFO_NUM		8

_attribute_data_retention_  u8 		 	spp_rx_fifo_b[SPP_RXFIFO_SIZE * SPP_RXFIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	spp_rx_fifo = {
												SPP_RXFIFO_SIZE,
												SPP_RXFIFO_NUM,
												0,
												0,
												spp_rx_fifo_b,};

_attribute_data_retention_  u8 		 	spp_tx_fifo_b[SPP_TXFIFO_SIZE * SPP_TXFIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	spp_tx_fifo = {
												SPP_TXFIFO_SIZE,
												SPP_TXFIFO_NUM,
												0,
												0,
												spp_tx_fifo_b,};



//RF Tx / Rx fifo
//MYFIFO_INIT(blt_rxfifo, 64, 8);
//MYFIFO_INIT(blt_txfifo, 40, 16);


#define RX_FIFO_SIZE	64
#define RX_FIFO_NUM		8

#define TX_FIFO_SIZE	40
#define TX_FIFO_NUM		16



_attribute_data_retention_  u8 		 	blt_rxfifo_b[RX_FIFO_SIZE * RX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_rxfifo = {
												RX_FIFO_SIZE,
												RX_FIFO_NUM,
												0,
												0,
												blt_rxfifo_b,};


_attribute_data_retention_  u8 		 	blt_txfifo_b[TX_FIFO_SIZE * TX_FIFO_NUM] = {0};
_attribute_data_retention_	my_fifo_t	blt_txfifo = {
												TX_FIFO_SIZE,
												TX_FIFO_NUM,
												0,
												0,
												blt_txfifo_b,};















#define     MY_APP_ADV_CHANNEL					BLT_ENABLE_ADV_37
#define 	MY_ADV_INTERVAL_MIN					ADV_INTERVAL_30MS
#define 	MY_ADV_INTERVAL_MAX					ADV_INTERVAL_40MS

#define 	MY_DIRECT_ADV_TMIE					2000000  //us

#define		MY_RF_POWER_INDEX					RF_POWER_P3p01dBm






//////////////////////////////////////////////////////////////////////////////
//	Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
const u8	tbl_advData[] = {
	 0x05, 0x09, 'k', 'M', 'o', 'd',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
};

const u8	tbl_scanRsp [] = {
		 0x07, 0x09, 'k', 'M', 'o', 'd', 'u', 'l',	//scan name " tmodul"
	};


_attribute_data_retention_	u8 	ui_ota_is_working = 0;

_attribute_data_retention_	u32	lowBattDet_tick   = 0;



_attribute_data_retention_ u8 conn_update_cnt;

int app_conn_param_update_response(u8 id, u16  result)
{
#if 0
	if(result == CONN_PARAM_UPDATE_ACCEPT){
		printf("SIG: the LE master Host has accepted the connection parameters.\n");
		conn_update_cnt = 0;
	}
	else if(result == CONN_PARAM_UPDATE_REJECT)
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
#endif

	return 0;
}



#if (BLE_OTA_ENABLE)
void entry_ota_mode(void)
{
	bls_ota_setTimeout(15 * 1000 * 1000); //set OTA timeout  15 seconds

	#if(BLT_APP_LED_ENABLE)
		gpio_set_output_en(GPIO_LED, 1);  //output enable
		gpio_write(GPIO_LED, 1);  //LED on for indicate OTA mode
	#endif
}

void show_ota_result(int result)
{

	#if(1 && BLT_APP_LED_ENABLE)
		gpio_set_output_en(GPIO_LED, 1);

		if(result == OTA_SUCCESS){  //OTA success
			gpio_write(GPIO_LED, 1);
			sleep_us(500000);
			gpio_write(GPIO_LED, 0);
			sleep_us(500000);
			gpio_write(GPIO_LED, 1);
			sleep_us(500000);
			gpio_write(GPIO_LED, 0);
			sleep_us(500000);
		}
		else{  //OTA fail

			#if 0 //this is only for debug,  can not use this in application code
				irq_disable();
				WATCHDOG_DISABLE;

				write_reg8(0x40000, 0x33);
				while(1){
					gpio_write(GPIO_LED, 1);
					sleep_us(200000);
					gpio_write(GPIO_LED, 0);
					sleep_us(200000);
				}
				write_reg8(0x40000, 0x44);
			#endif

		}

		gpio_set_output_en(GPIO_LED, 0);
	#endif
}
#endif


#define MAX_INTERVAL_VAL		16






_attribute_data_retention_	u32 tick_wakeup;
_attribute_data_retention_	int	mcu_uart_working;
_attribute_data_retention_	int	module_uart_working;
_attribute_data_retention_	int module_task_busy;


_attribute_data_retention_	int	module_uart_data_flg;
_attribute_data_retention_	u32 module_wakeup_module_tick;

#define UART_TX_BUSY			( (spp_tx_fifo.rptr != spp_tx_fifo.wptr) || uart_tx_is_busy() )
#define UART_RX_BUSY			(spp_rx_fifo.rptr != spp_rx_fifo.wptr)

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

#if (BLE_MODULE_INDICATE_DATA_TO_MCU)
	module_uart_working = UART_TX_BUSY || UART_RX_BUSY;

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
		#if (PM_DEEPSLEEP_RETENTION_ENABLE)
			bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
		#else
			bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
		#endif

		bls_pm_setWakeupSource(PM_WAKEUP_PAD);  // GPIO_WAKEUP_MODULE needs to be wakened
	}

	if (tick_wakeup && clock_time_exceed (tick_wakeup, 500))
	{
		GPIO_WAKEUP_MODULE_LOW;
		tick_wakeup = 0;
	}

#endif
}


void user_init_normal(void)
{
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value

	random_generator_init();  //this is must

////////////////// BLE stack initialization ////////////////////////////////////
	u8  tbl_mac [6];  //BLE public address
	blc_initMacAddress(CFG_ADR_MAC, tbl_mac, NULL);

	////// Controller Initialization  //////////
	blc_ll_initBasicMCU();                      //mandatory
	blc_ll_initStandby_module(tbl_mac);				//mandatory
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,




	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization

	blc_l2cap_registerConnUpdateRspCb(app_conn_param_update_response);         //register sig process handler


	//// smp initialization ////
 	//// smp initialization ////
#if (BLE_SECURITY_ENABLE)
	blc_smp_param_setBondingDeviceMaxNumber(4);  	//default is SMP_BONDING_DEVICE_MAX_NUM, can not bigger that this value
													//and this func must call before bls_smp_enableParing
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );
#else
	bls_smp_enableParing (SMP_PARING_DISABLE_TRRIGER );
#endif

	///////////////////// USER application initialization ///////////////////

	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));



	////////////////// config adv packet /////////////////////
	u8 status = bls_ll_setAdvParam( MY_ADV_INTERVAL_MIN, MY_ADV_INTERVAL_MAX,
								 ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC,
								 0,  NULL,
								 MY_APP_ADV_CHANNEL,
								 ADV_FP_NONE);

	if(status != BLE_SUCCESS) { write_reg8(0x40002, 0x11); 	while(1); }  //debug: adv setting err


	bls_ll_setAdvEnable(1);  //adv enable
	rf_set_power_level_index (MY_RF_POWER_INDEX);






	////////////////// SPP initialization ///////////////////////////////////
	//note: dma addr must be set first before any other uart initialization! (confirmed by sihui)
	u8 *uart_rx_addr = (spp_rx_fifo_b + (spp_rx_fifo.wptr & (spp_rx_fifo.num-1)) * spp_rx_fifo.size);
	uart_recbuff_init( (unsigned short *)uart_rx_addr, spp_rx_fifo.size);

	uart_gpio_set(UART_TX_PB1, UART_RX_PB0);

	uart_reset();  //will reset uart digital registers from 0x90 ~ 0x9f, so uart setting must set after this reset

	//baud rate: 115200
	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
		uart_init(9, 13, PARITY_NONE, STOP_BIT_ONE);
	#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
		uart_init(12, 15, PARITY_NONE, STOP_BIT_ONE);
	#endif

	uart_dma_enable(1, 1); 	//uart data in hardware buffer moved by dma, so we need enable them first

	irq_set_mask(FLD_IRQ_DMA_EN);
	dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 1);   	//uart Rx/Tx dma irq enable

	uart_irq_enable(0, 0);  	//uart Rx/Tx irq no need, disable them

	extern int rx_from_uart_cb (void);
	extern int tx_to_uart_cb (void);
	blc_register_hci_handler(rx_from_uart_cb, tx_to_uart_cb);				//customized uart handler


	extern int event_handler(u32 h, u8 *para, int n);
	blc_hci_registerControllerEventHandler(event_handler);		//register event callback
	bls_hci_mod_setEventMask_cmd(0xfffff);			//enable all 18 events,event list see ll.h





#if (BLE_MODULE_PM_ENABLE)
	blc_ll_initPowerManagement_module();        //pm module:      	 optional

	#if (PM_DEEPSLEEP_RETENTION_ENABLE)
		bls_pm_setSuspendMask (SUSPEND_ADV | DEEPSLEEP_RETENTION_ADV | SUSPEND_CONN | DEEPSLEEP_RETENTION_CONN);
		blc_pm_setDeepsleepRetentionThreshold(95, 95);
		blc_pm_setDeepsleepRetentionEarlyWakeupTiming(400);
	#else
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	#endif


	//mcu can wake up module from suspend or deepsleep by pulling up GPIO_WAKEUP_MODULE
	cpu_set_gpio_wakeup (GPIO_WAKEUP_MODULE, Level_High, 1);  // pad high wakeup deepsleep

	GPIO_WAKEUP_MODULE_LOW;

	bls_pm_registerFuncBeforeSuspend( &app_suspend_enter );
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif


#if (BATT_CHECK_ENABLE)  //battery check must do before OTA relative operation
	if(analog_read(DEEP_ANA_REG2) ==  LOW_BATT_FLG){
		app_battery_power_check(VBAT_ALRAM_THRES_MV + 200);  //2.2 V
	}
#endif


#if (BLE_OTA_ENABLE)
	// OTA init
	bls_ota_clearNewFwDataArea(); //must
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);
#endif

}


#if (PM_DEEPSLEEP_RETENTION_ENABLE)
_attribute_ram_code_ void user_init_deepRetn(void)
{
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value

	blc_ll_initBasicMCU();   //mandatory
	rf_set_power_level_index (MY_RF_POWER_INDEX);

	blc_ll_recoverDeepRetention();

	irq_enable();

	DBG_CHN0_HIGH;    //debug


	////////////////// SPP initialization ///////////////////////////////////
	//note: dma addr must be set first before any other uart initialization! (confirmed by sihui)

	u8 *uart_rx_addr = (spp_rx_fifo_b + (spp_rx_fifo.wptr & (spp_rx_fifo.num-1)) * spp_rx_fifo.size);
	uart_recbuff_init( (unsigned short *)uart_rx_addr, spp_rx_fifo.size);


	uart_gpio_set(UART_TX_PB1, UART_RX_PB0);

	uart_reset();  //will reset uart digital registers from 0x90 ~ 0x9f, so uart setting must set after this reset


	DBG_CHN0_LOW;  //debug

	//baud rate: 115200
	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
		uart_init(9, 13, PARITY_NONE, STOP_BIT_ONE);
	#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
		uart_init(12, 15, PARITY_NONE, STOP_BIT_ONE);
	#endif

	uart_dma_enable(1, 1); 	//uart data in hardware buffer moved by dma, so we need enable them first

	irq_set_mask(FLD_IRQ_DMA_EN);
	dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 1);   	//uart Rx/Tx dma irq enable

	uart_irq_enable(0, 0);  	//uart Rx/Tx irq no need, disable them

	//mcu can wake up module from suspend or deepsleep by pulling up GPIO_WAKEUP_MODULE
	cpu_set_gpio_wakeup (GPIO_WAKEUP_MODULE, Level_High, 1);  // pad high wakeup deepsleep

	GPIO_WAKEUP_MODULE_LOW;




	DBG_CHN0_HIGH;   //debug
}
#endif

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
void main_loop (void)
{
	static u32 tick_loop;
	tick_loop ++;

	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop();

	////////////////////////////////////// UI entry /////////////////////////////////

#if (BATT_CHECK_ENABLE)
	if(battery_get_detect_enable() && clock_time_exceed(lowBattDet_tick, 500000) ){
		lowBattDet_tick = clock_time();
		app_battery_power_check(VBAT_ALRAM_THRES_MV);  //2000 mV low battery
	}
#endif


	//  add spp UI task
	app_power_management ();


	spp_restart_proc();
}
