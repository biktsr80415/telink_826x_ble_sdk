
#pragma once


#include "common/types.h"
#include "common/bit.h"
#include "common/utility.h"
#include "common/static_assert.h"
#include "common/assert.h"
#include "mcu/analog.h"
#include "common/compatibility.h"
#include "../vendor/common/user_config.h"
#include "mcu/config.h"
#include "mcu/compiler.h"
#include "mcu/register.h"
#include "mcu/gpio.h"

#include "mcu/cpu.h"
#include "mcu/irq_i.h"
#include "mcu/clock.h"
#include "mcu/clock_i.h"
#include "mcu/random.h"
#include "common/breakpoint.h"
#include "common/log.h"

#include "drivers/flash.h"

//#include "../ble/ble_globals.h"

#ifdef WIN32
#include <stdio.h>
#include <string.h>
#else
#include "common/printf.h"
#include "common/string.h"
#endif

