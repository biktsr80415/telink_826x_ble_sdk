#include "../../proj/tl_common.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "app_att.h"

#if (1)

#define			MY_ATT_PERMISSION			(ATT_PERMISSIONS_READ | ATT_PERMISSIONS_WRITE)

u8 beacon_resetCharater = 0; //reset Charateristic stored in EEPROM
u8 beacon_txLevelIndex[4] = {3,2,1, 0};
s8 beacon_txPowerValue[4] = {-4,0,5, 7};//-4: 0xFC, 0:0x00, 5:0x05, 7:0x07

const u16 beacon_deviceServiceUUID = SERVICE_UUID_DEVICE_INFORMATION;
const u16 beacon_gapServiceUUID = SERVICE_UUID_GENERIC_ACCESS;
const u16 beacon_batteryServiceUUID = SERVICE_UUID_BATTERY;

const u16 beacon_devicefwv = GATT_UUID_FIRMWARE_VER;
const u16 beacon_deviceName = GATT_UUID_DEVICE_NAME;
const u16 beacon_batteryLevelUUID = GATT_UUID_BATTERY_LEVEL;
u8 beacon_battery_level = 100;
const u8 fwm[]={'t','l','8','2','6','3','0','1'};
const u8 beacon_devName[12]= {'t','e','l','i','n','k','b','e','a','c','o','n'};
const u8 beacon_txPowerModeConfigUserDesc[] = "Tx Power Mode";
const u8 beacon_txPowerLevelReadUserDesc[] = "Read Adv Power Level";
const u8 beacon_advPeriodUserDesc[] = "Beacon Period";
const u8 beacon_resetCharacterUserDesc[] = "Reset Config";
const u8 beacon_proxiUUIDUserDesc[] = "Proximity UUID";
const u8 beacon_majorConfigUserDesc[] = "Major";
const u8 beacon_minorConfigUserDesc[] = "Minor";
const u8 beacon_eddystoneIDUserDesc[] = "Eddystone ID";
const u8 beacon_beaconModeUserDesc[] = "Beacon Mode";

const u16 beacon_primaryServiceUUID 	= GATT_UUID_PRIMARY_SERVICE;
const u16 beacon_characterUUID 	= GATT_UUID_CHARACTER;
const u16 beacon_characterUserDescriptionUUID = GATT_UUID_CHAR_USER_DESC;

const u8 beacon_confServiceUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x80,0x20,0x0c,0xee};
const u8 beacon_advTxPowerConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x86,0x20,0x0c,0xee};
const u8 beacon_txPowerModeConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x88,0x20,0x0c,0xee};
const u8 beacon_proxiUUIDConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x89,0x20,0x0c,0xee};
const u8 beacon_txPM_BeaconPeriodConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x8A,0x20,0x0c,0xee};
const u8 beacon_BeaconCharacterResetUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x83,0x20,0x0c,0xee};
const u8 beacon_majorConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x8B,0x20,0x0c,0xee};
const u8 beacon_minorConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x8C,0x20,0x0c,0xee};
const u8 beacon_eddystoneIDConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x8D,0x20,0x0c,0xee};
const u8 beacon_workMode[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x8E,0x20,0x0c,0xee};
// Device Name Characteristic Properties
const u8 beacon_confCharacterProp = CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP;
const u8 beacon_deviceInf  = CHAR_PROP_READ | CHAR_PROP_NOTIFY;


const u8 my_OtaServiceUUID[16]		= TELINK_OTA_UUID_SERVICE;
const u8 my_OtaUUID[16]		= TELINK_SPP_DATA_OTA;
const u16 userdesc_UUID		= GATT_UUID_CHAR_USER_DESC;
static u8 my_OtaProp		= CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RSP;
u8	 	my_OtaData 		= 0x00;
const u8  my_OtaName[] = {'O', 'T', 'A'};
static const u16 my_characterUUID = GATT_UUID_CHARACTER;


ibeacon_adv_t ibeacon_tbl_adv = {
		0x02, 0x01, 0x06,
		0x1a, 			//len
		0xff,			//manufacture
		0x004c,			//aple
		0x1502,			//beacon
		{0xFD,0xA5,0x06,0x93,0xA4,0xE2,0x4F,0xB1,0xAF,0xCF,0xC6,0xEB,0x07,0x64,0x78,0x25}, //prox UUID
		0x3327, 		//major
		0x1fba, 		//minor
		0x01			//TX Power
};

telink_beacon_config_t telink_beacon_config =
{
		1000,				// 1 s
		2,					//0 dBm
		{0, }
};

void beacon_configParamInit(telink_beacon_config_t * config_p)
{
}



extern u8	tbl_advData[];
int setIbeaconProximityUUID(void* p){
	rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
	memcpy(ibeacon_tbl_adv.proximity_UUID, &pm->value,16);
	app_update_adv ();
}

int setBeaconMajor(void* p){
	rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
	u16 beacon_major_tmp = pm->value + (*(&pm->value + 1)<<8);
	ibeacon_tbl_adv.major = beacon_major_tmp;
	app_update_adv ();
}
int setBeaconMinor(void* p){
	rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
	u16 beacon_minor_tmp = pm->value + (*(&pm->value + 1)<<8);

	ibeacon_tbl_adv.minor = beacon_minor_tmp;
	app_update_adv ();
}

int setEddystoneID(void* p){
#if EDDYSTONE_UID_ENABLE
	rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
	memcpy(eddystone_UID_tbl_adv.namespace_ID, &pm->value,16);
#endif
}

int setBeaconMode(void* p){
    u8 i=0;
    u8 status = BLE_SUCCESS;
    rf_packet_att_write_t *pm = (rf_packet_att_write_t*)p;
    beacon_adv_mode_config_t* mode_config = (beacon_adv_mode_config_t*)(&(pm->value));
    if((mode_config->adv_type_num>0) && (mode_config->adv_type_num<MAX_ADV_TYPE_PER_PERIOD)){
        /*Check mode_config para is valid*/
        for(i=0; i<mode_config->adv_type_num; i++){
            status = (mode_config->adv_mode[i].adv_type<4)?BLE_SUCCESS:1;
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
            beacon_iBeaconPDUassemble((u8*)&ibeacon_tbl_adv);
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

// TM : to modify
const attribute_t beacon_Attributes[] = {
	{37 + 4,MY_ATT_PERMISSION,0,0,0,0},	//
	/*Handle 1~3, 0x180A, device info service*/
	// gatt
	{3,MY_ATT_PERMISSION,2,2,(u8*)(&beacon_primaryServiceUUID), 	(u8 *)(&beacon_deviceServiceUUID), 0},
	//firmware version
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 	(u8*)(&beacon_deviceInf), 0},
	{0,MY_ATT_PERMISSION,2,sizeof(fwm),(u8*)(&beacon_devicefwv),fwm, 0},//characteristic value

    /*Handle 4~6, 0x1800, GAP Service*/
	{3,MY_ATT_PERMISSION,2,2,(u8*)(&beacon_primaryServiceUUID), (u8 *)(&beacon_gapServiceUUID), 0},
	//device name
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 	(u8*)(&beacon_deviceInf), 0},
	{0,MY_ATT_PERMISSION,2,sizeof(beacon_devName),(u8*)(&beacon_deviceName), beacon_devName, 0},//characteristic value

    /*Handle 7~9, 0x180F, Battery Service*/
	//battery
	{3,MY_ATT_PERMISSION,2,2,(u8*)(&beacon_primaryServiceUUID), 	(u8*)(&beacon_batteryServiceUUID), 0},
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 		(u8*)(&beacon_deviceInf), 0},
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_batteryLevelUUID), (u8*)(&beacon_battery_level), 0},

    /*Handle 0x0A~0x25, Beacon configuration Service, 0xEE0C.....D8*/
	// gatt
	{28,MY_ATT_PERMISSION,2,16,(u8*)(&beacon_primaryServiceUUID), 	(u8 *)(&beacon_confServiceUUID), 0},		//beacon conf service

	//tx power mode configure characteristic declare
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 		(u8*)(&beacon_confCharacterProp), 0},
	{0,MY_ATT_PERMISSION,16,1,(u8*)(&beacon_txPowerModeConfUUID), 	&(telink_beacon_config.txpower_mode), &setTxPowerMode},//characteristic value
	{0,MY_ATT_PERMISSION,2,sizeof(beacon_txPowerModeConfigUserDesc),(u8*)(&beacon_characterUserDescriptionUUID),beacon_txPowerModeConfigUserDesc, 0},//characteristic user description

	//ADV Tx Power configuration characteristic declare
					//type							//prop
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 		(u8*)(&beacon_deviceInf), 0},
	{0,MY_ATT_PERMISSION,16, 4,(u8*)(&beacon_advTxPowerConfUUID), 	beacon_txPowerValue, 0},//characteristic value
	{0,MY_ATT_PERMISSION,2,sizeof(beacon_txPowerLevelReadUserDesc),(u8*)(&beacon_characterUserDescriptionUUID),beacon_txPowerLevelReadUserDesc, 0},//characteristic user description

	//Beacon Period configuration characteristic declare
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 		(u8*)(&beacon_confCharacterProp), 0},
	{0,MY_ATT_PERMISSION,16,2,(u8*)(&beacon_txPM_BeaconPeriodConfUUID), 	(u8 *)(&(telink_beacon_config.beacon_period)), &setBeaconPeriod},//characteristic value
	{0,MY_ATT_PERMISSION,2,sizeof(beacon_advPeriodUserDesc),(u8*)(&beacon_characterUserDescriptionUUID),beacon_advPeriodUserDesc, 0},//characteristic user description

    //Beacon charateristic reset
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 		(u8*)(&beacon_confCharacterProp), 0},
	{0,MY_ATT_PERMISSION,16,1,(u8*)(&beacon_BeaconCharacterResetUUID), 	&beacon_resetCharater, &resetBeaconCharater},//characteristic value
	{0,MY_ATT_PERMISSION,2,sizeof(beacon_resetCharacterUserDesc),(u8*)(&beacon_characterUserDescriptionUUID),beacon_resetCharacterUserDesc, 0},//characteristic user description


	//Company UUID configuration characteristic declare, 0x17
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 		(u8*)(&beacon_confCharacterProp), 0},
	{0,MY_ATT_PERMISSION,16,16,(u8*)(&beacon_proxiUUIDConfUUID), 	ibeacon_tbl_adv.proximity_UUID, &setIbeaconProximityUUID},//characteristic value
	{0,MY_ATT_PERMISSION,2,sizeof(beacon_proxiUUIDUserDesc),(u8*)(&beacon_characterUserDescriptionUUID),beacon_proxiUUIDUserDesc, 0},//characteristic user description


	//Major configuration characteristic declare
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 		(u8*)(&beacon_confCharacterProp), 0},
	{0,MY_ATT_PERMISSION,16,2,(u8*)(&beacon_majorConfUUID), (u8*)(&(ibeacon_tbl_adv.major)), &setBeaconMajor},//characteristic value
	{0,MY_ATT_PERMISSION,2,sizeof(beacon_majorConfigUserDesc),(u8*)(&beacon_characterUserDescriptionUUID),beacon_majorConfigUserDesc, 0},//characteristic user description


	//Minor configuration characteristic declare
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 		(u8*)(&beacon_confCharacterProp), 0},
	{0,MY_ATT_PERMISSION,16,2,(u8*)(&beacon_minorConfUUID), (u8*)(&(ibeacon_tbl_adv.minor)), &setBeaconMinor},//characteristic value
	{0,MY_ATT_PERMISSION,2,sizeof(beacon_minorConfigUserDesc),(u8*)(&beacon_characterUserDescriptionUUID),beacon_minorConfigUserDesc, 0},//characteristic user description


	//Eddystone UUID configuration characteristic declare
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 		(u8*)(&beacon_confCharacterProp), 0},
	{0,MY_ATT_PERMISSION,16,16,(u8*)(&beacon_eddystoneIDConfUUID), 	eddystone_UID_tbl_adv.namespace_ID, &setEddystoneID},//characteristic value
	{0,MY_ATT_PERMISSION,2,sizeof(beacon_eddystoneIDUserDesc),(u8*)(&beacon_characterUserDescriptionUUID),beacon_eddystoneIDUserDesc, 0},//characteristic user description


	//Beacon work mode
	{0,MY_ATT_PERMISSION,2,1,(u8*)(&beacon_characterUUID), 		(u8*)(&beacon_confCharacterProp), 0},
	{0,MY_ATT_PERMISSION,16,sizeof(beacon_adv_mode_config_t),(u8*)(&beacon_workMode), 	(u8*)(&(telink_beacon_config.beacon_mode)), &setBeaconMode},//characteristic value
	{0,MY_ATT_PERMISSION,2,sizeof(beacon_beaconModeUserDesc),(u8*)(&beacon_characterUserDescriptionUUID),beacon_beaconModeUserDesc, 0},//characteristic user description

	// OTA
	{4,MY_ATT_PERMISSION, 2,16,(u8*)(&beacon_primaryServiceUUID), 	(u8*)(&my_OtaServiceUUID), 0},
	{0,MY_ATT_PERMISSION, 2, 1,(u8*)(&my_characterUUID), 		(u8*)(&my_OtaProp), 0},				//prop
	{0,MY_ATT_PERMISSION,16,sizeof(my_OtaData),(u8*)(&my_OtaUUID),	(&my_OtaData), &otaWrite, &otaRead},			//value
	{0,MY_ATT_PERMISSION, 2,sizeof (my_OtaName),(u8*)(&userdesc_UUID), (u8*)(my_OtaName), 0},
};

void	my_att_init ()
{
	bls_att_setAttributeTable ((u8 *)beacon_Attributes);
}

#endif  //end of __PROJECT_8267_BLE_REMOTE__ || __PROJECT_8261_BLE_REMOTE__ || __PROJECT_8269_BLE_REMOTE__
