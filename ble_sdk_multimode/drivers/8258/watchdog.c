/*
 * watchdog.c
 *
 *  Created on: 2018-6-6
 *      Author: Administrator
 */
#include "register.h"
#include "gpio.h"
#include "clock.h"


/**
 * @brief     This function set the seconds period.It is likely with WD_SetInterval.
 *            Just this function calculate the value to set the register automatically .
 * @param[in] period_s - The seconds to set. unit is second
 * @return    none
 */
void wd_setintervalms(unsigned int period_ms)
{
	static unsigned short tmp_period_ms = 0;
	//tmp_period_ms = (period_ms*CLOCK_SYS_CLOCK_1MS>>18);
	tmp_period_ms = (period_ms*CLOCK_16M_SYS_TIMER_CLK_1MS>>18);
	reg_tmr2_tick = 0x00000000;    //reset tick register
	reg_tmr_ctrl &= (~FLD_TMR_WD_CAPT);
	reg_tmr_ctrl |= (tmp_period_ms<<9); //set the capture register
}

