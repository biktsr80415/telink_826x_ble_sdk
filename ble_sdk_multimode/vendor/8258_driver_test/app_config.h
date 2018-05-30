#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif




/////////////////// TEST FEATURE SELECTION /////////////////////////////////

#define	TEST_HW_TIMER									1



#define	TEST_GPIO_IRQ									10



#define	TEST_UART										20


#define TEST_IIC										30


#define TEST_SPI										40


#define TEST_ADC										50


#define TEST_PWM										60


#define TEST_LOW_POWER									70



#define DRIVER_TEST_MODE								TEST_ADC





#if (DRIVER_TEST_MODE == TEST_ADC)
	#define BATT_CHECK_ENABLE    0
#endif


/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ  	16000000

enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),
};




/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms







#define DEBUG_GPIO_ENABLE							0

#if(DEBUG_GPIO_ENABLE)
	//define debug GPIO here according to your hardware
#if 0
	#define GPIO_CHN0							GPIO_PB4
	#define GPIO_CHN1							GPIO_PB5
	#define GPIO_CHN2							GPIO_PB6
	#define GPIO_CHN3							GPIO_PB7
	#define GPIO_CHN4							GPIO_PB0
	#define GPIO_CHN5							GPIO_PB1
	#define GPIO_CHN6							GPIO_PC2
	#define GPIO_CHN7							GPIO_PC3

	#define PB4_OUTPUT_ENABLE					1
	#define PB5_OUTPUT_ENABLE					1
	#define PB6_OUTPUT_ENABLE					1
	#define PB7_OUTPUT_ENABLE					1
	#define PB0_OUTPUT_ENABLE					1
	#define PB1_OUTPUT_ENABLE					1
	#define PC2_OUTPUT_ENABLE					1
	#define PC3_OUTPUT_ENABLE					1
#else
	#define GPIO_CHN0							GPIO_PD0
	#define GPIO_CHN1							GPIO_PD1
	#define GPIO_CHN2							GPIO_PD2
	#define GPIO_CHN3							GPIO_PD3
	#define GPIO_CHN4							GPIO_PD4
	#define GPIO_CHN5							GPIO_PD5
	#define GPIO_CHN6							GPIO_PD6
	#define GPIO_CHN7							GPIO_PD7

	#define PD0_OUTPUT_ENABLE					1
	#define PD1_OUTPUT_ENABLE					1
	#define PD2_OUTPUT_ENABLE					1
	#define PD3_OUTPUT_ENABLE					1
	#define PD4_OUTPUT_ENABLE					1
	#define PD5_OUTPUT_ENABLE					1
	#define PD6_OUTPUT_ENABLE					1
	#define PD7_OUTPUT_ENABLE					1
#endif




	#define DBG_CHN0_LOW		gpio_write(GPIO_CHN0, 0)
	#define DBG_CHN0_HIGH		gpio_write(GPIO_CHN0, 1)
	#define DBG_CHN0_TOGGLE		gpio_toggle(GPIO_CHN0)
	#define DBG_CHN1_LOW		gpio_write(GPIO_CHN1, 0)
	#define DBG_CHN1_HIGH		gpio_write(GPIO_CHN1, 1)
	#define DBG_CHN1_TOGGLE		gpio_toggle(GPIO_CHN1)
	#define DBG_CHN2_LOW		gpio_write(GPIO_CHN2, 0)
	#define DBG_CHN2_HIGH		gpio_write(GPIO_CHN2, 1)
	#define DBG_CHN2_TOGGLE		gpio_toggle(GPIO_CHN2)
	#define DBG_CHN3_LOW		gpio_write(GPIO_CHN3, 0)
	#define DBG_CHN3_HIGH		gpio_write(GPIO_CHN3, 1)
	#define DBG_CHN3_TOGGLE		gpio_toggle(GPIO_CHN3)
	#define DBG_CHN4_LOW		gpio_write(GPIO_CHN4, 0)
	#define DBG_CHN4_HIGH		gpio_write(GPIO_CHN4, 1)
	#define DBG_CHN4_TOGGLE		gpio_toggle(GPIO_CHN4)
	#define DBG_CHN5_LOW		gpio_write(GPIO_CHN5, 0)
	#define DBG_CHN5_HIGH		gpio_write(GPIO_CHN5, 1)
	#define DBG_CHN5_TOGGLE		gpio_toggle(GPIO_CHN5)
	#define DBG_CHN6_LOW		gpio_write(GPIO_CHN6, 0)
	#define DBG_CHN6_HIGH		gpio_write(GPIO_CHN6, 1)
	#define DBG_CHN6_TOGGLE		gpio_toggle(GPIO_CHN6)
	#define DBG_CHN7_LOW		gpio_write(GPIO_CHN7, 0)
	#define DBG_CHN7_HIGH		gpio_write(GPIO_CHN7, 1)
	#define DBG_CHN7_TOGGLE		gpio_toggle(GPIO_CHN7)
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
	#define DBG_CHN6_LOW
	#define DBG_CHN6_HIGH
	#define DBG_CHN6_TOGGLE
	#define DBG_CHN7_LOW
	#define DBG_CHN7_HIGH
	#define DBG_CHN7_TOGGLE
#endif  //end of DEBUG_GPIO_ENABLE





#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
