#include "../tl_common.h"
//#include "bsp.h"
#include "spi_8267.h"

void spi_pin_init_8267(enum SPI_PIN gpio_pin_x){
	if(gpio_pin_x == SPI_PIN_GPIOB){
		write_reg8(0x58e,read_reg8(0x58e)&0x0f);///disable GPIO:B<4~7>
		write_reg8(0x5b1,read_reg8(0x5b1)|0xf0);///enable SPI function:B<4~7>

		write_reg8(0x5b0,read_reg8(0x5b0)&0xC3);///disable SPI function:A<2~5>
		gpio_set_input_en(GPIO_PB4,1);
		gpio_set_input_en(GPIO_PB5,1);
		gpio_set_input_en(GPIO_PB6,1);
		gpio_set_input_en(GPIO_PB7,1);
	}
	if(gpio_pin_x == SPI_PIN_GPIOA){
		write_reg8(0x586,read_reg8(0x586)&0xC3);///disable GPIO:A<2~5>
		write_reg8(0x5b0,read_reg8(0x5b0)|0x3C);///enable SPI function:A<2~5>

		write_reg8(0x5b1,read_reg8(0x5b1)&0x0f);///disable SPI function:B<4~7>
		gpio_set_input_en(GPIO_PA2,1);
		gpio_set_input_en(GPIO_PA3,1);
		gpio_set_input_en(GPIO_PA4,1);
		gpio_set_input_en(GPIO_PA5,1);
	}	
	write_reg8(0x0a,read_reg8(0x0a)|0x80);////enable spi
}

void spi_master_init_8267(unsigned char div_clock,enum SPI_MODE mode){
	write_reg8(0x0a,read_reg8(0x0a)|div_clock);/////spi clock=system clock/((div_clock+1)*2)
	write_reg8(0x09,read_reg8(0x09)|0x02);////enable master mode
	
	write_reg8(0x0b,read_reg8(0x0b)|mode);////select SPI mode,surpport four modes
}

void spi_slave_init_8267(unsigned char div_clock,enum SPI_MODE mode){
	write_reg8(0x0a,read_reg8(0x0a)|div_clock);/////spi clock=system clock/((div_clock+1)*2)
	write_reg8(0x09,read_reg8(0x09)&0xfd);////disable master mode

	write_reg8(0x0b,read_reg8(0x0b)|mode);////select SPI mode,surpport four modes
}

#if 1
void spi_write_buff_8267(unsigned short addr ,unsigned char* pbuff,unsigned int len){
	unsigned int i = 0;
	write_reg8(0x09,read_reg8(0x09)&0xfe);////CS level is low
	write_reg8(0x09,read_reg8(0x09)&0xfb);///enable output
	write_reg8(0x09,read_reg8(0x09)&0xf7);///enable write
	
	/***send addr***/
	write_reg8(0x08,(addr>>8)&0xff);/////high addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	write_reg8(0x08,addr&0xff);/////low addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	
	/***send write command:0x00***/
	write_reg8(0x08,0x00);/////0x80:read  0x00:write
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	/***send data***/
	for(i=0;i<len;i++){
		write_reg8(0x08,pbuff[i]);
		while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	}
	/***pull up CS***/
	write_reg8(0x09,read_reg8(0x09)|0x01);///CS level is high
}


void spi_read_buff_8267(unsigned short addr,unsigned char* pbuff,unsigned int len){
	unsigned int i = 0;
	unsigned char temp = 0;
	
	write_reg8(0x09,read_reg8(0x09)&0xfe);////CS level is low
	write_reg8(0x09,read_reg8(0x09)&0xfb);///enable output

	/***send addr***/
	write_reg8(0x08,(addr>>8)&0xff);/////high addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	write_reg8(0x08,addr&0xff);/////low addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	
	/***send read command:0x80***/
	write_reg8(0x08,0x80);/////0x80:read  0x00:write
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status

	/***when the read_bit was set 1,you can read 0x800008 to take eight clock cycle***/
	write_reg8(0x09,read_reg8(0x09)|0x08);////set read_bit to 1
	temp = read_reg8(0x08);///first byte isn't useful data,only take 8 clock cycle
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	
	/***send one byte data to read data***/
	for(i=0;i<len;i++){
		pbuff[i] = read_reg8(0x08);///take 8 clock cycles
		while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	}
	/////pull up CS
	write_reg8(0x09,read_reg8(0x09)|0x01);///CS level is high
}
#endif
