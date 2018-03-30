
#pragma once


enum{
	GPIO_DIR_IN 	= 0,
	GPIO_DIR_OUT	= 1
};

// do not use enum,  because we use this in preprocessor diretive,  #if
#define AS_GPIO		0
#define AS_MSPI		1
#define AS_SWIRE	2
#define AS_UART		3
#define AS_I2C		4
#define AS_SPI		5
#define AS_I2S		6
#define AS_AMIC		7
#define AS_DMIC		8
#define AS_SDM		9
#define AS_USB		10
#define AS_ADC		11
#define AS_CMP		12

#define AS_PWM0		20
#define AS_PWM1		21
#define AS_PWM2		22
#define AS_PWM3		23
#define AS_PWM4		24
#define AS_PWM5		25
#define AS_PWM0_N	26
#define AS_PWM1_N	27
#define AS_PWM2_N	28
#define AS_PWM3_N	29
#define AS_PWM4_N	30
#define AS_PWM5_N	31


#include "../common/static_assert.h"

#if(__TL_LIB_8266__ || MCU_CORE_TYPE == MCU_CORE_8266)
#include "../mcu_spec/gpio_default_8266.h"
#include "../mcu_spec/gpio_8266.h"
#elif(__TL_LIB_MSSOC__ || (MCU_CORE_TYPE == MCU_CORE_MSSOC))
#include "../mcu_spec/gpio_default_mssoc.h"
#include "../mcu_spec/gpio_mssoc.h"
#endif

