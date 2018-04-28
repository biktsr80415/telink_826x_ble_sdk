#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "rc_ir.h"

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





//_attribute_ram_code_
int main (void) {

//	DBG_CHN7_LOW;   //debug   PB7 low

	cpu_wakeup_init();

//	clock_init();
#if (CLOCK_SYS_CLOCK_HZ == 16000000)
	REG_ADDR8(0x66) = 0x43;			//change to PLL clock: 48/3 = 16M
#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
	REG_ADDR8(0x66) = 0x42;         //change to PLL clock: 48/2 = 24M
#endif

	rf_drv_init(CRYSTAL_TYPE);

	gpio_init();

//	DBG_CHN0_HIGH;  //debug

	deep_wakeup_proc();

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

