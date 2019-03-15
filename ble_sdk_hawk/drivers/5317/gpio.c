#include "gpio.h"
#include "config.h"
#include "bsp.h"
#include "compiler.h"
#include "register.h"
#include "analog.h"


void gpio_init(void){
	//|Input| IE |OEN|Output| DS |GPIO function|

	/* GPIOA Init ------------------------------------------------------------*/
	//PA IE settings
	reg_gpioPA0_4_ie = (PA0_INPUT_ENABLE<<0)| (PA1_INPUT_ENABLE<<1)|(PA2_INPUT_ENABLE<<2)|(PA3_INPUT_ENABLE<<3)|(PA4_INPUT_ENABLE<<4);
	analog_write(analogRegAddr_gpioPA5_7_ie, (PA5_INPUT_ENABLE<<5)|(PA6_INPUT_ENABLE<<6)|(PA7_INPUT_ENABLE<<7));
	//PA OEN settings
	reg_gpio_pa_oen = (PA0_OUTPUT_ENABLE?0:BIT(0))|(PA1_OUTPUT_ENABLE?0:BIT(1))|(PA2_OUTPUT_ENABLE?0:BIT(2))|(PA3_OUTPUT_ENABLE?0:BIT(3))|
					  (PA4_OUTPUT_ENABLE?0:BIT(4))|(PA5_OUTPUT_ENABLE?0:BIT(5))|(PA6_OUTPUT_ENABLE?0:BIT(6))|(PA7_OUTPUT_ENABLE?0:BIT(7));
    //PA Output settings
	reg_gpio_pa_out = (PA0_DATA_OUT<<0)|(PA1_DATA_OUT<<1)|(PA2_DATA_OUT<<2)|(PA3_DATA_OUT<<3)|
			          (PA4_DATA_OUT<<4)|(PA5_DATA_OUT<<5)|(PA6_DATA_OUT<<6)|(PA7_DATA_OUT<<7);
	//PA DS settings
	reg_gpioPA0_4_ds = (PA0_DATA_STRENGTH<<0)|(PA1_DATA_STRENGTH<<1)|(PA2_DATA_STRENGTH<<2)|(PA3_DATA_STRENGTH<<3)|(PA4_DATA_STRENGTH<<4);
	analog_write(analogRegAddr_gpioPA5_7_ds, (PA5_DATA_STRENGTH<<5)|(PA6_DATA_STRENGTH<<6)|(PA7_DATA_STRENGTH<<7));
	//PA GPIO function
    reg_gpio_pa_gpio = (PA0_FUNC == AS_GPIO?BIT(0):0)|(PA1_FUNC == AS_GPIO?BIT(1):0)|(PA2_FUNC == AS_GPIO?BIT(2):0)|(PA3_FUNC == AS_GPIO?BIT(3):0)|
    				   (PA4_FUNC == AS_GPIO?BIT(4):0)|(PA5_FUNC == AS_GPIO?BIT(5):0)|(PA6_FUNC == AS_GPIO?BIT(6):0)|(PA7_FUNC == AS_GPIO?BIT(7):0);

    /* GPIOB Init ------------------------------------------------------------*/
    //PB IE settings
    analog_write(analogRegAddr_gpioPB_ie,(PB0_INPUT_ENABLE<<0)|(PB1_INPUT_ENABLE<<1)|(PB2_INPUT_ENABLE<<2)|(PB3_INPUT_ENABLE<<3)|
    									 (PB4_INPUT_ENABLE<<4)|(PB5_INPUT_ENABLE<<5)|(PB6_INPUT_ENABLE<<6)|(PB7_INPUT_ENABLE<<7));
    //PB OEN settings
    reg_gpio_pb_oen = (PB0_OUTPUT_ENABLE?0:BIT(0))|(PB1_OUTPUT_ENABLE?0:BIT(1))|(PB2_OUTPUT_ENABLE?0:BIT(2))|(PB3_OUTPUT_ENABLE?0:BIT(3))|
    				  (PB4_OUTPUT_ENABLE?0:BIT(4))|(PB5_OUTPUT_ENABLE?0:BIT(5))|(PB6_OUTPUT_ENABLE?0:BIT(6))|(PB7_OUTPUT_ENABLE?0:BIT(7));
    //PB Output settings
    reg_gpio_pb_out = (PB0_DATA_OUT<<0)|(PB1_DATA_OUT<<1)|(PB2_DATA_OUT<<2)|(PB3_DATA_OUT<<3)|
    		          (PB4_DATA_OUT<<4)|(PB5_DATA_OUT<<5)|(PB6_DATA_OUT<<6)|(PB7_DATA_OUT<<7);
    //PB DS settings
    analog_write(analogRegAddr_gpioPB_ds,(PB0_DATA_STRENGTH<<0)|(PB1_DATA_STRENGTH<<1)|(PB2_DATA_STRENGTH<<2)|(PB3_DATA_STRENGTH<<3)|
    									 (PB4_DATA_STRENGTH<<4)|(PB5_DATA_STRENGTH<<5)|(PB6_DATA_STRENGTH<<6)|(PB7_DATA_STRENGTH<<7));
    //PB GPIO function
    reg_gpio_pb_gpio = (PB0_FUNC == AS_GPIO?BIT(0):0)|(PB1_FUNC == AS_GPIO?BIT(1):0)|(PB2_FUNC == AS_GPIO?BIT(2):0)|(PB3_FUNC == AS_GPIO?BIT(3):0)|
    				   (PB4_FUNC == AS_GPIO?BIT(4):0)|(PB5_FUNC == AS_GPIO?BIT(5):0)|(PB6_FUNC == AS_GPIO?BIT(6):0)|(PB7_FUNC == AS_GPIO?BIT(7):0);

    /* PC Init ---------------------------------------------------------------*/
	reg_gpio_pc_setting1 =
		(PC0_INPUT_ENABLE<<8) | (PC1_INPUT_ENABLE<<9) |(PC2_INPUT_ENABLE<<10)|(PC3_INPUT_ENABLE<<11) |
		(PC4_INPUT_ENABLE<<12)| (PC5_INPUT_ENABLE<<13)|(PC6_INPUT_ENABLE<<14)|(PC7_INPUT_ENABLE<<15) |
		((PC0_OUTPUT_ENABLE?0:1)<<16)|((PC1_OUTPUT_ENABLE?0:1)<<17)|((PC2_OUTPUT_ENABLE?0:1)<<18)|((PC3_OUTPUT_ENABLE?0:1)<<19) |
		((PC4_OUTPUT_ENABLE?0:1)<<20)|((PC5_OUTPUT_ENABLE?0:1)<<21)|((PC6_OUTPUT_ENABLE?0:1)<<22)|((PC7_OUTPUT_ENABLE?0:1)<<23) |
		(PC0_DATA_OUT<<24)|(PC1_DATA_OUT<<25)|(PC2_DATA_OUT<<26)|(PC3_DATA_OUT<<27)|
		(PC4_DATA_OUT<<28)|(PC5_DATA_OUT<<29)|(PC6_DATA_OUT<<30)|(PC7_DATA_OUT<<31);
	reg_gpio_pc_setting2 =
		(PC0_DATA_STRENGTH<<8) |(PC1_DATA_STRENGTH<<9) |(PC2_DATA_STRENGTH<<10)|(PC3_DATA_STRENGTH<<11)|
		(PC4_DATA_STRENGTH<<12)|(PC5_DATA_STRENGTH<<13)|(PC6_DATA_STRENGTH<<14)|(PC7_DATA_STRENGTH<<15)|
		(PC0_FUNC==AS_GPIO ? BIT(16):0)|(PC1_FUNC==AS_GPIO ? BIT(17):0)|(PC2_FUNC==AS_GPIO ? BIT(18):0)|(PC3_FUNC==AS_GPIO ? BIT(19):0) |
		(PC4_FUNC==AS_GPIO ? BIT(20):0)|(PC5_FUNC==AS_GPIO ? BIT(21):0)|(PC6_FUNC==AS_GPIO ? BIT(22):0)|(PC7_FUNC==AS_GPIO ? BIT(23):0);

	analog_write(0x08,  PULL_WAKEUP_SRC_PA5|
						(PULL_WAKEUP_SRC_PA6<<2)|
						(PULL_WAKEUP_SRC_PA7<<4)|
						(PULL_WAKEUP_SRC_PB0<<6));

	analog_write(0x09,  PULL_WAKEUP_SRC_PB1|
						(PULL_WAKEUP_SRC_PB2<<2)|
						(PULL_WAKEUP_SRC_PB3<<4)|
						(PULL_WAKEUP_SRC_PB4<<6));

	analog_write(0x0a,  PULL_WAKEUP_SRC_PB5|
						(PULL_WAKEUP_SRC_PB6<<2)|
						(PULL_WAKEUP_SRC_PB7<<4)|
						(PULL_WAKEUP_SRC_PA0<<6));

	analog_write(0x0b,  PULL_WAKEUP_SRC_PA1|
						(PULL_WAKEUP_SRC_PA2<<2)|
						(PULL_WAKEUP_SRC_PA3<<4)|
						(PULL_WAKEUP_SRC_PA4<<6));

	analog_write(0x0c,  PULL_WAKEUP_SRC_PC0|
						(PULL_WAKEUP_SRC_PC1<<2)|
						(PULL_WAKEUP_SRC_PC2<<4)|
						(PULL_WAKEUP_SRC_PC3<<6));

	analog_write(0x0d,  PULL_WAKEUP_SRC_PC4|
						(PULL_WAKEUP_SRC_PC5<<2)|
						(PULL_WAKEUP_SRC_PC6<<4)|
						(PULL_WAKEUP_SRC_PC7<<6));
}

/**
 * @Brief: Set gpio pull resistor.
 * @Param: gpio    -> indicate the pin
 *         up_down -> 0 : float
 * 				      1 : 1M   pull up
 * 				      2 : 10K  pull up
 * 				      3 : 100K pull down
 *-----------------------------------------------
 *      BIT(7.6)   BIT(5.4)   BIT(3.2)   BIT(1.0)
 *-----------------------------------------------
 *offset|  6  |    |  4  |    |  2  |    |  0  |
 *-----------------------------------------------
 * 08     B0         A7         A6         A5
 * 09     B4         B3         B2         B1
 * 0a	  A0         B7         B6         B5
 * 0b	  A4         A3         A2         A1
 * 0c	  C3         C2         C1         C0
 * 0d     C7         C6         C5         C4
 *-----------------------------------------------
 */
const unsigned char gpioPullResistorMapTab[3][8]=
{
	//b0	b1	  b2	b3	  b4	b5	  b6	b7
	{0xa6, 0xb0, 0xb2, 0xb4, 0xb6, 0x80, 0x82, 0x84 }, //GPIO_GROUPA
	{0x86, 0x90, 0x92, 0x94, 0x96, 0xa0, 0xa2, 0xa4 }, //GPIO_GROUPB
	{0xc0, 0xc2, 0xc4, 0xc6, 0xd0, 0xd2, 0xd4, 0xd6 }  //GPIO_GROUPC
};

//if GPIO_DP,please check usb_dp_pullup_en() valid or not first.
void gpio_setup_up_down_resistor(GPIO_PinTypeDef gpio, GPIO_PullTypeDef up_down)
{
	unsigned char bit = gpio & 0xff;
	unsigned char pinId = 0;
	unsigned short gpioGroup = gpio & 0xf00;
	unsigned char gpioGroupId = gpio >> 8;
	unsigned char pullResistorAddr = 0;
	unsigned char offset = 0;
	unsigned char temp = 0;

	for(volatile int i = 0; i<8; i++)
	{
		if((bit>>i) & 0x01)
		{
			pinId = i;
			break;
		}
	}
	if(pinId >= 8)//parameter error.
		return;

	temp = gpioPullResistorMapTab[gpioGroupId][pinId];
	pullResistorAddr = (temp>>4) & 0x0f;
	offset = temp & 0x0f;

	temp = analog_read(pullResistorAddr);
	temp &= ~(0x03<<offset);
	temp |= (up_down << offset);
	analog_write(pullResistorAddr,temp);

	if(up_down == GPIO_PULL_DOWN_100K &&
	   (gpio == GPIO_PA6 || gpio == GPIO_PA7 || gpioGroup == GPIO_GROUPB))
	{
		//disable 50k pull up of PA6-PA7 and PB0-PB7
		temp = analog_read(0x05);
		temp |= 0x80;
		analog_write(0x05,temp);
	}
}

int gpio_is_input_en(GPIO_PinTypeDef pin)
{
	unsigned short gpioGroup = pin & 0x0f00;
	unsigned char bit = pin & 0xff;

	if(gpioGroup == GPIO_GROUPA)
	{
		if((bit >= 1) && (bit <= 16))//PA0-PA4
		{
			return BM_IS_SET(reg_gpio_ie(pin), bit);
		}
		else //PA5-PA7
		{
			unsigned char temp = analog_read(analogRegAddr_gpioPA5_7_ie);
			return BM_IS_SET(temp, bit);
		}
	}
	else if(gpioGroup == GPIO_GROUPB)
	{
		unsigned char temp = analog_read(analogRegAddr_gpioPB_ie);
		return BM_IS_SET(temp, bit);
	}

	return BM_IS_SET(reg_gpio_ie(pin), bit);
}

void gpio_set_input_en(GPIO_PinTypeDef pin, unsigned int value)
{
	unsigned short gpioGroup = pin & 0x0f00;
	unsigned char bit = pin & 0xff;
	unsigned char temp = 0;

	if(gpioGroup == GPIO_GROUPA)
	{
		if(bit >= 1 && bit <= 16)//PA0-PA4
		{
			if(value)
			{
				BM_SET(reg_gpioPA0_4_ie, bit);
			}
			else
			{
				BM_CLR(reg_gpioPA0_4_ie, bit);
			}
		}
		else//PA5-PA7
		{
			if(value)
			{
				temp = analog_read(analogRegAddr_gpioPA5_7_ie);
				temp |= bit;
				analog_write(analogRegAddr_gpioPA5_7_ie, temp);
			}
			else
			{
				temp = analog_read(analogRegAddr_gpioPA5_7_ie);
				temp &= ~bit;
				analog_write(analogRegAddr_gpioPA5_7_ie, temp);
			}
		}
	}
	else if(gpioGroup == GPIO_GROUPB)
	{
		if(value)
		{
			temp = analog_read(analogRegAddr_gpioPB_ie);
			temp |= bit;
			analog_write(analogRegAddr_gpioPB_ie, temp);
		}
		else
		{
			temp = analog_read(analogRegAddr_gpioPB_ie);
			temp &= ~bit;
			analog_write(analogRegAddr_gpioPB_ie, temp);
		}
	}
	else
	{
		if(value)
		{
			BM_SET(reg_gpio_ie(pin), bit);
		}
		else
		{
			BM_CLR(reg_gpio_ie(pin), bit);
		}
	}
}

void gpio_set_data_strength(GPIO_PinTypeDef pin, unsigned int value)
{
	unsigned short gpioGroup = pin & 0x0f00;
	unsigned char bit = pin & 0xff;
	unsigned char temp = 0;

	if(gpioGroup == GPIO_GROUPA)
	{
		if(bit >= 1 && bit <= 16)//PA0-PA4
		{
			if(value)
			{
				BM_SET(reg_gpioPA0_4_ds, bit);
			}
			else
			{
				BM_CLR(reg_gpioPA0_4_ds, bit);
			}
		}
		else//PA5-PA7
		{
			if(value)
			{
				temp = analog_read(analogRegAddr_gpioPA5_7_ds);
				temp |= bit;
				analog_write(analogRegAddr_gpioPA5_7_ds, temp);
			}
			else
			{
				temp = analog_read(analogRegAddr_gpioPA5_7_ds);
				temp &= ~bit;
				analog_write(analogRegAddr_gpioPA5_7_ds, temp);
			}
		}
	}
	else if(gpioGroup == GPIO_GROUPB)
	{
		if(value)
		{
			temp = analog_read(analogRegAddr_gpioPB_ds);
			temp |= bit;
			analog_write(analogRegAddr_gpioPB_ds, temp);
		}
		else
		{
			temp = analog_read(analogRegAddr_gpioPB_ds);
			temp &= ~bit;
			analog_write(analogRegAddr_gpioPB_ds, temp);
		}
	}
	else
	{
		if(value)
		{
			BM_SET(reg_gpio_ds(pin), bit);
		}
		else
		{
			BM_CLR(reg_gpio_ds(pin), bit);
		}
	}
}

/**
 * @Brief: Set GPIO(digital) as wke-up source.
 * @Param:
 * @Param:
 * @Param:
 * @Return: None.
 */
void gpio_set_wakeup(GPIO_PinTypeDef pin, GPIO_LevelTypeDef level, int en)
{
	unsigned char bit = pin & 0xff;
    if (en) {
        BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
    }
    else {
        BM_CLR(reg_gpio_irq_wakeup_en(pin), bit);
    }

    if(level){
        BM_CLR(reg_gpio_pol(pin), bit);
    }else{
        BM_SET(reg_gpio_pol(pin), bit);
    }

    gpio_set_func(pin,AS_GPIO);
    gpio_set_input_en(pin,1);//must
    reg_gpio_wakeup_and_irq_en |= FLD_GPIO_CORE_WAKEUP_EN;
}

void gpio_clear_gpio_irq_flag(void)
{
	REG_ADDR8(0x64A) |= BIT(2);
}

#if 0
void gpio_set_func(GPIO_PinTypeDef pin, GPIO_FuncTypeDef func)
{
	unsigned char bit = pin & 0xff;
	if(func == AS_GPIO){
		BM_SET(reg_gpio_gpio_func(pin), bit);
		return;
	}else{
		BM_CLR(reg_gpio_gpio_func(pin), bit);
	}
}
#else
void gpio_set_func(GPIO_PinTypeDef pin, GPIO_FuncTypeDef func)
{
	unsigned char bit = pin & 0xff;
	if(func == AS_GPIO){
		BM_SET(reg_gpio_gpio_func(pin), bit);
		return;
	}else{
		BM_CLR(reg_gpio_gpio_func(pin), bit);
	}

	//config gpio special func
	unsigned char val = 0;
	unsigned char mask = 0xff;
	switch(pin)
	{
		case GPIO_PA0:
		{
			//0x5a8[1:0]
			//0. PWM0
			//1. PWM3
			mask = (unsigned char)~(BIT(1)|BIT(0));
			if(func == AS_PWM0){

			}else if(func == AS_PWM3){
				val = BIT(0);
			}
		}
		break;
		case GPIO_PA1:
		{
			//0x5a8[3:2]
			//0. PWM3N/PWM0N
			//1. UART_TX
			//2. I2C_MCK
			mask= (unsigned char)~(BIT(3)|BIT(2));
			if(func == AS_PWM3_N){
				BM_SET(reg_gpio_pb_multi_func_select, BIT(1));
			}
			else if(func == AS_PWM0_N){
				BM_CLR(reg_gpio_pb_multi_func_select, BIT(1));
			}
			else if(func == AS_UART){
				val = BIT(2);
			}
			else if(func == AS_I2C_M){
				val = BIT(3);
			}
		}
		break;

		case GPIO_PA2:
		{
			//0x5a8[5:4]
			//0. PWM1_N
			//1. UART_RX
			//2. I2C_MCK
			mask= (unsigned char)~(BIT(5)|BIT(4));
			if(func == AS_PWM1_N){

			}else if(func == AS_UART){
				val = BIT(4);
			}
			else if(func == AS_I2C_M){
				val = BIT(5);
			}
		}
		break;

		case GPIO_PA3:
		{
			//0x5a8[7:6]
			//0. PWM4
			//1. DMIC
			mask= (unsigned char)~(BIT(7)|BIT(6));
			if(func == AS_PWM4){

			}else if(func == AS_DMIC){
				val = BIT(6);
			}
		}
		break;


		case GPIO_PA4:
		{
			//0x5a9[1:0]
			//0. PWM2
			//1. DMIC_CLK
			mask= (unsigned char)~(BIT(1)|BIT(0));
			if(func == AS_PWM2){

			}else if(func == AS_DMIC){
				val = BIT(0);
			}
		}
		break;

		case GPIO_PA5:
		{
//			//0x5a9[3:2]
//			//0. PWM5
//			//1. I2C_S_CLK
//			//2. I2C_M_CLK
//			mask= (unsigned char)~(BIT(3)|BIT(2));
//			if(func == AS_PWM5){
//
//			}else if(func == AS_I2C_S){
//				val = BIT(2);
//			}else if(func == AS_I2C_M){
//				val = BIT(3);
//			}
		}
		case GPIO_PA6:
		{
//			//0x5a9[5:4]
//			//0. AS_PWM4_N
//			//1. I2C_S_SD
//			//2. I2C_M_SD
//			mask= (unsigned char)~(BIT(5)|BIT(4));
//			if(func == AS_PWM4_N){
//
//			}else if(func == AS_I2C_S){
//				val = BIT(4);
//			}else if(func == AS_I2C_M){
//				val = BIT(5);
//			}
		}
		break;

		case GPIO_PA7:
		{
			//0x5a9[7:6]
			//0. SWS:
			//1. UART_RTS
			mask = (unsigned char)~(BIT(7)|BIT(6));
			if(func == AS_PWM5){

			}else if(func == AS_DMIC){
				val = BIT(6);
			}
		}
		break;

		case GPIO_PB0:
		{
			//0x5aa[1:0]
			//0. PWM3
			//1. AS_DMIC
			//2. AS_SPI
			mask = (unsigned char)~(BIT(1)|BIT(0));
			if(func == AS_PWM3){

			}else if(func == AS_DMIC){
				val = BIT(0);
			}
			else if(func == AS_SPI){
				val = BIT(1)|BIT(0);
			}
		}
		break;


		case GPIO_PB1:
		{
			//0x5aa[3:2]
			//0. PWM1
			//1.
			//3. AS_SPI
			mask = (unsigned char)~(BIT(3)|BIT(2));
			if(func == AS_PWM1){

			}
			else if(func == AS_SPI){
				val = BIT(3)|BIT(2);
			}
		}
		break;


		case GPIO_PB2:
		{
			//0x5aa[5:4]
			//0. PWM2
			//1. AS_SDM
			//2. RX_CYC2LNA
			mask = (unsigned char)~(BIT(5)|BIT(4));
			if(func == AS_PWM2){

			}else if(func == AS_SDM){
				val = BIT(4);
			}
			else if(func == AS_UART){
				val = BIT(5);
			}
		}
		break;


		case GPIO_PB3:
		{
			//0x5aa[7:6]
			//0. PWM0
			//1. CMP_DF
			//2. TX_CYC2PA
			mask = (unsigned char)~(BIT(7)|BIT(6));
			if(func == AS_PWM0){

			}else if(func == AS_SDM){
				val = BIT(6);
			}
			else if(func == AS_UART){
				val = BIT(7);
			}
		}
		break;

		case GPIO_PB4:
		{
			//0x5ab[1:0]
			//0. AS_PWM1_N
			//1. RX_CYC2LNA
			//2. UART
			mask = (unsigned char)~(BIT(1)|BIT(0));
			if(func == AS_PWM1_N){

			}else if(func == AS_ETH_MAC){
				val = BIT(0);
			}
			else if(func == AS_UART){
				val = BIT(1);
			}
		}
		break;


		case GPIO_PB5:
		{
			//0x5ab[3:2]
			//0. AS_PWM
			//1. RX_CYC2LNA
			//2. UART
			mask = (unsigned char)~(BIT(3)|BIT(2));
			if(func == AS_PWM4){

			}else if(func == AS_ETH_MAC){
				val = BIT(2);
			}
			else if(func == AS_UART){
				val = BIT(3);
			}
		}
		break;

		case GPIO_PB6:
		{
			//0x5ab[5:4]
			//0. AS_PWM5_N
			//1. AS_I2C_S
			//2. UART_RTS
			mask = (unsigned char)~(BIT(5)|BIT(4));
			if(func == AS_PWM5_N){

			}else if(func == AS_I2C_S){
				val = BIT(4);
			}
			else if(func == AS_UART){
				val = BIT(5);
			}
		}
		break;


		case GPIO_PB7:
		{
			//0x5ab[7:6]
			//0. AS_PWM1
			//1. AS_I2C_S
			//2. UART_RX
			mask = (unsigned char)~(BIT(7)|BIT(6));
			if(func == AS_PWM1){

			}else if(func == AS_I2C_S){
				val = BIT(6);
			}
			else if(func == AS_UART){
				val = BIT(7);
			}
		}
		break;

		case GPIO_PC0:
		{
			//NO PIN
		}
		break;

		case GPIO_PC1:
		{
			//0x5ac[3:2]
			//0. AS_PWM2_N
			mask = (unsigned char)~(BIT(3)|BIT(2));
			if(func == AS_PWM2_N){
			}
		}
		break;


		case GPIO_PC2:
		{
			//0x5ac[5:4]
			//0. SPI_CN
			//1. AS_PWM0_N
			//2. I2C_SDA
			mask = (unsigned char)~(BIT(5)|BIT(4));
			if(func == AS_SPI){

			}
			else if(func == AS_PWM0_N){
				val = BIT(4);
			}
			else if(func == AS_SPI){
				val = BIT(5);
			}
		}
		break;


		case GPIO_PC3:
		{
			//0x5ac[7:6]
			//0. PWM1
			//1. UART_RX
			//2. I2C_SCK
			mask = (unsigned char)~(BIT(7)|BIT(6));
			if(func == AS_SPI){

			}else if(func == AS_PWM5_N){
				val = BIT(6);
			}
			else if(func == AS_SPI){
				val = BIT(7);
			}
		}
		break;

		case GPIO_PC4:
		{
			//0x5ad[1:0]
			//0. PWM2
			//1. UART_CTS
			//2. PWM0_N
			mask = (unsigned char)~(BIT(1)|BIT(0));
			if(func == AS_SPI){

			}else if(func == AS_I2C_M){
				val = BIT(0);
			}
			else if(func == AS_SPI){
				val = BIT(1);
			}
			else if(func == AS_UART){
				val = BIT(1)|BIT(0);
			}
		}
		break;

		case GPIO_PC5:
		{
			//0x5ad[3:2]
			//0. PWM3_N
			//1. UART_RX
			//2. ATSEL_0
			//3. UART_RX
			mask = (unsigned char)~(BIT(3)|BIT(2));
			if(func == AS_SPI){

			}else if(func == AS_I2C_M){
				val = BIT(2);
			}
			else if(func == AS_SPI){
				val = BIT(3);
			}
			else if(func == AS_UART){
				val = BIT(2)|BIT(3);
			}
		}
		break;

		case GPIO_PC6:
		{
			//0x5ad[5:4]
			//0. RX_CYC2LNA
			//1. ATSEL1
			//2. PWM4_N
			mask = (unsigned char)~(BIT(5)|BIT(4));
			if(func ==AS_PWM4 ){

			}
			else
			if(func == AS_PWM3_N){
				val = BIT(4);
			}

		}
		break;


		case GPIO_PC7:
		{
			//0x5ad[7:6]
			//0. SWS_IO
			//1. PWM5
			mask = (unsigned char)~(BIT(7)|BIT(6));
			if(func == AS_SWS ){

			}
			else if(func == AS_PWM5){
				val = BIT(6);
			}
		}
		break;

		default: break;
	}
	unsigned short reg = 0x5a8 + ((pin>>8)<<1) + ((pin&0x0f0) ? 1 : 0 );
	write_reg8(reg, ( read_reg8(reg) & mask) | val);
}
#endif




