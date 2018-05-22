#include "tl_common.h"
#include "drivers.h"

#include "../../vendor/common/user_config.h"
#include "../../stack/ble/ll/ll.h"
#include "../../stack/ble/blt_config.h"


extern void user_init_normal();
extern void user_init_deepRetn();

extern void main_loop (void);

extern void deep_wakeup_proc(void);

extern void rc_ir_irq_prc(void);

_attribute_ram_code_ void irq_handler(void)
{
#if (REMOTE_IR_ENABLE)
	rc_ir_irq_prc();
#endif

	irq_blt_sdk_handler ();

}





_attribute_ram_code_ int main (void)    //must run in ramcode
{
	cpu_wakeup_init();

	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
		clock_init(SYS_CLK_16M_Crystal);
	#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
		clock_init(SYS_CLK_24M_Crystal);
	#endif

	rf_drv_init(RF_MODE_BLE_1M);

	gpio_init();

	//deep_wakeup_proc();

	if( pm_is_MCU_deepRetentionWakeup() ){
		user_init_deepRetn ();
	}
	else{
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

