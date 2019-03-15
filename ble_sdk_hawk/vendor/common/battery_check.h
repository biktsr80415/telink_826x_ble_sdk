#ifndef BATTERY_CHECK_H_
#define BATTERY_CHECK_H_

#include "drivers.h"

#define BATTERY_CHECK_PIN     GPIO_PB3
#define BATTERY_CHECK_ADC_CHN B3P

#define BATTERY_VOL_OK        0x00
#define BATTERY_VOL_LOW       0x01

#define BATTERY_VOL_MIN      ((u16)2000)//Unit: mV

#define ADC_SAMPLE_NUM       8


u8 TL_IsNeedInitAdcForBattCheck(void);
void TL_ClearBatteryCheckConfig(u8 clear);
void TL_BatteryCheckInit(void);
void TL_BattteryCheckProc(int minVol_mV);//

void battery_set_detect_enable (int en);
int  battery_get_detect_enable (void);

#endif /* BATTERY_CHECK_H_ */
