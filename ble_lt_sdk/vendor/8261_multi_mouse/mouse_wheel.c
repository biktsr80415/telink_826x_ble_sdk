/*
 * mouse_wheel.c
 *
 *  Created on: Feb 12, 2014
 *      Author: xuzhen
 */

#include "../../proj/tl_common.h"
#include "mouse_wheel.h"

/**************************************************************************
PE0 -> 0, PE1 -> 1
PD2 -> 2, PD3 -> 3
PC4 -> 4, PC5 -> 5
PB6 -> 6, PB7 -> 7

**************************************************************************/
static inline u8 gpio_to_wheel_value(u32 pin)
{
#if 0
	if(pin & 0x7f00){
		pin>>=8;
	}
	else if(pin & 0xf0){
		pin<<=3;
	}

	u8 i = 0;
	for(i=0;i<12;i++){
		if(pin &(1<<i)){
			return	i;
		}
	}

	return 0;
#else

	u8 wheel = ( u8 )(pin);

	for(int i=0; i<8; i++){
		if(wheel & BIT(i)){
			return i;
		}
	}

#endif
}



void mouse_wheel_init(mouse_hw_t *mouse_hw)
{
#if MOUSE_SENSOR_IO_FIXED
	gpio_setup_up_down_resistor(M_HW_WHEEL_Z0,1);
	gpio_setup_up_down_resistor(M_HW_WHEEL_Z1,1);

	write_reg8(0x8000d2,gpio_to_wheel_value(M_HW_WHEEL_Z0));
	write_reg8(0x8000d3,gpio_to_wheel_value(M_HW_WHEEL_Z1));
#else
	gpio_setup_up_down_resistor(mouse_hw->wheel[0], 1 );
	gpio_setup_up_down_resistor(mouse_hw->wheel[1], 1 );

	//write_reg8(0x8000d2,0x05);   //wheel1  GP5
	//write_reg8(0x8000d3,0x00);   //wheel2  GP0
	write_reg8(0xd2,gpio_to_wheel_value(mouse_hw->wheel[0]));
	write_reg8(0xd3,gpio_to_wheel_value(mouse_hw->wheel[1]));


#endif


	u8 core_reg65 = read_reg8(0x65);
	core_reg65 |=  (BIT(0) | BIT(5));
	write_reg8(0x65,core_reg65);   //wheel clk enable

#if(WHEEL_TWO_STEP_PROC)
	write_reg8(0xd7,0x01);   //BIT(0) 0: 1¸ñ        1:  2¸ñ      	BIT(1)  wakeup enable
#else
	write_reg8(0xd7,0x02);
#endif
	write_reg8(0xd1,0x00);   //filter   00-07     00 is best
 }



void mouse_wheel_detect(mouse_status_t  * mouse_status)
{

}



