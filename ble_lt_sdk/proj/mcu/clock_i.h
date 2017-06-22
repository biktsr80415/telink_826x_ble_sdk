
#pragma once

static inline void clock_enable_tmr_irq(int tmr, u32 en){
	if (tmr > 2 || tmr < 0)
		return;
	if (en)
		irq_set_mask(FLD_IRQ_TMR0_EN << tmr);
	else
		irq_clr_mask(FLD_IRQ_TMR0_EN << tmr);
}

// we use clock insteady of timer, to differentiate OS timers utility
static inline void clock_enable_clock(int tmr, u32 en){
	if (tmr > 2 || tmr < 0)
		return;
	SET_FLD_V(reg_tmr_ctrl, FLD_TMR0_EN << (tmr * 3), en);
}

#ifdef WIN32
static inline unsigned __int64   clock_get_cycle()
{ 
  __asm   _emit   0x0F 
  __asm   _emit   0x31 
}
#endif

static inline u32 clock_time(){
#ifdef WIN32
#if (__LOG_RT_ENABLE__)
	unsigned __int64 tt = (unsigned __int64)clock_get_cycle();
	u32 tick = (u32)(tt * 1000000 / (2*1000*1000*1000));	// assuming the cpu clock of PC is 2 giga HZ
	return tick;
#else
	static u32 tick = 0;
	// 经验数据，不是太准。比较好的做法是，设置一个10秒的定时器，然后看实际时间差多少
	// 再自行调整比例。不能用系统时钟，因为需要在断点后保持timer 准确
	tick += 2; //(CLOCK_SYS_CLOCK_1US * 20);	
	return tick;
#endif
#else
	//return reg_tmr0_tick;
	return reg_system_tick;
#endif
}

static inline u32 clock_tmr_get_tick(int tmr){
	return reg_tmr_tick(tmr);
}

// check if the current time is exceed span_us from ref time
#ifndef			USE_SYS_TICK_PER_US
static inline u32 clock_time_exceed(u32 ref, u32 span_us){
	return ((u32)(clock_time() - ref) > span_us * CLOCK_SYS_CLOCK_1US);
}
#else
static inline u32 clock_time_exceed(u32 ref, u32 span_us){
	return ((u32)(clock_time() - ref) > span_us * sys_tick_per_us);
}
#endif

// more efficient than clock_set_interval
static inline void clock_set_tmr_interval(int tmr, u32 intv){
	reg_tmr_capt(tmr) = intv;
}

static inline void clock_set_tmr_tick(int tmr, u32 tick){
	reg_tmr_tick(tmr) = tick;
}

static inline void clock_set_tmr_mode(int tmr, u32 m){
	if(0 == tmr){
		SET_FLD_V(reg_tmr_ctrl16, FLD_TMR0_MODE, m);
	}else if(1 == tmr){
		SET_FLD_V(reg_tmr_ctrl16, FLD_TMR1_MODE, m);
	}else{
		SET_FLD_V(reg_tmr_ctrl16, FLD_TMR2_MODE, m);
	}
}

static inline u32 clock_get_tmr_status(int tmr) {
	if (tmr > 2 || tmr < 0)
		return 0;
	return reg_tmr_ctrl & (FLD_TMR0_STA << tmr);
}

static inline void clock_clr_tmr_irq(int tmr) {
	if (tmr > 2 || tmr < 0)
		return;

	u8 timer0_irq_src = BIT(0);

	reg_tmr_sta |= FLD_TMR_STA_TMR0;
	reg_irq_src |= (timer0_irq_src << tmr);
}

/**
 * @brief     The mode1 and mode2 of timer are related to gpio. so need to config the gpio.
 *            This function config polarity and mode of pin.(mode1:gpio count. mode2:gpio pulse)
 * @param[in] timer_n - select the timer to config.enum varialbe(timer0,timer1, timer2)
 * @param[in] pin - select the pin to capture.
 * @param[in] falling - config the polarity of pin.
 * @return    none.
 */
static inline void clock_set_tmr_gpio_mode(int tmr, u32 pin, u8 falling)
{
    u8 bit = pin & 0xff;

    if (tmr > 2 || tmr < 0)
        return;

    clock_set_tmr_tick(tmr, 0);
    gpio_set_interrupt_pol(pin, falling);
    BM_SET(reg_gpio_risc(tmr, pin), bit);
}

