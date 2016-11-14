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
#if (HCI_ACCESS==HCI_USE_UART)
	unsigned char irqS = reg_dma_rx_rdy0;
    if(irqS & BIT(0))	//rx
    {
    	reg_dma_rx_rdy0 = FLD_DMA_UART_RX;
		rx_uart_w_index = (rx_uart_w_index + 1)&0x01;
		reg_dma0_addr = (unsigned short)((unsigned int)(&T_rxdata_buf[rx_uart_w_index]));//set receive buffer address
    }

    if(irqS & BIT(1))	//tx
    {
    	reg_dma_rx_rdy0 = FLD_DMA_UART_TX;
#if __PROJECT_8266_MODULE__
		uart_clr_tx_busy_flag();
#endif
    }
#endif
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
