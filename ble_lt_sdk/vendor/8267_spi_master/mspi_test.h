/*
 * mspi_test.h
 *
 *  Created on: 2017-2-8
 *      Author: Administrator
 */

#ifndef MSPI_TEST_H_
#define MSPI_TEST_H_

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

#endif /* MSPI_TEST_H_ */
