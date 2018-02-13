
#pragma once

#define	CHIP_PACKAGE_QFN64  1
#define	CHIP_PACKAGE_QFN48  2
#define	CHIP_PACKAGE_QFN40  3
#define	CHIP_PACKAGE_QFN32  4

#ifndef CHIP_PACKAGE
#define	CHIP_PACKAGE 		CHIP_PACKAGE_QFN64
#endif

#define	CHIP_TYPE_8266  	1
#define	CHIP_TYPE_MSSOC  	4



#ifndef CHIP_TYPE
#define	CHIP_TYPE 			1000
#endif

#ifndef CHIP_8366_A1
#define CHIP_8366_A1    1
#endif

#define	MCU_CORE_8266 		1
#define	MCU_CORE_MSSOC 		4


#if(CHIP_TYPE == CHIP_TYPE_8266)
	#define MCU_CORE_TYPE	MCU_CORE_8266
#elif(CHIP_TYPE == CHIP_TYPE_MSSOC)
	#define MCU_CORE_TYPE	MCU_CORE_MSSOC
#else
	#define MCU_CORE_TYPE	1000
#endif


