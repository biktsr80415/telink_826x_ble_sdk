
#include "../../proj/tl_common.h"

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"

#include "../link_layer/rf_ll.h"

#if (__PROJECT_8261_MULTI_MOUSE__ )

int uart_rx_irq_en = 0;

extern int SysMode;
extern my_fifo_t hci_rx_fifo;

extern void user_init();
extern void deep_wakeup_proc(void);

_attribute_ram_code_ void irq_handler(void)
{
	static u16 src_rf;
#if(UART_INIT_EN)

	unsigned char irqS = reg_dma_rx_rdy0;
    if(irqS & FLD_DMA_UART_RX)	//rx
    {
    	reg_dma_rx_rdy0 = FLD_DMA_UART_RX;
    	u8* w = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
    	if(w[0]!=0)
    	{
    		my_fifo_next(&hci_rx_fifo);
    		u8* p = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
    		reg_dma0_addr = (u16)((u32)p);

    	}

    	rx_from_uart_cb();

    }
#endif
    if(SysMode == RF_2M_2P4G_MODE){

        src_rf = reg_rf_irq_status;
		if(src_rf & FLD_RF_IRQ_RX){

			irq_device_rx();
		}

		if(src_rf & FLD_RF_IRQ_TX){
			irq_device_tx();
		}

    }
    else{
    	irq_blt_sdk_handler ();
    }





}

int main (void) {

	cpu_wakeup_init();

	clock_init();
	set_tick_per_us(CLOCK_SYS_CLOCK_HZ/1000000);

	gpio_init();

	deep_wakeup_proc();

	SysMode = (analog_read(DEEP_ANA_REG1) & 0xf0) >> 4;

	rf_drv_init( SysMode == RF_2M_2P4G_MODE ? XTAL_12M_RF_2m_MODE : XTAL_12M_RF_1m_MODE);

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
