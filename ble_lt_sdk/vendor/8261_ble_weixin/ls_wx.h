#ifndef LX_WEIXIN_H

#define LX_WEIXIN_H

#include "../../proj/tl_common.h"
//CMD
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


//scale data sent to server.
#define	ACCURACY_02LB_DIVISION				0x20
#define APPENDIX_PACK_AVAILABLE				0x10
#define WEIGHT_STATUS_STABLE				0x01
#define IMP_STATUS_MASK						0x0E

//&IMP_STATUS_MASK
#define IMP_STATUS_IDLE						0x00
#define	IMP_STATUS_PRO						0x02
#define	IMP_STATUS_SHOE						0x04
#define	IMP_STATUS_BAREFOOT					0x06
#define	IMP_STATUS_FINISH					0x08
#define	IMP_STATUS_ERR						0x0A

//0xC3 Status
#define STATUS_DATA_OK						0x01
#define STATUS_DATA_RESEND					0x00

typedef struct{
	u8 bFlag;
	u8 bData[3];					//big Endian.
}ls_float_t;

typedef struct{
	u8 bFlag;
	u8 bData;
}ls_short_float_t;
/////////////////////////////////////////////////////////
// when auth ok. sequencely send 0xCC , {0xC3, ...0xC3}
////////////////////////////////////////////////////////
//0xC0 : not need when weixin communication.
//Req . just one byte for cmd = 0xC0;

//Rsp .ls_device_info_t is used when server respond to 0xC0 cmd.
typedef struct{
	u8 	bCmd;						//0xC0
	u8	abDeviceType[15];
	u8  abDeviceId[12];
	u8	abManuData[6];
}ls_device_info_t;

//0xCC :first command sent when weixin.
//0xCC req .
/* example
 * u8	ab_op_cc_code[]		= {	0xCC,
						   	0x08,0x7C,0xBE,0x96,0x27,0xBF,
						   	0x01,0x01,0x02,0x0A,0x00,
						   	0x0A,0x00,0x00,0x00,
						   	0x0B,0x00,0x00,0x01,
						   	0x08
						   };
 */
typedef struct{
	u8 bCmd;						//0xCC.
	u8 abMac[6];
	u8 abModel[5];
	u8 abFWVer[4];
	u8 abHWVer[4];
	u8 bTimeZone;
}ls_req_download_info_t;

//0xCC rsp .
typedef struct{
	u8  bCmd;						//0xCC.
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

//0xC3 . 0xC3 is used to send data to weixin server.
//0xC3 req.
/*	example
 * u8 ab_op_c3_code[]		= { 0xC3,
							0xFE,0x00,0x14,0xC3,
							0x58,0x53,0x7E,0xD3,		//UTC must be earlier than the UTC got in 0xCC.
							0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,
							0x00,
							0x09
						  };
 */
typedef struct{
	u8			bCmd;				//0xC3
	ls_float_t	stWeight;
	u32			dwUTC;				//big Endian.
	ls_float_t	st5k_Imp;			//can be 0
	ls_float_t	st50k_Imp;			//can be 0
	u8			bUserId;			//0~254. 0 for all user.
	u8			bMeasureStatus;		//typically 0x09.
}ls_scale_data_req_t;

//0xC3 rsp.
typedef struct{
	u8			bCmd;				//0xC3
	u8			bStatus;			//STATUS_DATA_OK , STATUS_DATA_RESEND.
}ls_scale_data_rsp_t;

//for push data and cfm. not used.
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

u8 						g_ls_cur_cmd;

ls_req_download_info_t 	g_ls_req_download_info_data;
ls_rsp_download_info_t	g_ls_rsp_download_info_data;
ls_scale_data_req_t		g_ls_scale_data_req_message;
ls_scale_data_rsp_t		g_ls_scale_data_rsp_message;


int ls_set_cc_cmd(u8 *abBuff,u8 *abMac, u8 *abMode, u8 *abFwVer, u8 *abHwVer, u8 bTimeZone);
void ls_get_cc_rsp(u8 *abBuff);

int ls_set_c3_cmd(u8 *abBuff, u8 *stWeight, u32 dwUTC, u8 *st5k_Imp, u8 *st50k_Imp, u8 bUserId, u8 bMeasureStatus);
void ls_get_c3_rsp(u8 *abBuff);
#endif
