
#include "register.h"
#include "clock.h"
#include "irq.h"



void clock_init()
{




}


unsigned long pm_systick_offset = 0;

_attribute_ram_code_ u32 clock_time(void)
{
	u8 r = irq_disable();

	u32 ret = ( reg_system_tick + pm_systick_offset);

	irq_restore(r);

	return ret;
}

_attribute_ram_code_ void sleep_us (unsigned long us)
{
	unsigned long t = clock_time();
	while(!clock_time_exceed(t, us)){
	}
}


