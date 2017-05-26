/*
 * mouse_spi.h
 *
 *  Created on: 2017-3-11
 *      Author: Administrator
 */

#ifndef MOUSE_SPI_H_
#define MOUSE_SPI_H_

#include "../../proj/tl_common.h"


#if(!TELINK_MOUSE_DEMO)
#define 	SPI_CS   	GPIO_PB4
#define		SPI_SCLK 	GPIO_PA4
#define		SPI_MOSI	GPIO_PB6
#define		SPI_MISO	GPIO_PB5
#define		GPIO_MOTION_PIN GPIO_PA7
#define 	GPIO_RESET_PIN GPIO_PA3
#endif


#ifndef GPIO_MOTION_PIN
#define GPIO_MOTION_PIN		GPIO_PE0
#endif
#ifndef GPIO_RESET_PIN
#define GPIO_RESET_PIN		GPIO_PE1
#endif



#ifndef SPI_SCLK
#define SPI_SCLK	GPIO_PC2
#endif


#ifndef	SPI_MOSI
#define SPI_MOSI	GPIO_PC3
#endif

#ifndef	SPI_MISO
#define SPI_MISO	GPIO_PC4
#endif

#ifndef	SPI_CS
#define SPI_CS		GPIO_PC5
#endif




#define SPI_SPEED_100K	100000
#define SPI_SPEED_200K	200000
#define SPI_SPEED_500K	500000
#define SPI_SPEED_1M	1000000

#define SPI_SPEED		SPI_SPEED_500K
#define SPI_BIT_INTERVAL_US	( (CLOCK_SYS_CLOCK_HZ/SPI_SPEED) >> 4)


int mouse_spi_init(void);
int mouse_spi_write_byte(char byte);
u8 mouse_spi_read_byte(char data);

u8 mouse_spi_ReadRegister(char addr);
int mouse_spi_WriteRegister(char addr, char byte);


#endif /* MOUSE_SPI_H_ */
