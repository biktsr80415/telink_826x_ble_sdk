
#pragma once

#include "register.h"
#include "gpio.h"


#define AS_GPIO		0
#define AS_MSPI		1
#define AS_SWIRE	2
#define AS_UART		3
#define AS_I2C		4
#define AS_SPI		5
#define AS_I2S		6
#define AS_AMIC		7
#define AS_DMIC		8
#define AS_SDM		9
#define AS_USB		10
#define AS_ADC		11
#define AS_CMP		12

#define AS_PWM0		20
#define AS_PWM1		21
#define AS_PWM2		22
#define AS_PWM3		23
#define AS_PWM4		24
#define AS_PWM5		25
#define AS_PWM0_N	26
#define AS_PWM1_N	27
#define AS_PWM2_N	28
#define AS_PWM3_N	29
#define AS_PWM4_N	30
#define AS_PWM5_N	31



enum{
		GPIO_GROUPA    = 0x000,
		GPIO_GROUPB    = 0x100,
		GPIO_GROUPC    = 0x200,
		GPIO_GROUPD    = 0x300,
		GPIO_GROUPE    = 0x400,

	    GPIO_PA0 = 0x000 | BIT(0),	GPIO_PWM0A0=GPIO_PA0,	GPIO_DMIC_DI=GPIO_PA0,
		GPIO_PA1 = 0x000 | BIT(1), 							GPIO_DMIC_CK=GPIO_PA1,
		GPIO_PA2 = 0x000 | BIT(2),	GPIO_PWM0NA2=GPIO_PA2,	GPIO_DO=GPIO_PA2,
		GPIO_PA3 = 0x000 | BIT(3),	GPIO_PWM1A3=GPIO_PA3,	GPIO_DI=GPIO_PA3,
		GPIO_PA4 = 0x000 | BIT(4), 	GPIO_PWM1NA4=GPIO_PA4,	GPIO_CK=GPIO_PA4,
		GPIO_PA5 = 0x000 | BIT(5), 	GPIO_DM=GPIO_PA5,							GPIO_PWM2NA5=GPIO_PA5,	GPIO_CN=GPIO_PA5,
		GPIO_PA6 = 0x000 | BIT(6),  GPIO_DP=GPIO_PA6,							GPIO_UTXA6=GPIO_PA6,
		GPIO_PA7 = 0x000 | BIT(7), 	GPIO_SWS=GPIO_PA7,		GPIO_URXA7=GPIO_PA7,

		GPIO_PB0 = 0x100 | BIT(0),	GPIO_PWM2B0=GPIO_PB0,
		GPIO_PB1 = 0x100 | BIT(1),	GPIO_PWM2NB0=GPIO_PB1,
		GPIO_PB2 = 0x100 | BIT(2), 	GPIO_PWM3B2=GPIO_PB2,	GPIO_UTXB2=GPIO_PB2,
		GPIO_PB3 = 0x100 | BIT(3),	GPIO_PWM3NB3=GPIO_PB3,	GPIO_URXB3=GPIO_PB3,
		GPIO_PB4 = 0x100 | BIT(4),	GPIO_PWM4B4=GPIO_PB4,
		GPIO_PB5 = 0x100 | BIT(5),	GPIO_PWM4NB5=GPIO_PB5,
		GPIO_PB6 = 0x100 | BIT(6),	GPIO_PWM5B6=GPIO_PB6,
		GPIO_PB7 = 0x100 | BIT(7),	GPIO_PWM5NB7=GPIO_PB7,

		GPIO_PC0 = 0x200 | BIT(0), GPIO_PWM0C0=GPIO_PC0,
		GPIO_PC1 = 0x200 | BIT(1), GPIO_PWM1C1=GPIO_PC1,
		GPIO_PC2 = 0x200 | BIT(2), GPIO_PWM2C2=GPIO_PC2,	GPIO_UTXC2=GPIO_PC2,
		GPIO_PC3 = 0x200 | BIT(3), GPIO_PWM3C3=GPIO_PC3, 	GPIO_URXC3=GPIO_PC3,
		GPIO_PC4 = 0x200 | BIT(4), GPIO_PWM4C4=GPIO_PC4,	GPIO_URTSC4=GPIO_PC4,
		GPIO_PC5 = 0x200 | BIT(5), GPIO_PWM4C5=GPIO_PC5,	GPIO_UCTSC5=GPIO_PC5,
		GPIO_PC6 = 0x200 | BIT(6),
		GPIO_PC7 = 0x200 | BIT(7),

		GPIO_PD0 = 0x300 | BIT(0),
		GPIO_PD1 = 0x300 | BIT(1),
		GPIO_PD2 = 0x300 | BIT(2),
		GPIO_PD3 = 0x300 | BIT(3),
		GPIO_PD4 = 0x300 | BIT(4),
		GPIO_PD5 = 0x300 | BIT(5), GPIO_PWM0D5=GPIO_PD5,
		GPIO_PD6 = 0x300 | BIT(6), GPIO_PWM1D6=GPIO_PD6,
		GPIO_PD7 = 0x300 | BIT(7), GPIO_PWM2D7=GPIO_PD7,

		GPIO_PE4 = 0x400 | BIT(4), 							GPIO_MSDO=GPIO_PE4,
		GPIO_PE5 = 0x400 | BIT(5), 							GPIO_MCLK=GPIO_PE5,
		GPIO_PE6 = 0x400 | BIT(6), 							GPIO_MSCN=GPIO_PE6,
		GPIO_PE7 = 0x400 | BIT(7), 							GPIO_MSDI=GPIO_PE7,

};



#define reg_gpio_in(i)				REG_ADDR8(0x580+((i>>8)<<3))
#define reg_gpio_ie(i)				REG_ADDR8(0x581+((i>>8)<<3))
#define anaReg_gpioPB_ie			0xBD
#define anaReg_gpioPC_ie			0xC0
#define reg_gpio_oen(i)				REG_ADDR8(0x582+((i>>8)<<3))
#define reg_gpio_out(i)				REG_ADDR8(0x583+((i>>8)<<3))
#define reg_gpio_pol(i)				REG_ADDR8(0x584+((i>>8)<<3))
#define reg_gpio_ds(i)				REG_ADDR8(0x585+((i>>8)<<3))
#define anaReg_gpioPB_ds			0xBF
#define anaReg_gpioPC_ds			0xC2

#define reg_gpio_gpio_func(i)		REG_ADDR8(0x586+((i>>8)<<3))


#define reg_gpio_irq_wakeup_en(i)	REG_ADDR8(0x587+((i>>8)<<3))  // reg_irq_mask: FLD_IRQ_GPIO_EN

#define reg_gpio_irq_risc0_en(i)  REG_ADDR8(0x5b8 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC0_EN
#define reg_gpio_irq_risc1_en(i)  REG_ADDR8(0x5c0 + (i >> 8))	  // reg_irq_mask: FLD_IRQ_GPIO_RISC1_EN



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



static inline int gpio_is_output_en(u32 pin){
	return !BM_IS_SET(reg_gpio_oen(pin), pin & 0xff);
}

static inline int gpio_is_input_en(u32 pin){
	return BM_IS_SET(reg_gpio_ie(pin), pin & 0xff);
}

static inline void gpio_set_output_en(u32 pin, u32 value){
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
	p[0] = REG_ADDR8(0x580);
	p[1] = REG_ADDR8(0x588);
	p[2] = REG_ADDR8(0x590);
	p[3] = REG_ADDR8(0x598);
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







void gpio_init(void);
void gpio_set_wakeup(u32 pin, u32 level, int en);
void gpio_setup_up_down_resistor(u32 gpio, u32 up_down);
void gpio_set_input_en(u32 pin, u32 value);
void gpio_set_func(u32 pin, u32 func);
