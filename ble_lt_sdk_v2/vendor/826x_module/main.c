#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/blt_config.h"

extern my_fifo_t hci_rx_fifo;
extern void user_init();

_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler ();

#if 0
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

#endif

}


#define		APP_ADV_LEN		18
u8	app_pkt_adv[32] = {
		APP_ADV_LEN - 4, 0, 0, 0,	// dma_len
		0,							// type
		APP_ADV_LEN - 6,			// rf_len
		0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5,	// advA
		0x05, 0x09, '5', '5', '6', '2',
		// data
};

u8	app_pkt_wx[256];


void  app_send_adv(void)
{
	rf_set_ble_access_code_adv ();
	rf_set_ble_crc_adv ();
	rf_set_ble_channel (37);
	REG_ADDR16(0xc0c) = (u16)((u32)app_pkt_adv);
	REG_ADDR16(0xc08) = (u16)((u32)app_pkt_wx);
	REG_ADDR16(0xc0a) = 0x110;

	while (1)
	{
		reg_rf_irq_status = FLD_RF_IRQ_TX | FLD_RF_IRQ_RX;
		//rf_start_stx2rx ((void *)app_pkt_adv, clock_time() + 100);
		REG_ADDR8(0xf00) = 0x87;
		sleep_us (1000);
		REG_ADDR8(0xf00) = 0x80;
		if (REG_ADDR8(0xf20) & FLD_RF_IRQ_RX)
		{
			app_pkt_adv[6]++;
		}
		sleep_us (99000);
	}
}

int main (void) {

	cpu_wakeup_init();

	//clock_init();

	rf_drv_init(CRYSTAL_TYPE);

	REG_ADDR8(0x66) = 0x43;			//change to PLL clock: 48/3 = 16M
	//analog_write (0x05, 0x06);		//turn off 32M RC clock to turn off DCDC
	analog_write (0xb7, 0xb0);		//turn on RF clock

#if 0
	app_send_adv ();

	while (1);
	return 1;
#endif

	gpio_init();


	user_init ();


    irq_enable();

	//while (1);
	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}
