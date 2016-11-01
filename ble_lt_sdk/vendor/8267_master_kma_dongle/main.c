
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

	if(reg_irq_src & FLD_IRQ_IRQ4_EN){
		usb_endpoints_irq_handler();
	}


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
	cpu_wakeup_init();

	clock_init();
	set_tick_per_us (32);

	gpio_init();

	rf_drv_init(CRYSTAL_TYPE);

	usb_init ();

	user_init ();

    irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}
