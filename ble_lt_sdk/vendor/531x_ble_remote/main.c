
#include "../../proj/tl_common.h"

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/ble_phy.h"
#include "../../proj/drivers/uart.h"


#if (REMOTE_IR_ENABLE)
#include "rc_ir.h"
#endif


#if (__PROJECT_5316_BLE_REMOTE__)

extern void user_init();
extern void main_loop (void);
extern void deep_wakeup_proc(void);

_attribute_ram_code_ void task_dbg(void)
{
#if 0
	static u8 bVal = 0;

	gpio_write(GPIO_PA1, bVal);

	bVal++;

	bVal%=2;
#endif
	return;
}

_attribute_ram_code_ void irq_handler(void)
{
#if (REMOTE_IR_ENABLE)
	#if (MCU_CORE_TYPE == MCU_CORE_5316)
		IR_IRQHandler();
	#else
			u8 pwm_sta = reg_pwm_irq_sta;
		#if (IR_PWM_SELECT == PWM0_IR_MODE)
			if(pwm_sta & FLD_IRQ_PWM0_PNUM){
				ir_irq_send();
				reg_pwm_irq_sta = FLD_IRQ_PWM0_PNUM;
			}
		#elif (IR_PWM_SELECT == PWM1_IR_MODE)
			if(pwm_sta & FLD_IRQ_PWM1_PNUM){
				ir_irq_send();
				reg_pwm_irq_sta = FLD_IRQ_PWM1_PNUM;
			}
		#endif


		u32 src = reg_irq_src;
		if(src & FLD_IRQ_TMR2_EN){
			ir_repeat_handle();
			reg_tmr_sta = FLD_TMR_STA_TMR2;
		}
	#endif/* end of MCU_CORE_TYPE == MCU_CORE_5316 */
#endif/* end of REMOTE_IR_ENABLE */

	u32 src = reg_irq_src;
	if(src & FLD_IRQ_TMR1_EN){
		task_dbg();
		reg_tmr_sta |= FLD_TMR_STA_TMR1;
	}

	irq_blt_sdk_handler ();

#if (BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE)
	extern void irq_phyTest_handler(void);
	irq_phyTest_handler();
#endif
}


int main (void) {
    blc_pm_select_internal_32k_crystal();

    cpu_wakeup_init(CRYSTAL_TYPE);

    set_tick_per_us (32000000/1000000);

	//clock_init();
    //write_reg8(0x66, 0x43);  	/* Set system clock to PLL 16MHz. */
    //write_reg8(0x70,0x00);//must

    /* Set system clock to 32MHz. */
	write_reg8(0x66,0x60);
	write_reg8(0x70, read_reg8(0x70)&0xfe);

    gpio_init();

    deep_wakeup_proc();

    rf_drv_init(CRYSTAL_TYPE);

    user_init ();

    irq_enable();

	cpu_enable_timer1_interrupt(100 * CLOCK_SYS_CLOCK_1MS);
	register_timer_irq(&task_dbg);

    while (1) {
#if (MODULE_WATCHDOG_ENABLE)
      wd_clear(); //clear watch dog
#endif
      main_loop ();
    }
}



#endif
