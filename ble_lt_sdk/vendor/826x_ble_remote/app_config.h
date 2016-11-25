#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif


#if (__PROJECT_8267_BLE_REMOTE__)
	#define CHIP_TYPE				CHIP_TYPE_8267
#else
	#define CHIP_TYPE				CHIP_TYPE_8261
#endif




/////////////////// MODULE /////////////////////////////////
#define BLE_REMOTE_PM_ENABLE				1
#define BLE_REMOTE_SECURITY_ENABLE      	1

#if (__PROJECT_8267_BLE_REMOTE__)
	#define BLE_AUDIO_ENABLE				1
#else
	#define BLE_AUDIO_ENABLE				0
#endif



////////////////////////// AUDIO CONFIG /////////////////////////////
#if (BLE_AUDIO_ENABLE)
	#define BLE_DMIC_ENABLE					0  //0: Amic   1: Dmic
	#define	ADPCM_PACKET_LEN				128
	#define TL_MIC_ADPCM_UNIT_SIZE			248

	#if BLE_DMIC_ENABLE
		#define	TL_MIC_32K_FIR_16K			0
	#else
		#define	TL_MIC_32K_FIR_16K			1
	#endif


	#if TL_MIC_32K_FIR_16K
		#define	TL_MIC_BUFFER_SIZE				1984
	#else
		#define	TL_MIC_BUFFER_SIZE				992
	#endif
#endif




//////////////////////////// KEYSCAN/MIC  GPIO //////////////////////////////////
#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K
#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K

#define	KB_LINE_HIGH_VALID				0   //dirve pin output 0 when keyscan, scanpin read 0 is valid
#define DEEPBACK_FAST_KEYSCAN_ENABLE	0   //proc fast scan when deepsleep back trigged by key press, in case key loss
#define KEYSCAN_IRQ_TRIGGER_MODE		0
#define LONG_PRESS_KEY_POWER_OPTIMIZE	1   //lower power when pressing key without release

//stuck key
#define STUCK_KEY_PROCESS_ENABLE		0
#define STUCK_KEY_ENTERDEEP_TIME		60  //in s

//repeat key
#define KB_REPEAT_KEY_ENABLE			0
#define	KB_REPEAT_KEY_INTERVAL_MS		200
#define KB_REPEAT_KEY_NUM				1
#define KB_MAP_REPEAT					{VK_1, }


#define	PC4_INPUT_ENABLE				0	//amic digital input disable
#define	PC5_INPUT_ENABLE				0	//amic digital input disable

//CR: consumer report,  media key
#define			CR_VOL_UP		0xfd	//0x0001
#define			CR_VOL_DN		0xfe    //0x0002

#define			CR_VOL_MUTE		0xf1  	//0x0004, 1<<2
#define			CR_POWER		0xf2  	//0x0008, 2<<2
#define			CR_SEL			0xf3  	//0x000c, 3<<2
#define			CR_UP			0xf4  	//0x0010, 4<<2
#define			CR_DN			0xf5  	//0x0014, 5<<2
#define			CR_LEFT			0xf6  	//0x0018, 6<<2
#define			CR_RIGHT		0xf7  	//0x001c, 7<<2
#define			CR_HOME			0xf8  	//0x0020, 8<<2
#define			CR_REWIND		0xf9  	//0x0024, 9<<2
#define			CR_NEXT			0xfa  	//0x0028, a<<2
#define			CR_PREV			0xfb  	//0x002c, b<<2
#define			CR_STOP			0xfc  	//0x0030, c<<2


#define	GPIO_LED				GPIO_PA4
#define		KB_MAP_NORMAL	{\
				{CR_VOL_MUTE,	VK_3,	  	VK_1,		VK_MEDIA,	}, \
				{VK_2,	 		VK_5,	  	VK_M,		VK_4,	 	}, \
				{CR_RIGHT,	 	VK_NONE,	CR_SEL,		CR_LEFT,	}, \
				{CR_REWIND,	 	VK_NONE,	CR_DN,		CR_HOME,	}, \
				{CR_VOL_UP,	 	VK_NONE,	VK_MMODE,	CR_VOL_DN,	}, \
				{VK_WEB,		VK_NONE,	CR_UP,		CR_POWER,	}, }

#define  KB_DRIVE_PINS  {GPIO_PB1, GPIO_PB2, GPIO_PB3, GPIO_PB6}
#define  KB_SCAN_PINS   {GPIO_PD4, GPIO_PD5, GPIO_PD6, GPIO_PD7, GPIO_PE0, GPIO_PE1}

#define	PULL_WAKEUP_SRC_PD4		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD5		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD6		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD7		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PE0		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PE1		MATRIX_COL_PULL
#define PD4_INPUT_ENABLE		1
#define PD5_INPUT_ENABLE		1
#define PD6_INPUT_ENABLE		1
#define PD7_INPUT_ENABLE		1
#define PE0_INPUT_ENABLE		1
#define PE1_INPUT_ENABLE		1



#define	PULL_WAKEUP_SRC_PB1		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB2		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB3		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB6		MATRIX_ROW_PULL
#define PB1_INPUT_ENABLE		1
#define PB2_INPUT_ENABLE		1
#define PB3_INPUT_ENABLE		1
#define PB6_INPUT_ENABLE		1



#define		KB_MAP_NUM		KB_MAP_NORMAL
#define		KB_MAP_FN		KB_MAP_NORMAL




/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms



#define  SIHUI_DEBUG_BLE_SLAVE				0

#if(SIHUI_DEBUG_BLE_SLAVE)
#define	LOG_IN_RAM							0

#undef	__LOG_RT_ENABLE__
#define __LOG_RT_ENABLE__					0

#define PA2_INPUT_ENABLE					0
#define PA3_INPUT_ENABLE					0
#define PA5_INPUT_ENABLE					0
#define PC0_INPUT_ENABLE					0
#define PC1_INPUT_ENABLE					0
#define PC6_INPUT_ENABLE					0
#define PA2_OUTPUT_ENABLE					1
#define PA3_OUTPUT_ENABLE					1
#define PA5_OUTPUT_ENABLE					1
#define PC0_OUTPUT_ENABLE					1
#define PC1_OUTPUT_ENABLE					1
#define PC6_OUTPUT_ENABLE					1

#define DBG_CHN0_LOW		( *(unsigned char *)0x800583 &= (~0x04) )
#define DBG_CHN0_HIGH		( *(unsigned char *)0x800583 |= 0x04 )
#define DBG_CHN0_TOGGLE		( *(unsigned char *)0x800583 ^= 0x04 )
#define DBG_CHN1_LOW		( *(unsigned char *)0x800583 &= (~0x08) )
#define DBG_CHN1_HIGH		( *(unsigned char *)0x800583 |= 0x08 )
#define DBG_CHN1_TOGGLE		( *(unsigned char *)0x800583 ^= 0x08 )
#define DBG_CHN2_LOW		( *(unsigned char *)0x800583 &= (~0x20) )
#define DBG_CHN2_HIGH		( *(unsigned char *)0x800583 |= 0x20 )
#define DBG_CHN2_TOGGLE		( *(unsigned char *)0x800583 ^= 0x20 )
#define DBG_CHN3_LOW		( *(unsigned char *)0x800593 &= (~0x01) )
#define DBG_CHN3_HIGH		( *(unsigned char *)0x800593 |= 0x01 )
#define DBG_CHN3_TOGGLE		( *(unsigned char *)0x800593 ^= 0x01 )
#define DBG_CHN4_LOW		( *(unsigned char *)0x800593 &= (~0x02) )
#define DBG_CHN4_HIGH		( *(unsigned char *)0x800593 |= 0x02 )
#define DBG_CHN4_TOGGLE		( *(unsigned char *)0x800593 ^= 0x02 )
#define DBG_CHN5_LOW		( *(unsigned char *)0x800593 &= (~0x40) )
#define DBG_CHN5_HIGH		( *(unsigned char *)0x800593 |= 0x40 )
#define DBG_CHN5_TOGGLE		( *(unsigned char *)0x800593 ^= 0x40 )

#else
#define DBG_CHN0_LOW
#define DBG_CHN0_HIGH
#define DBG_CHN0_TOGGLE
#define DBG_CHN1_LOW
#define DBG_CHN1_HIGH
#define DBG_CHN1_TOGGLE
#define DBG_CHN2_LOW
#define DBG_CHN2_HIGH
#define DBG_CHN2_TOGGLE
#define DBG_CHN3_LOW
#define DBG_CHN3_HIGH
#define DBG_CHN3_TOGGLE
#define DBG_CHN4_LOW
#define DBG_CHN4_HIGH
#define DBG_CHN4_TOGGLE
#define DBG_CHN5_LOW
#define DBG_CHN5_HIGH
#define DBG_CHN5_TOGGLE

#endif  //end of SIHUI_DEBUG_GPIO



/////////////////// set default   ////////////////

#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
