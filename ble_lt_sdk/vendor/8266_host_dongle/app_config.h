#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif

#if		__PROJECT_8266_HOST_DONGLE__
#define CHIP_TYPE				CHIP_TYPE_8266		// 8866-24, 856M6-32
#else
#define CHIP_TYPE				CHIP_TYPE_8267		// 8866-24, 856M6-32
#endif

#define APPLICATION_DONGLE		0				// or else APPLICATION_DEVICE
#define	USB_PRINTER				1
#define	FLOW_NO_OS				1

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
