
#include "spi.h"
#include "register.h"
#include "gpio.h"

static eGPIO_PinTypeDef SpiCsPin = GPIO_NONE;/* <! Store SPI Master CS pin. */

/**
* @brief: spi bus can drive more than one spi slave. so we can use cs line to select spi slave that response master.
*         the telink's chip can use normal gpio to as cs function, not only the CN pin of spi hardware module.
*         but if device act as spi slave,the cs pin must use the CN pin of spi hardware module.
*/
void spi_master_pin_init(eSPI_PinTypeDef spi_pin, eGPIO_PinTypeDef cs_pin)
{
	if(spi_pin == SPI_PIN_GROUPB)
	{
		//Disable GPIO fucntion of PB1 PB2 and PB3.
		gpio_set_func(GPIO_PB1,AS_SPI);//MDO
		gpio_set_func(GPIO_PB2,AS_SPI);//MDI
		gpio_set_func(GPIO_PB3,AS_SPI);//MCK

		//Set PB1 PB2 and PB3 as SPI.
		GPIOB_AF->RegBits.P1_AF = GPIOB1_SPI_MDO;
		GPIOB_AF->RegBits.P2_AF = GPIOB2_SPI_MDI;
		GPIOB_AF->RegBits.P3_AF = GPIOB3_SPI_MCK;

		//must
		gpio_set_data_strength(GPIO_PB1,0);
		gpio_set_data_strength(GPIO_PB2,0);
		gpio_set_data_strength(GPIO_PB3,0);

		//Disable SPI function of PC2 PC3 PC4 PC5.
		if(GPIOC_AF->RegBits.P2_AF == GPIOC2_SPI_MCN)
		{
			gpio_set_func(GPIO_PC2,AS_GPIO);//MCN
		}
		if(GPIOC_AF->RegBits.P3_AF == GPIOC3_SPI_MDO)
		{
			gpio_set_func(GPIO_PC3,AS_GPIO);//MDO
		}
		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_SPI_MDI)
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);//MDI
		}
		if(GPIOC_AF->RegBits.P5_AF == GPIOC5_SPI_MCK)
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);//MCK
		}
	}
	else if(spi_pin == SPI_PIN_GROUPC)
	{
		//Disable GPIO fucntion of PB1 PB2 and PB3.
		gpio_set_func(GPIO_PC3,AS_SPI);//MDO
		gpio_set_func(GPIO_PC4,AS_SPI);//MDI
		gpio_set_func(GPIO_PC5,AS_SPI);//MCK

		//Set  PC3 PC4 and PC5 as SPI.
		GPIOC_AF->RegBits.P3_AF = GPIOC3_SPI_MDO;
		GPIOC_AF->RegBits.P4_AF = GPIOC4_SPI_MDI;
		GPIOC_AF->RegBits.P5_AF = GPIOC5_SPI_MCK;

		//must
		gpio_set_data_strength(GPIO_PC3,0);
		gpio_set_data_strength(GPIO_PC4,0);
		gpio_set_data_strength(GPIO_PC5,0);

		//Disable SPI function of PB0 PB1 PB2 and PB3.
		if(GPIOB_AF->RegBits.P0_AF == GPIOB0_SPI_MCN)
		{
			gpio_set_func(GPIO_PB0,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P1_AF == GPIOB1_SPI_MDO)
		{
			gpio_set_func(GPIO_PB1,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P2_AF == GPIOB2_SPI_MDI)
		{
			gpio_set_func(GPIO_PB2,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P3_AF == GPIOB3_SPI_MCK)
		{
			gpio_set_func(GPIO_PB3,AS_GPIO);
		}
	}

	//Set CS pin
	SpiCsPin = (eGPIO_PinTypeDef)cs_pin;

	gpio_set_func(cs_pin, AS_GPIO); //cs pin as gpio function
	gpio_set_input_en(cs_pin,0); //disable input
	gpio_setup_up_down_resistor(cs_pin,PM_PIN_PULLUP_10K);
	gpio_write(cs_pin,1); // output high level in idle status.
	gpio_set_output_en(cs_pin,1); //enable output
}


void spi_slave_pin_init(eSPI_PinTypeDef spi_pin)
{
	if(spi_pin == SPI_PIN_GROUPB)/* SPI slave GPIO settings. */
	{
		//Disable GPIO fucntion of PB0 PB1 PB2 and PB3.
		gpio_set_func(GPIO_PB0,AS_SPI);//CN
		gpio_set_func(GPIO_PB1,AS_SPI);//DO
		gpio_set_func(GPIO_PB2,AS_SPI);//DI
		gpio_set_func(GPIO_PB3,AS_SPI);//CK

		//Enable GPIO(SPI) Input.(must)
		gpio_set_input_en(GPIO_PB0,1);
		gpio_set_input_en(GPIO_PB1,1);
		gpio_set_input_en(GPIO_PB2,1);
		gpio_set_input_en(GPIO_PB3,1);

		//Set PB0 PB1 PB2 and PB3 as SPI slave function.
		GPIOB_AF->RegBits.P0_AF = GPIOB0_RX_CYC2LNA_OR_SPI_CN;
		GPIOB_AF->RegBits.P1_AF = GPIOB1_TX_CYC2LNA_OR_SPI_DO;
		GPIOB_AF->RegBits.P2_AF = GPIOB2_UART_CTS_OR_SPI_DI;
		GPIOB_AF->RegBits.P3_AF = GPIOB3_UART_RTS_OR_SPI_CK;

		//must
		gpio_set_data_strength(GPIO_PB0,0);
		gpio_set_data_strength(GPIO_PB1,0);
		gpio_set_data_strength(GPIO_PB2,0);
		gpio_set_data_strength(GPIO_PB3,0);

		reg_gpio_pb_multi_func_select &= ~FLD_PB_MULTI_FUNC_SEL;//must

		//Disable SPI function of PC2 PC3 PC4 PC5.
		if(GPIOC_AF->RegBits.P2_AF == GPIOC2_SPI_CN)
		{
			gpio_set_func(GPIO_PC2,AS_GPIO);//CN
		}
		if(GPIOC_AF->RegBits.P3_AF == GPIOC3_SPI_DO)
		{
			gpio_set_func(GPIO_PC3,AS_GPIO);//DO
		}
		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_SPI_DI_OR_I2C_SD)
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);//DI
		}
		if(GPIOC_AF->RegBits.P5_AF == GPIOC5_SPI_CK_OR_I2C_CK)
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);//CK
		}
	}
	else if(spi_pin == SPI_PIN_GROUPC)
	{
		//Disable GPIO fucntion of PC2 PC3 PC4 and PC5.
		gpio_set_func(GPIO_PC2,AS_SPI);//CN
		gpio_set_func(GPIO_PC3,AS_SPI);//DO
		gpio_set_func(GPIO_PC4,AS_SPI);//DI
		gpio_set_func(GPIO_PC5,AS_SPI);//CK

		//Enable GPIO(SPI) Input.(must)
		gpio_set_input_en(GPIO_PC2,1);
		gpio_set_input_en(GPIO_PC3,1);
		gpio_set_input_en(GPIO_PC4,1);
		gpio_set_input_en(GPIO_PC5,1);

		//Set PC2 PC3 PC4 and PC5 as SPI.
		GPIOC_AF->RegBits.P2_AF = GPIOC2_SPI_CN;
		GPIOC_AF->RegBits.P3_AF = GPIOC3_SPI_DO;
		GPIOC_AF->RegBits.P4_AF = GPIOC4_SPI_DI_OR_I2C_SD;
		GPIOC_AF->RegBits.P5_AF = GPIOC5_SPI_CK_OR_I2C_CK;

		//must
		gpio_set_data_strength(GPIO_PC2,0);
		gpio_set_data_strength(GPIO_PC3,0);
		gpio_set_data_strength(GPIO_PC4,0);
		gpio_set_data_strength(GPIO_PC5,0);

		//Disable SPI function of PB0 PB1 PB2 and PB3.
		if(GPIOB_AF->RegBits.P0_AF == GPIOB0_RX_CYC2LNA_OR_SPI_CN)
		{
			gpio_set_func(GPIO_PB0,AS_GPIO);//CN
		}
		if(GPIOB_AF->RegBits.P1_AF == GPIOB1_TX_CYC2LNA_OR_SPI_DO)
		{
			gpio_set_func(GPIO_PB1,AS_GPIO);//DO
		}
		if(GPIOB_AF->RegBits.P2_AF == GPIOB2_UART_CTS_OR_SPI_DI)
		{
			gpio_set_func(GPIO_PB2,AS_GPIO);//DI
		}
		if(GPIOB_AF->RegBits.P3_AF == GPIOB3_UART_RTS_OR_SPI_CK)
		{
			gpio_set_func(GPIO_PB3,AS_GPIO);//CK
		}
	}
}

/**
 * @brief     This function configures the clock and working mode for SPI interface
 * @param[in] div_clk - the division factor for SPI module
 *            SPI clock = System clock / ((div_clk+1)*2); notice system clock should be at least 5x faster than spi clock.
 * @param[in] spi_mode - the selected working mode of SPI module. refer to datasheet for spi mode
 * @return    none
 */
void spi_master_init(unsigned char div_clk, eSPI_ModeTypDef spi_mode)
{
	BM_SET(reg_spi_sp, FLD_SPI_ENABLE);  //enable spi function. because i2c and spi share part of the hardware in the chip.

	/***set the spi clock. spi_clk = system_clock/((div_clk+1)*2)***/
	BM_CLR(reg_spi_sp, FLD_MASTER_SPI_CLK);  //clear the spi clock division bits
	reg_spi_sp |= MASK_VAL(FLD_MASTER_SPI_CLK, div_clk&0x7f); //set the clock div bits

	//Enable SPI peripheral clock
	reg_rst0 &= ~FLD_RST_SPI;
	reg_clk_en0 |= FLD_CLK_EN0_SPI_EN;

	//Enable SPI Master.
	reg_spi_ctrl &= ~(FLD_SPI_MASTER_EN|FLD_SPI_SLAVE_EN);
	reg_spi_ctrl |= FLD_SPI_MASTER_EN;

	/***config the spi woking mode.For spi mode spec, pls refer to datasheet***/
	BM_CLR(reg_spi_inv_clk, FLD_INVERT_SPI_CLK|FLD_DAT_DLY_HALF_CLK);//clear the mode bits

	BM_SET(reg_spi_inv_clk, spi_mode&0x03);  //set the mode
}

/**
 * @brief: when chip act as spi slave, the cs pin must be the pin of hardware module. i.e. 8266's E6 and 8267's A5/B4
 *           spi slave get the interrupt from cs pin. so can not instead of the cs pin using the other normal gpio.
 */
void spi_slave_init(eSPI_ModeTypDef spi_mode)
{
	//Enable SPI peripheral clock
	reg_rst0 &= ~FLD_RST_SPI;
	reg_clk_en0 |= FLD_CLK_EN0_SPI_EN;

	//Enable SPI slave.
	reg_spi_ctrl &= ~(FLD_SPI_MASTER_EN|FLD_SPI_SLAVE_EN);
	reg_spi_ctrl |= FLD_SPI_SLAVE_EN;

	BM_SET(reg_spi_sp, FLD_SPI_ENABLE); //enalbe spi function

	/***config the spi woking mode.For spi mode spec, pls refer to datasheet***/
	BM_CLR(reg_spi_inv_clk, FLD_INVERT_SPI_CLK|FLD_DAT_DLY_HALF_CLK); //clear the mode bits
	BM_SET(reg_spi_inv_clk, spi_mode);                         //set the mode
}

/**---------------------------------Author£º Gaoqiu------------------------------
 * @Brief:  SPI send data.
 * @Param:  addr    ->
 * @Param:  addrLen -> max = 2,min = 1.
 * @Param:  pBuf    ->
 * @Param:  len     ->
 * @Return: None.
 */
void spi_write(unsigned short addr,unsigned char addrLen, unsigned char* pBuf, unsigned int len)
{
	unsigned int i = 0;

	if(SpiCsPin == GPIO_NONE || addrLen > 2 || addrLen == 0)
		return;

	/* Set CS signal as low level. */
	gpio_write(SpiCsPin,0);

	/* Enable SPI data output and SPI write command. */
	reg_spi_ctrl &= ~(FLD_SPI_DATA_OUT_DIS|FLD_SPI_RD);

	/* Send Address. */
	if(addrLen == 1)
	{
		reg_spi_data = addr & 0xff;
		while(SPI_BUSY_FLAG);
	}
	else
	{
		//Write Address High bit first.
		reg_spi_data = (addr>>8) & 0xff;
		while(SPI_BUSY_FLAG);
		reg_spi_data = addr & 0xff;
		while(SPI_BUSY_FLAG);
	}

	/* Send write command. */
	reg_spi_data = SPI_WRITE_CMD;
	while(SPI_BUSY_FLAG);

	/* Send data. */
	for(i = 0; i < len; i++)
	{
		reg_spi_data = *pBuf++;
		while(SPI_BUSY_FLAG);
	}

	/* Set CS signal as high level. */
	gpio_write(SpiCsPin,1);
}

/**---------------------------------Author£º Gaoqiu------------------------------
 * @Brief:  SPI Read data.
 * @Param:  addr    ->
 * @Param:  addrLen -> max = 2,min = 1.
 * @Param:  pBuf    ->
 * @Param:  len     ->
 * @Return: None.
 */
void spi_read(unsigned short addr, unsigned char addrLen, unsigned char *pBuf, unsigned int len)
{
	unsigned i = 0;
	unsigned char dummyData = 0;

	if(SpiCsPin == GPIO_NONE || addrLen > 2 || addrLen == 0)
		return;

	/* Set CS signal as low level. */
	gpio_write(SpiCsPin,0);

	/* Enable SPI data output and SPI write command. */
	reg_spi_ctrl &= ~(FLD_SPI_DATA_OUT_DIS|FLD_SPI_RD);

	/* Send Address. */
	if(addrLen == 1)
	{
		reg_spi_data = addr & 0xff;
		while(SPI_BUSY_FLAG);
	}
	else
	{
		//Write Address High bit first.
		reg_spi_data = (addr>>8) & 0xff;
		while(SPI_BUSY_FLAG);
		reg_spi_data = addr & 0xff;
		while(SPI_BUSY_FLAG);
	}

	/* Send Read command. */
	reg_spi_data = SPI_READ_CMD;
	while(SPI_BUSY_FLAG);

	/* Read dummy data. */
	reg_spi_ctrl |= FLD_SPI_DATA_OUT_DIS|FLD_SPI_RD;
	dummyData = reg_spi_data;
	while(SPI_BUSY_FLAG);

	/* Read real data. */
	for(i = 0; i < len; i++)
	{
		*pBuf++ = reg_spi_data;
		while(SPI_BUSY_FLAG);
	}

	/* Set CS signal as high level. */
	gpio_write(SpiCsPin,1);
}

/*-------------------------------- End of File -------------------------------*/
