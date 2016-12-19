#include "uart_sim.h"

u32 pin_tx = 0;

_attribute_ram_code_ void delay_etu()
{
	sleep_us(ETU_US_9600);
}

_attribute_ram_code_ void delay_half_etu()
{
	sleep_us(ETU_US_9600/2);
}

_attribute_ram_code_ void uart_sim_send_one_byte(u8 bData)
{
	//send start
	u8 i = 0x80;
	gpio_write(pin_tx,0);
	delay_etu();

	while(i)
	{
		gpio_write(pin_tx,bData&0x01);
		bData >>= 1;
		i >>= 1;
		delay_etu();
	}

	gpio_write(pin_tx,1);
	delay_etu();
	return;
}


void uart_sim_init(u32 pin)
{
	pin_tx = pin;

	gpio_set_func(pin_tx, AS_GPIO);

	gpio_set_output_en(pin_tx, 1);
	gpio_set_input_en(pin_tx, 0);

	gpio_write(pin_tx, 1);
	gpio_setup_up_down_resistor(pin_tx,PM_PIN_PULLUP_10K);

	return;
}
