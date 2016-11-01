/*
 * ble_ll_ota.h
 *
 *  Created on: 2015-7-20
 *      Author: Administrator
 */

#ifndef BLE_LL_OTA_H_
#define BLE_LL_OTA_H_

#ifndef	BLE_OTA_ENABLE
#define BLE_OTA_ENABLE		0
#endif


#ifndef			NEW_FW_ADR
#define			NEW_FW_ADR			0x20000
#endif

#ifndef			OTA_FLG_ADR
#define			OTA_FLG_ADR			0x3f000
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

#endif /* BLE_LL_OTA_H_ */
