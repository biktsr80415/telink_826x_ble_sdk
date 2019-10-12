/********************************************************************************************************
 * @file     main.c
 *
 * @brief    for TLSR chips
 *
 * @author	 BLE Group
 * @date     May. 12, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai)
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in.
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided.
 *
 *******************************************************************************************************/
#include "../../proj/tl_common.h"

#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/phy/phy_test.h"
#include "../../proj/drivers/uart.h"


#if (REMOTE_IR_ENABLE)
#include "rc_ir.h"
#endif


#if (__PROJECT_8261_BLE_REMOTE__ || __PROJECT_8267_BLE_REMOTE__ || __PROJECT_8269_BLE_REMOTE__)

extern void user_init();
extern void main_loop (void);
extern void deep_wakeup_proc(void);





_attribute_ram_code_ void irq_handler(void)
{


#if (REMOTE_IR_ENABLE)
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
	if(src & FLD_IRQ_TMR1_EN){
		ir_repeat_handle();
		reg_tmr_sta = FLD_TMR_STA_TMR1;
	}
#endif




	irq_blt_sdk_handler ();

#if (BLE_PHYTEST_MODE != PHYTEST_MODE_DISABLE)
	extern void irq_phyTest_handler(void);
	irq_phyTest_handler();
#endif


}

int main (void) {

	blc_pm_select_internal_32k_crystal(); //blc_pm_select_external_32k_crystal

	cpu_wakeup_init(CRYSTAL_TYPE);

	set_tick_per_us (CLOCK_SYS_CLOCK_HZ/1000000);
	clock_init();

	gpio_init();

	deep_wakeup_proc();

	///NOTE:This function must be placed before the following function rf_drv_init().
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

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



#endif
