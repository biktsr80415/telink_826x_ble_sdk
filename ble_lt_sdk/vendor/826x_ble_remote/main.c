
#include "../../proj/tl_common.h"

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"

#if REMOTE_IR_ENABLE
#include "rc_ir.h"
#endif

#if (__PROJECT_8261_BLE_REMOTE__ || __PROJECT_8266_BLE_REMOTE__ || __PROJECT_8267_BLE_REMOTE__ || __PROJECT_8269_BLE_REMOTE__)

extern void user_init();
extern void main_loop (void);
extern void deep_wakeup_proc(void);

_attribute_ram_code_ void irq_handler(void)
{
#if (REMOTE_IR_ENABLE)
	u32 src = reg_irq_src;
	if (src & FLD_IRQ_TMR1_EN) {
		ir_irq_send();
		reg_tmr_sta = FLD_TMR_STA_TMR1;
	}

	if (src & FLD_IRQ_TMR2_EN) {
		ir_repeat_handle();
		reg_tmr_sta = FLD_TMR_STA_TMR2;
	}

	if (src & IR_LEARN_INTERRUPT_MASK) {    //红外学习IO口产生中断
		//if (!gpio_read(GPIO_IR_LEARN_IN)) {
		if (gpio_read(GPIO_IR_LEARN_IN)) {
			gpio_toggle(GPIO_PA0);
			ir_learn_irq_handler();
		}
		//reg_irq_src = FLD_IRQ_GPIO_EN;
		gpio_toggle(GPIO_PA1);
	}
#endif

	irq_blt_sdk_handler ();

}

int main (void) {

	cpu_wakeup_init();

	set_tick_per_us (CLOCK_SYS_CLOCK_HZ/1000000);
	clock_init();

	gpio_init();

	deep_wakeup_proc();

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



#endif
