
#pragma once

#include "../common/types.h"

#if(MCU_CORE_TYPE == MCU_CORE_5316)
	/* SPI Read/Write Command define. */
	#ifndef SPI_WRITE_CMD
	#define SPI_WRITE_CMD   0x00
	#endif
	#ifndef SPI_READ_CMD
	#define SPI_READ_CMD    0x80
	#endif
#endif

///spi irq handler
#define SPI_IRQ_EN()        BM_SET(reg_irq_mask, FLD_IRQ_HOST_CMD_EN)
#define SPI_IRQ_DIS()       BM_CLR(reg_irq_mask, FLD_IRQ_HOST_CMD_EN)
#define SPI_IRQ_GET()       ( (reg_spi_irq_status & FLD_SPI_STATUS_WR) ? 1: 0 )
#if(MCU_CORE_TYPE == MCU_CORE_5316)
	#define SPI_IRQ_CLR()   BM_SET(reg_spi_irq_status, FLD_SPI_STATUS_WR)
#else
	#define SPI_IRQ_CLR()   BM_SET(reg_spi_clr_status, FLD_SPI_STATUS_WR)
#endif

typedef void (*spi_callback_func)(u8 *);


enum spi_pin_t{
#if((MCU_CORE_TYPE == MCU_CORE_8266)||(MCU_CORE_TYPE == MCU_CORE_8261)||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269))
	SPI_PIN_GROUPA,
	SPI_PIN_GROUPB,
#elif (MCU_CORE_TYPE == MCU_CORE_5316)
	SPI_PIN_GROUPB,//SPI Master and Slave GPIO
	SPI_PIN_GROUPC,
#endif
};

enum spi_mode_t{
	SPI_MODE0 = 0,
	SPI_MODE1 = 2,
	SPI_MODE2 = 1,
	SPI_MODE3 = 3,
};

/****
* @brief: spi bus can drive more than one spi slave. so we can use cs line to select spi slave that response master.
*         the telink's chip can use normal gpio to as cs function, not only the CN pin of spi hardware module.
*         but if device act as spi slave,the cs pin must use the CN pin of spi hardware module.
*/
#if(MCU_CORE_TYPE == MCU_CORE_8266)
void spi_master_pin_init(unsigned int cs_pin);
#elif((MCU_CORE_TYPE == MCU_CORE_8261)||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269)||(MCU_CORE_TYPE == MCU_CORE_5316))
void spi_master_pin_init(enum spi_pin_t data_clk_pin, unsigned int cs_pin);
#endif

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] div_clk - the division factor for SPI module
 *            SPI clock = System clock / ((div_clk+1)*2); notice system clock should be at least 5x faster than spi clock.
 * @param[in] spi_mode - the selected working mode of SPI module. refer to datasheet for spi mode
 * @return    none
 */
void spi_master_init(unsigned char div_clk, enum spi_mode_t spi_mode);

/*****
*   @brief: when chip act as spi slave, the cs pin must be the pin of hardware module. i.e. 8266's E6 and 8267's A5/B4
*           spi slave get the interrupt from cs pin. so can not instead of the cs pin using the other normal gpio.
*/
#if(MCU_CORE_TYPE == MCU_CORE_8266)
void spi_slave_init(enum spi_mode_t spi_mode);
#elif((MCU_CORE_TYPE == MCU_CORE_8261)||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269)||(MCU_CORE_TYPE == MCU_CORE_5316))
void spi_slave_init(enum spi_pin_t spi_grp, enum spi_mode_t spi_mode);
#endif

/**
 * @brief      This function serves to write a bulk of data to the SPI slave
 *             device specified by the CS pin
 * @param[in]  addr_cmd - pointer to the command bytes needed written into the
 *             slave device first before the writing operation of actual data
 * @param[in]  cmd_len - length in byte of the command bytes
 * @param[in]  pbuf - pointer to the data need to write
 * @param[in]  buf_len - length in byte of the data need to write
 * @param[in]  cs_pin - the CS pin specifing the slave device
 * @return     none
 * @ timing chart: addr(0or1) + cmd_0x00 + pbuf[0]+ ... + pbuf[buf_len]
 * for example : addr_cmd[3] = {0x80,0x00,0x00} or addr_cmd[3]={0x80,0x00,0x80}
 *               the first two bytes indicate slave address. the third bytes is the command that 0x00 indicate write and 0x80 read.
 */
void spi_write(unsigned char* addr_cmd, unsigned char addr_cmd_len, unsigned char* pbuf, int buf_len, unsigned int cs_pin);

/**
 * @brief      This function serves to read a bulk of data from the SPI slave
 *             device specified by the CS pin
 * @param[in]  addr_cmd - pointer to the command bytes needed written into the
 *             slave device first before the reading operation of actual data
 * @param[in]  addr_cmd_len - length in byte of the command bytes
 * @param[out] pbuf - pointer to the buffer that will cache the reading out data
 * @param[in]  buf_len - length in byte of the data need to read
 * @param[in]  cs_pin - the CS pin specifing the slave device
 * @return     none
 */
void spi_read(unsigned char* addr_cmd, unsigned char addr_cmd_len, unsigned char* pbuf, int buf_len, unsigned int cs_pin);


/**
 * These functions are defined for convenience of using SPI and only open to chip,5316.
 */
#if(MCU_CORE_TYPE == MCU_CORE_5316)
void spi_writeEx(unsigned short addr,unsigned char addrLen, unsigned char* pBuf, unsigned int len);
void spi_ReadEx(unsigned short addr, unsigned char addrLen, unsigned char* pBuf, unsigned int len);
#endif
