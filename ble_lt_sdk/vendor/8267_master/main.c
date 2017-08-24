
#include "../../proj/tl_common.h"

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"

extern void user_init();
extern void deep_wakeup_proc(void);

_attribute_ram_code_ void irq_handler(void)
{
	irq_ble_master_handler ();

#if (HCI_ACCESS==HCI_USE_UART)
    unsigned char irqS = uart_IRQSourceGet();
    if(irqS & BIT(0))	//rx
    {
    	extern unsigned char uart_rx_true;
        uart_rx_true = 1;
    }

    if(irqS & BIT(1))	//tx
    {
        uart_clr_tx_busy_flag();
    }
#endif
}

int main (void) {
	blc_pm_select_internal_32k_crystal();

	blt_set_masetr_role();  //as master
	cpu_wakeup_init();

	set_tick_per_us (32);
	clock_init();

	gpio_init();

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
