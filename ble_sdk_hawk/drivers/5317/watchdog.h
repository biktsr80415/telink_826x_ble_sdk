
#pragma once

#include "driver_config.h"
#include "register.h"


static inline void wd_start(void){
#if(MODULE_WATCHDOG_ENABLE)		//  if watchdog not set,  start wd would cause problem
	BM_SET(reg_tmr_ctrl, FLD_TMR_WD_EN);
#endif
}

static inline void wd_startEx(unsigned int timer_ms)
{
#if(MODULE_WATCHDOG_ENABLE)
	reg_tmr_ctrl = MASK_VAL(FLD_TMR_WD_CAPT, ((timer_ms*CLOCK_SYS_CLOCK_1MS >> WATCHDOG_TIMEOUT_COEFF)));
	reg_tmr_ctrl |= FLD_TMR_WD_EN;
#endif
}

static inline void wd_stop(void){
#if(MODULE_WATCHDOG_ENABLE)
	BM_CLR(reg_tmr_ctrl, FLD_TMR_WD_EN);
#endif
}

static inline void wd_clear(void)
{
	reg_tmr_sta = FLD_TMR_STA_WD;
}
/*----------------------------- End of File ----------------------------------*/
