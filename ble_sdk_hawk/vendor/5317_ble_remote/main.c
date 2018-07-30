#include "app.h"
#include <tl_common.h>
#include "drivers.h"
#include "stack/ble/ble.h"
#include "vendor/common/user_config.h"


#if(REMOTE_IR_ENABLE)
#include "rc_ir.h"
#endif

_attribute_ram_code_ void irq_handler(void)
{
#if(REMOTE_IR_ENABLE)
	IR_IRQHandler();
#endif

	irq_blt_sdk_handler();

#if(BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE)
	extern void irq_phyTest_handler(void);
	irq_phyTest_handler();
#endif
}

int main(void){

	pm_init(LSC_32kSrc_RC);

	cpu_wakeup_init();

	clock_init();

	gpio_init();

	deep_wakeup_proc();

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


