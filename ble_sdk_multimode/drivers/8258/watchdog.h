
#pragma once


#include "register.h"

/**
 * @brief     This function set the seconds period.It is likely with WD_SetInterval.
 *            Just this function calculate the value to set the register automatically .
 * @param[in] period_s - The seconds to set. unit is second
 * @return    none
 */
extern void wd_setintervalms(unsigned int period_ms);

/**
 * @brief     start watchdog. ie enable watchdog
 * @param[in] none
 * @return    none
 */
static inline void wd_start(void){
#if(MODULE_WATCHDOG_ENABLE)		//  if watchdog not set,  start wd would cause problem
	BM_SET(reg_tmr_ctrl, FLD_TMR2_EN);
	BM_SET(reg_tmr_ctrl, FLD_TMR_WD_EN);
#endif
}
/**
 * @brief     stop watchdog. ie disable watchdog
 * @param[in] none
 * @return    none
 */
static inline void wd_stop(void){
#if(MODULE_WATCHDOG_ENABLE)
	BM_CLR(reg_tmr_ctrl, FLD_TMR_WD_EN);
#endif
}

static inline void wd_clear(void)
{
	reg_tmr_sta = FLD_TMR_STA_WD;
}

