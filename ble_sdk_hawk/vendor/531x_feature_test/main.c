#include "app.h"
#include <tl_common.h>
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/user_config.h"


_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler();
}

int main(void){

	cpu_wakeup_init();

	clock_init();

	pm_init(LSC_32kSrc_RC);

	gpio_init();

	rf_drv_init(RF_MODE_BLE_1M);

	user_init();

	irq_enable();

	while(1)
	{
	#if(MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
	#endif
		main_loop ();
	}
}


