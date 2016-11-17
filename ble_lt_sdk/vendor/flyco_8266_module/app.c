#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll_whitelist.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "spp.h"

#if (HCI_ACCESS==HCI_USE_UART)
#include "../../proj/drivers/uart.h"
#endif

#define				CFG_ADR_MAC				0x76000
#define 			CUST_CAP_INFO_ADDR		0x77000
#define 			CUST_TP_INFO_ADDR		0x77040

MYFIFO_INIT(blt_rxfifo, 64, 8);

MYFIFO_INIT(blt_txfifo, 40, 16);

u32 ui_advertise_begin_tick;
//FLYCO project add
u8 bls_adv_enable  = 1;
u32 adv_timeout    = 0;//advertise timeout
u16 advinterval    = 0;//advertise interval
u32 baudrate       = 0;
u8  rfpower        = 0;
u8  advTem[20]     = {0};
u8  scanRspTem[20] = {0};
u8  identified[6]  = {0};
u8  devName[20]    = {0};
u8  baudratetmp[3] = {0};
u8  devName1[20]   = {DEFLUT_DEV_NAME1_LEN, DEFLUT_DEV_NAME1};
u8  devName2[20]   = {DEFLUT_DEV_NAME2_LEN, DEFLUT_DEV_NAME2};

u8 	ui_ota_is_working = 0;
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
//gatt device name init
extern  u8 ble_devName[MAX_DEV_NAME_LEN];

//flyco ota
#if FLYCO_OTA_ENABLE
void entry_ota_mode(void)
{
	ui_ota_is_working = 1;
	bls_ota_setTimeout(50 * 1000000); //set OTA timeout  50 S
}
void send_ota_fw_num(void){
	extern u8 flyco_version[4];
	extern u8 ota_hdl;
	bls_att_pushIndicateData(ota_hdl, flyco_version, sizeof(flyco_version));
}
void blt_pm_proc(void)
{
#if(BLE_PM_ENABLE)
	if(ui_ota_is_working || gpio_read(BRTS_WAKEUP_MODULE)){
		bls_pm_setSuspendMask (SUSPEND_DISABLE);
	}
	else{
		bls_pm_setSuspendMask (SUSPEND_ADV);
	}
#endif //END of  BLE_PM_ENABLE
}

#endif

//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [6] = {0x16 , 0xfd, 0x62, 0x38, 0xc1,0xa4 };

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

void blt_system_power_optimize(void)  //to lower system power
{
	//disable_unnecessary_module_clock
	reg_rst_clk0 &= ~FLD_RST_SPI;  //spi not use
	reg_rst_clk0 &= ~FLD_RST_I2C;  //iic not use
	reg_rst_clk0 &= ~(FLD_RST_USB | FLD_RST_USB_PHY);//usb not use
}

void user_init()
{
	rf_customized_param_load();  //load customized freq_offset cap value and tp value
	blt_system_power_optimize();
//	usb_log_init ();
//	usb_dp_pullup_en (1);  //open USB enum

	////////////////// BLE stack initialization ////////////////////////////////////
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff)
	{
	    memcpy (tbl_mac, pmac, 6);
	}
    else
    {
        //TODO : should write mac to flash after pair OK
        //tbl_mac[0] = (u8)rand();
        //flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
    }

	u8 tbl_advData[ ] = {
		0x02, //length
		0x01, 0x05,
		0x05, //length
		0x02, 0x00, 0xff, //FLYCO BLE Service UUID:0xFF00
			  0x12, 0x19, //Telink OTA service
		//https://www.bluetooth.com/specifications/assigned-numbers/Company-Identifiers [529]
		0x0B, //length
		0xff, 0x02, 0x11,  //Telink Semiconductor Co. Ltd. [Company ID] UUID:0x0211
		tbl_mac[5],tbl_mac[4],tbl_mac[3],tbl_mac[2],tbl_mac[1],tbl_mac[0],
		0x70,0x05, //Device Module Number:7005
	};

	u8 tbl_scanRsp [ ] = {
		0x13, 0x09, 'F', 'L', 'Y', 'C', 'O', ' ', 'F', 'H', '7', '0', '0', '5', '/', '6', '/', '8', 0x20, 0x00
	};

	///////////////////////////////init data///////////////////////////////////
	advinterval       = DEFLUT_ADV_INTERVAL;//30ms
	rfpower           = DEFLUT_RF_PWR_LEVEL;//0dB
	adv_timeout       = DEFLUT_ADV_TIMEOUT;//0s
	reverse_data(tbl_mac, BLE_ADDR_LEN, identified);// The default product identification code equale ble MAC address!
	//device name init
	memcpy(ble_devName, &DEV_NAME, 18);
	flyco_load_para_addr(DEV_NAME_ADDR, &devname_index, devName, 20);
	if((devName[0]) !=0){
		memset(ble_devName, 0, 18); //clear device name
		memcpy(ble_devName, devName + 2, devName[0] - 1);
	}
	//user para init load in flash
	flyco_load_para_addr(ADV_INTERVAL_ADDR, &adv_interval_index, (u8 *)&advinterval, 2);
	flyco_load_para_addr(RF_POWER_ADDR, &rf_power_index, (u8 *)&rfpower, 1);
	flyco_load_para_addr(ADV_TIMEOUT_ADDR, &adv_timeout_index, (u8 *)&adv_timeout, 4);
	flyco_load_para_addr(ADV_DATA_ADDR, &adv_data_index, advTem, 20);
	flyco_load_para_addr(DEV_NAME_ADDR, &devname_index, scanRspTem, 20);
	flyco_load_para_addr(IDENTIFIED_ADDR, &identified_index, identified, 6);
	flyco_load_para_addr(BAUD_RATE_ADDR, &baudrate_index, baudratetmp, 3);
	//if should erase user data in flash area
	flyco_erase_para(ADV_INTERVAL_ADDR, &adv_interval_index);
	flyco_erase_para(RF_POWER_ADDR, &rf_power_index);
	flyco_erase_para(ADV_TIMEOUT_ADDR, &adv_timeout_index);
	flyco_erase_para(ADV_DATA_ADDR, &adv_data_index);
	flyco_erase_para(DEV_NAME_ADDR, &devname_index);
	flyco_erase_para(IDENTIFIED_ADDR, &identified_index);
	flyco_erase_para(BAUD_RATE_ADDR, &baudrate_index);
	flyco_erase_para(DEV_NAME1_ADDR, &devname1_index);
	flyco_erase_para(DEV_NAME2_ADDR, &devname2_index);
	//adv timeout
	bls_ll_setAdvDuration(adv_timeout, adv_timeout == 0 ? 0 : 1);//close adv timeout
	//adv /rsp parameter init
	if(advTem[0]){
		bls_ll_setAdvData( advTem+1, advTem[0]);
	}
	else{
		bls_ll_setAdvData( tbl_advData, sizeof(tbl_advData) );
	}
	//scan_rsp init
	if(scanRspTem[0]){
		bls_ll_setScanRspData(scanRspTem+1, scanRspTem[0]);
	}
	else{
		bls_ll_setScanRspData(tbl_scanRsp, sizeof(tbl_scanRsp));
	}///////////////////////////////init data///////////////////////////////////

	//link layer initialization
	bls_ll_init (tbl_mac);

	//gatt initialization
    //NOTE: my_att_init  must after bls_ll_init, and before bls_ll_setAdvParam
	extern void my_att_init ();
	my_att_init ();

	//l2cap initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);

	//smp initialization
	bls_smp_enableParing (SMP_PARING_DISABLE_TRRIGER );

	u8 status = bls_ll_setAdvParam( advinterval, advinterval, \
				 	 	 	 	 	ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
				 	 	 	 	 	0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);
	bls_ll_setAdvEnable(1);  //adv enable

	rf_set_power_level_index (rfpower);

	//flyco ble state indicate.
	gpio_set_func(BLE_STA_OUT, AS_GPIO);
	gpio_set_output_en(BLE_STA_OUT, 1);
	gpio_write(BLE_STA_OUT,0);//LOW

	//Wakeup source configuration
	gpio_set_wakeup(BRTS_WAKEUP_MODULE, 1, 1);  	   //drive pin core(gpio) high wakeup suspend
	cpu_set_gpio_wakeup (BRTS_WAKEUP_MODULE, BRTS_WAKEUP_LEVEL, 1);  //drive pin pad high wakeup deepsleep
	gpio_core_wakeup_enable_all(1);

	////////////////// SPP initialization ///////////////////////////////////
	#if (HCI_ACCESS==HCI_USE_USB)
		//usb_bulk_drv_init (0);
		//blc_register_hci_handler (blc_hci_rx_from_usb, blc_hci_tx_to_usb);
	#else	//uart
		//one gpio should be configured to act as the wakeup pin if in power saving mode; pending
		//todo:uart init here
	    rx_uart_r_index = -1;//should set as -1,otherwise we should send data to Uart twice, the second time,spp function will work
		rx_uart_w_index = 0;
		gpio_set_func(GPIO_UTX, AS_UART);
		gpio_set_func(GPIO_URX, AS_UART);
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
		uart_BuffInit((u8 *)(&T_rxdata_buf), sizeof(T_rxdata_buf), (u8 *)(&T_txdata_buf));
//		blc_register_hci_handler (blc_rx_from_uart, blc_hci_tx_to_uart);		//default handler
		blc_register_hci_handler (flyco_rx_from_uart, flyco_tx_to_uart);//customized uart handler
	#endif
	extern int ble_event_handler(u32 h, u8 *para, int n);
	bls_register_event_data_callback(ble_event_handler);		//register event callback
	bls_hci_mod_setEventMask_cmd(0x7fff);			//enable all 15 events,event list see ble_ll.h

	// OTA init
#if FLYCO_OTA_ENABLE
	//OTA register callback function
	extern void flyco_ble_setOtaResIndicateCb(ota_resIndicateCb_t cb);
	extern void flyco_send_ota_result(int errorcode);
	flyco_ble_setOtaResIndicateCb(&flyco_send_ota_result);
	flyco_ble_setOtaStartCb(entry_ota_mode);
	flyco_ble_setOtaVersionCb(send_ota_fw_num);
#endif

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
	blt_user_timerCb_proc();

	blt_pm_proc();
}
