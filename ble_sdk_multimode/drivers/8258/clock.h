
#pragma once


#include "compiler.h"
#include "register.h"
// constants
// system clock



//system timer clock source is constant 16M, never change
enum{
	CLOCK_16M_SYS_TIMER_CLK_1S =  16000000,
	CLOCK_16M_SYS_TIMER_CLK_1MS = 16000,
	CLOCK_16M_SYS_TIMER_CLK_1US = 16,
};


#define 	sys_tick_per_us			16



#define _ASM_NOP_				asm("tnop")

//  delay precisely
#define		CLOCK_DLY_1_CYC    _ASM_NOP_
#define		CLOCK_DLY_2_CYC    _ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_3_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_4_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_5_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_6_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_7_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_8_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_9_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define		CLOCK_DLY_10_CYC   _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_


typedef enum{
	SYS_CLK_12M_Crystal = 0x44,
	SYS_CLK_16M_Crystal = 0x43,
	SYS_CLK_24M_Crystal = 0x42,
	SYS_CLK_32M_Crystal = 0x60,
	SYS_CLK_48M_Crystal = 0x20,
	SYS_CLK_24M_RC      = 0x00,
}SYS_CLK_TYPEDEF;

//void clock_init(SYS_CLK_TYPEDEF SYS_CLK);

static inline void clock_init(SYS_CLK_TYPEDEF SYS_CLK)
{
	reg_clk_sel = (unsigned char)SYS_CLK;

#if (MODULE_WATCHDOG_ENABLE)
	reg_tmr_ctrl = MASK_VAL(
		FLD_TMR_WD_CAPT, (MODULE_WATCHDOG_ENABLE ? (WATCHDOG_INIT_TIMEOUT * CLOCK_SYS_CLOCK_1MS >> WATCHDOG_TIMEOUT_COEFF):0)
		, FLD_TMR_WD_EN, (MODULE_WATCHDOG_ENABLE?1:0));
#endif
}


_attribute_ram_code_ void sleep_us (unsigned long microsec);		//  use register counter to delay


#define WaitUs				sleep_us
#define WaitMs(t)			sleep_us((t)*1000)


static inline unsigned int clock_time(void)
{
	return reg_system_tick;
}


#define ClockTime				 clock_time


static inline unsigned int clock_time_exceed(unsigned int ref, unsigned int span_us){
	return ((unsigned int)(clock_time() - ref) > span_us * 16);
}


void start_reboot(void);
