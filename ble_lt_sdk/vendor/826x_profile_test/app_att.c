#if(__PROJECT_826X_PROFILE_TEST__)

#include "../../proj/tl_common.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#if (PRINT_DEBUG_INFO)
#include "../common/myprintf.h"
#endif

typedef struct
{
  /** Minimum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMin;
  /** Maximum value for the connection event (interval. 0x0006 - 0x0C80 * 1.25 ms) */
  u16 intervalMax;
  /** Number of LL latency connection events (0x0000 - 0x03e8) */
  u16 latency;
  /** Connection Timeout (0x000A - 0x0C80 * 10 ms) */
  u16 timeout;
} gap_periConnectParams_t;

const u16 clientCharacterCfgUUID        		= GATT_UUID_CLIENT_CHAR_CFG;

const u16 extReportRefUUID              		= GATT_UUID_EXT_REPORT_REF;

const u16 reportRefUUID                			= GATT_UUID_REPORT_REF;

const u16 characterPresentFormatUUID    		= GATT_UUID_CHAR_PRESENT_FORMAT;

const u16 my_primaryServiceUUID        			= GATT_UUID_PRIMARY_SERVICE;

static const u16 my_characterUUID       		= GATT_UUID_CHARACTER;

const u16 my_devServiceUUID             		= SERVICE_UUID_DEVICE_INFORMATION;

const u16 my_PnPUUID                    		= CHARACTERISTIC_UUID_PNP_ID;

const u16 my_devNameUUID                		= GATT_UUID_DEVICE_NAME;

//device information
const u16 my_gapServiceUUID             		= SERVICE_UUID_GENERIC_ACCESS;
// Device Name Characteristic Properties
static u8 my_devNameCharacter           		= CHAR_PROP_READ | CHAR_PROP_NOTIFY;
// Appearance Characteristic Properties
const u16 my_appearanceUIID             		= 0x2a01;
const u16 my_periConnParamUUID          		= 0x2a04;
static u8 my_appearanceCharacter        		= CHAR_PROP_READ;
// Peripheral Preferred Connection Parameters Characteristic Properties
static u8 my_periConnParamChar          		= CHAR_PROP_READ;
u16 my_appearance                       		= GAP_APPEARE_UNKNOWN;
gap_periConnectParams_t my_periConnParameters 	= {20, 40, 0, 1000};

#define DEV_NAME                        "tyfModule"
extern u8  ble_devName[];



/////////////////////////////  Battery ///////////////////////////////////////////////////
const u16 my_batServiceUUID       				= SERVICE_UUID_BATTERY;
static u8 my_batProp 							= CHAR_PROP_READ | CHAR_PROP_NOTIFY;
const u16 my_batCharUUID       					= CHARACTERISTIC_UUID_BATTERY_LEVEL;
const u16 my_batPresentationFormatUUID     		= GATT_UUID_CHAR_PRESENT_FORMAT;
u8 		  my_batVal[1] 							= {99};
// Battery Level Presentation format
/*Format 1byte, Exponent 1, Unit 2, Name Space 1, Description 2*/
u8 batty_level_presentFmt[7]                	={4,0,0xA7,0x2D,1,0,1};
// Battery Level Client Characteristic Configuration
u16 batty_level_clientCharCfg;
const u16 userdesc_UUID		                	= GATT_UUID_CHAR_USER_DESC;
//////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////// device information ////////////////////////////////////////
const u16 devInfo_serviceUUID  					= SERVICE_UUID_DEVICE_INFORMATION;
// Characterstics UUID
u16 manuNameString_charUUID  					= CHARACTERISTIC_UUID_MANU_NAME_STRING;
u16 modleNameString_charUUID 					= CHARACTERISTIC_UUID_MODEL_NUM_STRING;
u16 serialString_charUUID    					= CHARACTERISTIC_UUID_SERIAL_NUM_STRING;
u16 hwRevision_charUUID      					= CHARACTERISTIC_UUID_HW_REVISION_STRING;
u16 fwRevision_charUUID      					= CHARACTERISTIC_UUID_FW_REVISION_STRING;
u16 swRevision_charUUID      					= CHARACTERISTIC_UUID_SW_REVISION_STRING;
u16 systemID_charUUID        					= CHARACTERISTIC_UUID_SYSTEM_ID;
u16 ieee11703_charUUID       					= CHARACTERISTIC_UUID_IEEE_11073_CERT_LIST;
u16 pnpID_charUUID           					= CHARACTERISTIC_UUID_PNP_ID;
// Characterstics Property
u8 manuNameString_prop  						= CHAR_PROP_READ;
u8 modleNameString_prop 						= CHAR_PROP_READ;
u8 serialString_prop    						= CHAR_PROP_READ;
u8 hwRevision_prop      						= CHAR_PROP_READ;
u8 fwRevision_prop      						= CHAR_PROP_READ;
u8 swRevision_prop      						= CHAR_PROP_READ;
u8 systemID_prop        						= CHAR_PROP_READ;
u8 ieee11703_prop       						= CHAR_PROP_READ;
u8 pnpID_prop           						= CHAR_PROP_READ;
// Characterstics Value
const u8 manuNameString_value[] 				= "TELINK Tech.";
const u8 modelNameString_value[] 				= "FH005";
const u8 serialString_value[] 					= "serial";
const u8 hwRevision_value[] 					= "V1.1";
const u8 fwRevision_value[] 					= "V1.6";
const u8 swRevision_value[] 					= "V1.6";
const u8 systemID_value[SYSTEM_ID_LEN] 			= {0};
const u8 ieee11703_value[DEVINFO_PNP_ID_LEN] 	= {0};
const u8 pnpID_value[DEVINFO_PNP_ID_LEN] 		= {1, 0x0d, 0x00, 0x00, 0x00, 0x10, 0x01};
/////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////// Scan parameter /////////////////////////////////////////////
#define CHARACTERISTIC_UUID_SCAN_INTERVAL_WINDOW	0x2A4F
#define CHARACTERISTIC_UUID_SCAN_REFRESH        	0x2A31
const u16 scanPara_serviceUUID  				= SERVICE_UUID_SCAN_PARAMETER;
// Scan Parameters Characteristic Property
u8 scanIntervalWin_prop 						= CHAR_PROP_WRITE_WITHOUT_RSP;
u8 scanRefresh_prop     						= CHAR_PROP_NOTIFY;

typedef struct {
	u16 scanInterval;
	u16 scanWindow;
} scan_intervalWindow_t;

// Scan Parameters Characteristic Values
scan_intervalWindow_t scanIntervalWin_value;
u8 scanRefresh_value 							= FALSE;
// Scan Parameters Characteristic UUID
u16 scanIntervalWin_charUUID 					= CHARACTERISTIC_UUID_SCAN_INTERVAL_WINDOW;
u16 scanRefresh_charUUID     					= CHARACTERISTIC_UUID_SCAN_REFRESH;
// Scan Refresh Client Characteristic Configuration
u16 scanRefresh_clientCharCfg;
//////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////// Tx power /////////////////////////////////////////////////
// Txpower_uuid Txpower Charactersitc UUID
#define CHARACTERISTIC_UUID_TX_POWER_LEVEL      0x2A07
//brief  Definition for TX Power Level Range
#define TX_POWER_MIN                           	-100
#define TX_POWER_MAX                            20

const u16 txPower_serviceUUID  					= SERVICE_UUID_TX_POWER;
// TX Power Level Property
u8 txPowerLevel_prop 							= CHAR_PROP_READ;
// TX Power Level Value in Percentage
s8 txPowerLevel_value 							= TX_POWER_MAX;
// TX Power Level UUID
u16 txPowerLevel_charUUID 						= CHARACTERISTIC_UUID_TX_POWER_LEVEL;
//////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////// PXP profile //////////////////////////////////////////////
//============================= 1.link loss =========================================
#define CHARACTERISTIC_UUID_ALERT_LEVEL         0x2A06
//Definiton for Alert Level
#define ALERT_LEVEL_NO_ALERT                    0
#define ALERT_LEVEL_MILD_ALERT                  1
#define ALERT_LEVEL_HIGH_ALERT                  2

const u16 linkLoss_serviceUUID  				= SERVICE_UUID_LINK_LOSS;
// Alert Level Property
u8 alertLevel_prop 								= CHAR_PROP_READ | CHAR_PROP_WRITE;
// Alert Level Value in Percentage
u8 alertLevel_value 							= ALERT_LEVEL_HIGH_ALERT;
// Alert Level UUID
u16 alertLevel_charUUID 						= CHARACTERISTIC_UUID_ALERT_LEVEL;
//============================= 2.Immediate Alert ===================================
const u16 immediateAlert_serviceUUID  = SERVICE_UUID_IMMEDIATE_ALERT;
// Alert Level Property
#if 1//_CERTIFICATION_TEST_MODE_
u8 immediateAlertLevel_prop                     = CHAR_PROP_WRITE_WITHOUT_RSP;
#else
u8 immediateAlertLevel_prop                     = CHAR_PROP_WRITE_WITHOUT_RSP | CHAR_PROP_NOTIFY;
#endif
// Alert Level Value in Percentage
u8 immediateAlertLevel_value                    = ALERT_LEVEL_NO_ALERT;
// Alert Level UUID
u16 immediateAlertLevel_charUUID                = CHARACTERISTIC_UUID_ALERT_LEVEL;
//=============================== 3.Tx power ========================================
//line:129 ~ 143.above.
///////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////// weight scale /////////////////////////////////////////////
#define CHARACTERISTIC_UUID_WEIGHT_SCALE_FEATURE                    0x2A9E
#define CHARACTERISTIC_UUID_WEIGHT_MEASUREMENT						0x2A9D

const u16 weightScale_serviceUUID  				= SERVICE_UUID_WEIGHT_SCALE;
const u16 weightScale_featureUUID 				= CHARACTERISTIC_UUID_WEIGHT_SCALE_FEATURE;
// weight scale feature Property
u8 weightScale_feature_prop 					= CHAR_PROP_READ;
//weight scale feature value
u32 weightScale_feature_val 					= 0x0000019f;//Time Stamp Supported \supported bmi\Multiple Users Supported, weight resolution 0.1kg, height resolution 0.001m
const u16 weightScale_measureUUID 				= CHARACTERISTIC_UUID_WEIGHT_MEASUREMENT;
// weight measure feature Property
u8 weightScale_measure_prop 					= CHAR_PROP_INDICATE;
//weight measure value, needs modify according requirements
//u8 weightScale_measure_val[19] = {0};
weight_measure_packet weightScale_measure_val;
u16 weightScale_measureCCC 						= 0x0000;
/////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////// weight scale Profile ///////////////////////////////////////
//=============================== 1.Tx power ========================================
//line:179 ~ 254.above.
//========================== 2.device information ===================================
//line:71 ~ 103.above.
//========================User DataService (Optional)================================
//====================Body CompositionService (Optional)=============================
//=========================Battery Service (Optional)================================
//======================Current Time Service (Optional)==============================
/////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////// blood pressure ///////////////////////////////////////////
#define CHARACTERISTIC_UUID_BLOOD_PRESSURE_MEASUREMENT		 0x2A35
#define CHARACTERISTIC_UUID_INTERMEDIATE_CUFF_PRESSURE       0x2A36
#define CHARACTERISTIC_UUID_BLOOD_PRESSURE_FEATURE           0x2A49

const u16 bloodPressure_serviceUUID             = SERVICE_UUID_BLOOD_PRESSURE;

const u16 bloodPressure_measureUUID             = CHARACTERISTIC_UUID_BLOOD_PRESSURE_MEASUREMENT;
//Blood Pressure Measurement Property
u8 bloodPressureMeasurement_prop				= CHAR_PROP_INDICATE;
// Blood Pressure Measurement Client Characteristic Configuration
u16 bloodPressureMeasurement_clientCharCfg;

u16 intermediate_cuff_pressureUUID              = CHARACTERISTIC_UUID_INTERMEDIATE_CUFF_PRESSURE;
//Intermediate Cuff Pressure Property
u8 intermediate_cuff_pressure_prop				= CHAR_PROP_NOTIFY;
// Intermediate Cuff Pressure Client Characteristic Configuration
u16 intermediate_cuff_pressure_clientCharCfg;

u16 blood_pressure_featureUUID                  = CHARACTERISTIC_UUID_BLOOD_PRESSURE_FEATURE;
//Blood Pressure Feature Property
u8 blood_pressure_feature_prop					= CHAR_PROP_READ;

blood_pressure_measure_packet blood_pressure_measure_val;
blood_pressure_measure_packet intermediate_cuff_pressure_val;
u16 blood_pressure_feature_val = 0x0c;//00001100 	Irregular Pulse Detection feature supported
                                                  //Pulse Rate Range Detection feature supported
///////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////// Heart Rate /////////////////////////////////////////////
#define CHARACTERISTIC_UUID_HEART_RATE_MEASUREMENT      0x2A37
#define CHARACTERISTIC_UUID_BODY_SENSOR_LOCATION        0x2A38
#define CHARACTERISTIC_UUID_HEART_RATE_CONTROL_POINT    0x2A39

const u16 heartRate_serviceUUID             	= SERVICE_UUID_HEART_RATE;

const u16 heart_rate_measurementUUID            = CHARACTERISTIC_UUID_HEART_RATE_MEASUREMENT;
//Heart Rate Measurement Property
u8 heart_rate_measurement_prop				    = CHAR_PROP_NOTIFY;
// Heart Rate Measurement Client Characteristic Configuration
u16 heart_rate_measurement_clientCharCfg;

const u16 body_sensor_locationUUID              = CHARACTERISTIC_UUID_BODY_SENSOR_LOCATION;
//Body Sensor Location Property
u8 body_sensor_location_prop				    = CHAR_PROP_READ;

u16 heart_rate_control_pointUUID                = CHARACTERISTIC_UUID_HEART_RATE_CONTROL_POINT;
//Heart Rate Control Point Property
u8 heart_rate_control_point_prop				= CHAR_PROP_WRITE;

heart_rate_measurement_packet heart_rate_measure_val;
u8 body_sensor_location_val = 3;//Finger
u8 heart_rate_control_point_val;
/////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////// continuous glucose monitoring //////////////////////////////////
#define CHARACTERISTIC_UUID_CGM_MEASUREMENT	           	 	  0x2AA7
#define CHARACTERISTIC_UUID_CGM_FEATURE                	 	  0x2AA8
#define CHARACTERISTIC_UUID_CGM_STATUS                  	  0x2AA9
#define CHARACTERISTIC_UUID_CGM_SESSION_START_TIME      	  0x2AAA
#define CHARACTERISTIC_UUID_CGM_SESSION_RUN_TIME        	  0x2AAB
#define CHARACTERISTIC_UUID_RECORD_ACCESS_CONTROL_POINT	      0x2A52
#define CHARACTERISTIC_UUID_CGM_SPECIFIC_OPS_CONTROL_POINT    0x2AAC

const u16 continuousGlucoseMonitoring_serviceUUID = SERVICE_UUID_CONTINUOUS_GLUCOSE_MONITORING;

const u16 cgm_measurementUUID                     = CHARACTERISTIC_UUID_CGM_MEASUREMENT;
//CGM Measurement Property
u8 cgm_measurement_prop				    	      = CHAR_PROP_NOTIFY;
// cgm measurement Client Characteristic Configuration
u16 cgm_measurement_clientCharCfg;

const u16 cgm_featureUUID                         = CHARACTERISTIC_UUID_CGM_FEATURE;
//CGM Feature Property
u8 cgm_feature_prop				    	          = CHAR_PROP_READ;

const u16 cgm_statusUUID                          = CHARACTERISTIC_UUID_CGM_STATUS;
//CGM Status Property
u8 cgm_status_prop				    	          = CHAR_PROP_READ;

const u16 cgm_session_start_timeUUID              = CHARACTERISTIC_UUID_CGM_SESSION_START_TIME;
//CGM Status Property
u8 cgm_session_start_time_prop				      = CHAR_PROP_READ | CHAR_PROP_WRITE;

const u16 cgm_session_run_timeUUID                = CHARACTERISTIC_UUID_CGM_SESSION_RUN_TIME;
//CGM Session Run Time Property
u8 cgm_session_run_time_prop				      = CHAR_PROP_READ;


const u16 record_access_control_pointUUID         = CHARACTERISTIC_UUID_RECORD_ACCESS_CONTROL_POINT;
//CGM Session Run Time Property
u8 record_access_control_point_prop				  = CHAR_PROP_WRITE | CHAR_PROP_INDICATE;
//CGM Session Run Time Client Characteristic Configuration
u16 record_access_control_point_clientCharCfg;

const u16 cgm_specific_ops_control_pointUUID      = CHARACTERISTIC_UUID_CGM_SPECIFIC_OPS_CONTROL_POINT;
//CGM Specific Ops Control Point Property
u8 cgm_specific_ops_control_point_prop			  = CHAR_PROP_WRITE | CHAR_PROP_INDICATE;
//CGM Specific Ops Control Point Client Characteristic Configuration
u16 cgm_specific_ops_control_point_clientCharCfg;

cgm_measurement_packet cgm_measurement_packet_val;
cgm_feature_packet cgm_feature_packet_val;
cgm_status_packet cgm_status_packet_val;
cgm_session_start_time_packet cgm_session_start_time_packet_val;
cgm_session_run_time_packet cgm_session_run_time_packet_val;
record_access_control_point_packet record_access_control_point_packet_val;
cgm_specific_ops_control_point_packet cgm_specific_ops_control_point_packet_val;

////////////////////////////////////////////////////////////////////////////
#if  !CGMS_SEN_RAA_BV_01_C
#define RECORD_NUMS                      4

//模拟的测量数据库：
cgm_measurement_packet cgm_measurement_val[RECORD_NUMS] = {
		{10, 0b00000011, 14,  0, 12, 13},
		{10, 0b00000011, 24,  5, 22, 23},
		{10, 0b00000011, 34, 10, 32, 33},
		{10, 0b00000011, 44, 15, 42, 43},
};
#else
#define RECORD_NUMS                      200
//模拟的测量数据库
cgm_measurement_packet cgm_measurement_val[RECORD_NUMS];
void simulate_cgm_measurement_data(void){
	foreach(i, RECORD_NUMS){
		cgm_measurement_val[i].size = 10;
		cgm_measurement_val[i].cgmMflg = 0b00000011;
		cgm_measurement_val[i].cgmGlucoseConcentration = 10;
		cgm_measurement_val[i].timeOffset = i*5;
		cgm_measurement_val[i].cgmTrendInformation = 11;
		cgm_measurement_val[i].cgmQuality =22;
#if E2E_CRC_FLAG_ENABLE
		cgm_measurement_val[i].e2eCRC = crc16((u8*)&cgm_measurement_val, sizeof(cgm_measurement_packet)-2); // This field is mandatory,if the device supports E2E-CRC (Bit 12 in CGM Feature is set to 1) otherwise excluded.
#endif
	}
}
#endif
///////////////////////////////////////////////////////////////////////////

extern const rf_packet_att_writeRsp_t pkt_writeRsp;
extern rf_packet_att_errRsp_t pkt_errRsp;
extern void	blt_push_fifo_hold (u8 *p);

int cgm_session_start_time_write_callback(void * p){
	rf_packet_att_write_t *req = (rf_packet_att_write_t*)p;
	cgm_session_start_time_packet * tmp = (cgm_session_start_time_packet*)&req->value;

	if(!(-48 <= tmp->timeZone && tmp->timeZone <= 56)){
		pkt_errRsp.errReason = 0xFF;//Out of Range
		u8* r = (u8 *)(&pkt_errRsp);
		blt_push_fifo_hold (r + 4);
	}

	if((req->l2capLen - 3) == (sizeof(time_packet) + 2)){//No CRC
		pkt_errRsp.errReason = 0x80;//Missing CRC
		u8* r = (u8 *)(&pkt_errRsp);
		blt_push_fifo_hold (r + 4);
	}
	memcpy(&cgm_session_start_time_packet_val, tmp, sizeof(cgm_session_start_time_packet));


	return 0;
}


//RACP
u8 abort_operation_procedure_flg;
u8 cnt;
u8 write_racp_flg;
rf_packet_att_write_t *tmp_racp_req;
u8 Procedure_in_progress_flg;//表示每一个op处理是否结束
int record_access_control_point_write_callback(void *p){
	tmp_racp_req = (rf_packet_att_write_t*)p;
	record_access_control_point_packet * tmp_racp = (record_access_control_point_packet*)&tmp_racp_req->value;
	//首先执行write_callback,执行完了后，回复write_rsp.
	//应用层不要做write_rsp动作，协议底层在write_callback执行完后，会发送该指令！
	//blt_push_fifo_hold((u8*)(&pkt_writeRsp) + 4);//先回复Write RSP ，then do write_callback!
    printf("tmp_racp: opCode= %d	operator= %d	operand = %d\n",tmp_racp->opCode,tmp_racp->operator,tmp_racp->operand);
    printf("req->len: %d\n",tmp_racp_req->l2capLen -3);
    printf("record_access_control_point_write_callback\n\r");


	if(Procedure_in_progress_flg){
		pkt_errRsp.errReason = 0xFE;//Procedure Already in Progress
		u8* r = (u8 *)(&pkt_errRsp);
		blt_push_fifo_hold (r + 4);
		printf("write_rsp_with_error:0xFE\n");
		return 0;
	}

    write_racp_flg = 1;

	return 0;
}

u8 Report_Stored_Records_procedure;
void process_RACP_write_callback(void){
	if(!write_racp_flg)
		return;
	else{//write_RACP_occur
		record_access_control_point_packet * tmp_racp = (record_access_control_point_packet*)&tmp_racp_req->value;
		write_racp_flg = 0;
    	Procedure_in_progress_flg = 1;
		u8 timeoffset;
		u16 min_filterVal;
		u8 tmp_operand[3];
		switch(tmp_racp->opCode){
			case Report_stored_records:
				switch(tmp_racp->operator){
					case All_records://No Operand Used
//						if(tmp_racp->operand && (tmp_racp_req->l2capLen-3 == 2)){
//							tmp_racp->operator = 0;//NULL
//							tmp_racp->operand = tmp_racp->opCode | Invalid_Operand<<8 ;
//							tmp_racp->opCode = Response_Code;
//							bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
//							break;
//						}
#if 0
	                    foreach(i, RECORD_NUMS){
	                    	cnt++;
	                    	if(abort_operation_procedure_flg){
								abort_operation_procedure_flg = 0;
								Procedure_in_progress_flg = 0;
								return;
							}
	                    	printf("notify Report_stored_records All_records\n");

	                    	bls_att_pushNotifyData(10, (u8*)&cgm_measurement_val[i], sizeof(cgm_measurement_packet));
	                    }
						tmp_racp->operator = 0;//NULL
						tmp_racp->operand = tmp_racp->opCode | Success<<8 ;
						tmp_racp->opCode = Response_Code;
						bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
#else
						Report_Stored_Records_procedure = 1;
#endif
						break;

					case Greater_than_or_equal_to:
						//In the context of the CGM service, only Filter Type 0x01 (Time Offset) is allowed to be used.
						//tmp_racp->operand;// Filter Type | Filter Parameters(//Time Offset | <minimum filter value>)
						memcpy(tmp_operand, (u8*)&tmp_racp->operand, 3);
						printf("%d	%d	%d\n",tmp_operand[0],tmp_operand[1],tmp_operand[2]);
						timeoffset = tmp_operand[0];
						min_filterVal = tmp_operand[1] + (tmp_operand[2]<<8);
						/*If the Filter Type used within CGM Service is 0x02, the Server shall indicate the Control Point with
						a Response Code Op Code and Response Code Value in the Operand set to Operand not supported*/
						if(timeoffset >= 0x02 || timeoffset == 0){
							tmp_racp->operator = 0;//NULL
							tmp_racp->operand = tmp_racp->opCode | Operand_not_supported<<8;
							tmp_racp->opCode = Response_Code;
							bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
						}
						else if(timeoffset == 0x01){
							u8 filterflg = 0;
#if 0
							foreach(i, RECORD_NUMS){
								if(cgm_measurement_val[i].timeOffset >= min_filterVal){
									filterflg = 1;
									if(abort_operation_procedure_flg){
										abort_operation_procedure_flg = 0;
										Procedure_in_progress_flg = 0;
										return;
									}
									bls_att_pushNotifyData(10, (u8*)&cgm_measurement_val[i], sizeof(cgm_measurement_packet));
								}
							}

							if(filterflg){
								tmp_racp->operator = 0;//NULL
								tmp_racp->operand = tmp_racp->opCode | Success<<8 ;
								tmp_racp->opCode = Response_Code;
								bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
							}
							else{
								tmp_racp->operator = 0;//NULL
								tmp_racp->operand = tmp_racp->opCode | No_records_found<<8  ;
								tmp_racp->opCode = Response_Code;
								bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
							}
#endif
						}
						break;
//					case Less_than_or_equal_to:
//					case Within_range_of:
//					case First_record:
//					case Last_record:

					default:
						//operator==NULL:Operator_not_supported
						//operator==RSVD:Invalid_Operator
						tmp_racp->operand = tmp_racp->opCode | (tmp_racp->operator ? Operator_not_supported : Invalid_Operator)<<8 ;
						tmp_racp->operator = 0;//NULL
						tmp_racp->opCode = Response_Code;
						bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
						break;
				}
				break;

			case Abort_operation://the Server shall stop any RACP procedures currently in progress and shall make a best effort to stop sending any further data.
				if(tmp_racp->operator == 0){//operator == Null (0x00) & No Operand Used
//					if(tmp_racp->operand && (tmp_racp_req->l2capLen-3 == 2)){
//						tmp_racp->operator = 0;//NULL
//						tmp_racp->operand = tmp_racp->opCode | Invalid_Operand<<8 ;
//						tmp_racp->opCode = Response_Code;
//						bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
//						break;
//					}
					abort_operation_procedure_flg = 1;

					tmp_racp->operator = 0;//NULL
					tmp_racp->operand = tmp_racp->opCode | Success<<8 ;
					tmp_racp->opCode = Response_Code;
					bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
				}
				break;

			case Report_number_of_stored_records:
				switch(tmp_racp->operator){
				    record_access_control_point_packet rsps;
					case All_records://No Operand Used
//						if(tmp_racp->operand && (tmp_racp_req->l2capLen-3 == 2)){
//							tmp_racp->operator = 0;//NULL
//							tmp_racp->operand = tmp_racp->opCode | Invalid_Operand<<8 ;
//							tmp_racp->opCode = Response_Code;
//							bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
//							break;
//						}
	                    //Number of Stored Records Response
						rsps.opCode = Number_of_stored_records_response;
						rsps.operator = 0;//NULL
						rsps.operand = RECORD_NUMS;//模拟本地有x笔测量数据
						bls_att_pushIndicateData(21, (u8*)&rsps, sizeof(record_access_control_point_packet));
						break;

					case Greater_than_or_equal_to:
						//In the context of the CGM service, only Filter Type 0x01 (Time Offset) is allowed to be used.
						//tmp_racp->operand;// Filter Type | Filter Parameters bit(//Time Offset | <minimum filter value>)
						memcpy(tmp_operand, (u8*)&tmp_racp->operand, 3);
						printf("%d	%d	%d\n",tmp_operand[0],tmp_operand[1],tmp_operand[2]);
						timeoffset = tmp_operand[0];
						min_filterVal = tmp_operand[1] + (tmp_operand[2]<<8);
						/*If the Filter Type used within CGM Service is 0x02, the Server shall indicate the Control Point with
						a Response Code Op Code and Response Code Value in the Operand set to Operand not supported*/
						if(timeoffset == 0x02){
							tmp_racp->operator = 0;//NULL
							tmp_racp->operand = Report_number_of_stored_records | Operand_not_supported<<8 ;
							tmp_racp->opCode = Response_Code;
							bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
						}
						else if(timeoffset == 0x01){
	                        foreach(i, RECORD_NUMS){
	                        	if(cgm_measurement_val[i].timeOffset >= min_filterVal){
	                        		//Number of Stored Records Response
									tmp_racp->opCode = Number_of_stored_records_response;
									tmp_racp->operator = 0;//NULL
									tmp_racp->operand = RECORD_NUMS - i;
									bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
									break;
	                        	}
	                        }
						}
						break;

//					case Less_than_or_equal_to:
//					case Within_range_of:
//					case First_record:
//					case Last_record:

					default:
						tmp_racp->operator = 0;//NULL
						tmp_racp->operand = tmp_racp->opCode | Invalid_Operator<<8 ;
						tmp_racp->opCode = Response_Code;
						bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
						break;
				}
				break;

//			case Delete_stored_records:
//			case Number_of_stored_records_response:
//			case Response_Code:
//				break;
			default://not support
				tmp_racp->operator = 0;//NULL
				tmp_racp->operand = tmp_racp->opCode  | OpCode_not_supported <<8;
				tmp_racp->opCode = Response_Code;
				bls_att_pushIndicateData(21, (u8*)tmp_racp, sizeof(record_access_control_point_packet));
				break;

		}
		Procedure_in_progress_flg = 0;
    }
}

u8 write_csocp_flg;
cgm_specific_ops_control_point_packet *tmp_csocp;
int cgm_specific_ops_control_point_write_callback(void *p){
	rf_packet_att_write_t *req = (rf_packet_att_write_t*)p;
	tmp_csocp = (cgm_specific_ops_control_point_packet*)&req->value;
	//首先执行write_callback,执行完了后，回复write_rsp.
	//应用层不要做write_rsp动作，协议底层在write_callback执行完后，会发送该指令！
	//blt_push_fifo_hold((u8*)(&pkt_writeRsp) + 4);//先回复Write RSP ，then do write_callback!

	write_csocp_flg = 1;

	return 0;
}

u8 periodic_communication_enable_flg = 1;
u8 communication_interval =                   2;//2min init
#define COMMUNICATION_INTERVAL_SMALLEST       1//device smallest communication interval

extern unsigned short crc16 (unsigned char *pD, int len);

void process_CSOCP_write_callback(void){
	if(!write_csocp_flg)
		return;
	else{//write_RACP_occur
		write_csocp_flg = 0;
    	switch(tmp_csocp->opCode){
    		case Set_CGM_Communication_Interva:
    			periodic_communication_enable_flg = 0;//disable the periodic communication
    			if(tmp_csocp->operand == 0xFF){//Set smallest CGM Communication Interva
    				communication_interval = COMMUNICATION_INTERVAL_SMALLEST;
    			}
    			else{
    				communication_interval = tmp_csocp->operand;//Communication_Interval_in_minutes
    			}
    			*(u16*)&tmp_csocp->operand = tmp_csocp->opCode | csocp_Success<<8;
    			tmp_csocp->opCode = csocp_Response_Code;
    			bls_att_pushIndicateData(24, (u8*)tmp_csocp, 4);//sizeof(cgm_specific_ops_control_point_packet));
    			break;

    		case Get_CGM_Communication_Interva:
                //Operand: N.A.
    			tmp_csocp->opCode = CGM_Communication_Interval_response;
    			tmp_csocp->operand = communication_interval;
    			bls_att_pushIndicateData(24, (u8*)tmp_csocp, 4);//sizeof(cgm_specific_ops_control_point_packet));
    			break;

//    		case Set_Glucose_Calibration_Value:
//    		case Get_Glucose_Calibration_Value:
//    			 //......
//    		case Stop_the_Session:
//    			break

    		default:

				break;
    	}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////


////////////////////// continuous glucose monitoring Profile ////////////////////////////////
//======================== 1.continuous glucose monitoring ==========================
//line:267 ~ 321.above.
//====================== Bond Management Service (Optional) =========================
//========================== 2.device information ===================================
//line:71 ~ 103.above.
/////////////////////////////////////////////////////////////////////////////////////////////


// TM : to modify
const attribute_t my_Attributes[] = {
#if PXP//PXP
	{16,0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

	//PXP profile:Link loss Service\ Immediate Alert Service\ Tx Power Service
	// 0008 - 000a  Link loss Service
	{3,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&linkLoss_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&alertLevel_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&alertLevel_charUUID), (u8*)(&alertLevel_value), 0},

	// 000b - 000d  Immediate Alert Service
	{3,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&immediateAlert_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&immediateAlertLevel_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&immediateAlertLevel_charUUID), (u8*)(&immediateAlertLevel_value), 0},

	// 000e - 0010  TX power Service
	{3,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&txPower_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&txPowerLevel_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&txPowerLevel_charUUID), (u8*)(&txPowerLevel_value), 0},

#elif CGMS
	{25,0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

	// 0008 - 0019 continuous glucose monitoring Service
	{18,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&continuousGlucoseMonitoring_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_measurement_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (cgm_measurement_packet_val),(u8*)(&cgm_measurementUUID), (u8*)(&cgm_measurement_packet_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&cgm_measurement_clientCharCfg), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_feature_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (cgm_feature_packet_val),(u8*)(&cgm_featureUUID), (u8*)(&cgm_feature_packet_val), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_status_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (cgm_status_packet_val),(u8*)(&cgm_statusUUID), (u8*)(&cgm_status_packet_val), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_session_start_time_prop), 0},
	{0,ATT_PERMISSIONS_RDWR,2,sizeof (cgm_session_start_time_packet_val),(u8*)(&cgm_session_start_timeUUID), (u8*)(&cgm_session_start_time_packet_val), &cgm_session_start_time_write_callback},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_session_run_time_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (cgm_session_run_time_packet_val),(u8*)(&cgm_session_run_timeUUID), (u8*)(&cgm_session_run_time_packet_val), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&record_access_control_point_prop), 0},
	{0,ATT_PERMISSIONS_RDWR,2,sizeof (record_access_control_point_packet_val),(u8*)(&record_access_control_pointUUID), (u8*)(&record_access_control_point_packet_val), &record_access_control_point_write_callback},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&record_access_control_point_clientCharCfg), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_specific_ops_control_point_prop), 0},
	{0,ATT_PERMISSIONS_RDWR,2,sizeof (cgm_specific_ops_control_point_packet_val),(u8*)(&cgm_specific_ops_control_pointUUID), (u8*)(&cgm_specific_ops_control_point_packet_val), &cgm_specific_ops_control_point_write_callback},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&cgm_specific_ops_control_point_clientCharCfg), 0},


#elif CGMP
	{44,0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

	// 0008 - 0019 continuous glucose monitoring Service
	{18,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&continuousGlucoseMonitoring_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_measurement_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (cgm_measurement_packet_val),(u8*)(&cgm_measurementUUID), (u8*)(&cgm_measurement_packet_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&cgm_measurement_clientCharCfg), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_feature_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (cgm_feature_packet_val),(u8*)(&cgm_featureUUID), (u8*)(&cgm_feature_packet_val), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_status_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (cgm_status_packet_val),(u8*)(&cgm_statusUUID), (u8*)(&cgm_status_packet_val), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_session_start_time_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (cgm_session_start_time_packet_val),(u8*)(&cgm_session_start_timeUUID), (u8*)(&cgm_session_start_time_packet_val), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_session_run_time_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (cgm_session_run_time_packet_val),(u8*)(&cgm_session_run_timeUUID), (u8*)(&cgm_session_run_time_packet_val), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&record_access_control_point_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (record_access_control_point_packet_val),(u8*)(&record_access_control_pointUUID), (u8*)(&record_access_control_point_packet_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&record_access_control_point_clientCharCfg), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&cgm_specific_ops_control_point_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (cgm_specific_ops_control_point_packet_val),(u8*)(&cgm_specific_ops_control_pointUUID), (u8*)(&cgm_specific_ops_control_point_packet_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&cgm_specific_ops_control_point_clientCharCfg), 0},

	// 0020 - 002C  device Information Service
	{19,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&devInfo_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&systemID_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (systemID_value),(u8*)(&systemID_charUUID), (u8*)(systemID_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&modleNameString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (modelNameString_value),(u8*)(&modleNameString_charUUID), (u8*)(modelNameString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&serialString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (serialString_value),(u8*)(&serialString_charUUID), (u8*)(serialString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&fwRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (fwRevision_value),(u8*)(&fwRevision_charUUID), (u8*)(fwRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&hwRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (hwRevision_value),(u8*)(&hwRevision_charUUID), (u8*)(hwRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&swRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (swRevision_value),(u8*)(&swRevision_charUUID), (u8*)(swRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&manuNameString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (manuNameString_value),(u8*)(&manuNameString_charUUID), (u8*)(manuNameString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&ieee11703_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (ieee11703_value),(u8*)(&ieee11703_charUUID), (u8*)(ieee11703_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&pnpID_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (pnpID_value),(u8*)(&pnpID_charUUID), (u8*)(pnpID_value), 0},

#elif BLS
	{16,0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

	// 0008 - 0010 blood pressure Service
	{9,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&bloodPressure_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&bloodPressureMeasurement_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (blood_pressure_measure_val),(u8*)(&bloodPressure_measureUUID), (u8*)(&blood_pressure_measure_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&bloodPressureMeasurement_clientCharCfg), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&intermediate_cuff_pressure_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (intermediate_cuff_pressure_val),(u8*)(&intermediate_cuff_pressureUUID), (u8*)(&intermediate_cuff_pressure_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&intermediate_cuff_pressure_clientCharCfg), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&blood_pressure_feature_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (blood_pressure_feature_val),(u8*)(&blood_pressure_featureUUID), (u8*)(&blood_pressure_feature_val), 0},
#elif BLP
	{32,0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

	// 0008 - 0010 blood pressure Service
	{9,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&bloodPressure_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&bloodPressureMeasurement_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (blood_pressure_measure_val),(u8*)(&bloodPressure_measureUUID), (u8*)(&blood_pressure_measure_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&bloodPressureMeasurement_clientCharCfg), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&intermediate_cuff_pressure_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (intermediate_cuff_pressure_val),(u8*)(&intermediate_cuff_pressureUUID), (u8*)(&intermediate_cuff_pressure_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&intermediate_cuff_pressure_clientCharCfg), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&blood_pressure_feature_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (blood_pressure_feature_val),(u8*)(&blood_pressure_featureUUID), (u8*)(&blood_pressure_feature_val), 0},

	// 000e - 0020  device Information Service
	{19,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&devInfo_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&systemID_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (systemID_value),(u8*)(&systemID_charUUID), (u8*)(systemID_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&modleNameString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (modelNameString_value),(u8*)(&modleNameString_charUUID), (u8*)(modelNameString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&serialString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (serialString_value),(u8*)(&serialString_charUUID), (u8*)(serialString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&fwRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (fwRevision_value),(u8*)(&fwRevision_charUUID), (u8*)(fwRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&hwRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (hwRevision_value),(u8*)(&hwRevision_charUUID), (u8*)(hwRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&swRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (swRevision_value),(u8*)(&swRevision_charUUID), (u8*)(swRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&manuNameString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (manuNameString_value),(u8*)(&manuNameString_charUUID), (u8*)(manuNameString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&ieee11703_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (ieee11703_value),(u8*)(&ieee11703_charUUID), (u8*)(ieee11703_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&pnpID_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (pnpID_value),(u8*)(&pnpID_charUUID), (u8*)(pnpID_value), 0},
#elif HRS
	{14,0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

	// 0008 - 000f  heart rate Service
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&heartRate_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&heart_rate_measurement_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (heart_rate_measure_val),(u8*)(&heart_rate_measurementUUID), (u8*)(&heart_rate_measure_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&heart_rate_measurement_clientCharCfg), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&body_sensor_location_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (body_sensor_location_val),(u8*)(&body_sensor_locationUUID), (u8*)(&body_sensor_location_val), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&heart_rate_control_point_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (heart_rate_control_point_val),(u8*)(&heart_rate_control_pointUUID), (u8*)(&heart_rate_control_point_val), 0},


#elif HRP
	{33,0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

	// 0008 - 000f  heart rate Service
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&heartRate_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&heart_rate_measurement_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (heart_rate_measure_val),(u8*)(&heart_rate_measurementUUID), (u8*)(&heart_rate_measure_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&heart_rate_measurement_clientCharCfg), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&body_sensor_location_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (body_sensor_location_val),(u8*)(&body_sensor_locationUUID), (u8*)(&body_sensor_location_val), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&heart_rate_control_point_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (heart_rate_control_point_val),(u8*)(&heart_rate_control_pointUUID), (u8*)(&heart_rate_control_point_val), 0},

	// 0010 - 0021  device Information Service
	{19,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&devInfo_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&systemID_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (systemID_value),(u8*)(&systemID_charUUID), (u8*)(systemID_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&modleNameString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (modelNameString_value),(u8*)(&modleNameString_charUUID), (u8*)(modelNameString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&serialString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (serialString_value),(u8*)(&serialString_charUUID), (u8*)(serialString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&fwRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (fwRevision_value),(u8*)(&fwRevision_charUUID), (u8*)(fwRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&hwRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (hwRevision_value),(u8*)(&hwRevision_charUUID), (u8*)(hwRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&swRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (swRevision_value),(u8*)(&swRevision_charUUID), (u8*)(swRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&manuNameString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (manuNameString_value),(u8*)(&manuNameString_charUUID), (u8*)(manuNameString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&ieee11703_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (ieee11703_value),(u8*)(&ieee11703_charUUID), (u8*)(ieee11703_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&pnpID_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (pnpID_value),(u8*)(&pnpID_charUUID), (u8*)(pnpID_value), 0},
#elif WSS
	{13,0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

	// 0008 - 000d Weight Scale Service
	{6,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&weightScale_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&weightScale_measure_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (weightScale_measure_val),(u8*)(&weightScale_measureUUID), (u8*)(&weightScale_measure_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&weightScale_measureCCC), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&weightScale_feature_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (weightScale_feature_val),(u8*)(&weightScale_featureUUID), (u8*)(&weightScale_feature_val), 0},

#elif WSP
	{32,0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

	// 0008 - 000d Weight Scale Service
	{6,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&weightScale_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&weightScale_measure_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (weightScale_measure_val),(u8*)(&weightScale_measureUUID), (u8*)(&weightScale_measure_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&weightScale_measureCCC), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&weightScale_feature_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (weightScale_feature_val),(u8*)(&weightScale_featureUUID), (u8*)(&weightScale_feature_val), 0},

	// 000e - 0020  device Information Service
	{19,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&devInfo_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&systemID_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (systemID_value),(u8*)(&systemID_charUUID), (u8*)(systemID_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&modleNameString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (modelNameString_value),(u8*)(&modleNameString_charUUID), (u8*)(modelNameString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&serialString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (serialString_value),(u8*)(&serialString_charUUID), (u8*)(serialString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&fwRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (fwRevision_value),(u8*)(&fwRevision_charUUID), (u8*)(fwRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&hwRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (hwRevision_value),(u8*)(&hwRevision_charUUID), (u8*)(hwRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&swRevision_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (swRevision_value),(u8*)(&swRevision_charUUID), (u8*)(swRevision_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&manuNameString_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (manuNameString_value),(u8*)(&manuNameString_charUUID), (u8*)(manuNameString_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&ieee11703_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (ieee11703_value),(u8*)(&ieee11703_charUUID), (u8*)(ieee11703_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&pnpID_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (pnpID_value),(u8*)(&pnpID_charUUID), (u8*)(pnpID_value), 0},


#elif BAS_DIS_SCPS_TPS
	{40,0,0,0,0,0},	// total num of attribute
	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},


	// 0008 - 000b  Battery Service
	{5,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_batServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_batProp), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_batVal),(u8*)(&my_batCharUUID), (u8*)(my_batVal), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof(batty_level_presentFmt),(u8*)(&my_batPresentationFormatUUID),(u8*)(batty_level_presentFmt), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&batty_level_clientCharCfg), 0},


	// 000c - 001f  device Information Service
	{19,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&devInfo_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&systemID_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (systemID_value),(u8*)(&systemID_charUUID), (u8*)(systemID_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&modleNameString_prop), 0},
    {0,ATT_PERMISSIONS_READ,2,sizeof (modelNameString_value),(u8*)(&modleNameString_charUUID), (u8*)(modelNameString_value), 0},
    {0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&serialString_prop), 0},
    {0,ATT_PERMISSIONS_READ,2,sizeof (serialString_value),(u8*)(&serialString_charUUID), (u8*)(serialString_value), 0},
    {0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&fwRevision_prop), 0},
    {0,ATT_PERMISSIONS_READ,2,sizeof (fwRevision_value),(u8*)(&fwRevision_charUUID), (u8*)(fwRevision_value), 0},
    {0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&hwRevision_prop), 0},
    {0,ATT_PERMISSIONS_READ,2,sizeof (hwRevision_value),(u8*)(&hwRevision_charUUID), (u8*)(hwRevision_value), 0},
    {0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&swRevision_prop), 0},
    {0,ATT_PERMISSIONS_READ,2,sizeof (swRevision_value),(u8*)(&swRevision_charUUID), (u8*)(swRevision_value), 0},
    {0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&manuNameString_prop), 0},
    {0,ATT_PERMISSIONS_READ,2,sizeof (manuNameString_value),(u8*)(&manuNameString_charUUID), (u8*)(manuNameString_value), 0},
    {0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&ieee11703_prop), 0},
    {0,ATT_PERMISSIONS_READ,2,sizeof (ieee11703_value),(u8*)(&ieee11703_charUUID), (u8*)(ieee11703_value), 0},
    {0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&pnpID_prop), 0},
    {0,ATT_PERMISSIONS_READ,2,sizeof (pnpID_value),(u8*)(&pnpID_charUUID), (u8*)(pnpID_value), 0},

    // 0020 - 0025  scan parameters Service
    {6,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&scanPara_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&scanIntervalWin_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (scanIntervalWin_value),(u8*)(&scanIntervalWin_charUUID), (u8*)(&scanIntervalWin_value), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&scanRefresh_prop), 0},
    {0,ATT_PERMISSIONS_READ,2,1,(u8*)(&scanRefresh_charUUID), (u8*)(&scanRefresh_value), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&scanRefresh_clientCharCfg), 0},

	// 0026 - 0025  TX power Service
    {3,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&txPower_serviceUUID), 0},
    {0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&txPowerLevel_prop), 0},
    {0,ATT_PERMISSIONS_READ,2,1,(u8*)(&txPowerLevel_charUUID), (u8*)(&txPowerLevel_value), 0},
#endif

};

void	my_att_init ()
{
	bls_att_setAttributeTable ((u8 *)my_Attributes);

	u8 device_name[] = DEV_NAME;
	bls_att_setDeviceName(device_name, sizeof(DEV_NAME));
}

#endif
