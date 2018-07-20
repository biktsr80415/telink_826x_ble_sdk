#ifndef BATTERY_CHECK_H_
#define BATTERY_CHECK_H_

#include "drivers.h"

#define BATTERY_CHECK_PIN    GPIO_PB3
#define BATTERY_VOL_OK       0x00
#define BATTERY_VOL_LOW      0x01

//void TL_BatteryCheckInit(eADC_ChannelTypeDef ADC_Channel);
void TL_BatteryCheckInit(void);
void TL_BattteryCheckProc(void);

#endif /* BATTERY_CHECK_H_ */
