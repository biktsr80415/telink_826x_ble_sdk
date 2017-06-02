#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../common/rf_frame.h"
#include "../common/blt_led.h"
#include "../link_layer/rf_ll.h"
#include "../common/emi.h"


#include "mouse_info.h"
#include "mouse.h"
#include "mouse_rf.h"
#include "mouse_button.h"
#include "mouse_wheel.h"
#include "mouse_sensor_pix.h"
#include "mouse_sensor.h"
#include "mouse_rf.h"
#include "mouse_custom.h"
#include "trace.h"
#include "mouse_pm.h"
#include "drv_mouse_pmw3610.h"

#include "../../proj/drivers/uart.h"

#ifndef MOUSE_GOLDEN_VPATTERN
#define MOUSE_GOLDEN_VPATTERN   0
#endif

#ifndef MOUSE_NO_HW_SIM
#define MOUSE_NO_HW_SIM         (MOUSE_GOLDEN_VPATTERN || MOUSE_EMI_4_FCC || 0)
#endif

#define	MOUSE_V_PATERN_MODE	    (MOUSE_NO_HW_SIM || 0)

mouse_status_t   mouse_status;

mouse_hw_t       mouse_hw;

mouse_data_t     mouse_data;

extern mouse_sleep_t mouse_sleep;
void mouse_pair_and_sync_process(mouse_status_t * mouse_status);

void platform_init( mouse_status_t *pStatus )
{
	mouse_hw_t *pHW = pStatus->hw_define;
	mouse_button_init(pHW);			/*********** Button Init ****************/
    mouse_wheel_init(pHW);			/*********** Wheel Init ****************/
#if(MOUSE_SENSOR_MODULE_EN)

#if(TELINK_MOUSE_DEMO)
	mouse_sensor_hw_init(pHW, &pStatus->mouse_sensor, (pStatus->mouse_mode == STATE_POWERON));
#else
	drv_mouse_sensor_init();
#endif



#endif

#if MOUSE_SW_CUS
//#define led_cnt_rate    (8 << (pStatus->high_end == MS_HIGHEND_250_REPORTRATE))
	//mouse_led_init(pHW->led_cntr, pHW->gpio_level_led, led_cnt_rate);
#endif

#if MOUSE_DEEPSLEEP_EN
	mouse_sleep_wakeup_init(pStatus->hw_define, pStatus->mouse_sensor);
#endif


}
u32 cpu_wakup_last_tick;
extern u8 SysMode;

int mouse_hw_init(void)
{
	mouse_status.hw_define = &mouse_hw;
	mouse_status.data = &mouse_data;
	mouse_status.no_ack = 1;    //no_ack == 0, wakeup sensor from deep-sleep every time

	//gpio_pullup_dpdm_internal( FLD_GPIO_DM_PULLUP | FLD_GPIO_DP_PULLUP );

	mouse_custom_init( &mouse_status );

	if(SysMode == RF_2M_2P4G_MODE)
	{
		device_info_load(&mouse_status);
	}

#if ( !MOUSE_NO_HW_SIM )
	platform_init(&mouse_status);

#endif



#if(MOUSE_SENSOR_MODULE_EN)

#if(TELINK_MOUSE_DEMO)
	mouse_sensor_init(&mouse_status.mouse_sensor, &mouse_status.cpi);
    if ( mouse_status.mouse_sensor & SENSOR_MODE_WORKING ){
        cpu_set_gpio_wakeup(mouse_status.hw_define->sensor_int, 0, 1);  //PAD wakeup enable when wakeup sensor
    	gpio_set_wakeup(mouse_status.hw_define->sensor_int,0,1);  	   //drive pin core(gpio) high wakeup suspend
    }
#else
#endif

#endif

	return 1;
}
extern led_cfg_t led_cfg[];
void  normal_user_init(void)
{
	//swire2usb_init();
	cpu_wakup_last_tick = clock_time();
    mouse_rf_init(&mouse_status);

#if(TELINK_MOUSE_DEMO)
    device_led_init(GPIO_LED, 1);
    //gpio_set_output_en(GPIO_LED, 1);
#else
    device_led_init(GPIO_LED_R, 0);
#endif

}


u32 mouse_linked_tick;
void mouse_pair_and_sync_process(mouse_status_t * mouse_status){
    static u16 pairing_time = 0;
    u32 pairing_end = 0;
#if MOUSE_GOLDEN_VPATTERN
    if ( HOST_NO_LINK ){
        mouse_status->mouse_mode = STATE_POWERON;
    }
#endif
    static u8 led_linked_host = 1;      //mouse, wakeup from deep-sleep, can blinky 3 times when linked with dongle

    if ( (mouse_rf_pkt != (u8*)&pkt_mouse) && mouse_status->link ){

    	mouse_linked_tick = clock_time();
        rf_set_tx_pipe (PIPE_MOUSE);

        mouse_rf_pkt = (u8*)&pkt_mouse;						//use data packet
        rf_set_power_level_index (mouse_tx_power);

        mouse_status->mouse_mode = STATE_NORMAL;
    }

#if DEVICE_LED_MODULE_EN
        //if linked with dongle, led blinky 3 times
        if( led_linked_host && (mouse_status->no_ack == 0) ){
            mouse_led_setup( mouse_led_cfg[E_LED_RSVD] );
            led_linked_host = 0;
        }
#endif

            //device_info_save (mouse_status, 0);

        if ( mouse_status->mouse_mode == STATE_PAIRING ){
        	pkt_mouse_pairing.flow = PKT_FLOW_PARING;
        }
        else if (mouse_status->mouse_mode == STATE_SYNCING ){
        	if(mouse_status->loop_cnt < PARING_POWER_ON_CNT ){
        		pkt_mouse_pairing.flow = PKT_FLOW_TOKEN | PKT_FLOW_PARING;
        	}
        	else{
        		pkt_mouse_pairing.flow = PKT_FLOW_TOKEN;
        	}
        }

#if 0
    if ( mouse_status->mouse_mode == STATE_POWERON ) {        
        //debug mouse get access code-1 from flash/otp
        if ( GET_HOST_ACCESS_CODE_FLASH_OTP != U16_MAX ){
		    rf_set_access_code1 ( rf_access_code_16to32( GET_HOST_ACCESS_CODE_FLASH_OTP ) );            
        }
        else{            
            rf_set_access_code1 ( U32_MAX );            //powe-on pkt link fail
        }



        mouse_sync_status_update( mouse_status );
	}

    if( mouse_status->mouse_mode == STATE_PAIRING ){
    	if( mouse_status->no_ack == 0 ){
    	    pairing_end = U8_MAX;        //pairing OK
    	}
        if( pairing_time >= 1024 ){
            pairing_end = 1;             //pairing fail, time out
        }
        else{            
            pairing_time += 1;
        }
        
        if( pairing_end ){
            pairing_time = 0;           //pairing-any-time could re-try many time
            mouse_led_setup( mouse_led_pairing_end_cfg_cust(pairing_end) );
            mouse_sync_status_update( mouse_status );
        }
        else{
            mouse_led_setup( mouse_led_cfg[E_LED_PAIRING] );
        }
    }
    else if ( mouse_status->mouse_mode != STATE_EMI ){
        mouse_sync_status_update( mouse_status );
    }
#endif

}
//u32 debug_last_wakeup_level;
extern unsigned short battValue[10];
extern void mouse_battery_check(unsigned short *batt, u8 len, unsigned short alarm_thresh);
_attribute_ram_code_ void mouse_task_when_rf ( void ){


	*((u32 *)(mouse_status.data)) = 0;	//clear mouse_event_data
#if(MOUSE_WHEEL_MODULE_EN)
    u32 wheel_prepare_tick = mouse_wheel_prepare_tick();
#endif

#if 0
    if ( 1 || MOUSE_V_PATERN_MODE || (mouse_status.dbg_mode & STATE_TEST_V_BIT) ){
        static u8 fno;
        mouse_status.data->x = fno & BIT(6) ? 6 : -6;
        mouse_status.data->y = fno & BIT(5) ? -6 : 6;
        fno++;
    }
#endif

#if MOUSE_NO_HW_SIM
    mouse_rf_post_proc(&mouse_status);
    mouse_sync_process(&mouse_status);
    mouse_power_saving_process(&mouse_status);
#else

//    mouse_rf_post_proc(&mouse_status);
//    mouse_pair_and_sync_process(&mouse_status);

#if(MOUSE_WHEEL_MODULE_EN)
    mouse_wheel_process(&mouse_status, wheel_prepare_tick);
#endif


#if(MOUSE_BTN_MODULE_EN)
    mouse_button_process(&mouse_status);
#endif
    
#if(MOUSE_SLEEP_MODULE_EN)
    mouse_power_saving_process(&mouse_status);
#endif

#endif
}
void mouse_task_in_ram( void ){
    p_task_when_rf = mouse_task_when_rf;

    mouse_rf_process(&mouse_status);

    if (p_task_when_rf != NULL) {

       (*p_task_when_rf) ();
    }

}



void mouse_main_loop(void)
{

	if( MOUSE_EMI_4_FCC || (mouse_status.mouse_mode == STATE_EMI) ){
		mouse_emi_process(&mouse_status);
	}
	else
	{
		mouse_sleep.wakeup_tick = clock_time();
        if(mouse_status.mouse_mode <= STATE_PAIRING){				//pair and sync
        	mouse_pair_and_sync_process(&mouse_status);
        }
        mouse_button_detect(&mouse_status, MOUSE_BTN_LOW);		//Debug, no button

#if(MOUSE_SENSOR_MODULE_EN)

#if(TELINK_MOUSE_DEMO)
        mouse_sensor_data( &mouse_status );
#else
        drv_mouse_motion_report(&mouse_status.data->x, 0);			//Debug, no sensor
#endif

#endif
        mouse_task_in_ram();

#if(MOUSE_BATT_MOUDULE_EN)
        mouse_battery_check(battValue, sizeof(battValue)/sizeof(battValue[0]),2200);
#endif
        device_led_process();										//Debug, no led
        int wakeup_status = cpu_sleep_wakeup( mouse_sleep.mode == SLEEP_MODE_DEEPSLEEP, mouse_sleep.wakeup_src, mouse_sleep.wakeup_next_tick );

    	if(!(wakeup_status & 2)){  // if not timer wakeup, need re_sync chn
    		device_sync = 0;
    	}
	}
    mouse_status.loop_cnt ++;
}


