#pragma once

#include "driver_config.h"
#include "compiler.h"
#include "bsp.h"
#include "gpio.h"

#define SUSPEND_MODE	0
#define DEEPSLEEP_MODE	1

//5316 analog register 0x34-0x3e can store infomation when MCU in deepsleep mode
//store your information in these ana_regs before deepsleep by calling analog_write function
//when MCU wakeup from deepsleep, read the information by by calling analog_read function

//these five below are stable
#define DEEP_ANA_REG0    0x3a
#define DEEP_ANA_REG1    0x3b
#define DEEP_ANA_REG2    0x3c
#define DEEP_ANA_REG3    0x3d

//these six below may have some problem when user enter deepsleep but ERR wakeup
// for example, when set a GPIO PAD high wakeup deepsleep, but this gpio is high before
// you call func cpu_sleep_wakeup, then deepsleep will be ERR wakeup, these 6 register
//   infomation loss.
#define DEEP_ANA_REG5    0x34
#define DEEP_ANA_REG6    0x35
#define DEEP_ANA_REG7    0x36
#define DEEP_ANA_REG8    0x37
#define DEEP_ANA_REG9    0x38
#define DEEP_ANA_REG10   0x39

#define ADV_DEEP_FLG	 0x01
#define CONN_DEEP_FLG	 0x02

#define SYS_DEEP_ANA_REG 0x3e  //ana_3e system use for external 32k mode, user can not use


#define BLT_RESET_WAKEUP_TIME_2000		1
#if (BLT_RESET_WAKEUP_TIME_2000)
	#define RESET_TIME_US	    	  2000//1500
	#define EARLYWAKEUP_TIME_US       2150//1620
	#define EMPTYRUN_TIME_US       	  2450//1920
#elif(BLT_RESET_WAKEUP_TIME_2200)
	#define RESET_TIME_US	    	  2200
	#define EARLYWAKEUP_TIME_US       2300
	#define EMPTYRUN_TIME_US       	  2600
#elif(BLT_RESET_WAKEUP_TIME_2400)
	#define RESET_TIME_US	    	  2400
	#define EARLYWAKEUP_TIME_US       2500
	#define EMPTYRUN_TIME_US       	  2800
#elif(BLT_RESET_WAKEUP_TIME_2600)
	#define RESET_TIME_US	    	  2600
	#define EARLYWAKEUP_TIME_US       2700
	#define EMPTYRUN_TIME_US       	  3000
#elif(BLT_RESET_WAKEUP_TIME_2800)
	#define RESET_TIME_US	    	  2800
	#define EARLYWAKEUP_TIME_US       2900
	#define EMPTYRUN_TIME_US       	  3200
#else

#endif

/* Sleep mode define */
typedef enum{
	PM_SleepMode_Suspend = 0x00,
	PM_SLeepMode_Deep    = 0x01,
}PM_SleepModeTypdeDef;

/* Wakeup source define */
typedef enum {
	 PM_WAKEUP_PAD   = BIT(4),
	 PM_WAKEUP_CORE  = BIT(5),
	 PM_WAKEUP_TIMER = BIT(6),
}PM_WakeupSrcTypeDef;

/* Wakeup status from return value of "cpu_sleep_wakeup()" */
typedef enum {
	 WAKEUP_STATUS_COMP   = BIT(0),  //wakeup by comparator
	 WAKEUP_STATUS_TIMER  = BIT(1),
	 WAKEUP_STATUS_CORE   = BIT(2),
	 WAKEUP_STATUS_PAD    = BIT(3),

	 STATUS_GPIO_ERR_NO_ENTER_PM  = BIT(7),//Abnormal wakeup
}PM_WakeupStatusTypeDef;

#define WAKEUP_STATUS_TIMER_CORE	( WAKEUP_STATUS_TIMER | WAKEUP_STATUS_CORE)

/* 32K clock source define.(Low Speed Clock(LSC)) */
typedef enum{
	LSC_32kSrc_RC   = (1<<0),
	LSC_32kSrc_XTAL = (1<<1),
}LSC_32kSrcTypeDef;

typedef void (*pm_optimize_handler_t)(void);
typedef int (*suspend_handler_t)(void);
typedef int (*cpu_pm_handler_t)(int, int, unsigned int);

extern cpu_pm_handler_t cpu_sleep_wakeup;

/*-- User Interface ----------------------------------------------------------*/
void cpu_wakeup_init(void);
void pm_init(LSC_32kSrcTypeDef LSC_32KSrc);
_attribute_ram_code_ _attribute_no_inline_ void  sleep_start(void);

void MCU_32kClockSourceSelect(LSC_32kSrcTypeDef LSC_32kSrc);
void mcu_32k_rc_clock_calibration(void);

void bls_pm_registerFuncBeforeSuspend(suspend_handler_t func );
int cpu_sleep_wakeup_32krc(int deepsleep, int wakeup_src, unsigned int wakeup_tick);
int cpu_sleep_wakeup_32kpad(int deepsleep, int wakeup_src, unsigned int wakeup_tick);

void blt_pm_ext32k_crystal_init(void);

void cpu_set_gpio_wakeup(GPIO_PinTypeDef pin, int pol, int en);
void pm_set_filter(unsigned char en);

void cpu_stall_wakeup_by_timer0(unsigned int tick_stall);
void cpu_stall_wakeup_by_timer1(unsigned int tick_stall);
void cpu_stall_wakeup_by_timer2(unsigned int tick_stall);
/*---------------------------- End of File -----------------------------------*/
