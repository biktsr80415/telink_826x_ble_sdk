#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif


/////////////////// TEST FEATURE SELECTION /////////////////////////////////

//ble link layer test
#define	TEST_ADVERTISING_ONLY							1
#define TEST_SCANNING_ONLY								2
#define TEST_ADVERTISING_IN_CONN_SLAVE_ROLE				3
#define TEST_SCANNING_IN_ADV_AND_CONN_SLAVE_ROLE        4
#define TEST_ADVERTISING_SCANNING_IN_CONN_SLAVE_ROLE	5


//power test
#define TEST_POWER_ADV									10



//smp test
#define TEST_SMP_PASSKEY_ENTRY							20



//other test
#define TEST_USER_BLT_SOFT_TIMER						30
#define TEST_WHITELIST									31
 //phy test
#define TEST_BLE_PHY									32



#define FEATURE_TEST_MODE								TEST_BLE_PHY



#if (   FEATURE_TEST_MODE == TEST_SCANNING_ONLY || FEATURE_TEST_MODE == TEST_SCANNING_IN_ADV_AND_CONN_SLAVE_ROLE \
	 || FEATURE_TEST_MODE == TEST_ADVERTISING_SCANNING_IN_SLAVE_ROLE || FEATURE_TEST_MODE==TEST_BLE_PHY \
	 || FEATURE_TEST_MODE == TEST_ADVERTISING_SCANNING_IN_CONN_SLAVE_ROLE)
	#define BLE_PM_ENABLE								0
#else
	#define BLE_PM_ENABLE								1
#endif



/////////////////////HCI ACCESS OPTIONS///////////////////////////////////////

#define 		PHYTEST_MODE_THROUGH_2_WIRE_UART		1   //Direct Test Mode through a 2-wire UART interface
#define 		PHYTEST_MODE_OVER_HCI_WITH_USB			2   //Direct Test Mode over HCI(UART hardware interface)
#define 		PHYTEST_MODE_OVER_HCI_WITH_UART			3   //Direct Test Mode over HCI(USB  hardware interface)

#if (FEATURE_TEST_MODE == TEST_BLE_PHY)
	#define BLE_PHYTEST_MODE					     PHYTEST_MODE_OVER_HCI_WITH_UART
#endif

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000





#include "../common/default_config.h"


/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

