#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/blt_config.h"


extern void user_init();

_attribute_ram_code_ void irq_handler(void)
{

	irq_blt_sdk_handler ();

}


int main (void) {

	cpu_wakeup_init();

	//clock_init();

	//rf_drv_init(CRYSTAL_TYPE);

	//REG_ADDR8(0x66) = 0x43;			//change to PLL clock: 48/3 = 16M
	//analog_write (0x05, 0x06);		//turn off 32M RC clock to turn off DCDC
	//analog_write (0xb7, 0xb0);		//turn on RF clock

	gpio_init();

	user_init ();

    irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}
