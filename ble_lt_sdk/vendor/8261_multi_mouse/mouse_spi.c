/*
 * mouse_spi.c
 *
 *  Created on: 2017-3-11
 *      Author: Administrator
 */

/*
 *	This file is created for communication with PWM3360 sensor.
 * 	Four wire SPI protocol
 * 	CS, SCLK, MOSI, MISO
 *
 */



#include "mouse_spi.h"

#define 	spi_mosi_low		gpio_write(SPI_MOSI, 0)
#define 	spi_mosi_high		gpio_write(SPI_MOSI, 1)

#define 	spi_sclk_high		gpio_write(SPI_SCLK, 1)
#define		spi_sclk_low		gpio_write(SPI_SCLK, 0)

#define 	spi_cs_high			gpio_write(SPI_CS, 1)
#define		spi_cs_low			gpio_write(SPI_CS, 0)



/**********************************************************************
 * @Function: mouse_spi_init()
 * @Description: Initialize the spi interference
 * @Param £º None
 *
**********************************************************************/
int mouse_spi_init(void)
{

	gpio_set_output_en(SPI_CS, 1);

	gpio_set_output_en(SPI_SCLK, 1);		//SCLK only output enable
	gpio_set_output_en(SPI_MOSI, 1);		//MOSI only output enable
	gpio_set_input_en(SPI_MISO, 1);			//MISO only input enable

	spi_cs_low;							    //CS PULL¡¡UP
	spi_sclk_high;							//SCLK PULL UP

	return 1;
}


int mouse_spi_write_byte(char byte)
{
	u8 temp = 0x80;
	while(temp){

		spi_sclk_low;				//failing edge, start to write
		if(temp & byte){
			spi_mosi_high;
		}
		else{
			spi_mosi_low;
		}
		sleep_us(SPI_BIT_INTERVAL_US);
		spi_sclk_high;			   		//rising edge, hold on
		sleep_us(SPI_BIT_INTERVAL_US);

		temp >>= 1;
	}

	return 1;
}

u8 mouse_spi_read_byte(char data)
{
	u8 dat = 0;

	for(int i=7; i>=0; i--){
		spi_sclk_low;				//failing edge, hold on
		sleep_us(SPI_BIT_INTERVAL_US);
		spi_sclk_high;				//rising edge, start to read
		if(gpio_read(SPI_MISO)){
			dat |= (1<<i);
		}
		sleep_us(SPI_BIT_INTERVAL_US);
	}
	return dat;
}

u8 mouse_spi_ReadRegister(char addr)
{
	spi_cs_low;						//cs pull down, spi start
	addr &= 0x7f;			//write command
	mouse_spi_write_byte(addr);
	sleep_us(2);			//between write to read command
	mouse_spi_read_byte(addr);
	spi_cs_high;
}

int mouse_spi_WriteRegister(char addr, char byte)
{
	spi_cs_low;						//cs pull down, spi start
	addr |= 0x80;			//read command
	mouse_spi_write_byte(addr);
	sleep_us(2);
	mouse_spi_write_byte(byte);
	spi_cs_high;
	return 1;
}
