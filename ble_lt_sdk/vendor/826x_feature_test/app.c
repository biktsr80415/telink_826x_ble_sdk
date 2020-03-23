/********************************************************************************************************
 * @file     app.c
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
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/hci/hci.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj_lib/ble/phy/phy_test.h"
#include "../../proj/drivers/uart.h"
#include "../common/blt_soft_timer.h"


#if (__PROJECT_8261_FEATURE_TEST__ || __PROJECT_8266_FEATURE_TEST__ || __PROJECT_8267_FEATURE_TEST__ || __PROJECT_8269_FEATURE_TEST__)



//void	task_encryption_done (u8 e, u8 *p, int n)
//{
//	if(*p == SMP_STANDARD_PAIR){  //first paring
//
//	}
//	else if(*p == SMP_FAST_CONNECT){  //auto connect
//
//	}
//}





void user_init()
{

#if ( FEATURE_TEST_MODE == TEST_SMP_PASSKEY_ENTRY  )

	feature_passkey_entry_init();
#elif(FEATURE_TEST_MODE == TEST_USER_BLT_SOFT_TIMER)

	feature_soft_timer_init();

#elif(FEATURE_TEST_MODE == TEST_WHITELIST)

	feature_whitelist_test_init();
#elif (FEATURE_TEST_MODE == TEST_POWER_ADV)

	feature_adv_power_test_init();

#elif (FEATURE_TEST_MODE == TEST_BLE_PHY)

	feature_phy_test_init();

#elif(FEATURE_TEST_MODE == TEST_SDATA_LENGTH_EXTENSION)

	feature_sdle_test_init();

#elif (FEATURE_TEST_MODE == TEST_2M_PHY_CONNECTION)

	feature_2m_phy_conn_init();
#else  ///all link layer test.

	feature_linklayer_state_test_init();
#endif


#if (BLE_PM_ENABLE)
	blc_ll_initPowerManagement_module();
	bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);

	#if (FEATURE_TEST_MODE == TEST_ADVERTISING_SCANNING_IN_CONN_SLAVE_ROLE)
		bls_pm_setSuspendMask (SUSPEND_ADV);
	#else
		bls_pm_setSuspendMask (SUSPEND_ADV | SUSPEND_CONN);
	#endif

	//bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_ENTER, &func_suspend_enter);
	//bls_app_registerEventCallback (BLT_EV_FLAG_SUSPEND_EXIT, &func_suspend_exit);
#endif

}




/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
u32 tick_wakeup;
void main_loop (void)
{
	static u32 tick_loop;

	tick_loop ++;


#if (FEATURE_TEST_MODE == TEST_USER_BLT_SOFT_TIMER)
	blt_soft_timer_process(MAINLOOP_ENTRY);
#endif

	blt_sdk_main_loop();

#if (FEATURE_TEST_MODE == TEST_SDATA_LENGTH_EXTENSION)
	feature_sdle_test_mainloop();
#elif (FEATURE_TEST_MODE == TEST_2M_PHY_CONNECTION)
	feature_2m_phy_conn_mainloop();
#endif
}




#endif  // end of __PROJECT_826x_FEATURE_TEST__
