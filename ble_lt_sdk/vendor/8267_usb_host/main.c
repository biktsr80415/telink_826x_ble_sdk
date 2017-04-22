
#include "../../proj/tl_common.h"

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"

extern void user_init();
extern void deep_wakeup_proc(void);

_attribute_ram_code_ void irq_handler(void)
{
	irq_ble_master_handler ();

#if (HCI_ACCESS==HCI_USE_UART)
    unsigned char irqS = uart_IRQSourceGet();
    if(irqS & BIT(0))	//rx
    {
    	extern unsigned char uart_rx_true;
        uart_rx_true = 1;
    }

    if(irqS & BIT(1))	//tx
    {
        uart_clr_tx_busy_flag();
    }
#endif
}

//host:        Sync-->IN-->ADDR-->ENDP-->CRC5-->EOP-->IDLE
//D12:         Sync-->NACK-->EOP-->IDLE
//host:        Sync-->IN-->ADDR-->ENDP-->CRC5-->EOP-->IDLE
//D12:         Sync-->NACK-->EOP-->IDLE
//             ......
//D12:         Sync-->DATA0/1-->DATA-->CRC16-->EOP-->IDLE
//             ......
//host:        Sync-->ACK-->EOP-->IDLE
int main (void) {
	//cpu_wakeup_init();
	REG_ADDR32(0x60) = 0xff000000;
	REG_ADDR8(0x64) = 0xff;
	REG_ADDR8(0x73) = 0x04;
	REG_ADDR8(0x74f) = 0x01;
	analog_write (0x05, 0x62);
	analog_write (0x88, 0x0f);			//enable 192M clock to core
	REG_ADDR8(0x66) = 0x24;				//48M clock
	set_tick_per_us (48);

	//gpio_init();
	usb_dp_pullup_en (1);

	user_init ();

	printf("Hello!!!\n");
	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}
