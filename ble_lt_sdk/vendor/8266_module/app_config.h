#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#define _USER_CONFIG_DEFINED_	1	// must define this macro to make others known
#define	__LOG_RT_ENABLE__		0
//#define	__DEBUG_PRINT__			0
//////////// product  Information  //////////////////////////////
#define ID_VENDOR				0x248a			// for report
#define ID_PRODUCT_BASE			0x880C
#define STRING_VENDOR			L"Telink"
#define STRING_PRODUCT			L"BLE Remote"
#define STRING_SERIAL			L"TLSR8266"

#define CHIP_TYPE				CHIP_TYPE_8266		// 8866-24, 8566-32
#define APPLICATION_DONGLE		0					// or else APPLICATION_DEVICE
#define	FLOW_NO_OS				1

#define		RF_FAST_MODE_1M		1

//////////////////////CAN NOT CHANGE CONFIG ABOVE ////////////////////////////


/////////////////////HCI ACCESS OPTIONS///////////////////////////////////////
#define HCI_USE_UART	1
#define HCI_USE_USB		0
#define HCI_ACCESS		HCI_USE_UART//HCI_USE_USB


/////////////////// MODULE /////////////////////////////////
#define BLE_REMOTE_PM_ENABLE			0
#define TELIK_SPP_SERVICE_ENABLE		1



//////////////////////////// KEYSCAN/MIC  GPIO //////////////////////////////////
#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K
#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K
#define	KB_LINE_HIGH_VALID				0   //dirve pin output 0 when keyscan, scanpin read 0 is valid

#define			CR_VOL_UP		0xf0
#define			CR_VOL_DN		0xf1

#if (TELIK_SPP_SERVICE_ENABLE)
	#define	 GPIO_LED		0
	#define	 KB_MAP_NORMAL	{{CR_VOL_UP}, {CR_VOL_DN}}

	#define  KB_DRIVE_PINS  {GPIO_PD3}
	#define  KB_SCAN_PINS   {GPIO_PD4, GPIO_PD5}

	#define	PULL_WAKEUP_SRC_PD3		MATRIX_ROW_PULL

	#define	PULL_WAKEUP_SRC_PD4		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PD5		MATRIX_COL_PULL

	#define PD3_INPUT_ENABLE		1
	#define PD4_INPUT_ENABLE		1
	#define PD5_INPUT_ENABLE		1

	//#define	PC5_DATA_OUT			1

	#define		KB_MAP_NUM		KB_MAP_NORMAL
	#define		KB_MAP_FN		KB_MAP_NORMAL

#else

	#define	LONG_PRESS_KEY_POWER_OPTIMIZE		1
	//led output low
	#define	GPIO_LED							GPIO_PC2
	#define PC2_FUNC							AS_GPIO
	#define	PC2_OUTPUT_ENABLE					1
	#define	PC2_DATA_OUT						0

	#define		KB_MAP_NORMAL	{\
					{VK_NONE,		VK_3,	  	VK_1,		VK_NONE,	}, \
					{VK_2,	 		VK_5,	  	VK_NONE,	VK_4,	 	}, \
					{VK_NONE,	 	VK_NONE,	VK_NONE,	VK_NONE,	}, \
					{VK_NONE,	 	VK_NONE,	VK_NONE,	VK_NONE,	}, \
					{CR_VOL_UP,	 	VK_NONE,	VK_NONE,	CR_VOL_DN,	}, \
					{VK_NONE,		VK_NONE,	VK_NONE,	VK_NONE,	}, }

	#define  KB_DRIVE_PINS  {GPIO_PA1, GPIO_PA5, GPIO_PA6, GPIO_PA7}
	#define  KB_SCAN_PINS   {GPIO_PB7, GPIO_PC6, GPIO_PE5, GPIO_PE6, GPIO_PF0, GPIO_PE4}

	//drive pin need 100K pulldown
	#define	PULL_WAKEUP_SRC_PA1		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PA5		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PA6		MATRIX_ROW_PULL
	#define	PULL_WAKEUP_SRC_PA7		MATRIX_ROW_PULL
	//scan  pin need 10K pullup
	#define	PULL_WAKEUP_SRC_PB7		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PC6		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PE5		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PE6		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PF0		MATRIX_COL_PULL
	#define	PULL_WAKEUP_SRC_PE4		MATRIX_COL_PULL

	//drive pin open input to read gpio wakeup level
	#define PA1_INPUT_ENABLE		1
	#define PA5_INPUT_ENABLE		1
	#define PA6_INPUT_ENABLE		1
	#define PA7_INPUT_ENABLE		1

	//scan pin open input to read gpio level
	#define PB7_INPUT_ENABLE		1
	#define PC6_INPUT_ENABLE		1
	#define PE5_INPUT_ENABLE		1
	#define PE6_INPUT_ENABLE		1
	#define PF0_INPUT_ENABLE		1
	#define PE4_INPUT_ENABLE		1

	#define		KB_MAP_NUM		KB_MAP_NORMAL
	#define		KB_MAP_FN		KB_MAP_NORMAL

	#define SIHUI_DEBUG_GPIO					1
	#if(SIHUI_DEBUG_GPIO)
		//chn0: PB0
		//chn1:	PC3
		//chn2: PE7
		//chn3:	PF1
		#define PB0_INPUT_ENABLE					0
		#define PC3_INPUT_ENABLE					0
		#define PE7_INPUT_ENABLE					0
		#define PF1_INPUT_ENABLE					0
		#define PB0_OUTPUT_ENABLE					1
		#define PC3_OUTPUT_ENABLE					1
		#define PE7_OUTPUT_ENABLE					1
		#define PF1_OUTPUT_ENABLE					1

		#define DBG_CHN0_LOW		( *(unsigned char *)0x80058b &= (~0x01) )
		#define DBG_CHN0_HIGH		( *(unsigned char *)0x80058b |= 0x01 )
		#define DBG_CHN0_TOGGLE		( *(unsigned char *)0x80058b ^= 0x01 )
		#define DBG_CHN1_LOW		( *(unsigned char *)0x800593 &= (~0x08) )
		#define DBG_CHN1_HIGH		( *(unsigned char *)0x800593 |= 0x08 )
		#define DBG_CHN1_TOGGLE		( *(unsigned char *)0x800593 ^= 0x08 )
		#define DBG_CHN2_LOW		( *(unsigned char *)0x8005a3 &= (~0x80) )
		#define DBG_CHN2_HIGH		( *(unsigned char *)0x8005a3 |= 0x80 )
		#define DBG_CHN2_TOGGLE		( *(unsigned char *)0x8005a3 ^= 0x80 )
		#define DBG_CHN3_LOW		( *(unsigned char *)0x8005ab &= (~0x02) )
		#define DBG_CHN3_HIGH		( *(unsigned char *)0x8005ab |= 0x02 )
		#define DBG_CHN3_TOGGLE		( *(unsigned char *)0x8005ab ^= 0x02 )
	#endif  //end of SIHUI_DEBUG_GPIO

#endif


/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms



/////////////////// set default   ////////////////

#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
