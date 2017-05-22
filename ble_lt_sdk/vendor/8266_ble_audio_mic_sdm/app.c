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

//ATT handle of HID,audio,speaker
#define			HID_HANDLE_CONSUME_REPORT			0x19
#define			HID_HANDLE_KEYBOARD_REPORT			0x1D
#define			AUDIO_HANDLE_MIC					0x2F
#define			SPEAKER_HANDLE 					    0x32

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 80, 8);
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


s16 buff_adpcm[64];//128bytes数据
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


void user_init()
{
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

	REG_ADDR8(0x74) = 0x53;
	REG_ADDR16(0x7e) = 0x08d1;
	REG_ADDR8(0x74) = 0x00;
	usb_log_init ();
	usb_dp_pullup_en (1);  //open USB enum

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

	//blc_ll_initScanning_module(tbl_mac);		//scan module: 		 optional
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
	blc_ll_initPowerManagement_module();        //pm module:      	 optional


	////// Host Initialization  //////////
	extern void my_att_init ();
	my_att_init (); //gatt initialization
	blc_l2cap_register_handler (blc_l2cap_packet_receive);  	//l2cap initialization

	//smp initialization
	bls_smp_enableParing (SMP_PARING_CONN_TRRIGER );


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
    printf("adv parameters setting success!\n\r");
	bls_ll_setAdvEnable(1);  //adv enable
	printf("enable ble adv!\n\r");
	rf_set_power_level_index (RF_POWER_8dBm);

	bls_pm_setSuspendMask (SUSPEND_DISABLE);//(SUSPEND_ADV | SUSPEND_CONN)

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);
	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &ble_remote_terminate);

	// OTA init
	bls_ota_clearNewFwDataArea(); //must
	bls_ota_registerStartCmdCb(entry_ota_mode);
	bls_ota_registerResultIndicateCb(show_ota_result);

	//////////////////////////// Audio config ////////////////////////////////
#if MODULE_AUDIO_ENABLE
	//buffer_mic set must before audio_init !!!
	config_mic_buffer ((u32)buffer_mic, TL_MIC_BUFFER_SIZE);
    #if (!BLE_DMIC_ENABLE)//AMIC  config
		config_adc (FLD_ADC_PGA_C01, FLD_ADC_CHN_D0, SYS_32M_AMIC_8K);
		gpio_set_output_en (GPIO_PB7, 1);		//AMIC Bias output
		gpio_write (GPIO_PB7, 1);
	#else//DMIC config
		config_dmic (16000);//16k
	#endif
	sdm_step = config_sdm  ((u32)buffer_sdm, TL_SDM_BUFFER_SIZE, 16000, 4);//16k
#endif

#if (BATT_CHECK_ENABLE)
	#if((MCU_CORE_TYPE == MCU_CORE_8261)||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269))
		adc_BatteryCheckInit(ADC_CLK_4M, 1, Battery_Chn_VCC, 0, SINGLEEND, RV_1P428, RES14, S_3);
	#elif(MCU_CORE_TYPE == MCU_CORE_8266)
		adc_Init(ADC_CLK_4M, ADC_CHN_D2, SINGLEEND, ADC_REF_VOL_1V3, ADC_SAMPLING_RES_14BIT, ADC_SAMPLING_CYCLE_6);
	#endif
#endif
	ui_advertise_begin_tick = clock_time();
}

void task_audio(void){
//////////////// mic --> sdm //////////////
#if 1
	proc_mic_encoder ();
	if (mic_encoder_data_ready ((int*)buff_adpcm)) {
		//decoding dat
		sdm_decode_rate (sdm_step, 2);
		adpcm_to_sdm (buff_adpcm, 128);
	}
#else
	proc_mic_encoder ();
	int *p = mic_encoder_data_buffer ();
	if (p)					//around 3.2 ms @16MHz clock
	{
		#if 1
			sdm_decode_rate (sdm_step, 2);//decoding dat
			adpcm_to_sdm(p,128);
		#else//same as above
			sdm_decode_rate (sdm_step, 2);//decoding dat
			adpcm_to_pcm((s16 *)(p),buff_adpcm, 128);
			pcm_to_sdm (buff_adpcm, 128);
		#endif
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
