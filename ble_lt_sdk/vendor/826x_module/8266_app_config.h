#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif
#if __PROJECT_8266_MODULE__
#define _USER_CONFIG_DEFINED_	1	// must define this macro to make others known
#define	__LOG_RT_ENABLE__		0
//#define	__DEBUG_PRINT__			0
//////////// product  Information  //////////////////////////////
#define ID_VENDOR				0x248a			// for report
#define ID_PRODUCT_BASE			0x880C
#define STRING_VENDOR			L"Telink"
#define STRING_PRODUCT			L"BLE Module"
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
#define BLE_MODULE_PM_ENABLE				1
#define TELIK_SPP_SERVICE_ENABLE			1
#define BLE_MODULE_APPLICATION_ENABLE		1



//////////////////////////// MODULE PM GPIO	/////////////////////////////////
#define GPIO_WAKEUP_MODULE					GPIO_PC5   //mcu wakeup module
#define	PC5_FUNC							AS_GPIO
#define PC5_INPUT_ENABLE					1
#define	PC5_OUTPUT_ENABLE					0
#define	PC5_DATA_OUT						0
#define GPIO_WAKEUP_MODULE_HIGH				gpio_setup_up_down_resistor(GPIO_PC5, PM_PIN_PULLUP_10K);
#define GPIO_WAKEUP_MODULE_LOW				gpio_setup_up_down_resistor(GPIO_PC5, PM_PIN_PULLDOWN_100K);


#define GPIO_WAKEUP_MCU						GPIO_PC3   //module wakeup mcu
#define	PC3_FUNC							AS_GPIO
#define PC3_INPUT_ENABLE					1
#define	PC3_OUTPUT_ENABLE					1
#define	PC3_DATA_OUT						0
#define GPIO_WAKEUP_MCU_HIGH				do{gpio_set_output_en(GPIO_PC3, 1); gpio_write(GPIO_PC3, 1);}while(0)
#define GPIO_WAKEUP_MCU_LOW					do{gpio_set_output_en(GPIO_PC3, 1); gpio_write(GPIO_PC3, 0);}while(0)
#define GPIO_WAKEUP_MCU_FLOAT				do{gpio_set_output_en(GPIO_PC3, 0); gpio_write(GPIO_PC3, 0);}while(0)


//////////////////////////// KEYSCAN  GPIO //////////////////////////////////
#define	 GPIO_LED		0
#define	MATRIX_ROW_PULL					PM_PIN_PULLDOWN_100K
#define	MATRIX_COL_PULL					PM_PIN_PULLUP_10K


#define	PULL_WAKEUP_SRC_PD3		PM_PIN_PULLDOWN_100K

#define	PULL_WAKEUP_SRC_PD4		PM_PIN_PULLUP_10K
#define	PULL_WAKEUP_SRC_PD5		PM_PIN_PULLUP_10K

#define PD3_INPUT_ENABLE		1
#define PD4_INPUT_ENABLE		1
#define PD5_INPUT_ENABLE		1


/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms


#if 0
//debug chn  P2 : C4
#define PC4_FUNC							AS_GPIO
#define	PC4_OUTPUT_ENABLE					1
#define	PC4_DATA_OUT						0
#define DBG_CHN0_LOW		( *(unsigned char *)0x800593 &= (~0x10) )
#define DBG_CHN0_HIGH		( *(unsigned char *)0x800593 |= 0x10 )
#define DBG_CHN0_TOGGLE		( *(unsigned char *)0x800593 ^= 0x10 )
#endif


#endif
/////////////////// set default   ////////////////

#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
