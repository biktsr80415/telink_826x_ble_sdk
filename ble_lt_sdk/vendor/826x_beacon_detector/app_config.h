#pragma once
#include "../../proj/common/types.h"

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
#define STRING_PRODUCT			L"826x detector"
#define STRING_SERIAL			L"TLSR826X"

#if		(__PROJECT_BEACON_DETECTOR_8261__)
#define CHIP_TYPE				CHIP_TYPE_8261
#elif   (__PROJECT_BEACON_DETECTOR_8267__)
#define CHIP_TYPE				CHIP_TYPE_8267
#endif

#define APPLICATION_DONGLE		0					// or else APPLICATION_DEVICE
#define	FLOW_NO_OS				1

#define		RF_FAST_MODE_1M		1

/////////////////// UART /////////////////////////////////
#define UART_ENABLE         0


////////////////////PA////////////////////////////////////
#define PA_TXEN_PIN                         GPIO_PF0
#define PA_RXEN_PIN                         GPIO_PF1

////////////////////RSSI////////////////////////////////////

//#define GET_RSSI_MODE GET_RSSI_FROM_PACKET
//#define AGC_GAIN_MODE AGC_MODE_ENABLE


/////////////////// Clock  /////////////////////////////////
#define	USE_SYS_TICK_PER_US
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	32000000 

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE	0
#define WATCHDOG_INIT_TIMEOUT	500  //ms


/////////////open SWS digital pullup to prevent MCU err, this is must ////////////
//GPIO_SWS = GPIO_PB0,  when we enter suspend/deepsleep, we close this
//digital pullup to prevent io current leakage
#define PB0_DATA_OUT					1

extern void hw_timer1_start(u32 t_us,void *cb);
extern void hw_timer1_stop(void);
extern u8 timer_expired(void);

/////////////////// set default   ////////////////

#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
