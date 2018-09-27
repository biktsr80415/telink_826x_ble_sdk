/*
 * battery_check.h
 *
 *  Created on: 2018-8-3
 *      Author: Administrator
 */

#ifndef BATTERY_CHECK_H_
#define BATTERY_CHECK_H_

#define	LOW_BATT_FLG					0x5A

#define VBAT_ALRAM_THRES_MV				2000   // 2000 mV low battery alarm



void battery_set_detect_enable (int en);
int  battery_get_detect_enable (void);

void app_battery_power_check(u16 alram_vol_mv);


#endif /* APP_BATTDET_H_ */
