#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll_whitelist.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../vendor/common/flyco_spp.h"

#include "../../proj/drivers/uart.h"


#define				CFG_ADR_MAC				0x76000
#define 			CUST_CAP_INFO_ADDR		0x77000
#define 			CUST_TP_INFO_ADDR		0x77040

//FLYCO project add
u32 adv_timeout = 0;//advertise timeout
u16 advinterval_min = 0;//advertise interval
u16 advinterval_max = 0;//advertise interval
u32 advinterval_times = 0;
u8 rfpower;

extern u8  ble_devName[MAX_DEV_NAME_LEN];
u32 adv_times_cnt = 0;//advertisement counts,used to store the total number of times the timing of the timer
extern u8 adv_enable_flag;//Broadcast enable status flag, decide whether module ble broadcast enable
u8 service_dsy_finished_flg = 0;//seivice discovery finished flag
u8 suspend_on_adv_flag = ADV_NEED_SUSPEND_FLG;//Defalut:The ble module can be on suspend state during broadcast!
u8 suspend_on_conn_flag = CONN_NEED_SUSPEND_FLG;//Add by tuyf 16-09-28

//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [6] = {0x16 , 0xfd, 0x62, 0x38, 0xc1,0xa4 };

u32	advertise_begin_tick;

u8 sendTerminate_before_enterDeep = 0;


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
	gpio_write (BLE_STA_OUT, 0);//Set 0
#if (SIHUI_DEBUG_BLE_SLAVE)
	bls_l2cap_requestConnParamUpdate (6, 6, 99, 400);   //7.5 ms
#else
	bls_l2cap_requestConnParamUpdate (12, 32, 99, 400);  //interval=10ms latency=99 timeout=4s
#endif
	//latest_user_event_tick = clock_time();
}

//user Timer callback proc for spp cmd ack
void blt_user_timerCb_proc(void){
	extern u32 spp_cmd_restart_tick;
	extern u32 spp_cmd_disconnect_tick;
	extern u32 spp_cmd_deep_sleep_tick;
	extern u8 spp_cmd_restart_flg;
	extern u8 spp_cmd_disconnect_flg;
	extern u8 spp_cmd_deep_sleep_flg;

	if(spp_cmd_restart_flg && clock_time_exceed(spp_cmd_restart_tick , 20000)){  //spp cmd ack restart timeout
		spp_cmd_restart_flg =0;
		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, clock_time() + 5000 * sys_tick_per_us);
	}

	if(spp_cmd_disconnect_flg && clock_time_exceed(spp_cmd_disconnect_tick , 10000)){  //spp cmd ack disconnect timeout
		spp_cmd_disconnect_flg = 0;
		blt_ll_terminateConnetion(HCI_ERR_REMOTE_USER_TERM_CONN);
	}

	if(spp_cmd_deep_sleep_flg && clock_time_exceed(spp_cmd_deep_sleep_tick , 20000)){  //spp cmd ack deep sleep timeout
		spp_cmd_deep_sleep_flg = 0;
		cpu_sleep_wakeup(1, PM_WAKEUP_PAD, 0);
	}
}
/////////////////////////////////////////////////////////////////////
void blt_pm_proc(void)
{
#if(BLE_REMOTE_PM_ENABLE)
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
#endif  //END of  BLE_REMOTE_PM_ENABLE
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


//shut down some io before entry suspend,in case of io leakage
//restore them after suspend wakeup
//PC3:bias,if not shut down,150 uA leakage at suspend state

void io_isolate_before_suspend(void)
{

}

void restore_io_after_suspend(void)
{

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

void task_adv (u8 e, u8 *p){
	static u32 adv_no;
	gpio_write (BLE_STA_OUT, adv_no++ & BIT(2));
}

void ble_flyco_set_sleep_wakeup(u8 e, u8 *p){
	bls_pm_setWakeupSource(PM_WAKEUP_CORE|PM_WAKEUP_PAD);
}

/////////////////////////////////////spp process ///////////////////////////////////////////////////
#define		UART_WB_NUM			8
u8		uart_wb[UART_WB_NUM][64];
u8		uart_wb_wptr = 0;
u8		uart_wb_rptr = 0;

u8		uart_rb[256];

u8	flyco_uart_push_fifo (u16 st, int n, u8 *p)
{
	int num = (uart_wb_wptr - uart_wb_rptr) & 31;
	if (num >= UART_WB_NUM)
	{
		return 0;
	}
	u8 *pw = uart_wb[uart_wb_wptr++ & (UART_WB_NUM - 1)];
	pw[0] = n + 3;
	pw[1] = st;
	pw[2] = st >> 8;
	pw[3] = n;
	if (n)
	{
		memcpy (pw + 4, p, n);
	}
	return 1;
}

u32 rx_len;
int flyco_blc_rx_from_uart (void)//UART data send to Master,we will handler the data as CMD or DATA
{
	if(rx_uart_w_index==rx_uart_r_index)  //rx buff empty
	{
        return 0;
	}

	rx_len = T_rxdata_buf[rx_uart_w_index].len + 4 > sizeof(T_rxdata_for_flyco) ? sizeof(T_rxdata_for_flyco) : T_rxdata_buf[rx_uart_w_index].len + 4;
	memcpy(&T_rxdata_for_flyco, &T_rxdata_buf[rx_uart_w_index], rx_len);
	memset(&T_rxdata_buf[rx_uart_w_index],0,sizeof(uart_data_t));

	if (rx_len)
	{
		flyco_module_uartCmdHandler(T_rxdata_for_flyco.data, rx_len - 4);//handler SPP CMD/DATA
	}
	rx_uart_w_index = (rx_uart_w_index + 1)&0x01;

	return 0;
}

int flyco_blc_tx_to_uart ()//Master data send to UART,we will handler the data as CMD or DATA
{
	if (uart_wb_rptr != uart_wb_wptr)
	{
		u8 *p = uart_wb[uart_wb_rptr & (UART_WB_NUM-1)];

		flyco_module_masterCmdHandler(p+4, p[3]);

		uart_wb_rptr++;
	}
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////


void user_init()
{
	rf_customized_param_load();  //load customized freq_offset cap value and tp value

	////////////////// BLE slave initialization ////////////////////////////////////
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff){
		memcpy (tbl_mac, pmac, 6);
	}
	else{
//		//TODO : should write mac to flash after pair OK
//		tbl_mac[0] = (u8)rand();
//		flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
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

	////////////////////NV ////////////////////////////////////////////////////////
	advinterval_min   = DEFLUT_ADV_INTERVAL_MIN;//30ms
	advinterval_max   = DEFLUT_ADV_INTERVAL_MAX;//30ms
	rfpower           = DEFLUT_RF_PWR_LEVEL;//0dB
	adv_timeout       = DEFLUT_ADV_TIMEOUT;//0s
	u8 advTem[20]     = {0};
	u8 scanRspTem[20] = {0};

	//device name init
	u8 devName[20] = {0};
	if(!nv_read(NV_FLYCO_ITEM_DEV_NAME, devName, 20)) {
		memset(ble_devName, 0, 18); //clear device name
		memcpy(ble_devName, devName + 2, devName[0] - 1);
	}
	//adv para
	nv_read(NV_FLYCO_ITEM_ADV_INTERVAL, (u8 *)&advinterval_min, 2);
	nv_read(NV_FLYCO_ITEM_RF_POWER, (u8*)&rfpower, 1);
	nv_read(NV_FLYCO_ITEM_ADV_TIMEOUT, (u8 *)&adv_timeout, 4);
	nv_read(NV_FLYCO_ITEM_ADV_DATA, advTem, 20);
	nv_read(NV_FLYCO_ITEM_DEV_NAME, scanRspTem, 20);

	rf_set_power_level_index (rfpower);

	bls_ll_init (tbl_mac);

	//adv init
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
	}

	//identified code init
	u8 identified[6];//Need to use the nvflash to save module identification code. The default product identification code equales the ble module MAC address!
	reverse_data(tbl_mac, BLE_ADDR_LEN, identified);
	nv_read(NV_FLYCO_ITEM_IDENTIFIED, identified, 6);
	nv_write(NV_FLYCO_ITEM_IDENTIFIED, identified, 6);

	//ble event callback register
	blt_register_event_callback (BLT_EV_FLAG_ADV_PRE, &task_adv);
	blt_register_event_callback (BLT_EV_FLAG_CONNECT, &task_connect);
	blt_register_event_callback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);
	blt_register_event_callback (BLT_EV_FLAG_SET_WAKEUP_SOURCE, &ble_flyco_set_sleep_wakeup);

	blc_l2cap_register_handler (blc_l2cap_packet_receive);

    //NOTE: my_att_init  must after bls_ll_init, and before bls_ll_setAdvParam
	extern void my_att_init ();
	my_att_init ();

	u8 status = bls_ll_setAdvParam( advinterval_min, advinterval_max, \
			 	 	 	 	 	     ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
			 	 	 	 	 	     0,  NULL,  BLT_ENABLE_ADV_37, ADV_FP_NONE);
	if(status != BLE_SUCCESS){
		while(1);
	}

#if 1
	//OTA register callback function
	extern void flyco_ble_setOtaResIndicateCb(ota_resIndicateCb_t cb);
	extern void flyco_send_ota_result(int errorcode);
	flyco_ble_setOtaResIndicateCb(&flyco_send_ota_result);
#endif

	bls_ll_setAdvEnable(1);  //adv enable

	/////////////////// Wakeup source configuration /////////
	gpio_set_wakeup(BRTS_WAKEUP_MODULE, 1, 1);  	   //drive pin core(gpio) high wakeup suspend
	cpu_set_gpio_wakeup (BRTS_WAKEUP_MODULE, BRTS_WAKEUP_LEVEL, 1);  //drive pin pad high wakeup deepsleep
	gpio_core_wakeup_enable_all(1);

#if(BLE_REMOTE_PM_ENABLE)
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
#else
	bls_pm_setSuspendMask (SUSPEND_DISABLE);
#endif


	/////////////////////////////////////////////////////////////////
	ll_whiteList_reset();

	// BLE STATE OUT
	gpio_set_func(BLE_STA_OUT, AS_GPIO);
	gpio_set_output_en(BLE_STA_OUT, 1);
	gpio_write(BLE_STA_OUT,0);//LOW

	advertise_begin_tick = clock_time();

	//todo:uart init here
	rx_uart_r_index = -1;//should set as -1,otherwise we should send data to Uart twice, the second time,spp function will work
	rx_uart_w_index = 0;

	gpio_set_func(GPIO_UTX, AS_UART);
	gpio_set_func(GPIO_URX, AS_UART);
	gpio_set_input_en(GPIO_UTX, 1);
	gpio_set_input_en(GPIO_URX, 1);

	//uart bode rate init
	u32 baudrate;
	u8 param[3]={0};
	nv_read(NV_FLYCO_ITEM_BAUD_RATE, param, 3);
	if(param[2] != 0xef)//3×Ö½Ú
		baudrate = (param[0]<<16) + (param[1]<<8) + param[2];
	else
		baudrate = (param[0]<<8) + param[1];

	if(baudrate == 9600)
		CLK16M_UART9600;
	else if(baudrate == 115200)
		CLK16M_UART115200;
	else
		CLK16M_UART9600;

	uart_BuffInit((u8 *)(&T_rxdata_buf), sizeof(T_rxdata_buf), (u8 *)(&T_txdata_buf));
	blc_register_hci_handler (flyco_blc_rx_from_uart, flyco_blc_tx_to_uart);//flyco_spp
	//blc_register_hci_handler(blc_rx_from_uart,blc_hci_tx_to_uart);//telink_spp

	/** smp test **/
	smpRegisterCbInit();

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
    blt_user_timerCb_proc();//user Timer callback process

	blt_pm_proc();
}
