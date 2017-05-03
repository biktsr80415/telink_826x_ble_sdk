#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"

//extern my_fifo_t hci_rx_fifo;
extern u8 *spi_rx_buff;
extern u8 *spi_tx_buff;
extern void user_init();

volatile u8 dbg_irq;
_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler ();
    unsigned char spi_irq = reg_i2c_irq_status;
    if(spi_irq & BIT(1) )
	{
    	reg_i2c_clr_status |= BIT(1);  //clear irq src

		if(*(u32*)(spi_rx_buff) != 0)
		{//MSPI write irq

			spi_write_handler();
		}
		else
		{//MSPI read irq
			if(*(u32*)(spi_tx_buff))
			{
				dbg_irq++;
				spi_read_handler (); // ACK command, go to next status
			}
		}
	}
}

int main (void) {
	cpu_wakeup_init();

	set_tick_per_us(CLOCK_SYS_CLOCK_HZ/1000000);
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
