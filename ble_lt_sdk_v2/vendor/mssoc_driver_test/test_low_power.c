/*
 * test_low_power.c
 *
 *  Created on: 2018-4-17
 *      Author: Administrator
 */

#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj/drivers/keyboard.h"
#include "../common/tl_audio.h"
#include "../common/blt_led.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/audio.h"
#include "../../proj/drivers/adc.h"
#include "../../proj/drivers/battery.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj/drivers/uart.h"




#define TEST_LP_SUSPEND_NO_WAKEUP						1
#define TEST_LP_SUSPEND_GPIO_CORE_WAKEUP				2
#define TEST_LP_SUSPEND_TIMER_WAKEUP					3

#define TEST_LP_SUSPEND_TIMER_GPIO_CORE_WAKEUP			4

#define TEST_LP_DEEPSLEEP_NO_WAKEUP						5
#define TEST_LP_DEEPSLEEP_GPIO_PAD_WAKEUP				6
#define TEST_LP_DEEPSLEEP_TIMER_WAKEUP					7



#define TEST_LP_MODE									TEST_LP_SUSPEND_NO_WAKEUP



int first_power_on = 0;

u8 AA_wakeupSrc[256];
u8 AA_wkp_index;

void test_low_power(void)
{
	if(analog_read(0x3a) == 0x5a){  //read flag
		first_power_on = 0;
	}
	else{
		first_power_on = 1;
	}


	if(first_power_on){
		DBG_CHN0_TOGGLE;
		sleep_us(500000);
		DBG_CHN0_TOGGLE;
		sleep_us(500000);
		DBG_CHN0_TOGGLE;
		sleep_us(500000);
		DBG_CHN0_TOGGLE;
		sleep_us(500000);
	}
	else{
		DBG_CHN0_TOGGLE;
		sleep_us(500000);
		DBG_CHN0_TOGGLE;
		sleep_us(500000);
		DBG_CHN0_TOGGLE;
		sleep_us(500000);
		DBG_CHN0_TOGGLE;
		sleep_us(500000);
	}

	analog_write(0x3a, 0x5a); //set flag



#if(TEST_LP_MODE == TEST_LP_SUSPEND_NO_WAKEUP)

	shutdown_gpio();  //all GPIO high_Z state, no current leakage

	cpu_sleep_wakeup(SUSPEND_MODE , 0, 0);

#elif(TEST_LP_MODE == TEST_LP_SUSPEND_GPIO_CORE_WAKEUP)


	///////////////  gpio core wakeup  /////////////////////
	gpio_set_input_en(GPIO_PA0, 1);
	gpio_setup_up_down_resistor(GPIO_PA0, PM_PIN_PULLDOWN_100K);
	gpio_set_wakeup(GPIO_PA0, 1, 1);
	gpio_core_wakeup_enable_all(1);


	while(1){

		u32 wakeup_src = cpu_sleep_wakeup(SUSPEND_MODE, PM_WAKEUP_CORE, 0);  //PM_WAKEUP_CORE

		AA_wakeupSrc[AA_wkp_index++] = wakeup_src;

		DBG_CHN1_TOGGLE;
		sleep_us(300000);
		DBG_CHN1_TOGGLE;
		sleep_us(100000);
		DBG_CHN1_TOGGLE;
		sleep_us(600000);
		DBG_CHN1_TOGGLE;
		sleep_us(100000);
	}

#elif(TEST_LP_MODE == TEST_LP_SUSPEND_TIMER_WAKEUP)


	u32 now_tick;
	while(1){

		now_tick = clock_time();

		u32 wakeup_src = cpu_sleep_wakeup(SUSPEND_MODE, PM_WAKEUP_TIMER,  now_tick + 200 * CLOCK_16M_SYS_TIMER_CLK_1MS);

		AA_wakeupSrc[AA_wkp_index++] = wakeup_src;

		DBG_CHN1_TOGGLE;
		sleep_us(20000);
		DBG_CHN1_TOGGLE;
		sleep_us(10000);
		DBG_CHN1_TOGGLE;
		sleep_us(60000);
		DBG_CHN1_TOGGLE;
		sleep_us(10000);
	}

#elif(TEST_LP_MODE == TEST_LP_SUSPEND_TIMER_GPIO_CORE_WAKEUP)

	///////////////  gpio core wakeup  /////////////////////
	gpio_set_input_en(GPIO_PA0, 1);
	gpio_setup_up_down_resistor(GPIO_PA0, PM_PIN_PULLDOWN_100K);
	gpio_set_wakeup(GPIO_PA0, 1, 1);
	gpio_core_wakeup_enable_all(1);


	u32 now_tick;
	while(1){

		now_tick = clock_time();

		u32 wakeup_src = cpu_sleep_wakeup(SUSPEND_MODE, PM_WAKEUP_TIMER | PM_WAKEUP_CORE,  now_tick + 2000 * CLOCK_16M_SYS_TIMER_CLK_1MS);

		AA_wakeupSrc[AA_wkp_index++] = wakeup_src;

		DBG_CHN1_TOGGLE;
		sleep_us(300000);
		DBG_CHN1_TOGGLE;
		sleep_us(100000);
		DBG_CHN1_TOGGLE;
		sleep_us(600000);
		DBG_CHN1_TOGGLE;
		sleep_us(100000);
	}

#elif(TEST_LP_MODE == TEST_LP_DEEPSLEEP_NO_WAKEUP)

	shutdown_gpio();  //all GPIO high_Z state, no current leakage

	cpu_sleep_wakeup(DEEPSLEEP_MODE , 0, 0);

#elif(TEST_LP_MODE == TEST_LP_DEEPSLEEP_GPIO_PAD_WAKEUP)


	gpio_setup_up_down_resistor(GPIO_PA0, PM_PIN_PULLDOWN_100K);
	cpu_set_gpio_wakeup(GPIO_PA0, 1, 1);

	while(1){
		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);  //PM_WAKEUP_PAD
	}


#elif(TEST_LP_MODE == TEST_LP_DEEPSLEEP_TIMER_WAKEUP)

	u32 sleep_time = 0;
	if(first_power_on){
		sleep_time = 2*CLOCK_16M_SYS_TIMER_CLK_1S;
	}
	else{
		sleep_time = 1*CLOCK_16M_SYS_TIMER_CLK_1S;
	}


	u32 now_tick;
	while(1){

		now_tick = clock_time();
		cpu_sleep_wakeup(DEEPSLEEP_MODE , PM_WAKEUP_TIMER, now_tick + sleep_time);


		DBG_CHN1_TOGGLE;
		sleep_us(700000);
		DBG_CHN1_TOGGLE;
		sleep_us(300000);
		DBG_CHN1_TOGGLE;
		sleep_us(700000);
		DBG_CHN1_TOGGLE;
		sleep_us(300000);
	}

#else


#endif


}
