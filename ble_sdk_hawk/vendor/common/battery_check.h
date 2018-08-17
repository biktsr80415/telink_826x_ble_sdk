#ifndef BATTERY_CHECK_H_
#define BATTERY_CHECK_H_

#include "drivers.h"

#define BATTERY_CHECK_PIN    GPIO_PB3
#define ADC_CHANNEL_P        B3P
#define BATTERY_VOL_OK       0x00
#define BATTERY_VOL_LOW      0x01

#define BATTERY_VOL_MIN      ((unsigned short)2000)//Unit: mV

#define ADC_SAMPLE_NUM       8

extern u16 adcValue[ADC_SAMPLE_NUM];

//void TL_BatteryCheckInit(eADC_ChannelTypeDef ADC_Channel);
void TL_BatteryCheckInit(void);
void TL_BattteryCheckProc(void);

#endif /* BATTERY_CHECK_H_ */
