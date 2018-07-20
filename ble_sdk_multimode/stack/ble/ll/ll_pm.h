/*
 * ll_pm.h
 *
 *  Created on: 2017-3-7
 *      Author: Administrator
 */

#ifndef LL_PM_H_
#define LL_PM_H_



////////////////// Power Management ///////////////////////
#define			SUSPEND_DISABLE				0
#define			SUSPEND_ADV					BIT(0)
#define			SUSPEND_CONN				BIT(1)
#define			DEEPSLEEP_RETENTION_ADV		BIT(2)
#define			DEEPSLEEP_RETENTION_CONN	BIT(3)
#define			MCU_STALL					BIT(6)





typedef struct {
	u8		suspend_mask;
	u8		advMcuStall_en;
	u16		wakeup_src;


	u32 	appWakeup_tick;

	u8		appWakeup_en;
	u8		appWakeup_flg;
	u8		latency_off;
	u8		no_latency;

	u16		sys_latency;
	u16 	user_latency;
	u16		valid_latency;
	u16 	latency_use;

	u8		conn_no_suspend;
	u8 		timer_wakeup;
	u8      ble_busy_pending;


	u16 	latency_en;
	u8 		deepRt_en;
	u8		deepRet_type;

	u32     deepRet_advThresTick;
	u32     deepRet_connThresTick;
	u32     deepRet_earlyWakeupTick;

}st_ll_pm_t;

extern st_ll_pm_t  bltPm;


typedef 	void (*ll_module_pm_callback_t)(void);


typedef 	void (*pm_appWakeupLowPower_callback_t)(int);




/******************************* User Interface  ************************************/
void 		blc_ll_initPowerManagement_module(void);


void		bls_pm_setSuspendMask (u8 mask);
u8 			bls_pm_getSuspendMask (void);
void 		bls_pm_setWakeupSource(u16 source);
u32 		bls_pm_getSystemWakeupTick(void);

void 		bls_pm_setManualLatency(u16 latency); //manual set latency to save power
void 		bls_pm_enableAdvMcuStall(u8 en);

void 		bls_pm_setAppWakeupLowPower(u32 wakeup_tick, u8 enable);
void 		bls_pm_registerAppWakeupLowPowerCb(pm_appWakeupLowPower_callback_t cb);

void 		blc_pm_setDeepsleepRetentionThreshold(u32 adv_thres_ms, u32 conn_thres_ms);
void 		blc_pm_setDeepsleepRetentionEarlyWakeupTiming(u32 earlyWakeup_us);
void 		blc_pm_setDeepsleepRetentionType(SleepMode_TypeDef sleep_type);

/************************* Stack Interface, user can not use!!! ***************************/




#endif /* LL_PM_H_ */
