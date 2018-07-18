#pragma once

#include "driver_config.h"
#include "register.h"

typedef enum {
	I2C_PIN_GROUP_M_A1A2 = 0x00,//IIC Master GPIO
	I2C_PIN_GROUP_M_A5A6 = 0x01,//(A5A6目前不能使用)
	I2C_PIN_GROUP_M_C4C5 = 0x02,

	I2C_PIN_GROUP_S_A5A6 = 0x03,//IIC slave GPIO (A5A6目前不能使用)
	I2C_PIN_GROUP_S_B6B7 = 0x04,
	I2C_PIN_GROUP_S_C4C5 = 0x05,
}eI2C_PinTypeDef;

//I2C irq handler
//typedef enum {
//	I2C_IRQ_NONE = 0,
//	I2C_IRQ_HOST_WRITE_ONLY,
//	I2C_IRQ_HOST_READ_ONLY,
//}eI2C_IrqSrcTypeDef;

/* IIC irq Flag define */
typedef enum{
	I2C_IrqFlag_Read  = FLD_I2C_STATUS_RD,
	I2C_IrqFlag_Write = FLD_I2C_STATUS_WR,
}eI2C_IrqFlagTypeDef;

typedef enum{
	I2C_SlaveMode_DMA = 0x00,
	I2C_SlaveMode_MAP = 0x01,

	/* Reserved for compatibility */
//	I2C_SLAVE_DMA = 0,
//	I2C_SLAVE_MAP = 1,
}eI2C_SlaveModeTypeDef;

#define I2C_IRQ_EN()    BM_SET(reg_irq_mask, FLD_IRQ_HOST_CMD_EN)
#define I2C_IRQ_DIS()   BM_CLR(reg_irq_mask, FLD_IRQ_HOST_CMD_EN)

#define I2C_CMD_BUSY_FLAG		((reg_i2c_status & FLD_I2C_CMD_BUSY)? 1:0)

#define I2C_MODULE_RESET()      do{\
									BM_SET(reg_rst0, FLD_RST0_I2C);\
									BM_CLR(reg_rst0, FLD_RST0_I2C);\
								  }while(0)

void i2c_pin_init(eI2C_PinTypeDef i2c_pin_group);

void i2c_master_init_div(unsigned char slave_id, unsigned char div_clock);
void i2c_master_init_khz(unsigned char slave_id, unsigned int i2c_speed);
void i2c_slave_init(unsigned char device_id, eI2C_SlaveModeTypeDef i2c_slave_mode,
		            unsigned char* pbuf);

/* Use for DMA/MAP mode of I2C Slave */
void i2c_write_byte(unsigned int addr, unsigned int addr_len, unsigned char data);
unsigned char i2c_read_byte(unsigned int addr, unsigned char addr_len);
void i2c_burst_write(unsigned short addr, unsigned char addr_len, unsigned char* pbuf,
		             unsigned int len);
void i2c_burst_read(unsigned short addr, unsigned char addr_len, unsigned char* pbuf,
		            unsigned int len);

/* Only use for I2C slave */
#if 1
unsigned char i2c_get_irq_flag(eI2C_IrqFlagTypeDef I2C_IrqFlag);
void i2c_clear_irq_flag(eI2C_IrqFlagTypeDef I2C_IrqFlag);
#else
eI2C_IrqSrcTypeDef i2c_get_slave_irq_flag(void);
void i2c_clear_slave_irq_flag(eI2C_IrqSrcTypeDef I2C_IrqSrc);
#endif


