#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif


//////////// product  Information  //////////////////////////////
#define ID_VENDOR				0x248a			// for report
#define ID_PRODUCT_BASE			0x880C
#define STRING_VENDOR			L"Telink"
#define STRING_PRODUCT			L"USB Host"
#define STRING_SERIAL			L"TLSR8267"

#define CHIP_TYPE				CHIP_TYPE_8267		// 8866-24, 856M6-32
//////////////////////CAN NOT CHANGE CONFIG ABOVE ////////////////////////////

/////////////////////HCI ACCESS OPTIONS///////////////////////////////////////

/////////////////// MODULE /////////////////////////////////
#define BLE_REMOTE_PM_ENABLE			0
#define BLE_REMOTE_SECURITY_ENABLE      0
#define BLE_IR_ENABLE					0

// PB5/PB6 dp/dm for 8266
//USB DM DP input enable
#define PB5_INPUT_ENABLE	1
#define PB6_INPUT_ENABLE	1
#define LED_ON_LEVAL 		1 		//gpio output high voltage to turn on led


#define	PULL_WAKEUP_SRC_PD0		PM_PIN_PULLDOWN_100K
#define	PULL_WAKEUP_SRC_PD1		PM_PIN_PULLDOWN_100K


/////////////////// PRINT DEBUG INFO ///////////////////////
/* 826x module's pin simulate as a uart tx, Just for debugging */
#define PRINT_DEBUG_INFO                    1//open/close myprintf
#if PRINT_DEBUG_INFO
//defination debug printf pin
#define PRINT_BAUD_RATE             		1000000 //3M baud rate,should Not bigger than 1M, when system clock is 48M.
#define DEBUG_INFO_TX_PIN           		GPIO_PC6//G0 for 8261/8267 EVK board(C1T80A30_V1.0)
//#define PC6_OUTPUT_ENABLE	        		1       //mini_printf function contain this
#define PULL_WAKEUP_SRC_PC6         		PM_PIN_PULLUP_1M
#endif


/////////////////////// POWER OPTIMIZATION  AT SUSPEND ///////////////////////
//notice that: all setting here aims at power optimization ,they depends on
//the actual hardware design.You should analyze your hardware board and then
//find out the io leakage

//shut down the input enable of some gpios, to lower io leakage at suspend state
//for example:  #define PA2_INPUT_ENABLE   0



/////////////////// Clock  /////////////////////////////////
#define	USE_SYS_TICK_PER_US
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	48000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


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
