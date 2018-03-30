#include "../config/user_config.h"
#include "../mcu/config.h"

#if(__TL_LIB_5316__ || (MCU_CORE_TYPE == MCU_CORE_5316))

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
void gpio_setup_up_down_resistor(u32 gpio, u32 up_down)
{
	u8 bit = gpio & 0xff;
	u8 pinId = 0;
	u16 gpioGroup = gpio & 0xf00;
	u8 gpioGroupId = gpio >> 8;
	u8 pullResistorAddr = 0;
	u8 offset = 0;
	u8 temp = 0;

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

	if(up_down == PM_PIN_PULLDOWN_100K &&
	   (gpio == GPIO_PA6 || gpio == GPIO_PA7 || gpioGroup == GPIO_GROUPB))
	{
		//disable 50k pull up of PA6-PA7 and PB0-PB7
		temp = analog_read(0x05);
		temp |= 0x80;
		analog_write(0x05,temp);
	}
}

/**
 * @Brief:Disable or Enable 50k pull up resistor. only PA6-PA7 and PB0-PB7 have
 *        50K pull up resistor.
 * @Param: enable -> 0: disable;
 *                   1: enable. (default enable for MCU)
 * @Reval: None.
 */
void gpio_set_50k_pullup(u8 enable)
{
	u8 temp = 0;

	if(enable)
	{
		temp = analog_read(0x05);
		temp &= ~0x80;
		analog_write(0x05,temp);
	}
	else
	{
		temp = analog_read(0x05);
		temp |= 0x80;
		analog_write(0x05,temp);
	}
}

int gpio_is_input_en(u32 pin)
{
	u16 gpioGroup = pin & 0x0f00;
	u8 bit = pin & 0xff;

	if(gpioGroup == GPIO_GROUPA)
	{
		if((bit >= 1) && (bit <= 16))//PA0-PA4
		{
			return BM_IS_SET(reg_gpio_ie(pin), bit);
		}
		else //PA5-PA7
		{
			u8 temp = analog_read(analogRegAddr_gpioPA5_7_ie);
			return BM_IS_SET(temp, bit);
		}
	}
	else if(gpioGroup == GPIO_GROUPB)
	{
		u8 temp = analog_read(analogRegAddr_gpioPB_ie);
		return BM_IS_SET(temp, bit);
	}

	return BM_IS_SET(reg_gpio_ie(pin), bit);
}

void gpio_set_input_en(u32 pin, u32 value)
{
	u16 gpioGroup = pin & 0x0f00;
	u8 bit = pin & 0xff;
    u8 temp = 0;

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

void gpio_set_data_strength(u32 pin, u32 value)
{
	u16 gpioGroup = pin & 0x0f00;
	u8 bit = pin & 0xff;
	u8 temp = 0;

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
void gpio_set_wakeup(u32 pin, u32 pol, int en)
{
    u8 bit = pin & 0xff;
    if (en) {
        BM_SET(reg_gpio_irq_wakeup_en(pin), bit);
    }
    else {
        BM_CLR(reg_gpio_irq_wakeup_en(pin), bit);
    }

    if(pol){
        BM_CLR(reg_gpio_pol(pin), bit);
        gpio_setup_up_down_resistor(pin,GPIO_PULL_DN_100K);
    }else{
        BM_SET(reg_gpio_pol(pin), bit);
        gpio_setup_up_down_resistor(pin,GPIO_PULL_UP_1M);
    }

    gpio_set_func(pin,AS_GPIO);
    gpio_set_input_en(pin,1);//must
    reg_gpio_wakeup_and_irq_en |= FLD_GPIO_CORE_WAKEUP_EN;
}


void gpio_set_func(u32 pin, u32 func)
{
	u8	bit = pin & 0xff;
	if(func == AS_GPIO){
		BM_SET(reg_gpio_gpio_func(pin), bit);
		return;
	}else{
		BM_CLR(reg_gpio_gpio_func(pin), bit);
	}
}

void gpio_clear_gpio_int_flag(void)
{
	REG_ADDR8(0x64A) |= BIT(2);
}

#endif

