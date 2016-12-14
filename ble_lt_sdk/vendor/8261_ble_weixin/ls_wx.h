#ifndef LX_WEIXIN_H

#define LX_WEIXIN_H

#include "../../proj/tl_common.h"

#define SEND_REQ_DEVICE_INFO				0xC0
#define SEND_SCALE_DATA						0xC3
#define SEND_PB_SUCCESS						0xC8
#define	SEND_PUSH_CFM						0xCB
#define SEND_SCALE_INFO						0xCC

#define RSP_DEVICE_INFO						0xC0
#define RSP_UTC								0xC1
#define RSP_SCALE_DATA						0xC3
#define	RSP_PB_SUCCESS						0xC8
#define	RSP_PUSH_CFM						0xCB
#define RSP_USER_INFO						0xCC
#define SERVER_PUSH_USER_INFO				0xD1

typedef struct{
	u8	abDeviceType[15];			//ASCII
	u8  abDeviceId[12];				//ASCII
	u8	abManuData[6];				//0xXX
}ls_device_info_t;


////////////////////////////////////////////
//		scale data sent to server.		  //
#define	ACCURACY_02LB_DIVISION				0x20
#define APPENDIX_PACK_AVAILABLE				0x10
#define IMP_STATUS_MASK						0x0E
#define WEIGHT_STATUS_STABLE				0x01

//&IMP_STATUS_MASK
#define IMP_STATUS_IDLE						0x00
#define	IMP_STATUS_PRO						0x02
#define	IMP_STATUS_SHOE						0x04
#define	IMP_STATUS_BAREFOOT					0x06
#define	IMP_STATUS_FINISH					0x08
#define	IMP_STATUS_ERR						0x0A


typedef struct{
	u8 bFlag;
	u8 bData[3];					//big Endian.
}ls_float_t;

typedef struct{
	u8 bFlag;
	u8 bData;
}ls_short_float_t;

typedef struct{
	ls_float_t	stWeight;
	u32			dwUTC;				//big Endian.
	ls_float_t	st5k_Imp;
	ls_float_t	st50k_Imp;
	u8			bUserId;			//0~254
	u8			bMeasureStatus;
}ls_scale_data_t;

////////////////////////////////////////////
//		sent req to download cmd 		  //

typedef struct{
	u8 abMac[6];
	u8 abModel[5];					//ascii
	u8 abFWVer[4];					//ascii
	u8 abHWVer[4];					//ascii
	u8 bTimeZone;
}ls_req_download_info_t;

typedef struct{
	u32 dwUTC;						//big endian
	u8 	bUserID;
	u8  bSex;
	u8  bAge;
	u8  bUnit;
	ls_float_t stHeight;
	u8  bActivityLvl;
	ls_short_float_t stWaistLine;
	u8  xToDelData;
}ls_rsp_download_info_t;


typedef struct{
	u8 bUserId;
	u8 bSex;
	u8 bAge;
	u8 bUnit;
	ls_float_t	stHeight;
	u8 bActivityLvl;
	ls_short_float_t stWaistLine;
	u8 xToDelData;
}ls_push_data_t;


typedef struct{
	u8 bCfmCmd;
	ls_push_data_t stPushData;
}ls_user_cfm_push_data_t;




#endif
