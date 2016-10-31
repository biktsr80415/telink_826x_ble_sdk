#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"

extern void user_init();

_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_slave_handler ();
    unsigned char irqS = uart_IRQSourceGet();
    if(irqS & BIT(0))	//rx
    {
		rx_uart_r_index = (rx_uart_r_index + 1)&0x01;
		write_reg16(0x800500,(unsigned short)((unsigned int)(&T_rxdata_buf[rx_uart_r_index])));//set receive buffer address
    }

    if(irqS & BIT(1))	//tx
    {
        uart_clr_tx_busy_flag();
    }
}

int main (void) {
	cpu_wakeup_init();

	clock_init();
	set_tick_per_us(CLOCK_SYS_CLOCK_HZ/1000000);

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
