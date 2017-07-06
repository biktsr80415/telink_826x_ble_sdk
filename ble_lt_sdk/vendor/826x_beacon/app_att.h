#include "../../proj/tl_common.h"

/*Define adv mode config para*/
#define MAX_ADV_TYPE_PER_PERIOD  5
#define BEACON_PERIOD_MAX 60000 //beacon_period max 60s
#define TX_POWERMODE_MAX  3 //tx_powerMode, 0, 1, 2, 3

//typedef unsigned char u8;
//typedef unsigned short u16;

typedef struct{
    u8 adv_type;//store adv type
    u8 adv_num; //store adv times for every period
}beacon_adv_mode_t;

typedef struct{
    u8 adv_type_num;
    beacon_adv_mode_t adv_mode[MAX_ADV_TYPE_PER_PERIOD];
}beacon_adv_mode_config_t;


typedef struct{
    u8 flag_len; //constant, 0x02
    u8 flag_type;//constant, 0x01, flags type
    u8 flag; //constant, 0x06, flags indicating LE General discoverable and not supporting BR/EDR
    u8 ibeacon_len;//constant, 0x1A
    u8 manu_spec; //constant, 0xFF
    u16 commpany_ID;//constant,0X004C, Apple Inc
    u16 beacon_type; //constant,0X0215
    u8 proximity_UUID[16];//Defined by customer
    u16 major;//Defied by Customer
    u16 minor;//Defied by Customer
    s8 measured_power;//Defied by Customer, TX Power-41
}ibeacon_adv_t;

typedef struct{

    u8 flag_len; //constant, 0x02
    u8 flag_type;//constant, 0x01, flags type
    u8 flag; //constant, 0x06, flags indicating LE General discoverable and not supporting BR/EDR
    u8 eddystone_type_len; //constant,0x03
    u8 complete_uuid_type;//constant,0x03
    u16 eddystone_UUID; //constant,0xFEAA
    u8 eddystone_len;   //constant, UID, 0x17
    u8 service_data_type; //constant, 0x16
    u16 eddystone_UUID_in_service_data; //constant, 0xFEAA

    u8 frame_type; //constant, 0x00
    s8 tx_power;//Defined by customer, TX Power
    u8 namespace_ID[10]; //Defined by customer
    u8 instance_ID[6]; //Defined by customer
    u16 reserved_bytes; //Reserved for Future
    u8 adv_crc[3];


}eddystone_UID_t;

typedef struct{
    u8 adv_type; //define connectable or non-connectable adv type
    u8 adv_len; //define adv total len
    u8 adv_addr[6]; //define adv mac addr
    u8 flag_len; //constant, 0x02
    u8 flag_type;//constant, 0x01, flags type
    u8 flag; //constant, 0x06, flags indicating LE General discoverable and not supporting BR/EDR
    u8 eddystone_type_len; //constant,0x03
    u8 complete_uuid_type;//constant,0x03
    u16 eddystone_UUID; //constant,0xFEAA
    u8 eddystone_len;   //Defined by customer, max 0x17
    u8 service_data_type; //constant, 0x16
    u16 eddystone_UUID_in_service_data; //constant, 0xFEAA

    u8 frame_type; //constant, 0x10
    s8 tx_power; //Defined by customer, TX Power
    u8 scheme_URL; //Defined by customer, ex: 0x00 means "http://www."
    u8 encoded_URl[17];//Defined by customer, max 0x11
    u8 adv_crc[3];


}eddystone_URL_t;

typedef struct{
    u8 adv_type; //define connectable or non-connectable adv type
    u8 adv_len; //define adv total len
    u8 adv_addr[6]; //define adv mac addr
    u8 flag_len; //constant, 0x02
    u8 flag_type;//constant, 0x01, flags type
    u8 flag; //constant, 0x06, flags indicating LE General discoverable and not supporting BR/EDR
    u8 eddystone_type_len; //constant,0x03
    u8 complete_uuid_type;//constant,0x03
    u16 eddystone_UUID; //constant,0xFEAA
    u8 eddystone_len;   //constant, TLM 0x11
    u8 service_data_type; //constant, 0x16
    u16 eddystone_UUID_in_service_data; //constant, 0xFEAA

    u8 frame_type; //constant, 0x20
    u8 version_TLM; //constant, 0x00
    u16 battery_volt; //calculated by ADC, not supported currently, set 0x0000
    s16 temperature; //calculated by ADC, not supported currently, set0x8000
    u32 adv_cnt; //calculated automatically, advertising numbers since power on
    u32 sec_cnt;//calculated automatically, time since power on(unite: 0.1s)
    u8 adv_crc[3];
}eddystone_TLM_t;

typedef struct{
    u16 beacon_period; //beacon periode, unit: ms
    u8 txpower_mode; //txpower mode
    //u32 txpower_level;
    beacon_adv_mode_config_t beacon_mode;
}telink_beacon_config_t;

void    app_update_adv ();
int setTxPowerMode(void* p);
int setAdvTxLevel(void* p);
int setBeaconPeriod(void* p);
int setIbeaconProximityUUID(void* p);
int setBeaconMajor(void* p);
int setBeaconMinor(void* p);
int setEddystoneID(void* p);
int setBeaconMode(void* p);
int resetBeaconCharater(void * p);


ibeacon_adv_t ibeacon_tbl_adv;
telink_beacon_config_t telink_beacon_config;
ibeacon_adv_t ibeacon_tbl_adv;
eddystone_UID_t eddystone_UID_tbl_adv;
eddystone_URL_t eddystone_URL_tbl_adv;
eddystone_TLM_t eddystone_TLM_tbl_adv;

