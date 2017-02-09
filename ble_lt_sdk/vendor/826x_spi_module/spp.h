/*
 * spp_8269.h
 *
 *  Created on: 2016-9-1
 *      Author: Administrator
 */

#pragma once

#define	SPP_RESULT_SUCCESS				0
#define	SPP_RESULT_FAILED				1
#define	SPP_GET_OR_SETPARA_FAILED_NOCONNECTION	3 //Add by tuyf

#define	DATA_SEND_FAILED_NOCONNECTION	4
#define	DATA_SEND_FAILED_BUSY			5
#define	DATA_SEND_FAILED_TXLEN			2

#define	NOTIFY_MAXIMUM_DATA_LEN			20

/**
 *  @brief  Function to check spp cmd/data type
 */

#define SPP_GET_EVT_FROM_CMD(cmd)    	((cmd&0x03ff)|0x0400)



#define	FLAG_NOTIFY_STATE_CHANGE		BIT(1)
#define	FLAG_HOST_STATUS_BUSY			BIT(2)

#define				EV_STATE_CHANGE				0xff30		//0x0730
#define				EV_DATA_SENT				0xff32		//EventID = (0xff32 & 0x03ff) | 0x0400 = 0x0732
#define				EV_DATA_REC					0xff31		//EventID = 0x0731

enum {
	/* Module Command ID */
	HC_RESET_MODULE_CONF = 0xff00,
	HC_SET_ADV_INTERVAL,
	HC_SET_ADV_DATA,
	HC_SET_MODULE_RF_POWER,
	HC_GET_MODULE_MAC = 0xff07,
	HC_ENABLE_DISABLE_ADV = 0xff0a, //FF0A
	HC_NOTIFY_DATA,
	HC_GET_BUFFER_NUM,
	HC_SET_ADV_TYPE,
	HC_SET_DIRECT_ADDR,
	HC_SET_DEVIVE_NAME = 0xff13,
	HC_GET_CONNECTION_PARAM,
	HC_SET_CONNECTION_PARAM,//ff15
	HC_GET_MODULE_STATE,//ff16
	HC_TERMINATE_CONNECTION,
	HC_RESTART,

	HC_ACK = 0xfffe,
}hc_command_type;

/*********************************************************************
 * TYPES
 */
typedef struct {
	u16	cmdID;
	u16 paramLen;
	u8 data[1];
}  spp_moduleCmd_sendData_t;

typedef struct {
	u16	cmdID;
	u16 paramLen;
	u8	data[1];
} spp_cmd_t;

typedef struct {
	u16	cmdID;
	u16 paramLen;
	u8	data[1];
} spp_dat_t;

typedef struct {
	u16	cmdID;
	u16 paramLen;
	u8 intv[2];       // Advertising intervals (units of 625us, 160=100ms)
} spp_cmd_adv_intv_t;

typedef struct {
	u16	cmdID;
	u16 paramLen;
	u8	data[31];
} spp_cmd_adv_data_t;

typedef struct {
	u16	cmdID;
	u16 paramLen;
	u8	data[6];
} spp_cmd_identified_t;

typedef struct {
	u16	cmdID;
	u16 paramLen;
	u8	pwr;
} spp_cmd_rf_pwr_t;

typedef struct {
	u16	cmdID;
	u16 paramLen;
	u8	tim;
} spp_cmd_adv_timeout_t;

typedef struct {
	u16	cmdID;
	u16 paramLen;
	u8	rate[3];
} spp_cmd_baud_rate_t;

typedef struct {
	u16	cmdID;
	u16 paramLen;
	u8 enable;
} spp_moduleCmd_advEnable_t;

typedef struct {
	u16	cmdID;
	u16 paramLen;
	u8 data[18];
} spp_set_dev_name;

/**
 *  @brief  Definition SPP Data writted by client callback function type for user application
 */
typedef void (*spp_dataCb_t)(spp_moduleCmd_sendData_t *pSendDataCmd);

/**
 *  @brief  Definition for foundation command callbacks.
 */

//////////////////////////////////////////////////////////////////////////////////////////////////

//Flags used in BLE module projects
#if PM_ENABLE
#define				FLAG_HIGH_DUTY_TIMER			BIT(0)
#define				FLAG_NOTIFY_STATE_CHANGE		BIT(1)
#define				FLAG_HOST_STATUS_BUSY			BIT(2)

#else
#define				FLAG_HIGH_DUTY_TIMER			BIT(0)
#define				FLAG_NOTIFY_STATE_CHANGE		BIT(1)
#define				FLAG_HOST_STATUS_BUSY			BIT(2)
#endif


typedef enum{
	MODULE_STATE_STANDBY    = 0x01,
	MODULE_STATE_ADV        = 0x03,
	MODULE_STATE_CONNECTED  = 0x04,
	MODULE_STATE_TERMINATED = 0x05,
}module_state;
/////////////////////////////////////////////////////////////////////////////////////////////////////

/*********************************************************************
 * GLOBAL VARIABLES
 */


/*********************************************************************
 * Public Functions
 */

void spp_init(void);
void spp_onModuleCmd(u8* p, int n);
u8	host_push_status (u16 st, int n, u8 *p);

///////////////////////////////8267_spi.h////////////////////////////////////////////
enum SPI_MODE{
	SPI_MODE0=0,
	SPI_MODE2,
	SPI_MODE1,
	SPI_MODE3,
};

enum SPI_PIN{
	SPI_PIN_GPIOA=0,
	SPI_PIN_GPIOB,
};
#define     spi_slave_rev_irq_en()        write_reg8(0x640, read_reg8(0x640)|0x80)
#define     spi_slave_rev_irq_dis()       write_reg8(0x640, read_reg8(0x640)&0x7f)
#define     spi_slave_rev_irq_clear()     write_reg8(0x22,read_reg8(0x22)|0x02)

void spi_pin_init_8267(enum SPI_PIN gpio_pin_x);

void spi_master_init_8267(unsigned char div_clock,enum SPI_MODE mode);
void spi_slave_init_8267(unsigned char div_clock,enum SPI_MODE mode);
#if 1
void spi_write_buff_8267(unsigned short addr ,unsigned char* pbuff,unsigned int len);
void spi_read_buff_8267(unsigned short addr,unsigned char* pbuff,unsigned int len);
#endif
/////////////////////////////////////////////////////////////////////////////////////

