/*
 * spp.h
 *
 *  Created on: 2016-11-3
 *      Author: Administrator
 */
#pragma once
/*********************************************************************
 * CONSTANTS
 */
#include "../../proj_lib/ble/blt_config.h"

// FLYCO_spp cmd/data type
#define IS_FLYCO_SPP_CMD(p)           ((p[0] == 0x46) && (p[1] == 0x4c) && (p[2] == 0x59) && (p[3] == 0x43) && (p[4] == 0x4F))
//#define IS_FLYCO_SPP_DATA(p)          (!((p[0] == 0x46) && (p[1] == 0x4c) && (p[2] == 0x59) && (p[3] == 0x43) && (p[4] == 0x4F)))
#define IS_FLYCO_SPP_DATA(p)          (((p[0] == 0xA5) || (p[0] == 0x5A)) && p[15] == 0xAA)

//ACK//0x66,0x65,0x69,0x6b,0x65
#define IS_FLYCO_SPP_CMD_ACK(p)        ((p[0] == 0x66) && (p[1] == 0x65) && (p[2] == 0x69) && (p[3] == 0x6b) && (p[4] == 0x65))

//OTA error CODE send to MCU UART
#define IS_OTA_ERROR_CODE2MCU(p)      ((p[0] == 0x08) && (p[1] == 0xaa) && (p[2] == 0xbb) && (p[3] == 0xcc) &&  \
		                               (p[4] == 0x46) && (p[5] == 0x4c) && (p[6] == 0x59) && (p[7] == 0x43) && (p[8] == 0x4F))
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
    FLYCO_SPP_CMD_MODULE_OTA_START_REQ = 0x1C,//
    FLYCO_SPP_CMD_MODULE_OTA_START,//
    FLYCO_SPP_CMD_MODULE_OTA_FAILED = 0x1E,//
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
//please take 826x BLE SDK Developer Handbook for reference(page 24).
//         8266 512K flash address setting:
//          0x80000 |~~~~~~~~~~~~~~~~~~|
//                  |  user data area  |
//          0x78000 |~~~~~~~~~~~~~~~~~~|
//                  |  customed value  |
//          0x77000 |~~~~~~~~~~~~~~~~~~|
//                  |    MAC address   |
//          0x76000 |~~~~~~~~~~~~~~~~~~|
//                  |    pair & sec    |
//                  |       info       |
//          0x74000 |~~~~~~~~~~~~~~~~~~|
//                  |   ota_boot_flg   |
//          0x73000 |~~~~~~~~~~~~~~~~~~|
//                  |   ota_boot.bin   |
//          0x72000 |~~~~~~~~~~~~~~~~~~|
//                  |  user data area  |
//                  |                  |
//                  |                  |
//          0x40000 |~~~~~~~~~~~~~~~~~~|
//                  |   OTA new bin    |
//                  |   storage area   |
//                  |                  |
//          0x20000 |~~~~~~~~~~~~~~~~~~|
//                  | old firmwave bin |
//                  |                  |
//                  |                  |
//          0x00000 |~~~~~~~~~~~~~~~~~~|
//////////////////////User data in FLASH//////////////////////
#define				BAUD_RATE_ADDR        0x69000
#define				RF_POWER_ADDR         0x6a000
#define				IDENTIFIED_ADDR       0x6b000
#define				ADV_TIMEOUT_ADDR      0x6c000
#define				DEV_NAME1_ADDR        0x6d000
#define				DEV_NAME2_ADDR        0x6e000
#define				DEV_NAME_ADDR         0x6f000
#define				ADV_DATA_ADDR         0x70000
#define				ADV_INTERVAL_ADDR     0x71000

//--------------------------nv management----------------------------
#define             PARAM_NV_PDU_UNIT                30
#define				PARAM_NV_UNIT					(PARAM_NV_PDU_UNIT + 2)//header(2B) + userData(30B)
#define             PARAM_NV_MAX_IDX                (4096 - PARAM_NV_UNIT)
typedef struct{//used to indicate para address index
	u8  curNum;
	u8  tmp;
	u8  data[PARAM_NV_PDU_UNIT];
}nv_manage_t;

nv_manage_t baudrate_manage;
nv_manage_t rf_power_manage;
nv_manage_t identified_manage;
nv_manage_t adv_timeout_manage;
nv_manage_t dev_name1_manage;
nv_manage_t dev_name2_manage;
nv_manage_t dev_name_manage;
nv_manage_t adv_data_manage;
nv_manage_t adv_interval_manage;

typedef enum {
	NV_FLYCO_ITEM_BAUD_RATE,   //0 0x69000
	NV_FLYCO_ITEM_RF_POWER,    //1 0x6a000
	NV_FLYCO_ITEM_IDENTIFIED,  //2 0x6b000
	NV_FLYCO_ITEM_ADV_TIMEOUT, //3 0x6c000
	NV_FLYCO_ITEM_DEV_NAME1,   //4 0x6d000
	NV_FLYCO_ITEM_DEV_NAME2,   //5 0x6e000
	NV_FLYCO_ITEM_DEV_NAME,    //6 0x6f000
	NV_FLYCO_ITEM_ADV_DATA,    //7 0x70000
	NV_FLYCO_ITEM_ADV_INTERVAL,//8 0x71000
} nv_flycoItemId_t;

/*********************************************************************
 * Public Functions
 */
//user data save in FLASH management
u8 nv_write(u8 id, u8 *buf, u16 len);
//获取当前FLASH指定区域中参数存储的当前idx值
int get_current_flash_idx(u32 addr);
//从FLASH中获取当前idx存储的参数
u8 load_param_from_flash(u32 addr, u8* p, u8 len);
//判断是否需要将指定区域的FLASH sector擦除（4K）
void param_clear_flash(u32 addr);
//将用户数据存储到FLASH中当前idx区域
u8 save_param_nv(u32 addr, u8* buf, u16 len);

//flyco spp process function
void reverse_data(u8 *p,u8 len,u8*rp);
void flyco_module_uartCmdHandler(unsigned char* p, u32 len);
void flyco_module_masterCmdHandler(u8 *p, u32 len);
int  flyco_rx_from_uart (void);  //UART data send to Master,we will handle the data as CMD or DATA
int  flyco_tx_to_uart ();        //Master data send to UART,we will handle the data as CMD or DATA
int	 flyco_uart_push_fifo (int n, u8 *p);//flyco write callback to save master data
void blt_user_timerCb_proc(void);//user Timer callback proc for spp cmd ack

