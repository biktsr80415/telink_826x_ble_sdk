#include "../../proj/tl_common.h"
#if 1
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/ll_whitelist.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj/drivers/keyboard.h"
#include "../common/tl_audio.h"
#include "../common/blt_led.h"
#include "../common/rf_frame.h"
#include "../../proj_lib/ble/trace.h"
#include "../../proj/mcu/pwm.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/audio.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/blt_config.h"


#if (HCI_ACCESS==HCI_USE_UART)
#include "../../proj/drivers/uart.h"
#endif

//void att_req_write_cmd (u8 *p, u16 h, u8 *pd, int n)

#define				ATT_DB_UUID16_NUM		20
#define				ATT_DB_UUID128_NUM		8

MYFIFO_INIT(hci_tx_fifo, 72, 4);
////////////////////////////////////////////////////////////////////
u16	conn_handle;					//	handle of  connection
u8 	conn_char_handler[8][8] = {{0}};
u8	peer_type;
u8	peer_mac[12];

u8		dev_mac[12] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff,  0xc4,0xe1,0xe2,0x63, 0xe4,0xc7};
s8		dev_rssi_th = 0x7f;

u32		spp_st = 0;
u32		spp_num = 0;
u16		spp_conn = 0;
u16		spp_handle = 0;
u32		spp_err = 0;
u32		spp_err_seq = 0;

void app_send_spp_status ()
{
	u8 dat[16];
	memcpy (dat + 0, &spp_st, 4);
	memcpy (dat + 4, &spp_err, 4);
	blc_hci_send_data (0x7e2 | HCI_FLAG_EVENT_TLK_MODULE, dat, 8);
}

int app_hci_cmd_from_usb (void)
{
	u8 buff[72];
	extern int usb_bulk_out_get_data (u8 *p);
	int n = usb_bulk_out_get_data (buff);
	if (n && blc_master_handler)
	{
		if (buff[0] == 0xe0 && buff[1] == 0xff)		//set mac address filter
		{
			dev_rssi_th = buff[4];
			memcpy (dev_mac, buff + 5, 12);
		}
		else if (buff[0] == 0xe1 && buff[1] == 0xff)		//spp test data from host to slave: write_cmd
		{
			memcpy (&spp_conn, buff + 4, 2);
			spp_handle = conn_char_handler[spp_conn & 7][7];
			memcpy (&spp_num, buff + 6, 4);
			spp_err = 0;
			spp_err_seq = 0;
		}
		else if (buff[0] == 0xe2 && buff[1] == 0xff)		//spp test status: spp_num & spp_err
		{
			app_send_spp_status ();
		}
		else
		{
			blc_master_handler (buff, n);
		}
	}
	return 0;
}

int	app_device_mac_match (u8 *mac, u8 *mask)
{
	u8 m[6];
	for (int i=0; i<6; i++)
	{
		m[i] = mac[i] & mask[i];
	}
	return memcmp (m, mask + 6, 6) == 0;
}

////////////////////////////////////////////////////////////////////
//p_att_rsp
////////////////////////////////////////////////////////////////////
extern	void abuf_init ();
extern	void abuf_mic_add (u8 *p);
extern	void abuf_mic_dec (void);
extern	void abuf_dec_usb (void);
extern 	void usbkb_hid_report(kb_data_t *data);

u8		att_mic_rcvd = 0;
u32		tick_adpcm;
u8		buff_mic_adpcm[MIC_ADPCM_FRAME_SIZE];

u32		tick_iso_in;
int		mode_iso_in;
_attribute_ram_code_ void  usb_endpoints_irq_handler (void)
{
	u32 t = clock_time ();
	/////////////////////////////////////
	// ISO IN
	/////////////////////////////////////
	if (reg_usb_irq & BIT(7)) {
		mode_iso_in = 1;
		tick_iso_in = t;
		reg_usb_irq = BIT(7);	//clear interrupt flag of endpoint 7

		/////// get MIC input data ///////////////////////////////
		//usb_iso_in_1k_square ();
		//usb_iso_in_from_mic ();
		abuf_dec_usb ();
	}

}

void	att_mic (u16 conn, u8 *p)
{
	att_mic_rcvd = 1;
	memcpy (buff_mic_adpcm, p, MIC_ADPCM_FRAME_SIZE);
	abuf_mic_add ((u32 *)buff_mic_adpcm);
}

u32		spp_dbg[4];
void	att_spp_read (u16 conn, u8 *p, int n)
{
	static u32 spp_read = 0;
	u32 seq;
	memcpy (&seq, p, 4);
	if (spp_read != seq)
	{
		spp_err++;
		if (spp_err < 3)
		{
			spp_err_seq = seq;
			spp_dbg[spp_err] = (seq & 0xff) | ((spp_read & 0xff) << 8);
		}
		//spp_err |= BIT(16);
	}
	else
	{
		for (int i=4; i<n; i++)
		{
			if ((u8)(p[0] + i) != p[i])
			{
				spp_err++;
				if (spp_err < 3)
				{
					spp_err_seq = i << 16;
				}
				break;
			}
		}
	}
	spp_read = seq - 1;
	spp_st = seq;

	if ((spp_read & 0xff) == 0)
	{
		app_send_spp_status ();
	}

	if (spp_err >= 2)
	{
		REG_ADDR8 (0x60) = 0x80;
	}
}

//////////////////////// mouse handle ////////////////////////////////////////

#define KEY_MASK_PRESS		0x10
#define KEY_MASK_REPEAT		0x20
#define KEY_MASK_RELEASE	0x30
u8 release_key_pending;
u32 release_key_tick;

void    report_to_pc_tool(u8 len,u8 * keycode)
{
#if 1  //pc tool verison_1.9 or later
		static u8 last_len = 0;
		static u8 last_key = 0;
		static u32 last_key_tick = 0;

		u8 mask = 0;

		if(!(read_reg8(0x8004)&0xf0)){ //pc tool cleared 0x8004
			if(!len){  //release
				write_reg8(0x8004,KEY_MASK_RELEASE);
				write_reg8(0x8005,0);
			}
			else{//press or repeat
				if(last_len==len && last_key==keycode[0]){//repeat
					mask = KEY_MASK_REPEAT;
				}
				else{ //press
					mask = KEY_MASK_PRESS;
				}
				write_reg8(0x8004,mask | len);
				write_reg8(0x8005,keycode[0]);
			}
		}
		else{  //pc tool not clear t0x8004, drop the key
			if(!len){  //release can not drop
				release_key_pending = 1;
				release_key_tick = clock_time();
			}
		}

		last_len = len;
		last_key = keycode[0];
#else //old pc tool
		write_reg8(0x8004,len);
		write_reg8(0x8005,keycode[0]);
#endif
}

rf_packet_mouse_t	pkt_mouse = {
		sizeof (rf_packet_mouse_t) - 4,	// dma_len

		sizeof (rf_packet_mouse_t) - 5,	// rf_len
		RF_PROTO_BYTE,		// proto
		PKT_FLOW_DIR,		// flow
		FRAME_TYPE_MOUSE,					// type

//		U32_MAX,			// gid0

		0,					// rssi
		0,					// per
		0,					// seq_no
		1,					// number of frame
};

void	att_mouse (u16 conn, u8 *p)
{
	memcpy (pkt_mouse.data, p, 4);
	pkt_mouse.seq_no++;
    usbmouse_add_frame(&pkt_mouse);
}

kb_data_t		kb_dat_debug = {1, 0, 0x04};
u8		cr_map_key[16] = {
		VK_VOL_UP, 	VK_VOL_DN,	VK_W_MUTE,	0,
		VK_ENTER,	VK_UP,		VK_DOWN,	VK_LEFT,
		VK_RIGHT,	VK_HOME,	0,			VK_NEXT_TRK,
		VK_PREV_TRK,VK_STOP,	0,			0
};

void	att_keyboard_media (u16 conn, u8 *p)
{
	if (1)			//notify data
	{
		//send_packet_usb (p + 6, p[5]);
		#if VIN_TEST_MODE
		if(p[0]){
			set_key_simulator(p);
			}
		else{
			// if the audio key release ,and we will reset the audio test cnt
				extern u8  vin_sample_OK_cnt ;
				extern u8  vin_sample_Fail_cnt ;
				vin_sample_OK_cnt =0;
				vin_sample_Fail_cnt =0;
		}
		#endif
		u16 bitmap = p[0] + p[1] * 256;
		kb_dat_debug.cnt = 0;
		kb_dat_debug.keycode[0] = 0;
		for (int i=0; i<16; i++)
		{
			if (bitmap & BIT(i))
			{
				kb_dat_debug.cnt = 1;
				kb_dat_debug.keycode[0] = cr_map_key[i];// cr_map_key[i];
				break;
			}
		}
#if CUSTOMER_2_MODE
		switch(bitmap){
		case 0x20: kb_dat_debug.keycode[0] = VK_WEB ;break;
		case 0x80: kb_dat_debug.keycode[0] = VK_SLEEP ;break;
		}
#endif

	#if VIN_TEST_MODE
	#else
		if(read_reg8(0) == 0x5a){ //report to pc_tool  mode
			report_to_pc_tool(kb_dat_debug.cnt,kb_dat_debug.keycode);
		}
		else{
			usbkb_hid_report((kb_data_t *) &kb_dat_debug);
		}
	#endif
	}
}

int slave_send_unpair_req;
int slave_send_unpair_tick;
//////////////// keyboard ///////////////////////////////////////////////////
void	att_keyboard (u16 conn, u8 *p)
{
	if (1)
	{
		//send_packet_usb (p + 6, p[5]);
		if(p[1] == 0xff){  //for kt_remote use only:slave unpair cmd
			slave_send_unpair_req = 1;
			slave_send_unpair_tick = clock_time();
			st_ll_conn_master_t *pm = blm_ll_getConnection (conn);
			smp_param_flashRemove (TYPE_WHITELIST, pm->peer_adr_type, pm->peer_adr);
			return;
		}

		kb_dat_debug.cnt = 0;
		kb_dat_debug.keycode[0] = 0;
		if (p[2])  //keycode[0]
		{
			kb_dat_debug.cnt = 1;
			kb_dat_debug.keycode[0] = p[2];
		}

		if(read_reg8(0) == 0x5a){ //report to pc_tool  mode
			report_to_pc_tool(kb_dat_debug.cnt,kb_dat_debug.keycode);
		}
		else{
			usbkb_hid_report((kb_data_t *) &kb_dat_debug);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//	Initialization: MAC address, Adv Packet, Response Packet
//////////////////////////////////////////////////////////////////////////////
u8  tbl_mac [] = {0xe1, 0xe1, 0xe2, 0xe3, 0xe4, 0xc7};

/////////////////////////// led management /////////////////////
enum{
	LED_POWER_ON = 0,
	LED_AUDIO_ON,	//1
	LED_AUDIO_OFF,	//2
	LED_SHINE_SLOW, //3
	LED_SHINE_FAST, //4
	LED_SHINE_OTA, //5
};

const led_cfg_t led_cfg[] = {
	    {1000,    0,      1,      0x00,	 },    //power-on, 1s on
	    {100,	  0 ,	  0xff,	  0x02,  },    //audio on, long on
	    {0,	      100 ,   0xff,	  0x02,  },    //audio off, long off
	    {500,	  500 ,   3,	  0x04,	 },    //1Hz for 3 seconds
	    {250,	  250 ,   6,	  0x04,  },    //2Hz for 3 seconds
	    {125,	  125 ,   200,	  0x08,  },    //4Hz for 50 seconds
};

void app_led_en (int id, int en)
{
	id &= 7;
	en = !(LED_ON_LEVAL ^ en);
	if (id == 0)
	{
		gpio_write(GPIO_LED_RED, en);
	}
	else if (id == 1)
	{
		gpio_write(GPIO_LED_GREEN, en);
	}
	else if (id == 2)
	{
		gpio_write(GPIO_LED_BLUE, en);
	}
	else if (id == 3)
	{
		gpio_write(GPIO_LED_WHITE, en);
	}
}
void rf_customized_param_load(void)
{
	  //flash 0x77000 customize freq_offset adjust cap value, if not customized, default ana_81 is 0xd0
	 if( (*(unsigned char*) CUST_CAP_INFO_ADDR) != 0xff ){
		 //ana_81<4:0> is cap value(0x00 - 0x1f)
		 analog_write(0x81, (analog_read(0x81)&0xe0) | ((*(unsigned char*) CUST_CAP_INFO_ADDR)&0x1f) );
	 }

	 //flash 0x77040 customize TP0, flash 0x77041 customize TP1
	 if( ((*(unsigned char*) (CUST_TP_INFO_ADDR)) != 0xff) && ((*(unsigned char*) (CUST_TP_INFO_ADDR+1)) != 0xff) ){
		 rf_update_tp_value(*(unsigned char*) (CUST_TP_INFO_ADDR), *(unsigned char*) (CUST_TP_INFO_ADDR+1));
	 }

	 smp_param_loadflash ();
}

//////////////////////////////////////////
// Service Discovery
//////////////////////////////////////////
typedef void (*main_service_t) (void);

main_service_t		main_service = 0;

static const u8 my_MicUUID[16]		= TELINK_MIC_DATA;
static const u8 my_SpeakerUUID[16]	= TELINK_SPEAKER_DATA;
static const u8 my_OtaUUID[16]		= TELINK_SPP_DATA_OTA;
static const u8 my_SppS2CUUID[16]		= TELINK_SPP_DATA_SERVER2CLIENT;
static const u8 my_SppC2SUUID[16]		= TELINK_SPP_DATA_CLIENT2SERVER;

void app_service_discovery ()
{
	att_db_uuid16_t 	db16[ATT_DB_UUID16_NUM];
	att_db_uuid128_t 	db128[ATT_DB_UUID128_NUM];
	memset (db16, 0, ATT_DB_UUID16_NUM * sizeof (att_db_uuid16_t));
	memset (db128, 0, ATT_DB_UUID128_NUM * sizeof (att_db_uuid128_t));

	if (blm_att_discoveryService (conn_handle, db16, ATT_DB_UUID16_NUM, db128, ATT_DB_UUID128_NUM) == 0)	// service discovery OK
	{
		int h = conn_handle & 7;
		conn_char_handler[h][0] = blm_att_findHandleOfUuid128 (db128, my_MicUUID);			//MIC
		conn_char_handler[h][1] = blm_att_findHandleOfUuid128 (db128, my_SpeakerUUID);		//Speaker
		conn_char_handler[h][2] = blm_att_findHandleOfUuid128 (db128, my_OtaUUID);			//OTA


		conn_char_handler[h][3] = blm_att_findHandleOfUuid16 (db16, CHARACTERISTIC_UUID_HID_REPORT,
					HID_REPORT_ID_CONSUME_CONTROL_INPUT | (HID_REPORT_TYPE_INPUT<<8));		//consume report

		conn_char_handler[h][4] = blm_att_findHandleOfUuid16 (db16, CHARACTERISTIC_UUID_HID_REPORT,
					HID_REPORT_ID_KEYBOARD_INPUT | (HID_REPORT_TYPE_INPUT<<8));				//normal key report

		conn_char_handler[h][5] = blm_att_findHandleOfUuid16 (db16, CHARACTERISTIC_UUID_HID_REPORT,
					HID_REPORT_ID_MOUSE_INPUT | (HID_REPORT_TYPE_INPUT<<8));				//mouse report

		conn_char_handler[h][6] = blm_att_findHandleOfUuid128 (db128, my_SppS2CUUID);			//notify
		conn_char_handler[h][7] = blm_att_findHandleOfUuid128 (db128, my_SppC2SUUID);			//write_cmd
		//-----	save to whitelist table & flash -------------------------

		st_ll_conn_master_t *pm = blm_ll_getConnection (conn_handle);
		//memcpy (peer_mac + 6, pm->peer_adr, 6);
		if(ll_whiteList_search (pm->peer_adr_type, pm->peer_adr))
		{
			smp_param_save_t save_param;
			save_param.rsvd = conn_char_handler[h][0] ? 8 : 32;
			save_param.peer_addr_type = pm->peer_adr_type;
			memcpy (save_param.peer_addr, pm->peer_adr, 6 );
			memcpy (&save_param.peer_random, conn_char_handler[h], 8);
			smp_param_nv (&save_param, TYPE_WHITELIST);
			blm_ll_disconnect (conn_handle | BLM_CONN_HANDLE, HCI_ERR_REMOTE_USER_TERM_CONN);
		}

	}
}

void app_register_service (void *p)
{
	main_service = p;
}

int			dongle_pairing_enable = 0;


int app_l2cap_handler (u16 conn, u8 *raw_pkt)
{

#if 0					//test code to monitor Mic handle in service discovery
	static u32			app_mic_handle = 0;
	u16 h = blm_att_discoveryHandleOfUUID (p, my_MicUUID);
	if (h)
	{
		app_mic_handle = h | (conn << 16);
	}
#endif

	u8 *p = blm_l2cap_packet_pack (conn, raw_pkt);
	if (!p)
	{
		return 0;
	}

	l2cap_att_client_handler (conn, p);

	conn &= 7;
	att_notify_t	*pn = (att_notify_t *) p;

//-------	user process ------------------------------------------------
	int test_spp_read = conn_char_handler[conn][6] == pn->handle && spp_conn;
	if (!test_spp_read)
	{
		blc_hci_send_data (conn | BLM_CONN_HANDLE | HCI_FLAG_ACL_BT_STD, p, p[1]);	//can be removed, debug purpose
	}


	//---------------	consumer key --------------------------
	if (conn_char_handler[conn][3] == pn->handle)			//consume key
	{
		static u32 app_key;
		app_key++;
		att_keyboard_media (conn, pn->value);
	}
	//---------------	consumer key --------------------------
	else if (conn_char_handler[conn][4] == pn->handle)			//consume key
	{
		static u32 app_key;
		app_key++;
		att_keyboard (conn, pn->value);
	}

	//---------------	Mic -----------------------------------
	else if (conn_char_handler[conn][0] == pn->handle)		//Mic data
	{
		static u32 app_mic;
		app_mic	++;
		att_mic (conn, pn->value);
	}

	//---------------	OTA ----------------------------------
	else if (conn_char_handler[conn][2] == pn->handle)		//OTA DAT
	{
		static u32 app_ota;
		app_ota++;
		att_keyboard_media (conn, pn->value);
	}

	//---------------	SPP  ----------------------------------
	else if (conn_char_handler[conn][6] == pn->handle)		//SPP data
	{
		static u32 app_spp;
		app_spp++;
		att_spp_read (conn, pn->value, pn->l2capLen - 3);
	}


	return 0;
}

//////////////////////////////////////////////////////////
// event call back
//////////////////////////////////////////////////////////
void app_event_callback (u32 h, u8 *p, int n)
{
	static u32 event_cb_num;
	event_cb_num++;
	int send_to_hci = 1;

	if (h == (HCI_FLAG_EVENT_BT_STD | HCI_EVT_LE_META))		//LE event
	{
		u8 subcode = p[0];

	//------------ ADV packet --------------------------------------------
		if (subcode == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
		{
			event_adv_report_t *pa = (event_adv_report_t *)p;
			s8 rssi = pa->data[pa->len];
			const u8 tag_pairing[8] = {0x07,0x09,0x4b,0x54, 0x50,0x41,0x49,0x52};
			int mac_match = app_device_mac_match (pa->mac, dev_mac);
			if (ll_whiteList_search ((pa->type >> 6) & 1, pa->mac) == 0)
			{
				u8 interval = ll_whiteList_rsvd_field ((pa->type >> 6) & 1, pa->mac);
				if (interval != 8)
				{
					interval = 32;
				}
				blm_ll_connectDevice (0, pa->mac, interval, interval + 4, 0, 100);
			}
			else if (	(dongle_pairing_enable && (rssi > -56)) || 		//device is close enough
					(memcmp (pa->data, tag_pairing, 8) == 0) ||		//valid pairing flag
					((dev_rssi_th == 0x7f) && mac_match)
					)
			{
				//blm_ll_connectDevice (0, pa->mac, 30, 50, 0, 100);
				blm_ll_connectDevice (0, pa->mac, 8, 8 + 4, 0, 100);
			}
			else
			{
				send_to_hci = mac_match && (rssi >= dev_rssi_th) ;
			}
		}

	//------------ connection complete -------------------------------------
		else if (subcode == HCI_SUB_EVT_LE_CONNECTION_COMPLETE)	// connection complete
		{
			event_connection_complete_t *pc = (event_connection_complete_t *)p;
			if (!pc->status)							// status OK
			{
				static u32 peer_pairing;
				conn_handle = pc->handle;				// connection handle
				app_led_en (conn_handle, 1);
				if (ll_whiteList_search (pc->peer_adr_type, pc->mac))
				{
					peer_pairing++;
					peer_type = pc->peer_adr_type;
					memcpy (peer_mac, pc->mac, 6);
					app_register_service(&app_service_discovery);
				}
				else		//device paired
				{
					peer_type = 0xa5;
					smp_param_save_t param;
					smp_param_loadByAddr (pc->peer_adr_type, pc->mac, &param);
					// load UUID handle from flash
					memcpy (conn_char_handler[conn_handle & 3], param.peer_random, 8);
				}
			}
		}

	//------------ connection update complete -------------------------------
		else if (subcode == HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE)	// connection update
		{

		}
	}

	//------------ disconnect -------------------------------------
	else if (h == (HCI_FLAG_EVENT_BT_STD | HCI_CMD_DISCONNECTION_COMPLETE))		//disconnect
	{

		event_disconnection_t	*pd = (event_disconnection_t *)p;
		app_led_en (pd->handle, 0);
		//terminate reason
		if(pd->reason == HCI_ERR_CONN_TIMEOUT){

		}
		else if(pd->reason == HCI_ERR_REMOTE_USER_TERM_CONN){  //0x13

		}
		else if(pd->reason == SLAVE_TERMINATE_CONN_ACKED || pd->reason == SLAVE_TERMINATE_CONN_TIMEOUT){

		}
	}

	if (send_to_hci)
	{
		blc_hci_send_data (h, p, n);
	}
}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
int main_idle_loop ()
{
	static u32 tick_loop;
	tick_loop ++;

	// bcopy
	////////////////////////////////////// BLE entry /////////////////////////////////
	blm_main_loop ();

	///////////////////////////////////// proc usb cmd from host /////////////////////
	usb_handle_irq();

	////////////////////////////////////// proc audio ////////////////////////////////
	if (att_mic_rcvd)
	{
		tick_adpcm = clock_time ();
		att_mic_rcvd = 0;
	}
	if (clock_time_exceed (tick_adpcm, 200000))
	{
		tick_adpcm = clock_time ();
		abuf_init ();
	}
	abuf_mic_dec ();

	////////////////////////// proc pc_tool key release /////////////////////////////
	if(release_key_pending){
		if(!(read_reg8(0x8004)&0xf0)){ //pc tool cleared 0x8004
			write_reg8(0x8004,KEY_MASK_RELEASE);
			write_reg8(0x8005,0);
			release_key_pending = 0;
		}

		if(clock_time_exceed(release_key_tick,100000)){
			release_key_pending = 0;
		}
	}
	/////////////////////////////////////// HCI ///////////////////////////////////////
	extern int blc_hci_proc ();
	blc_hci_proc ();

	static u32 tick_bo;
	if (REG_ADDR8(0x125) & BIT(0))
	{
		tick_bo = clock_time ();
	}
	else if (clock_time_exceed (tick_bo, 200000))
	{
		REG_ADDR8(0x125) = BIT(0);

	}

	////////////////////////////////////// UI entry /////////////////////////////////
	device_led_process();

	dongle_pairing_enable = !gpio_read (SW1_GPIO);

	static u32 gpio2 = 0;
	u8 gpio =	!gpio_read (SW2_GPIO);
	if (gpio & !gpio2)
	{
		smp_param_reset ();
	}
	gpio2 = gpio;

	//////////////////////////////////// SPP test code ///////////////////////////////
	if (spp_num && spp_handle && blm_fifo_num (spp_conn))
	{
		u8		dat[20], tx[32];
		memcpy (dat, &spp_num, 4);
		for (int i=4; i<20; i++)
		{
			dat[i] = dat[0] + i;
		}
		att_req_write_cmd (tx, spp_handle, dat, 20);
		if (blm_push_fifo (spp_conn, tx))
		{
			spp_num--;

			if ((spp_num & 0xff) == 0)
			{
				app_send_spp_status ();
			}
		}
	}
	return 0;
}

void main_loop ()
{
	main_idle_loop ();

	if (main_service)
	{
		main_service ();
		main_service = 0;
	}
}

///////////////////////////////////////////
void user_init()
{
	rf_customized_param_load();  //load customized freq_offset cap value and tp value

	//set UAB ID
	usb_log_init ();
	REG_ADDR8(0x74) = 0x53;
	REG_ADDR16(0x7e) = 0x08d0;
	REG_ADDR8(0x74) = 0x00;

	//////////////// config USB ISO IN/OUT interrupt /////////////////
	reg_usb_mask = BIT(7);			//audio in interrupt enable
	reg_irq_mask |= FLD_IRQ_IRQ4_EN;
	reg_usb_ep6_buf_addr = 0x80;
	reg_usb_ep7_buf_addr = 0x60;
	reg_usb_ep_max_size = (256 >> 3);

	usb_dp_pullup_en (1);  //open USB enum

	/////////////////// keyboard drive/scan line configuration /////////

	blc_enable_hci_master_handler ();
	blm_register_event_data_callback (app_event_callback, app_l2cap_handler);
	att_register_idle_func (main_idle_loop);

#if (HCI_ACCESS==HCI_USE_USB)
	usb_bulk_drv_init (0);
	blc_register_hci_handler (app_hci_cmd_from_usb, blc_hci_tx_to_usb);
#else	//uart
	//one gpio should be configured to act as the wakeup pin if in power saving mode; pending
	//todo:uart init here
	uart_io_init(UART_GPIO_8267_PB2_PB3);
	CLK32M_UART115200;
	uart_BuffInit((u8 *)(&T_rxdata_buf), sizeof(T_rxdata_buf), (u8 *)(&T_txdata_buf));
	blc_register_hci_handler (blc_rx_from_uart, blc_hci_tx_to_uart);
#endif

	////////////////// BLE slave initialization ////////////////////////////////////
	u32 *pmac = (u32 *) CFG_ADR_MAC;
	if (*pmac != 0xffffffff)
	{
	    memcpy (tbl_mac, pmac, 6);
	}
    else
    {
        //TODO : should write mac to flash after pair OK
        tbl_mac[0] = (u8)rand();
        flash_write_page (CFG_ADR_MAC, 6, tbl_mac);
    }

	rf_set_power_level_index (RF_POWER_8dBm);

	ble_master_init (pmac);

	static u32 aasize;
	aasize = sizeof (st_ll_conn_master_t);

}

#endif
