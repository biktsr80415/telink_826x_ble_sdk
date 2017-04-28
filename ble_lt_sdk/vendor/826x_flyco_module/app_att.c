#include "../../proj/tl_common.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "spp.h"

#if(1)

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

const u16 clientCharacterCfgUUID     = GATT_UUID_CLIENT_CHAR_CFG;

const u16 extReportRefUUID           = GATT_UUID_EXT_REPORT_REF;

const u16 reportRefUUID              = GATT_UUID_REPORT_REF;

const u16 characterPresentFormatUUID = GATT_UUID_CHAR_PRESENT_FORMAT;

const u16 my_primaryServiceUUID      = GATT_UUID_PRIMARY_SERVICE;

static const u16 my_characterUUID    = GATT_UUID_CHARACTER;

const u16 my_devServiceUUID          = SERVICE_UUID_DEVICE_INFORMATION;

const u16 my_PnPUUID                 = CHARACTERISTIC_UUID_PNP_ID;

const u16 my_devNameUUID             = GATT_UUID_DEVICE_NAME;

//device information
const u16 my_gapServiceUUID          = SERVICE_UUID_GENERIC_ACCESS;
// Device Name Characteristic Properties
static u8 my_devNameCharacter        = CHAR_PROP_READ | CHAR_PROP_NOTIFY;
// Appearance Characteristic Properties
const u16 my_appearanceUIID          = 0x2a01;
const u16 my_periConnParamUUID       = 0x2a04;
static u8 my_appearanceCharacter     = CHAR_PROP_READ;
// Peripheral Preferred Connection Parameters Characteristic Properties
static u8 my_periConnParamChar       = CHAR_PROP_READ;
u16 my_appearance                    = GAP_APPEARE_UNKNOWN;
gap_periConnectParams_t my_periConnParameters = {20, 40, 0, 1000};

extern u8  ble_devName[MAX_DEV_NAME_LEN];

//////////////////////////Ota ///////////////////////////////
const u16 my_OtaServiceUUID = 0x1912;
const u16 my_OtaUUID		= 0x2B12;

const u16 userdesc_UUID		= GATT_UUID_CHAR_USER_DESC;

static u8 my_OtaProp		= CHAR_PROP_READ | CHAR_PROP_NOTIFY | CHAR_PROP_WRITE_WITHOUT_RSP;

u8	 	my_OtaData 		    = 0x00;

//const u8  my_OtaName[]      = {'O', 'T', 'A'};

u8 ota_hdl = 16;//ATT TABLE OTA index:18,Handle value

/////////////////////////////////////////spp/////////////////////////////////////
#if (TELIK_SPP_SERVICE_ENABLE)
//const u16 my_SppNameUUID		              = GATT_UUID_CHAR_USER_DESC;
//const u8  my_SppName[]                        = {'F', 'L', 'Y', 'C', 'O'};
const u16 TelinkSppServiceUUID                = 0xff00;
const u16 TelinkSppDataServer2ClientUUID      = 0xff02;
const u16 TelinkSppDataClient2ServiceUUID     = 0xff01;

// Spp data from Server to Client characteristic variables
static u8 SppDataServer2ClientProp            = CHAR_PROP_READ | CHAR_PROP_NOTIFY;
u8  SppDataServer2ClientData[ATT_MTU_SIZE - 3];
static u8 SppDataServer2ClientDataCCC[2] = {0};

// Spp data from Client to Server characteristic variables
//CHAR_PROP_WRITE: Need response from slave, low transmission speed
static u8 SppDataClient2ServerProp            = CHAR_PROP_READ | CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RSP;
u8  SppDataClient2ServerData[ATT_MTU_SIZE - 3];

//SPP data descriptor
//const u8 TelinkSPPS2CDescriptor[]             = "FLYCO: S2C";
//const u8 TelinkSPPC2SDescriptor[]             = "FLYCO: C2S";

u8 spp_s2c_hdl = 10;//ATT TABLE OTA index:10,Handle value
u8 spp_c2s_hdl = 13;//ATT TABLE OTA index:14,Handle value


//Master write callback function
int module_onReceiveMasterData(rf_packet_att_write_t *p)
{
	u8 len = p->l2capLen - 3;
	if(len > 0)
	{
#if FLYCO_SPP_MODULE
		memset(SppDataClient2ServerData, 0, sizeof(SppDataClient2ServerData));//clear
		memcpy(SppDataClient2ServerData, &p->value, len);
		flyco_uart_push_fifo(len, &p->value);
#endif
	}
	return 0;
}
#endif


// TM : to modify
extern int flyco_otaRead(void * p);
extern int flyco_otaWrite(void * p);
const attribute_t my_Attributes[] = {
	{16,0,0,0,0,0},	// total num of attribute

	// 0001 - 0007  gap
	{7,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_gapServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_devNameCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,MAX_DEV_NAME_LEN, (u8*)(&my_devNameUUID), (u8*)(&ble_devName), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_appearanceCharacter), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_appearance), (u8*)(&my_appearanceUIID), 	(u8*)(&my_appearance), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_periConnParamChar), 0},
	{0,ATT_PERMISSIONS_READ,2,sizeof (my_periConnParameters),(u8*)(&my_periConnParamUUID), 	(u8*)(&my_periConnParameters), 0},

    // 0008 - 000F  spp Service
	{6,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&TelinkSppServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&SppDataServer2ClientProp), 0},//prop
	// serice to client
	{0,ATT_PERMISSIONS_READ,2,sizeof(SppDataServer2ClientData),(u8*)(&TelinkSppDataServer2ClientUUID), (u8*)(SppDataServer2ClientData), 0},	//value
	{0,ATT_PERMISSIONS_RDWR,2,2,(u8*)&clientCharacterCfgUUID,(u8*)(&SppDataServer2ClientDataCCC)},
	//{0,ATT_PERMISSIONS_READ,2,sizeof(TelinkSPPS2CDescriptor),(u8*)&userdesc_UUID,(u8*)(&TelinkSPPS2CDescriptor)},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&SppDataClient2ServerProp), 0},//prop
	//client to service
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(SppDataClient2ServerData),(u8*)(&TelinkSppDataClient2ServiceUUID), (u8*)(SppDataClient2ServerData), &module_onReceiveMasterData},	//value
	//{0,ATT_PERMISSIONS_READ,2,sizeof(TelinkSPPC2SDescriptor),(u8*)&userdesc_UUID,(u8*)(&TelinkSPPC2SDescriptor)},

	// 0010 - 0013  ota Service
	{3,ATT_PERMISSIONS_READ,2,2,(u8*)(&my_primaryServiceUUID), 	(u8*)(&my_OtaServiceUUID), 0},
	{0,ATT_PERMISSIONS_READ,2,1,(u8*)(&my_characterUUID), 		(u8*)(&my_OtaProp), 0},				 //prop
	{0,ATT_PERMISSIONS_RDWR,2,sizeof(my_OtaData),(u8*)(&my_OtaUUID),	(&my_OtaData), &flyco_otaWrite, &flyco_otaRead},//value
	//{0,ATT_PERMISSIONS_RDWR,2,sizeof (my_OtaName),(u8*)(&userdesc_UUID), (u8*)(my_OtaName), 0},

};

void	my_att_init ()
{
	bls_att_setAttributeTable ((u8 *)my_Attributes);

//	u8 device_name[] = DEV_NAME;
//	bls_att_setDeviceName(device_name, sizeof(DEV_NAME));
}

#endif
