#include "clock.h"
#include "i2c.h"
#include "gpio.h"
#include "common/string.h"

/**
 * brief: the function initial the relevant gpio as i2c.
 *        when enable one group of pins as i2c, the other two groups of pin should be disable the i2c function.
 *        in addition, when disable pin as i2c, we can enable the pin as gpio.
 * param[in] gpio_sda -- the pin as data line of i2c.
 * param[in] gpio_scl -- the pin as clock line of i2c.
 * return none
 */
void i2c_pin_init(eI2C_PinTypeDef i2c_pin){
	unsigned int gpio_sda = 0;
	unsigned int gpio_scl = 0;

	switch(i2c_pin)
	{
	case I2C_PIN_GROUP_M_A3A4: /* IIC master GPIO settings. */
		gpio_scl = GPIO_PA3;
		gpio_sda = GPIO_PA4;

		/* Set GPIO AF as IIC. */
		GPIOA_AF->RegBits.P3_AF = GPIOA3_I2C_MCK;
		GPIOA_AF->RegBits.P4_AF = GPIOA4_I2C_MSD;

		/* Close other GPIO which have I2C Master function when their's IIC function is enabled. */
		//PA5 PA6
		if(GPIOA_AF->RegBits.P5_AF == GPIOA5_I2C_MCK)
		{
			gpio_set_func(GPIO_PA5,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P6_AF == GPIOA6_I2C_MSD)
		{
			gpio_set_func(GPIO_PA6,AS_GPIO);
		}

		//PB2 PB3
		if(GPIOB_AF->RegBits.P2_AF == GPIOB2_I2C_MCK)
		{
			gpio_set_func(GPIO_PB2,AS_GPIO);
		}
		if( GPIOB_AF->RegBits.P3_AF == GPIOB3_I2C_MSD)
		{
			gpio_set_func(GPIO_PB3,AS_GPIO);
		}

		//PB6 PB7
		if(GPIOB_AF->RegBits.P6_AF == GPIOB6_I2C_MCK)
		{
			gpio_set_func(GPIO_PB6,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P7_AF == GPIOB7_I2C_MSD)
		{
			gpio_set_func(GPIO_PB7,AS_GPIO);
		}

		//PC4 PC5
		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_I2C_MSD)
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);
		}
		if(GPIOC_AF->RegBits.P5_AF == GPIOC5_I2C_MCK)
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);
		}
		break;
	case I2C_PIN_GROUP_M_A5A6:
		gpio_scl = GPIO_PA5;
		gpio_sda = GPIO_PA6;

		/* Set GPIO AF as IIC. */
		GPIOA_AF->RegBits.P5_AF = GPIOA5_I2C_MCK;
		GPIOA_AF->RegBits.P6_AF = GPIOA6_I2C_MSD;

		/* Close other GPIO which have I2C Master function when their's IIC function is enabled. */
		if(GPIOA_AF->RegBits.P3_AF == GPIOA3_I2C_MCK)
		{
			gpio_set_func(GPIO_PA3,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P4_AF == GPIOA4_I2C_MSD)
		{
			gpio_set_func(GPIO_PA4,AS_GPIO);
		}

		if(GPIOB_AF->RegBits.P2_AF == GPIOB2_I2C_MCK)
		{
			gpio_set_func(GPIO_PB2,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P3_AF == GPIOB3_I2C_MSD)
		{
			gpio_set_func(GPIO_PB3,AS_GPIO);
		}

		if(GPIOB_AF->RegBits.P6_AF == GPIOB6_I2C_MCK)
		{
			gpio_set_func(GPIO_PB6,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P7_AF == GPIOB7_I2C_MSD)
		{
			gpio_set_func(GPIO_PB7,AS_GPIO);
		}

		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_I2C_MSD)
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);

		}
		if(GPIOC_AF->RegBits.P5_AF == GPIOC5_I2C_MCK)
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);
		}
		break;
	case I2C_PIN_GROUP_M_B2B3:
		gpio_scl = GPIO_PB2;
		gpio_sda = GPIO_PB3;

		/* Set GPIO as IIC. */
		GPIOB_AF->RegBits.P2_AF = GPIOB2_I2C_MCK;
		GPIOB_AF->RegBits.P3_AF = GPIOB3_I2C_MSD;

		/* Close other GPIO which have I2C Master function when their's IIC function is enabled. */
		if(GPIOA_AF->RegBits.P3_AF == GPIOA3_I2C_MCK)
		{
			gpio_set_func(GPIO_PA3,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P4_AF == GPIOA4_I2C_MSD)
		{
			gpio_set_func(GPIO_PA4,AS_GPIO);
		}

		if(GPIOA_AF->RegBits.P5_AF == GPIOA5_I2C_MCK)
		{
			gpio_set_func(GPIO_PA5,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P6_AF == GPIOA6_I2C_MSD)
		{
			gpio_set_func(GPIO_PA6,AS_GPIO);
		}

		if(GPIOB_AF->RegBits.P6_AF == GPIOB6_I2C_MCK)
		{
			gpio_set_func(GPIO_PB6,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P7_AF == GPIOB7_I2C_MSD)
		{
			gpio_set_func(GPIO_PB7,AS_GPIO);
		}

		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_I2C_MSD)
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);
		}
		if( GPIOC_AF->RegBits.P5_AF == GPIOC5_I2C_MCK)
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);
		}
		break;
	case I2C_PIN_GROUP_M_B6B7:
		gpio_scl = GPIO_PB6;
		gpio_sda = GPIO_PB7;

		/* Set GPIO AF as IIC. */
		GPIOB_AF->RegBits.P6_AF = GPIOB6_I2C_MCK;
		GPIOB_AF->RegBits.P7_AF = GPIOB7_I2C_MSD;

		/* Close other GPIO which have I2C Master function when their's IIC function is enabled. */
		if(GPIOA_AF->RegBits.P3_AF == GPIOA3_I2C_MCK)
		{
			gpio_set_func(GPIO_PA3,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P4_AF == GPIOA4_I2C_MSD)
		{
			gpio_set_func(GPIO_PA4,AS_GPIO);
		}

		if(GPIOA_AF->RegBits.P5_AF == GPIOA5_I2C_MCK)
		{
			gpio_set_func(GPIO_PA5,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P6_AF == GPIOA6_I2C_MSD)
		{
			gpio_set_func(GPIO_PA6,AS_GPIO);
		}

		if(GPIOB_AF->RegBits.P2_AF == GPIOB2_I2C_MCK)
		{
			gpio_set_func(GPIO_PB2,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P3_AF == GPIOB3_I2C_MSD)
		{
			gpio_set_func(GPIO_PB3,AS_GPIO);
		}

		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_I2C_MSD)
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);
		}
		if(GPIOC_AF->RegBits.P5_AF == GPIOC5_I2C_MCK)
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);
		}
		break;
	case I2C_PIN_GROUP_M_C4C5:
		gpio_scl = GPIO_PC5;
		gpio_sda = GPIO_PC4;

		/* Set GPIO as IIC. */
		GPIOC_AF->RegBits.P4_AF = GPIOC4_I2C_MSD;
		GPIOC_AF->RegBits.P5_AF = GPIOC5_I2C_MCK;

		/* Close other GPIO which have I2C Master function when their's IIC function is enabled. */
		if(GPIOA_AF->RegBits.P3_AF == GPIOA3_I2C_MCK)
		{
			gpio_set_func(GPIO_PA3,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P4_AF == GPIOA4_I2C_MSD)
		{
			gpio_set_func(GPIO_PA4,AS_GPIO);
		}

		if(GPIOA_AF->RegBits.P5_AF == GPIOA5_I2C_MCK)
		{
			gpio_set_func(GPIO_PA5,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P6_AF == GPIOA6_I2C_MSD)
		{
			gpio_set_func(GPIO_PA6,AS_GPIO);
		}

		if(GPIOB_AF->RegBits.P2_AF == GPIOB2_I2C_MCK)
		{
			gpio_set_func(GPIO_PB2,AS_GPIO);
		}
		if(GPIOB_AF->RegBits.P3_AF == GPIOB3_I2C_MSD)
		{
			gpio_set_func(GPIO_PB3,AS_GPIO);
		}

		if(GPIOB_AF->RegBits.P6_AF == GPIOB6_I2C_MCK)
		{
			gpio_set_func(GPIO_PB6,AS_GPIO);
		}
		if( GPIOB_AF->RegBits.P7_AF == GPIOB7_I2C_MSD)
		{
			gpio_set_func(GPIO_PB7,AS_GPIO);
		}
		break;
	case I2C_PIN_GROUP_S_A3A4:/* IIC slave GPIO settings. */
		gpio_scl = GPIO_PA4;
		gpio_sda = GPIO_PA3;

		/* Set GPIO AF as IIC. */
		GPIOA_AF->RegBits.P3_AF = GPIOA3_I2C_SD_OR_SPI_DI;
		GPIOA_AF->RegBits.P4_AF = GPIOA4_I2C_CK_OR_SPI_CK;

		/* Close other GPIO which have I2C Slave function. */
		if(GPIOA_AF->RegBits.P5_AF == GPIOA5_I2C_CK)
		{
			gpio_set_func(GPIO_PA5,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P6_AF == GPIOA6_I2C_SD)
		{
			gpio_set_func(GPIO_PA6,AS_GPIO);
		}

		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_SPI_DI_OR_I2C_SD)
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);
		}
		if(GPIOC_AF->RegBits.P5_AF == GPIOC5_SPI_CK_OR_I2C_CK)
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);
		}
		break;
	case I2C_PIN_GROUP_S_A5A6:
		gpio_scl = GPIO_PA5;
		gpio_sda = GPIO_PA6;

		/* Set GPIO AF as IIC. */
		GPIOA_AF->RegBits.P5_AF = GPIOA5_I2C_CK;
		GPIOA_AF->RegBits.P6_AF = GPIOA6_I2C_SD;

		/* Close other GPIO which have I2C Slave function. */
		if(GPIOA_AF->RegBits.P3_AF == GPIOA3_I2C_SD_OR_SPI_DI)
		{
			gpio_set_func(GPIO_PA3,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P4_AF == GPIOA4_I2C_CK_OR_SPI_CK)
		{
			gpio_set_func(GPIO_PA4,AS_GPIO);
		}

		if(GPIOC_AF->RegBits.P4_AF == GPIOC4_SPI_DI_OR_I2C_SD)
		{
			gpio_set_func(GPIO_PC4,AS_GPIO);
		}
		if(GPIOC_AF->RegBits.P5_AF == GPIOC5_SPI_CK_OR_I2C_CK)
		{
			gpio_set_func(GPIO_PC5,AS_GPIO);
		}
		break;
	case I2C_PIN_GROUP_S_C4C5:
		gpio_scl = GPIO_PC5;
		gpio_sda = GPIO_PC4;

		/* Set GPIO as IIC. */
		GPIOC_AF->RegBits.P4_AF = GPIOC4_SPI_DI_OR_I2C_SD;
		GPIOC_AF->RegBits.P5_AF = GPIOC5_SPI_CK_OR_I2C_CK;

		/* Close other GPIO which have I2C Slave function. */
		if(GPIOA_AF->RegBits.P3_AF == GPIOA3_I2C_SD_OR_SPI_DI)
		{
			gpio_set_func(GPIO_PA3,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P4_AF == GPIOA4_I2C_CK_OR_SPI_CK)
		{
			gpio_set_func(GPIO_PA4,AS_GPIO);
		}

		if(GPIOA_AF->RegBits.P5_AF == GPIOA5_I2C_CK)
		{
			gpio_set_func(GPIO_PA5,AS_GPIO);
		}
		if(GPIOA_AF->RegBits.P6_AF == GPIOA6_I2C_SD)
		{
			gpio_set_func(GPIO_PA6,AS_GPIO);
		}
		break;
	default:
		break;
	}

	//Disable GPIO function.
	gpio_set_func(gpio_sda, AS_I2C);
	gpio_set_func(gpio_scl, AS_I2C);

	//Set GPIO as input.
	gpio_set_input_en(gpio_sda, 1);
	gpio_set_input_en(gpio_scl, 1);

	//Enable 10k pull up.
	gpio_setup_up_down_resistor(gpio_sda, PM_PIN_PULLUP_10K);
	gpio_setup_up_down_resistor(gpio_scl, PM_PIN_PULLUP_10K);
}

/**
 * @brief      This function set the id of slave device and the speed of I2C interface
 *             note: the param ID contain the bit of writting or reading.
 *             eg:the parameter 0x5C. the reading will be 0x5D and writting 0x5C.
 * @param[in]  slave_id - the id of slave device. it don't contain write or read bit.
 * @param[in]  div_clock - the division factor of I2C clock,
 *             I2C clock = System clock / (4*div_clock);if the datasheet you look at is 2*,pls modify it.
 * @return     none
 */
void i2c_master_init_div(unsigned char slave_id, unsigned char div_clock)
{
	reg_i2c_speed = div_clock;                 //configure the i2c's clock

	reg_i2c_id = MASK_VAL(FLD_I2C_ID,slave_id);//set the id of i2c module.

	reg_i2c_mode &= ~(FLD_I2C_MASTER_EN|FLD_I2C_SLAVE_EN);
	reg_i2c_mode |= FLD_I2C_MASTER_EN;

	BM_SET(reg_rst_clk0,FLD_CLK_I2C_EN);       //enable i2c clock

	BM_CLR(reg_spi_sp,FLD_SPI_ENABLE);         //force PADs act as I2C; i2c and spi share the hardware of IC
}

/**
 * @brief      This function set the id of slave device and the speed of I2C interface
*
 * @param[in]  slave_id - the id of slave device.it don't contains write or read bit,the lsb is write or read bit.
 * @param[in]  i2c_speed is in Khz. for example: i2c_speed is 200, indicate 200k
 * @return     none
 */
void i2c_master_init_khz(unsigned char slave_id, unsigned int i2c_speed)
{
	reg_i2c_speed = (CLOCK_SYS_CLOCK_1MS/(4*i2c_speed)); //set i2c clock

	reg_i2c_id = MASK_VAL(FLD_I2C_ID, slave_id);//set the id of i2c module.

	reg_i2c_mode &= ~(FLD_I2C_MASTER_EN|FLD_I2C_SLAVE_EN);
	reg_i2c_mode |= FLD_I2C_MASTER_EN;

	BM_SET(reg_rst_clk0,FLD_CLK_I2C_EN);       //enable i2c clock

	BM_CLR(reg_spi_sp,FLD_SPI_ENABLE);         //force PADs act as I2C; i2c and spi share the hardware of IC
}

/**
 *  @brief      the function config the ID of slave and mode of slave.
 *  @param[in]  device_id - it don't contains write or read bit,the lsb is write or read bit.
 *  @param[in]  i2c_mode - set slave mode. slave has two modes, one is DMA mode, the other is MAPPING mode.
 *  @param[in]  pbuf - if slave mode is MAPPING, set the first address of buffer master write or read slave.
 *              notice: the buffer must align 128 bytes. the write address is pbuf while the read address is pbuf+64.
 *  @return     none
 */
void i2c_slave_init(unsigned char device_id, eI2C_SlaveModeTypeDef i2c_slave_mode,unsigned char* pbuf)
{
	reg_i2c_slave_id = MASK_VAL(FLD_I2C_ID,device_id); //configure the slave id of i2c module.

	if(i2c_slave_mode == I2C_SlaveMode_MAP)
	{
		reg_i2c_mode |= FLD_I2C_SLAVE_MAPPING;
		reg_i2c_mem_map = (unsigned int)pbuf & 0xffff;
	}
	else//DMA mode
	{
		reg_i2c_mode &= ~FLD_I2C_SLAVE_MAPPING;
	}

	reg_i2c_mode &= ~FLD_I2C_MASTER_EN;
	reg_i2c_mode |= FLD_I2C_SLAVE_EN;

	BM_SET(reg_rst_clk0,FLD_CLK_I2C_EN);

	BM_CLR(reg_spi_sp,FLD_SPI_ENABLE);        //force PADs act as I2C; i2c and spi share the hardware of IC
}


/**
 * @Brief:  I2C write a byte.
 * @Param:  addr -> address of data
 * @Param:  addr_len -> the length of address.
 *            0: IIC is working at MAP mode, address length is 0byte.
 *            1: IIC is working at DMA mode, address length is 1byte.
 *            2: IIC is working at DMA mode, address length is 2byte2.
 * @Param:   data ->
 * @Return:  None.
 */
void i2c_write_byte(unsigned int addr, unsigned int addr_len, unsigned char data)
{
	/* start + ID(w) + addr(1or2) */
	BM_CLR(reg_i2c_id, FLD_I2C_WRITE_READ_BIT);  //ID|rw_bit; if rw_bit=1,read data; if rw_bit=0,write data

	/* Slave mode is MAP mode. */
	if(addr_len == 0)//address: 0 byte
	{

	}
	/* Slave mode is DMA mode.*/
	else if(addr_len == 1)//address: 1 byte
	{
		reg_i2c_adr = (unsigned char)(addr & 0xff);
		reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_START, 1, FLD_I2C_CMD_ID, 1, FLD_I2C_CMD_ADR, 1);
		while(I2C_CMD_BUSY_FLAG);
	}
	else if(addr_len == 2)//address: 2 bytes
	{
		reg_i2c_adr = (unsigned char)(addr>>8);
		reg_i2c_do  = (unsigned char)(addr & 0xff);
		reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_START, 1, FLD_I2C_CMD_ID, 1, FLD_I2C_CMD_ADR, 1, FLD_I2C_CMD_DO, 1);
		while(I2C_CMD_BUSY_FLAG);
	}
	/* Parameter invalid */
	else{
		while(1);
	}

	/* Send data */
	reg_i2c_di = data;
	reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_DI, 1);
	while(I2C_CMD_BUSY_FLAG);

	/* Send stop command */
	reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_STOP, 1);
	while(I2C_CMD_BUSY_FLAG);
}

/**
 * @Brief:  I2C read a byte.
 * @Param:  addr -> address of data
 * @Param:  addr_len -> the length of address.
 *            0: IIC is working at MAP mode, address length is 0byte.
 *            1: IIC is working at DMA mode, address length is 1byte.
 *            2: IIC is working at DMA mode, address length is 2byte2.
 * @Param:   data ->
 * @Return:  None.
 */
unsigned char i2c_read_byte(unsigned int addr, unsigned char addr_len)
{
	/* start + ID(w) + addr(1or2) + stop */
	BM_CLR(reg_i2c_id, FLD_I2C_WRITE_READ_BIT);//ID|rw_bit; if rw_bit=1,read data; if rw_bit=0,write data

	/* Slave mode is MAP mode. */
	if(addr_len == 0)//address: 0byte
	{

	}
	/* Slave mode is DMA mode.*/
	if(addr_len == 1)//address: 1byte
	{
		reg_i2c_adr = (unsigned char)addr;
		reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_START, 1, FLD_I2C_CMD_ID, 1, FLD_I2C_CMD_ADR, 1,FLD_I2C_CMD_STOP, 1);
		while(I2C_CMD_BUSY_FLAG);
	}
	else if(addr_len == 2)//address: 2bytes
	{
		reg_i2c_adr = (unsigned char)(addr>>8);
		reg_i2c_do  = (unsigned char)addr;
		reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_START, 1, FLD_I2C_CMD_ID, 1, FLD_I2C_CMD_ADR, 1, FLD_I2C_CMD_DO, 1, FLD_I2C_CMD_STOP, 1);
		while(I2C_CMD_BUSY_FLAG);
	}
	/* Parameter invalid */
	else{
		while(1);
	}

	/* restart + ID(r) */
	BM_SET(reg_i2c_id, FLD_I2C_WRITE_READ_BIT); //ID|rw_bit; if rw_bit=1,read data; if rw_bit=0,write data

	/* simulate restart. when chip don't send stop bit, chip will not occur start bit. so reset module to occur restart***/
	I2C_MODULE_RESET();    //reset i2c module and occur "restart"

	reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_START, 1, FLD_I2C_CMD_ID, 1);
	while(I2C_CMD_BUSY_FLAG);

	/* Get a byte data */
	unsigned char data = 0;
	reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_DI, 1, FLD_I2C_CMD_READ_ID, 1, FLD_I2C_CMD_NAK, 1);//enable read and send 8 clock
	while(I2C_CMD_BUSY_FLAG);
	data = reg_i2c_di;

	/* Send stop command */
	reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_STOP, 1);
	while(I2C_CMD_BUSY_FLAG);

	return data;
}

/**
 * @Brief:  I2C write multiple data.
 * @Param:  addr -> address of data
 * @Param:  addr_len -> the length of address.
 *            0: IIC is working at MAP mode, address length is 0byte.
 *            1: IIC is working at DMA mode, address length is 1byte.
 *            2: IIC is working at DMA mode, address length is 2byte2.
 * @Param:   pbuf -> pointer point to buffer of storing data.
 * @Param:   len  -> the length of data.
 * @Return:  None.
 */
void i2c_burst_write(unsigned short addr, unsigned char addr_len, unsigned char* pbuf,
		             unsigned int len)
{
	/***start + ID(w) + addr(1or2)***/
	BM_CLR(reg_i2c_id, FLD_I2C_WRITE_READ_BIT);  //ID|rw_bit; if rw_bit=1,read data; if rw_bit=0,write data

	/* Slave mode is MAP mode. */
	if(addr_len == 0)//address: 0 byte
	{

	}
	/* Slave mode is DMA mode.*/
	else if(addr_len == 1)//address: 1 byte
	{
		reg_i2c_adr = (unsigned char)addr;
		reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_START, 1, FLD_I2C_CMD_ID, 1, FLD_I2C_CMD_ADR, 1);
		while(I2C_CMD_BUSY_FLAG);
	}
	else if(addr_len == 2)//address: 2 bytes
	{
		reg_i2c_adr = (unsigned char)(addr>>8);
		reg_i2c_do  = (unsigned char)addr;
		reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_START, 1, FLD_I2C_CMD_ID, 1, FLD_I2C_CMD_ADR, 1, FLD_I2C_CMD_DO, 1);
		while(I2C_CMD_BUSY_FLAG);
	}
	/* Parameter invalid */
	else{
		while(1);
	}

	/* Write data to slave */
	for(int i = 0; i < len; i++){
		reg_i2c_di = pbuf[i];
		reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_DI, 1);
		while(I2C_CMD_BUSY_FLAG);
	}

	/* Send Stop command */
	reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_STOP , 1);
	while(I2C_CMD_BUSY_FLAG);
}

/**
 * @Brief:  I2C read multiple data.
 * @Param:  addr -> address of data
 * @Param:  addr_len -> the length of address.
 *            0: IIC is working at MAP mode, address length is 0byte.
 *            1: IIC is working at DMA mode, address length is 1byte.
 *            2: IIC is working at DMA mode, address length is 2bytes.
 * @Param:   pbuf -> pointer point to buffer of storing data.
 * @Param:   len  -> the length of data.
 * @Return:  None.
 */
void i2c_burst_read(unsigned short addr, unsigned char addr_len, unsigned char* pbuf,
		            unsigned int len)
{
	int idx = 0;

	/* start + ID(w) + addr(1or2) + stop */
	BM_CLR(reg_i2c_id, FLD_I2C_WRITE_READ_BIT);//ID|rw_bit; if rw_bit=1,read data; if rw_bit=0,write data

	/* Slave mode is MAP mode. */
	if(addr_len == 0)//address: 0byte
	{

	}
	/* Slave mode is DMA mode.*/
	if(addr_len == 1)//address: 1byte
	{
		reg_i2c_adr = (unsigned char)addr;
		reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_START, 1, FLD_I2C_CMD_ID, 1, FLD_I2C_CMD_ADR, 1,FLD_I2C_CMD_STOP, 1);
		while(I2C_CMD_BUSY_FLAG);
	}
	else if(addr_len == 2)//address: 2bytes
	{
		reg_i2c_adr = (unsigned char)(addr>>8);
		reg_i2c_do  = (unsigned char)addr;
		reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_START, 1, FLD_I2C_CMD_ID, 1, FLD_I2C_CMD_ADR, 1, FLD_I2C_CMD_DO, 1, FLD_I2C_CMD_STOP, 1);
		while(I2C_CMD_BUSY_FLAG);
	}
	/* Parameter invalid */
	else{
		while(1);
	}

	/* start + ID(r) */
	I2C_MODULE_RESET();//Reset IIC to send start command again.
	BM_SET(reg_i2c_id, FLD_I2C_WRITE_READ_BIT);  //ID|rw_bit; if rw_bit=1,read data; if rw_bit=0,write data
	reg_i2c_ctrl =  MASK_VAL(FLD_I2C_CMD_START, 1, FLD_I2C_CMD_ID, 1);
	while(I2C_CMD_BUSY_FLAG);

	len--;
	while(len){
		reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_DI, 1, FLD_I2C_CMD_READ_ID, 1);
		while(I2C_CMD_BUSY_FLAG);
		pbuf[idx] = reg_i2c_di;
		idx++;
		len--;
	}
	reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_DI, 1, FLD_I2C_CMD_READ_ID, 1, FLD_I2C_CMD_NAK, 1);
	while(I2C_CMD_BUSY_FLAG);

	pbuf[idx] = reg_i2c_di;

	/* Send stop command */
	reg_i2c_ctrl = MASK_VAL(FLD_I2C_CMD_STOP, 1);
	while(I2C_CMD_BUSY_FLAG);
}

#if 1
/**
 * @Brief:  Get IIC slave Irq flag.
 * @Param:  I2C_IrqFlag ->
 * @Return: 0/1
 */
unsigned char i2c_get_irq_flag(eI2C_IrqFlagTypeDef I2C_IrqFlag)
{
	return (reg_i2c_irq_status & (I2C_IrqFlag))? 1:0;
}

/**
 * @Brief:  Clear IIC slave Irq flag.
 * @Param:  I2C_IrqFlag ->
 * @Return: None.
 */
void i2c_clear_irq_flag(eI2C_IrqFlagTypeDef I2C_IrqFlag)
{
	reg_i2c_irq_status |= (I2C_IrqFlag);
}

#else
/**
 * @Brief   judge what the irq source is. host write or host read.
 * @Param:  None.
 * @return: I2C_IRQ_HOST_READ_ONLY/I2C_IRQ_HOST_WRITE_ONLY/I2C_IRQ_NONE
 */
eI2C_IrqSrcTypeDef i2c_get_irq_flag(void){
	unsigned char irqFlag = reg_i2c_irq_status;

	if(irqFlag & FLD_I2C_STATUS_RD)
	{
		return I2C_IRQ_HOST_READ_ONLY;
	}
	else if(irqFlag & FLD_I2C_STATUS_WR)
	{
		return I2C_IRQ_HOST_WRITE_ONLY;
	}
	else
	{
		return I2C_IRQ_NONE;
	}
}

/**
 * @Brief:  clear the irq of IIC.
 * @Param:  I2C_IrqSrc ->
 * @Return: None.
 */
void i2c_clear_irq_flag(eI2C_IrqSrcTypeDef I2C_IrqSrc)
{
 	if(I2C_IrqSrc == I2C_IRQ_HOST_READ_ONLY)
 	{
 		reg_i2c_irq_status |= FLD_I2C_STATUS_RD;//write 1 to clear.
 	}
 	else
 	{
 		reg_i2c_irq_status |= FLD_I2C_STATUS_WR;//write 1 to clear.
 	}
}
#endif
/*---------------------------End of File -------------------------------------*/




