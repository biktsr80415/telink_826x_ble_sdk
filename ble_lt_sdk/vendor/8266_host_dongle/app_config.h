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
#define STRING_PRODUCT			L"BLE Master"
#define STRING_SERIAL			L"TLSR826X"

#if		__PROJECT_8266_HOST_DONGLE__
#define CHIP_TYPE				CHIP_TYPE_8266		// 8866-24, 856M6-32
#else
#define CHIP_TYPE				CHIP_TYPE_8267		// 8866-24, 856M6-32
#endif

#define APPLICATION_DONGLE		0				// or else APPLICATION_DEVICE
#define	USB_PRINTER				1
#define	FLOW_NO_OS				1

#define		RF_FAST_MODE_1M		1
//////////////////////CAN NOT CHANGE CONFIG ABOVE ////////////////////////////

#define DEV_NAME                        "tModule"
#define MAX_DEV_NAME_LEN		18

#if(APPLICATION_DONGLE)
#define	USB_PRINTER_ENABLE 		USB_PRINTER	//
#define	USB_SPEAKER_ENABLE 		0
#define	USB_MIC_ENABLE 			1
#define	USB_MOUSE_ENABLE 		1
#define	USB_KEYBOARD_ENABLE 	1
#define	USB_SOMATIC_ENABLE      0   //  when USB_SOMATIC_ENABLE, USB_EDP_PRINTER_OUT disable
#define USB_CUSTOM_HID_REPORT	1
#endif

//////////////////// Audio /////////////////////////////////////
#define MIC_RESOLUTION_BIT		16
#define MIC_SAMPLE_RATE			16000
#define MIC_CHANNLE_COUNT		1
#define	MIC_ENOCDER_ENABLE		0

#define	MIC_ADPCM_FRAME_SIZE		128 //128
#define	MIC_SHORT_DEC_SIZE			248 //248

#define	TL_MIC_BUFFER_SIZE				512
//#define	TL_SDM_BUFFER_SIZE				1024


/////////////////////HCI ACCESS OPTIONS///////////////////////////////////////
#define HCI_USE_UART	1
#define HCI_USE_USB		0
#define HCI_ACCESS		HCI_USE_USB

#if (HCI_ACCESS==HCI_USE_UART)
#define UART_DATA_LEN    64      // data max 252
typedef struct{
    unsigned int len;        // data max 252
    unsigned char data[UART_DATA_LEN];
}uart_data_t;

uart_data_t T_txdata_user;
uart_data_t T_txdata_buf;      // not for user

uart_data_t T_rxdata_user;
uart_data_t T_rxdata_buf;   // data max 252, user must copy rxdata to other Ram,but not use directly
unsigned char uart_rx_true;
#endif
/////////////////// MODULE /////////////////////////////////
#define BLE_REMOTE_PM_ENABLE			0
#define BLE_REMOTE_SECURITY_ENABLE      1
#define BLE_IR_ENABLE					0
#define HID_MOUSE_ATT_ENABLE			0


////////////////////////// MIC BUFFER /////////////////////////////
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




//////////////////////////// KEYSCAN/MIC  GPIO //////////////////////////////////
#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K
#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K

#define	KB_LINE_HIGH_VALID				0   //dirve pin output 0 when keyscan, scanpin read 0 is valid
#define DEEPBACK_FAST_KEYSCAN_ENABLE	1   //proc fast scan when deepsleep back trigged by key press, in case key loss
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

#define			CR_VOL_UP		0xf0
#define			CR_VOL_DN		0xf1
#define			CR_VOL_MUTE		0xf2
#define			CR_POWER		0xf3
#define			CR_SEL			0xf4
#define			CR_UP			0xf5
#define			CR_DN			0xf6
#define			CR_LEFT			0xf7
#define			CR_RIGHT		0xf8
#define			CR_HOME			0xf9
#define			CR_REWIND		0xfa
#define			CR_NEXT			0xfb
#define			CR_PREV			0xfc
#define			CR_STOP			0xfd

#define		KB_MAP_NORMAL	{{CR_VOL_UP}, {CR_VOL_DN}}
#define		KB_MAP_NUM		KB_MAP_NORMAL
#define		KB_MAP_FN		KB_MAP_NORMAL

#if __PROJECT_8266_HOST_DONGLE__
//----------------------- GPIO for UI --------------------------------
//---------------  Button ----------------------------------
#define PD4_INPUT_ENABLE		1
#define PD5_INPUT_ENABLE		1
#define	SW2_GPIO				GPIO_PD5
#define	SW1_GPIO				GPIO_PD4
#define PULL_WAKEUP_SRC_PD5     PM_PIN_PULLUP_10K	//btn
#define PULL_WAKEUP_SRC_PD4     PM_PIN_PULLUP_10K  	//btn

// 8267
#define	 GPIO_LED_WHITE			GPIO_PA1
#define	 GPIO_LED_GREEN			GPIO_PA5
#define	 GPIO_LED_RED			GPIO_PA6
#define	 GPIO_LED_BLUE			GPIO_PB0
#define	 GPIO_LED_YELLOW		GPIO_PB0
#define  GPIO_LED				GPIO_PB0

#define  PA1_INPUT_ENABLE		0
#define	 PA1_OUTPUT_ENABLE		1
#define  PA5_INPUT_ENABLE		0
#define	 PA5_OUTPUT_ENABLE		1
#define  PA6_INPUT_ENABLE		0
#define  PA6_OUTPUT_ENABLE		1
#define  PB0_INPUT_ENABLE		0
#define	 PB0_OUTPUT_ENABLE		1

#else

#define PD4_INPUT_ENABLE		1
#define PD5_INPUT_ENABLE		1
#define	SW1_GPIO				GPIO_PD5
#define	SW2_GPIO				GPIO_PD4
#define PULL_WAKEUP_SRC_PD4		PM_PIN_PULLUP_1M	//btn
#define PULL_WAKEUP_SRC_PD5		PM_PIN_PULLUP_1M	//btn

#define	 GPIO_LED_GREEN			GPIO_PC0
#define	 GPIO_LED_RED			GPIO_PC4
#define	 GPIO_LED_BLUE			GPIO_PC2
#define	 GPIO_LED_WHITE			GPIO_PA1

#define  PC0_INPUT_ENABLE		0
#define	 PC0_OUTPUT_ENABLE		1
#define  PC4_INPUT_ENABLE		0
#define	 PC4_OUTPUT_ENABLE		1
#define  PC2_INPUT_ENABLE		0
#define  PC2_OUTPUT_ENABLE		1
#define  PA1_INPUT_ENABLE		0
#define	 PA1_OUTPUT_ENABLE		1

#endif

// PB5/PB6 dp/dm for 8266
//USB DM DP input enable
#define PB5_INPUT_ENABLE	1
#define PB6_INPUT_ENABLE	1
#define LED_ON_LEVAL 		1 		//gpio output high voltage to turn on led

//////////////////////////// MODULE PM GPIO	/////////////////////////////////
#define GPIO_WAKEUP_MODULE					GPIO_PC5   //mcu wakeup module
#define	PC5_FUNC							AS_GPIO
#define PC5_INPUT_ENABLE					1
#define	PC5_OUTPUT_ENABLE					0
#define	PC5_DATA_OUT						0
#define GPIO_WAKEUP_MODULE_HIGH				do{gpio_write(GPIO_PC5, 1);gpio_set_output_en(GPIO_PC5, 1);}while(0)
#define GPIO_WAKEUP_MODULE_RELEASE			do{gpio_set_output_en(GPIO_PC5, 0);gpio_write(GPIO_PC5, 0);}while(0)


#define GPIO_WAKEUP_MCU						GPIO_PC3   //module wakeup mcu
#define	PC3_FUNC							AS_GPIO
#define PC3_INPUT_ENABLE					1
#define	PC3_OUTPUT_ENABLE					1
#define	PC3_DATA_OUT						1
#define GPIO_WAKEUP_MCU_HIGH				do{gpio_write(GPIO_PC3, 1); gpio_set_output_en(GPIO_PC3, 1);}while(0)
#define GPIO_WAKEUP_MCU_LOW					do{gpio_write(GPIO_PC3, 0); gpio_set_output_en(GPIO_PC3, 1); }while(0)
#define GPIO_WAKEUP_MCU_FLOAT				do{gpio_set_output_en(GPIO_PC3, 0); gpio_write(GPIO_PC3, 0);}while(0)



/////////////////// Clock  /////////////////////////////////
#define	USE_SYS_TICK_PER_US
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	32000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms



/////////////open SWS digital pullup to prevent MCU err, this is must ////////////
#define PB0_DATA_OUT					1
#define PA0_DATA_OUT					1

/////////////////// set default   ////////////////

#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
