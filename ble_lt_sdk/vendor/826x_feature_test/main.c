#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj/drivers/uart.h"

#if (__PROJECT_8261_FEATURE_TEST__ || __PROJECT_8266_FEATURE_TEST__ || __PROJECT_8267_FEATURE_TEST__ || __PROJECT_8269_FEATURE_TEST__)

extern my_fifo_t hci_rx_fifo;
extern void user_init();
extern void main_loop (void);


_attribute_ram_code_ void irq_handler(void)
{

	irq_blt_sdk_handler ();

#if (FEATURE_TEST_MODE == TEST_BLE_PHY)
	app_phytest_irq_porc();
#endif
}

int main (void) {
	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init(CRYSTAL_TYPE);

	set_tick_per_us (CLOCK_SYS_CLOCK_HZ/1000000);
	clock_init();

	gpio_init();

	///NOTE:This function must be placed before the following function rf_drv_init().
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

	rf_drv_init(CRYSTAL_TYPE);

	user_init ();

    irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}




#endif   //end of __PROJECT_826x_FEATURE_TEST__
