#pragma once

#ifndef __SPI_H
#define __SPI_H

#include "register.h"
#include "gpio.h"

/* SPI Read/Write Command define. */
#ifndef SPI_WRITE_CMD
	#define SPI_WRITE_CMD   0x00
#endif
#ifndef SPI_READ_CMD
	#define SPI_READ_CMD    0x80
#endif

/* SPI irq Macro */
#define SPI_IRQ_EN()      BM_SET(reg_irq_mask, FLD_IRQ_HOST_CMD_EN)
#define SPI_IRQ_DIS()     BM_CLR(reg_irq_mask, FLD_IRQ_HOST_CMD_EN)

#define SPI_IRQ_GET()     ( (reg_spi_irq_status & FLD_SPI_STATUS_WR) ? 1: 0 )
#define SPI_IRQ_CLR()     BM_SET(reg_spi_irq_status, FLD_SPI_STATUS_WR)

#define SPI_BUSY_FLAG     ((reg_spi_ctrl & FLD_SPI_BUSY)?1:0)

typedef enum{
	SPI_PIN_GROUPB,//SPI Master and Slave GPIO
	SPI_PIN_GROUPC,
}eSPI_PinTypeDef;

typedef enum{
	SPI_MODE0 = 0,
	SPI_MODE1 = 2,
	SPI_MODE2 = 1,
	SPI_MODE3 = 3,
}eSPI_ModeTypDef;

/* !!!Caution: Master GND must link with Slave GND. */
void spi_master_pin_init(eSPI_PinTypeDef spi_pin, eGPIO_PinTypeDef cs_pin);
void spi_slave_pin_init( eSPI_PinTypeDef spi_pin );

void spi_master_init(unsigned char div_clk, eSPI_ModeTypDef spi_mode);
void spi_slave_init(eSPI_ModeTypDef spi_mode);

/**
 * These functions are defined for convenience of using SPI and only open to 5316 or 5317.
 */
void spi_write(unsigned short addr,unsigned char addrLen, unsigned char* pBuf, unsigned int len);
void spi_read(unsigned short addr, unsigned char addrLen, unsigned char* pBuf, unsigned int len);

#endif /* End of __SPI_H */

