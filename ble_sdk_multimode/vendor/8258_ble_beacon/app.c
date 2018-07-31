//#include "../../proj_lib/rf_drv.h"
//#include "../../proj_lib/pm.h"

#include "stack/ble/ble.h"
//#include "../../proj_lib/ble/ll/ll.h"
//#include "../../proj_lib/ble/blt_config.h"
//#include "../../proj_lib/ble/trace.h"

#include "drivers.h"
//#include "../../proj/drivers/adc.h"
//#include "../../proj/drivers/battery.h"


#include "tl_common.h"


#include "../common/keyboard.h"
#include "../common/tl_audio.h"
#include "../common/blt_led.h"


#define CLOCK_SYS_CLOCK_100MS   CLOCK_SYS_CLOCK_1MS*100

/************************************************************************
 * 				External Variable and Definitions
 */

/*BLE connection status in stack*/
extern	u8 blt_state;

/*Mac addr pointer in stack*/
extern u16 *	blt_p_mac;

/************************************************************************
 * 				Global Variable and Definitions
 */
MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 40, 16);

/*Default beacon state after system start*/
u8 current_state = BEACON_INVALIAD_STAUS;

/*System boot time, used to calculate connectable-advertising time after system boot
  * and used to  calculate Eddystone TLM SEC Counter*/
u32 system_boot_time = 0;

/* Beacon adv_sended pointer, must initial before send  */
u8*	beacon_p_pkt = 0;

/*Default Ibeacon UUID(NID+BID), note it should be set as big-endian mode*/
#define IBeacon_UUID_WeChat_Telink01  	0
#define IBeacon_UUID_WeChat_Telink02  	0
#define IBeacon_UUID_WeChat_Test        0
#define IBEACON_UUID_SYSGRATION         1


#if (IBeacon_UUID_WeChat_Telink01 | IBeacon_UUID_WeChat_Telink02 | IBeacon_UUID_WeChat_Test)
/*Wechat (weixin yaoyiyao) mode*/
const u8 telink_uuid4beacon[16]= {0xFD,0xA5,0x06,0x93,0xA4,0xE2,0x4F,0xB1,0xAF,0xCF,0xC6,0xEB,0x07,0x64,0x78,0x25};

#elif (IBEACON_UUID_SYSGRATION)
const u8 telink_uuid4beacon[16]= {0xb5,0x4a,0xdc,0x00,0x67,0xf9,0x11,0xd9,0x96,0x69,0x08,0x00,0x20,0x0c,0x9a,0x6A};

#else

/*Telink defined mode*/
const u8 telink_uuid4beacon[16]= {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10};

#endif

/*Default Beacon mode*/
//const u8 telink_defaultBeaconMode[]={0x01, TELINK_IBEACON_MODE, 0xFF};

/*Default Eddyston encoded URL, "http://www.telink-semi.com",note it should be set as big-endian mode*/
const u8 telink_defaultURL4beacon[]={0x00, 't', 'e', 'l', 'i','n','k','-','s','e','m','i',0x07};

/*Different beacon tx power level*/
u8 beacon_txLevelIndex[4] = {3,2,1, 0};
s8 beacon_txPowerValue[4] = {-4,0,5, 7};//-4: 0xFC, 0:0x00, 5:0x05, 7:0x07

/*Eddystone URL ADV PDU Length*/
u8 beacon_eddystoneURLAdvLen = 0;

/*Eddystone TLM ADV Counter*/
u32 beacon_adv_couter = 0;
/*Eddystone TLM SEC Counter*/
u32 beacon_lastClockTime = 0x00; //Record last clock time while calculate TLM Sec time
u32 beacon_TLMSecCounter = 0x00;   //Record TLM Sec Conter

telink_beacon_config_t telink_beacon_config;

ibeacon_adv_t ibeacon_tbl_adv;
eddystone_UID_t eddystone_UID_tbl_adv;
eddystone_URL_t eddystone_URL_tbl_adv;
eddystone_TLM_t eddystone_TLM_tbl_adv;

u32 beacon_advPDUAddrBuf[]={
			(u32)&ibeacon_tbl_adv,
			(u32)&eddystone_UID_tbl_adv,
			(u32)&eddystone_URL_tbl_adv,
			(u32)&eddystone_TLM_tbl_adv,
};

u32 beacon_advPDULen[]={
			sizeof(ibeacon_adv_t),
			sizeof(eddystone_UID_t),
			sizeof(eddystone_URL_t),
			sizeof(eddystone_TLM_t)
};

#if 1// driver
extern u8 blc_adv_channel[];
extern u32 blt_advExpectTime;
extern blt_event_callback_t		blt_p_event_callback ;
extern st_ll_adv_t  blta;
u8 beacon_adv_send[48] = {0};
int  blt_send_beacon_adv( int adv_mask, u8* adv_pkt)
{
	u8 adv_rf_len = adv_pkt[1];
	memcpy((u8*)(beacon_adv_send + 4), adv_pkt, adv_rf_len + 2);
	beacon_adv_send[0] = adv_rf_len + 2;   // set beacom dma len

	if(bltParam.adv_en)
	{
		rf_set_ble_access_code_adv ();
		rf_set_ble_crc_adv ();

		u32  t_us = (adv_rf_len + 10) * 8 + 370;

		for (int i=0; i<3; i++)
		{
			if (adv_mask & BIT(i))
			{
				STOP_RF_STATE_MACHINE;						// stop SM
				rf_set_ble_channel (blc_adv_channel[i]);   //phy poweron in this func
				reg_rf_irq_status = FLD_RF_IRQ_TX | FLD_RF_IRQ_RX;
				//if(blc_rf_pa_cb){	blc_rf_pa_cb(PA_TYPE_TX_ON);  } // NULL currently
				
				////////////// start TX //////////////////////////////////
				rf_start_stx (beacon_adv_send, clock_time() + 100);
				u32 tx_begin_tick = clock_time ();
				while (!(reg_rf_irq_status & FLD_RF_IRQ_TX) && (clock_time() - tx_begin_tick) < (t_us)*sys_tick_per_us);

				//if(blc_rf_pa_cb){	blc_rf_pa_cb(PA_TYPE_OFF);  }   // NULL currently

			}

		}
		
		blt_p_event_callback (BLT_EV_FLAG_BEACON_DONE, 0, 0);

		//clear stx2rx stateMachine status
		STOP_RF_STATE_MACHINE;

		PHY_POWER_DOWN;
		POWER_DOWN_64MHZ_CLK;
	}


	u32 cur_tick = clock_time();
	if(abs( (int)(blt_advExpectTime - cur_tick) ) < 5000 * sys_tick_per_us){
		blt_advExpectTime += blta.adv_interval;
	}
	else{
		blt_advExpectTime = cur_tick + blta.adv_interval;
	}

	reg_system_tick_irq = cur_tick + BIT(31);  //adv state, system tick irq will not happen

	return 0;
}

#endif
#if EDDYSTONE_TLM_ENABLE
#define BEACON_SEC_CNT_ENABLE  1// To enable sec counter for Eddystone TLM Beacon, need about 14us for each interval
#define BEACON_ADV_CNT_ENABLE  1//To enable adv counter for Eddystone TLM Beacon
#else
#define BEACON_SEC_CNT_ENABLE  0// To enable sec counter for Eddystone TLM Beacon, need about 14us for each interval
#define BEACON_ADV_CNT_ENABLE  0//To enable adv counter for Eddystone TLM Beacon
#endif

#define TX_POWERMODE_MAX  3 //tx_powerMode, 0, 1, 2, 3
#define BEACON_PERIOD_MAX 60000 //beacon_period max 60s

/*Define connectable adv and beacon adv channel*/
#define CONNECTABLE_ADV_CHANNEL    BLT_ENABLE_ADV_ALL
#define BEACON_ADV_CHANNEL         BLT_ENABLE_ADV_ALL//BLT_ENABLE_ADV_ALL

/*Define if SDK need send connectable adv, if don't need, set as 0 to save ROM resource*/
#define SDK_NEED_CONNECTABLE_ADV     1

/*Define to counter adv num for every adv type*/
u8 beacon_adv_type_index = 0x00;
u8 beacon_adv_type_num = 0x00;

void beacon_init(void);
void beacon_iBeaconPDUassemble( ibeacon_adv_t* ibeacon_p);
void beacon_eddystonePDUassemble(eddystone_mode esm, u8* eddystone_p);
void updateAdvDataPointer(u8*p);
void updateAdvDataMacAddr(void);
void rf_beaconInit(void);
void beacon_save_paramter();
void beacon_para_init();
void beacon_configParamInit(telink_beacon_config_t * config_p);


////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////
/*Reserved for customer to add some vendor-specific operations*/
void proc_ui ()
{
}

/*Used for Beacon state transition*/
void check_state( ){
     /*Calculate TLM Sec Counter */
     #if BEACON_SEC_CNT_ENABLE
     if(beacon_lastClockTime){
         /*Consider if clock_time has exceeded 2^32*/
         u32 timeDiff = (clock_time() >= beacon_lastClockTime)? (clock_time()-beacon_lastClockTime):(U32_MAX-beacon_lastClockTime+clock_time());
         while(timeDiff> (CLOCK_16M_SYS_TIMER_CLK_1MS*100) ){
             beacon_TLMSecCounter++;
             beacon_lastClockTime += (CLOCK_16M_SYS_TIMER_CLK_1MS*100);
             timeDiff -= (CLOCK_16M_SYS_TIMER_CLK_1MS*100);
         }
     }
     #endif

      /*Already in Beacon state, continue sending beacon (non connectable adv)*/

	if(current_state == MODULE_STATE_BEACON){
		return;
	}
	/*Connection terminated meaning configuration finished, start sending beacon*/
	else if(current_state == MODULE_STATE_TERMINATED){
		//config finished . to save data
		beacon_save_paramter();
		current_state = MODULE_STATE_BEACON;
		beacon_init();
	}
	/*System boot, start connectable advertising waiting conn_req from master*/
	else if(current_state == BEACON_INVALIAD_STAUS){
		current_state = MODULE_STATE_CONFIGURE;
		system_boot_time = clock_time();

		/*Used for Eddystone TLM Sec Counter*/
		#if BEACON_SEC_CNT_ENABLE
		beacon_lastClockTime = clock_time();
		beacon_TLMSecCounter = 0;
		#endif
		return;
	}

	/*Connected by master, is being configured*/
	else if(blt_state == BLS_LINK_STATE_CONN){
		current_state = MODULE_STATE_CONNECTED;
		system_boot_time = 0;
		return;
	}

	/*Connectable advertising exceed 30s without been connected by master, stop conn_adv, start sending beacon (non connectable adv)*/
	else if(system_boot_time && (clock_time()-system_boot_time>BEACON_MODE_WAIT_TIME)){
		current_state = MODULE_STATE_BEACON;
		beacon_init();
		system_boot_time = 0;
	}

}

void check_battery()
{
    static int loop =0;
    
    if((loop++ %300) ==0)
    {    
        // around 50mv offset
        //printf("sv %d \n", adc_BatteryValueGet()/4 );// below 2.2V low valotage      
    }

}


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
void app_power_management ()
{
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

//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xef, 0xe1, 0xe2, 0x11, 0x12, 0xc5};
const u8	tbl_advData[] = {
	0x02, 0x01, 0x05,					   //BLE litmited discoverable mode and BR/EDR not supported
	0x02, 0x0A, 0x07,					   //Tx transmission power 7dBm
	0x05,0x02, 0x0a, 0x18,0x00,0x18,	   //incomplete list of service class UUIDS (0x1812, 0x180f)
	0x06, 0x09,  't', 'l', 'B', 'c', 'n'
};

const u8	tbl_scanRsp [] = {
	0x07, 0x09, 'b', 'e', 'a', 'c', 'o', 'n',	//scan name " beacon"
};


/*Call back function if connection terminated (configuration finished)*/
void on_connection_terminate(u8 e,u8 *p, int n){
	current_state = MODULE_STATE_TERMINATED;
	check_state();
}

void beacon_nextBeacon(u8 e,u8 *p, int n){
       /*If it is only one type advertising; or if current adv type shall repeat again*/
	if((telink_beacon_config.beacon_mode.adv_type_num < 2) || (++beacon_adv_type_num < telink_beacon_config.beacon_mode.adv_mode[beacon_adv_type_index].adv_num) ){
		return;
    }

    beacon_adv_type_index = (++beacon_adv_type_index <= (telink_beacon_config.beacon_mode.adv_type_num-1))?beacon_adv_type_index:0;
    beacon_adv_type_num= 0;

    u8 firstAdvType = telink_beacon_config.beacon_mode.adv_mode[beacon_adv_type_index].adv_type;
	//u32 advPDULen = (firstAdvType == TELINK_EDDYSTONE_URL_MODE)?beacon_eddystoneURLAdvLen:beacon_advPDULen[firstAdvType];

	/*Set adv data*/
	if(beacon_advPDUAddrBuf[firstAdvType] == NULL){
	   while(1); //used for debug
	}

	/*For Eddystone TLM, should update adv data every time, ex: adv_cnt, sec_cnt, temp, voltage, etc*/
	#if EDDYSTONE_TLM_ENABLE
		if(firstAdvType == TELINK_EDDYSTONE_TLM_MODE){
			//u8* eddystoneTLM_p = (u8*)(beacon_advPDUAddrBuf[firstAdvType]);
			eddystone_TLM_t* eddystoneTLM_p = (eddystone_TLM_t*)(beacon_advPDUAddrBuf[firstAdvType]);
			eddystoneTLM_p->adv_cnt = beacon_adv_couter;
			eddystoneTLM_p->sec_cnt = beacon_TLMSecCounter;
		}
	/*Because adv_cnt and sec_cnt change all the time, so we don't need to save it */
	#endif
	updateAdvDataPointer( (u8*)beacon_advPDUAddrBuf[firstAdvType]);
}

//reserved
void blt_system_power_optimize(void)  //to lower system power
{
	// TODO : power optimize
	//disable_unnecessary_module_clock
	//reg_rst_clk0 &= ~FLD_RST_SPI;  //spi not use
	//reg_rst_clk0 &= ~FLD_RST_I2C;  //iic not use
#if(!MODULE_USB_ENABLE) //if usb not use
	//reg_rst_clk0 &= ~(FLD_RST_USB | FLD_RST_USB_PHY);
#endif
	//reg_clk_en1 &= ~(FLD_CLK_UART_EN);

}

void main_loop ()
{
	static u32 tick_loop = 0;
	tick_loop ++;
	if(current_state == MODULE_STATE_BEACON){
		#if BEACON_ADV_CNT_ENABLE
			beacon_adv_couter++; //Used for Eddystone TLM ADV Counter
		#endif
		blt_send_beacon_adv(BEACON_ADV_CHANNEL, beacon_p_pkt);//beacon_advPDUAddrBuf[firstAdvType]);
	}else
	{
	}

	blt_sdk_main_loop();
	
	if(blt_state == BLS_LINK_STATE_ADV){
		extern volatile u8 blt_busy;
		blt_busy = 0;
	}
	check_state();

#if(BATT_CHECK_ENABLE)
    //battery_power_check();
	if(tick_loop%300 == 0)
	{
		// TODO : battery check
		//ADC_MODULE_ENABLE;

		//extern void battery_power_check(void);
		//battery_power_check();
	}
#endif

	app_power_management ();
}

void user_init()
{
	sleep_us(1000*1000);// test
	//swire2usb_init(); 
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

	////////////////// BLE slave initialization ////////////////////////////////////
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff)
	{
	    memcpy (tbl_mac, pmac, 6);
	}
    else
    {
        tbl_mac[0] = (u8)rand();
        flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
    }

	//blc_ll_initBasicMCU(tbl_mac);   //mandatory
	blc_ll_initBasicMCU();   //mandatory
	blc_ll_initAdvertising_module(tbl_mac); 	//adv module: 		 mandatory for BLE slave,
	blc_ll_initSlaveRole_module();				//slave module: 	 mandatory for BLE slave,
	blc_ll_initPowerManagement_module();        //pm module:      	 optional

	blc_l2cap_register_handler (blc_l2cap_packet_receive);

	bls_ll_setAdvData( (u8 *)tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData( (u8 *)tbl_scanRsp, sizeof(tbl_scanRsp));

	extern void	beacon_att_init ();
	beacon_att_init ();

	//never bonded: undirected adv
	bls_ll_setAdvDuration (0, 0);  //duration disable

	// set adv channel
	bls_ll_setAdvChannelMap(CONNECTABLE_ADV_CHANNEL);

	// set connectible adv(not include beacon adv) enable
	bls_ll_setAdvEnable(1);  //adv enable

	//TODO : tune power consumption
	blt_system_power_optimize();

	bls_app_registerEventCallback (BLT_EV_FLAG_TERMINATE, &on_connection_terminate);

	/*Load para from Flash or not*/
	beacon_para_init();

	//bls_pm_setSuspendMask(SUSPEND_CONN | SUSPEND_ADV);
	bls_pm_setSuspendMask(SUSPEND_DISABLE);


	smp_param_save_t  bondInfo;
	blc_smp_param_loadByIndex( 0, &bondInfo);  //get the latest bonding device (index: bond_number-1 )

	//u8 status =
	bls_ll_setAdvParam( 46,
						58,
						ADV_TYPE_NONCONNECTABLE_UNDIRECTED,
						OWN_ADDRESS_PUBLIC,
						bondInfo.peer_addr_type,
						bondInfo.peer_addr,
						BLT_ENABLE_ADV_ALL,
						ADV_FP_NONE);


    // adc and battery
#if (BATT_CHECK_ENABLE)
	#if((MCU_CORE_TYPE == MCU_CORE_8261)||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269))
	adc_BatteryCheckInit(ADC_CLK_4M, 1, Battery_Chn_VCC, 0, SINGLEEND, RV_1P428, RES14, S_3);
	#elif(MCU_CORE_TYPE == MCU_CORE_8266)
	adc_Init(ADC_CLK_4M, ADC_CHN_D2, SINGLEEND, ADC_REF_VOL_1V3, ADC_SAMPLING_RES_14BIT, ADC_SAMPLING_CYCLE_6);
	#endif
#endif
#if (BLE_MODULE_PM_ENABLE)
	//mcu can wake up module from suspend or deepsleep by pulling up GPIO_WAKEUP_MODULE
	gpio_set_wakeup 	(GPIO_WAKEUP_MODULE, 1, 1);  // core(gpio) high wakeup suspend
	cpu_set_gpio_wakeup (GPIO_WAKEUP_MODULE, 1, 1);  // pad high wakeup deepsleep

	GPIO_WAKEUP_MODULE_LOW;

	bls_pm_registerFuncBeforeSuspend( &app_suspend_enter );
#endif
}

void beacon_para_init()
{
	#if(ADV_STORE_IN_FLASH_EN)
		u8 nv_init_tmp[48];
		/*Set telink beacon config*/
		memset(nv_init_tmp, 0, 48);
		telink_beacon_config_t* config_p = (telink_beacon_config_t*)nv_init_tmp;
		flash_read_page(FLASH_TELINK_BEACON_CONFIG, sizeof(telink_beacon_config_t), nv_init_tmp);

		/*If any parameter is invalid, reset all parameters in EEPROM_TELINK_BEACON_CONFIG*/
		if((config_p->txpower_mode > TX_POWERMODE_MAX) || (config_p->beacon_period > BEACON_PERIOD_MAX) \
			||(config_p->beacon_mode.adv_type_num>= MAX_ADV_TYPE_PER_PERIOD)){
			beacon_configParamInit(config_p);
		}
		// value saved data
		memcpy((u8*)&telink_beacon_config, nv_init_tmp,sizeof(telink_beacon_config_t));

        /*Set ibeacon adv data*/
        #if (IBEACON_ADV_ENABLE)
            memset(nv_init_tmp, 0, 48);
            ibeacon_adv_t* ibeacon_p = (ibeacon_adv_t*)nv_init_tmp;
            flash_read_page (FLASH_ADDR_IBEACON_ADV,  sizeof(ibeacon_adv_t),  nv_init_tmp);

            /*If any parameter is invalid, reset all parameters in EEPROM_ADDR_IBEACON_ADV*/
            if((*(u32*)(&(ibeacon_p->flag_len)) != (0x1A060102))  || (*(u32*)(&(ibeacon_p->manu_spec)) != (0x02004CFF))){
                beacon_iBeaconPDUassemble(ibeacon_p);
            }

			memcpy((u8*)&ibeacon_tbl_adv, nv_init_tmp, sizeof(ibeacon_adv_t));
        #endif

        /*Set eddystone UID adv data*/
        #if (EDDYSTONE_UID_ENABLE)
            memset(nv_init_tmp, 0, 48);
            eddystone_UID_t* eddystoneUID_p = (eddystone_UID_t*)nv_init_tmp;
            flash_read_page (FLASH_ADDR_EDDYSTONE_UID_ADV,  sizeof(eddystone_UID_t), nv_init_tmp);

            /*If any parameter is invalid, reset all parameters in EEPROM_ADDR_EDDYSTONE_UID_ADV*/
            if((*(u32*)(&(eddystoneUID_p->flag_len)) != (0x03060102))  || (*(u32*)(&(eddystoneUID_p->complete_uuid_type)) != (0x17FEAA03))){
                beacon_eddystonePDUassemble(EDDYSTONE_UID, nv_init_tmp);
            }

			memcpy((u8*)&eddystone_UID_tbl_adv, nv_init_tmp, sizeof(eddystone_UID_t));
        #endif

        /*Set eddystone URL adv data*/
        #if (EDDYSTONE_URL_ENABLE)
            memset(nv_init_tmp, 0, 48);
            eddystone_URL_t* eddystoneURL_p = (eddystone_URL_t*)nv_init_tmp;
            flash_read_page (FLASH_ADDR_EDDYSTONE_URL_ADV,  sizeof(eddystone_URL_t), nv_init_tmp);
            /*If any parameter is invalid, reset all parameters in EEPROM_ADDR_EDDYSTONE_UID_ADV*/
            if((*(u32*)(&(eddystoneURL_p->flag_len)) != (0x03060102))  || (*(u16*)(&(eddystoneURL_p->complete_uuid_type)) != (0xAA03))){
                beacon_eddystonePDUassemble(EDDYSTONE_URL, nv_init_tmp);
            }

			memcpy((u8*)&eddystone_URL_tbl_adv, nv_init_tmp, sizeof(eddystone_URL_t));
        #endif

        /*Set eddystone TLM adv data*/
        #if (EDDYSTONE_TLM_ENABLE)
            memset(nv_init_tmp, 0, 48);
            eddystone_TLM_t* eddystoneTLM_p = (eddystone_TLM_t*)nv_init_tmp;
            flash_read_page (FLASH_ADDR_EDDYSTONE_TLM_ADV,  sizeof(eddystone_TLM_t), nv_init_tmp);

            /*If any parameter is invalid, reset all parameters in EEPROM_ADDR_EDDYSTONE_UID_ADV*/
            if((*(u32*)(&(eddystoneTLM_p->flag_len)) != (0x03060102))  || (*(u32*)(&(eddystoneTLM_p->complete_uuid_type)) != (0x11FEAA03))){
                beacon_eddystonePDUassemble(EDDYSTONE_TLM, nv_init_tmp);
            }

			memcpy((u8*)&eddystone_TLM_tbl_adv, nv_init_tmp, sizeof(eddystone_TLM_t));
        #endif


    #else

		beacon_configParamInit(&telink_beacon_config);

        #if (IBEACON_ADV_ENABLE)
            beacon_iBeaconPDUassemble(&ibeacon_tbl_adv);
        #endif

        #if (EDDYSTONE_UID_ENABLE)
            beacon_eddystonePDUassemble(EDDYSTONE_UID, (u8*)&eddystone_UID_tbl_adv);
        #endif

        #if (EDDYSTONE_URL_ENABLE)
            beacon_eddystonePDUassemble(EDDYSTONE_URL, (u8*)&eddystone_URL_tbl_adv);
        #endif

        #if (EDDYSTONE_TLM_ENABLE)
            beacon_eddystonePDUassemble(EDDYSTONE_TLM, (u8*)&eddystone_TLM_tbl_adv);
        #endif
    #endif

}


void beacon_configParamInit(telink_beacon_config_t * config_p){
	config_p->txpower_mode = BEACON_TX_POWER_DEFAULT;//default 0 dbm
	config_p->beacon_period = BEACON_PERIOD_DEFAULT; //default 1000ms

	u8 adv_type_num_tmp = 0;
	#if (IBEACON_ADV_ENABLE)
		config_p->beacon_mode.adv_mode[adv_type_num_tmp].adv_type=TELINK_IBEACON_MODE;
		config_p->beacon_mode.adv_mode[adv_type_num_tmp].adv_num = 0x01;
		adv_type_num_tmp++;
	#endif

	#if (EDDYSTONE_UID_ENABLE)
		config_p->beacon_mode.adv_mode[adv_type_num_tmp].adv_type=TELINK_EDDYSTONE_UID_MODE;
		config_p->beacon_mode.adv_mode[adv_type_num_tmp].adv_num = 0x01;
		adv_type_num_tmp++;
	#endif

	#if (EDDYSTONE_URL_ENABLE)
		config_p->beacon_mode.adv_mode[adv_type_num_tmp].adv_type=TELINK_EDDYSTONE_URL_MODE;
		config_p->beacon_mode.adv_mode[adv_type_num_tmp].adv_num = 0x01;
		adv_type_num_tmp++;
	#endif

	#if (EDDYSTONE_TLM_ENABLE)
		config_p->beacon_mode.adv_mode[adv_type_num_tmp].adv_type=TELINK_EDDYSTONE_TLM_MODE;
		config_p->beacon_mode.adv_mode[adv_type_num_tmp].adv_num = 0x01;
		adv_type_num_tmp++;
	#endif

	config_p->beacon_mode.adv_type_num = adv_type_num_tmp; //default only ibeacon one mode

}
/*Used for save beacon Parameter, advise to call it after configuration finish in adv state(not in connect state).*/
void beacon_save_paramter()
{

#if (ADV_STORE_IN_FLASH_EN)
	// Save beacon configuration_parameter

	//1. erase flash, length base on flash type
	flash_erase_sector(FLASH_TELINK_BEACON_CONFIG);
	//2. write back
	flash_write_page(FLASH_TELINK_BEACON_CONFIG, sizeof(telink_beacon_config_t), (u8*)&telink_beacon_config);

	#if (IBEACON_ADV_ENABLE)
		//1. erase flash, length base on flash type
		flash_erase_sector(FLASH_ADDR_IBEACON_ADV);
		//2. write back.
		flash_write_page(FLASH_ADDR_IBEACON_ADV, sizeof(ibeacon_tbl_adv), (u8*)&ibeacon_tbl_adv);
	#endif

	#if (EDDYSTONE_UID_ENABLE)
		//1. erase flash, length base on flash type
		flash_erase_sector(FLASH_ADDR_EDDYSTONE_UID_ADV);
		//2. write back.
		flash_write_page(FLASH_ADDR_EDDYSTONE_UID_ADV, sizeof(eddystone_UID_tbl_adv), (u8*)&eddystone_UID_tbl_adv);

	#endif

	#if (EDDYSTONE_URL_ENABLE)
		//1. erase flash, length base on flash type
		flash_erase_sector(FLASH_ADDR_EDDYSTONE_URL_ADV);
		//2. write back.
		flash_write_page(FLASH_ADDR_EDDYSTONE_URL_ADV, sizeof(eddystone_URL_tbl_adv), (u8*) &eddystone_URL_tbl_adv);

	#endif

	#if (EDDYSTONE_TLM_ENABLE)
		//1. erase flash, length base on flash type
		flash_erase_sector(FLASH_ADDR_EDDYSTONE_TLM_ADV);
		//2. write back.
		flash_write_page(FLASH_ADDR_EDDYSTONE_TLM_ADV, sizeof(eddystone_TLM_tbl_adv), (u8*) &eddystone_TLM_tbl_adv);
	#endif
#endif
	return;
}

void beacon_iBeaconPDUassemble( ibeacon_adv_t* ibeacon_p){
    ibeacon_p->adv_type = 0x02; //Set ADV type to non-connectable
    ibeacon_p->adv_len = IBEACON_PDU_LEN - 5; //ADV PDU length
    memcpy(ibeacon_p->adv_addr, (u8*)tbl_mac, 6);
    ibeacon_p->flag_len = 0x02;
    ibeacon_p->flag_type = 0x01;
    ibeacon_p->flag = 0x06;
    ibeacon_p->ibeacon_len = 0x1A;
    ibeacon_p->manu_spec = 0xFF;
    ibeacon_p->commpany_ID = 0x004C; //apple

    ibeacon_p->beacon_type = 0x1502;//0x0215
    memcpy(ibeacon_p->proximity_UUID, (u8 *)telink_uuid4beacon, 16); //Defined by Customer
    #if IBeacon_UUID_WeChat_Telink01
    ibeacon_p->major = 0x3327; //Telink Ibeacon1: 10035=0x2733
    ibeacon_p->minor = 0x1FBA; //Telink Ibeacon1: 47647=0xBA1F

    #elif IBeacon_UUID_WeChat_Telink02
    ibeacon_p->major = 0x3327; //Telink Ibeacon2: 10035=0x2733
    ibeacon_p->minor = 0x20BD; //Telink Ibeacon2: 48416=0xBD20

    #elif IBeacon_UUID_WeChat_Test
    ibeacon_p->major = 0x0A00; //Defined by Customer, Big Edian, 0x0A00->10
    ibeacon_p->minor = 0x0700; //Defined by Customer, Big Edian,0x0700->7

	#elif IBEACON_UUID_SYSGRATION
    ibeacon_p->major = 0x0A00; //Defined by Customer, Big Edian, 0x0A00->10
    ibeacon_p->minor = 0x0700; //Defined by Customer, Big Edian,0x0700->7

    #endif
    ibeacon_p->measured_power = beacon_txPowerValue[(telink_beacon_config.txpower_mode)]-41;//Measured power one meter distance, link loss -41dbm
}

void beacon_eddystonePDUassemble(eddystone_mode esm, u8* eddystone_p){
	eddystone_UID_t* eddystonePDU = (eddystone_UID_t*)eddystone_p;
    if(esm == EDDYSTONE_UID){
        eddystone_UID_t* eddystonePDU = (eddystone_UID_t*)eddystone_p;
        eddystonePDU->adv_len = EDDYSTONE_UID_PDU_LEN -5;
        eddystonePDU->eddystone_len = EDDYSTONE_UID_PDU_LEN-19; //0x17, 19: head 2+ macaddr 6 + eddystone first 8 bytes + crc 3
        eddystonePDU->frame_type = 0x00;
        eddystonePDU->tx_power =  beacon_txPowerValue[(telink_beacon_config.txpower_mode)];
        memset(eddystonePDU->namespace_ID, 0, sizeof(eddystonePDU->namespace_ID));
        memset(eddystonePDU->instance_ID, 0, sizeof(eddystonePDU->instance_ID));
        eddystonePDU->reserved_bytes = 0x0000;
    }
	else if(esm == EDDYSTONE_URL){
	    u8 URLParaLen=0;
	    URLParaLen = (sizeof(telink_defaultURL4beacon)<=18)?sizeof(telink_defaultURL4beacon):18; //URL Scheme: 1byte; Encoded URL 0~17 bytes
	    beacon_eddystoneURLAdvLen = EDDYSTONE_URL_PDU_MAX_LEN - (18 - URLParaLen);

	    eddystone_URL_t* eddystonePDU = (eddystone_URL_t*)eddystone_p;
	    eddystonePDU->adv_len = beacon_eddystoneURLAdvLen - 5;
	    eddystonePDU->eddystone_len = beacon_eddystoneURLAdvLen-19; //Variable, 19: head 2+ macaddr 6 + eddystone first 8 bytes + crc 3
	    eddystonePDU->frame_type = 0x10;//URL
	    eddystonePDU->tx_power =  beacon_txPowerValue[(telink_beacon_config.txpower_mode)];

	    eddystonePDU->scheme_URL = telink_defaultURL4beacon[0];
	    memcpy(eddystonePDU->encoded_URl,telink_defaultURL4beacon+1, (URLParaLen-1) );
	}
	else if(esm == EDDYSTONE_TLM){
	    eddystone_TLM_t* eddystonePDU = (eddystone_TLM_t*)eddystone_p;
	    eddystonePDU->adv_len = EDDYSTONE_TLM_PDU_LEN - 5; //5: head(2)+crc(3)
	    eddystonePDU->eddystone_len = EDDYSTONE_TLM_PDU_LEN-19; //0x11, 19: head 2+ macaddr 6 + eddystone first 8 bytes + crc 3
	    eddystonePDU->frame_type = 0x20;//TLM

	    eddystonePDU->version_TLM = 0x00;
	    eddystonePDU->battery_volt = 0x0000;//calculated by ADC, not supported currently, set 0x0000
	    eddystonePDU->temperature = 0x8000;//calculated by ADC, not supported currently, set 0x8000
	    eddystonePDU->adv_cnt = beacon_adv_couter;
	    eddystonePDU->sec_cnt = beacon_TLMSecCounter;
	}

   /*Configer eddystone PDU common part*/
    if((esm == EDDYSTONE_UID) || (esm == EDDYSTONE_URL) || (esm == EDDYSTONE_TLM)){
     eddystonePDU->adv_type = 0x02; //Set ADV type to non-connectable
     memcpy(eddystonePDU->adv_addr, (u8*)tbl_mac, 6);
     eddystonePDU->flag_len = 0x02;
	 eddystonePDU->flag_type = 0x01;
	 eddystonePDU->flag = 0x06;
	 eddystonePDU->eddystone_type_len = 0x03;
	 eddystonePDU->complete_uuid_type = 0x03;
	 eddystonePDU->eddystone_UUID = 0xFEAA;
	 eddystonePDU->service_data_type = 0x16;
	 eddystonePDU->eddystone_UUID_in_service_data = 0xFEAA;
    }
}

void beacon_init(void ){

	/*Read first adv type stored in beacon_adv_mode_config*/
	u8 firstAdvType = telink_beacon_config.beacon_mode.adv_mode[0].adv_type;

	/*Set adv data*/
	if(beacon_advPDUAddrBuf[firstAdvType] == NULL){
	   while(1); //used for debug
	}

	updateAdvDataPointer( (u8*)beacon_advPDUAddrBuf[firstAdvType]);

	if(telink_beacon_config.beacon_mode.adv_type_num >1){
		beacon_adv_type_index= 0x00;
		beacon_adv_type_num = 0x01;
		bls_app_registerEventCallback (BLT_EV_FLAG_BEACON_DONE, &beacon_nextBeacon); //Register callback function
	}else{
		bls_app_registerEventCallback (BLT_EV_FLAG_BEACON_DONE, NULL); //Deregister callback function
	}

	/*Init beacon adv tx power*/
	rf_set_power_level_index(beacon_txLevelIndex[(telink_beacon_config.txpower_mode)]);

//	/*Init beacon RF*/
//	rf_beaconInit();

	/*Start send beacon adv*/
	u16 adv_interval = (telink_beacon_config.beacon_period * 1000)/625;
	bls_ll_setAdvInterval(adv_interval, adv_interval);

}

void rf_beaconInit(void){

    REG_ADDR8(0xf04) = 0x50; //Set TX Settle time as 80us
    REG_ADDR8(0xf0e) = 0x00;  //Set TX Wait time as 0us

    //change system clock to rc type for low power consumption.
    //can't used for connectable adv .
    REG_ADDR8(0x66) = 0xa2;


}

void rf_connAdvInit(){

    REG_ADDR8(0xf04) = 0x61; //Set TX Settle time as 97us
    REG_ADDR8(0xf0e) = 0x09;  //Set TX Wait time as 9us

    // change system clock to pll type
    REG_ADDR8(0x66) = 0x2c;
}
int setTxPowerMode(void* p){
    rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
    if((pm->value <= TX_POWERMODE_MAX) && (pm->value >= 0)){
        telink_beacon_config.txpower_mode = pm->value;
        rf_set_power_level_index(telink_beacon_config.txpower_mode);
    }
    return 0;
}

int setBeaconPeriod(void* p){
    rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
    u16 temV = 0;
    temV = pm->value + (*(&pm->value + 1)<<8);
    if((temV>=100) && (temV<=BEACON_PERIOD_MAX)&&(telink_beacon_config.beacon_period != temV)){
        telink_beacon_config.beacon_period = temV;   //  unit of ms
    }
    return 0;
}

int setIbeaconProximityUUID(void* p){
	rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
	memcpy(ibeacon_tbl_adv.proximity_UUID, &pm->value,16);
    return 0;
}

int setBeaconMajor(void* p){
	rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
	u16 beacon_major_tmp = pm->value + (*(&pm->value + 1)<<8);
	ibeacon_tbl_adv.major = beacon_major_tmp;
    return 0;
}
int setBeaconMinor(void* p){
	rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
	u16 beacon_minor_tmp = pm->value + (*(&pm->value + 1)<<8);

	ibeacon_tbl_adv.minor = beacon_minor_tmp;
    return 0;
}
int setEddystoneID(void* p){
#if EDDYSTONE_UID_ENABLE
	rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
	memcpy(eddystone_UID_tbl_adv.namespace_ID, &pm->value,16);
#endif
	return 0;
}

int setBeaconMode(void* p){
    u8 i=0;
    u8 status = BLE_SUCCESS;
    rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
    beacon_adv_mode_config_t* mode_config = (beacon_adv_mode_config_t*)(&(pm->value));
    if((mode_config->adv_type_num>0) && (mode_config->adv_type_num<MAX_ADV_TYPE_PER_PERIOD)){
        /*Check mode_config para is valid*/
        for(i=0; i<mode_config->adv_type_num; i++){
            status = (mode_config->adv_mode[i].adv_type<TELINK_BEACON_MODE_MAX)?BLE_SUCCESS:1;
            if(status){
                break;
            }
        }

        if(status == BLE_SUCCESS){
            u8 len = 1+mode_config->adv_type_num*2; //type_num(1) + config (type_num*2)
            memcpy((u8*)&(telink_beacon_config.beacon_mode), (u8*)mode_config, len);
        }
    }
    return 0;
}

int resetBeaconCharater(void*p){
    rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
    if(pm->value){
		beacon_configParamInit(&telink_beacon_config);

        #if (IBEACON_ADV_ENABLE)
            beacon_iBeaconPDUassemble(&ibeacon_tbl_adv);
        #endif

        #if (EDDYSTONE_UID_ENABLE)
            beacon_eddystonePDUassemble(EDDYSTONE_UID, (u8*)&eddystone_UID_tbl_adv);
        #endif

        #if (EDDYSTONE_URL_ENABLE)
            beacon_eddystonePDUassemble(EDDYSTONE_URL, (u8*)&eddystone_URL_tbl_adv);
        #endif

        #if (EDDYSTONE_TLM_ENABLE)
            beacon_eddystonePDUassemble(EDDYSTONE_TLM, (u8*)&eddystone_TLM_tbl_adv);
        #endif
    }
    return 0;
}

void updateAdvDataPointer(u8*p){
	beacon_p_pkt = p;
}


