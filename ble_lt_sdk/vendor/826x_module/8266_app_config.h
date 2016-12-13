#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif
#if __PROJECT_8266_MODULE__


#define CHIP_TYPE				CHIP_TYPE_8266



/////////////////////HCI ACCESS OPTIONS///////////////////////////////////////
#define HCI_USE_UART	1
#define HCI_USE_USB		0
#define HCI_ACCESS		HCI_USE_UART//HCI_USE_USB


/////////////////// MODULE /////////////////////////////////
#define BLE_MODULE_PM_ENABLE				0
#define TELIK_SPP_SERVICE_ENABLE			1
#define BLE_MODULE_APPLICATION_ENABLE		1
#define BLE_MODULE_INDICATE_DATA_TO_MCU		1



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
