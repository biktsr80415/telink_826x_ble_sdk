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
#define STRING_PRODUCT			L"BLE mSPI"
#define STRING_SERIAL			L"TLSR8267"

#define CHIP_TYPE				CHIP_TYPE_8267		// 8866-24, 8566-32

#define spi_master_mode     1

///////////////////add by tuyf for test 8-31//////////////////////////////////
#define RED_LED       				GPIO_PD5
#define GREEN_LED     				GPIO_PD7
#define WHITE_LED     				GPIO_PD6
#define BLUE_LED      				GPIO_PB4
#define ON            				1
#define	OFF           				0
#define TEST_FW_ADR					0x40000
#define MSPI_RX_NOTIFY_PIN  		GPIO_PD4   //GPIO_GP6 for 8267 EVK

#define	PM_PIN_PULL_DEFAULT		    1
#define PULL_WAKEUP_SRC_PE2         PM_PIN_PULL_DEFAULT   //DM
#define PULL_WAKEUP_SRC_PE3         PM_PIN_PULL_DEFAULT   //DP

#define SLAVE_SPI_PM_ENABLE			0
//to Wake up 8269 sspi EVK
#if SLAVE_SPI_PM_ENABLE
	#define MSPI_PM_WAKEUP_PIN      GPIO_PB5   //GPIO_P4N for 8267 EVK
#endif

#define	DEBUG_SUSPEND_PIN           GPIO_PD0  //GP2 for 8267 EVK
#define PD0_FUNC 				    AS_GPIO
#define PD0_OUTPUT_ENABLE		    1
#define PULL_WAKEUP_SRC_PD0		    PM_PIN_PULLDOWN_100K


#define PRINT_DEBUG_INFO            0
//Debug pin defination
#if PRINT_DEBUG_INFO
	#define DEBUG_INFO_TX_PIN           GPIO_PD3   //G5 for 8267 EVK board
	//#define PD3_OUTPUT_ENABLE	        1          //mini_printf function contain this
	#define PULL_WAKEUP_SRC_PD3         PM_PIN_PULLUP_1M
#endif
/////////////////////////////////////////////////////////

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000

/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms



/////////////open SWS digital pullup to prevent MCU err, this is must ////////////
#define PB0_DATA_OUT					1

/////////////////// set default   ////////////////
#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
