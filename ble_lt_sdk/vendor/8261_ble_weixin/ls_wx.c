#include "ls_wx.h"

////////////////////////////////////////////////////////
///////////////////		hex & ascii func 	////////////////
////////////////////////////////////////////////////////
char hex2ascii(u8 bhex)	//input data should be [0x00,0x0F].
{
	if( bhex < 10 )
		return (bhex + 0x30);		//return (bhex + '0');
	else
		return (bhex + 0x37);		//return (bhex - 0x0a + 'A');
//		return (bhex - 0x0a + 'a');
}

u8	ascii2hex( u8 bascii)	//input data should be [0,9]|[A,F]|[a,f]
{
#if 0
		if( bascii >= '0' && bascii <= '9')
			return ( bascii - '0' );
		else if( bascii >= 'A' && bascii <= 'F' )
			return ( 0x0a + bascii - 'A' );
		else
			return ( 0x0a + bascii - 'a' );
#else
		//simpler and can work correctly when input data is as requested.
		if( bascii <= '9' )
			return ( bascii - '0' );
		else if( bascii <= 'F' )
			return ( 0x0a + bascii - 'A' );
		else
			return ( 0x0a + bascii - 'a' );
#endif
}

void convert_hex_to_ascii(u8 *abbuff, u8 *abarray, int len)
{
	for(int i=0; i<len; i++)
	{
		abbuff[2*i] 	= hex2ascii((abarray[i]>>4)&0x0F);
		abbuff[2*i+1]	= hex2ascii((abarray[i])&0x0F);
	}
	return;
}

void convert_ascii_to_hex(u8 *abbuff, u8 *abarray, int len)
{
//	memset(abbuff,0xff,len/2);//not necessary.
	for( int i=0; i<2*len; i+=2 )
	{
		abbuff[i/2]  = ascii2hex(abarray[i])<<4;
		abbuff[i/2] |= ascii2hex(abarray[i+1]);
	}
	return;
}

int ls_set_cc_cmd(	u8 *abBuff,u8 *abMac, u8 *abMode, u8 *abFwVer, u8 *abHwVer, u8 bTimeZone)
{
	memset(&g_ls_req_download_info_data,0x00,sizeof(g_ls_req_download_info_data));
	g_ls_req_download_info_data.bCmd = 0xCC;
	g_ls_cur_cmd 					 = 0xCC;
	if( abMac )
		memcpy(g_ls_req_download_info_data.abMac,abMac,sizeof(g_ls_req_download_info_data.abMac));
	if( abMode )
		memcpy(g_ls_req_download_info_data.abModel,abMode,sizeof(g_ls_req_download_info_data.abModel));
	if( abFwVer )
		memcpy(g_ls_req_download_info_data.abFWVer,abFwVer,sizeof(g_ls_req_download_info_data.abFWVer));
	if( abHwVer )
		memcpy(g_ls_req_download_info_data.abHWVer,abHwVer,4);//sizeof(g_ls_req_download_info_data.abHWVer));

	g_ls_req_download_info_data.bTimeZone = bTimeZone;
	convert_hex_to_ascii(abBuff, (u8 *)&g_ls_req_download_info_data, sizeof(g_ls_req_download_info_data));
	return sizeof(g_ls_req_download_info_data)*2;
}

void ls_get_cc_rsp(u8 *abBuff)
{
	u8 tmpbuff[sizeof(ls_rsp_download_info_t)*2];
	convert_ascii_to_hex(tmpbuff,abBuff,sizeof(ls_rsp_download_info_t));
	memcpy(&g_ls_rsp_download_info_data,tmpbuff,sizeof(ls_rsp_download_info_t));
	return;
}

int ls_set_c3_cmd(u8 *abBuff, u8 *stWeight, u32 dwUTC, u8 *st5k_Imp, u8 *st50k_Imp, u8 bUserId, u8 bMeasureStatus)
{
	memset(&g_ls_scale_data_req_message,0,sizeof(g_ls_scale_data_req_message));
	g_ls_scale_data_req_message.bCmd = 0xC3;
	g_ls_cur_cmd					 = 0xC3;
	if( stWeight )
		memcpy(&(g_ls_scale_data_req_message.stWeight),stWeight,sizeof(ls_float_t));
	if( st5k_Imp )
		memcpy(&(g_ls_scale_data_req_message.st5k_Imp),st5k_Imp,sizeof(ls_float_t));
	if( st50k_Imp )
		memcpy(&(g_ls_scale_data_req_message.st50k_Imp),st5k_Imp,sizeof(ls_float_t));

	g_ls_scale_data_req_message.dwUTC = dwUTC;
	g_ls_scale_data_req_message.bUserId = bUserId;
	g_ls_scale_data_req_message.bMeasureStatus = bMeasureStatus;

	convert_hex_to_ascii(abBuff, (u8 *)&g_ls_scale_data_req_message, sizeof(g_ls_scale_data_req_message));

	return 2*sizeof(g_ls_scale_data_req_message);
}

void ls_get_c3_rsp(u8 *abBuff)
{
	u8 tmpbuff[sizeof(ls_scale_data_rsp_t)*2];
	if( abBuff )
	{
		convert_ascii_to_hex(tmpbuff,abBuff,sizeof(ls_scale_data_rsp_t));
		memcpy(&g_ls_scale_data_rsp_message, tmpbuff, sizeof(g_ls_scale_data_rsp_message));
	}
}

