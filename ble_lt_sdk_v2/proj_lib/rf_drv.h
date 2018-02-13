
#pragma once

#if(__TL_LIB_8266__ || MCU_CORE_TYPE == MCU_CORE_8266)
#include "rf_drv_8266.h"
#elif(__TL_LIB_MSSOC__ || (MCU_CORE_TYPE == MCU_CORE_MSSOC))
#include "rf_drv_mssoc.h"
#endif

