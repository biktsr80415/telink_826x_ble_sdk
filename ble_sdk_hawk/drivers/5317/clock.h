
#pragma once

#include "driver_config.h"
#include "compiler.h"
#include "register.h"
#include "gpio.h"

#if (CLOCK_SYS_CLOCK_HZ == 16000000)
	#define	SYS_TICK_DIV	1
#elif (CLOCK_SYS_CLOCK_HZ == 32000000)
	#define	SYS_TICK_DIV	2
#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
	#define	SYS_TICK_DIV	3
#endif


#ifndef SYS_TICK_DIV
#define	SYS_TICK_DIV	1   //16M system clock
#endif

typedef enum{
	SYS_CLK_16M_Crystal = 0x43,
	SYS_CLK_24M_Crystal = 0x42,
	SYS_CLK_32M_Crystal = 0x60,
	SYS_CLK_16M_RC 		= 0xE0,
	SYS_CLK_24M_RC      = 0x00,
}SYS_CLK_TYPEDEF;



/* 5316 system clock source define. */
#define SYS_CLK_SRC_24M_RC              0
#define SYS_CLK_SRC_FHS                 1
#define SYS_CLK_SRC_FHS_DIV             2
#define SYS_CLK_SRC_FHS_2V3_DIV         3

/* 5316 FHS clock source define. */
//use for 0x66[7]
#define FHS_CLK_SRC_L_48M_OR_24M_PAD     0
#define FHS_CLK_SRC_L_24M_RC             1

//use for 0x70[0]
#define FHS_CLK_SRC_H_48M_OR_24M_RC      0
#define FHS_CLK_SRC_H_24M_PAD            1

//system timer clock source is constant 16M, never change
enum{
	CLOCK_16M_SYS_TIMER_CLK_1S =  16000000,
	CLOCK_16M_SYS_TIMER_CLK_1MS = 16000,
	CLOCK_16M_SYS_TIMER_CLK_1US = 16,
};
#define sys_tick_per_us		 16

enum{
	CLOCK_MODE_SCLK = 0,
	CLOCK_MODE_GPIO = 1,
	CLOCK_MODE_WIDTH_GPI = 2,
	CLOCK_MODE_TICK = 3
};


void clock_init(SYS_CLK_TYPEDEF SYS_CLK);

static inline unsigned int clock_time(void)
{
 		return reg_system_tick;
}

unsigned int clock_time_exceed(unsigned int ref, unsigned int span_us);

void sleep_us (unsigned int microsec);		//  use register counter to delay

void MCU_24M_RC_ClockCalibrate(void);

/* Delay precisely -----------------------------------------------------------*/
#define WaitUs	     sleep_us
#define WaitMs(ms)	 sleep_us((ms)*1000)
#define sleep_ms(ms) sleep_us((ms)*1000)

#define _ASM_NOP_          asm("tnop")

#define	CLOCK_DLY_1_CYC    _ASM_NOP_
#define	CLOCK_DLY_2_CYC    _ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_3_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_4_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_5_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_6_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_7_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_8_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_9_CYC    _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_
#define	CLOCK_DLY_10_CYC   _ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_;_ASM_NOP_

/*----------------------------- End of File ----------------------------------*/
