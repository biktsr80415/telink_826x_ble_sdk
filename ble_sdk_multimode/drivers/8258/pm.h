
#pragma once

#include "bsp.h"
#include "gpio.h"


static inline void usb_dp_pullup_en (int en)
{
	unsigned char dat = ReadAnalogReg(0x0b);
	if (en) {
		dat = dat | BIT(7);
	}
	else
	{
		dat = dat & 0x7f ;
	}

	WriteAnalogReg (0x0b, dat);
}





//analog register below can store infomation when MCU in deepsleep mode
//store your information in these ana_regs before deepsleep by calling analog_write function
//when MCU wakeup from deepsleep, read the information by by calling analog_read function

//these five below are stable
#define DEEP_ANA_REG0    0x3a
#define DEEP_ANA_REG1    0x3b
#define DEEP_ANA_REG2    0x3c
#define DEEP_ANA_REG3    0x3d
#define DEEP_ANA_REG4  	 0x3e


//these analog register below may have some problem when user enter deepsleep but ERR wakeup
// for example, when set a GPIO PAD high wakeup deepsleep, but this gpio is high before
// you call func cpu_sleep_wakeup, then deepsleep will be ERR wakeup, these analog register
//   infomation loss.
#define DEEP_ANA_REG6    0x35
#define DEEP_ANA_REG7    0x36
#define DEEP_ANA_REG8    0x37
#define DEEP_ANA_REG9    0x38
#define DEEP_ANA_REG10   0x39


#define ADV_DEEP_FLG	 0x01
#define CONN_DEEP_FLG	 0x02


typedef enum {
	SUSPEND_MODE						= 0,
	DEEPSLEEP_MODE						= 0x80,
	DEEPSLEEP_MODE_RET_SRAM_LOW8K		= 0x61,
	DEEPSLEEP_MODE_RET_SRAM_HIGH8K 		= 0x52,
	DEEPSLEEP_MODE_RET_SRAM_LOW16K  	= 0x43,
	DEEPSLEEP_MODE_RET_SRAM_HIGH16K  	= 0x34,
	DEEPSLEEP_MODE_RET_SRAM_LOW32K  	= 0x07,

	DEEPSLEEP_RETENTION_FLAG	= 0x7F,
}SleepMode_TypeDef;



//set wakeup source
typedef enum {
	 PM_WAKEUP_PAD   = BIT(4),
	 PM_WAKEUP_CORE  = BIT(5),
	 PM_WAKEUP_TIMER = BIT(6),
	 PM_WAKEUP_COMP  = BIT(7),
}SleepWakeupSrc_TypeDef;;



//wakeup status from return value of "cpu_sleep_wakeup"
enum {
	 WAKEUP_STATUS_COMP   = BIT(0),  //wakeup by comparator
	 WAKEUP_STATUS_TIMER  = BIT(1),
	 WAKEUP_STATUS_CORE   = BIT(2),
	 WAKEUP_STATUS_PAD    = BIT(3),

	 STATUS_GPIO_ERR_NO_ENTER_PM  = BIT(7),
};

#define 	WAKEUP_STATUS_TIMER_CORE	( WAKEUP_STATUS_TIMER | WAKEUP_STATUS_CORE)
#define 	WAKEUP_STATUS_TIMER_PAD		( WAKEUP_STATUS_TIMER | WAKEUP_STATUS_PAD)



typedef struct{
	unsigned char is_deepRetn_back;
	unsigned char is_pad_wakeup;
	unsigned char wakeup_src;
}pm_para_t;

extern pm_para_t	pmParam;



void cpu_stall_wakeup_by_timer0(unsigned int tick_stall);
void cpu_stall_wakeup_by_timer1(unsigned int tick_stall);
void cpu_stall_wakeup_by_timer2(unsigned int tick_stall);

typedef int (*suspend_handler_t)(void);
void	bls_pm_registerFuncBeforeSuspend (suspend_handler_t func );




void cpu_wakeup_init(void);
void cpu_set_gpio_wakeup (GPIO_PinTypeDef pin, GPIO_LevelTypeDef pol, int en);

int cpu_sleep_wakeup (SleepMode_TypeDef sleep_mode, SleepWakeupSrc_TypeDef wakeup_src, u32 wakeup_tick);



static inline int pm_is_MCU_deepRetentionWakeup(void)
{
	return pmParam.is_deepRetn_back;
}



static inline int pm_is_deepPadWakeup(void)
{
	return pmParam.is_pad_wakeup;
}


//only for debug below, will remove them later

typedef enum {
	SUSPENDWAKEUP_SRC_NONE       = 0,
	SUSPENDWAKEUP_SRC_PAD        = BIT(4),
    SUSPENDWAKEUP_SRC_DIG_GPIO   = BIT(5) | 0X0800,
    SUSPENDWAKEUP_SRC_DIG_USB    = BIT(5) | 0X0400,
    SUSPENDWAKEUP_SRC_DIG_QDEC   = BIT(5) | 0X1000,  //0x6e[4] of 8267 is qdec wakeup enbale bit
    SUSPENDWAKEUP_SRC_TIMER      = BIT(6),
    SUSPENDWAKEUP_SRC_COMP       = BIT(7),
}SuspendWakeupSrc_TypeDef;

typedef enum {
	DEEPWAKEUP_SRC_NONE       = 0,
    DEEPWAKEUP_SRC_PAD        = BIT(4),
    DEEPWAKEUP_SRC_TIMER      = BIT(6),
    DEEPWAKEUP_SRC_COMP       = BIT(7),
}DeepWakeupSrc_TypeDef;

typedef enum {
	DEEP_RET_SRAM_NONE		= 0x00,
	DEEP_RET_SRAM_LOW8K		= 0x61,
	DEEP_RET_SRAM_HIGH8K 	= 0x52,
	DEEP_RET_SRAM_ALL16K  	= 0x34,
}DeepRetSram_TypeDef;


int PM_SuspendEnter(SuspendWakeupSrc_TypeDef WakeupSrc, unsigned long WakeupTick);
int PM_DeepSleepEnter(DeepWakeupSrc_TypeDef WakeupSrc, unsigned long WakeupTick,DeepRetSram_TypeDef RetSRAM);

#define PM_DC_RESET_DURATION     3000 //us
#define PM_WAKE_UP_MARGIN        3100 //us
#define PM_SLEEP_DURATION_MIN    3400 //us
#define PM_SLEEP_DURATION_MAX    0xc0000000 //sys ticks



void shutdown_gpio(void);  //for debug


#define WRITE_REG8				 write_reg8

#define PM_Get32kTick			 cpu_get_32k_tick
#define pm_start				 sleep_start

