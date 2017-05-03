
#include "../../proj/tl_common.h"

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"

#if (__PROJECT_8261_BLE_WEIXIN__)

extern void user_init();
extern void deep_wakeup_proc(void);

_attribute_ram_code_ void irq_handler(void)
{

	irq_blt_sdk_handler ();

}

int main (void) {
	cpu_wakeup_init();

	set_tick_per_us(CLOCK_SYS_CLOCK_HZ/1000000);

	clock_init();

	gpio_init();

	deep_wakeup_proc();

	rf_drv_init(CRYSTAL_TYPE);

	user_init ();

    irq_enable();
    sleep_us(3*1000*1000);//for PM Debug.
	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}



#endif
