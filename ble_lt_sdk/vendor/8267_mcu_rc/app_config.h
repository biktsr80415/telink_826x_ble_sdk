#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif


#define CHIP_TYPE				CHIP_TYPE_8267		// 8866-24, 8566-32


/////////////////// MODULE /////////////////////////////////
#define REMOTE_PM_ENABLE				1
#define BLE_REMOTE_SECURITY_ENABLE      1
#define BLE_IR_ENABLE					0
#define HID_MOUSE_ATT_ENABLE			0
#define BLE_AUDIO_ENABLE				1
#define	BLS_HCI_PROCESS_ENABLE			0


#define DEV_NAME                        "tRemote"


////////////////////////// AUDIO CONFIG /////////////////////////////
#if (BLE_AUDIO_ENABLE)
	#define BLE_DMIC_ENABLE					0  //0: Amic   1: Dmic
	#define	ADPCM_PACKET_LEN				128
	#define TL_MIC_ADPCM_UNIT_SIZE			248

	#define	TL_MIC_32K_FIR_16K				1

	#if TL_MIC_32K_FIR_16K
		#define	TL_MIC_BUFFER_SIZE				1984
	#else
		#define	TL_MIC_BUFFER_SIZE				992
	#endif

	#define GPIO_AMIC_BIAS					GPIO_PC6

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


#define GPIO_UTX						GPIO_PB2
#define GPIO_URX						GPIO_PB3


#define			GPIO_LED				GPIO_PC7

#define		 	VOICE					0xC0
#define			RED_KEY					0xC1



#define		KB_MAP_NORMAL	{\
				VK_7,		VK_4,			VK_1,		RED_KEY,			VK_VOL_DN,	VK_VOL_UP,	VK_ESC	, \
				VK_LEFT,	VK_NONE,		VK_NONE,	VK_FAST_BACKWARD,	VOICE,		VK_NONE,	VK_POWER, \
				VK_RIGHT,	VK_HOME,		VK_NONE,	VK_FAST_FORWARD,	VK_STOP,	VK_NONE,	VK_NONE , \
				VK_9,	 	VK_6,			VK_3,		VK_NONE,			VK_CH_DN,	VK_CH_UP,	VK_APP , \
				VK_NONE,	VK_W_MUTE,		VK_NONE,	VK_DOWN,			VK_ENTER,	VK_UP,		VK_NONE , \
				VK_NONE,	VK_NONE,		VK_0,		VK_8,				VK_5,		VK_2,		VK_NONE , \
				VK_W_STOP,	VK_PLAY_PAUSE,	VK_NONE,	VK_NONE,			VK_NONE,	VK_NONE,	VK_NONE, }


#define  KB_DRIVE_PINS  {GPIO_PA6, GPIO_PA7, GPIO_PB1, GPIO_PB4, GPIO_PB5, GPIO_PB6, GPIO_PB7}
#define  KB_SCAN_PINS   {GPIO_PD3, GPIO_PD4, GPIO_PD5, GPIO_PD6, GPIO_PD7, GPIO_PE0, GPIO_PE1}

#define	PA6_FUNC				AS_GPIO
#define	PA7_FUNC				AS_GPIO
#define	PB1_FUNC				AS_GPIO
#define	PB4_FUNC				AS_GPIO
#define	PB5_FUNC				AS_GPIO
#define	PB6_FUNC				AS_GPIO
#define	PB7_FUNC				AS_GPIO

#define	PULL_WAKEUP_SRC_PA6		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PA7		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB1		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB4		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB5		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB6		MATRIX_ROW_PULL
#define	PULL_WAKEUP_SRC_PB7		MATRIX_ROW_PULL

#define PA6_INPUT_ENABLE		1
#define PA7_INPUT_ENABLE		1
#define PB1_INPUT_ENABLE		1
#define PB4_INPUT_ENABLE		1
#define PB5_INPUT_ENABLE		1
#define PB6_INPUT_ENABLE		1
#define PB7_INPUT_ENABLE		1


#define	PD3_FUNC				AS_GPIO
#define	PD4_FUNC				AS_GPIO
#define	PD5_FUNC				AS_GPIO
#define	PD6_FUNC				AS_GPIO
#define	PD7_FUNC				AS_GPIO
#define	PE0_FUNC				AS_GPIO
#define	PE1_FUNC				AS_GPIO

#define	PULL_WAKEUP_SRC_PD3		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD4		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD5		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD6		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PD7		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PE0		MATRIX_COL_PULL
#define	PULL_WAKEUP_SRC_PE1		MATRIX_COL_PULL

#define PD3_INPUT_ENABLE		1
#define PD4_INPUT_ENABLE		1
#define PD5_INPUT_ENABLE		1
#define PD6_INPUT_ENABLE		1
#define PD7_INPUT_ENABLE		1
#define PE0_INPUT_ENABLE		1
#define PE1_INPUT_ENABLE		1


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



#define	GPIO_WAKEUP_MODULE					GPIO_PA2
#define	PA2_FUNC							AS_GPIO
#define PA2_INPUT_ENABLE					1
#define	PA2_OUTPUT_ENABLE					1
#define	PA2_DATA_OUT						0
#define GPIO_WAKEUP_MODULE_HIGH				do{gpio_set_output_en(GPIO_PA2, 1); gpio_write(GPIO_PA2, 1);}while(0)
#define GPIO_WAKEUP_MODULE_LOW				do{gpio_set_output_en(GPIO_PA2, 1); gpio_write(GPIO_PA2, 0);}while(0)
#define GPIO_WAKEUP_MODULE_FLOAT			do{gpio_set_output_en(GPIO_PA2, 0); gpio_write(GPIO_PA2, 0);}while(0)




#define	GPIO_WAKEUP_MCU						GPIO_PA3
#define	PA3_FUNC							AS_GPIO
#define PA3_INPUT_ENABLE					1
#define	PA3_OUTPUT_ENABLE					0
#define	PA3_DATA_OUT						0
#define PULL_WAKEUP_SRC_PA3					PM_PIN_PULLUP_10K
#define GPIO_WAKEUP_MCU_HIGH				gpio_setup_up_down_resistor(GPIO_PA3, PM_PIN_PULLUP_10K)
#define GPIO_WAKEUP_MCU_LOW					gpio_setup_up_down_resistor(GPIO_PA3, PM_PIN_PULLDOWN_100K)


#define	PA4_FUNC							AS_GPIO
#define PA4_INPUT_ENABLE					0
#define	PA4_OUTPUT_ENABLE					1
#define	PA4_DATA_OUT						0
#define DEBUG_GPIO_HIGH						gpio_write(GPIO_PA4, 1)
#define DEBUG_GPIO_LOW						gpio_write(GPIO_PA4, 0)
#define DEBUG_GPIO_TOGGLE					BM_FLIP(reg_gpio_out(GPIO_PA4), GPIO_PA4 & 0xff);



#define  SIHUI_GPIO_DEBUG					0
#if(SIHUI_GPIO_DEBUG)

#define PA2_FUNC				AS_GPIO //debug gpio chn0 : A2
#define PA3_FUNC				AS_GPIO //debug gpio chn1 : A3
#define PA4_FUNC				AS_GPIO //debug gpio chn2 : A4
#define PA5_FUNC				AS_GPIO //debug gpio chn3 : A5

#define PA2_OUTPUT_ENABLE					1
#define PA3_OUTPUT_ENABLE					1
#define PA4_OUTPUT_ENABLE					1
#define PA5_OUTPUT_ENABLE					1

#define DBG_CHN0_LOW		( *(unsigned char *)0x800583 &= (~0x04) )
#define DBG_CHN0_HIGH		( *(unsigned char *)0x800583 |= 0x04 )
#define DBG_CHN0_TOGGLE		( *(unsigned char *)0x800583 ^= 0x04 )
#define DBG_CHN1_LOW		( *(unsigned char *)0x800583 &= (~0x08) )
#define DBG_CHN1_HIGH		( *(unsigned char *)0x800583 |= 0x08 )
#define DBG_CHN1_TOGGLE		( *(unsigned char *)0x800583 ^= 0x08 )
#define DBG_CHN2_LOW		( *(unsigned char *)0x800583 &= (~0x10) )
#define DBG_CHN2_HIGH		( *(unsigned char *)0x800583 |= 0x10 )
#define DBG_CHN2_TOGGLE		( *(unsigned char *)0x800583 ^= 0x10 )
#define DBG_CHN3_LOW		( *(unsigned char *)0x800583 &= (~0x20) )
#define DBG_CHN3_HIGH		( *(unsigned char *)0x800583 |= 0x20 )
#define DBG_CHN3_TOGGLE		( *(unsigned char *)0x800583 ^= 0x20 )

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
