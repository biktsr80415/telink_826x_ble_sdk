#include "../tl_common.h"
#include "spi.h"

#if(MCU_CORE_TYPE != MCU_CORE_8263)

/****
* @brief: spi bus can drive more than one spi slave. so we can use cs line to select spi slave that response master.
*         the telink's chip can use normal gpio to as cs function, not only the CN pin of spi hardware module.
*         but if device act as spi slave,the cs pin must use the CN pin of spi hardware module.
*/
#if(MCU_CORE_TYPE == MCU_CORE_8266)
void spi_master_pin_init(unsigned int cs_pin)
#elif((MCU_CORE_TYPE == MCU_CORE_8261)||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269))
void spi_master_pin_init(enum spi_pin_t data_clk_pin, unsigned int cs_pin)
#endif 
{
	#if(MCU_CORE_TYPE == MCU_CORE_8266)
		//disable the other function and the gpio will be spi.
		reg_gpio_config_func4 &= (~BIT_RNG(6,7));     //disable E6/E7 keyscan function
		reg_gpio_config_func5 &= (~BIT(5));           //disable E6/F0 as uart function
		gpio_set_func(GPIO_PE7,AS_SPI);               //disable E7 as gpio
		gpio_set_func(GPIO_PF0|GPIO_PF1,AS_SPI);      //disable F0/F1 as gpio
		gpio_set_input_en(GPIO_PE6|GPIO_PE7,1);       //enable input
		gpio_set_input_en(GPIO_PF0|GPIO_PF1,1);       //enable input
	#elif((MCU_CORE_TYPE == MCU_CORE_8261)||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269))
		if(data_clk_pin == SPI_PIN_GROUPB)
		{
			gpio_set_func(GPIO_PB5|GPIO_PB6|GPIO_PB7,AS_SPI); //disable gpio function
			reg_gpio_config_func1 |= (FLD_SPI_DO_PWM4N|FLD_SPI_DI_PWM5|FLD_SPI_CK_PWM5N); //enable B4/B5/B6 spi function
			reg_gpio_config_func0 &= (~BIT_RNG(2,5));        //disable A2/A3/A4/A5 as spi function
			gpio_set_input_en(GPIO_PB5|GPIO_PB6|GPIO_PB7,1); //enable input function
		}
		else if(data_clk_pin == SPI_PIN_GROUPA)
		{
			gpio_set_func(GPIO_PA2|GPIO_PA3|GPIO_PA4,AS_SPI); //disable gpio function
			reg_gpio_config_func0 |= (FLD_SPI_DO_PWM0_N|FLD_SPI_DI_PWM1|FLD_SPI_CK_PWM1_N);//enable spi function
			reg_gpio_config_func1 &= (~BIT_RNG(4,7)); //disable B4/B5/B6/B7 as spi function.
			gpio_set_func(GPIO_PB4|GPIO_PB5|GPIO_PB6|GPIO_PB7,AS_GPIO); //enable B4/B5/B6/B7 gpio function,or they will be pwm function
			gpio_set_input_en(GPIO_PA2|GPIO_PA3|GPIO_PA4,1); //enable input
		}
	#endif
	gpio_set_func(cs_pin,1);      //cs pin as gpio function
	gpio_set_input_en(cs_pin,0);  //disable input
	gpio_write(cs_pin,1);         // output high level in idle status.
	gpio_set_output_en(cs_pin,1); //enable output
	
	reg_spi_sp |= FLD_SPI_ENABLE; //enable spi function. because i2c and spi share part of the hardware in the chip.
}
/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] div_clk - the division factor for SPI module
 *            SPI clock = System clock / ((div_clk+1)*2); notice system clock should be at least 5x faster than spi clock.
 * @param[in] spi_mode - the selected working mode of SPI module. refer to datasheet for spi mode
 * @return    none
 */
void spi_master_init(unsigned char div_clk, enum spi_mode_t spi_mode)
{
	/***set the spi clock. spi_clk = system_clock/((div_clk+1)*2)***/
	reg_spi_sp &= (~FLD_MASTER_SPI_CLK);     //clear the spi clock division bits
	reg_spi_sp |= (div_clk&0x7f);            //set the clock div bits
	reg_spi_ctrl |= FLD_SPI_MASTER_MODE_EN;  //enable spi master mode
	/***config the spi woking mode.For spi mode spec, pls refer to datasheet***/
	reg_spi_inv_clk &= (~BIT_RNG(0,1));      //clear the mode bits
	reg_spi_inv_clk |= (spi_mode&0x03);      //set the mode
}

/*****
*   @brief: when chip act as spi slave, the cs pin must be the pin of hardware module. i.e. 8266's E6 and 8267's A5/B4
*           spi slave get the interrupt from cs pin. so can not instead of the cs pin using the other normal gpio.
*/
#if(MCU_CORE_TYPE == MCU_CORE_8266)
void spi_slave_init(enum spi_mode_t spi_mode)
#elif((MCU_CORE_TYPE == MCU_CORE_8261)||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269))
void spi_slave_init(enum spi_pin_t spi_grp, enum spi_mode_t spi_mode)
#endif
{
	/***config the corresponding gpio as spi and enable or disable other function***/
	#if(MCU_CORE_TYPE == MCU_CORE_8266)

	#elif((MCU_CORE_TYPE == MCU_CORE_8261)||(MCU_CORE_TYPE == MCU_CORE_8267)||(MCU_CORE_TYPE == MCU_CORE_8269))
		if(spi_grp == SPI_PIN_GROUPB){
			gpio_set_func(GPIO_PB4|GPIO_PB5|GPIO_PB6|GPIO_PB7,AS_SPI); //disable gpio function
			reg_gpio_config_func1 |= (FLD_SPI_CN_PWM4|FLD_SPI_DO_PWM4N|FLD_SPI_DI_PWM5|FLD_SPI_CK_PWM5N); //enable B4/B5/B6 spi function
			reg_gpio_config_func0 &= (~BIT_RNG(2,5));                 //disable A2/A3/A4/A5 as spi function
			gpio_set_input_en(GPIO_PB4|GPIO_PB5|GPIO_PB6|GPIO_PB7,1); //enable input
		}
		else if(spi_grp == SPI_PIN_GROUPA){
			gpio_set_func(GPIO_PA2|GPIO_PA3|GPIO_PA4|GPIO_PA5,AS_SPI); //disable gpio function
			reg_gpio_config_func0 |= (FLD_SPI_DO_PWM0_N|FLD_SPI_DI_PWM1|FLD_SPI_CK_PWM1_N|FLD_SPI_CN_PWM2_N);//enable spi function
			reg_gpio_config_func1 &= (~BIT_RNG(4,7)); //disable B4/B5/B6 as spi function.
			gpio_set_func(GPIO_PB4|GPIO_PB5|GPIO_PB6|GPIO_PB7,AS_GPIO); //enable B4/B5/B6/B7 gpio function,or they will be pwm function
			gpio_set_input_en(GPIO_PA2|GPIO_PA3|GPIO_PA4,1); //enable input
		}
	#endif
	/***enable slave***/
	reg_spi_ctrl &= (~FLD_SPI_MASTER_MODE_EN);  //disable spi master mode, ie enable spi slave mode
	/***config the spi woking mode.For spi mode spec, pls refer to datasheet***/
	reg_spi_inv_clk &= (~BIT_RNG(0,1));      //clear the mode bits
	reg_spi_inv_clk |= (spi_mode&0x03);      //set the mode
}

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
void spi_write(unsigned char* addr_cmd, unsigned char addr_cmd_len, unsigned char* pbuf, int buf_len, unsigned int cs_pin)
{
	int i = 0;
	/***pull down cs line and enable write***/
	gpio_write(cs_pin, 0); //pull down cs line and select the slave to handle.
	reg_spi_ctrl &= (~FLD_SPI_DATA_OUT_DIS); //enable output
	reg_spi_ctrl &= (~FLD_SPI_RD);           //enable write
	/***write cmd,refer to datasheet, cmd_0x00 is write; cmd_0x80 is read***/
	for(i=0;i<addr_cmd_len;i++){
		reg_spi_data = addr_cmd[i];
		while(reg_spi_ctrl & FLD_SPI_BUSY);
	}
	/***write data to slave****/
	for(i=0;i<buf_len;i++){
		reg_spi_data = pbuf[i];
		while(reg_spi_ctrl & FLD_SPI_BUSY); //wait data sending
	}
	/***pull up cs line to release the slave***/
	gpio_write(cs_pin, 1);
}

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
void spi_read(unsigned char* addr_cmd, unsigned char addr_cmd_len, unsigned char* pbuf, int buf_len, unsigned int cs_pin)
{
	int i = 0;
	unsigned char temp_spi_data = 0;
	/***pull down cs line and enable write***/
	gpio_write(cs_pin, 0); //pull down cs line and select the slave to handle.
	reg_spi_ctrl &= (~FLD_SPI_DATA_OUT_DIS); //enable output
	reg_spi_ctrl &= (~FLD_SPI_RD);           //enable write
	
	/***write cmd,refer to datasheet, cmd_0x00 is write; cmd_0x80 is read***/
	for(i=0;i<addr_cmd_len;i++){
		reg_spi_data = addr_cmd[i];
		while(reg_spi_ctrl & FLD_SPI_BUSY);
	}
	
	reg_spi_ctrl |= (FLD_SPI_RD|FLD_SPI_DATA_OUT_DIS); //enable read and disable output
	temp_spi_data = reg_spi_data;
	while(reg_spi_ctrl & FLD_SPI_BUSY);
	
	/***read the data.when read register reg_sip_data(0x08),the scl will generate 8 clock cycles to get the data from slave***/
	for(i=0;i<buf_len;i++){
		pbuf[i] = reg_spi_data;
		while(reg_spi_ctrl & FLD_SPI_BUSY);
	}
	/***pull up cs line to release the slave***/
	gpio_write(cs_pin, 1);
}

#endif
