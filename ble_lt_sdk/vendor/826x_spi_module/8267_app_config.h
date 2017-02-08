#pragma once

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif
#if __PROJECT_826X_SPI_MODULE__


#define CHIP_TYPE				CHIP_TYPE_8267


/////////////////// MODULE /////////////////////////////////
#define PM_ENABLE				            0
#define TELIK_SPP_SERVICE_ENABLE			1
#define BLE_MODULE_APPLICATION_ENABLE		1

/////////////////////for test by tuyf 16-9-01//////////////////////////////////
#define RED_LED                 GPIO_PD5
#define GREEN_LED     			GPIO_PD6
#define WHITE_LED    		    GPIO_PD7
#define BLUE_LED                GPIO_PB4

#define ON            1
#define OFF           0

#define SSPI_TX_NOTIFY_PIN      GPIO_PD0 //GPIO_G2 for 8269 EVK
#define PD0_FUNC 				AS_GPIO
#define PD0_INPUT_ENABLE		0
#define PD0_DATA_OUT			0
#define PULL_WAKEUP_SRC_PD0		PM_PIN_PULLUP_1M

#if 1
//////////////////////////// MODULE PM GPIO	/////////////////////////////////
//-----------Wake up pin (mcu wakeup module)--------------------
#define SSPI_PM_WAKEUP_PIN                  GPIO_PC6//G0 for 8269 EVK
#define	PC6_FUNC							AS_GPIO
#define PC6_INPUT_ENABLE					1
#define	PC6_OUTPUT_ENABLE					0
#define	PC6_DATA_OUT						0
#define GPIO_WAKEUP_MODULE_HIGH				gpio_setup_up_down_resistor(GPIO_PC6, PM_PIN_PULLUP_10K);
#define GPIO_WAKEUP_MODULE_LOW				gpio_setup_up_down_resistor(GPIO_PC6, PM_PIN_PULLDOWN_100K);

//-----------Wake up pin (module wakeup mcu)--------------------
//#define GPIO_WAKEUP_MCU						GPIO_PB1   //module wakeup mcu
//#define	PB1_FUNC							AS_GPIO
//#define PB1_INPUT_ENABLE					1
//#define	PB1_OUTPUT_ENABLE					1
//#define	PB1_DATA_OUT						0
//#define GPIO_WAKEUP_MCU_HIGH				do{gpio_set_output_en(GPIO_PB1, 1); gpio_write(GPIO_PB1, 1);}while(0)
//#define GPIO_WAKEUP_MCU_LOW					do{gpio_set_output_en(GPIO_PB1, 1); gpio_write(GPIO_PB1, 0);}while(0)
//#define GPIO_WAKEUP_MCU_FLOAT				do{gpio_set_output_en(GPIO_PB1, 0); gpio_write(GPIO_PB1, 0);}while(0)

//suspend indicate status for Debug!!!
//Debug suspend
#define DEBUG_SUSPEND           0

#if DEBUG_SUSPEND
#define DEBUG_GPIO_OUT              gpio_write
#endif
#define	DEBUG_SUSPEND_PIN           GPIO_PD1  //GP3 for 8269 EVK
#define PD1_FUNC 				    AS_GPIO
#define PD1_OUTPUT_ENABLE		    1
#define PULL_WAKEUP_SRC_PD1		    PM_PIN_PULLDOWN_100K

#endif

#define SPI_MODULE_DATA_FINISH	gpio_setup_up_down_resistor(GPIO_PC4, PM_PIN_PULLUP_1M)//gpio_write(SSPI_TX_NOTIFY_PIN, 1);
#define SPI_MODULE_DATA_READY	gpio_setup_up_down_resistor(GPIO_PC4, PM_PIN_PULLDOWN_100K)//gpio_write(SSPI_TX_NOTIFY_PIN, 0)



////////////////////////////////////////////////////////





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

#endif
/////////////////// set default   ////////////////
#define	PULL_WAKEUP_SRC_PB1		PM_PIN_PULLDOWN_100K
#define	PULL_WAKEUP_SRC_PB2		PM_PIN_PULLDOWN_100K
#define	PULL_WAKEUP_SRC_PB3		PM_PIN_PULLDOWN_100K
#define	PULL_WAKEUP_SRC_PB6		PM_PIN_PULLDOWN_100K
#define PB1_INPUT_ENABLE		1
#define PB2_INPUT_ENABLE		1
#define PB3_INPUT_ENABLE		1
#define PB6_INPUT_ENABLE		1

#if 0  //debug GPIO
	#define	PD0_FUNC							AS_GPIO
	#define PD0_INPUT_ENABLE					0
	#define	PD0_OUTPUT_ENABLE					1
	#define DBG_CHN0_LOW						gpio_write(GPIO_PD0, 0)
	#define DBG_CHN0_HIGH						gpio_write(GPIO_PD0, 1)
	#define DBG_CHN0_TOGGLE						BM_FLIP(reg_gpio_out(GPIO_PD0), GPIO_PD0 & 0xff);


	#define	PD1_FUNC							AS_GPIO
	#define PD1_INPUT_ENABLE					0
	#define	PD1_OUTPUT_ENABLE					1
	#define DBG_CHN1_LOW						gpio_write(GPIO_PD1, 0)
	#define DBG_CHN1_HIGH						gpio_write(GPIO_PD1, 1)
	#define DBG_CHN1_TOGGLE						BM_FLIP(reg_gpio_out(GPIO_PD1), GPIO_PD1 & 0xff);


	#define	PC5_FUNC							AS_GPIO
	#define PC5_INPUT_ENABLE					0
	#define	PC5_OUTPUT_ENABLE					1
	#define DBG_CHN2_LOW						gpio_write(GPIO_PC5, 0)
	#define DBG_CHN2_HIGH						gpio_write(GPIO_PC5, 1)
	#define DBG_CHN2_TOGGLE						BM_FLIP(reg_gpio_out(GPIO_PC5), GPIO_PC5 & 0xff);


	#define	PC6_FUNC							AS_GPIO
	#define PC6_INPUT_ENABLE					0
	#define	PC6_OUTPUT_ENABLE					1
	#define DBG_CHN3_LOW						gpio_write(GPIO_PC6, 0)
	#define DBG_CHN3_HIGH						gpio_write(GPIO_PC6, 1)
	#define DBG_CHN3_TOGGLE						BM_FLIP(reg_gpio_out(GPIO_PC6), GPIO_PC6 & 0xff);
#endif


#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
