
#pragma once


#define    VBAT_ALARM_THRES_MV         2000  ///2000 mV low battery alarm


#define    BATTERY_VOL_OK              0x00
#define    BATTERY_VOL_LOW             0x01


void battery_power_check(u16 alarm_vol_mv);
