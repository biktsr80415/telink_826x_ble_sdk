/********************************************************************************************************
 * @file     gpio_8258.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     May. 12, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/

#pragma once

#include "register.h"
#include "gpio.h"



typedef enum{
		GPIO_GROUPA    = 0x000,
		GPIO_GROUPB    = 0x100,
		GPIO_GROUPC    = 0x200,
		GPIO_GROUPD    = 0x300,
		GPIO_GROUPE    = 0x400,

	    GPIO_PA0 = 0x000 | BIT(0),
		GPIO_PA1 = 0x000 | BIT(1),
		GPIO_PA2 = 0x000 | BIT(2),
		GPIO_PA3 = 0x000 | BIT(3),
		GPIO_PA4 = 0x000 | BIT(4),
		GPIO_PA5 = 0x000 | BIT(5), 	GPIO_DM=GPIO_PA5,
		GPIO_PA6 = 0x000 | BIT(6),  GPIO_DP=GPIO_PA6,
		GPIO_PA7 = 0x000 | BIT(7), 	GPIO_SWS=GPIO_PA7,

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
		GPIO_PC7 = 0x200 | BIT(7),

		GPIO_PD0 = 0x300 | BIT(0),
		GPIO_PD1 = 0x300 | BIT(1),
		GPIO_PD2 = 0x300 | BIT(2),
		GPIO_PD3 = 0x300 | BIT(3),
		GPIO_PD4 = 0x300 | BIT(4),
		GPIO_PD5 = 0x300 | BIT(5),
		GPIO_PD6 = 0x300 | BIT(6),
		GPIO_PD7 = 0x300 | BIT(7),

		GPIO_PE0 = 0x400 | BIT(0),  GPIO_MSDO=GPIO_PE0,
		GPIO_PE1 = 0x400 | BIT(1),  GPIO_MCLK=GPIO_PE1,
		GPIO_PE2 = 0x400 | BIT(2),  GPIO_MSCN=GPIO_PE2,
		GPIO_PE3 = 0x400 | BIT(3),  GPIO_MSDI=GPIO_PE3,

}GPIO_PinTypeDef;



typedef enum{
	NOT_AS_GPIO =  0,   // !GPIO
	AS_GPIO 	=  1,

	AS_MSPI 	=  2,
	AS_SWIRE	=  3,
	AS_UART		=  4,
	AS_I2C		=  5,
	AS_SPI		=  6,
	AS_I2S		=  7,
	AS_AMIC		=  8,
	AS_DMIC		=  9,
	AS_SDM		=  10,
	AS_USB		=  11,
	AS_ADC		=  12,
	AS_CMP		=  13,
	AS_ATS		=  14,

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




typedef enum{
	Level_Low = 0,
	Level_High,
}GPIO_LevelTypeDef;




typedef enum{
	pol_rising = 0,
	pol_falling,
}GPIO_PolTypeDef;




#define reg_gpio_wakeup_irq  REG_ADDR8(0x5b5)
enum{
    FLD_GPIO_CORE_WAKEUP_EN  = BIT(2),
    FLD_GPIO_CORE_INTERRUPT_EN = BIT(3),
};

static inline void gpio_core_wakeup_enable_all (int en)
{
    if (en) {
        BM_SET(reg_gpio_wakeup_irq, FLD_GPIO_CORE_WAKEUP_EN);
    }
    else {
        BM_CLR(reg_gpio_wakeup_irq, FLD_GPIO_CORE_WAKEUP_EN);
    }
}

static inline void gpio_core_irq_enable_all (int en)
{
    if (en) {
        BM_SET(reg_gpio_wakeup_irq, FLD_GPIO_CORE_INTERRUPT_EN);
    }
    else {
        BM_CLR(reg_gpio_wakeup_irq, FLD_GPIO_CORE_INTERRUPT_EN);
    }
}



static inline int gpio_is_output_en(GPIO_PinTypeDef pin)
{
	return !BM_IS_SET(reg_gpio_oen(pin), pin & 0xff);
}

static inline int gpio_is_input_en(GPIO_PinTypeDef pin)
{
	return BM_IS_SET(reg_gpio_ie(pin), pin & 0xff);
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






static inline void gpio_write(GPIO_PinTypeDef pin, unsigned int value)
{
	unsigned char	bit = pin & 0xff;
	if(value){
		BM_SET(reg_gpio_out(pin), bit);
	}else{
		BM_CLR(reg_gpio_out(pin), bit);
	}
}

static inline void gpio_toggle(GPIO_PinTypeDef pin)
{
	reg_gpio_out(pin) ^= (pin & 0xFF);
}


static inline unsigned int gpio_read(GPIO_PinTypeDef pin)
{
	return BM_IS_SET(reg_gpio_in(pin), pin & 0xff);
}

static inline unsigned int gpio_read_cache(GPIO_PinTypeDef pin, unsigned char *p)
{
	return p[pin>>8] & (pin & 0xff);
}

static inline void gpio_read_all(unsigned char *p)
{
	p[0] = REG_ADDR8(0x580);
	p[1] = REG_ADDR8(0x588);
	p[2] = REG_ADDR8(0x590);
	p[3] = REG_ADDR8(0x598);
}





static inline void gpio_set_interrupt_pol(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling)
{
	unsigned char	bit = pin & 0xff;
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}



static inline void gpio_en_interrupt(GPIO_PinTypeDef pin, int en)   // reg_irq_mask: FLD_IRQ_GPIO_EN
{
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_wakeup_en(pin), bit);
	}
}

static inline void gpio_set_interrupt(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling)
{
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}


static inline void gpio_en_interrupt_risc0(GPIO_PinTypeDef pin, int en)  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
{
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

static inline void gpio_en_interrupt_risc1(GPIO_PinTypeDef pin, int en)  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN
{
	unsigned char	bit = pin & 0xff;
	if(en){
		BM_SET(reg_gpio_irq_risc1_en(pin), bit);
	}
	else{
		BM_CLR(reg_gpio_irq_risc1_en(pin), bit);
	}
}

static inline void gpio_set_interrupt_risc1(GPIO_PinTypeDef pin, GPIO_PolTypeDef falling)
{
	unsigned char	bit = pin & 0xff;
	BM_SET(reg_gpio_irq_risc1_en(pin), bit);
	if(falling){
		BM_SET(reg_gpio_pol(pin), bit);
	}else{
		BM_CLR(reg_gpio_pol(pin), bit);
	}
}







void gpio_init(int anaRes_init_en);
void gpio_set_wakeup(GPIO_PinTypeDef pin, GPIO_LevelTypeDef level, int en);
void gpio_setup_up_down_resistor(GPIO_PinTypeDef gpio, GPIO_PullTypeDef up_down);
void gpio_set_input_en(GPIO_PinTypeDef pin, unsigned int value);
void gpio_set_func(GPIO_PinTypeDef pin, GPIO_FuncTypeDef func);
void gpio_config_special_func(GPIO_PinTypeDef pin, GPIO_FuncTypeDef func);












#define GPIO_PB0_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PB03_pull);  val &= ~0x03;   				analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB0_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PB03_pull);  val &= ~0x02;	val |=  0x01;   analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB0_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PB03_pull);  val |=  0x02;	val &= ~0x01;   analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB0_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PB03_pull);  val |=  0x03;   				analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB0_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PB03_pull);  val ^=  0x01;   				analog_write(anareg_PB03_pull, val); }while(0)



#define GPIO_PB1_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PB03_pull);  val &= ~0x0C;   				analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB1_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PB03_pull);  val &= ~0x08;	val |=  0x04;   analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB1_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PB03_pull);  val |=  0x08;	val &= ~0x04;   analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB1_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PB03_pull);  val |=  0x0C;   				analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB1_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PB03_pull);  val ^=  0x04;   				analog_write(anareg_PB03_pull, val); }while(0)


#define GPIO_PB2_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PB03_pull);  val &= ~0x30;   				analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB2_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PB03_pull);  val &= ~0x20;	val |=  0x10;   analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB2_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PB03_pull);  val |=  0x20;	val &= ~0x10;   analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB2_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PB03_pull);  val |=  0x30;   				analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB2_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PB03_pull);  val ^=  0x10;   				analog_write(anareg_PB03_pull, val); }while(0)


#define GPIO_PB3_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PB03_pull);  val &= ~0xC0;   				analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB3_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PB03_pull);  val &= ~0x80;	val |=  0x40;   analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB3_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PB03_pull);  val |=  0x80;	val &= ~0x40;   analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB3_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PB03_pull);  val |=  0xC0;   				analog_write(anareg_PB03_pull, val); }while(0)
#define GPIO_PB3_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PB03_pull);  val ^=  0x40;   				analog_write(anareg_PB03_pull, val); }while(0)

#define GPIO_PB4_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PB47_pull);  val &= ~0x03;   				analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB4_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PB47_pull);  val &= ~0x02;	val |=  0x01;   analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB4_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PB47_pull);  val |=  0x02;	val &= ~0x01;   analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB4_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PB47_pull);  val |=  0x03;   				analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB4_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PB47_pull);  val ^=  0x01;   				analog_write(anareg_PB47_pull, val); }while(0)


#define GPIO_PB5_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PB47_pull);  val &= ~0x0C;   				analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB5_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PB47_pull);  val &= ~0x08;	val |=  0x04;   analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB5_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PB47_pull);  val |=  0x08;	val &= ~0x04;   analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB5_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PB47_pull);  val |=  0x0C;   				analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB5_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PB47_pull);  val ^=  0x04;   				analog_write(anareg_PB47_pull, val); }while(0)

#define GPIO_PB6_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PB47_pull);  val &= ~0x30;   				analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB6_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PB47_pull);  val &= ~0x20;	val |=  0x10;   analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB6_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PB47_pull);  val |=  0x20;	val &= ~0x10;   analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB6_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PB47_pull);  val |=  0x30;   				analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB6_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PB47_pull);  val ^=  0x10;   				analog_write(anareg_PB47_pull, val); }while(0)

#define GPIO_PB7_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PB47_pull);  val &= ~0xC0;   				analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB7_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PB47_pull);  val &= ~0x80;	val |=  0x40;   analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB7_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PB47_pull);  val |=  0x80;	val &= ~0x40;   analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB7_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PB47_pull);  val |=  0xC0;   				analog_write(anareg_PB47_pull, val); }while(0)
#define GPIO_PB7_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PB47_pull);  val ^=  0x40;   				analog_write(anareg_PB47_pull, val); }while(0)



#define GPIO_PC0_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PC03_pull);  val &= ~0x03;   				analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC0_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PC03_pull);  val &= ~0x02;	val |=  0x01;   analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC0_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PC03_pull);  val |=  0x02;	val &= ~0x01;   analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC0_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PC03_pull);  val |=  0x03;   				analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC0_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PC03_pull);  val ^=  0x01;   				analog_write(anareg_PC03_pull, val); }while(0)



#define GPIO_PC1_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PC03_pull);  val &= ~0x0C;   				analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC1_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PC03_pull);  val &= ~0x08;	val |=  0x04;   analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC1_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PC03_pull);  val |=  0x08;	val &= ~0x04;   analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC1_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PC03_pull);  val |=  0x0C;   				analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC1_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PC03_pull);  val ^=  0x04;   				analog_write(anareg_PC03_pull, val); }while(0)


#define GPIO_PC2_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PC03_pull);  val &= ~0x30;   				analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC2_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PC03_pull);  val &= ~0x20;	val |=  0x10;   analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC2_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PC03_pull);  val |=  0x20;	val &= ~0x10;   analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC2_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PC03_pull);  val |=  0x30;   				analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC2_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PC03_pull);  val ^=  0x10;   				analog_write(anareg_PC03_pull, val); }while(0)


#define GPIO_PC3_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PC03_pull);  val &= ~0xC0;   				analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC3_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PC03_pull);  val &= ~0x80;	val |=  0x40;   analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC3_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PC03_pull);  val |=  0x80;	val &= ~0x40;   analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC3_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PC03_pull);  val |=  0xC0;   				analog_write(anareg_PC03_pull, val); }while(0)
#define GPIO_PC3_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PC03_pull);  val ^=  0x40;   				analog_write(anareg_PC03_pull, val); }while(0)

#define GPIO_PC4_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PC47_pull);  val &= ~0x03;   				analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC4_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PC47_pull);  val &= ~0x02;	val |=  0x01;   analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC4_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PC47_pull);  val |=  0x02;	val &= ~0x01;   analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC4_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PC47_pull);  val |=  0x03;   				analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC4_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PC47_pull);  val ^=  0x01;   				analog_write(anareg_PC47_pull, val); }while(0)


#define GPIO_PC5_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PC47_pull);  val &= ~0x0C;   				analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC5_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PC47_pull);  val &= ~0x08;	val |=  0x04;   analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC5_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PC47_pull);  val |=  0x08;	val &= ~0x04;   analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC5_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PC47_pull);  val |=  0x0C;   				analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC5_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PC47_pull);  val ^=  0x04;   				analog_write(anareg_PC47_pull, val); }while(0)

#define GPIO_PC6_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PC47_pull);  val &= ~0x30;   				analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC6_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PC47_pull);  val &= ~0x20;	val |=  0x10;   analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC6_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PC47_pull);  val |=  0x20;	val &= ~0x10;   analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC6_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PC47_pull);  val |=  0x30;   				analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC6_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PC47_pull);  val ^=  0x10;   				analog_write(anareg_PC47_pull, val); }while(0)

#define GPIO_PC7_PULL_FLOAT			do{ unsigned char val = analog_read(anareg_PC47_pull);  val &= ~0xC0;   				analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC7_PULL_UP_1M			do{ unsigned char val = analog_read(anareg_PC47_pull);  val &= ~0x80;	val |=  0x40;   analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC7_PULL_DOWN_100K		do{ unsigned char val = analog_read(anareg_PC47_pull);  val |=  0x80;	val &= ~0x40;   analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC7_PULL_UP_10K		do{ unsigned char val = analog_read(anareg_PC47_pull);  val |=  0xC0;   				analog_write(anareg_PC47_pull, val); }while(0)
#define GPIO_PC7_PULL_TOGGLE		do{ unsigned char val = analog_read(anareg_PC47_pull);  val ^=  0x40;   				analog_write(anareg_PC47_pull, val); }while(0)










#define GPIO_PA0_OUTPUT_LOW			( (*(volatile unsigned char *)0x800583) &= (~0x01) )
#define GPIO_PA0_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800583) |= 0x01    )
#define GPIO_PA0_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800583) ^= 0x01    )
#define GPIO_PA1_OUTPUT_LOW			( (*(volatile unsigned char *)0x800583) &= (~0x02) )
#define GPIO_PA1_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800583) |= 0x02    )
#define GPIO_PA1_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800583) ^= 0x02    )
#define GPIO_PA2_OUTPUT_LOW			( (*(volatile unsigned char *)0x800583) &= (~0x04) )
#define GPIO_PA2_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800583) |= 0x04    )
#define GPIO_PA2_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800583) ^= 0x04    )
#define GPIO_PA3_OUTPUT_LOW			( (*(volatile unsigned char *)0x800583) &= (~0x08) )
#define GPIO_PA3_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800583) |= 0x08    )
#define GPIO_PA3_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800583) ^= 0x08    )
#define GPIO_PA4_OUTPUT_LOW			( (*(volatile unsigned char *)0x800583) &= (~0x10) )
#define GPIO_PA4_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800583) |= 0x10    )
#define GPIO_PA4_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800583) ^= 0x10    )
#define GPIO_PA5_OUTPUT_LOW			( (*(volatile unsigned char *)0x800583) &= (~0x20) )
#define GPIO_PA5_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800583) |= 0x20    )
#define GPIO_PA5_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800583) ^= 0x20    )
#define GPIO_PA6_OUTPUT_LOW			( (*(volatile unsigned char *)0x800583) &= (~0x40) )
#define GPIO_PA6_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800583) |= 0x40    )
#define GPIO_PA6_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800583) ^= 0x40    )
#define GPIO_PA7_OUTPUT_LOW			( (*(volatile unsigned char *)0x800583) &= (~0x80) )
#define GPIO_PA7_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800583) |= 0x80    )
#define GPIO_PA7_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800583) ^= 0x80    )

#define GPIO_PB0_OUTPUT_LOW			( (*(volatile unsigned char *)0x80058b) &= (~0x01) )
#define GPIO_PB0_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80058b) |= 0x01    )
#define GPIO_PB0_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80058b) ^= 0x01    )
#define GPIO_PB1_OUTPUT_LOW			( (*(volatile unsigned char *)0x80058b) &= (~0x02) )
#define GPIO_PB1_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80058b) |= 0x02    )
#define GPIO_PB1_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80058b) ^= 0x02    )
#define GPIO_PB2_OUTPUT_LOW			( (*(volatile unsigned char *)0x80058b) &= (~0x04) )
#define GPIO_PB2_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80058b) |= 0x04    )
#define GPIO_PB2_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80058b) ^= 0x04    )
#define GPIO_PB3_OUTPUT_LOW			( (*(volatile unsigned char *)0x80058b) &= (~0x08) )
#define GPIO_PB3_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80058b) |= 0x08    )
#define GPIO_PB3_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80058b) ^= 0x08    )
#define GPIO_PB4_OUTPUT_LOW			( (*(volatile unsigned char *)0x80058b) &= (~0x10) )
#define GPIO_PB4_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80058b) |= 0x10    )
#define GPIO_PB4_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80058b) ^= 0x10    )
#define GPIO_PB5_OUTPUT_LOW			( (*(volatile unsigned char *)0x80058b) &= (~0x20) )
#define GPIO_PB5_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80058b) |= 0x20    )
#define GPIO_PB5_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80058b) ^= 0x20    )
#define GPIO_PB6_OUTPUT_LOW			( (*(volatile unsigned char *)0x80058b) &= (~0x40) )
#define GPIO_PB6_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80058b) |= 0x40    )
#define GPIO_PB6_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80058b) ^= 0x40    )
#define GPIO_PB7_OUTPUT_LOW			( (*(volatile unsigned char *)0x80058b) &= (~0x80) )
#define GPIO_PB7_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80058b) |= 0x80    )
#define GPIO_PB7_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80058b) ^= 0x80    )

#define GPIO_PC0_OUTPUT_LOW			( (*(volatile unsigned char *)0x800593) &= (~0x01) )
#define GPIO_PC0_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800593) |= 0x01    )
#define GPIO_PC0_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800593) ^= 0x01    )
#define GPIO_PC1_OUTPUT_LOW			( (*(volatile unsigned char *)0x800593) &= (~0x02) )
#define GPIO_PC1_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800593) |= 0x02    )
#define GPIO_PC1_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800593) ^= 0x02    )
#define GPIO_PC2_OUTPUT_LOW			( (*(volatile unsigned char *)0x800593) &= (~0x04) )
#define GPIO_PC2_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800593) |= 0x04    )
#define GPIO_PC2_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800593) ^= 0x04    )
#define GPIO_PC3_OUTPUT_LOW			( (*(volatile unsigned char *)0x800593) &= (~0x08) )
#define GPIO_PC3_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800593) |= 0x08    )
#define GPIO_PC3_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800593) ^= 0x08    )
#define GPIO_PC4_OUTPUT_LOW			( (*(volatile unsigned char *)0x800593) &= (~0x10) )
#define GPIO_PC4_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800593) |= 0x10    )
#define GPIO_PC4_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800593) ^= 0x10    )
#define GPIO_PC5_OUTPUT_LOW			( (*(volatile unsigned char *)0x800593) &= (~0x20) )
#define GPIO_PC5_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800593) |= 0x20    )
#define GPIO_PC5_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800593) ^= 0x20    )
#define GPIO_PC6_OUTPUT_LOW			( (*(volatile unsigned char *)0x800593) &= (~0x40) )
#define GPIO_PC6_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800593) |= 0x40    )
#define GPIO_PC6_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800593) ^= 0x40    )
#define GPIO_PC7_OUTPUT_LOW			( (*(volatile unsigned char *)0x800593) &= (~0x80) )
#define GPIO_PC7_OUTPUT_HIGH		( (*(volatile unsigned char *)0x800593) |= 0x80    )
#define GPIO_PC7_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x800593) ^= 0x80    )

#define GPIO_PD0_OUTPUT_LOW			( (*(volatile unsigned char *)0x80059b) &= (~0x01) )
#define GPIO_PD0_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80059b) |= 0x01    )
#define GPIO_PD0_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80059b) ^= 0x01    )
#define GPIO_PD1_OUTPUT_LOW			( (*(volatile unsigned char *)0x80059b) &= (~0x02) )
#define GPIO_PD1_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80059b) |= 0x02    )
#define GPIO_PD1_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80059b) ^= 0x02    )
#define GPIO_PD2_OUTPUT_LOW			( (*(volatile unsigned char *)0x80059b) &= (~0x04) )
#define GPIO_PD2_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80059b) |= 0x04    )
#define GPIO_PD2_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80059b) ^= 0x04    )
#define GPIO_PD3_OUTPUT_LOW			( (*(volatile unsigned char *)0x80059b) &= (~0x08) )
#define GPIO_PD3_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80059b) |= 0x08    )
#define GPIO_PD3_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80059b) ^= 0x08    )
#define GPIO_PD4_OUTPUT_LOW			( (*(volatile unsigned char *)0x80059b) &= (~0x10) )
#define GPIO_PD4_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80059b) |= 0x10    )
#define GPIO_PD4_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80059b) ^= 0x10    )
#define GPIO_PD5_OUTPUT_LOW			( (*(volatile unsigned char *)0x80059b) &= (~0x20) )
#define GPIO_PD5_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80059b) |= 0x20    )
#define GPIO_PD5_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80059b) ^= 0x20    )
#define GPIO_PD6_OUTPUT_LOW			( (*(volatile unsigned char *)0x80059b) &= (~0x40) )
#define GPIO_PD6_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80059b) |= 0x40    )
#define GPIO_PD6_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80059b) ^= 0x40    )
#define GPIO_PD7_OUTPUT_LOW			( (*(volatile unsigned char *)0x80059b) &= (~0x80) )
#define GPIO_PD7_OUTPUT_HIGH		( (*(volatile unsigned char *)0x80059b) |= 0x80    )
#define GPIO_PD7_OUTPUT_TOGGLE		( (*(volatile unsigned char *)0x80059b) ^= 0x80    )



