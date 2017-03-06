/*
 * dongle_rf.h
 *
 *  Created on: Feb 13, 2014
 *      Author: xuzhen
 */

#ifndef _RF_LINK_LAYER_H_
#define _RF_LINK_LAYER_H_

typedef void (*callback_rx_func) (u8 *);
typedef void (*task_when_rf_func) (void);

#if(CLOCK_SYS_CLOCK_HZ == 32000000 || CLOCK_SYS_CLOCK_HZ == 24000000)
	#define			SHIFT_US		5
#elif(CLOCK_SYS_CLOCK_HZ == 16000000 || CLOCK_SYS_CLOCK_HZ == 12000000)
	#define			SHIFT_US		4
#elif(CLOCK_SYS_CLOCK_HZ == 8000000)
	#define			SHIFT_US		3
#else
	#error clock not set properly
#endif


u8	get_next_channel_with_mask(u32 mask, u8 chn);
void ll_device_init (void);
void irq_device_rx(void);
void irq_device_tx(void);
int	device_send_packet (u8 * p, u32 timeout, int retry, int pairing_link);
void ll_add_clock_time (u32 ms);

extern int device_sync;
extern task_when_rf_func p_task_when_rf;

#define DO_TASK_WHEN_RF_EN      1



#endif /* _RF_LINK_LAYER_H_ */
