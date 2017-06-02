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
#include "../../vendor/common/tl_audio.h"

#define			SDM_SAMPLE_RATE						16000
//#define			SDM_SAMPLE_RATE						8000

//ATT handle of HID,audio,speaker
#define			HID_HANDLE_CONSUME_REPORT			0x19
#define			HID_HANDLE_KEYBOARD_REPORT			0x1D
#define			AUDIO_HANDLE_MIC					0x2F
#define			SPEAKER_HANDLE 					    0x32

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 8);
//////////////////////////////////////////////////////////////////////////////
//	Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
const u8	tbl_advData[] = {
	 0x06, 0x09, 'A', 'u', 'd', 'i', 'o',
	 0x02, 0x01, 0x05, 			    // BLE limited discoverable mode and BR/EDR not supported
	 0x03, 0x02, 0x12, 0x18,		// incomplete list of service class UUIDs (0x1812)
};

const u8	tbl_scanRsp [] = {
		 0x07, 0x09, 't', 'A', 'u', 'd', 'i', 'o',	//scan name " tmodul"
	};


u16 sdm_step;
u8 	ui_ota_is_working = 0;
u8  ui_task_flg;
u32	ui_advertise_begin_tick;
int ui_mtu_size_exchange_req = 0;

void entry_ota_mode(void)
{
	ui_ota_is_working = 1;
	bls_ota_setTimeout(30 * 1000000); //set OTA timeout  30 S
	gpio_write(BLUE_LED, 1);  //LED on for indicate OTA mode
}

void show_ota_result(int result)
{
#if 1
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


void	task_connect (void)
{
	//bls_l2cap_requestConnParamUpdate (8, 16, 0, 400);//18.75ms IOS
	bls_l2cap_requestConnParamUpdate (8, 8, 99, 400);  //interval=10ms latency=99 timeout=4s
	gpio_write(RED_LED, ON);
	ui_mtu_size_exchange_req = 1;
}

void 	ble_remote_terminate(u8 e,u8 *p, int n) //*p is terminate reason
{
	if(*p == HCI_ERR_CONN_TIMEOUT){

	}
	else if(*p == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

	}
	else if(*p == HCI_ERR_CONN_TERM_MIC_FAILURE){

	}
}

u32 tick_wakeup;


void app_suspend_exit ()
{

}

int app_suspend_enter ()
{

}

void app_power_management ()
{

}


#define		APP_CFG_ADR_MAC				0x8000
#define		APP_CAP_INFO_ADDR			0x9000
#define		APP_TP_INFO_ADDR			0x9040
#define		APP_RC32K_CAP_INFO_ADDR		0x9080

static inline void app_loadCustomizedParameters(void)
{
	  //customize freq_offset adjust cap value, if not customized, default ana_81 is 0xd0
	 if( (*(unsigned char*) APP_CAP_INFO_ADDR) != 0xff ){
		 //ana_81<4:0> is cap value(0x00 - 0x1f)
		 analog_write(0x81, (analog_read(0x81)&0xe0) | ((*(unsigned char*) APP_CAP_INFO_ADDR)&0x1f) );
	 }

	 // customize TP0/TP1
	 if( ((*(unsigned char*) (APP_TP_INFO_ADDR)) != 0xff) && ((*(unsigned char*) (APP_TP_INFO_ADDR+1)) != 0xff) ){
		 rf_update_tp_value(*(unsigned char*) (APP_TP_INFO_ADDR), *(unsigned char*) (APP_TP_INFO_ADDR+1));
	 }

	  //customize 32k RC cap, if not customized, default ana_32 is 0x80
	 if( (*(unsigned char*) APP_RC32K_CAP_INFO_ADDR) != 0xff ){
		 //ana_81<4:0> is cap value(0x00 - 0x1f)
		 analog_write(0x32, *(unsigned char*) APP_RC32K_CAP_INFO_ADDR );
	 }
}


void user_init()
{
#if 1
	//app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

	REG_ADDR8(0x74) = 0x53;
	REG_ADDR16(0x7e) = 0x01d8;
	REG_ADDR8(0x74) = 0x00;
	usb_log_init ();
	usb_dp_pullup_en (1);  //open USB enum

	u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};
	u32 *pmac = (u32 *) APP_CFG_ADR_MAC;
	if (*pmac != 0xffffffff)
	{
	    memcpy (tbl_mac, pmac, 6);
	}
    else
    {
        //TODO : should write mac to flash after pair OK
        tbl_mac[0] = (u8)rand();
        flash_write_page (APP_CFG_ADR_MAC, 6, tbl_mac);
    }


///////////// BLE stack Initialization ////////////////
	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory

	//blc_ll_initScanning_module(tbl_mac);		//scan module: 		 optional
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
	blc_ll_initPowerManagement_module();        //pm module:      	 optional


	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization

	//smp initialization
	//bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );


	///////////////////// USER application initialization ///////////////////
	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));


	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_20MS, ADV_INTERVAL_30MS + 16, \
			 	 	 	 	 	     ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
			 	 	 	 	 	     0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}
    bls_ll_setAdvEnable(1);  //adv enable
	rf_set_power_level_index (RF_POWER_8dBm);

	bls_pm_setSuspendMask (SUSPEND_DISABLE);//(SUSPEND_ADV | SUSPEND_CONN)

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);

	// OTA init
#if 0
	bls_ota_clearNewFwDataArea(); //must
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);
#endif

	ui_advertise_begin_tick = clock_time();
#endif

	//////////////////////////// Audio config ////////////////////////////////
#if MODULE_AUDIO_ENABLE
	sdm_step = config_sdm  ((u32)buffer_sdm, TL_SDM_BUFFER_SIZE, SDM_SAMPLE_RATE, 4);//16k
#endif

#if (BATT_CHECK_ENABLE)
	#if((MCU_CORE_TYPE == MCU_CORE_8261)||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269))
		adc_BatteryCheckInit(ADC_CLK_4M, 1, Battery_Chn_VCC, 0, SINGLEEND, RV_1P428, RES14, S_3);
	#elif(MCU_CORE_TYPE == MCU_CORE_8266)
		adc_Init(ADC_CLK_4M, ADC_CHN_D2, SINGLEEND, ADC_REF_VOL_1V3, ADC_SAMPLING_RES_14BIT, ADC_SAMPLING_CYCLE_6);
	#endif
#endif
}

////////////////// audio ///////////////////////////////////////
#define			ADPCM_FLASH_ADR			0x10000
#define			ADPCM_FLASH_SIZE		(0x10000 * 6)
u32				adpcm_offset = 0;

u8			mute = 0;

void task_audio(void)
{
	if ( sdm_decode_ready (256) )
	{
		if (mute)
		{
			silence2sdm (256);
		}
		else
		{
			adpcm_offset += adpcm2sdm((s16 *)(ADPCM_FLASH_ADR + adpcm_offset));

			if (adpcm_offset >=  ADPCM_FLASH_SIZE)
			{
				adpcm_offset = 0;
			}
		}
	}
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
	task_audio();

#if (BATT_CHECK_ENABLE)
	extern void battery_power_check(void);
	battery_power_check();
#endif
	//  add spp UI task
	app_power_management ();

}
