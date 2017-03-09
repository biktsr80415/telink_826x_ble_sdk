#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"
#if (PRINT_DEBUG_INFO)
#include "../common/myprintf.h"
#endif
//extern my_fifo_t hci_rx_fifo;
extern u8 *spi_rx_buff;
extern u8 *spi_tx_buff;
extern void user_init();

volatile u8 dbg_irq;
_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_slave_handler ();

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
