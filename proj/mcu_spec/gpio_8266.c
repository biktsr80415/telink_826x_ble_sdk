

#include "../config/user_config.h"
#include "../mcu/config.h"

//#if (1)
#if(__TL_LIB_8266__ || (MCU_CORE_TYPE && MCU_CORE_TYPE == MCU_CORE_8266))

#include "../common/types.h"
#include "../common/compatibility.h"
#include "../common/bit.h"
#include "../common/utility.h"
#include "../common/static_assert.h"
#include "../mcu/compiler.h"
#include "../mcu/register.h"
#include "../mcu/anareg.h"
#include "../mcu/analog.h"

#include "../mcu/gpio.h"



#define    MOUSE_GPIO_MAX    19
u32 mouse_gpio_table[MOUSE_GPIO_MAX] =           //for mouse only       u16 is enough
{
	    GPIO_PA0, // GPIO_SWS		0
		GPIO_PA1, // GPIO_PWM3		1
		GPIO_PA2, // GPIO_MSDI		2
		GPIO_PA3, // GPIO_MCLK		3
		GPIO_PB2, // GPIO_MSDO		4
		GPIO_PB3, // GPIO_MSCN		5
		GPIO_PB5, // GPIO_DM		6
		GPIO_PB6, // GPIO_DP		7
		GPIO_PC0, // GPIO_PWM0		8
		GPIO_PC1, // GPIO_GP1		9
		GPIO_PC2, // GPIO_PWM1		10
		GPIO_PC4, // GPIO_PWM2		11
		GPIO_PC5, // GPIO_GP3		12
		GPIO_PC6, // GPIO_GP4		13
		GPIO_PE0, // GPIO_GP14		14
		GPIO_PE6, // GPIO_CN		15
		GPIO_PE7, // GPIO_DI		16
		GPIO_PF0, // GPIO_DO		17
		GPIO_PF1, // GPIO_CK		18
};


/************
 *
 * gpio:         indicate the pin
 * up_down:      1 need pull up, 0 need pull down
 */
const unsigned char resistor_at[MOUSE_GPIO_MAX*2] ={
	0x0a, 4,     //PA0	SWS		0
	0x0a, 6,     //PA1	PWM3	1
	0x0b, 0,     //PA2	MSDI	2
	0x0b, 2,     //PA3	MCLK	3

	0x0d, 0,	 //PB2	MSDO	4
	0x0d, 2,	 //PB3	MSCN	5
	0x0d, 6,	 //PB5	DM		6
	0x0e, 0,	 //PB6	DP		7

	0x0e, 4,	 //PC0	PWM0	8
	0x0e, 6,	 //PC1	GP1		9
	0x0f, 0,	 //PC2	PWM1	10
	0x0f, 4,	 //PC4	PWM2	11
	0x0f, 6,	 //PC5	GP3		12
	0x10, 0,	 //PC6	GP4		13

	0x12, 4,	 //PE0	GP14	14
	0x14, 0,	 //PE6	CN		15
	0x14, 2,	 //PE7	DI		16
	0x14, 4,	 //PF0	DO		17
	0x14, 6,	 //PF1	CK		18
};

void gpio_setup_up_down_resistor(u32 gpio, u32 up_down)
{
	u8 r_val;

	if( up_down == PM_PIN_UP_DOWN_FLOAT )
		r_val = 0;
	else if(up_down)
		r_val = PM_PIN_PULLUP_1M;
	else
		r_val = PM_PIN_PULLDOWN_100K;

	analog_write(resistor_at[gpio*2],
			(analog_read(resistor_at[gpio*2]) & (~(0x3<<resistor_at[gpio*2+1])))
												 |(r_val<<resistor_at[gpio*2+1]));
}

void gpio_set_wakeup(u32 pin, u32 level, int en)
{
    u8 bit = pin & 0xff;
    if (en) {
        BM_SET(reg_gpio_irq_en0(pin), bit);
    }
    else {
        BM_CLR(reg_gpio_irq_en0(pin), bit);
    }
    if(level){
        BM_CLR(reg_gpio_pol(pin), bit);
    }else{
        BM_SET(reg_gpio_pol(pin), bit);
    }
}


#endif

