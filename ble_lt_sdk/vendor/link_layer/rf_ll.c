/*
 * rf_ll.c: link layer
 *
 *  Created on: Mar 5, 2014
 *      Author: MZ
 */

#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
//#include "../../proj_lib/blt_ll/blt_ll.h"
#include "../common/tl_audio.h"
//#include "../../proj_lib/blt_ll/trace.h"

#include "../common/rf_frame.h"


#include "rf_ll.h"

u8		chn_mask = 0x80;


#ifndef			CHANNEL_SLOT_TIME
#define			CHANNEL_SLOT_TIME			1000
#endif


#define		PKT_BUFF_SIZE		64

#define		LL_CHANNEL_SYNC_TH				1
#define		LL_CHANNEL_SEARCH_TH			24
#define		LL_CHANNEL_SEARCH_FLAG			BIT(16)
#define		LL_NEXT_CHANNEL(c)				((c + 6) & 14)


unsigned char  		rf_rx_buff[PKT_BUFF_SIZE*2] __attribute__((aligned(4)));
int		rf_rx_wptr;


u8		host_channel = 0;
u8		device_channel;
u8		ll_chn_sel;
u8		ll_rssi;

u16		ll_chn_tick;
u32		ll_chn_rx_tick;
u32		ll_chn_mask = LL_CHANNEL_SEARCH_FLAG;
u32		ll_clock_time;
int		device_packet_received;


volatile int		device_ack_received = 0;
int		device_sync = 0;

/////////////////////////////////////////////////////////////////////


u8	get_next_channel_with_mask(u32 mask, u8 chn)
{
	int chn_high = (mask >> 4) & 0x0f;

	if (mask & LL_CHANNEL_SEARCH_FLAG) {
		return LL_NEXT_CHANNEL (chn);
	}
	else if (chn_high != chn) {
		ll_chn_sel = 1;
		return chn_high;
	}
	else {
		ll_chn_sel = 0;
		return mask & 0x0f;
	}
}


///////////////////////////////////////////////////////////////////////////////
static u32 tick_last_tx;


void ll_device_init (void)
{
	reg_dma_rf_rx_addr = (u16)(u32) (rf_rx_buff);
	reg_dma2_ctrl = FLD_DMA_WR_MEM | (PKT_BUFF_SIZE>>4);   // rf rx buffer enable & size
	reg_dma_chn_irq_msk = 0;
	reg_irq_mask |= FLD_IRQ_ZB_RT_EN;    //enable RF & timer1 interrupt
	reg_rf_irq_mask = FLD_RF_IRQ_RX | FLD_RF_IRQ_TX;
}



_attribute_ram_code_ void irq_device_rx(void)
{
	static u32 irq_device_rx_no = 0;

	u8 * raw_pkt = (u8 *) (rf_rx_buff + rf_rx_wptr * PKT_BUFF_SIZE);
	rf_rx_wptr = (rf_rx_wptr + 1) & 1;
	reg_dma_rf_rx_addr = (u16)(u32) (rf_rx_buff + rf_rx_wptr * PKT_BUFF_SIZE); //set next buffer

	reg_rf_irq_status = FLD_RF_IRQ_RX;

	if	(	raw_pkt[0] >= 15 &&
			RF_PACKET_2M_LENGTH_OK(raw_pkt) &&
			RF_PACKET_CRC_OK(raw_pkt) )	{
		rf_packet_ack_pairing_t *p = (rf_packet_ack_pairing_t *)(raw_pkt + 8);


		extern int  rf_rx_process(u8 *);
		if (rf_rx_process (raw_pkt) && ll_chn_tick != p->tick) {

			ll_chn_tick = p->tick;			//sync time
#if 0
			device_sync = 1;
#endif
			device_ack_received = 1;
			ll_chn_mask = p->chn;			//update channel
			ll_chn_rx_tick = clock_time ();
			ll_rssi = raw_pkt[4];
			irq_device_rx_no++;
		}
		rf_set_channel (device_channel, RF_CHN_TABLE);
		raw_pkt[0] = 1;
	}
}


_attribute_ram_code_ void irq_device_tx(void)
{
	tick_last_tx = clock_time ();
	reg_rf_irq_status = FLD_RF_IRQ_TX;
}



extern rf_packet_pairing_t	pkt_pairing;
task_when_rf_func p_task_when_rf = NULL;

_attribute_ram_code_ int	device_send_packet (u8 * p, u32 timeout, int retry, int pairing_link)
{
	extern u32  cpu_wakup_last_tick;
    while ( !clock_time_exceed (cpu_wakup_last_tick, 500) );    //delay to get stable pll clock
    
	rf_power_enable (1);
	static	u32 ack_miss_no;

	device_ack_received = 0;
	int i;
	int step = 1;
	if (device_sync)
		step = retry + 1;

	for (i=0; i<=retry; i += step) {

		rf_set_channel (device_channel, RF_CHN_TABLE);

		u32 t = clock_time ();
		rf_send_packet (p, 320, step - 1);

		reg_rf_irq_status = 0xffff;

#if 1
        if ( DO_TASK_WHEN_RF_EN && p_task_when_rf != NULL) {
           (*p_task_when_rf) ();
           p_task_when_rf = NULL;
        }
#endif
		while (	!device_ack_received &&
				!clock_time_exceed (t, timeout*step) &&
				!(reg_rf_irq_status & (FLD_RF_IRX_RETRY_HIT | FLD_RF_IRX_CMD_DONE)) );

		if (device_ack_received) {
			ack_miss_no = 0;
			break;
		}
		ack_miss_no ++;
		if (ack_miss_no >= LL_CHANNEL_SEARCH_TH) {
			device_sync = 0;
			ll_chn_mask = LL_CHANNEL_SEARCH_FLAG;
		}
		else if (ack_miss_no >= LL_CHANNEL_SYNC_TH) {
			device_sync = 0;
		}

		if (!device_sync) {	//alternate channel while device not in sync mode
			device_channel = get_next_channel_with_mask (ll_chn_mask, device_channel);
		}
	}

	rf_power_enable (0);

#if 1
	if (i <= retry) {
		return 1;
	}
	else{
		return 0;
	}
#else
	static int adjust = 0;
	int ret = 0;
	if (i <= retry) {
		//sync timing
		u32 t0 = (ll_chn_tick & 0xff) * 32;			//time in host side
		u32 t1 = (ll_chn_rx_tick  - tick) >> SHIFT_US; 	//timeout * (i - 1);

		if (adjust) {
			adjust = 0;
		}
		else if (t0 < t1) {
			adjust = MCU_CORE_TYPE == MCU_CORE_8266 ? 3000 : 3;
		}
		else {
			t1 = t0 - t1;
			if (t1 > 3000) {			//clock slow, wait to next slot
				adjust = MCU_CORE_TYPE == MCU_CORE_8266 ? -1000 : -1;
			}
			else if (t1 < 1000){
				adjust = MCU_CORE_TYPE == MCU_CORE_8266 ? 1000 : 1;
			}
		}
		ret = 1;
	}
	else {
		adjust = 0;
	}

	cpu_adjust_system_tick (adjust);		//adjust 1ms
	return ret;
#endif
}

void ll_add_clock_time (u32 ms)
{
	if (ms > CHANNEL_SLOT_TIME * 8) {
		device_sync = 0;
		ll_clock_time = 0;
	}
	else {
		ll_clock_time += ms;
		while (ll_clock_time >= CHANNEL_SLOT_TIME) {
			ll_clock_time -= CHANNEL_SLOT_TIME;
			device_channel = get_next_channel_with_mask (ll_chn_mask, device_channel);
		}
	}

}


