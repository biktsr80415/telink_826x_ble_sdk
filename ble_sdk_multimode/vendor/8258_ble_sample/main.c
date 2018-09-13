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

	DBG_CHN0_LOW;   //debug

	cpu_wakeup_init();

	rf_drv_init(RF_MODE_BLE_1M);

	gpio_init(1);

#if (CLOCK_SYS_CLOCK_HZ == 16000000)
	clock_init(SYS_CLK_16M_Crystal);
#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
	clock_init(SYS_CLK_24M_Crystal);
#endif



#if	(PM_DEEPSLEEP_RETENTION_ENABLE)
	if( pm_is_MCU_deepRetentionWakeup() ){
		user_init_deepRetn ();
	}
	else
#endif
	{
		user_init_normal ();
	}


    irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}

