#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"

#include "../../vendor/common/user_config.h"



extern void user_init_normal();
extern void user_init_deepRetn();

extern void main_loop (void);



_attribute_ram_code_ void irq_handler(void)
{

	irq_blt_sdk_handler ();

}





_attribute_ram_code_ int main (void)    //must run in ramcode
{

	cpu_wakeup_init();

	clock_init(SYS_CLK_16M_Crystal);

	rf_drv_init(RF_MODE_BLE_1M);

	gpio_init();


	user_init_normal ();


    irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}

