/********************************************************************************************************
 * @file     main.c
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
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

//#include "../../proj/tl_common.h"
//#include "../../vendor/common/user_config.h"
#include "tl_common.h"

//#include "../../proj/mcu/watchdog_i.h"
#include "drivers.h"


//#include "../../proj_lib/rf_drv.h"
//#include "../../proj_lib/pm.h"
//#include "../../proj_lib/ble/ll/ll.h"
#include "stack/ble/ble.h"


extern void user_init();
extern void main_loop (void);

_attribute_ram_code_ void irq_handler(void)
{
	irq_blt_sdk_handler() ;
}

int loop_cnt = 0;
int main (void) {

	cpu_wakeup_init();

	#if (CLOCK_SYS_CLOCK_HZ == 16000000)
		clock_init(SYS_CLK_16M_Crystal);
	#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
		clock_init(SYS_CLK_24M_Crystal);
	#endif

	rf_drv_init(RF_MODE_BLE_1M);

	gpio_init();

	//deep_wakeup_proc();

	if( pm_is_MCU_deepRetentionWakeup() ){
		//user_init_deepRetn ();
		user_init();
	}
	else{
		//user_init_normal ();
		user_init();
	}

    irq_enable();

	while (1) {
		loop_cnt++;
		main_loop ();

		//TODO : enable printf
		//printf(".");
		//reg_usb_ep8_dat = 0x80;
		//reg_usb_ep8_ctrl = 0x80;

	}
}

