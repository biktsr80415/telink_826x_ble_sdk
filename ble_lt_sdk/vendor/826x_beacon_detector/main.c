
#include "../../proj/tl_common.h"

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"

extern void user_init();
extern void main_loop (void);

#if 1
_attribute_ram_code_ void irq_handler(void)
{
#if 0
	u32 src = reg_irq_src;
    if(src & FLD_IRQ_TMR1_EN){
		extern void irq_timer1(void);
		irq_timer1();
		reg_tmr_sta = FLD_TMR_STA_TMR1;
	}
	if(src & FLD_IRQ_TMR2_EN){
		extern void irq_timer2(void);
		irq_timer2();
		reg_tmr_sta = FLD_TMR_STA_TMR2;
	}
#endif
	irq_blt_sdk_handler() ;

    #if UART_ENABLE
    static unsigned char enterRXIrq,enterTXIrq;
    // todo : check uart_IRQSourceGet() bit mask
    unsigned char irqS = uart_IRQSourceGet();
    if(irqS & UARTRXIRQ_MASK){
        uart_rx_true = 1;
        enterRXIrq++;
        
        //printf("rx ");
        // loopback test , no delay
        u32 rx_len = T_rxdata_buf.len + 4 > sizeof(T_rxdata_user) ? sizeof(T_rxdata_user) : T_rxdata_buf.len + 4;
        memcpy(&T_rxdata_user, &T_rxdata_buf, rx_len);
        uart_Send((u8 *)(&T_rxdata_user));
        
    }
    
    if(irqS & UARTTXIRQ_MASK){
        uart_clr_tx_busy_flag();
        enterTXIrq++;
        //printf("tx ");
    }
    #endif

}
#endif


int main (void) {
	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init(CRYSTAL_TYPE);

	set_tick_per_us (CLOCK_SYS_CLOCK_HZ/1000000);
	clock_init();

	gpio_init();

	//rf_drv_set_agc_maxgain_mode(AGC_MODE_ENABLE);
	rf_drv_init(CRYSTAL_TYPE);

	user_init ();

    irq_enable();

	while (1) {
		main_loop ();
	}
}

