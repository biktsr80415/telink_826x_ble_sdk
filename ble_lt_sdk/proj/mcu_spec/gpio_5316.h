
#pragma once

#include "../common/types.h"
#include "../common/bit.h"
#include "../common/utility.h"
#include "../mcu/compiler.h"
#include "../mcu/register.h"
#include "gpio_default_5316.h"

typedef enum{
		GPIO_GROUPA    = 0x000,
		GPIO_GROUPB    = 0x100,
		GPIO_GROUPC    = 0x200,
		GPIO_GROUPD    = 0x300,
		GPIO_GROUPE    = 0x400,

	    GPIO_PA0 = 0x000 | BIT(0),	GPIOA0_PWM0  = GPIO_PA0, GPIOA0_PWM3     = GPIO_PA0,  GPIOA0_32K_CLK_OUTPUT = GPIO_PA0,
		GPIO_PA1 = 0x000 | BIT(1), 	GPIOA1_PWM3N = GPIO_PA1, GPIOA1_UART_CTS = GPIO_PA1,
		GPIO_PA2 = 0x000 | BIT(2),	GPIOA2_PWM1N = GPIO_PA2, GPIOA2_UART_RTS = GPIO_PA2,
		GPIO_PA3 = 0x000 | BIT(3),	GPIOA3_PWM4  = GPIO_PA3, GPIOA3_UART_TX  = GPIO_PA3,  GPIOA3_I2C_MCK = GPIO_PA3, GPIOA3_SPI_DI = GPIO_PA3, GPIOA3_I2C_SD = GPIO_PA3,
		GPIO_PA4 = 0x000 | BIT(4), 	GPIOA4_PWM2  = GPIO_PA4, GPIOA4_UART_RX  = GPIO_PA4,  GPIOA4_I2C_MSD = GPIO_PA4, GPIOA4_SPI_CK = GPIO_PA4, GPIOA4_I2C_CK = GPIO_PA4,
		GPIO_PA5 = 0x000 | BIT(5), 	GPIOA5_PWM5  = GPIO_PA5, GPIOA5_I2C_CK   = GPIO_PA5,  GPIOA5_I2C_MCK = GPIO_PA5,
		GPIO_PA6 = 0x000 | BIT(6), 	GPIOA6_PWM4N = GPIO_PA6, GPIOA6_I2C_SD   = GPIO_PA6,  GPIOA6_I2C_MSD = GPIO_PA6, GPIOA6_RX_CYC2LNA = GPIO_PA6,
		GPIO_PA7 = 0x000 | BIT(7), 	GPIOA7_PWM5  = GPIO_PA7, GPIOA7_TX_CYC2LNA = GPIO_PA7,

		GPIO_PB0 = 0x100 | BIT(0),	GPIOB0_PWM3  = GPIO_PB0,  GPIOB0_SPI_MCN = GPIO_PB0, GPIOB0_SPI_CN = GPIO_PB0,
		GPIO_PB1 = 0x100 | BIT(1),	GPIOB1_PWM1  = GPIO_PB1,  GPIOB0_SPI_MDO = GPIO_PB1, GPIOB0_SPI_DO = GPIO_PB1,
		GPIO_PB2 = 0x100 | BIT(2), 	GPIOB2_PWM2  = GPIO_PB2,  GPIOB2_SPI_MDI = GPIO_PB2, GPIOB2_SPI_DI = GPIO_PB2, GPIOB2_UART_CTS = GPIO_PB2, GPIOB2_I2C_MCK = GPIO_PB2,
		GPIO_PB3 = 0x100 | BIT(3),	GPIOB3_PWM0  = GPIO_PB3,  GPIOB3_SPI_MCK = GPIO_PB3, GPIOB3_SPI_DI = GPIO_PB3, GPIOB3_UART_RTS = GPIO_PB3, GPIOB3_I2C_MSD = GPIO_PB3,
		GPIO_PB4 = 0x100 | BIT(4),	GPIOB4_PWM1N = GPIO_PB4,  GPIOB4_UART_TX = GPIO_PB4,
		GPIO_PB5 = 0x100 | BIT(5),	GPIOB5_PWM4  = GPIO_PB5,  GPIOB5_UART_RX = GPIO_PB5,
		GPIO_PB6 = 0x100 | BIT(6),	GPIOB6_PWM0N = GPIO_PB6,  GPIOB6_I2C_MCK = GPIO_PB6, GPIOB6_UART_RTS = GPIO_PB6,
		GPIO_PB7 = 0x100 | BIT(7),	GPIOB7_PWM1  = GPIO_PB7,  GPIOB7_I2C_MSD = GPIO_PB7, GPIOB7_ART_CTS  = GPIO_PB7,

		GPIO_PC0 = 0x200 | BIT(0),
		GPIO_PC1 = 0x200 | BIT(1), GPIOC1_PWM2N  = GPIO_PC1,
		GPIO_PC2 = 0x200 | BIT(2), GPIOC2_SPI_CN = GPIO_PC2, GPIOC2_PWM0N = GPIO_PC2, GPIOC2_SPI_MCN = GPIO_PC2, GPIOC2_UART_CTS = GPIO_PC2,
		GPIO_PC3 = 0x200 | BIT(3), GPIOC3_SPI_DO = GPIO_PC3, GPIOC3_PWM5N = GPIO_PC3, GPIOC3_SPI_MDO = GPIO_PC3, GPIOC3_UART_RTS = GPIO_PC3,
		GPIO_PC4 = 0x200 | BIT(4), GPIOC4_SPI_DI = GPIO_PC4, GPIOC4_I2C_SD= GPIO_PC4, GPIOC4_I2C_MSD = GPIO_PC4, GPIOC4_SPI_MDI  = GPIO_PC4,
		GPIO_PC5 = 0x200 | BIT(5), GPIOC5_SPI_CK = GPIO_PC5, GPIOC5_I2C_CK= GPIO_PC5, GPIOC5_I2C_MSCK= GPIO_PC5, GPIOC5_SPI_MCK  = GPIO_PC5,
		GPIO_PC6 = 0x200 | BIT(6), GPIOC6_PWM4   = GPIO_PC6,
		GPIO_PC7 = 0x200 | BIT(7), GPIOC6_SWS    = GPIO_PC7, GPIOC6_PWM3 = GPIO_PC7,

		GPIO_PE0 = 0x400 | BIT(0), GPIOE0_MSDO = GPIO_PE0,
		GPIO_PE1 = 0x400 | BIT(1), GPIOE1_MCLK = GPIO_PE1,
		GPIO_PE2 = 0x400 | BIT(2), GPIOE2_MSCN = GPIO_PE2,
		GPIO_PE3 = 0x400 | BIT(3), GPIOE3_MSDI = GPIO_PE3,


		GPIO_PE4 = 0x400 | BIT(4),
		GPIO_PE5 = 0x400 | BIT(5),
		GPIO_PE6 = 0x400 | BIT(6),

		GPIO_PF0 = 0x500 | BIT(0),

		GPIO_NONE = 0xfff,
}eGPIO_PinTypeDef;


/* PA Port Alternative Function define */
//PA0
#define GPIOA0_PWM0                       0
#define GPIOA0_PWM3                       1
#define GPIOA0_32K_CLK_OUTPUT             2
#define GPIOA0_RESERVE_3                  3
//PA1
#define GPIOA1_PWM3N                      0
#define GPIOA1_UART_CTS                   1
#define GPIOA1_RESERVE_2                  2
#define GPIOA1_RESERVE_3                  3
//PA2
#define GPIOA2_PWM1N                      0
#define GPIOA2_UART_RTS                   1
#define GPIOA2_RESERVE_2                  2
#define GPIOA2_RESERVE_3                  3
//PA3
#define GPIOA3_PWM4                       0
#define GPIOA3_UART_TX                    1
#define GPIOA3_I2C_MCK                    2
#define GPIOA3_I2C_SD_OR_SPI_DI           3
//PA4
#define GPIOA4_PWM2                       0
#define GPIOA4_UART_RX                    1
#define GPIOA4_I2C_MSD                    2
#define GPIOA4_I2C_CK_OR_SPI_CK           3
//PA5
#define GPIOA5_PWM5                       0
#define GPIOA5_I2C_CK                     1
#define GPIOA5_RESERVE_2                  2
#define GPIOA5_I2C_MCK                    3
//PA6
#define GPIOA6_PWM4N                      0
#define GPIOA6_I2C_SD                     1
#define GPIOA6_RX_CYC2LNA                 2
#define GPIOA6_I2C_MSD                    3
//PA7
#define GPIOA7_PWM5                       0
#define GPIOA7_SYS_CLK_OUTPUT             1
#define GPIOA7_TX_CYC2PA                  2
#define GPIOA7_RESERVE_3                  3

/* PB Port Alternative Function define */
//PB0
#define GPIOB0_PWM3                       0
#define GPIOB0_SPI_MCN                    1
#define GPIOB0_RX_CYC2LNA_OR_SPI_CN       2
#define GPIOB0_RESERVE_3                  3
//PB1
#define GPIOB1_PWM1                       0
#define GPIOB1_SPI_MDO                    1
#define GPIOB1_TX_CYC2LNA_OR_SPI_DO       2
#define GPIOB1_RESERVE_3                  3
//PB2
#define GPIOB2_PWM2                       0
#define GPIOB2_SPI_MDI                    1
#define GPIOB2_UART_CTS_OR_SPI_DI         2
#define GPIOB2_I2C_MCK                    3
//PB3
#define GPIOB3_PWM0                       0
#define GPIOB3_SPI_MCK                    1
#define GPIOB3_UART_RTS_OR_SPI_CK         2
#define GPIOB3_I2C_MSD                    3
//PB4
#define GPIOB4_PWM1N                      0
#define GPIOB4_RESERVE_1                  1
#define GPIOB4_UART_TX                    2
#define GPIOB4_RESERVE_3                  3
//PB5
#define GPIOB5_PWM4                       0
#define GPIOB5_RESERVE_1                  1
#define GPIOB5_UART_RX                    2
#define GPIOB5_RESERVE_3                  3
//PB6
#define GPIOB6_PWM0N                      0
#define GPIOB6_I2C_MCK                    1
#define GPIOB6_UART_RTS                   2
#define GPIOB6_RESERVE_3                  3
//PB7
#define GPIOB7_PWM1                       0
#define GPIOB7_I2C_MSD                    1
#define GPIOB7_UART_CTS                   2
#define GPIOB7_RESERVE_3                  3

/* PC Port Alternative Function define */
//PC0
#define GPIOC0_RESERVE_0                  0
#define GPIOC0_RESERVE_1                  1
#define GPIOC0_RESERVE_2                  2
#define GPIOC0_RESERVE_3                  3
//PC1
#define GPIOC1_PWM2N                      0
#define GPIOC1_RESERVE_1                  1
#define GPIOC1_RESERVE_2                  2
#define GPIOC1_RESERVE_3                  3
//PC2
#define GPIOC2_SPI_CN                     0
#define GPIOC2_PWM0N                      1
#define GPIOC2_SPI_MCN                    2
#define GPIOC2_UART_CTS                   3
//PC3
#define GPIOC3_SPI_DO                     0
#define GPIOC3_PWM5N                      1
#define GPIOC3_SPI_MDO                    2
#define GPIOC3_UART_RTS                   3
//PC4
#define GPIOC4_SPI_DI_OR_I2C_SD           0
#define GPIOC4_I2C_MSD                    1
#define GPIOC4_SPI_MDI                    2
#define GPIOC4_UART_TX                    3
//PC5
#define GPIOC5_SPI_CK_OR_I2C_CK           0
#define GPIOC5_I2C_MCK                    1
#define GPIOC5_SPI_MCK                    2
#define GPIOC5_UART_RX                    3
//PC6
#define GPIOC6_PWM4                       0
#define GPIOC6_RESERVE_1                  1
#define GPIOC6_RESERVE_2                  2
#define GPIOC6_RESERVE_3                  3
//PC7
#define GPIOC7_SWS                        0
#define GPIOC7_PWM3                       1
#define GPIOC7_RESERVE_2                  2
#define GPIOC7_RESERVE_3                  3
/* End of GPIO Alternative Function define  */


#define reg_gpio_in(i)				REG_ADDR8(0x580+((i>>8)<<3))

#define reg_gpio_ie(i)				REG_ADDR8(0x581+((i>>8)<<3))
#define reg_gpioPA0_4_ie            REG_ADDR8(0x581)
#define analogRegAddr_gpioPA5_7_ie  0xb6
#define analogRegAddr_gpioPB_ie     0xb9

#define reg_gpio_oen(i)				REG_ADDR8(0x582+((i>>8)<<3))
#define reg_gpio_out(i)				REG_ADDR8(0x583+((i>>8)<<3))
#define reg_gpio_pol(i)				REG_ADDR8(0x584+((i>>8)<<3))

#define reg_gpio_ds(i)				REG_ADDR8(0x585+((i>>8)<<3))
#define reg_gpioPA0_4_ds            REG_ADDR8(0x585)
#define analogRegAddr_gpioPA5_7_ds  0xb8
#define analogRegAddr_gpioPB_ds     0xbb

#define reg_gpio_gpio_func(i)		REG_ADDR8(0x586+((i>>8)<<3))
#define reg_gpio_config_func(i)     REG_ADDR16(0x5a8 + ((i>>8)<<1))
//#define reg_gpio_multi_func(i)      REG_ADDR16(0x5a8 + ((i>>8)<<1))

#define reg_gpio_irq_wakeup_en(i)	REG_ADDR8(0x587+((i>>8)<<3))  // reg_irq_mask: FLD_IRQ_GPIO_EN

#define reg_gpio_irq_risc0_en(i)    REG_ADDR8(0x5b8 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
#define reg_gpio_irq_risc1_en(i)    REG_ADDR8(0x5c0 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN
#define reg_gpio_irq_risc2_en(i)    REG_ADDR8(0x5c8 + (i >> 8))   // reg_irq_mask: FLD_IRQ_GPIO_RISC2_EN

#define reg_gpio_wakeup_and_irq_en  REG_ADDR8(0x5b5)
enum{
    FLD_GPIO_CORE_WAKEUP_EN    = BIT(2),
    FLD_GPIO_CORE_INTERRUPT_EN = BIT(3),
};

static inline void gpio_core_wakeup_enable_all (int en)
{
    if (en) {
        BM_SET(reg_gpio_wakeup_and_irq_en, FLD_GPIO_CORE_WAKEUP_EN);
    }
    else {
        BM_CLR(reg_gpio_wakeup_and_irq_en, FLD_GPIO_CORE_WAKEUP_EN);
    }
}

static inline void gpio_core_irq_enable_all (int en)
{
    if (en) {
        BM_SET(reg_gpio_wakeup_and_irq_en, FLD_GPIO_CORE_INTERRUPT_EN);
    }
    else {
        BM_CLR(reg_gpio_wakeup_and_irq_en, FLD_GPIO_CORE_INTERRUPT_EN);
    }
}

static inline int gpio_is_output_en(u32 pin){
	return !BM_IS_SET(reg_gpio_oen(pin), pin & 0xff);
}

static inline void gpio_set_output_en(u32 pin, u32 value)
{
	u8	bit = pin & 0xff;
	if(!value){
		BM_SET(reg_gpio_oen(pin), bit);
	}else{
		BM_CLR(reg_gpio_oen(pin), bit);
	}
}

static inline void gpio_write(u32 pin, u32 value){
	u8	bit = pin & 0xff;
	if(value){
		BM_SET(reg_gpio_out(pin), bit);
	}else{
		BM_CLR(reg_gpio_out(pin), bit);
	}
}

static inline void gpio_toggle(u32 pin) {
	reg_gpio_out(pin) ^= (pin & 0xFF);
}


static inline u32 gpio_read(u32 pin){
	return BM_IS_SET(reg_gpio_in(pin), pin & 0xff);
}

static inline u32 gpio_read_cache(u32 pin, u8 *p){
	return p[pin>>8] & (pin & 0xff);
}

static inline void gpio_read_all(u8 *p){
	p[0] = REG_ADDR8(0x580);//PA
	p[1] = REG_ADDR8(0x588);//PB
	p[2] = REG_ADDR8(0x590);//PC
//	p[3] = REG_ADDR8(0x598);
//	p[4] = REG_ADDR8(0x5a0);//PE
//	p[5] = REG_ADDR8(0x5a8);
}

static inline void gpio_set_interrupt_pol(u32 pin, u32 falling){
	u8	bit = pin & 0xff;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}


static inline void gpio_en_interrupt(u32 pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_EN
	u8	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_wakeup_en(pin), bit);
	}
}

static inline void gpio_set_interrupt(u32 pin, u32 falling){
	u8	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}


static inline void gpio_en_interrupt_risc0(u32 pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
	u8	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc0_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc0_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc0(u32 pin, u32 falling){
	u8	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc0_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

static inline void gpio_en_interrupt_risc1(u32 pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN
	u8	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc1_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc1_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc1(u32 pin, u32 falling){
	u8	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc1_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

static inline void gpio_en_interrupt_risc2(u32 pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_RISC2_EN
	u8	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc2_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc2_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc2(u32 pin, u32 falling){
	u8	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc2_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}


static inline void gpio_init(void){
	//return;
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

	/* PE Init ---------------------------------------------------------------*/
	//PE IE settings
//	reg_gpio_pe_ie = (PE0_INPUT_ENABLE<<0)|(PE1_INPUT_ENABLE<<1)|(PE2_INPUT_ENABLE<<2)|(PE3_INPUT_ENABLE<<3);
	//PE OEN settings
//	reg_gpio_pe_oen = (PE0_OUTPUT_ENABLE ?0:BIT(0))|(PE1_OUTPUT_ENABLE ?0:BIT(1))|(PE2_OUTPUT_ENABLE ?0:BIT(2))|(PE3_OUTPUT_ENABLE ?0:BIT(3));
	//PE Output settings
//	reg_gpio_pe_out = (PE0_DATA_OUT<<0)|(PE1_DATA_OUT<<1)|(PE2_DATA_OUT<<2)|(PE3_DATA_OUT<<3);
	//PE DS settings
//	reg_gpio_pe_ds = (PE0_DATA_STRENGTH<<0)|(PE1_DATA_STRENGTH<<1)|(PE2_DATA_STRENGTH<<2)|(PE3_DATA_STRENGTH<<3);
	//PE GPIO function
//	reg_gpio_pe_gpio = (PE0_FUNC == AS_GPIO ?BIT(0):0)|(PE1_FUNC == AS_GPIO ?BIT(1):0)|(PE2_FUNC == AS_GPIO ?BIT(2):0)|(PE3_FUNC == AS_GPIO ?BIT(3):0);

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



void gpio_setup_up_down_resistor(u32 gpio, u32 up_down);
void gpio_set_50k_pullup(u8 enable);
int gpio_is_input_en(u32 pin);
void gpio_set_input_en(u32 pin, u32 value);
void gpio_set_data_strength(u32 pin, u32 value);
void gpio_set_wakeup(u32 pin, u32 pol, int en);
void gpio_set_func(u32 pin, u32 func);
void gpio_clear_gpio_int_flag(void);
