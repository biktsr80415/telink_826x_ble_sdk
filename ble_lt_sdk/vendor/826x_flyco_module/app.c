#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll/ll_whitelist.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "spp.h"
#if (HCI_ACCESS==HCI_USE_UART)
#include "../../proj/drivers/uart.h"
#endif

////////////// uart spp tx\rx fifo /////////////////
MYFIFO_INIT(hci_tx_fifo, 72, 8);//used in spp.c
MYFIFO_INIT(hci_rx_fifo, 72, 2);//used in spp.c

////////////// ble tx\rx fifo //////////////////////
MYFIFO_INIT(blt_rxfifo, 64, 8);
//MYFIFO_INIT(blt_txfifo, 40, 16);
MYFIFO_INIT(blt_txfifo, 80, 8);

#define UART_TX_BUSY			( (hci_tx_fifo.rptr != hci_tx_fifo.wptr) || uart_tx_is_busy() )
#define UART_RX_BUSY			( hci_rx_fifo.rptr != hci_rx_fifo.wptr)

u8 	ui_ota_is_working = 0;
extern u8 uart_task_terminate_flg;
extern u8 ble_connected_flg;
extern u32 ble_connected_tick;

//FLYCO project add
u8  bls_adv_enable = 1;
u32 adv_timeout    = 0;
u16 advinterval    = 0;
u16 advinterval_max= 0;
u32 baudrate       = 0;
u8  rfpower        = 0;
u8  advTem[20]     = {0};
u8  scanRspTem[20] = {0};
u8  identified[6]  = {0};
u8  devName[20]    = {0};
u8  baudratetmp[3] = {0};
u8  devName1[20]   = {DEFLUT_DEV_NAME1_LEN, DEFLUT_DEV_NAME1};
u8  devName2[20]   = {DEFLUT_DEV_NAME2_LEN, DEFLUT_DEV_NAME2};
//user data save in flash,used in initialization
extern int adv_interval_index;
extern int rf_power_index;
extern int adv_timeout_index;
extern int adv_data_index;
extern int devname_index;
extern int devname1_index;
extern int devname2_index;
extern int identified_index;
extern int baudrate_index;
extern  u8 ble_devName[MAX_DEV_NAME_LEN];//gatt device name init

extern void flyco_ble_OtaEraseNewFwSection(void);
extern void flyco_ble_setOtaResIndicateCb(ota_resIndicateCb_t cb);
extern void flyco_send_ota_result(int errorcode);
extern void flyco_ble_OtaEraseNewFwSection(void);
extern void flyco_ble_setOtaVersionCb(ota_versionCb_t cb);
extern void flyco_ble_setOtaStartCb(ota_startCb_t cb);
extern u32 flyco_blt_ota_timeout_us;
extern u32 flyco_blt_ota_start_tick;
extern ota_resIndicateCb_t flyco_otaResIndicateCb;
extern void flyco_start_reboot(void);

//flyco ota
#if FLYCO_OTA_ENABLE
void entry_ota_mode(void)
{
	ui_ota_is_working = 1;
	extern void flyco_ble_setOtaTimeout(u32 timeout_us);
	flyco_ble_setOtaTimeout(50 * 1000000); //set OTA timeout  50 S
}
void send_ota_fw_num(void){
	extern u8 flyco_version[4];
	extern u8 ota_hdl;
	bls_att_pushIndicateData(ota_hdl, flyco_version, sizeof(flyco_version));
}
#endif


#if DEBUG_FOR_TEST
volatile u8 id_uart_work;
volatile u8 test_is_uart_work;
#endif
void blt_pm_proc(void)
{
	//UART work state check,importment!!!
#if DEBUG_FOR_TEST
	id_uart_work = uart_ErrorCLR();//clear error state of uart rx, maybe used when application detected UART not work
#else
	uart_ErrorCLR();//clear error state of uart rx, maybe used when application detected UART not work
#endif

#if(BLE_PM_ENABLE)
	if(ui_ota_is_working){
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
		//OTA timeout process!!!
		if(clock_time_exceed(flyco_blt_ota_start_tick , flyco_blt_ota_timeout_us)){  //OTA timeout

			if(flyco_otaResIndicateCb){
				flyco_otaResIndicateCb(OTA_TIMEOUT);   //OTA fail indicate
			}

			sleep_us(2*bls_ll_getConnectionInterval()*(1250 * sys_tick_per_us));//wait for ota fail result sent

			extern int flyco_ota_adr_index;
			if(flyco_ota_adr_index>=0){
				irq_disable();
				for(int i=0;i<=flyco_ota_adr_index;i+=256){  //4K/16 = 256
					extern u32 new_firmware_address;
					flash_erase_sector(new_firmware_address + (i<<4));
				}
			}
			flyco_start_reboot();
		}
	}
	else{//ota not work

		//Once ble connected,waiting for 100ms,than open uart data send to Master or as a CMD!
		if(ble_connected_flg && clock_time_exceed(ble_connected_tick, 100000)){//After 100ms,turn on uart module
			ble_connected_flg = 0;
			//UART_ENABLE;
			gpio_set_input_en(GPIO_UTX, 1);
			gpio_set_input_en(GPIO_URX, 1);
		}

		//Once ble terminate,turn on suspend
		if(uart_task_terminate_flg && (!(UART_TX_BUSY || UART_RX_BUSY))){//terminate is true
			uart_task_terminate_flg = 0;
			FLYCO_BLE_STATE_LOW;

			//9600baudrate:1.2B/ms , 20Byte should use 18ms
			sleep_us(20000);//Add for the last uart tx fifo send, duling UART sending data, suspend will interrupt UART send data timing!

			bls_pm_setSuspendMask (SUSPEND_ADV);
			bls_pm_setWakeupSource(PM_WAKEUP_CORE);//setting suspend wakeup source
			//UART_DISABLE;
			gpio_set_input_en(GPIO_UTX, 0);
			gpio_set_input_en(GPIO_URX, 0);
		}
	}
#else
	if(ui_ota_is_working){
		//OTA timeout supervision
		extern u32 flyco_blt_ota_timeout_us;
		extern u32 flyco_blt_ota_start_tick;
		extern ota_resIndicateCb_t flyco_otaResIndicateCb;
		extern void flyco_start_reboot(void);

		//OTA timeout process!!!
		if(clock_time_exceed(flyco_blt_ota_start_tick , flyco_blt_ota_timeout_us)){  //OTA timeout

			if(flyco_otaResIndicateCb){
				flyco_otaResIndicateCb(OTA_TIMEOUT);   //OTA fail indicate
			}

			sleep_us(2*bls_ll_getConnectionInterval()*(1250 * sys_tick_per_us));//wait for ota fail result sent

			extern int flyco_ota_adr_index;
			if(flyco_ota_adr_index>=0){
				irq_disable();
				for(int i=0;i<=flyco_ota_adr_index;i+=256){  //4K/16 = 256
					extern u32 new_firmware_address;
					flash_erase_sector(new_firmware_address + (i<<4));
				}
			}
			flyco_start_reboot();
		}
	}
#endif


}

void task_connect (void)
{
	//bls_l2cap_requestConnParamUpdate (16, 40, 0, 400);  //interval=20ms~50ms latency=0 timeout=4s
}

// 0x77000:freqoffset compensation
// 0x77040:low freq compensation
// 0x77041:high freq compensation
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

void blt_system_power_optimize(void)  //to lower system power
{
	//disable_unnecessary_module_clock
	reg_rst_clk0 &= ~FLD_RST_SPI;  //spi not use
	reg_rst_clk0 &= ~FLD_RST_I2C;  //iic not use
	reg_rst_clk0 &= ~(FLD_RST_USB | FLD_RST_USB_PHY);//usb not use
}



u32	ui_advertise_begin_tick;

void user_init()
{
    /////////// load customized freq_offset cap value and tp value////////////
	rf_customized_param_load();

	blt_system_power_optimize();

	//////////////////////// ota callback register //////////////////////////
#if FLYCO_OTA_ENABLE
	//detect if should erase ota flash area!!!
	flyco_ble_OtaEraseNewFwSection();//bls_ota_setFirmwareSizeAndOffset(40, 0x40000);//setting ota new fw area
	flyco_ble_setOtaResIndicateCb(&flyco_send_ota_result);//ota process result show
	flyco_ble_setOtaStartCb(entry_ota_mode);              //1 min ota timeout process
	flyco_ble_setOtaVersionCb(send_ota_fw_num);           //ota fw_num show
#endif

	////////////////// flyco ble state indicate ./////////////////////////////
	gpio_set_func(BLE_STA_OUT, AS_GPIO);
	gpio_set_output_en(BLE_STA_OUT, 1);
	gpio_write(BLE_STA_OUT,0);//LOW


	//////////////////////////////////////////////////////////////////////////////
	//	Initialization: MAC address, Adv Packet, Response Packet
	//////////////////////////////////////////////////////////////////////////////
	u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};

	////////////////// BLE stack initialization ////////////////////////////////////
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff){
	    memcpy (tbl_mac, pmac, 6);
	}

	u8 tbl_advData[ ] = {
		0x02, //length
		0x01, 0x06,
		0x05, //length
		0x02, 0x00, 0xff, //FLYCO BLE Service UUID:0xFF00
			  0x12, 0x19, //Telink OTA service
		//https://www.bluetooth.com/specifications/assigned-numbers/Company-Identifiers [529]
		0x0B, //length
		0xff, 0x11, 0x02,  //Telink Semiconductor Co. Ltd. [Company ID] UUID:0x0211
		tbl_mac[5],tbl_mac[4],tbl_mac[3],tbl_mac[2],tbl_mac[1],tbl_mac[0],
		0x70,0x05, //Device Module Number:7005
	};

	u8 tbl_scanRsp [ ] = {
		0x13, 0x09, 'F', 'L', 'Y', 'C', 'O', ' ', 'F', 'H', '7', '0', '0', '5', '/', '6', '/', '8', 0x20, 0x00
	};

	advinterval       = DEFLUT_ADV_INTERVAL;
	advinterval_max   = DEFLUT_ADV_INTERVAL_MAX;
	rfpower           = DEFLUT_RF_PWR_LEVEL;//0dBm
	adv_timeout       = DEFLUT_ADV_TIMEOUT; //0s
	reverse_data(tbl_mac, BLE_ADDR_LEN, identified);// The default product identification code equale ble MAC address!

	memcpy(ble_devName, &DEV_NAME, 18);
	load_param_from_flash(DEV_NAME_ADDR, devName, 20);
	if((devName[0]) !=0){
		memset(ble_devName, 0, 18); //clear device name
		memcpy(ble_devName, devName + 2, devName[0] - 1);
	}
	//user param init load in flash
	load_param_from_flash(ADV_INTERVAL_ADDR, (u8 *)&advinterval, 2);
	load_param_from_flash(RF_POWER_ADDR, (u8 *)&rfpower, 1);
	load_param_from_flash(ADV_TIMEOUT_ADDR, (u8 *)&adv_timeout, 4);
	load_param_from_flash(ADV_DATA_ADDR, advTem, 20);
	load_param_from_flash(DEV_NAME_ADDR, scanRspTem, 20);
	load_param_from_flash(IDENTIFIED_ADDR, identified, 6);
	load_param_from_flash(BAUD_RATE_ADDR, baudratetmp, 3);
	load_param_from_flash(DEV_NAME1_ADDR, devName1, 20);
	load_param_from_flash(DEV_NAME2_ADDR, devName2, 20);
	//if should erase user data in flash area
	param_clear_flash(ADV_INTERVAL_ADDR);
	param_clear_flash(RF_POWER_ADDR);
	param_clear_flash(ADV_TIMEOUT_ADDR);
	param_clear_flash(ADV_DATA_ADDR);
	param_clear_flash(DEV_NAME_ADDR);
	param_clear_flash(IDENTIFIED_ADDR);
	param_clear_flash(BAUD_RATE_ADDR);
	param_clear_flash(DEV_NAME1_ADDR);
	param_clear_flash(DEV_NAME2_ADDR);

	if(advinterval_max < advinterval){
		advinterval_max = advinterval;
	}
	if(advTem[0])
		bls_ll_setAdvData( advTem+1, advTem[0]);
	else
		bls_ll_setAdvData( tbl_advData, sizeof(tbl_advData) );
	if(scanRspTem[0])
		bls_ll_setScanRspData(scanRspTem+1, scanRspTem[0]);
	else
		bls_ll_setScanRspData(tbl_scanRsp, sizeof(tbl_scanRsp));

	//adv timeout unit:us
	bls_ll_setAdvDuration(adv_timeout, adv_timeout == 0 ? 0 : 1);//close adv timeout




///////////// BLE stack Initialization ////////////////
	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory

	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
	blc_ll_initPowerManagement_module();        //pm module:      	 optional



	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization

	//bls_smp_enableParing (SMP_PARING_CONN_TRRIGER ); 	//smp initialization




	ble_sts_t status =  \
	bls_ll_setAdvParam( advinterval, advinterval_max, \
					    ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
					 	0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
#if DEBUG_FOR_TEST
	if(status != BLE_SUCCESS){
		while(1); //JSUT for debug
	}
#endif

    ///////////////////////// register event callback ///////////////////////////
	extern int ble_event_handler(u32 h, u8 *para, int n);
	blc_hci_registerControllerEventHandler(ble_event_handler); //register event callback
	bls_hci_mod_setEventMask_cmd(0x0018);//enable conn\terminate events,event list see ble_ll.h

    rf_set_power_level_index (rfpower);
	bls_ll_setAdvEnable(1);  //adv enable
    printf("Adv enable.\n");
///////////////// wakeup source configuration ////////////////////////////
#if BLE_PM_ENABLE
	gpio_set_wakeup(BRTS_WAKEUP_MODULE, 1, 1);  	                 //drive pin core(gpio) high wakeup suspend
	cpu_set_gpio_wakeup (BRTS_WAKEUP_MODULE, BRTS_WAKEUP_LEVEL, 1);  //drive pin pad high wakeup deepsleep
	bls_pm_setSuspendMask (SUSPEND_ADV);
#endif

//////////////////// SPP initialization ///////////////////////////////////
#if (HCI_ACCESS==HCI_USE_USB)
	//usb_bulk_drv_init (0);
	//blc_register_hci_handler (blc_hci_rx_from_usb, blc_hci_tx_to_usb);
#else	//uart
	//one gpio should be configured to act as the wakeup pin if in power saving mode; pending
	//todo:uart init here
	gpio_set_func(GPIO_UTX, AS_UART);
	gpio_set_func(GPIO_URX, AS_UART);
#if BLE_PM_ENABLE
	//UART_DISABLE;
	gpio_set_input_en(GPIO_UTX, 0);
	gpio_set_input_en(GPIO_URX, 0);
#endif
	gpio_set_input_en(GPIO_UTX, 1);
	gpio_set_input_en(GPIO_URX, 1);
	gpio_write (GPIO_UTX, 1);			//pull-high RX to avoid mis-trig by floating signal
	gpio_write (GPIO_URX, 1);			//pull-high RX to avoid mis-trig by floating signal

	if(baudratetmp[2] != 0xef)
		baudrate = (baudratetmp[0]<<16) + (baudratetmp[1]<<8) + baudratetmp[2];
	else
		baudrate = (baudratetmp[0]<<8) + baudratetmp[1];
	if(baudrate == 9600)
		CLK16M_UART9600;
	else if(baudrate == 115200)
		CLK16M_UART115200;
	else{//default baud rate
		baudrate = 9600;
		CLK16M_UART9600;
	}
	uart_BuffInit(hci_rx_fifo_b, hci_rx_fifo.size, hci_tx_fifo_b);
	reg_dma_rx_rdy0 = FLD_DMA_UART_RX | FLD_DMA_UART_TX; //clear uart rx/tx status
	blc_register_hci_handler (flyco_rx_from_uart, flyco_tx_to_uart);//flyco spp uart handle
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
	blt_sdk_main_loop ();

	blt_pm_proc();

	////////////////////////////////////// UI entry /////////////////////////////////
	blt_user_timerCb_proc();
}
