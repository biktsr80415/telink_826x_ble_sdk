
#include "../../proj/tl_common.h"

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"

#if (__PROJECT_8267_MCU_RC__)

extern void user_init();
extern void deep_wakeup_proc(void);


extern unsigned char my_rx_uart_r_index;
extern unsigned char my_rx_uart_w_index;
extern uart_data_t	my_rxdata_buf[];
_attribute_ram_code_ void irq_handler(void)
{

	static	u32	dbg_tx, dbg_rx;

    unsigned char irqS = reg_dma_rx_rdy0;
    if(irqS & FLD_DMA_UART_RX)	//rx
    {
    	reg_dma_rx_rdy0 = FLD_DMA_UART_RX;
    	dbg_rx++;
		my_rx_uart_w_index = (my_rx_uart_w_index + 1) & 0x01;
		reg_dma0_addr =(u16)(u32) (&my_rxdata_buf[my_rx_uart_w_index]);  //change rx dma addr
    }

    if(irqS & FLD_DMA_UART_TX)	//tx
    {
    	reg_dma_rx_rdy0 = FLD_DMA_UART_TX;

    	dbg_tx ++;
        uart_clr_tx_busy_flag();
    }
}

int main (void) {
	cpu_wakeup_init();

	clock_init();
	set_tick_per_us(CLOCK_SYS_CLOCK_HZ/1000000);

	gpio_init();

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
