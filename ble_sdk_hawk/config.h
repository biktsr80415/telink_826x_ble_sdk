#pragma once

#define CHIP_TYPE_5316      1
#define CHIP_TYPE_5317      2

#ifndef CHIP_TYPE
#define	CHIP_TYPE 			1000
#endif



#define MCU_CORE_5316       1
#define MCU_CORE_5317       2

#if(CHIP_TYPE == CHIP_TYPE_5316)
	#define MCU_CORE_TYPE   MCU_CORE_5316
#elif(CHIP_TYPE == CHIP_TYPE_5317)
	#define MCU_CORE_TYPE   MCU_CORE_5317
#else
	#define MCU_CORE_TYPE	1000
#endif


