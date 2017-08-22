#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/hci/hci.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll/ll_whitelist.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#if (PRINT_DEBUG_INFO)
#include "../common/myprintf.h"
#endif

#if E2E_CRC_FLAG_ENABLE
extern unsigned short e2e_crc16 (unsigned char *pD, int len);
#endif

u8 tx_done_status = 1;

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 80, 8);
//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xa4, 0xc1, 0x38, 0x11, 0x22, 0x1c};

u8	tbl_advData[] = {
	 0x05, 0x09, 't', 'M', 'o', 'd',
	 0x02, 0x01, 0x05, 							// BLE limited discoverable mode and BR/EDR not supported
};

u8	tbl_scanRsp [] = {
		 0x07, 0x09, 't', 'M', 'o', 'd', 'u', 'l',	//scan name " tmodul"
	};

void	task_connect (void)
{
	//bls_l2cap_requestConnParamUpdate (12, 32, 0, 400);
}


#if E2E_CRC_FLAG_ENABLE//Test [CDC Demo Continuous Glucose Monitoring Service CGMS_v1.0.1 P35 :computation for a sample
u8 data[10] ={0x3e,1,2,3,4,5,6,7,8,9};
volatile u16 crc;
#endif

void user_init()
{
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

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
	bls_smp_enableParing (SMP_PARING_PEER_TRRIGER );

#if E2E_CRC_FLAG_ENABLE
	crc = e2e_crc16(data, 10);printf("Demo test CRC function! CRC =%d",crc);
#endif
	///////////////////// USER application initialization ///////////////////

	bls_ll_setAdvData( tbl_advData, sizeof(tbl_advData) );
	bls_ll_setScanRspData(tbl_scanRsp, sizeof(tbl_scanRsp));


	u8 status = bls_ll_setAdvParam( ADV_INTERVAL_20MS, ADV_INTERVAL_30MS, \
			 	 	 	 	 	     ADV_TYPE_CONNECTABLE_UNDIRECTED, OWN_ADDRESS_PUBLIC, \
			 	 	 	 	 	     0,  NULL,  BLT_ENABLE_ADV_ALL, ADV_FP_NONE);

	if(status != BLE_SUCCESS){  //adv setting err
		write_reg8(0x8000, 0x11);  //debug
		while(1);
	}

#if BLP || CGMP
	bls_ll_setAdvEnable(0);  //adv disable
#else
	bls_ll_setAdvEnable(1);  //adv enable
#endif

	rf_set_power_level_index (RF_POWER_8dBm);

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);

#if CGMS || CGMP//params init
	extern cgm_feature_packet cgm_feature_packet_val;
	extern cgm_status_packet cgm_status_packet_val;
	extern cgm_session_run_time_packet cgm_session_run_time_packet_val;
	extern void simulate_cgm_measurement_data(void);
#if !E2E_CRC_FLAG_ENABLE
	cgm_feature_packet_val.cgmFeature[0] = 0b00000000;
	cgm_feature_packet_val.cgmFeature[1] = 0b10000000;//CGM Quality supported;
	cgm_feature_packet_val.cgmFeature[2] = 0b00000001;//CGM Trend Information supported;
	cgm_feature_packet_val.cgmTypeSample = 3 | 2<<4 ;//cgmType-Capillary Whole blood ; cgmSample-Alternate Site Test (AST)
	cgm_feature_packet_val.e2eCRC = 0xFFFF;//the device doesn妤规悤 support E2E-safety & cgmFeature bit12:0
#else
	cgm_feature_packet_val.cgmFeature[0] = 0b00000000;
	cgm_feature_packet_val.cgmFeature[1] = 0b10010000;//CGM Quality supported;E2E-safety supported
	cgm_feature_packet_val.cgmFeature[2] = 0b00000001;//CGM Trend Information supported;
	cgm_feature_packet_val.cgmTypeSample = 3 | 2<<4 ;//cgmType-Capillary Whole blood ; cgmSample-Alternate Site Test (AST)
	cgm_feature_packet_val.e2eCRC = e2e_crc16((u8*)&cgm_feature_packet_val, sizeof(cgm_feature_packet)-2);//the device doesn妤规悤 support E2E-safety & cgmFeature bit12:0
#endif

	cgm_status_packet_val.timeOffset = 4;
#if E2E_CRC_FLAG_ENABLE
	cgm_status_packet_val.e2eCRC = e2e_crc16((u8*)&cgm_status_packet_val, sizeof(cgm_status_packet)-2);//the device doesn妤规悤 support E2E-safety & cgmFeature bit12:0
#endif

	cgm_session_run_time_packet_val.cgmSessionRunTime = 2;
#if E2E_CRC_FLAG_ENABLE
	cgm_session_run_time_packet_val.e2eCRC = e2e_crc16((u8*)&cgm_session_run_time_packet_val, sizeof(cgm_session_run_time_packet)-2);//the device doesn妤规悤 support E2E-safety & cgmFeature bit12:0
#endif

	//妯℃嫙鐨勬祴閲忔暟鎹�
	simulate_cgm_measurement_data();

#endif
}


void key_proc(){
    //Key2
	if(!gpio_read(KEY2)){
			sleep_us(50000);
			if(!gpio_read(KEY2)){
#if BLP || CGMP
				bls_ll_setAdvEnable(1);  //adv enable
#endif
#if WSP
				smp_param_reset();//闁瑰尅绠撳▍搴ｇ磼閹存繄鏆板ǎ鍥ｅ墲娴硷拷
#endif
			}
	}

	//Key1
	if(!gpio_read(KEY1)){
		sleep_us(50000);
		if(!gpio_read(KEY1)){
#if BAS_DIS_SCPS_TPS
			//u8 vv= 60;
			//bls_att_pushNotifyData(10, &vv, 1);//BS
			u8 vv = 1;
			bls_att_pushNotifyData(36, &vv, 1);
#elif BLS || BLP
			extern blood_pressure_measure_packet blood_pressure_measure_val;
			time_packet tm;
			tm.year = 2017;
			tm.month = 3;
			tm.day = 6;
			tm.hours = 11;
			tm.minutes = 56;
			tm.seconds = 43;

			//Blood Pressure Units Flag(mmg);Time Stamp Flag(present);Pulse Rate Flag(present);	User ID Flag(not present);Measurement Status Flag(present)
			blood_pressure_measure_val.bpmFlag = 0b0011110;
			blood_pressure_measure_val.bpmSys = 80.9;
			blood_pressure_measure_val.bpmSysDiastoli = 120.2;
			blood_pressure_measure_val.bpmMAR = 88;
			blood_pressure_measure_val.timeInf = tm;
			blood_pressure_measure_val.pulseRate = 78;
			blood_pressure_measure_val.userID = 0x01;
			blood_pressure_measure_val.measurementStatus = 0;

			bls_att_pushIndicateData(10, (u8*)&blood_pressure_measure_val, sizeof(blood_pressure_measure_packet));
#elif HRS || HRP
			extern heart_rate_measurement_packet heart_rate_measure_val;
			//Heart Rate Value Format(u16 bpm);Sensor Contact Status(Sensor Contact feature is not supported in the current connection);	Energy Expended Status( not present);
			//RR-Interval (One or more RR-Interval values are present)
			heart_rate_measure_val.hrmFlag = 0b00010011;
			heart_rate_measure_val.hrVal = 78;
			heart_rate_measure_val.eryexd = 5;
			heart_rate_measure_val.rr_interval = 7;
			bls_att_pushNotifyData(10, (u8*)&heart_rate_measure_val, sizeof(heart_rate_measure_val));

#elif WSS || WSP
			extern weight_measure_packet weightScale_measure_val;
			time_packet tm;
			tm.year = 2017;
			tm.month = 3;
			tm.day = 6;
			tm.hours = 11;
			tm.minutes = 56;
			tm.seconds = 43;
			//Measurement Units(kg);Time stamp present;	User ID present;BMI and Height present;
			weightScale_measure_val.wmFlag = 0b00001110;
			weightScale_measure_val.wmHeight = 655;
			weightScale_measure_val.timeInf = tm;
			weightScale_measure_val.userID = 1;
			weightScale_measure_val.wmBMI = 22;
			weightScale_measure_val.wmHeight = 173;

#if WSP//test for WSP/SEN/WST/BI-01-I [Single User Weight Scale 闁炽儻鎷種o Bond Relation]
			extern int flash_exist_data (u32 flash_addr);
			if(flash_exist_data(0x74000)){//闁哄被鍎冲﹢鍖淟ASH闁哄嫷鍨伴幆渚�嫉婢跺瞼鎷ㄩ悗瑙勭煯娣囧﹪骞侀銈囩婵炲备鍓濆﹢浣轰焊閸欐鐟濋柛娆愬灴閿熶粙寮悧鍫濈ウ
				bls_att_pushIndicateData(10, (u8*)&weightScale_measure_val, sizeof(weight_measure_packet));
			}
			else{
				//Do nothing
			}
#else
			bls_att_pushIndicateData(10, (u8*)&weightScale_measure_val, sizeof(weight_measure_packet));
#endif

#endif
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
	blt_sdk_main_loop ();

	key_proc();

#if CGMS || CGMP
	extern void process_RACP_write_callback(void);
	extern void process_CSOCP_write_callback(void);
	process_RACP_write_callback();
	process_CSOCP_write_callback();
#endif

}
