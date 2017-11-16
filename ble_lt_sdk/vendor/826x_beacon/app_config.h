#pragma once

#include "../../proj/common/types.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define _USER_CONFIG_DEFINED_	1	// must define this macro to make others known
#define	__LOG_RT_ENABLE__		0
//#define	__DEBUG_PRINT__			0
//////////// product  Information  //////////////////////////////
#define ID_VENDOR				0x248a			// for report
#define ID_PRODUCT_BASE			0x880C
// If ID_PRODUCT left undefined, it will default to be combination of ID_PRODUCT_BASE and the USB config USB_SPEAKER_ENABLE/USB_MIC_ENABLE...
// #define ID_PRODUCT			0x8869

#define STRING_VENDOR			L"Telink"
#define STRING_PRODUCT			L"826x ble beacon"
#define STRING_SERIAL			L"TLSR826X"

#define	DEV_NAME_MLEN			18
#define	ADV_DATA_MLEN			16

#if		(__PROJECT_BEACON_8261__)
	#define CHIP_TYPE				CHIP_TYPE_8261
#elif   (__PROJECT_BEACON_8267__)
	#define CHIP_TYPE				CHIP_TYPE_8267
#endif

#define APPLICATION_DONGLE		0					// or else APPLICATION_DEVICE
#define	FLOW_NO_OS				1

#define		RF_FAST_MODE_1M		1
//////////////////////CAN NOT CHANGE CONFIG ABOVE ////////////////////////////

///////////////////// BATTERY /////////////////////////////////////////
#define	BATT_CHECK_ENABLE		1
#define ADC_MODULE_CLOSED               BM_CLR(reg_adc_mod, FLD_ADC_CLK_EN)  // adc clk disable
#define ADC_MODULE_ENABLE               BM_SET(reg_adc_mod, FLD_ADC_CLK_EN) // adc clk open

///////////////////  Hardware  //////////////////////////////
#define PULL_WAKEUP_SRC_PB0		PM_PIN_PULLUP_1M			 //open SWS digital pullup to prevent MCU err
/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE	0
#define WATCHDOG_INIT_TIMEOUT	500  //ms

/////////////////// set default   ////////////////

#include "../common/default_config.h"

/******************************To be configured by developer**********************************************/
/*TODO: define beacon configuration parameter:Period / TX_Power  /  PM mode enable / connectible mode duration time  */
#define BEACON_PERIOD_DEFAULT		2000 //default beacon period 1000ms
#define BEACON_TX_POWER_DEFAULT 	1 //default tx power, level 1, 0 dbm
#define BEACON_MODE_WAIT_TIME		(10*CLOCK_SYS_CLOCK_1S) //wait 10 seconds before switch to the beacon mode, maximum 60s//test Eason

/*TODO: Define beacon type supported by this firmware*/
#define IBEACON_ADV_ENABLE    1
#define EDDYSTONE_UID_ENABLE  1
#define EDDYSTONE_URL_ENABLE  1
#define EDDYSTONE_TLM_ENABLE  1

/*TODO: Define beacon parameter save address of flash*/
#define 			ADV_STORE_IN_FLASH_EN  			0//ADV data store flash
#if		(__PROJECT_BEACON_8267__)
#define             FLASH_ADDR_IBEACON_ADV          0x70000                                                                                                                         //48 bytes, from 0
#define			    FLASH_ADDR_EDDYSTONE_UID_ADV	0x71000       //48 bytes, from 0x71000
#define             FLASH_ADDR_EDDYSTONE_URL_ADV    0x72000     	//48 bytes, from 0x72000
#define             FLASH_ADDR_EDDYSTONE_TLM_ADV    0x73000     	//48 bytes, from 0x73000
#define             FLASH_TELINK_BEACON_CONFIG      0x74000     	// 48 bytes, from 0x74000
#elif (__PROJECT_BEACON_8261__)
#define             FLASH_ADDR_IBEACON_ADV          0x10000                                                                                                                         //48 bytes, from 0
#define			    FLASH_ADDR_EDDYSTONE_UID_ADV	0x11000       //48 bytes, from 0x11000
#define             FLASH_ADDR_EDDYSTONE_URL_ADV    0x12000     	//48 bytes, from 0x12000
#define             FLASH_ADDR_EDDYSTONE_TLM_ADV    0x13000     	//48 bytes, from 0x13000
#define             FLASH_TELINK_BEACON_CONFIG      0x14000     	// 48 bytes, from 0x14000
#endif

/*Parameter below not support to modify*/
#define				IBEACON_PDU_LEN					41//2bytes header + 3bytes crc +36 bytes adv
#define				EDDYSTONE_UID_PDU_LEN			42//2bytes header + 3bytes crc +37 bytes adv
#define             EDDYSTONE_URL_PDU_MAX_LEN       42//2bytes header + 3bytes crc +37 bytes adv
#define             EDDYSTONE_TLM_PDU_LEN           36//2bytes header + 3bytes crc +31bytes adv

#define             FLASH_IBEACON_ADV_LEN           48
#define             FLASH_EDDYSTONE_UID_LEN         48
#define             FLASH_EDDYSTONE_URL_LEN         48
#define             FLASH_EDDYSTONE_TLM_LEN         48

#define				BEACON_INVALIAD_STAUS		0xFF		// define beacon invalid parameter statue value

/*typdef for beacon*/
typedef enum{
	MODULE_STATE_CONFIGURE,
	MODULE_STATE_CONNECTED,
	MODULE_STATE_TERMINATED,
	MODULE_STATE_BEACON,
	MODULE_STATE_SLEEP,
	MODULE_STATE_ERROR,
}module_state;
typedef enum{
	EDDYSTONE_UID,
	EDDYSTONE_URL = 0x10,
	EDDYSTONE_TLM = 0x20,
}eddystone_mode;

typedef enum{
    TELINK_IBEACON_MODE = 0x00,
    TELINK_EDDYSTONE_UID_MODE,
    TELINK_EDDYSTONE_URL_MODE,
    TELINK_EDDYSTONE_TLM_MODE,
    TELINK_BEACON_MODE_MAX,
}telink_beaconMode;

/*Define adv mode config para*/
#define MAX_ADV_TYPE_PER_PERIOD  5

typedef struct{
    u8 adv_type;//store adv type
    u8 adv_num; //store adv times for every period
}beacon_adv_mode_t;

typedef struct{
    u8 adv_type_num;
    beacon_adv_mode_t adv_mode[MAX_ADV_TYPE_PER_PERIOD];
}beacon_adv_mode_config_t;


typedef struct{
    u8 adv_type; //define connectable or non-connectable adv type
    u8 adv_len; //define adv total len
    u8 adv_addr[6]; //define adv mac addr
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
    u8 adv_crc[3];
}ibeacon_adv_t;

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


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
