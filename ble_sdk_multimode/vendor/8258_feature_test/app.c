#include "app.h"
#include <stack/ble/ble.h>
#include "tl_common.h"
#include "drivers.h"
#include "app_config.h"
#include "vendor/common/blt_led.h"
#include "application/keyboard/keyboard.h"
#include "vendor/common/tl_audio.h"
#include "vendor/common/blt_soft_timer.h"



void user_init_normal(void)
{
#if (   FEATURE_TEST_MODE == TEST_ADVERTISING_ONLY || FEATURE_TEST_MODE == TEST_SCANNING_ONLY \
	 || FEATURE_TEST_MODE == TEST_ADVERTISING_IN_CONN_SLAVE_ROLE \
	 || FEATURE_TEST_MODE==TEST_SCANNING_IN_ADV_AND_CONN_SLAVE_ROLE \
	 || FEATURE_TEST_MODE == TEST_ADVERTISING_SCANNING_IN_CONN_SLAVE_ROLE)

	feature_linklayer_state_test_init_normal();

#elif (FEATURE_TEST_MODE == TEST_POWER_ADV)

	feature_adv_power_test_init_normal();

#elif (FEATURE_TEST_MODE == TEST_SMP_PASSKEY_ENTRY)

	feature_security_test_init_normal();

#elif (FEATURE_TEST_MODE == TEST_USER_BLT_SOFT_TIMER)

	feature_soft_timer_test_init_normal();

#elif (FEATURE_TEST_MODE == TEST_WHITELIST)

	feature_whitelist_test_init_normal();

#elif (FEATURE_TEST_MODE == TEST_BLE_PHY)

	feature_phytest_init_normal();

#else

#endif
}




_attribute_ram_code_ void user_init_deepRetn(void)
{

#if (   FEATURE_TEST_MODE == TEST_ADVERTISING_ONLY || FEATURE_TEST_MODE == TEST_SCANNING_ONLY \
	 || FEATURE_TEST_MODE == TEST_ADVERTISING_IN_CONN_SLAVE_ROLE \
	 || FEATURE_TEST_MODE==TEST_SCANNING_IN_ADV_AND_CONN_SLAVE_ROLE \
	 || FEATURE_TEST_MODE == TEST_ADVERTISING_SCANNING_IN_CONN_SLAVE_ROLE)

	feature_linklayer_state_test_init_deepRetn();

#elif (FEATURE_TEST_MODE == TEST_POWER_ADV)

	feature_adv_power_test_init_deepRetn();

#elif (FEATURE_TEST_MODE == TEST_SMP_PASSKEY_ENTRY)

	feature_security_test_init_deepRetn();

#elif (FEATURE_TEST_MODE == TEST_USER_BLT_SOFT_TIMER)

	feature_soft_timer_test_init_deepRetn();

#elif (FEATURE_TEST_MODE == TEST_WHITELIST)

	feature_whitelist_test_init_deepRetn();

#elif (FEATURE_TEST_MODE == TEST_BLE_PHY)


#else

#endif
}





u32 tick_loop=0;
/*----------------------------------------------------------------------------*/
/*-------- Main Loop                                                ----------*/
/*----------------------------------------------------------------------------*/
_attribute_ram_code_ void main_loop (void)
{
	tick_loop++;

#if (FEATURE_TEST_MODE == TEST_USER_BLT_SOFT_TIMER)
	blt_soft_timer_process(MAINLOOP_ENTRY);
#endif

	blt_sdk_main_loop();
}

/*----------------------------- End of File ----------------------------------*/


