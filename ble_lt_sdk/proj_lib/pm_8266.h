
#pragma once

#if(__TL_LIB_8266__ || MCU_CORE_TYPE == MCU_CORE_8266)

static inline void usb_dp_pullup_en (int en)
{
	unsigned char dat = ReadAnalogReg(0x00);
	if (en) {
		dat &= ~BIT(4);
	}
	else {
		dat |= BIT(4);
	}

	WriteAnalogReg (0x00, dat);
}

enum {
	 PM_WAKEUP_CORE  = BIT(5),
	 PM_WAKEUP_TIMER = BIT(6),
	 PM_WAKEUP_COMP  = BIT(7),
	 PM_WAKEUP_PAD   = BIT(8),
};


enum {
	 WAKEUP_STATUS_COMP   = BIT(0),  //wakeup by comparator
	 WAKEUP_STATUS_TIMER  = BIT(1),
	 WAKEUP_STATUS_CORE   = BIT(2),
	 WAKEUP_STATUS_PAD    = BIT(3),

	 STATUS_GPIO_ERR_NO_ENTER_PM = BIT(7),
};

#define 	WAKEUP_STATUS_TIMER_CORE	( WAKEUP_STATUS_TIMER | WAKEUP_STATUS_CORE)

#define DEEP_ANA_REG0    0x34
#define DEEP_ANA_REG1    0x35
#define DEEP_ANA_REG2    0x36
#define DEEP_ANA_REG3    0x37
#define DEEP_ANA_REG4    0x38
#define DEEP_ANA_REG5    0x39

#define DEEP_ANA_REG6    0x3c
#define DEEP_ANA_REG7    0x3d
#define DEEP_ANA_REG8    0x3e


#define ADV_DEEP_FLG	 0x01
#define CONN_DEEP_FLG	 0x02



#define SYS_DEEP_ANA_REG	0x3a


void cpu_stall_wakeup_by_timer0(u32 tick_stall);
void cpu_stall_wakeup_by_timer1(u32 tick_stall);
void cpu_stall_wakeup_by_timer2(u32 tick_stall);


typedef int (*suspend_handler_t)(void);
void	bls_pm_registerFuncBeforeSuspend (suspend_handler_t func );



#define SUSPEND_MODE	0
#define DEEPSLEEP_MODE	1




//deepsleep mode must use this function for resume 1.8V analog register
void cpu_wakeup_init(int);
void cpu_set_gpio_wakeup (int pin, int pol, int en);
int cpu_sleep_wakeup (int deepsleep, int wakeup_src, u32 wakeup_tick);



void blc_pm_disableFlashShutdown_when_suspend(void);


#endif
