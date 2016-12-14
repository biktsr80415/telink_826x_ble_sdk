#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll_whitelist.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "spp_8269.h"


MYFIFO_INIT(hci_tx_fifo, 72, 8);

MYFIFO_INIT(blt_rxfifo, 64, 8);

//MYFIFO_INIT(blt_txfifo, 40, 16);
MYFIFO_INIT(blt_txfifo, 80, 8);

extern void task_host(void);
extern u8 spp_task_finished_flg;

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
int	module_is_working;
int	module_spi_working;
int module_task_busy;

u32 module_wakeup_module_tick;

int app_module_busy ()
{
	module_is_working = gpio_read(SSPI_PM_WAKEUP_PIN);//态
	module_spi_working = spp_task_finished_flg;
	module_task_busy = module_is_working || module_spi_working;
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


	//锟斤拷module锟斤拷uart锟斤拷莘锟斤拷锟斤拷锟较后，斤拷GPIO_WAKEUP_MCU锟斤拷锟酵伙拷锟斤拷(取锟斤拷锟斤拷user锟斤拷么锟斤拷锟�
	if(module_uart_data_flg && !module_uart_working){
		module_uart_data_flg = 0;
		module_wakeup_module_tick = 0;
		GPIO_WAKEUP_MCU_LOW;
	}
#endif


	// pullup GPIO_WAKEUP_MODULE: exit from suspend
	// pulldown GPIO_WAKEUP_MODULE: enter suspend

#if (PM_ENABLE)

	if (!app_module_busy() && !tick_wakeup)
	{
		bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);  //锟斤拷要锟斤拷 GPIO_WAKEUP_MODULE 锟斤拷锟斤拷
	}

	if (tick_wakeup && clock_time_exceed (tick_wakeup, 500))
	{
		GPIO_WAKEUP_MODULE_LOW;
		tick_wakeup = 0;
	}

#endif
}



void led_init(void){
	gpio_set_func(RED_LED, AS_GPIO);//LED RED EVK 8269
	gpio_set_input_en(RED_LED,0);
	gpio_set_output_en(RED_LED,1);
	gpio_write(RED_LED,1);
//////////////////////////////////////
	gpio_set_func(GREEN_LED, AS_GPIO);
	gpio_set_input_en(GREEN_LED,0);
	gpio_set_output_en(GREEN_LED,1);
	gpio_write(GREEN_LED,1);
/////////////////////////////////////
	gpio_set_func(WHITE_LED, AS_GPIO);
	gpio_set_input_en(WHITE_LED,0);
	gpio_set_output_en(WHITE_LED,1);
	gpio_write(WHITE_LED,1);
/////////////////////////////////////
	sleep_us(200000);
	gpio_write(RED_LED,0);
	gpio_write(GREEN_LED,0);
	gpio_write(WHITE_LED,0);
}

void led_onoff(u32 pin,u8 onoff){
	gpio_write(pin, onoff&&0x01);
}

//SPI function
void sspi_init(int pin, int divider){
	//SPI B port: GPIO:B<4~7>
	write_reg8(0x58e,read_reg8(0x58e)&0x0f);///disable GPIO:B<4~7>
	write_reg8(0x5b1,read_reg8(0x5b1)|0xf0);///enable SPI function:B<4~7>
	write_reg8(0x5b0,read_reg8(0x5b0)&0xC3);///disable SPI function:A<2~5>
	gpio_set_input_en(GPIO_PB4,1);
	gpio_set_input_en(GPIO_PB5,1);
	gpio_set_input_en(GPIO_PB6,1);
	gpio_set_input_en(GPIO_PB7,1);

	//div_clock: bit[6:0]
	write_reg8(0x0a,read_reg8(0x0a)|divider);// spi clock=system clock/((div_clock+1)*2)
	write_reg8(0x09,read_reg8(0x09)&0xfd);// disable master mode
	write_reg8(0x0b,read_reg8(0x0b)|SPI_MODE0);// select SPI mode,surpport four modes

	reg_spi_ctrl = FLD_SPI_ADDR_AUTO;
	reg_spi_sp = FLD_SPI_ENABLE;			//force PADs act as spi

	// pin used to notify the MSPI(8267 EVK), SSPI's data has been sent to MSPI
	gpio_set_func(pin, AS_GPIO);
	gpio_write(pin, 0);
	gpio_set_input_en(pin, 1);//gpio_set_output_en(pin, 1);//out

	//register CN pin's irq,when MSPI's data sent complete,the CN changed HIGH
	gpio_set_input_en(GPIO_PB4, 1); //GPIO_PB4 SPI B port : CN
	gpio_set_interrupt(GPIO_PB4, 0);	// rising edge

	reg_irq_mask |= FLD_IRQ_GPIO_RISC2_EN;

}

void user_init()
{
	rf_customized_param_load();  //load customized freq_offset cap value and tp value

    led_init();

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

#if(PM_ENABLE)
	gpio_set_wakeup(SSPI_PM_WAKEUP_PIN,1,1);//drive pin core(gpio) high wakeup suspend

	GPIO_WAKEUP_MODULE_LOW;

	bls_pm_registerFuncBeforeSuspend( &app_suspend_enter );

#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif

	////////////////// SPI initialization ///////////////////////////////////
	sspi_init(SSPI_TX_NOTIFY_PIN, 0x25);//SSPI_TX_NOTIFY_PIN : 8269 EVK G2

	extern void event_handler(u32 h, u8 *para, int n);
	bls_hci_registerEventHandler(event_handler);		//register event callback
	bls_hci_mod_setEventMask_cmd(0xffff);			//enable all 15 events,event list see ble_ll.h

	// OTA init
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);

	ui_advertise_begin_tick = clock_time();
}

void blt_pm_proc(void){
//Wakeup source setting
#if PM_ENABLE
	if((spp_task_finished_flg) || gpio_read(SSPI_PM_WAKEUP_PIN)){
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
	}
	else{
		bls_pm_setSuspendMask(SUSPEND_ADV | SUSPEND_CONN);
		bls_pm_setWakeupSource(PM_WAKEUP_CORE);
	}

#endif
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

	task_host();

	////////////////////////////////////// UI entry /////////////////////////////////
	//  add spp UI task
	blt_pm_proc();

}
