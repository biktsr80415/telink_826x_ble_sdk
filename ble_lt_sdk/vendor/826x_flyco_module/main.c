#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/blt_config.h"

extern my_fifo_t hci_rx_fifo;
extern void user_init();

_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler ();  //ble irq proc

#if (HCI_ACCESS==HCI_USE_UART)
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
    }

    if(irqS & FLD_DMA_UART_TX)	//tx
    {
    	reg_dma_rx_rdy0 = FLD_DMA_UART_TX;
#if __PROJECT_8266_MODULE__
		uart_clr_tx_busy_flag();
#endif
    }
#endif
}

//please take 826x BLE SDK Developer Handbook for reference(page 24).
//         8266 512K flash address setting:
//
//          0x80000 |~~~~~~~~~~~~~~~~~~|
//                  |  user data area  |
//          0x78000 |~~~~~~~~~~~~~~~~~~|
//                  |  customed value  |
//          0x77000 |~~~~~~~~~~~~~~~~~~|
//                  |    MAC address   |
//          0x76000 |~~~~~~~~~~~~~~~~~~|
//                  |    pair & sec    |
//                  |       info       |
//          0x74000 |~~~~~~~~~~~~~~~~~~|
//                  |   ota_boot_flg   |
//          0x73000 |~~~~~~~~~~~~~~~~~~|
//                  |   ota_boot.bin   |
//          0x72000 |~~~~~~~~~~~~~~~~~~|
//                  |  user data area  |
//                  |                  |
//                  |                  |
//          0x40000 |~~~~~~~~~~~~~~~~~~|
//                  |   OTA new bin    |
//                  |   storage area   |
//                  |                  |
//          0x20000 |~~~~~~~~~~~~~~~~~~|
//                  | old firmwave bin |
//                  |                  |
//                  |                  |
//          0x00000 |~~~~~~~~~~~~~~~~~~|

////OTA BOOT BIN: .\tcdb.exe wf 72000 -eb -i "E:\Telink_BLE\826x module sdk\826x module sdk git tmp\ble_lt_app\ble_lt_sdk\8266_ota_boot\8266_ota_boot.bin"
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
