#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif
#if __PROJECT_8266_FLYCO_MODULE__

#define CHIP_TYPE				CHIP_TYPE_8266		// 8866-24, 8566-32

/////////////////////HCI ACCESS OPTIONS///////////////////////////////////////
#define HCI_USE_UART	1
#define HCI_USE_USB		0
#define HCI_ACCESS		HCI_USE_UART//HCI_USE_USB

///////////////////////////  FLYCO project add  //////////////////////////////
#define BLE_FLYCO                                     1//FLYCO project add
#if BLE_FLYCO    //FLYCO project add
//////////// flyco module  version number.  //////////////////////////
	/*The version number of this module is V1.0.00.1
	BYTE0:0x00--0x09-Version number highest
	BYTE1:0x00--0x09-Version number center
	BYTE2:0x00--0x63-Version number lowest
	BYTE3:0x01--0x02-For the firmware upgrade*/
	//Update the firmware version, update the version number.
	#define FLYCO_VERSION                             {0x01, 0x00, 0x06, 0x01}  //The current version number of this module:1.0.06.1
//////////// flyco module  status pin setting.  //////////////////////
	#define BLE_STA_OUT               			       GPIO_PD4 //FLYCO BLE_STA_OUT
	//#define BLE_STA_OUT               	           GPIO_PA1 //Green EVK for test,Actually should use PD4 pin.
	//#define BLE_STA_OUT                              GPIO_PA6 //Blue EVK for test,Actually should use PD4 pin.
    //#define BLE_STA_OUT                              GPIO_PC2 //Dongle for test,Actually should use PD4 pin.
/////////////////////wakeup source pin ///////////////////////////////
    #define BRTS_WAKEUP_MODULE        				  GPIO_PC4   //FLYCO WAKE UP Pin  if MCU have data to send to module via uart/i2c/spi, high: not allow sleep, low: allow sleep
	#define BRTS_WAKEUP_LEVEL         				  1 //Voltage level of pin BRTS_WAKEUP_MODULE while not allow sleep mode, it should be set as 1 for 8266 chip
	/*Wakeup source high level, set analog pull down;Wakeup souce low level, set analog pull up*/
	#if BRTS_WAKEUP_LEVEL
	#define PULL_WAKEUP_SRC_PC4       			      PM_PIN_PULLDOWN_100K
	#define	PC4_FUNC				  			      AS_GPIO
	#define PC4_INPUT_ENABLE		  			      1
	#define	PC4_OUTPUT_ENABLE		  			      0
	#define	PC4_DATA_OUT			  			      0
    #endif
/////////////////////uart module open/close//////////////////////////
    #define UART_DISABLE      		  				  write_reg8(0x800061,0x08)  //reset uart
    #define UART_ENABLE       		  				  write_reg8(0x800061,0x08); \
                                                      write_reg8(0x800061,0x00)  //restart uart

///////////////////// flyco ble state High or low ///////////////////
    #define FLYCO_BLE_STATE_HIGH                      gpio_write (BLE_STA_OUT, 1) //BLE state HIGH
    #define FLYCO_BLE_STATE_LOW                       gpio_write (BLE_STA_OUT, 0) //BLE state LOW
/////////////////// heartbeat for FLYCO app//////////////////////////
    #define HEARTBEAT_FLYCO           				  0 //close heart beat
/////////////////// flyco OTA enable ////////////////////////////////
    #define FLYCO_OTA_ENABLE                          1
/////////////////// MODULE //////////////////////////////////////////
	#define BLE_PM_ENABLE				        	  1//suspend on adv enable/disable
	#define TELIK_SPP_SERVICE_ENABLE			  	  1//must be set 1
    #define FLYCO_SPP_MODULE                          1//FLYCO SPP project
    #define BLE_MODULE_APPLICATION_ENABLE		      1
/////////////////// ADD for FLYCO to set the defult adv param/////////
	#define YES                                       1
	#define NO                                        0
	#define DEV_NAME                                 "FLYCO FH7005/6/8 "
	#define DEFLUT_DEV_NAME1                         'F', 'L', 'Y', 'C', 'O', ' ', 'F', 'H', '7', '0', '0', '5'
	#define DEFLUT_DEV_NAME2                         '/', '6', '/', '8', 0x20, 0x00
	#define DEFLUT_DEV_NAME1_LEN                     ( 12 )   //the length of DEFLUT_DEV_NAME1
	#define DEFLUT_DEV_NAME2_LEN                     ( 6 )    //the length of DEFLUT_DEV_NAME2
	#define DEFLUT_ADV_INTERVAL                      (0x1E)   //defalut adv interval min 18.75ms
  //#define DEFLUT_ADV_INTERVAL                      (0x1F)   //defalut adv interval min 19.375ms
	#define DEFLUT_ADV_INTERVAL_MAX                  (0x46)   //defalut adv interval max 43.75ms
	#define DEFLUT_RF_PWR_LEVEL                      ( 2 )    //defalut RF pwr level Power level: 2 (phy tx power:0dBm)The transmit power level is 1~8
	#define DEFLUT_ADV_TIMEOUT                       ( 0 )    //defalut adv timeout:0s (enable ADV forever) //(30*1000*1000) 30s
/*********************** ble4.0 vol3,partC,section12.5***********
 * Apple product preferred Connection Parameters characteristic:
 * IntervalMax *(SlaveLatency +1) <= 2s
 * IntervalMin >= w
 * IntervalMin + 20ms <= IntervalMax
 * Slave Latency <= 4
 * IntervalMax*(SlaveLatency +1)*3 < connSupervisionTimeout
 * connSupervisionTimeout <= 6s
 ****************************************************************/
//connInterval unit:1.25ms  spec:connInterval range:7.5ms~4s
//connSupervisionTimeout unit:10ms
//////////////////// Debug for flyco /////////////////////////////////
#define DEBUG_FOR_TEST                                 0//Just for test for flyco project

/////////////////// PRINT DEBUG INFO ///////////////////////
/* 826x module's pin simulate as a uart tx, Just for debugging */
#define PRINT_DEBUG_INFO                    			0//open/close myprintf
#if PRINT_DEBUG_INFO
//defination debug printf pin
#define PRINT_BAUD_RATE             					1000000 //1M baud rate,should Not bigger than 1M, when system clock is 16M.
#define DEBUG_INFO_TX_PIN           					GPIO_PD3//G9 for 8266 EVK board(C1T53A20_V2.0)
//#define PD3_OUTPUT_ENABLE	        					1       //mini_printf function contain this
#define PULL_WAKEUP_SRC_PD3         					PM_PIN_PULLUP_1M
#endif
#endif


/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		1
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
