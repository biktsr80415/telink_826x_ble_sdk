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
	
#if 0
	gpio_write(RED_LED, ON);
#else
	gpio_write(GREEN_LED,ON);
#endif
}


#if SMP_BUTTON_ENABLE
u32 ctrl_btn[] = BTN_PINS;
u8 btn_map[MAX_BTN_SIZE] = BTN_MAP;

typedef	struct{
	u8 	cnt;				//count button num
	u8 	btn_press;
	u8 	keycode[MAX_BTN_SIZE];			//6 btn
}vc_data_t;
vc_data_t vc_event;

typedef struct{
	u8  btn_history[4];		//vc history btn save
	u8  btn_filter_last;
	u8	btn_not_release;
	u8 	btn_new;					//new btn  flag
}btn_status_t;
btn_status_t 	btn_status;

u8 mybtn_debounce_filter(u8 *btn_v);
u8 myvc_detect_button(int read_key);
extern u8  confirm_f_uart_btn;
void proc_button (void){
	if(blc_smp_getGenMethod() == NUMERIC_COMPARISON && !confirm_f_uart_btn &&\
	   blc_get_pc_start_tick() && !clock_time_exceed(blc_get_pc_start_tick(), blc_get_pc_timeout_duration())){
		static u32 button_det_tick;
		if(clock_time_exceed(button_det_tick, 5000))
		{
			button_det_tick = clock_time();
		}
		else{
			return;
		}

		extern void blc_smp_setNCTimeoutTick (u32 t);

		int det_key = myvc_detect_button(1);

		if (det_key){

			u8 key = vc_event.keycode[0];

			if(vc_event.cnt == 2)  //two key press
			{

			}
			else if(vc_event.cnt == 1) //one key press
			{
		        if(key == 1){
					printf("Slave confirmed: 'YES'(pairing).\n");
					confirm_f_uart_btn = 1;
					blc_smp_setNCconfirmValue(NC_CONFIRM_YES);
		        }
		        else if(key == 2){
		        	printf("Slave confirmed: 'NO'(cancel).\n");
					confirm_f_uart_btn = 1;
					blc_smp_setNCconfirmValue(NC_CONFIRM_NO);
		        }
			}
			else if(vc_event.cnt == 0){
				//printf("Key Released!\n");
			}
		}
	}
}

u8 mybtn_debounce_filter(u8 *btn_v)
{
	u8 change = 0;
	for(int i=3; i>0; i--){
		btn_status.btn_history[i] = btn_status.btn_history[i-1];
	}
	btn_status.btn_history[0] = *btn_v;
	if(  btn_status.btn_history[0] == btn_status.btn_history[1] && btn_status.btn_history[1] == btn_status.btn_history[2] && \
		btn_status.btn_history[0] != btn_status.btn_filter_last ){
		change = 1;
		btn_status.btn_filter_last = btn_status.btn_history[0];
	}
	return change;
}

u8 myvc_detect_button(int read_key)
{
	u8 btn_changed, i;
	memset(&vc_event,0,sizeof(vc_data_t));			//clear vc_event
	//vc_event.btn_press = 0;
	for(i=0; i<MAX_BTN_SIZE; i++){
		if(BTN_VALID_LEVEL != !gpio_read(ctrl_btn[i])){
			vc_event.btn_press |= BIT(i);
		}
	}
	btn_changed = mybtn_debounce_filter(&vc_event.btn_press);
	if(btn_changed && read_key){
		for(i=0; i<MAX_BTN_SIZE; i++){
			if(vc_event.btn_press & BIT(i)){
				vc_event.keycode[vc_event.cnt++] = btn_map[i];
			}
		}
		return 1;
	}
	return 0;
}
#endif


void led_init(void)
{
#if 0
	gpio_set_func(RED_LED, AS_GPIO);
	gpio_set_input_en(RED_LED,0);
	gpio_set_output_en(RED_LED,1);
	gpio_write(RED_LED, OFF);
#else
	gpio_set_func(GREEN_LED, AS_GPIO);
	gpio_set_input_en(GREEN_LED,0);
	gpio_set_output_en(GREEN_LED,1);
	gpio_write(GREEN_LED,OFF);
#endif
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

#if (SECURE_CONNECTION_ENABLE && SMP_NUMERIC_COMPARISON && SMP_UART_ENABLE)//use uart confirm 'YES/NO'(NC)
	extern u8  pm_ctrl_flg;
	if(pm_ctrl_flg){
		bls_pm_setSuspendMask(SUSPEND_DISABLE);
		return;
	}
#endif

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


	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization
#if SIG_PROC_ENABLE
	blc_l2cap_reg_att_sig_hander(att_sig_proc_handler);         //register sig process handler
#endif

	//smp initialization
#if ( SMP_DO_NOT_SUPPORT )
	bls_smp_enableParing (SMP_PARING_DISABLE_TRRIGER );
#else //if (SMP_JUST_WORK || SMP_PASSKEY_ENTRY || SMP_NUMERIC_COMPARISON)
	//Just work encryption: TK default is 0, that is, pin code defaults to 0, without setting
	//Passkey entry encryption: generate random numbers pinCode, or set the default pinCode, processed in the event_handler function

	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );

	#if SECURE_CONNECTION_ENABLE
		blc_smp_enableScFlag (1);//support smp4.2
		#if (SMP_NUMERIC_COMPARISON)//if 0:SC_JUST_WORK; if 1:SC_NC.
		    blc_smp_enableAuthMITM (1, DEFAULT_PINCODE);
			blc_smp_setIoCapability (IO_CAPABLITY_DISPLAY_YESNO);
        #elif (SMP_JUST_WORK)
			//do nothing.
		#endif
    #endif

	#if SMP_PASSKEY_ENTRY//if blc_smp_enableScFlag (1) && enable BLE_P256_PUBLIC_KEY_ENABLE: SC_PASSKEY ENTRY
		blc_smp_enableAuthMITM (1, DEFAULT_PINCODE);
		//blc_smp_setIoCapability (IO_CAPABLITY_DISPLAY_ONLY);   //Responder displays PK, initiator inputs PK
		blc_smp_setIoCapability (IO_CAPABLITY_KEYBOARD_ONLY);   //Initiator displays PK, responder inputs PK
    #endif

	//HID_service_on_android7p0_init();
#endif

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


    printf("\n\rAdv parameters setting success!\n\r");
	bls_ll_setAdvEnable(1);  //adv enable
	printf("Enable ble adv!\n\r");
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



#if (BLE_MODULE_PM_ENABLE)
	//mcu can wake up module from suspend or deepsleep by pulling up GPIO_WAKEUP_MODULE
	gpio_set_wakeup		(GPIO_WAKEUP_MODULE, 1, 1);  // core(gpio) high wakeup suspend
	cpu_set_gpio_wakeup (GPIO_WAKEUP_MODULE, 1, 1);  // pad high wakeup deepsleep

	GPIO_WAKEUP_MODULE_LOW;

	bls_pm_registerFuncBeforeSuspend( &app_suspend_enter );
#endif

#if (BATT_CHECK_ENABLE)
	#if((MCU_CORE_TYPE == MCU_CORE_8261)||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269))
		adc_BatteryCheckInit(ADC_CLK_4M, 1, Battery_Chn_VCC, 0, SINGLEEND, RV_1P428, RES14, S_3);
	#elif(MCU_CORE_TYPE == MCU_CORE_8266)
		adc_Init(ADC_CLK_4M, ADC_CHN_D2, SINGLEEND, ADC_REF_VOL_1V3, ADC_SAMPLING_RES_14BIT, ADC_SAMPLING_CYCLE_6);
	#endif
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
	blt_sdk_main_loop();

	////////////////////////////////////// UI entry /////////////////////////////////
#if SMP_BUTTON_ENABLE
	proc_button();
#endif

#if (BATT_CHECK_ENABLE)
	if(tick_loop%300 == 0)
	{
		ADC_MODULE_ENABLE;
		battery_power_check();
	}
#endif
	//  add spp UI task
	app_power_management ();

}
