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

_attribute_ram_code_ void irq_handler(void)
{

	irq_blt_sdk_handler ();

}





int main (void) {


	cpu_wakeup_init();

	//clock_init();
	REG_ADDR8(0x66) = 0x43;			//change to PLL clock: 48/3 = 16M
	//analog_write (0x05, 0x06);		//turn off 32M RC clock to turn off DCDC
//	analog_write (0xb7, 0xb0);		//turn on RF clock

	rf_drv_init(CRYSTAL_TYPE);

	analog_write(0x01, 0x4d);
	write_reg16(0x420, 560);



	gpio_init();
	DBG_CHN0_HIGH;//debug suspend

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

