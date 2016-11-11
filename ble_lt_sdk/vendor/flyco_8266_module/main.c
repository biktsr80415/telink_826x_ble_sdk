#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj/drivers/uart.h"

extern void user_init();

_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_slave_handler ();
#if (HCI_ACCESS==HCI_USE_UART)
    unsigned char irqS = read_reg8(0x800526)& UARTIRQ_MASK;
    if(irqS & BIT(0))	//rx
    {
    	write_reg8(0x800526,FLD_DMA_UART_RX);//CLR irq source
		rx_uart_r_index = (rx_uart_r_index + 1)&0x01;
		write_reg16(0x800500,(unsigned short)((unsigned int)(&T_rxdata_buf[rx_uart_r_index])));//set receive buffer address
    }

    if(irqS & BIT(1))	//tx
    {
    	write_reg8(0x800526,FLD_DMA_UART_TX);//CLR irq source
        uart_clr_tx_busy_flag();
    }
#endif
}
//OTA BOOT¡¡£Â£É£Î¡¡.\tcdb.exe wf 1f000 -eb -i "E:\Telink_BLE\Telink_bts_spp_git\ble_lt_sdk\flyco_8266_module\8266_ota_boot.bin"
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
