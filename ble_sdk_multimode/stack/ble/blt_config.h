#pragma once

//////////////////////////////////////////////////////////////////////////////
/**
 *  @brief  Definition for Device info
 */
#include "drivers.h"
#include "tl_common.h"

#define  MAX_DEV_NAME_LEN 				18

#ifndef DEV_NAME
#define DEV_NAME                        "tModule"
#endif




/////////////////// Flash  Address Config ////////////////////////////
	#ifndef		CFG_ADR_MAC
	#define		CFG_ADR_MAC						0x76000
	#endif

	#ifndef		CUST_CAP_INFO_ADDR
	#define		CUST_CAP_INFO_ADDR				0x77000
	#endif

	#ifndef		CUST_TP_INFO_ADDR
	#define		CUST_TP_INFO_ADDR				0x77040
	#endif



//master
#ifndef		CFG_ADR_PEER
#define		CFG_ADR_PEER					0x78000
#endif





typedef struct{
	u8 conn_mark;
	u8 ext_cap_en;
	u8 pad32k_en;
	u8 pm_enter_en;
}misc_para_t;

misc_para_t blt_miscParam;



static inline void blc_app_setExternalCrystalCapEnable(u8  en)
{
	blt_miscParam.ext_cap_en = en;
}



static inline void blc_app_loadCustomizedParameters(void)
{
	 if(!blt_miscParam.ext_cap_en)
	 {
		 //customize freq_offset adjust cap value, if not customized, default ana_81 is 0xd0
		 u8 cap_frqoft = *(unsigned char*) CUST_CAP_INFO_ADDR;
		 if( cap_frqoft != 0xff ){
			 analog_write(0x81, cap_frqoft );
		 }
	 }


	 // customize TP0/TP1
	 u16 tpLH = *(unsigned short*) CUST_TP_INFO_ADDR;
	 if( tpLH != 0xffff ){
		 rf_update_tp_value(tpLH&0xff, tpLH>>8);
	 }
}







///////////////////  Feature ////////////////////////////


#ifndef BLE_P256_PUBLIC_KEY_ENABLE
#define BLE_P256_PUBLIC_KEY_ENABLE								0
#endif







#ifndef BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE
#define BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE			0
#endif





//default ll_master_multi connection
#ifndef  LL_MASTER_SINGLE_CONNECTION
#define  LL_MASTER_SINGLE_CONNECTION					0
#endif

#ifndef  LL_MASTER_MULTI_CONNECTION
#define  LL_MASTER_MULTI_CONNECTION						0
#endif




#if (BLE_MODULE_LIB_ENABLE || BLE_MODULE_APPLICATION_ENABLE)  //for ble module
	#define		BLS_SEND_TLK_MODULE_EVENT_ENABLE				1
#endif






#ifndef		BLS_ADV_INTERVAL_CHECK_ENABLE
#define		BLS_ADV_INTERVAL_CHECK_ENABLE					0
#endif

#if LIB_TELINK_MESH_SCAN_MODE_ENABLE
#define		BLS_TELINK_MESH_SCAN_MODE_ENABLE				1
#endif

/////////////////  scan mode config  //////////////////////////
#ifndef		BLS_TELINK_MESH_SCAN_MODE_ENABLE
#define		BLS_TELINK_MESH_SCAN_MODE_ENABLE				0
#endif

#if(BLS_TELINK_MESH_SCAN_MODE_ENABLE)
	#define		BLS_BT_STD_SCAN_MODE_ENABLE					0
#else
	#ifndef		BLS_BT_STD_SCAN_MODE_ENABLE
	#define		BLS_BT_STD_SCAN_MODE_ENABLE					1
	#endif
#endif




#ifndef BLE_LL_ADV_IN_MAINLOOP_ENABLE
#define BLE_LL_ADV_IN_MAINLOOP_ENABLE					1
#endif



#define	BLS_BLE_RF_IRQ_TIMING_EXTREMELY_SHORT_EN		0



//conn param update/map update
#ifndef	BLS_PROC_MASTER_UPDATE_REQ_IN_IRQ_ENABLE
#define BLS_PROC_MASTER_UPDATE_REQ_IN_IRQ_ENABLE		0
#endif


#ifndef BLS_PROC_LONG_SUSPEND_ENABLE
#define BLS_PROC_LONG_SUSPEND_ENABLE					0
#endif






///////////////////////////////////////dbg channels///////////////////////////////////////////
#ifndef	DBG_CHN0_TOGGLE
#define DBG_CHN0_TOGGLE
#endif

#ifndef	DBG_CHN0_HIGH
#define DBG_CHN0_HIGH
#endif

#ifndef	DBG_CHN0_LOW
#define DBG_CHN0_LOW
#endif

#ifndef	DBG_CHN1_TOGGLE
#define DBG_CHN1_TOGGLE
#endif

#ifndef	DBG_CHN1_HIGH
#define DBG_CHN1_HIGH
#endif

#ifndef	DBG_CHN1_LOW
#define DBG_CHN1_LOW
#endif

#ifndef	DBG_CHN2_TOGGLE
#define DBG_CHN2_TOGGLE
#endif

#ifndef	DBG_CHN2_HIGH
#define DBG_CHN2_HIGH
#endif

#ifndef	DBG_CHN2_LOW
#define DBG_CHN2_LOW
#endif

#ifndef	DBG_CHN3_TOGGLE
#define DBG_CHN3_TOGGLE
#endif

#ifndef	DBG_CHN3_HIGH
#define DBG_CHN3_HIGH
#endif

#ifndef	DBG_CHN3_LOW
#define DBG_CHN3_LOW
#endif

#ifndef	DBG_CHN4_TOGGLE
#define DBG_CHN4_TOGGLE
#endif

#ifndef	DBG_CHN4_HIGH
#define DBG_CHN4_HIGH
#endif

#ifndef	DBG_CHN4_LOW
#define DBG_CHN4_LOW
#endif

#ifndef	DBG_CHN5_TOGGLE
#define DBG_CHN5_TOGGLE
#endif

#ifndef	DBG_CHN5_HIGH
#define DBG_CHN5_HIGH
#endif

#ifndef	DBG_CHN5_LOW
#define DBG_CHN5_LOW
#endif

#ifndef	DBG_CHN6_TOGGLE
#define DBG_CHN6_TOGGLE
#endif

#ifndef	DBG_CHN6_HIGH
#define DBG_CHN6_HIGH
#endif

#ifndef	DBG_CHN6_LOW
#define DBG_CHN6_LOW
#endif

#ifndef	DBG_CHN7_TOGGLE
#define DBG_CHN7_TOGGLE
#endif

#ifndef	DBG_CHN7_HIGH
#define DBG_CHN7_HIGH
#endif

#ifndef	DBG_CHN7_LOW
#define DBG_CHN7_LOW
#endif


