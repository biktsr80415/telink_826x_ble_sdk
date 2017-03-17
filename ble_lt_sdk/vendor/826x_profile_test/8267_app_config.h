#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif
#if __PROJECT_826X_PROFILE_TEST__


#define CHIP_TYPE				CHIP_TYPE_8267


/////////////////// MODULE /////////////////////////////////
#define PM_ENABLE				            0
#define TELIK_SPP_SERVICE_ENABLE			1
#define BLE_MODULE_APPLICATION_ENABLE		1

/////////////////////for test by tuyf 16-9-01//////////////////////////////////
#define RED_LED                 GPIO_PD5
#define GREEN_LED     			GPIO_PD6
#define WHITE_LED    		    GPIO_PD7
#define BLUE_LED                GPIO_PB4

#define ON            1
#define OFF           0

//key
#define KEY1                    GPIO_PD2
#define KEY2                    GPIO_PC7

#define PD2_FUNC 				AS_GPIO
#define PD2_INPUT_ENABLE		1
#define PULL_WAKEUP_SRC_PD2		PM_PIN_PULLUP_1M

#define PC7_FUNC 				AS_GPIO
#define PC7_INPUT_ENABLE		1
#define PULL_WAKEUP_SRC_PC7		PM_PIN_PULLUP_1M

////////////////////////////////////////////////////////

/////////////////// PRINT DEBUG INFO ///////////////////////
/* 826x module's pin simulate as a uart tx, Just for debugging */
#define PRINT_DEBUG_INFO                    1//open/close myprintf
#if PRINT_DEBUG_INFO
//defination debug printf pin
#define PRINT_BAUD_RATE             		1000000 //1M baud rate,should Not bigger than 1M
#define DEBUG_INFO_TX_PIN           		GPIO_PC6//G0 for 8261/8267 EVK board(C1T80A30_V1.0)
//#define PC6_OUTPUT_ENABLE	        		1       //mini_printf function contain this
#define PULL_WAKEUP_SRC_PC6         		PM_PIN_PULLUP_1M
#endif


//////////////////////////////// service ATT table test /////////////////////////////////////
#define BAS_DIS_SCPS_TPS 					    0//BS_DIS_SCPS_TPS
#define PXP            							0//PXP profile
#define BLS            							0//BLS
#define BLP            							0//BLP
#define HRS            							0//HRS
#define HRP            							0//HRP
#define WSS            							0//WSS
#define WSP            							0//WSP
#define CGMS            						1//CGMS
#define CGMP            						0//CGMP

#if CGMS || CGMP //======================================================================================================================================

#define CGMS_SEN_RAA_BV_01_C                    0//濞村鐦ù瀣櫤濡剝瀚欓弫鐗堝祦閺堬拷00閺夆槄绱�CGMS/SEN/RAE/BI-02-C濞村鐦悽銊ょ伐娑旂喕顪呴幍鎾崇磻 娴犮儲膩閹风喕绻栨稊鍫濐檵閺佺増宓�#define CGMS_SEN_CN_BV_02_C                     0
#define CGMS_SEN_CN_BV_02_C                     0//CN两项开这个宏
#define CGMS_SEN_CGMCP_BV_03_C                  0//鐠囥儲绁寸拠鏇�閸栧懎鎯圕GMS/SEN/CGMCP/BV-01-C [CGM Specific Ops 閳ワ拷閳ユオet CGM Communication Interval with E2E-CRC閳ユ┚
#define CGMS_SEN_CBE_BI_06_C                    0//General Error Handling 閳ワ拷閳ユコissing CRC閳ユ┚.閸楃煼UT閺�垱瀵擡2E CRC

#if CGMS_SEN_CN_BV_02_C
#define SENSOR_STATUS_FLG_ENABLE                1
#endif

#if CGMS_SEN_CGMCP_BV_03_C || CGMS_SEN_CBE_BI_06_C
#define E2E_CRC_FLAG_ENABLE                     1
#endif

#ifndef E2E_CRC_FLAG_ENABLE
#define E2E_CRC_FLAG_ENABLE                     0
#endif

#ifndef SENSOR_STATUS_FLG_ENABLE
#define SENSOR_STATUS_FLG_ENABLE                0
#endif

#endif//=================================================================================================================================================

#include "../../proj/common/types.h"
//time info packet structure, should transfer to time service later
typedef struct {
	u16 year;
	u8 month;
	u8 day;
	u8 hours;
	u8 minutes;
	u8 seconds;
} time_packet;

//weight measurement data structure
typedef struct {
	u8 wmFlag;
	u16 wmWeight;
	time_packet timeInf;
	u8 userID;
	u16 wmBMI;
	u16 wmHeight;
} weight_measure_packet;

//Blood Pressure Measurement data structure
typedef struct {
	u8 bpmFlag;
	short bpmSys;        //Systolic
	short bpmSysDiastoli;//Diastoli
	short bpmMAR;        //Mean Arterial Pressure
	time_packet timeInf; //weight scale defined
	short pulseRate;     //pulse Rate
	u8 userID;
	u16 measurementStatus;
} blood_pressure_measure_packet;

//heart rate measurement data structure
typedef struct {
	u8 hrmFlag;
	u16 hrVal;        //Heart Rate Measurement Value
	u16 eryexd;//Energy Expended
	u16 rr_interval; //RR-Interval
} heart_rate_measurement_packet;

//cgm_measurement data structure
typedef struct {
	u8 size;
	u8 cgmMflg;
	short cgmGlucoseConcentration;//SFLOAT
	u16 timeOffset;
#if SENSOR_STATUS_FLG_ENABLE
	u8 sensorStatusAnnunciation[3];//	Optional if bit 5 or bit 6 or bit 7 of the cgmMflg field is set to 閳ワ拷閳ワ拷 otherwise excluded.
#endif
	short cgmTrendInformation;//This field is optional if the device supports CGM Trend information (Bit 15 in CGM Feature is set to 1) otherwise excluded.
	short cgmQuality;//This field is optional if the device supports CGM Quality (Bit 16 in CGM Feature is set to 1) otherwise excluded.
#if E2E_CRC_FLAG_ENABLE
	u16 e2eCRC; // This field is mandatory,if the device supports E2E-CRC (Bit 12 in CGM Feature is set to 1) otherwise excluded.
#endif
} cgm_measurement_packet;

//CGM Status data structure
typedef struct {
	u8 cgmFeature[3];
	u8 cgmTypeSample;//bit[0:3]:cgmType -- bit[4:7]:cgmSample
	           //if the device supports E2E-safety (E2E-CRC Supported bit is set in CGM Feature), the feature are secured by a CRC calculated over all data.
	u16 e2eCRC;// If the device doesn楹搕 support E2E-safety the value of the field shall be set to 0xFFFF.
} cgm_feature_packet;

//CGM Status data structure
typedef struct {
	u16 timeOffset;
	u8 cgmStatus[3];
#if E2E_CRC_FLAG_ENABLE
	u16 e2eCRC;//Mandatory if device supports E2E-CRC (Bit 12 is set in CGM Feature) otherwise excluded.
#endif
} cgm_status_packet;

//CGM Session Start Time data struct
typedef struct {
	time_packet sessionStartTime;
	s8 timeZone;
	u8 dstOffset;
#if E2E_CRC_FLAG_ENABLE
	u16 e2eCRC;//Mandatory if device supports E2E-CRC (Bit 12 is set in CGM Feature) otherwise excluded.
#endif
} cgm_session_start_time_packet;

//CGM Session Run Time data struct
typedef struct {
	u16 cgmSessionRunTime;
#if E2E_CRC_FLAG_ENABLE
	u16 e2eCRC;//Mandatory if device supports E2E-CRC (Bit 12 is set in CGM Feature) otherwise excluded.
#endif
} cgm_session_run_time_packet;

/////////////////////////////////////// Record Access Control Point /////////////////////////////////////////////
typedef struct {
	u8 opCode;
	u8 operator;
	u16 operand;
} record_access_control_point_packet;

enum record_access_control_point_opCode{
	Report_stored_records = 1,         // (Operator: Value from Operator Table)
	Delete_stored_records,             // (Operator: Value from Operator Table)
	Abort_operation =3,                // (Operator: Null 'value of 0x00 from Operator Table')
	Report_number_of_stored_records,   // (Operator: Value from Operator Table)
	Number_of_stored_records_response = 5, // (Operator: Null 'value of 0x00 from Operator Table')
	Response_Code,                     // (Operator: Null 'value of 0x00 from Operator Table')
};

enum record_access_control_point_operator{
	All_records = 1,
	Less_than_or_equal_to,
	Greater_than_or_equal_to =3,
	Within_range_of,    // (inclusive)
	First_record = 5,   //(i.e. oldest record)
	Last_record,        //(i.e. most recent record)
};

enum record_access_control_point_operand{
	Filter_parameters1 = 1,      // (as appropriate to Operator and Service)
	Filter_parameters2,          // (as appropriate to Operator and Service)
	Not_included = 3,
	Filter_parameters3,          // (as appropriate to Operator and Service)
	Number_of_Records = 5,      // (Field size defined per service)
	Request_Op_Code,            // Response Code Value
};

enum record_access_control_point_Response_Code_Value{
	Success = 1,                // Normal response for successful operation
	OpCode_not_supported,       // Normal response if unsupported Op Code is received
	Invalid_Operator = 3,       // Normal response if Operator received does not meet the requirements of the service (e.g. Null was expected)
	Operator_not_supported,     // Normal response if unsupported Operator is received
	Invalid_Operand = 5,        // Normal response if Operand received does not meet the requirements of the service
	No_records_found,           // Normal response if request to report stored records or request to delete stored records resulted in no records meeting criteria.
	Abort_unsuccessful = 7,     // Normal response if request for Abort cannot be completed
	Procedure_not_completed,    // Normal response if unable to complete a procedure for any reason
	Operand_not_supported = 9,  // Normal response if unsupported Operand is received
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////// CGM Specific Ops Control Point ////////////////////////////////////////////
//CGM Specific Ops Control
typedef struct {
	u8 opCode;
	u8 opCodeResponseCodes;
	u8 operand;
#if E2E_CRC_FLAG_ENABLE
	u16 e2eCRC;//Mandatory if device supports E2E-CRC (Bit 12 is set in CGM Feature) otherwise excluded.
#endif
	short  glucoseConcentrationOfCalibration;//Calibration Value - Glucose Concentration of Calibration
	u16 calibrationTime;
	u8 calibrationTypeSampleLocation;//bit[0:3]:Calibration Type -- bit[4:7]:Calibration Sample Location
	u16 nextCalibrationTime;
	u16 calibrationDataRecordNumber;
	u8  calibrationStatus;
} cgm_specific_ops_control_point_packet;

//actually
//Actually CGM Specific Ops Control
typedef struct {
	u8 opCode;
	u8 operand;
#if E2E_CRC_FLAG_ENABLE
	u16 e2eCRC;//Mandatory if dev
#endif
}actually_cgm_specific_ops_control_point_packet;

enum cgm_specific_ops_control_point_opCode{
	Set_CGM_Communication_Interva = 1,//The response to this control point is Response Code (Op Code 0x0F)
	Get_CGM_Communication_Interva = 2,//The normal response to this control point is Op Code 0x03. For error conditions, the response is defined in the Op Code - Response Codes field
	CGM_Communication_Interval_response =3,//This is the normal response to Op Code 0x02
	Set_Glucose_Calibration_Value =4,//The response to this control point is defined in Op Code - Response Codes field
	Get_Glucose_Calibration_Value =5,//The normal response to this control point is Op Code 0x06. for error conditions, the response is defined in the Op Code - Response Codes field
	Glucose_Calibration_Value_response =6,//Glucose Calibration Value response
	Set_Patient_High_Alert_Level =7,//The response to this control point is defined in Op Code - Response Codes field
	Get_Patient_High_Alert_Level =8,//The normal response to this control point is Op Code 0x09. For error conditions, the response is defined in the Op Code - Response Codes field
	Patient_High_Alert_Level_Response =9,//	This is the normal response to Op Code 0x08
	Set_Patient_Low_Alert_Level =10,//The response to this control point is defined in Op Code - Response Codes field
	Get_Patient_Low_Alert_Level =11,//The normal response to this control point is Op Code 0x0C. the response is defined in the Op Code - Response Codes field
	Patient_Low_Alert_Level_Response =12,//This is the normal response to Op Code 0x0B
	Set_Hypo_Alert_Level =13,//The response to this control point is defined in Op Code - Response Codes field
	Get_Hypo_Alert_Level =14,//The normal response to this control point is Op Code 0x0F. the response is defined in the Op Code - Response Codes field
	Hypo_Alert_Level_Response =15,//This is the normal response to Op Code 0x0E
	Set_Hyper_Alert_Level =16,//The response to this control point is defined in Op Code - Response Codes field
	Get_Hyper_Alert_Level =17,//The normal response to this control point is Op Code 0x12. The response is defined in the Op Code - Response Codes field
	Hyper_Alert_Level_Response =18,//This is the normal response to Op Code 0x11
	Set_Rate_of_Decrease_Alert_Level =19,//The response to this control point is defined in Op Code - Response Codes field
	Get_Rate_of_Decrease_Alert_Level =20,//The normal response to this control point is Op Code 0x15. For error conditions, the response is Response Code
	Rate_of_Decrease_Alert_Level_Response =21,//This is the normal response to Op Code 0x14
	Set_Rate_of_Increase_Alert_Level =22,//The response to this control point is defined in Op Code - Response Codes field
	Get_Rate_of_Increase_Alert_Level =23,//The normal response to this control point is Op Code 0x18. For error conditions, the response is Response Code
	Rate_of_Increase_Alert_Level_Response =24,//This is the normal response to Op Code 0x17
	Reset_Device_Specific_Alert =25,//The response to this control point is defined in Op Code - Response Codes field
	Start_the_Session =26,//The response to this control point is defined in Op Code - Response Codes field
	Stop_the_Session =27,//The response to this control point is defined in Op Code - Response Codes field
	csocp_Response_Code =28,//see Op Code - Response Codes field
};

enum cgm_specific_ops_control_point_opCode_rsp{
    csocp_Success =1,//Normal response for successful operation.
	Op_Code_not_supported =2,	//Normal response if unsupported Op Code is received.
	csocp_Invalid_Operand	=3,//Normal response if Operand received does not meet the requirements of the service.
	csocp_Procedure_not_completed	=4,//Normal response if unable to complete a procedure for any reason.
	Parameter_out_of_range	=5,//Normal response if Operand received does not meet the range requirements
};

enum cgm_specific_ops_control_point_Operand{
	Communication_Interval_in_minutes =1,
	Communication_Interval_in_minutes1 =3,
	Operand_value =4,//Operand value as defined in the Calibration Value Fields.
	Calibration_Data_Record_Number =5,
	Calibration_Data =6,
	Patient_High_bG_value =7,// in mg/dL
	Patient_High_bG_value1 = 9,// in mg/dL
	Patient_Low_bG_value =10,//in mg/dL
	Patient_Low_bG_value1 =12, //in mg/dL
	Hypo_Alert_Level_value =13,// in mg/dL
	Hypo_Alert_Level_value1 =15,// in mg/dL
	Hypo_Alert_Level_value2 =16,// in mg/dL
	Hypo_Alert_Level_value3 =18,// in mg/dL
	Rate_of_Decrease_Alert_Level =19,// value in mg/dL/min
	Rate_of_Decrease_Alert_Level1 =21,// value in mg/dL/min
	Rate_of_Increase_Alert_Level =22,// value in mg/dL/min
	Rate_of_Increase_Alert_Level1 =24,// value in mg/dL/min
	csocp_Request_Op_Code = 28, //Response Code Value
};

enum cgm_specific_ops_control_point_Calibration_Status{
	Calibration_Data_rejected = 0,
	Calibration_Data_success = 1,
	Calibration_Data_out_of_range = 2,
	Calibration_Process_Pending = 4,
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////// POWER OPTIMIZATION  AT SUSPEND ///////////////////////
//notice that: all setting here aims at power optimization ,they depends on
//the actual hardware design.You should analyze your hardware board and then
//find out the io leakage

//shut down the input enable of some gpios, to lower io leakage at suspend state
//for example:  #define PA2_INPUT_ENABLE   0



/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms



/////////////open SWS digital pullup to prevent MCU err, this is must ////////////
#define PB0_DATA_OUT					1

#endif
/////////////////// set default   ////////////////
#define	PULL_WAKEUP_SRC_PB1		PM_PIN_PULLDOWN_100K
#define	PULL_WAKEUP_SRC_PB2		PM_PIN_PULLDOWN_100K
#define	PULL_WAKEUP_SRC_PB3		PM_PIN_PULLDOWN_100K
#define	PULL_WAKEUP_SRC_PB6		PM_PIN_PULLDOWN_100K
#define PB1_INPUT_ENABLE		1
#define PB2_INPUT_ENABLE		1
#define PB3_INPUT_ENABLE		1
#define PB6_INPUT_ENABLE		1



#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
