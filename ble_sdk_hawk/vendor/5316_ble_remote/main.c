#include "app.h"
#include <tl_common.h>
#include "drivers.h"
#include "stack/ble/ble.h"
#include "../common/user_config.h"

#if (REMOTE_IR_ENABLE)
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

	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init();

#if NEW_STRUCT_EN
	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
		clock_init(SYS_CLK_16M_Crystal);
	#elif (CLOCK_SYS_CLOCK_HZ == 32000000)
		clock_init(SYS_CLK_32M_Crystal);
	#endif
#else
//		clock_init();
#endif

	gpio_init();

	#if(RC_BTN_ENABLE)
		deep_wakeup_proc();
	#endif

	rf_drv_init(RF_MODE_BLE_1M);

	user_init ();

    irq_enable();

	while (1) {
	#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
	#endif
		main_loop ();
	}
}
