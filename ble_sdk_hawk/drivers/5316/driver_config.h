#pragma once

#include "common/types.h"//include data type define.

/* System clock initialization -----------------------------------------------*/
#define INTERNAL_RC     0
#define EXTERNANL_XTAL  1

#define CLOCK_SRC               EXTERNANL_XTAL
#define CLOCK_SYS_CLOCK_HZ      16000000
enum{
	CLOCK_SYS_CLOCK_1S  = CLOCK_SYS_CLOCK_HZ,
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
};


/* WatchDog ------------------------------------------------------------------*/
#define MODULE_WATCHDOG_ENABLE	0
#define WATCHDOG_INIT_TIMEOUT	500  //Unit:ms



/* Include -------------------------------------------------------------------*/
//#include "drivers/5316/bsp.h"
//#include "drivers/5316/analog.h"
//#include "drivers/5316/compiler.h"
//#include "drivers/5316/register.h"
//#include "drivers/5316/gpio.h"
//#include "drivers/5316/pwm.h"
//#include "drivers/5316/irq.h"
//#include "drivers/5316/clock.h"
//#include "drivers/5316/random.h"
//#include "drivers/5316/flash.h"
//#include "drivers/5316/rf_drv.h"
//#include "drivers/5316/pm.h"
//#include "drivers/5316/audio.h"
//#include "drivers/5316/adc.h"
//#include "drivers/5316/i2c.h"
//#include "drivers/5316/spi.h"
//#include "drivers/5316/uart.h"
//#include "drivers/5316/register.h"
//#include "drivers/5316/watchdog.h"
//#include "drivers/5316/usbhw_i.h"
//#include "drivers/5316/usbhw.h"
//#include "drivers/5316/usbkeycode.h"
//#include "drivers/5316/register.h"
//#include "drivers/5316/dfifo.h"
//#include "drivers/5316/dma.h"
/*--------------------------End of File --------------------------------*/
