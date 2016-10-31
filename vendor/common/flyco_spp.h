/*
 * flyco_spp.h
 *
 *  Created on: 2016-6-15
 *      Author: yafei.tu
 */
#pragma once
/*********************************************************************
 * CONSTANTS
 */
#include "../../proj_lib/ble/blt_config.h"

// FLYCO_spp cmd/data type
#define IS_FLYCO_SPP_CMD(p)           ((p[0] == 0x46) && (p[1] == 0x4c) && (p[2] == 0x59) && (p[3] == 0x43) && (p[4] == 0x4F))
#define IS_FLYCO_SPP_DATA(p)          (!((p[0] == 0x46) && (p[1] == 0x4c) && (p[2] == 0x59) && (p[3] == 0x43) && (p[4] == 0x4F)))

#define IS_D2_DATA_FROM_MCU(p)        ((p[0] == 0xA5) && (p[1] == 0xD2) && (p[2] == 0x00) && (p[3] == 0x00) && (p[4] == 0x00) && (p[5] == 0x00))
// Definition for FLYCO SPP command header
#define FLYCO_SPP_CMD_FIELD		\
	u8 signature[5];            \
	u8 cmdID;             \
	u8 len

// Definition for FLYCO SPP command header
#define FLYCO_SPP_RSP2CMD_FIELD		\
	u8 signature[5];            \
	u8 cmdID;             \
	u8 len

/*********************************************************************
 * ENUMS
 */

// FLYCO Command ID
enum {
	/* FLYCO Module Command ID */
	FLYCO_SPP_CMD_MODULE_GET_MAC = 0x00,
	FLYCO_SPP_CMD_MODULE_GET_RSSI,
	FLYCO_SPP_CMD_GET_MODULE_STATE,
	FLYCO_SPP_CMD_MODULE_RESTART =0x03,
	FLYCO_SPP_CMD_MODULE_DISCONNECT,
	FLYCO_SPP_CMD_MODULE_SET_SLEEP,
	FLYCO_SPP_CMD_MODULE_GET_DEVNAME1 =0x06,
	FLYCO_SPP_CMD_MODULE_SET_DEVNAME1,
	FLYCO_SPP_CMD_MODULE_GET_BAUDRATE,
	FLYCO_SPP_CMD_MODULE_SET_BAUDRATE =0x09,
	FLYCO_SPP_CMD_MODULE_GET_ADV_ENABLE_FLAG,
	FLYCO_SPP_CMD_MODULE_SET_ADV_ENABLE_FLAG,
    FLYCO_SPP_CMD_MODULE_GET_IDENTIFIED =0x0C, //By default MAC address is the identification code for the module.
    FLYCO_SPP_CMD_MODULE_SET_IDENTIFIED,
    FLYCO_SPP_CMD_MODULE_GET_ADV_DATA,
    FLYCO_SPP_CMD_MODULE_SET_ADV_DATA =0x0F,
    FLYCO_SPP_CMD_MODULE_GET_ADV_INTV,
    FLYCO_SPP_CMD_MODULE_SET_ADV_INTV,
    FLYCO_SPP_CMD_MODULE_GET_RF_PWR =0x12,
    FLYCO_SPP_CMD_MODULE_SET_RF_PWR,
    FLYCO_SPP_CMD_MODULE_GET_VERSION_NUM,
    FLYCO_SPP_CMD_MODULE_GET_ADV_TIMEOUT =0x15,
    FLYCO_SPP_CMD_MODULE_SET_ADV_TIMEOUT,
    FLYCO_SPP_CMD_MODULE_GET_SERVICE_UUID,
    FLYCO_SPP_CMD_MODULE_GET_SERVICE_WRITE_UUID =0x18,
    FLYCO_SPP_CMD_MODULE_GET_SERVICE_NOTIFY_UUID,
    FLYCO_SPP_CMD_MODULE_GET_DEVNAME2 = 0x1A,
    FLYCO_SPP_CMD_MODULE_SET_DEVNAME2,
    FLYCO_SPP_CMD_MODULE_OTA_START_REQ = 0x1C,
    FLYCO_SPP_CMD_MODULE_OTA_START,
    FLYCO_SPP_CMD_MODULE_RESET = 0x1E,
    FLYCO_SPP_CMD_MODULE_MAX,
};

/*********************************************************************
 * TYPES
 */

typedef struct {
	u8 data[1];
}  flyco_spp_moduleCmd_sendData_t;

typedef void (*flyco_spp_dataCb_t)(flyco_spp_moduleCmd_sendData_t *pSendDataCmd);
typedef struct flyco_spp_AppCallbacks_s {
	flyco_spp_dataCb_t  flyco_spp_dataCb;   //!< send data callback function
} flyco_spp_AppCallbacks_t;

typedef struct {
	FLYCO_SPP_CMD_FIELD;
	u8	data[1];
} flyco_spp_cmd_t;

typedef struct {
	FLYCO_SPP_RSP2CMD_FIELD;
	u8	data[1];
} flyco_spp_rsp2cmd_t;

typedef struct {
	FLYCO_SPP_CMD_FIELD;
	u8 intv[2];       // Advertising intervals (units of 625us, 160=100ms)
} flyco_spp_cmd_adv_intv_t;

typedef struct {
	FLYCO_SPP_CMD_FIELD;
	u8	data[20];
} flyco_spp_cmd_adv_data_t;

typedef struct {
	FLYCO_SPP_CMD_FIELD;
	u8	data[6];
} flyco_spp_cmd_identified_t;

typedef struct {
	FLYCO_SPP_CMD_FIELD;
	u8	pwr;
} flyco_spp_cmd_rf_pwr_t;

typedef struct {
	FLYCO_SPP_CMD_FIELD;
	u8	tim;
} flyco_spp_cmd_adv_timeout_t;

typedef struct {
	FLYCO_SPP_CMD_FIELD;
	u8	rate[3];
} flyco_spp_cmd_baud_rate_t;

typedef struct {
	FLYCO_SPP_CMD_FIELD;
	u8 enable;
} flyco_spp_moduleCmd_advEnable_t;

typedef struct {
	FLYCO_SPP_CMD_FIELD;
	u8 data[MAX_DEV_NAME_LEN];
} flyco_spp_set_dev_name;

/*********************************************************************
 * GLOBAL VARIABLES
 */
//////////////////////User data in FLASH//////////////////////
#define				BAUD_RATE_ADDR        0x40000
#define				RF_POWER_ADDR         0x41000
#define				IDENTIFIED_ADDR       0x42000
#define				ADV_TIMEOUT_ADDR      0x43000
#define				DEV_NAME1_ADDR        0x44000
#define				DEV_NAME2_ADDR        0x45000
#define				DEV_NAME_ADDR         0x46000
#define				ADV_DATA_ADDR         0x47000
#define				ADV_INTERVAL_ADDR     0x48000

typedef enum {
	NV_FLYCO_ITEM_BAUD_RATE,
	NV_FLYCO_ITEM_RF_POWER,
	NV_FLYCO_ITEM_IDENTIFIED,
	NV_FLYCO_ITEM_ADV_TIMEOUT,
	NV_FLYCO_ITEM_DEV_NAME1,
	NV_FLYCO_ITEM_DEV_NAME2,
	NV_FLYCO_ITEM_DEV_NAME,
	NV_FLYCO_ITEM_ADV_DATA,
	NV_FLYCO_ITEM_ADV_INTERVAL,
} nv_flycoItemId_t;

/*********************************************************************
 * Public Functions
 */
void reverse_data(u8 *p,u8 len,u8*rp);

u8 nv_read(u8 id, u8 *buf, u16 len);
u8 nv_write(u8 id, u8 *buf, u16 len);
void flyco_module_uartCmdHandler(unsigned char* p, u32 len);
void flyco_module_masterCmdHandler(u8 *p, u32 len);
