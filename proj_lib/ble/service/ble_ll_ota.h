/*
 * ble_ll_ota.h
 *
 *  Created on: 2015-7-20
 *      Author: Administrator
 */

#pragma once

#ifndef	BLE_OTA_ENABLE
#define BLE_OTA_ENABLE		0
#endif

#if 0
#ifndef			NEW_FW_ADR
#define			NEW_FW_ADR			0x20000
#endif

#ifndef			OTA_FLG_ADR
#define			OTA_FLG_ADR			0x3f000//OTA FW ready flag at 0x3f000; (0x3f000 = #63 << 12)
#endif

#ifndef			OTA_8261_FLG_ADR
#define			OTA_8261_FLG_ADR	0x1c000
#endif

#ifndef			OTA_BOOT_BIN_ADR
#define			OTA_BOOT_BIN_ADR	0x1f000//(OTA128K: 0x1f000 = 31 << 12)
#endif

#define CMD_OTA_FW_VERSION					0xff00
#define CMD_OTA_START						0xff01
#define CMD_OTA_END							0xff02


extern int blt_ota_start_flag;
extern u32 blt_ota_start_tick;
extern u32 blt_ota_timeout_us;


typedef void (*ota_startCb_t)(void);
typedef void (*ota_versionCb_t)(void);

typedef void (*ota_resIndicateCb_t)(int result);
extern ota_resIndicateCb_t otaResIndicateCb;


enum{
	OTA_SUCCESS = 0,     //success
	OTA_PACKET_LOSS,     //lost one or more OTA PDU
	OTA_DATA_CRC_ERR,     //data CRC err
	OTA_WRITE_FLASH_ERR,  //write OTA data to flash ERR
 	OTA_DATA_UNCOMPLETE,  //lost last one or more OTA PDU
 	OTA_TIMEOUT, 		  //
};

void ble_setOtaStartCb(ota_startCb_t cb);
void ble_setOtaVersionCb(ota_versionCb_t cb);
void ble_setOtaResIndicateCb(ota_resIndicateCb_t cb);


void ble_setOtaTimeout(u32 timeout_us);

extern int otaWrite(void * p);
extern int otaRead(void * p);

extern void start_reboot(void);

#else

#ifndef			NEW_FW_ADR
#define			NEW_FW_ADR			0x20000
#endif

#ifndef			OTA_FLG_ADR
#define			OTA_FLG_ADR			0x3f000//OTA FW ready flag at 0x3f000; (0x3f000 = #63 << 12)
#endif

#ifndef			OTA_8261_FLG_ADR
#define			OTA_8261_FLG_ADR	0x1c000
#endif

#ifndef			OTA_BOOT_BIN_ADR
#define			OTA_BOOT_BIN_ADR	0x1f000//(OTA128K: 0x1f000 = 31 << 12)

#define CMD_OTA_FW_VERSION			0xff00
#define CMD_OTA_START				0xff01
#define CMD_OTA_END					0xff02
#define CMD_OTA_START_REQ           0xff03 //Request OTA start
#define CMD_OTA_START_RSP           0xff04 //Slave allow OTA start
#define CMD_OTA_TEST                0xff05 //Master send OTA TEST to slave to check slave's state
#define CMD_OTA_TEST_RSP            0xff06 //Slave respond to master's CMD_OTA_TEST
#define CMD_OTA_RESULT              0xff07 //Slave send OTA error to master once it stop OTA process because of some error


extern int flyco_blt_ota_start_flag;
extern u32 flyco_blt_ota_start_tick;
extern u32 flyco_blt_ota_timeout_us;


typedef void (*ota_startCb_t)(void);
typedef void (*ota_versionCb_t)(void);

typedef void (*ota_resIndicateCb_t)(int result);
extern ota_resIndicateCb_t flyco_otaResIndicateCb;
extern u8 ota_hdl;

enum{
	OTA_SUCCESS = 0,          //success
	OTA_PACKET_LOSS,          //lost one or more OTA PDU
	OTA_DATA_CRC_ERR,         //data CRC err
	OTA_WRITE_FLASH_ERR,      //write OTA data to flash ERR
 	OTA_DATA_UNCOMPLETE,      //lost last one or more OTA PDU
 	OTA_TIMEOUT, 		      //Slave OTA Timeout, OTA cost too long time for slave
 	OTA_FLAG_FLASH_ERR,       //Write OTA finish flag to flash error
 	OTA_PROCESS_DISCONNECTION,//Connection disconnected during OTA process
 	OTA_BOOT_BIN_NOT_EXIST,   //OTA Boot bin doesn't exist in addr 0x68000
 	OTA_ERROR_CODE_MAX,
};

void flyco_ble_setOtaStartCb(ota_startCb_t cb);
void flyco_ble_setOtaVersionCb(ota_versionCb_t cb);
void flyco_ble_setOtaResIndicateCb(ota_resIndicateCb_t cb);


void flyco_ble_setOtaTimeout(u32 timeout_us);

int flyco_otaWrite(void * p);
int flyco_otaRead(void * p);
void flyco_start_reboot(void);

#endif

#endif
