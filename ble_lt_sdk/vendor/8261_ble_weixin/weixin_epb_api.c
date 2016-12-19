#include "../../proj_lib/weixin/weixin.h"

void	send_status_to_usb (u8 *p, int n);

#define TAG_BaseResponse_ErrCode												0x08
#define TAG_BaseResponse_ErrMsg													0x12

#define TAG_AuthRequest_BaseRequest												0x0a
#define TAG_AuthRequest_Md5DeviceTypeAndDeviceId								0x12
#define TAG_AuthRequest_ProtoVersion											0x18
#define TAG_AuthRequest_AuthProto												0x20
#define TAG_AuthRequest_AuthMethod												0x28
#define TAG_AuthRequest_AesSign													0x32
#define TAG_AuthRequest_MacAddress												0x3a
#define TAG_AuthRequest_TimeZone												0x52
#define TAG_AuthRequest_Language												0x5a
#define TAG_AuthRequest_DeviceName												0x62

#define TAG_AuthResponse_BaseResponse											0x0a
#define TAG_AuthResponse_AesSessionKey											0x12

#define TAG_InitRequest_BaseRequest												0x0a
#define TAG_InitRequest_RespFieldFilter											0x12
#define TAG_InitRequest_Challenge												0x1a

#define TAG_InitResponse_BaseResponse											0x0a
#define TAG_InitResponse_UserIdHigh												0x10
#define TAG_InitResponse_UserIdLow												0x18
#define TAG_InitResponse_ChalleangeAnswer										0x20
#define TAG_InitResponse_InitScence												0x28
#define TAG_InitResponse_AutoSyncMaxDurationSecond								0x30
#define TAG_InitResponse_UserNickName											0x5a
#define TAG_InitResponse_PlatformType											0x60
#define TAG_InitResponse_Model													0x6a
#define TAG_InitResponse_Os														0x72
#define TAG_InitResponse_Time													0x78
#define TAG_InitResponse_TimeZone												0x8001
#define TAG_InitResponse_TimeString												0x8a01

#define TAG_SendDataRequest_BaseRequest											0x0a
#define TAG_SendDataRequest_Data												0x12
#define TAG_SendDataRequest_Type												0x18

#define TAG_SendDataResponse_BaseResponse										0x0a
#define TAG_SendDataResponse_Data												0x12

#define TAG_RecvDataPush_BasePush												0x0a
#define TAG_RecvDataPush_Data													0x12
#define TAG_RecvDataPush_Type													0x18

#define TAG_SwitchViewPush_BasePush												0x0a
#define TAG_SwitchViewPush_SwitchViewOp											0x10
#define TAG_SwitchViewPush_ViewId												0x18

#define TAG_SwitchBackgroudPush_BasePush										0x0a
#define TAG_SwitchBackgroudPush_SwitchBackgroundOp								0x10

//////////////////////////////////////////////////////////////////////////////////////////////////////////
int wx_pack_auth_request(uint8_t *buf, int buf_len, uint8_t *p_md5, uint8_t *p_aesSign, uint8_t *p_mac)
{
	int ret, len;
	Epb epb;
	epb_pack_init(&epb, buf, buf_len);
	
	ret = epb_set_message(&epb, TAG_AuthRequest_BaseRequest, 0, 0);
	len = ret;

	if (ret < 0) return ret;

	if (p_md5) {
		ret = epb_set_bytes(&epb, TAG_AuthRequest_Md5DeviceTypeAndDeviceId, p_md5, 16);
		if (ret < 0) return ret;
		len += ret;
	}

	ret = epb_set_int32(&epb, TAG_AuthRequest_ProtoVersion, 0x010000);
	if (ret < 0) return ret;
	len += ret;

	ret = epb_set_int32(&epb, TAG_AuthRequest_AuthProto, 1);
	if (ret < 0) return ret;
	len += ret;

	ret = epb_set_enum(&epb, TAG_AuthRequest_AuthMethod, p_md5 ? EAM_md5 : EAM_macNoEncrypt);
	if (ret < 0) return ret;
	len += ret;

	if (p_aesSign) {
		ret = epb_set_bytes(&epb, TAG_AuthRequest_AesSign, p_aesSign, 16);
		if (ret < 0) return ret;
		len += ret;
	}

	if (!p_md5 && p_mac) {
		ret = epb_set_bytes(&epb, TAG_AuthRequest_MacAddress, p_mac, 6);
		if (ret < 0) return ret;
		len += ret;
	}

	return len;
}

int wx_pack_init_request(uint8_t *buf, int buf_len, uint8_t filter, uint8_t *p_challenge)
{
	int ret, len;
	Epb epb;
	epb_pack_init(&epb, buf, buf_len);
	
	ret = epb_set_message(&epb, TAG_AuthRequest_BaseRequest, 0, 0);
	len = ret;

	if (filter) {
		ret = epb_set_bytes(&epb, TAG_InitRequest_RespFieldFilter, &filter, 1);
		if (ret < 0) return ret;
		len += ret;
	}

	if (p_challenge) {
		ret = epb_set_bytes(&epb, TAG_InitRequest_Challenge, p_challenge, 4);
		if (ret < 0) return ret;
		len += ret;
	}

	return len;
}

u32		wx_dbg_sddd;
int wx_pack_send_data_request(uint8_t *buf, int buf_len, uint8_t *p_data, int data_len, int type)
{
	int ret, len;
	Epb epb;
	epb_pack_init(&epb, buf, buf_len);
	
	wx_dbg_sddd = 1;
	ret = epb_set_message(&epb, TAG_AuthRequest_BaseRequest, 0, 0);
	len = ret;

	ret = epb_set_bytes(&epb, TAG_SendDataRequest_Data, p_data, data_len);
	if (ret < 0) return ret;
	len += ret;
	wx_dbg_sddd = 2;

	ret = epb_set_enum(&epb, TAG_SendDataRequest_Type, type);
	if (ret < 0) return ret;
	len += ret;
	wx_dbg_sddd = 3;

	return len;
}


// return session key
uint8_t * wx_unpack_auth_response(const uint8_t *buf, int buf_len)
{
	const uint8_t *tmp;
	int tmp_len;

	Epb epb;
	epb_unpack_init(&epb, buf, buf_len);

	if (!epb_has_tag(&epb, TAG_AuthResponse_BaseResponse)) {
		return 0;
	}

	if (!epb_has_tag(&epb, TAG_AuthResponse_AesSessionKey)) {
		return 0;
	}

	tmp = epb_get_message(&epb, TAG_AuthResponse_BaseResponse, &tmp_len);
	epb_unpack_init(&epb, tmp, tmp_len);

	if (!epb_has_tag(&epb, TAG_BaseResponse_ErrCode)) {
		return 0;
	}

	epb_unpack_init(&epb, buf, buf_len);
	return (uint8_t *)epb_get_bytes(&epb, TAG_AuthResponse_AesSessionKey, &tmp_len);
}

unsigned int		wx_dbg_crc32;
int wx_unpack_init_response(const uint8_t *buf, int buf_len, uint32_t *p_user_id_high, uint32_t *p_user_id_low, uint32_t crc32, uint32_t *p_scene)
{
	const uint8_t *tmp;
	int tmp_len;

	Epb epb;
	epb_unpack_init(&epb, buf, buf_len);

	if (!epb_has_tag(&epb, TAG_InitResponse_BaseResponse)) {
		return 0;
	}

	if (!epb_has_tag(&epb, TAG_InitResponse_UserIdHigh)) {
		return 0;
	}

	if (!epb_has_tag(&epb, TAG_InitResponse_UserIdLow)) {
		return 0;
	}


	tmp = epb_get_message(&epb, TAG_AuthResponse_BaseResponse, &tmp_len);
	epb_unpack_init(&epb, tmp, tmp_len);

	if (!epb_has_tag(&epb, TAG_BaseResponse_ErrCode)) {
		return 0;
	}

	epb_unpack_init(&epb, buf, buf_len);

	*p_user_id_high = epb_get_uint32(&epb, TAG_InitResponse_UserIdHigh);
	*p_user_id_low = epb_get_uint32(&epb, TAG_InitResponse_UserIdLow);

	if (epb_has_tag(&epb, TAG_InitResponse_ChalleangeAnswer)) {
		if (crc32 != epb_get_uint32(&epb, TAG_InitResponse_ChalleangeAnswer))
			return 0;
		
	}
	//wx_dbg_crc32 = epb_get_uint32(&epb, TAG_InitResponse_ChalleangeAnswer);
	if (p_scene && epb_has_tag(&epb, TAG_InitResponse_InitScence)) {
		*p_scene = (EmInitScence)epb_get_enum(&epb, TAG_InitResponse_InitScence);
	}

	return 1;
}

// return data pointer
uint8_t * wx_unpack_send_data_response(const uint8_t *buf, int buf_len, int * p_data_len)
{
	const uint8_t *tmp;
	int tmp_len;

	Epb epb;
	epb_unpack_init(&epb, buf, buf_len);

	if (!epb_has_tag(&epb, TAG_SendDataResponse_BaseResponse)) {
		return 0;
	}

	tmp = epb_get_message(&epb, TAG_SendDataResponse_BaseResponse, &tmp_len);
	epb_unpack_init(&epb, tmp, tmp_len);

	if (!epb_has_tag(&epb, TAG_BaseResponse_ErrCode)) {
		return 0;
	}

	epb_unpack_init(&epb, buf, buf_len);
	return epb_get_bytes(&epb, TAG_SendDataResponse_Data, p_data_len);
}

uint8_t * wx_unpack_recv_data_push(const uint8_t *buf, int buf_len, int * p_data_len, int *p_data_type)
{
	Epb epb;
	epb_unpack_init(&epb, buf, buf_len);

	if (!epb_has_tag(&epb, TAG_RecvDataPush_BasePush)) {
		return 0;
	}
	if (!epb_has_tag(&epb, TAG_RecvDataPush_Data)) {
		return 0;
	}

	*p_data_type = -1;
	if (epb_has_tag(&epb, TAG_RecvDataPush_Type)) {
		*p_data_type = (int)epb_get_enum(&epb, TAG_RecvDataPush_Type);
	}
	return epb_get_bytes(&epb, TAG_RecvDataPush_Data, p_data_len);
}


int wx_unpack_switch_view_push(const uint8_t *buf, int buf_len, int * p_view_op, int *p_view_id)
{
	Epb epb;
	epb_unpack_init(&epb, buf, buf_len);

	if (!epb_has_tag(&epb, TAG_SwitchViewPush_BasePush)) {
		return 0;
	}
	if (!epb_has_tag(&epb, TAG_SwitchViewPush_SwitchViewOp)) {
		return 0;
	}
	if (!epb_has_tag(&epb, TAG_SwitchViewPush_ViewId)) {
		return 0;
	}

	*p_view_op = (int)epb_get_enum(&epb, TAG_SwitchViewPush_SwitchViewOp);
	*p_view_id = (int)epb_get_enum(&epb, TAG_SwitchViewPush_ViewId);
	return 1;
}

int wx_unpack_switch_backgroud_push(const uint8_t *buf, int buf_len, int * p_background_op)
{
	Epb epb;
	epb_unpack_init(&epb, buf, buf_len);

	if (!epb_has_tag(&epb, TAG_SwitchBackgroudPush_BasePush)) {
		return 0;
	}
	if (!epb_has_tag(&epb, TAG_SwitchBackgroudPush_SwitchBackgroundOp)) {
		return 0;
	}

	*p_background_op = (int)epb_get_enum(&epb, TAG_SwitchBackgroudPush_SwitchBackgroundOp);
	return 1;
}


unsigned short	wx_req_seq = 0;

int wx_pack_request(uint8_t *buf, int data_len, unsigned short nCmd, const unsigned char * key)
{
	int len = 8;
	if (key)
	{
		len += aes_cbc_pks7_enc((const unsigned char *)(buf + 8), buf + 8, data_len, key);
	}
	else
	{
		len += data_len;
	}
	wx_req_seq ++;
	if (!wx_req_seq)
	{
		wx_req_seq = 1;
	}
	buf[0] = 0xfe;		//magic number
	buf[1] = 0x01;		//version
	buf[2] = len >> 8;
	buf[3] = len;
	buf[4] = nCmd >> 8;
	buf[5] = nCmd;
	buf[6] = wx_req_seq >> 8;
	buf[7] = wx_req_seq;	

	return len;
}

void	wx_u32_buf_be (unsigned char *pd, unsigned int dat)
{
	pd[0] = dat >> 24;
	pd[1] = dat >> 16;
	pd[2] = dat >> 8;
	pd[3] = dat >> 0;
}


///////////////////// request: to WeChat //////////////////////////////////////////////
int wx_gen_auth_request(uint8_t *pd, uint8_t *device_id, uint8_t * p_md5, uint8_t *p_mac, uint8_t *key)
{
	static unsigned int auth_seq = 0;
	unsigned char tbuf[64];
	unsigned int r, crc, wxn; 
	int id_len = strlen ((char *)device_id);
	if (key)
	{
		memcpy (tbuf, device_id, id_len);
		r = 0x11223344;				//random number
		wx_u32_buf_be (tbuf + id_len, r);
		wx_u32_buf_be (tbuf + id_len + 4, auth_seq++);
		crc = weixin_crc32 (0, tbuf, id_len + 8);
		wx_u32_buf_be (tbuf + id_len + 8, crc);
		memcpy (tbuf, tbuf + id_len, 12);
		aes_cbc_pks7_enc (tbuf, tbuf, 12, key);
	}
//	wxn = wx_pack_auth_request(pd + 8, 224, p_md5, key ? tbuf : 0, p_mac);
	wxn = wx_pack_auth_request(pd + 8, 224, 0, key ? tbuf : 0, p_mac);
	wxn = wx_pack_request (pd, wxn, ECI_req_auth, 0);
	return wxn;
}

int wx_gen_init_request(uint8_t *pd, uint8_t filter, uint8_t *p_challenge, uint8_t *key)
{
	int wxn = wx_pack_init_request(pd + 8, 224, filter, p_challenge);
	wxn = wx_pack_request (pd, wxn, ECI_req_init, key);
	return wxn;
}

int wx_gen_data_request(uint8_t *pd, uint8_t *p_data, int data_len, int html, uint8_t *key)
{
	int wxn = wx_pack_send_data_request (pd + 8, 224, p_data, data_len, html ? 10001 : 0);
	wxn = wx_pack_request (pd, wxn, ECI_req_sendData, key);
	return wxn;
}



///////////////////////////   response: from WeChat //////////////////////////////////////////////
int wx_unpack_response(uint8_t *buf, int data_len, unsigned short *p_nCmd, unsigned short *p_seq, unsigned char * key)
{
	int len = 0;

	if (key)
	{
		int nl = buf[3];
		if (nl < 8)
		{
			return 0;
		}
		len = aes_cbc_pks7_dec(buf + 8, (const unsigned char *)(buf + 8), nl - 8, (const unsigned char *)key);
	}
	else
	{
		len += data_len;
	}
	*p_nCmd = buf[4]*256 + buf[5];
	*p_seq = buf[6]*256 + buf[7];
	if (*p_nCmd == ECI_resp_auth)
	{
	}
	return len;
}

int wx_process_auth_response(uint8_t *buf, int data_len, unsigned char * key)
{
	uint8_t sk[32];
	unsigned short nCmd, seq;
	int len = wx_unpack_response (buf, data_len - 8, &nCmd, &seq, 0);
	unsigned char *p = wx_unpack_auth_response (buf + 8, len);
	if (!key)
	{
		return 16;
	}
	if (p && (seq == wx_req_seq) && (nCmd == ECI_resp_auth))
	{
		if (!aes_cbc_pks7_dec (sk, p, 32, key))
		{
			return 0;
		}
		memcpy (key, sk, 16);
		return 16;
	}
	return 0;
}

int wx_process_init_response(uint8_t *buf, int data_len, uint8_t * key, uint32_t *p_user_id_high, uint32_t *p_user_id_low, uint32_t crc32, uint32_t *p_scene)
{
	unsigned short nCmd, seq;
	int len = wx_unpack_response (buf, data_len - 8, &nCmd, &seq, key);
	if (!len)
	{
		return 0;
	}

	if (seq != wx_req_seq)
		return 0;

	return wx_unpack_init_response (buf + 8, len, p_user_id_high, p_user_id_low, crc32, p_scene);
}


//uint8_t * wx_unpack_send_data_response(const uint8_t *buf, int buf_len, int * p_data_len);
uint8_t * wx_process_send_data_response(uint8_t *buf, int data_len, uint8_t * key, int * p_data_len)
{
	unsigned short nCmd, seq;
	int len = wx_unpack_response (buf, data_len - 8, &nCmd, &seq, key);
	if (!len)
	{
		return 0;
	}

	return wx_unpack_send_data_response (buf + 8, len, p_data_len);
}

uint8_t * wx_process_recv_data_push(uint8_t *buf, int data_len, uint8_t * key, int * p_data_len, int *p_data_type)
{
	unsigned short nCmd, seq;
	int len = wx_unpack_response (buf, data_len - 8, &nCmd, &seq, key);
	if (!len)
	{
		return 0;
	}

	if (seq != 0)
		return 0;

	return wx_unpack_recv_data_push (buf + 8, len, p_data_len, p_data_type);
}

int wx_process_switch_view_push(uint8_t *buf, int data_len, uint8_t * key, int * p_view_op, int *p_view_id)
{
	unsigned short nCmd, seq;
	int len = wx_unpack_response (buf, data_len - 8, &nCmd, &seq, key);

	if (!len)
	{
		return 0;
	}

	if (seq != 0)
		return 0;

	return wx_unpack_switch_view_push (buf + 8, len, p_view_op, p_view_id);
}

int wx_process_switch_backgroud_push(uint8_t *buf, int data_len, uint8_t * key, int * p_background_op)
{
	unsigned short nCmd, seq;
	int len = wx_unpack_response (buf, data_len - 8, &nCmd, &seq, key);
	wx_dbg_crc32 = len << 16;
	if (!len)
	{
		return 0;
	}
	wx_dbg_crc32++;
	if (seq != 0)
		return 0;
	wx_dbg_crc32++;
	return wx_unpack_switch_backgroud_push (buf + 8, len, p_background_op);
}

///////////////////////////////////////////////////////////////////////////////////
void wx_set_adv_mac (unsigned char *p_adv, unsigned char * p_mac, int len)
{
//	unsigned char *ps = p_adv + 8;
	unsigned char *ps = p_adv;
	int ns = 0;

	while (ns < len)
	{
		if (ps[ns+1] == 0xff)
		{
			if (ps[ns] > 8)
			{
				//memcpy (ps + ns + 4, p_mac, 6);
				for (int i=0; i<6; i++)
					ps[ns + 4 + i] = p_mac[5 - i];
			}
			return;
		}
		ns += ps[ns] + 1;
	}
}

void LogMsg (char *str, u8 *p, int len)
{
	send_status_to_usb ((u8 *)str, 0);
	if (len > 32)
	{
		len = 32;
	}
	if (len)
	{
		send_status_to_usb (p, len);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
unsigned char crc_in[] = "test_device_ios";
unsigned char cbc_key[] = "3141592653589793";
unsigned char cbc_in[128] = {   0x6c,0x65,0x6e,0x67,0x74,0x68,0x5f,0x6f, 0x66,0x5f,0x31,0x36,0x5f,0x62,0x5f,0x5f,
								0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38, 0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68 };

unsigned int mdout[16] = {0};

#define DEVICE_TYPE "gh_d53f87f298e5"

int test_weixin()
{
	u32 t;
	int out_len;
	mdout[0] = weixin_crc32(0, crc_in, sizeof (crc_in) - 1);		//0x02e312f3

	t = clock_time ();
	mdout[1] = t;
	extern void ts_md5 (char * type, char *id, unsigned char *out);
	ts_md5 (DEVICE_TYPE, "test_device", mdout+4);				// 0x26cdd942b8ee68b022cc53bba16c7039
	mdout[2] = clock_time () - t;

	out_len = aes_cbc_pks7_enc(cbc_in, mdout+8, 16, cbc_key);
	mdout[3] = clock_time () - mdout[1];
	aes_cbc_pks7_dec(cbc_in, mdout+8, out_len, cbc_key);

	return 0;
}
#endif
