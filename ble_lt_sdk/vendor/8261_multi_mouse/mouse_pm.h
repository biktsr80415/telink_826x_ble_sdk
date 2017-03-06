/*
 * Author : zjs
 * Data : 2017-01-23
 *
 */

#ifndef _MOUSE_PM_H_
#define _MOUSE_PM_H_		1


#define SYNC_MOST_CNT					2000
#define THRESH_BASIC_SUSPEND   			2000  //1ms * 1000 = 1 S
#define THRESH_LONG_SUSPEND				300  //300 * 100 ms = 30 S
#define THRESH_NO_MOTION_LONG_SUSPEND	60  //6000 * 100 ms = 10min



enum{
	SLEEP_MODE_BASIC_SUSPEND = 0,
	SLEEP_MODE_LONG_SUSPEND = 1,
	SLEEP_MODE_WAIT_DEEP = 2,
	SLEEP_MODE_DEEPSLEEP = 3,
};

typedef struct{
    u8   mode;
    u8   device_busy;
    u8   quick_sleep;
    u8   mcu_sleep_en;  //SIGMA_8640 sensor, can not deepsleep

    u8   wakeup_src;
    u8   sensor_sleep;
    u16  rsvd;

    u16	 basic_suspend_cnt;
    u16  basic_suspend_thresh;

    u16  long_suspend_cnt;   	//65535 max
    u16  long_suspend_thresh;

    u32   wakeup_tick;  		//max 255 ms
    u32	  wakeup_next_tick;



}mouse_sleep_t;

mouse_sleep_t mouse_sleep;

void mouse_sleep_mode_machine(mouse_sleep_t *cfg_slp);
extern void mouse_power_saving_process( mouse_status_t *mouse_status );


#endif
