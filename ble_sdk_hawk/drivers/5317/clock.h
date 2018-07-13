
#pragma once

#include "driver_config.h"
#include "compiler.h"
#include "register.h"
#include "gpio.h"

typedef enum{
	SYS_CLK_16M_PAD = 16000000,
	SYS_CLK_24M_PAD = 24000000,
	SYS_CLK_32M_PAD = 32000000,

	SYS_CLK_16M_RC  = 16000000,
	SYS_CLK_24M_RC  = 24000000,
}eSYS_ClkTypeDef;

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

//void cpu_wakeup_init(void);
//void system_init(void);
void clock_init(void);
_attribute_ram_code_ unsigned int clock_time(void);
_attribute_ram_code_ unsigned int clock_time_exceed(unsigned int ref, unsigned int span_us);
_attribute_ram_code_ void sleep_us (unsigned int microsec);		//  use register counter to delay
void MCU_24M_RC_ClockCalibrate(void);

/* Delay precisely -----------------------------------------------------------*/
#define WaitUs	    sleep_us
#define WaitMs(ms)	sleep_us((ms)*1000)

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

#if (CLOCK_SYS_CLOCK_HZ == 30000000 || CLOCK_SYS_CLOCK_HZ == 32000000)
	#define		CLOCK_DLY_100NS		CLOCK_DLY_3_CYC							// 100,  94
	#define		CLOCK_DLY_200NS		CLOCK_DLY_6_CYC							// 200, 188
	#define 	CLOCK_DLY_600NS 	CLOCK_DLY_10_CYC;CLOCK_DLY_10_CYC 		// 200, 188
#elif (CLOCK_SYS_CLOCK_HZ == 24000000)
	#define 	CLOCK_DLY_63NS 		CLOCK_DLY_3_CYC 		//  63 ns
	#define		CLOCK_DLY_100NS		CLOCK_DLY_4_CYC			//  100 ns
	#define		CLOCK_DLY_200NS		CLOCK_DLY_8_CYC			//  200 ns
	#define 	CLOCK_DLY_600NS 	CLOCK_DLY_10_CYC 		//	600 ns
#elif (CLOCK_SYS_CLOCK_HZ == 12000000 || CLOCK_SYS_CLOCK_HZ == 16000000)
	#define 	CLOCK_DLY_63NS 		CLOCK_DLY_1_CYC 		//  63 ns
	#define		CLOCK_DLY_100NS		CLOCK_DLY_2_CYC			//  128 ns
	#define		CLOCK_DLY_200NS		CLOCK_DLY_4_CYC			//  253 ns
	#define 	CLOCK_DLY_600NS 	CLOCK_DLY_10_CYC 		//	253 ns
#elif (CLOCK_SYS_CLOCK_HZ == 48000000)
	#define		CLOCK_DLY_100NS		CLOCK_DLY_5_CYC			// 104
	#define		CLOCK_DLY_200NS		CLOCK_DLY_10_CYC		// 208
	#define 	CLOCK_DLY_600NS 	CLOCK_DLY_10_CYC;CLOCK_DLY_10_CYC;CLOCK_DLY_10_CYC		//	600 ns
#elif (CLOCK_SYS_CLOCK_HZ == 6000000 || CLOCK_SYS_CLOCK_HZ == 8000000)
	#define		CLOCK_DLY_100NS		CLOCK_DLY_1_CYC			//  125 ns
	#define		CLOCK_DLY_200NS		CLOCK_DLY_2_CYC			//  250
	#define 	CLOCK_DLY_600NS 	CLOCK_DLY_5_CYC 		//  725
#else
#define		CLOCK_DLY_100NS		CLOCK_DLY_1_CYC			//  125 ns
#define		CLOCK_DLY_200NS		CLOCK_DLY_2_CYC			//  250
#define 	CLOCK_DLY_600NS 	CLOCK_DLY_5_CYC 		//  725
#endif

/*----------------------------- End of File ----------------------------------*/
