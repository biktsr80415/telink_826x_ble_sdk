
#pragma once


#define	CHIP_TYPE_8255  	1
#define	CHIP_TYPE_8258  	2



#ifndef CHIP_TYPE
#define	CHIP_TYPE 			1000
#endif






#define	MCU_CORE_8255 		1
#define	MCU_CORE_8258 		2


#if(CHIP_TYPE == CHIP_TYPE_8255)
	#define MCU_CORE_TYPE	MCU_CORE_8255
#elif(CHIP_TYPE == CHIP_TYPE_8258)
	#define MCU_CORE_TYPE	MCU_CORE_8258
#else
	#define MCU_CORE_TYPE	1000
#endif


