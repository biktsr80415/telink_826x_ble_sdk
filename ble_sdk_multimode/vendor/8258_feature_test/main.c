#include "app.h"
#include <tl_common.h>
#include "drivers.h"
#include "app_config.h"
#include "stack/ble/ble.h"
#include "vendor/common/user_config.h"


extern my_fifo_t hci_rx_fifo;

extern void user_init_normal();
extern void user_init_deepRetn();

extern void main_loop (void);




_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler();

#if(FEATURE_TEST_MODE == TEST_BLE_PHY)
	app_phytest_irq_proc();
#endif
}

_attribute_ram_code_ int main(void)
{

	cpu_wakeup_init();

	int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp

	rf_drv_init(RF_MODE_BLE_1M);

	gpio_init(!deepRetWakeUp);

	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
		clock_init(SYS_CLK_16M_Crystal);
	#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
		clock_init(SYS_CLK_24M_Crystal);
	#endif

	if( deepRetWakeUp ){
		user_init_deepRetn ();
	}
	else{
		user_init_normal ();
	}

	irq_enable();

	while(1)
	{
	#if(MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
	#endif
		main_loop ();
	}
}


