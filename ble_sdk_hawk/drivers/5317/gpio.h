
#pragma once

#include "driver_config.h"
#include "register.h"
#include "gpio_default.h"

typedef enum{
	GPIO_GROUPA    = 0x000,
	GPIO_GROUPB    = 0x100,
	GPIO_GROUPC    = 0x200,

	GPIO_PA0 = 0x000 | BIT(0),
	GPIO_PA1 = 0x000 | BIT(1),
	GPIO_PA2 = 0x000 | BIT(2),
	GPIO_PA3 = 0x000 | BIT(3),
	GPIO_PA4 = 0x000 | BIT(4),
	GPIO_PA5 = 0x000 | BIT(5),
	GPIO_PA6 = 0x000 | BIT(6),
	GPIO_PA7 = 0x000 | BIT(7),

	GPIO_PB0 = 0x100 | BIT(0),
	GPIO_PB1 = 0x100 | BIT(1),
	GPIO_PB2 = 0x100 | BIT(2),
	GPIO_PB3 = 0x100 | BIT(3),
	GPIO_PB4 = 0x100 | BIT(4),
	GPIO_PB5 = 0x100 | BIT(5),
	GPIO_PB6 = 0x100 | BIT(6),
	GPIO_PB7 = 0x100 | BIT(7),

	GPIO_PC0 = 0x200 | BIT(0),
	GPIO_PC1 = 0x200 | BIT(1),
	GPIO_PC2 = 0x200 | BIT(2),
	GPIO_PC3 = 0x200 | BIT(3),
	GPIO_PC4 = 0x200 | BIT(4),
	GPIO_PC5 = 0x200 | BIT(5),
	GPIO_PC6 = 0x200 | BIT(6),
	GPIO_PC7 = 0x200 | BIT(7),//SWS

	GPIO_NONE = 0xfff,
}GPIO_PinTypeDef;

typedef enum {
	GPIO_PULL_UP_DOWN_FLOAT = 0,
	GPIO_PULL_UP_1M     	= 1,
	GPIO_PULL_UP_10K 		= 2,
	GPIO_PULL_DOWN_100K  	= 3,
}GPIO_PullTypeDef;

typedef enum{
	GPIO_Level_Low  = 0x00,
	GPIO_Level_High = 0x01,
}GPIO_LevelTypeDef;

typedef enum{
	GPIO_Pol_rising  = 0x00,
	GPIO_Pol_falling = 0x01,
}GPIO_PolTypeDef;

typedef enum{
	AS_GPIO    = 0,
	AS_AF      = (!0),

	AS_MSPI    = 1,
	AS_SWIRE   = 2,
	AS_UART    = 3,
	AS_I2C     = 4,
	AS_I2C_M   = 4,
	AS_I2C_S   = 5,
	AS_SPI	   = 6,
	AS_ETH_MAC = 7,
	AS_I2S	   = 8,
	AS_SDM	   = 9,
	AS_DMIC    = 10,
	AS_USB	   = 11,
	AS_SWS	   = 12,
	AS_SWM	   = 13,
	AS_ADC	   = 14,

	AS_PWM0 	= 20,
	AS_PWM1		= 21,
	AS_PWM2 	= 22,
	AS_PWM3		= 23,
	AS_PWM4 	= 24,
	AS_PWM5		= 25,
	AS_PWM0_N	= 26,
	AS_PWM1_N	= 27,
	AS_PWM2_N	= 28,
	AS_PWM3_N	= 29,
	AS_PWM4_N	= 30,
	AS_PWM5_N	= 31,
}GPIO_FuncTypeDef;
/* PA Port Alternative Function define */
//PA0
#define GPIOA0_PWM0                       0
#define GPIOA0_PWM3                       1
#define GPIOA0_RESERVE_2                  2
#define GPIOA0_RESERVE_3                  3
//PA1
#define GPIOA1_PWM3N_OR_PWM0N             0
#define GPIOA1_UART_TX                    1
#define GPIOA1_I2C_MCK                    2
#define GPIOA1_RESERVE_3                  3
//PA2
#define GPIOA2_PWM1N                      0
#define GPIOA2_UART_RX                    1
#define GPIOA2_I2C_MSD                    2
#define GPIOA2_RESERVE_3                  3
//PA3
#define GPIOA3_PWM4                       0
#define GPIOA3_DMIC_DAT                   1
#define GPIOA3_RESERVE_2                  2
#define GPIOA3_RESERVE_3                  3
//PA4
#define GPIOA4_PWM2                       0
#define GPIOA4_DMIC_CLK                   1
#define GPIOA4_RESERVE_2                  2
#define GPIOA4_RESERVE_3                  3
//PA5
#define GPIOA5_PWM5                       0
#define GPIOA5_I2C_CK                     1
#define GPIOA5_I2C_MCK                    2
#define GPIOA5_RESERVE_3                  3
//PA6
#define GPIOA6_PWM4N                      0
#define GPIOA6_I2C_SD                     1
#define GPIOA6_I2C_MSD                    2
#define GPIOA6_SPI_MCN                    3
//PA7
#define GPIOA7_PWM5                       0
#define GPIOA7_DMIC_CLK                   1
#define GPIOA7_TX_CYC2PA                  2
#define GPIOA7_SPI_MDO                    3

/* PB Port Alternative Function define */
//PB0
#define GPIOB0_PWM3                       0
#define GPIOB0_DMIC_DAT                   1
#define GPIOB0_RX_CYC2LNA                 2
#define GPIOB0_SPI_MDI                    3
//PB1
#define GPIOB1_PWM1                       0
#define GPIOB1_RESERVE_1                  1
#define GPIOB1_TX_CYC2LNA                 2
#define GPIOB1_SPI_MCK                    3
//PB2
#define GPIOB2_PWM2                       0
#define GPIOB2_SDM_P                      1
#define GPIOB2_UART_CTS                   2
#define GPIOB2_RESERVR_3                  3
//PB3
#define GPIOB3_PWM0                       0
#define GPIOB3_SDM_N                      1
#define GPIOB3_UART_RTS                   2
#define GPIOB3_RESERVE_3                  3
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
#define GPIOB6_PWM5N                      0
#define GPIOB6_I2C_CK                     1
#define GPIOB6_UART_RTS                   2
#define GPIOB6_RESERVE_3                  3
//PB7
#define GPIOB7_PWM1                       0
#define GPIOB7_I2C_SD                     1
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
#define GPIOC2_SPI_MCN_OR_I2S_LR          2
#define GPIOC2_UART_CTS                   3
//PC3
#define GPIOC3_SPI_DO                     0
#define GPIOC3_PWM5N                      1
#define GPIOC3_SPI_MDO_I2S_DI             2
#define GPIOC3_UART_RTS                   3
//PC4
#define GPIOC4_SPI_DI_OR_I2C_SD           0
#define GPIOC4_I2C_MSD                    1
#define GPIOC4_SPI_MDI_I2S_DO             2
#define GPIOC4_UART_TX                    3
//PC5
#define GPIOC5_SPI_CK_OR_I2C_CK           0
#define GPIOC5_I2C_MCK                    1
#define GPIOC5_SPI_MCK_I2S_CLK            2
#define GPIOC5_UART_RX                    3
//PC6
#define GPIOC6_PWM4                       0
#define GPIOC6_PWM3N                      1
#define GPIOC6_RESERVE_2                  2
#define GPIOC6_RESERVE_3                  3
//PC7
#define GPIOC7_SWS                        0
#define GPIOC7_PWM5                       1
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

static inline int gpio_is_output_en(GPIO_PinTypeDef pin){
	return !BM_IS_SET(reg_gpio_oen(pin), pin & 0xff);
}

static inline void gpio_set_output_en(GPIO_PinTypeDef pin, unsigned int value)
{
	unsigned char	bit = pin & 0xff;
	if(!value){
		BM_SET(reg_gpio_oen(pin), bit);
	}else{
		BM_CLR(reg_gpio_oen(pin), bit);
	}
}

static inline void gpio_write(GPIO_PinTypeDef pin, unsigned int value){
	unsigned char	bit = pin & 0xff;
	if(value){
		BM_SET(reg_gpio_out(pin), bit);
	}else{
		BM_CLR(reg_gpio_out(pin), bit);
	}
}

static inline void gpio_toggle(GPIO_PinTypeDef pin) {
	reg_gpio_out(pin) ^= (pin & 0xFF);
}


static inline unsigned char gpio_read(GPIO_PinTypeDef pin){
	return BM_IS_SET(reg_gpio_in(pin), pin & 0xff);
}

static inline unsigned char gpio_read_cache(GPIO_PinTypeDef pin, unsigned char *p){
	return p[pin>>8] & (pin & 0xff);
}

static inline void gpio_read_all(unsigned char *p){
	p[0] = REG_ADDR8(0x580);//PA
	p[1] = REG_ADDR8(0x588);//PB
	p[2] = REG_ADDR8(0x590);//PC
}

static inline void gpio_set_interrupt_pol(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling){
	unsigned char	bit = pin & 0xff;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

static inline void gpio_en_interrupt(GPIO_PinTypeDef pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_EN
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_wakeup_en(pin), bit);
	}
}

static inline void gpio_set_interrupt(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling){
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}


static inline void gpio_en_interrupt_risc0(GPIO_PinTypeDef pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc0_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc0_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc0(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling){
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc0_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

static inline void gpio_en_interrupt_risc1(GPIO_PinTypeDef pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc1_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc1_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc1(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling){
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc1_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

static inline void gpio_en_interrupt_risc2(GPIO_PinTypeDef pin, int en){  // reg_irq_mask: FLD_IRQ_GPIO_RISC2_EN
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc2_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc2_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc2(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling){
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc2_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}

void gpio_init(void);
void gpio_setup_up_down_resistor(GPIO_PinTypeDef gpio, GPIO_PullTypeDef up_down);
void gpio_set_50k_pullup(unsigned char enable);
int  gpio_is_input_en(GPIO_PinTypeDef pin);
void gpio_set_input_en(GPIO_PinTypeDef pin, unsigned int value);
void gpio_set_data_strength(GPIO_PinTypeDef pin, unsigned int value);
void gpio_set_wakeup(GPIO_PinTypeDef pin, GPIO_LevelTypeDef level, int en);
void gpio_set_func(GPIO_PinTypeDef pin, GPIO_FuncTypeDef func);
void gpio_clear_gpio_irq_flag(void);
