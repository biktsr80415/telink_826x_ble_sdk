/*
 * Author : zjs
 * Data : 2017-01-23
 *
 */

#ifndef _MOUSE_PM_
#define _MOUSE_PM_		1

#include "../../proj/tl_common.h"
#include "../../proj_lib/pm.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../common/rf_frame.h"
#include "../link_layer/rf_ll.h"
#include "../common/blt_led.h"


#include "mouse.h"
#include "mouse_pm.h"
#include "mouse_rf.h"
#include "mouse_sensor.h"
#include "mouse_custom.h"

mouse_sleep_t mouse_sleep ={
	SLEEP_MODE_BASIC_SUSPEND,
	0,
	0,
	1,								//mcu sleep en

	0,
	1,
	0,								//rsv

	0,								//basic suspend cnt
	THRESH_BASIC_SUSPEND,			//basic suspend thresh

	0,								//long suspend cnt
	THRESH_NO_MOTION_LONG_SUSPEND,	//long suspend thresh

	0,								//wakep tick
	0								//next wakeup tick

};




void mouse_sleep_wakeup_init( mouse_hw_t *pHW, u8 sensor ){
#ifdef reg_gpio_wakeup_en
	reg_gpio_wakeup_en |= FLD_GPIO_WAKEUP_EN;	//gpio wakeup initial
#endif
	reg_wakeup_en = FLD_WAKEUP_SRC_GPIO;        //core wakeup gpio enbable

	for (int i = MAX_MOUSE_BUTTON - 1; i>=0; i-- ){  //gpio¸´ÓÃÊ± ×óÖÐÓÒ»½ÐÑ
			u32 suspend_wakeuppin = pHW->button[i];
			u32 suspend_wakeupin_level = (pHW->gpio_level_button[i] == U8_MAX);
			gpio_set_wakeup(suspend_wakeuppin,0,1);  	   //drive pin core(gpio) high wakeup suspend
			cpu_set_gpio_wakeup(suspend_wakeuppin, !suspend_wakeupin_level, 1);  //PAD wakeup
		}

#if MOUSE_DEEPSLEEP_EN
    if ( SENSOR_IS_SIGMA_8640(mouse_status.mouse_sensor) )
        mouse_sleep.mcu_sleep_en = 0;
    else
        mouse_sleep.mcu_sleep_en = CUST_MCU_SLEEP_EN;

    mouse_sleep.sensor_sleep = CUST_SENSOR_SLEEP_EN;
    //u32 tick = ((p_custom_cfg->slp_tick << 1) + p_custom_cfg->slp_tick) >> 6; //tick = 200ms * 256 * 256 * 3 / 64 = 614s

    //mouse_sleep.long_suspend_thresh = sensor_motion_detct ? tick : (tick << 1);

#else
	device_sleep.thresh_100ms = U16_MAX;
#endif
}



void mouse_sleep_mode_machine (mouse_sleep_t *cfg_slp)
{
	if(cfg_slp->device_busy){

		cfg_slp->mode = SLEEP_MODE_BASIC_SUSPEND;
		cfg_slp->basic_suspend_cnt = 0;
		cfg_slp->long_suspend_cnt = 0;
	}
	else if(cfg_slp->mode == SLEEP_MODE_BASIC_SUSPEND){
		cfg_slp->basic_suspend_cnt++;
		if(cfg_slp->basic_suspend_cnt > cfg_slp->basic_suspend_thresh){
			cfg_slp->basic_suspend_cnt = 0;
			cfg_slp->mode = SLEEP_MODE_LONG_SUSPEND;
		}
	}


	if(cfg_slp->mode == SLEEP_MODE_LONG_SUSPEND){
		cfg_slp->long_suspend_cnt++;
		if(cfg_slp->long_suspend_cnt > cfg_slp->long_suspend_thresh){
			cfg_slp->long_suspend_cnt = 0;
			cfg_slp->mode = SLEEP_MODE_DEEPSLEEP;
		}
	}

	if(cfg_slp->mode == SLEEP_MODE_DEEPSLEEP || cfg_slp->quick_sleep){
		if(cfg_slp->mcu_sleep_en){
			cfg_slp->mode = SLEEP_MODE_DEEPSLEEP;
		}
		else{
			cfg_slp->mode = SLEEP_MODE_LONG_SUSPEND;
			cfg_slp->long_suspend_cnt = 0;
		}
		cfg_slp->sensor_sleep = 1;
	}



}



extern led_cfg_t led_cfg[];
#if(MOUSE_SLEEP_MODULE_EN )
extern int SysMode;
#define   DEBUG_NO_SUSPEND      0

#if 0
_attribute_ram_code_
#endif
void mouse_power_saving_process( mouse_status_t *mouse_status ){
    //log_event( TR_T_MS_MACHINE);
#if MOUSE_DEEPSLEEP_EN
    mouse_sleep.quick_sleep = M_HOST_NO_LINK && QUICK_SLEEP_EN;

    if ( mouse_status->mouse_mode <= STATE_PAIRING ){
        if ( mouse_status->loop_cnt < 4096 )       //sync dongle time 4096 *8 ms most
            mouse_sleep.quick_sleep = 0;
    }
#endif
    mouse_sleep.device_busy = M_DEVICE_PKT_ASK;

    mouse_sleep_mode_machine( &mouse_sleep );


#if (MOUSE_WKUP_SENSOR_SIM )
    u32 wkup_sesnor_sim = mouse_sensor_blinky_wkup( (mouse_sleep.device_busy && mouse_sleep.quick_sleep && !mouse_sleep.mcu_sleep_en) );
    if ( wkup_sesnor_sim ){
        mouse_status->no_ack = 0;
    }
    u32 wkup_sensor =  ((mouse_status->no_ack == 0) && M_DEVICE_PKT_ASK );
    u32 sensor_st = mouse_sensor_sleep_wakeup( &mouse_status->mouse_sensor, &mouse_sleep.sensor_sleep, wkup_sensor );

    if ( sensor_st & SENSOR_MODE_WORKING ){
       // mouse_sensor_set_cpi( &mouse_status->cpi );
        cpu_set_gpio_wakeup(mouse_status->hw_define->sensor_int, 0, 1);  //PAD wakeup enable when wakeup sensor
    }
    else if ( sensor_st & SENSOR_MODE_POWERDOWN ){
        cpu_set_gpio_wakeup(mouse_status->hw_define->sensor_int, 0, 0);  //PAD wakeup disable when shut-down sensor
    }
	if ( mouse_sleep.mode == SLEEP_MODE_DEEPSLEEP){
    	//mouse_info_save(mouse_status, 1);    	        //Save related information to 3.3V analog register
	}
#endif

	static u32 debug_clock;

	if ( mouse_sleep.mode == SLEEP_MODE_BASIC_SUSPEND ){
		debug_clock = clock_time();
		mouse_sleep.wakeup_next_tick = mouse_sleep.wakeup_tick + (BASIC_SUSPEND_TIME * CLOCK_SYS_CLOCK_1MS) - 1440;
		mouse_sleep.wakeup_src = PM_WAKEUP_TIMER;  //A1 wheel no need wkup 8ms suspend
	}
	//Suspend, deep sleep GPIO, 32K setting
	if ( mouse_sleep.mode == SLEEP_MODE_LONG_SUSPEND ){				//100ms

        mouse_sleep.wakeup_next_tick = mouse_sleep.wakeup_tick + LONG_SUSPEND_TIME * CLOCK_SYS_CLOCK_1MS - 160;
        mouse_sleep.wakeup_src = PM_WAKEUP_CORE | PM_WAKEUP_PAD | PM_WAKEUP_TIMER;
        device_sync = 0;        //ll_channel_alternate_mode ();
	}

#if MOUSE_DEEPSLEEP_EN
	else if ( mouse_sleep.mode ==  SLEEP_MODE_DEEPSLEEP){
		mouse_sleep.wakeup_next_tick = 0;
		mouse_sleep.wakeup_src = PM_WAKEUP_PAD;
        device_sync = 0;        //ll_channel_alternate_mode ();

        device_info_save(mouse_status, 0);
	}
#endif

#if MOUSE_NO_HW_SIM
    mouse_sleep.mode = SLEEP_MODE_BASIC_SUSPEND;
#endif
}


//low-end mouse higher current, active for longer time
#if (MOUSE_SW_CUS & 0)
static inline void mouse_power_consume_process (void){
    if ( M_SUSPEND_8MS & device_sleep.mode ){
        WaitUs(mouse_cust_low_end_delay_time);
    }
}
#endif

#endif


#endif
