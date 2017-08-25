#ifndef  _APP_GPIO_IRQ_H
#define  _APP_GPIO_IRQ_H

#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"

#define   GPIO_TEST_PIN     GPIO_PC7
#define   GPIO_LED_PIN      GPIO_PD5

enum gpio_pol{
	pol_rising,
	pol_falling,
};

enum gpio_irqmode {
	irq_gpio   = BIT(2),
	gpio2risc0 = BIT(5),
	gpio2risc1 = BIT(6),
	gpio2risc2 = BIT(7),
};

#endif
