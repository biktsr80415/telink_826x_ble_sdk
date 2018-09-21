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



#define DRIVER_TEST_MODE								TEST_LOW_POWER





#if (DRIVER_TEST_MODE == TEST_ADC)

	#define BATT_CHECK_ENABLE    0

#elif (DRIVER_TEST_MODE == TEST_IIC)
	/**************************************************************************************
	   8258 Demo Code Config
	   1. select i2c demo:  master or slave( app_config.h )
	   2. if i2c slave demo,  select slave working mode: mapping mode or dma mode
	   3. if i2c master demo, select the peer device i2c slave address mode:
					826x/8255 mapping mode/dma mode or other not telink's device

	 *************************************************************************************/
	#define  	I2C_DEMO_MASTER        	1   //i2c master demo
	#define     I2C_DEMO_SLAVE			2   //i2c master demo


	//select i2c demo here(master or slave)
	#define     I2C_DEMO_SELECT 		I2C_DEMO_MASTER



	#define 	DBG_DATA_LEN					16
	#define 	DBG_DATA_NUM					16


	#define 	SLAVE_DMA_MODE_ADDR_WRITE		0x4A000  //i2c master write data to  0x4A000
	#define 	SLAVE_DMA_MODE_ADDR_READ		0x4A100  //i2c master read data from 0x4A100

#elif (DRIVER_TEST_MODE == TEST_SPI)

	#define SPI_MASTER_MODE	1   //spi use master mode
	#define SPI_SLAVE_MODE	2   //spi use slave mode
	#define SPI_MODE		SPI_MASTER_MODE

	#define SLAVE_REGADDR1  0x04
	#define SLAVE_REGADDR2  0x80
	#define SLAVE_REGADDR3  0x00
	#define SPI_READ_CMD  	0x80// spi read command
	#define SPI_WRITE_CMD 	0x00// spi write command
	#define DBG_DATA_LEN    16

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
	#define GPIO_CHN0							GPIO_PC4
	#define GPIO_CHN1							GPIO_PC5
	#define GPIO_CHN2							GPIO_PC6
	#define GPIO_CHN3							GPIO_PC7

	#define PC4_OUTPUT_ENABLE					1
	#define PC5_OUTPUT_ENABLE					1
	#define PC6_OUTPUT_ENABLE					1
	#define PC7_OUTPUT_ENABLE					1


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
#endif  //end of DEBUG_GPIO_ENABLE





#include "../common/default_config.h"

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif
