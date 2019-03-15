
#pragma once

#include "config.h"

#if (__TL_LIB_5316__ || (MCU_CORE_TYPE == MCU_CORE_5316))
#include "drivers/5316/driver_5316.h"
#elif(__TL_LIB_5317__ || (MCU_CORE_TYPE == MCU_CORE_5317))
#include "drivers/5317/driver_5317.h"
#endif




