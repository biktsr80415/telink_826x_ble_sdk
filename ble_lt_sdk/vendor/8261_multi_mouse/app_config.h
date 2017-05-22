#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif


#if (__PROJECT_8261_MULTI_MOUSE__)
	#define CHIP_TYPE				CHIP_TYPE_8261
#else
	#define CHIP_TYPE				CHIP_TYPE_8267
#endif

enum{
	RF_1M_BLE_MODE = 0,
	RF_2M_2P4G_MODE = 1,
};

#define UART_INIT_EN	0

/** Setup printf tool use the uart to show the
 * 	log information.
 *
 * 	UART_TX: Use one gpio to simulate the tx pin
 *
 * 	@param: BAUD RATE			//波特率
 * 	@param: BAUD_INTERVAL		//间隔
 *
**/

#define SIMULATE_UART_FUNC_EN	0

#if(SIMULATE_UART_FUNC_EN)

#define 	UART_BAUD_RATE_9600 	9600
#define 	UART_BAUD_RATE_19200	19200
#define 	UART_BAUD_RATE_38400	38400
#define 	UART_BAUD_RATE_57600	57600


#define	BAUD_RATE			UART_BAUD_RATE_19200
#define BAUD_BIT_INTERVAL	( CLOCK_SYS_CLOCK_HZ/BAUD_RATE )
#define UART_TX_PIN_SIM		GPIO_PB7
#define	PB7_FUNC			AS_GPIO


#define SHOW_FUNC_IN(msg, arg...) printf("> %s(%d): " msg "\n", __FUNCTION__,__LINE__, ##arg)
#define SHOW_FUNC_OUT(msg, arg...) printf("< %s(%d): " msg "\n\n", __FUNCTION__,__LINE__, ##arg)
#define SHOW_DBG(msg, arg...) printf("%s:%s(%d): " msg "\n", __FILE__, __FUNCTION__,__LINE__, ##arg)
#endif


/*************   CONFIG ADDRESS   ****************/

#define 		CFG_ADDR_MAC			0x1f000
#define			CFG_ADDR_ACCESS_CODE0	0x1f008
#define 		CUST_CAP_INFO_ADDR		0x1f040
#define 		CUST_TP_INFO_ADDR		0x1f080

#define			BLE_CON_MODE_SWITCH_CNT		300			//8ms * 375 = 3s
#define 		BLE_ADV_MODE_SWITCH_CNT		100			//30ms * 100 = 3s
#define			_2P4G_MODE_SWITCH_CNT		300			//3s

#define			MOUSE_OPTICAL_EN		1			//open mouse_sensor_pix.c
#define			TELINK_MOUSE_DEMO		1		// 1 : Telink multi mouse demo, 0: MIWU multi mouse
//Set up as 2.4G mouse

/*************   Mouse Module   ****************/
#define 		MOUSE_LED_MODULE_EN		1
#define 		MOUSE_BTN_MODULE_EN		1
#define 		MOUSE_WHEEL_MODULE_EN	1
#define 		MOUSE_SENSOR_MODULE_EN	1
#define 		MOUSE_BATT_MOUDULE_EN	0					//battery check
#define 		MOUSE_SLEEP_MODULE_EN	1
#define			MOUSE_SENSOR_MOTION		1

#define			MOUSE_BUTTON_DEBOUNCE	3

/*************   Mouse RF Configuration   ****************/
#define RF_PROTOCOL						RF_PROTO_PROPRIETARY		//  RF_PROTO_PROPRIETARY / RF_PROTO_RF4CE / RF_PROTO_ZIGBEE
#define MOUSE_PIPE1_DATA_WITH_DID		1

#define MOUSE_BUTTON_GPIO_REUSABILITY   0 //support gpio reusability


/*************   Mouse I/O Configuration  ***************/
#if (TELINK_MOUSE_DEMO)

	#define PA1_INPUT_ENABLE		1		//LEFT
	#define PE2_INPUT_ENABLE		1		//RIGHT
	#define PE3_INPUT_ENABLE		1		//MIDDLE

	#define PC4_INPUT_ENABLE		1		//FB
	#define PC5_INPUT_ENABLE		1		//BB
	#define PC2_INPUT_ENABLE		1		//MOTION
	#define PC3_INPUT_ENABLE		1		//CPI

	#define PE0_INPUT_ENABLE		1		//Wheel0 Input enable
	#define PE1_INPUT_ENABLE		1		//Wheel1 Input enable

	#define PB6_INPUT_ENABLE		1		//SDIO, MUST SET

	#define PULL_WAKEUP_SRC_PA1		GPIO_PULL_UP_1M
	#define PULL_WAKEUP_SRC_PE2		GPIO_PULL_UP_1M
	#define PULL_WAKEUP_SRC_PE3		GPIO_PULL_UP_1M

	//BUTTON FB BB CPI
	#define PULL_WAKEUP_SRC_PC4		GPIO_PULL_UP_10K
	#define PULL_WAKEUP_SRC_PC5		GPIO_PULL_UP_10K
	#define PULL_WAKEUP_SRC_PC3		GPIO_PULL_UP_10K	//PC2 - PC5使用1M上拉，进suspend后电压会掉下来

	#define PULL_WAKEUP_SRC_PB1		GPIO_PULL_UP_1M
	#define PULL_WAKEUP_SRC_PB6		GPIO_PULL_UP_1M
	#define PULL_WAKEUP_SRC_PC2		GPIO_PULL_UP_10K	//Motion Pin 1M　PULL UP

	#define PULL_WAKEUP_SRC_PE0		GPIO_PULL_UP_1M
	#define PULL_WAKEUP_SRC_PE1		GPIO_PULL_UP_1M

	#define PE2_FUNC	AS_GPIO
	#define PE3_FUNC	AS_GPIO

#if(UART_INIT_EN)
	#define PC2_FUNC	AS_UART
	#define PC3_FUNC	AS_UART

	#define PC3_INPUT_ENABLE		0
#else
	#define PC2_FUNC	AS_GPIO
	#define PC3_FUNC	AS_GPIO

#endif

#define	GPIO_LED				GPIO_PA0

#else

#define PA1_INPUT_ENABLE		1		//LEFT
#define PE2_INPUT_ENABLE		1		//RIGHT
#define PE3_INPUT_ENABLE		1		//MIDDLE
#define PA0_INPUT_ENABLE		1		//BOUND

#define PE0_INPUT_ENABLE		1		//Wheel1 Input enable
#define PE1_INPUT_ENABLE		1		//Wheel0 Input enable
#define PA7_INPUT_ENABLE		1		//MOTION

#define PULL_WAKEUP_SRC_PA1		GPIO_PULL_UP_1M
#define PULL_WAKEUP_SRC_PE2		GPIO_PULL_UP_1M
#define PULL_WAKEUP_SRC_PE3		GPIO_PULL_UP_1M
#define PULL_WAKEUP_SRC_PA0		GPIO_PULL_UP_1M

#define PULL_WAKEUP_SRC_PA7		GPIO_PULL_UP_1M

#define PULL_WAKEUP_SRC_PD3		GPIO_PULL_DN_100K	//wheel3
#define PULL_WAKEUP_SRC_PA4		GPIO_PULL_UP_1M		//SCLK
#define PULL_WAKEUP_SRC_PB5		GPIO_PULL_UP_1M		//MISO
#define PULL_WAKEUP_SRC_PB6		GPIO_PULL_UP_1M		//MOSI


#define GPIO_LED_R		GPIO_PC4
#define GPIO_LED_G		GPIO_PC5
#define GPIO_LED_B		GPIO_PD2

#endif




/////////////////// MODULE /////////////////////////////////
#define BLE_MOUSE_PM_ENABLE					1
#define BLE_REMOTE_SECURITY_ENABLE      	1

#if (__PROJECT_8267_BLE_REMOTE__)
	#define BLE_AUDIO_ENABLE				1
#else
	#define BLE_AUDIO_ENABLE				0
#endif

#define HID_MOUSE_ATT_ENABLE				1
/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_TYPE  		CLOCK_TYPE_PLL	//  one of the following:  CLOCK_TYPE_PLL, CLOCK_TYPE_OSC, CLOCK_TYPE_PAD, CLOCK_TYPE_ADC
#define CLOCK_SYS_CLOCK_HZ  	16000000

//////////////////Extern Crystal Type///////////////////////
#define CRYSTAL_TYPE			XTAL_12M		//  extern 12M crystal


/////////////////// watchdog  //////////////////////////////
#define MODULE_WATCHDOG_ENABLE		0
#define WATCHDOG_INIT_TIMEOUT		500  //ms


/////////////////// set default   ////////////////

#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
