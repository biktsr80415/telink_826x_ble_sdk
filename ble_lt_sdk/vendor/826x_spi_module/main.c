#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"

//extern my_fifo_t hci_rx_fifo;
extern u8 *spi_rx_buff;
extern u8 *spi_tx_buff;
extern void user_init();

volatile u8 dbg_irq;
_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_slave_handler ();
#if (HCI_ACCESS==HCI_USE_UART)
	unsigned char irqS = reg_dma_rx_rdy0;
    if(irqS & BIT(0))	//rx
    {
    	reg_dma_rx_rdy0 = FLD_DMA_UART_RX;
		u8* w = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
		if(w[0]!=0)
		{
			my_fifo_next(&hci_rx_fifo);
			u8* p = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
			reg_dma0_addr = (u16)((u32)p);
		}
    }

    if(irqS & BIT(1))	//tx
    {
    	reg_dma_rx_rdy0 = FLD_DMA_UART_TX;
    }
#elif (HCI_ACCESS==HCI_USE_SPI)
    unsigned char spi_irq = reg_i2c_irq_status;
    if(spi_irq & BIT(1) )
	{
    	reg_i2c_clr_status |= BIT(1);  //clear irq src

		if(*(u16*)(spi_rx_buff) != 0)
		{//MSPI write irq
			dbg_irq++;
			spi_write_handler();
		}
		else
		{//MSPI read irq
			if(*(u32*)(spi_tx_buff))
			{
				spi_read_handler (); // ACK command, go to next status
			}
		}
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
