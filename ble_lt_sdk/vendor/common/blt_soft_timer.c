/*
 * blt_soft_timer.c
 *
 *  Created on: 2016-10-28
 *      Author: Administrator
 */

#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"

#include "../common/blt_soft_timer.h"


#if (BLT_SOFTWARE_TIMER_ENABLE)




blt_soft_timer_t	blt_timer;


//���ն�ʱʱ�佫timer���򣬱���processʱ ���δ���timer
int  blt_soft_timer_sort(void)
{
	if(blt_timer.currentNum < 1 || blt_timer.currentNum > MAX_TIMER_NUM){
		write_reg32(0x8000, 0x11111120); while(1); //debug ERR
		return 0;
	}
	else{
		// ð������  BubbleSort
		int n = blt_timer.currentNum;
		u8 temp[sizeof(blt_time_event_t)];

		for(int i=0;i<n-1;i++)
		{
			for(int j=0;j<n-i-1;j++)
			{
				if(TIME_COMPARE_BIG(blt_timer.timer[j].t, blt_timer.timer[j+1].t))
				{
					//swap
					memcpy(temp, &blt_timer.timer[j], sizeof(blt_time_event_t));
					memcpy(&blt_timer.timer[j], &blt_timer.timer[j+1], sizeof(blt_time_event_t));
					memcpy(&blt_timer.timer[j+1], temp, sizeof(blt_time_event_t));
				}
			}
		}
	}

	return 1;
}



//user add timer
int blt_soft_timer_add(blt_timer_callback_t func, u32 interval_us)
{
	int i;
	u32 now = clock_time();

	if(blt_timer.currentNum >= MAX_TIMER_NUM){  //timer full
		return 	0;
	}
	else{
		blt_timer.timer[blt_timer.currentNum].cb = func;
		blt_timer.timer[blt_timer.currentNum].interval = interval_us * CLOCK_SYS_CLOCK_1US;
		blt_timer.timer[blt_timer.currentNum].t = now + blt_timer.timer[blt_timer.currentNum].interval;
		blt_timer.currentNum ++;

		blt_soft_timer_sort();
		return  1;
	}
}


//timer ������������ģ�ɾ����ʱ�򣬲�����ǰ���ǣ����Բ����ƻ�˳�򣬲���Ҫ��������
int  blt_soft_timer_delete(u8 index)
{
	if(index >= blt_timer.currentNum){
		write_reg32(0x8000, 0x11111121); while(1); //debug ERR
		return 0;
	}


	for(int i=index; i<blt_timer.currentNum - 1; i++){
		memcpy(&blt_timer.timer[i], &blt_timer.timer[i+1], sizeof(blt_time_event_t));
	}

	blt_timer.currentNum --;
}



void  	blt_soft_timer_process(u8 e, u8 *p)
{
	if(e == BLT_EV_FLAG_USER_TIMER_WAKEUP){  //ealry wakeup

	}

	u32 now = clock_time();
	if(!blt_timer.currentNum || !blt_is_timer_expired(blt_timer.timer[0].t, now) ){
		return;
	}

	int change_flg = 0;
	int result;
	for(int i=0; i<blt_timer.currentNum; i++){
		if(blt_is_timer_expired(blt_timer.timer[i].t ,now) ){ //timer trigger

			if(blt_timer.timer[i].cb == NULL){
				write_reg32(0x8000, 0x11111122); while(1); //debug ERR
			}
			else{
				result = blt_timer.timer[i].cb();

				if(result < 0){
					blt_soft_timer_delete(i);
				}
				else if(result == 0){
					change_flg = 1;
					blt_timer.timer[i].t = now + blt_timer.timer[i].interval;
				}
				else{  //set new timer interval
					change_flg = 1;
					blt_timer.timer[i].interval = result * CLOCK_SYS_CLOCK_1US;
					blt_timer.timer[i].t = now + blt_timer.timer[i].interval;
				}
			}
		}
	}


	if(blt_timer.currentNum ){ //timer table not empty
		if(change_flg){
			blt_soft_timer_sort();
		}

		u32 nearest_timer_diff = (u32)(blt_timer.timer[0].t - now);
		if( nearest_timer_diff < 2100 *  CLOCK_SYS_CLOCK_1MS){
			//enable timer early wakeup long suspend(2100ms most)
			bls_pm_setUserTimerWakeup(nearest_timer_diff/CLOCK_SYS_CLOCK_1US, 1);
		}
		else{
			bls_pm_setUserTimerWakeup(0, 0);  //disable
		}

	}

}


void 	blt_soft_timer_init(void)
{
	//ʹ�� BLT_EV_FLAG_USER_TIMER_WAKEUP �ص�������ʱ��ʱ�����ô���suspend��ʱ������ͨ��
	//bls_pm_setUserTimerWakeup����ǰ��suspend���ѣ�Ȼ�󴥷�BLT_EV_FLAG_USER_TIMER_WAKEUP
	bls_app_registerEventCallback (BLT_EV_FLAG_USER_TIMER_WAKEUP, &blt_soft_timer_process);
}


#endif  //end of  BLT_SOFTWARE_TIMER_ENABLE

