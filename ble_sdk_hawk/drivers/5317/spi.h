#pragma once

#ifndef _SPI_H
#define _SPI_H

#include "driver_config.h"
#include "register.h"
#include "gpio.h"

#if(MCU_CORE_TYPE == MCU_CORE_5316 || MCU_CORE_TYPE == MCU_CORE_5317 )
	/* SPI Read/Write Command define. */
	#ifndef SPI_WRITE_CMD
	#define SPI_WRITE_CMD   0x00
	#endif
	#ifndef SPI_READ_CMD
	#define SPI_READ_CMD    0x80
	#endif
#endif

/* SPI irq handler */
#define SPI_IRQ_EN()      BM_SET(reg_irq_mask, FLD_IRQ_HOST_CMD_EN)
#define SPI_IRQ_DIS()     BM_CLR(reg_irq_mask, FLD_IRQ_HOST_CMD_EN)

#define SPI_GET_IRQ_FLAG()     ( (reg_spi_irq_status & FLD_SPI_STATUS_WR) ? 1: 0 )
#define SPI_CLR_IRQ_FLAG()     BM_SET(reg_spi_irq_status, FLD_SPI_STATUS_WR)

#define SPI_BUSY_FLAG          ((reg_spi_ctrl & FLD_SPI_BUSY)?1:0)

typedef enum{
	SPI_PIN_GROUPAB,//SPI Master
	SPI_PIN_GROUPC,//SPI Master and Slave GPIO
}SPI_PinTypeDef;

typedef enum{
	SPI_MODE0 = 0,
	SPI_MODE1 = 2,
	SPI_MODE2 = 1,
	SPI_MODE3 = 3,
}SPI_ModeTypDef;

/**
 *  @brief  Define the clk for SPI interface(system clock = 16M)
 */
enum {
    SPI_16M_CLK_200K = 0x27,
    SPI_16M_CLK_250K = 0x1f,
    SPI_16M_CLK_500K = 0x0f,
    SPI_16M_CLK_1M   = 0x07,
    SPI_16M_CLK_2M   = 0x03,
    SPI_16M_CLK_4M   = 0x01,
};

/**
 *  @brief  Define the clk for SPI interface(system clock = 24M)
 */
enum {
    SPI_24M_CLK_200K = 0x3c,
    SPI_24M_CLK_250K = 0x2e,
    SPI_24M_CLK_500K = 0x17,
    SPI_24M_CLK_1M   = 0x0b,
    SPI_24M_CLK_2M   = 0x05,
    SPI_24M_CLK_4M   = 0x02,
};

/* !!!Caution: Master GND must link with Slave GND. */
void spi_master_pin_init(SPI_PinTypeDef spi_pin, GPIO_PinTypeDef cs_pin);
void spi_slave_pin_init(SPI_PinTypeDef spi_pin );

void spi_master_init(unsigned char div_clk, SPI_ModeTypDef spi_mode);
void spi_slave_init(SPI_ModeTypDef spi_mode);

/**
 * These functions are defined for convenience of using SPI and only open to 5316 or 5317.
 */
void spi_write(unsigned short addr,unsigned char addrLen, unsigned char* pBuf, unsigned int len);
void spi_read(unsigned short addr, unsigned char addrLen, unsigned char* pBuf, unsigned int len);
#endif

