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
#if (PRINT_DEBUG_INFO)
#include "../common/myprintf.h"
#endif

extern unsigned short crc16 (unsigned char *pD, int len);

u8 tx_done_status = 1;

MYFIFO_INIT(blt_rxfifo, 64, 8);
MYFIFO_INIT(blt_txfifo, 80, 8);
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

void	task_connect (void)
{
	//bls_l2cap_requestConnParamUpdate (12, 32, 0, 400);
#if CGMS || CGMP//params init
	extern cgm_measurement_packet cgm_measurement_packet_val;
	extern cgm_feature_packet cgm_feature_packet_val;
	extern cgm_status_packet cgm_status_packet_val;
	extern cgm_session_start_time_packet cgm_session_start_time_packet_val;
	extern cgm_session_run_time_packet cgm_session_run_time_packet_val;
	extern record_access_control_point_packet record_access_control_point_packet_val;
	extern cgm_specific_ops_control_point_packet cgm_specific_ops_control_point_packet_val;

#if !E2E_CRC_FLAG_ENABLE
	cgm_feature_packet_val.cgmFeature[0] = 0b00000000;
	cgm_feature_packet_val.cgmFeature[1] = 0b10000000;//CGM Quality supported;
	cgm_feature_packet_val.cgmFeature[2] = 0b00000001;//CGM Trend Information supported;
	cgm_feature_packet_val.cgmTypeSample = 3 | 2<<4 ;//cgmType-Capillary Whole blood ; cgmSample-Alternate Site Test (AST)
	cgm_feature_packet_val.e2eCRC = 0xFFFF;//the device doesn´t support E2E-safety & cgmFeature bit12:0
#else
	cgm_feature_packet_val.cgmFeature[0] = 0b00000000;
	cgm_feature_packet_val.cgmFeature[1] = 0b10010000;//CGM Quality supported;E2E-safety supported
	cgm_feature_packet_val.cgmFeature[2] = 0b00000001;//CGM Trend Information supported;
	cgm_feature_packet_val.cgmTypeSample = 3 | 2<<4 ;//cgmType-Capillary Whole blood ; cgmSample-Alternate Site Test (AST)
	cgm_feature_packet_val.e2eCRC = crc16((u8*)&cgm_feature_packet_val, sizeof(cgm_feature_packet)-2);//the device doesn´t support E2E-safety & cgmFeature bit12:0
#endif

	cgm_measurement_packet_val.size = 10;
	cgm_measurement_packet_val.cgmMflg = 0b00000011;//CGM Trend Information Present;CGM Quality Present;
	cgm_measurement_packet_val.cgmGlucoseConcentration =44;
	cgm_measurement_packet_val.timeOffset = 5;
	cgm_measurement_packet_val.cgmTrendInformation = 42;
	cgm_measurement_packet_val.cgmQuality = 33;
#if E2E_CRC_FLAG_ENABLE
	cgm_feature_packet_val.e2eCRC = crc16((u8*)&cgm_measurement_packet_val, sizeof(cgm_measurement_packet)-2);//the device doesn´t support E2E-safety & cgmFeature bit12:0
#endif

	cgm_status_packet_val.timeOffset =4;
#if E2E_CRC_FLAG_ENABLE
	cgm_status_packet_val.e2eCRC = crc16((u8*)&cgm_status_packet_val, sizeof(cgm_status_packet)-2);//the device doesn´t support E2E-safety & cgmFeature bit12:0
#endif

	cgm_session_run_time_packet_val.cgmSessionRunTime = 2;
#if E2E_CRC_FLAG_ENABLE
	cgm_session_run_time_packet_val.e2eCRC = crc16((u8*)&cgm_session_run_time_packet_val, sizeof(cgm_session_run_time_packet)-2);//the device doesn´t support E2E-safety & cgmFeature bit12:0
#endif

#endif
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


void user_init()
{
	rf_customized_param_load();  //load customized freq_offset cap value and tp value

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
	bls_smp_enableParing (SMP_PARING_PEER_TRRIGER );


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

	bls_ll_setAdvEnable(1);  //adv enable

	rf_set_power_level_index (RF_POWER_8dBm);

	//ble event call back
	bls_app_registerEventCallback (BLT_EV_FLAG_CONNECT, &task_connect);

	//模拟200条测量数据
#if CGMS_SEN_RAA_BV_01_C
	simulate_cgm_measurement_data();
#endif
}


void key_proc(){
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
			bls_att_pushIndicateData(10, (u8*)&weightScale_measure_val, sizeof(weight_measure_packet));


#endif
		}
	}
}
/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
extern void process_RACP_write_callback(void);
void main_loop ()
{
	static u32 tick_loop;
	tick_loop ++;
	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_slave_main_loop ();

	key_proc();

	process_RACP_write_callback();
	process_CSOCP_write_callback();

}
