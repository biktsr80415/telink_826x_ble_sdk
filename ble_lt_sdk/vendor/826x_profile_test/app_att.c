#if(__PROJECT_826X_PROFILE_TEST__)

#include "../../proj/tl_common.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"


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

const u16 clientCharacterCfgUUID = GATT_UUID_CLIENT_CHAR_CFG;

const u16 extReportRefUUID = GATT_UUID_EXT_REPORT_REF;

const u16 reportRefUUID = GATT_UUID_REPORT_REF;

const u16 characterPresentFormatUUID = GATT_UUID_CHAR_PRESENT_FORMAT;

const u16 my_primaryServiceUUID = GATT_UUID_PRIMARY_SERVICE;

static const u16 my_characterUUID = GATT_UUID_CHARACTER;

const u16 my_devServiceUUID = SERVICE_UUID_DEVICE_INFORMATION;

const u16 my_PnPUUID = CHARACTERISTIC_UUID_PNP_ID;

const u16 my_devNameUUID = GATT_UUID_DEVICE_NAME;

//device information
const u16 my_gapServiceUUID = SERVICE_UUID_GENERIC_ACCESS;
// Device Name Characteristic Properties
static u8 my_devNameCharacter = CHAR_PROP_READ | CHAR_PROP_NOTIFY;
// Appearance Characteristic Properties
const u16 my_appearanceUIID = 0x2a01;
const u16 my_periConnParamUUID = 0x2a04;
static u8 my_appearanceCharacter = CHAR_PROP_READ;
// Peripheral Preferred Connection Parameters Characteristic Properties
static u8 my_periConnParamChar = CHAR_PROP_READ;
u16 my_appearance = GAP_APPEARE_UNKNOWN;
gap_periConnectParams_t my_periConnParameters = {20, 40, 0, 1000};


#define DEV_NAME                        "tyfModule"
extern u8  ble_devName[];

//////////////////////// Battery /////////////////////////////////////////////////
const u16 my_batServiceUUID       			= SERVICE_UUID_BATTERY;
static u8 my_batProp 						= CHAR_PROP_READ | CHAR_PROP_NOTIFY;
const u16 my_batCharUUID       				= CHARACTERISTIC_UUID_BATTERY_LEVEL;
const u16 my_batPresentationFormatUUID      = GATT_UUID_CHAR_PRESENT_FORMAT;
u8 		  my_batVal[1] 						= {99};
u8 		  my_batVal1[1] 						= {99};
// Battery Level Presentation format
/*Format 1byte, Exponent 1, Unit 2, Name Space 1, Description 2*/
u8 batty_level_presentFmt[7]={4,0,0xA7,0x2D,1,0,1};
u8 batty_level_presentFmt1[7] ={2};
// Battery Level Client Characteristic Configuration
u16 batty_level_clientCharCfg;
u16 batty_level_clientCharCfg1;
/////////////////////////////////////////////////////////
const u16 userdesc_UUID		= GATT_UUID_CHAR_USER_DESC;


//////////////////////// device ///////////////////////////////////////////////////
const u16 devInfo_serviceUUID  = SERVICE_UUID_DEVICE_INFORMATION;

// Characterstics UUID
u16 manuNameString_charUUID  = CHARACTERISTIC_UUID_MANU_NAME_STRING;
u16 modleNameString_charUUID = CHARACTERISTIC_UUID_MODEL_NUM_STRING;
u16 serialString_charUUID    = CHARACTERISTIC_UUID_SERIAL_NUM_STRING;
u16 hwRevision_charUUID      = CHARACTERISTIC_UUID_HW_REVISION_STRING;
u16 fwRevision_charUUID      = CHARACTERISTIC_UUID_FW_REVISION_STRING;
u16 swRevision_charUUID      = CHARACTERISTIC_UUID_SW_REVISION_STRING;
u16 systemID_charUUID        = CHARACTERISTIC_UUID_SYSTEM_ID;
u16 ieee11703_charUUID       = CHARACTERISTIC_UUID_IEEE_11073_CERT_LIST;
u16 pnpID_charUUID           = CHARACTERISTIC_UUID_PNP_ID;


// Characterstics Property
u8 manuNameString_prop  = CHAR_PROP_READ;
u8 modleNameString_prop = CHAR_PROP_READ;
u8 serialString_prop    = CHAR_PROP_READ;
u8 hwRevision_prop      = CHAR_PROP_READ;
u8 fwRevision_prop      = CHAR_PROP_READ;
u8 swRevision_prop      = CHAR_PROP_READ;
u8 systemID_prop        = CHAR_PROP_READ;
u8 ieee11703_prop       = CHAR_PROP_READ;
u8 pnpID_prop           = CHAR_PROP_READ;

// Characterstics Value
const u8 manuNameString_value[] = "TELINK Tech.";
const u8 modelNameString_value[] = "FH005";
const u8 serialString_value[] = "serial";
const u8 hwRevision_value[] = "V1.1";
const u8 fwRevision_value[] = "V1.6";
const u8 swRevision_value[] = "V1.6";
const u8 systemID_value[SYSTEM_ID_LEN] = {0};
const u8 ieee11703_value[DEVINFO_PNP_ID_LEN] = {0};
const u8 pnpID_value[DEVINFO_PNP_ID_LEN] = {1, 0x0d, 0x00, 0x00, 0x00, 0x10, 0x01};


///////////////////// Scan parameter /////////////////////////////////////////
#define CHARACTERISTIC_UUID_SCAN_INTERVAL_WINDOW                0x2A4F
#define CHARACTERISTIC_UUID_SCAN_REFRESH                        0x2A31

const u16 scanPara_serviceUUID  = SERVICE_UUID_SCAN_PARAMETER;

// Scan Parameters Characteristic Property
u8 scanIntervalWin_prop = CHAR_PROP_WRITE_WITHOUT_RSP;
u8 scanRefresh_prop     = CHAR_PROP_NOTIFY;

typedef struct {
	u16 scanInterval;
	u16 scanWindow;
} scan_intervalWindow_t;

// Scan Parameters Characteristic Values
scan_intervalWindow_t scanIntervalWin_value;
u8 scanRefresh_value = FALSE;

// Scan Parameters Characteristic UUID
u16 scanIntervalWin_charUUID = CHARACTERISTIC_UUID_SCAN_INTERVAL_WINDOW;
u16 scanRefresh_charUUID     = CHARACTERISTIC_UUID_SCAN_REFRESH;

// Scan Refresh Client Characteristic Configuration
u16 scanRefresh_clientCharCfg;

////////////////////// Tx power /////////////////////////////////////////////////
// Txpower_uuid Txpower Charactersitc UUID
#define CHARACTERISTIC_UUID_TX_POWER_LEVEL                    0x2A07

//brief  Definition for TX Power Level Range
#define TX_POWER_MIN                                          -100
#define TX_POWER_MAX                                          20

const u16 txPower_serviceUUID  = SERVICE_UUID_TX_POWER;
// TX Power Level Property
u8 txPowerLevel_prop = CHAR_PROP_READ;
// TX Power Level Value in Percentage
s8 txPowerLevel_value = TX_POWER_MAX;
// TX Power Level UUID
u16 txPowerLevel_charUUID = CHARACTERISTIC_UUID_TX_POWER_LEVEL;

////////////////////// PXP profile /////////////////////////////////////////////////////////////////////
////////////////////// 1.link loss //////////////////////////////////////////
#define CHARACTERISTIC_UUID_ALERT_LEVEL                    0x2A06
//Definiton for Alert Level
#define ALERT_LEVEL_NO_ALERT                               0
#define ALERT_LEVEL_MILD_ALERT                             1
#define ALERT_LEVEL_HIGH_ALERT                             2

const u16 linkLoss_serviceUUID  = SERVICE_UUID_LINK_LOSS;

// Alert Level Property
u8 alertLevel_prop = CHAR_PROP_READ | CHAR_PROP_WRITE;

// Alert Level Value in Percentage
u8 alertLevel_value = ALERT_LEVEL_HIGH_ALERT;

// Alert Level UUID
u16 alertLevel_charUUID = CHARACTERISTIC_UUID_ALERT_LEVEL;

////////////////////// 2.Immediate Alert ////////////////////////////////////
const u16 immediateAlert_serviceUUID  = SERVICE_UUID_IMMEDIATE_ALERT;

// Alert Level Property
#if 1//_CERTIFICATION_TEST_MODE_
u8 immediateAlertLevel_prop = CHAR_PROP_WRITE_WITHOUT_RSP;
#else
u8 immediateAlertLevel_prop = CHAR_PROP_WRITE_WITHOUT_RSP | CHAR_PROP_NOTIFY;
#endif

// Alert Level Value in Percentage
u8 immediateAlertLevel_value = ALERT_LEVEL_NO_ALERT;

// Alert Level UUID
u16 immediateAlertLevel_charUUID = CHARACTERISTIC_UUID_ALERT_LEVEL;

//////////////////////3.Tx power ////////////////////////////////////////////
//
//////////////////////// PXP profile ///////////////////////////////////////////////////////////////////


////////////////////// weight scale service////////////////////////////////////////////////////////////////////
#define CHARACTERISTIC_UUID_WEIGHT_SCALE_FEATURE                    0x2A9E
#define CHARACTERISTIC_UUID_WEIGHT_MEASUREMENT						0x2A9D
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
//weight scale feature configure strut, used to configure weight scale supported features
typedef struct {
	u8 timeStampSupported;
	u8 multiUserSupported;
	u8 bmiSupported;
	u8 weightRes;
	u8 heightRes;
} ws_feature_configure;
//weight resolution, unit in kg and lb
enum WEIGHT_RESOLUTION {
	WEIGHT_RES_NOT_SPECIFIED,
	WEIGHT_RES_P5_OR_1LB,
	WEIGHT_RES_P2_OR_P5LB,
	WEIGHT_RES_P1_OR_P2LB,
	WEIGHT_RES_P05_OR_P1LB,
	WEIGHT_RES_P02_OR_P05LB,
	WEIGHT_RES_P01_OR_P02LB,
	WEIGHT_RES_P005_OR_P01LB,
};
//height resolution, unit in meter and inch
enum HEIGHT_RESOLUTION {
	HEIGHT_RES_NOT_SPECIFIED,
	HEIGHT_RES_P01_OR_1INCH,
	HEIGHT_RES_P005_OR_P5INCH,
	HEIGHT_RES_P001_OR_P1INCH,
};
// flag used by function weightScale_valueSet, to set measurement values
enum SET_MEASUREMENT_VALUE {
	SET_WEIGHT,
	SET_TIMESTAMP,
	SET_USERID,
	SET_BMI,
	SET_HEIGHT,
};
//used to indicate which items need to be sent to the collector
enum SEND_ITEMS {
	SEND_WEIGHT_KG = 0,
	SEND_WEIGHT_LB = BIT(0),
	SEND_TIME = BIT(1),
	SEND_USERID = BIT(2),
	SEND_BMI = BIT(3),
};
const u16 weightScale_serviceUUID  = SERVICE_UUID_WEIGHT_SCALE;
const u16 weightScale_featureUUID = CHARACTERISTIC_UUID_WEIGHT_SCALE_FEATURE;
// weight scale feature Property
u8 weightScale_feature_prop = CHAR_PROP_READ;
//weight scale feature value
u32 weightScale_feature_val = 0x00000018;//supported bmi, weight resolution 0.1kg, height not specified
const u16 weightScale_measureUUID = CHARACTERISTIC_UUID_WEIGHT_MEASUREMENT;
// weight measure feature Property
u8 weightScale_measure_prop = CHAR_PROP_INDICATE;
//weight measure value, needs modify according requirements
//u8 weightScale_measure_val[19] = {0};
weight_measure_packet weightScale_measure_val;
u16 weightScale_measureCCC = 0x0000;
////////////////////// weight scale service/////////////////////////////////////////////////////////////////////

void module_onReceiveData(rf_packet_att_write_t *p)
{

}

#define PXP  0
#define WSS  1
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

	{6,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&weightScale_serviceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&weightScale_measure_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (weightScale_measure_val),(u8*)(&weightScale_measureUUID), (u8*)(&weightScale_measure_val), 0},
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&weightScale_measureCCC), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&weightScale_feature_prop), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (weightScale_feature_val),(u8*)(&weightScale_featureUUID), (u8*)(&weightScale_feature_val), 0},


#else
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
