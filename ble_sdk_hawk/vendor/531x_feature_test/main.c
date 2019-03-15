#include "app.h"
#include <tl_common.h>
#include "drivers.h"
#include "app_config.h"
#include "stack/ble/ble.h"
#include "vendor/common/user_config.h"

#if (__PROJECT_5317_FEATURE_TEST__)

extern my_fifo_t hci_rx_fifo;

_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler();

	#if (FEATURE_TEST_MODE)
		if(uart_get_irq_flag(UART_Flag_RxDone))
		{
			uart_clear_irq_flag(UART_Flag_RxDone);
			u8* w = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
			if(w[0]!=0)
			{
				my_fifo_next(&hci_rx_fifo);
				u8* p = hci_rx_fifo.p + (hci_rx_fifo.wptr & (hci_rx_fifo.num-1)) * hci_rx_fifo.size;
				reg_dma0_addr = (u16)((u32)p);
			}
		}
	#endif
}

int main(void){

	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init();

	clock_init();

	gpio_init();

	rf_drv_init(RF_MODE_BLE_1M_NO_PN);

	user_init();

	irq_enable();

	while(1)
	{
	#if(MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
	#endif
		main_loop ();
	}
}

#endif


