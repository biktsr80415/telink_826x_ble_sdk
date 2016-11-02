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
#define STRING_PRODUCT			L"BLE SPP"
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
#define BLE_REMOTE_PM_ENABLE			0
#define BLE_SECURITY_ENABLE             0
#define TELIK_SPP_SERVICE_ENABLE		1



//////////// HW BOARD  //////////////////////////////
#define BLE_EVK                   0
#define BLE_FLYCO                 1//FLYCO project add

#define TELINK_SPP_MODULE         0//TELINK SPP TEST DEMO
#define FLYCO_SPP_MODULE          1//FLYCO SPP TEST DEMO

#if BLE_EVK
// LED
#define LED1                  GPIO_PA1 //First LED in Demo EVK
#define LED2                  GPIO_PA6 //Second LED in Demo EVK
#define LED3                  GPIO_PA5 //Third LED in Demo EVK
#define LED4                  GPIO_PB0 //Fourth LED in Demo EVK

// BUTTON
#define BUTTON1               GPIO_PD4
#define BUTTON2               GPIO_PD5

#define PA1_OUTPUT_ENABLE     1
#define PA6_OUTPUT_ENABLE     1
#define PA5_OUTPUT_ENABLE     1
#define PB0_OUTPUT_ENABLE     1

#define PD4_INPUT_ENABLE      1
#define PD5_INPUT_ENABLE      1

#define PULL_WAKEUP_SRC_PA1   PM_PIN_PULLDOWN_100K
#define PULL_WAKEUP_SRC_PA6   PM_PIN_PULLDOWN_100K
#define PULL_WAKEUP_SRC_PA5   PM_PIN_PULLDOWN_100K
#define PULL_WAKEUP_SRC_PB0   PM_PIN_PULLDOWN_100K

#define PULL_WAKEUP_SRC_PD4   PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PD5   PM_PIN_PULLUP_10K

#elif BLE_FLYCO    //FLYCO project add

//////////// flyco module  version number.  //////////////////////////
	/*    The version number of this module is?¨ºoV1.0.00.1
			BYTE0:0x00--0x09-Version number highest
			BYTE1:0x00--0x09-Version number center
			BYTE2:0x00--0x63-Version number lowest
			BYTE3:0x01--0x02-For the firmware upgrade,
		  the detection of the firmware version, select
		  the relevant bin file upgrade                     */
	//Update the firmware version, update the version number.
	#define FLYCO_VERSION  {0x01, 0x00, 0x00, 0x01}  //The current version number of this module:1.0.00.1

    //OTA
    #define FLYCO_OTA_ENABLE          1
//////////// flyco module  status pin setting.  //////////////////////////
	//#define BLE_STA_OUT             GPIO_PD4 //FLYCO BLE_STA_OUT
	//#define BLE_STA_OUT               GPIO_PA1 //Green EVK for test,Actually should use PD4 pin.
	#define BLE_STA_OUT               GPIO_PA6 //Blue EVK for test,Actually should use PD4 pin.
    //#define BLE_STA_OUT             GPIO_PC2 //Dongle for test,Actually should use PD4 pin.

/////////////////heartbeat for FLYCO app/////////////////////////////////
    #define HEARTBEAT_FLYCO           1 //close heart beat

/////////////////wakeup source pin /////////////////////////////////////
    #define BRTS_WAKEUP_MODULE        GPIO_PC4   //FLYCO WAKE UP Pin  if MCU have data to send to module via uart/i2c/spi, high: not allow sleep, low: allow sleep
	#define BRTS_WAKEUP_LEVEL         1 //Voltage level of pin BRTS_WAKEUP_MODULE while not allow sleep mode, it should be set as 1 for 8266 chip
	/*Wakeup source high level, set analog pull down;Wakeup souce low level, set analog pull up*/
	#if BRTS_WAKEUP_LEVEL
	#define PULL_WAKEUP_SRC_PC4       PM_PIN_PULLDOWN_100K

#endif
#endif

#if BLE_FLYCO
//ADD for FLYCO to set the defult adv param
#define YES                                       1
#define NO                                        0

#define DEV_NAME                                 "FLYCO FH7005/6/8 "

#define DEFLUT_DEV_NAME1                         'F', 'L', 'Y', 'C', 'O', ' ', 'F', 'H', '7', '0', '0', '5'
#define DEFLUT_DEV_NAME2                         '/', '6', '/', '8', 0x20, 0x00
#define DEFLUT_DEV_NAME1_LEN                     ( 12 )   //the length of DEFLUT_DEV_NAME1
#define DEFLUT_DEV_NAME2_LEN                     ( 6 )    //the length of DEFLUT_DEV_NAME2

#define DEFLUT_ADV_INTERVAL                      (0x30) //defalut adv interval 30ms
#define DEFLUT_RF_PWR_LEVEL                      ( 2 )    //defalut RF pwr level Power level: 2 (PHY_TX_POWER_0DB)The transmit power level is 1~8
#define DEFLUT_ADV_TIMEOUT                       ( 0 )    //defalut adv timeout:0s (enable ADV forever) //(30*1000) 30s
//Set the ble module is entered into the deep sleep mode or just close the broadcast after adv timeout occurs. When adv_timeout = 0, close adv timeout function, ble module will always adv!
#define ADV_TIMEOUT_ENTER_DEEP                    YES     //defalut YES:if adv timeout Not 0,once timeout ocur BLE MODULE enter DEEP sleep;if adv timeout is equal to 0,BLE MODULE adv forever!
//#define ADV_TIMEOUT_ENTER_DEEP                    NO    // if NO, once timeout occur BLE MODULE disable adv;if adv timeout is equal to 0,BLE MODULE adv forever!

/*********************** ble4.0 vol3,partC,section12.5***********
 * Apple product preferred Connection Parameters characteristic:
 * IntervalMax *(SlaveLatency +1) <= 2s
 * IntervalMin >= 20ms
 * IntervalMin + 20ms <= IntervalMax
 * Slave Latency <= 4
 * IntervalMax*(SlaveLatency +1)*3 < connSupervisionTimeout
 * connSupervisionTimeout <= 6s
*****************************************************************/
//connInterval unit:1.25ms  spec:connInterval range:7.5ms~4s
//connSupervisionTimeout unit:10ms
#if 0
//The connection interval is set as needed, the master device has the right to reject the connection parameter request from the module.
#define USER_EXPECTED_CONN_PARAM                      16, 32, 0, 600, /*20ms, 40ms, 0, 6s*/ \
                                                      32, 48, 0, 600, /*40ms, 60ms, 0, 6s*/ \
                                                      48, 80, 0, 600, /*60ms,100ms, 0, 6s*/ \
                                                      60, 80,14, 600, /*75ms,100ms,14, 6s*/ \
													 144,160, 4, 600, /*180ms,200ms,4, 6s*/ \
													 784,800, 0, 600  /*980ms,  1s, 0, 6s*/
#else
#define USER_EXPECTED_CONN_PARAM					  16, 24, 0, 600,	/*20ms, 30ms, 0, 6s	*/ \
													  16, 28, 0, 600,	/*20ms, 35ms, 0, 6s	*/ \
													  24, 28, 0, 600,	/*30ms, 35ms, 0, 6s	*/ \
													  16, 32, 0, 600,   /*20ms, 40ms, 0, 6s	*/ \
													  32, 40, 0, 600,	/*40ms, 50ms, 0, 6s */ \
													  60, 80,14, 600,   /*75ms,100ms,14, 6s	*/ \
													 144,160, 4, 600,   /*180ms,200ms,4, 6s	*/ \
		                                             784,800, 0, 600    /*980ms,  1s, 0, 6s */
#endif
#endif
/////////////////////// Debug printf /////////////////////////////////////////
#define PRINT_DEBUG_INFO              0
//Debug pin defination
#if PRINT_DEBUG_INFO
#define DEBUG_INFO_TX_PIN             GPIO_PD2   //EVK GPIO_GP8
//#define PD2_OUTPUT_ENABLE	          1          //mini_printf function contain this
#define PULL_WAKEUP_SRC_PD2           PM_PIN_PULLUP_1M
#endif

/////////////////////// POWER OPTIMIZATION  AT SUSPEND ///////////////////////
//notice that: all setting here aims at power optimization ,they depends on
//the actual hardware design.You should analyze your hardware board and then
//find out the io leakage

//shut down the input enable of some gpios, to lower io leakage at suspend state
//for example:  #define PA2_INPUT_ENABLE   0



/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000

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
