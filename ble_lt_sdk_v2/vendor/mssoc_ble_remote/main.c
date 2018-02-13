#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"


extern void user_init_normal();
extern void user_init_deepRetn();

extern void main_loop (void);

extern void deep_wakeup_proc(void);

_attribute_ram_code_ void irq_handler(void)
{

	irq_blt_sdk_handler ();

}





_attribute_ram_code_
int main (void) {

//	gpio_write(GPIO_CHN4, 0);  //debug

	cpu_wakeup_init();

	//clock_init();
	REG_ADDR8(0x66) = 0x43;			//change to PLL clock: 48/3 = 16M

	rf_drv_init(CRYSTAL_TYPE);

	analog_write(0x08, 0x44);  //0x44:3.3V  0x4c:1.4V

	gpio_init();

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

