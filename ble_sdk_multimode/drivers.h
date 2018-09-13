
#pragma once

#include "config.h"

#if(__TL_LIB_8255__ || (MCU_CORE_TYPE == MCU_CORE_8255))
#include "drivers/8255/driver_8255.h"
#elif(__TL_LIB_8258__ || (MCU_CORE_TYPE == MCU_CORE_8258))
#include "drivers/8258/driver_8258.h"
#else
#include "drivers/8258/driver_8258.h"
#endif
