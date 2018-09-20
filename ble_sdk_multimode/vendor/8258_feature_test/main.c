#include "app.h"
#include <tl_common.h>
#include "drivers.h"
#include "app_config.h"
#include "stack/ble/ble.h"
#include "vendor/common/user_config.h"


extern my_fifo_t hci_rx_fifo;

extern void user_init_normal();
extern void user_init_deepRetn();

extern void main_loop (void);


#if (BLE_PHYTEST_MODE == PHYTEST_MODE_THROUGH_2_WIRE_UART )
	unsigned char uart_no_dma_rec_data[6] = {0x02,0, 0,0,0,0};
#elif(BLE_PHYTEST_MODE == PHYTEST_MODE_OVER_HCI_WITH_UART)
	unsigned char uart_no_dma_rec_data[72] = {0};
#endif

_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler();

	#if(FEATURE_TEST_MODE == TEST_BLE_PHY)
		unsigned char uart_dma_irqsrc;
		//1. UART irq
		uart_dma_irqsrc = dma_chn_irq_status_get();///in function,interrupt flag have already been cleared,so need not to clear DMA interrupt flag here
		if(uart_dma_irqsrc & FLD_DMA_CHN_UART_RX)
		{
			dma_chn_irq_status_clr(FLD_DMA_CHN_UART_RX);
			u8* w = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
			if(w[0]!=0)
			{
				my_fifo_next(&hci_rx_fifo);
				u8* p = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
				reg_dma0_addr = (u16)((u32)p);
			}
		}
		if(uart_dma_irqsrc & FLD_DMA_CHN_UART_TX){
			dma_chn_irq_status_clr(FLD_DMA_CHN_UART_TX);
		}
	#endif
}

_attribute_ram_code_ int main(void)
{

	cpu_wakeup_init();

	int deepRetWakeUp = pm_is_MCU_deepRetentionWakeup();  //MCU deep retention wakeUp

	rf_drv_init(RF_MODE_BLE_1M);

	gpio_init(!deepRetWakeUp);

	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
		clock_init(SYS_CLK_16M_Crystal);
	#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
		clock_init(SYS_CLK_24M_Crystal);
	#endif

	if( deepRetWakeUp ){
		user_init_deepRetn ();
	}
	else{
		user_init_normal ();
	}

	irq_enable();

	while(1)
	{
	#if(MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
	#endif
		main_loop ();
	}
}


