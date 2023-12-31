/********************************************************************************************************
 * @file     clock.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 public@telink-semi.com;
 * @date     May. 12, 2018
 *
 * @par      Copyright (c) Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/

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
	SYS_CLK_32M_RC 		= 0x01,
	SYS_CLK_48M_RC 		= 0x02,
	SYS_CLK_16M_Crystal = 0x43,
	SYS_CLK_24M_Crystal = 0x42,

	SYS_CLK_RC_THRES	= 0x10,  //not clock setting ,just for code write
}SYS_CLK_TYPEDEF;




void doubler_calibration(void);

void clock_rc_set(SYS_CLK_TYPEDEF SYS_CLK);
void rc_24m_cal (void);
void rc_32k_cal (void);
void rc_48m_cal (void);

extern int clock_rc_enable;
static inline int clock_isRcCLock(void)
{
	return clock_rc_enable;
}


/********************************* User Interface  *******************************************/

void clock_init(SYS_CLK_TYPEDEF SYS_CLK);

void sleep_us (unsigned long microsec);		//  use register counter to delay


static inline unsigned int clock_time(void)
{
	return reg_system_tick;
}



static inline unsigned int clock_time_exceed(unsigned int ref, unsigned int span_us){
	return ((unsigned int)(clock_time() - ref) > span_us * 16);
}



void start_reboot(void);

#define ClockTime				 clock_time
#define WaitUs				sleep_us
#define WaitMs(t)			sleep_us((t)*1000)

/*********************************************************************************************/
