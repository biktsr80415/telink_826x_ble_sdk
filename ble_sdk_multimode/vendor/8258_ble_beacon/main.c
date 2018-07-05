
//#include "../../proj/tl_common.h"
//#include "../../vendor/common/user_config.h"
#include "tl_common.h"

//#include "../../proj/mcu/watchdog_i.h"
#include "drivers.h"


//#include "../../proj_lib/rf_drv.h"
//#include "../../proj_lib/pm.h"
//#include "../../proj_lib/ble/ll/ll.h"
#include "stack/ble/ble.h"


extern void user_init();
extern void main_loop (void);

_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler() ;
}

int main (void) {

	cpu_wakeup_init();

	//no more 32k pm select in 8258
	//blc_pm_select_internal_32k_crystal();

	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
		clock_init(SYS_CLK_16M_Crystal);
	#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
		clock_init(SYS_CLK_24M_Crystal);
	#endif

	gpio_init();

	rf_drv_init(RF_MODE_BLE_1M);

    user_init ();

    irq_enable();

	while (1) {
		main_loop ();

		//TODO : enable printf
		//printf(".");
	}
}

