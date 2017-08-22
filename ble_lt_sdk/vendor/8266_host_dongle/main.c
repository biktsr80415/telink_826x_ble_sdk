
#include "../../proj/tl_common.h"

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"

extern void user_init();
extern void deep_wakeup_proc(void);
extern uart_data_t T_rxdata_buf[2];
extern unsigned char rx_uart_r_index;
_attribute_ram_code_ void irq_handler(void)
{
	static	u32	dbg_tx, dbg_rx;
    unsigned char irqS = uart_IRQSourceGet();
    if(irqS & BIT(0))	//rx
    {
    	dbg_rx++;
		rx_uart_r_index = (rx_uart_r_index + 1)&0x01;
		write_reg16(0x800500,(unsigned short)((unsigned int)(&T_rxdata_buf[rx_uart_r_index])));//set receive buffer address
    }

    if(irqS & BIT(1))	//tx
    {
    	dbg_tx ++;
        uart_clr_tx_busy_flag();
    }
}

int main (void) {
	cpu_wakeup_init(CRYSTAL_TYPE);

	set_tick_per_us (32);
	clock_init();

	gpio_init();

	rf_drv_init(CRYSTAL_TYPE);

	//usb_init ();

	user_init ();

    irq_enable();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}
