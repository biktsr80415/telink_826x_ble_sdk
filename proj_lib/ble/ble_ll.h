
#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"

#include "ble_common.h"
#include "att.h"
#include "uuid.h"
#include "gap.h"
#include "hci/hci_const.h"
#include "hci/hci_event.h"

#define         VENDOR_ID                       0x0211
#define			BLUETOOTH_VER_4_0				6
#define			BLUETOOTH_VER_4_1				7
#define			BLUETOOTH_VER_4_2				8
#define			BLUETOOTH_VER					BLUETOOTH_VER_4_0
#define			BLUETOOTH_VER_SUBVER			0x4103

/////////////////////////////////////////////////////////////////////////////
#define				BLE_LL_BUFF_SIZE		64
#define				BLE_LL_BUFF_NUM	        8

/////////////////////////////////////////////////////////////////////////////
#define		MAC_MATCH(m)	(blt_p_mac[0]==m[0] && blt_p_mac[1]==m[1] && blt_p_mac[2]==m[2])


#define				MESH_COMMAND_FROM_PHONE					1
#define				MESH_COMMAND_FROM_DEVICE				2


// op cmd 11xxxxxxzzzzzzzzzzzzzzzz z's=VENDOR_ID  xxxxxx=LGT_CMD_
#define			LGT_CMD_LIGHT_ONOFF				0x10
#define			LGT_CMD_LIGHT_ON				0x10
#define			LGT_CMD_LIGHT_OFF				0x11//internal use
#define			LGT_CMD_LIGHT_SET				0x12//set lumen
#define			LGT_CMD_SWITCH_CONFIG	        0x13//internal use
#define         LGT_CMD_LIGHT_GRP_RSP1          0x14//get group rsp: 8groups low 1bytes
#define         LGT_CMD_LIGHT_GRP_RSP2          0x15//get group rsp: front 4groups 2bytes
#define         LGT_CMD_LIGHT_GRP_RSP3          0x16//get group rsp: behind 4groups 2bytes
#define 		LGT_CMD_LIGHT_CONFIG_GRP 		0x17//add or del group
#define			LGT_CMD_LUM_UP					0x18//internal use
#define			LGT_CMD_LUM_DOWN				0x19//internal use
#define 		LGT_CMD_LIGHT_READ_STATUS 		0x1a//get status req
#define 		LGT_CMD_LIGHT_STATUS 			0x1b//get status rsp
#define 		LGT_CMD_LIGHT_ONLINE 			0x1c//get online status req//internal use
#define         LGT_CMD_LIGHT_GRP_REQ           0x1d//get group req
#define			LGT_CMD_LEFT_KEY				0x1e//internal use
#define			LGT_CMD_RIGHT_KEY				0x1f//internal use
#define			LGT_CMD_CONFIG_DEV_ADDR         0x20//add device address
#define         LGT_CMD_DEV_ADDR_RSP            0x21//rsp
#define         LGT_CMD_SET_RGB_VALUE           0x22//params[0]:1=R 2=G 3=B params[1]:percent of lumen 0~100
#define         LGT_CMD_KICK_OUT                0x23//
#define         LGT_CMD_SET_TIME                0x24//
#define         LGT_CMD_ALARM                   0x25//
#define         LGT_CMD_READ_ALARM              0x26//
#define         LGT_CMD_ALARM_RSP               0x27//
#define         LGT_CMD_GET_TIME                0x28//
#define         LGT_CMD_TIME_RSP                0x29//


#define 		ADV_INTERVAL_3_125MS                        5
#define 		ADV_INTERVAL_3_75MS                         6
#define 		ADV_INTERVAL_10MS                           16
#define 		ADV_INTERVAL_20MS                           32
#define 		ADV_INTERVAL_30MS                           48
#define 		ADV_INTERVAL_100MS                          160
#define 		ADV_INTERVAL_1_28_S                         0x0800
#define 		ADV_INTERVAL_10_24S                         16384

#define 		ADV_LOW_LATENCY_DIRECT_INTERVAL             ADV_INTERVAL_10MS
#define 		ADV_HIGH_LATENCY_DIRECT_INTERVAL            ADV_INTERVAL_3_75MS


/////////////////////////////////////////////////////////////////////////////
#define 				LL_ROLE_MASTER              0
#define 				LL_ROLE_SLAVE               1

#define			BLM_CONN_HANDLE					BIT(7)
/////////////////////////////////////////////////////////////////////////////
#define					LL_CONNECTION_UPDATE_REQ	0x00
#define					LL_CHANNEL_MAP_REQ			0x01
#define					LL_TERMINATE_IND			0x02

#define					LL_UNKNOWN_RSP				0x07

#define 				LL_FEATURE_REQ              0x08
#define 				LL_FEATURE_RSP              0x09

#define 				LL_VERSION_IND              0x0C

#define 				LL_PING_REQ					0x12
#define					LL_PING_RSP					0x13


#define					SLAVE_LL_ENC_OFF			0
#define					SLAVE_LL_ENC_REQ			1
#define					SLAVE_LL_ENC_RSP_T			2
#define					SLAVE_LL_ENC_START_REQ_T	3
#define					SLAVE_LL_ENC_START_RSP		4
#define					SLAVE_LL_ENC_START_RSP_T	5
#define					SLAVE_LL_ENC_PAUSE_REQ		6
#define					SLAVE_LL_ENC_PAUSE_RSP_T	7
#define					SLAVE_LL_ENC_PAUSE_RSP		8
#define					SLAVE_LL_REJECT_IND_T		9

#define					LL_ENC_REQ					0x03
#define					LL_ENC_RSP					0x04
#define					LL_START_ENC_REQ			0x05
#define					LL_START_ENC_RSP			0x06

#define					LL_PAUSE_ENC_REQ			0x0a
#define					LL_PAUSE_ENC_RSP			0x0b
#define					LL_REJECT_IND				0x0d

#define			BLT_ENABLE_ADV_37			BIT(0)
#define			BLT_ENABLE_ADV_38			BIT(1)
#define			BLT_ENABLE_ADV_39			BIT(2)
#define			BLT_ENABLE_ADV_ALL			(BLT_ENABLE_ADV_37 | BLT_ENABLE_ADV_38 | BLT_ENABLE_ADV_39)


/* Advertising Maximum data length */
#define 		ADV_MAX_DATA_LEN                    31

//
#define			BLT_LINK_STATE_ADV			0
#define			BLT_LINK_STATE_CONN			1

#if 0
#define			BLT_LINK_STATE_CONNECTED	2
#define			BLT_LINK_STATE_IDLE			3
#define			BLT_LINK_STATE_START		8
#define			BLT_LINK_STATE_LOST			9
#define         BLT_LINK_STATE_STANDBY      10
#endif

u8				blt_get_current_state(void);



//////////////////For 8267 passive scan
typedef u8 (*blt_passive_scan_callback_t)(int mask,u8*data, u8 rssi);

/*Passive scan control by user*/
typedef enum{
    PASSIVE_SCAN_CONTINUE  = 0x00,
    PASSIVE_SCAN_STOP  = 0x01,
}scan_control_type;

/*Passive scan func return type*/
typedef enum{
    PASSIVE_SCAN_TIMEOUT = 0x00, //passive scan finished because scan interval is arrived
    PASSIVE_SCAN_STOPED = 0x01,
}scan_func_rsp_type;

//////////////////////////////////////
typedef struct {
	u32		pkt;
	u8		dir;
	u8		iv[8];
} ble_cyrpt_nonce_t;


typedef struct {
	u32					enc_pno;
	u32					dec_pno;
	u8					sk[16];			//session key
	ble_cyrpt_nonce_t	nonce;
	u8					st;
	u8					enable;			//1: slave enable; 2: master enable
} ble_crypt_para_t;

//////////////////////////////////////

typedef void (*irq_st_func) (void);
typedef int (*l2cap_handler_t) (u8 * p);
typedef int (*l2cap_master_handler_t) (u16 conn, u8 * p);
int l2cap_att_client_handler (u16 conn, u8 *p);


////////////////// Power Management ///////////////////////
#define			SUSPEND_ADV				BIT(0)
#define			SUSPEND_CONN			BIT(1)
#define			DEEPSLEEP_ADV			BIT(2)
#define			DEEPSLEEP_CONN			BIT(3)
#define			LOWPOWER_IDLE			BIT(6)
#define			SUSPEND_DISABLE			BIT(7)


#define 		LOWPOWER_ADV			( SUSPEND_ADV  | DEEPSLEEP_ADV  )
#define 		LOWPOWER_CONN			( SUSPEND_CONN | DEEPSLEEP_CONN )


////////////////// Event Callback  ////////////////////////
typedef void (*blt_event_callback_t)(u8 e, u8 *p);
#define			BLT_EV_FLAG_BEACON_DONE				0
#define			BLT_EV_FLAG_ADV_PRE					1
#define			BLT_EV_FLAG_SCAN_RSP				2
#define			BLT_EV_FLAG_CONNECT					3
#define			BLT_EV_FLAG_TERMINATE				4
#define			BLT_EV_FLAG_PAIRING_BEGIN			5
#define			BLT_EV_FLAG_PAIRING_FAIL			6
#define			BLT_EV_FLAG_ENCRYPTION_CONN_DONE    7
#define			BLT_EV_FLAG_BRX						8
#define			BLT_EV_FLAG_IDLE					9
#define			BLT_EV_FLAG_EARLY_WAKEUP			10
#define			BLT_EV_FLAG_CHN_MAP_REQ				11
#define			BLT_EV_FLAG_CONN_PARA_REQ			12
#define			BLT_EV_FLAG_CHN_MAP_UPDATE			13
#define			BLT_EV_FLAG_CONN_PARA_UPDATE		14
#define			BLT_EV_FLAG_BOND_START				15
#define			BLT_EV_FLAG_SET_WAKEUP_SOURCE		16
#define			BLT_EV_FLAG_ADV_DURATION_TIMEOUT	17


typedef void (*general_void_func_t)(void);
typedef int (*hci_event_callback_t) (u32 h, u8 *para, int n);


/******************************* User Interface  ************************************/
//-------------------- SLAVE &  MASTER -------------------------------------
ble_sts_t 	blt_ll_readBDAddr(u8 *addr);


//--------------------  SLAVE ONLY   ---------------------------------------
// link layer
ble_sts_t 	bls_ll_setRandomAddr(u8 *randomAddr);
ble_sts_t	bls_ll_setAdvData(u8 *data, u8 len);
ble_sts_t 	bls_ll_setScanRspData(u8 *data, u8 len);
ble_sts_t   bls_ll_setAdvEnable(u8 en);
ble_sts_t   bls_ll_setAdvParam( u16 intervalMin, u16 intervalMax, u8 advType, 	   u8 ownAddrType,  \
							     u8 peerAddrType, u8 *peerAddr,   u8 adv_channelMap, u8 advFilterPolicy);

ble_sts_t   bls_ll_setAdvDuration (u32 duration_us, u8 duration_en);

ble_sts_t  	blt_ll_terminateConnetion (u8 reason);


// att
u8			bls_att_pushNotifyData (u16 handle, u8 *p, int len);
u8			bls_att_pushIndicateData (u16 handle, u8 *p, int len);


//l2cap
void		bls_l2cap_requestConnParamUpdate (u16 min_interval, u16 max_interval, u16 latency, u16 timeout);


// power management
void		bls_pm_setSuspendMask (u8 mask);
u8 			bls_pm_getSuspendMask (void);
void 		bls_pm_setWakeupSource(u8 source);
void 		bls_pm_setLatencyOff(void);
void 		bls_pm_setManualLatency(u16 latency); //manual set latency to save power
u32 		bls_pm_getSystemWakeupTick(void);
bool 		bls_pm_isRxTimingAligned(void);


// callback event register
void		blt_register_event_callback (u8 e, blt_event_callback_t p);

// ble Tx fifo
u8			blt_fifo_num ();

//--------------------  MASTER ONLY   ---------------------------------------







/******************************* Stack Interface  ************************************/

void			bls_ll_init (u8 *public_adr);

u8				blt_push_fifo (u8 *p);
void			blt_set_att_table (u8 *p);

// hci

ble_sts_t 		bls_hci_le_setEventMask_cmd(u16 evtMask);
ble_sts_t 		bls_hci_le_getLocalSupportedFeatures(u8 *features);
ble_sts_t 		bls_hci_le_getRemoteSupportedFeatures(u16 connHandle);

ble_sts_t 		bls_hci_le_setAdvParam(adv_para_t *para);
ble_sts_t 		bls_hci_le_readChannelMap(u16 connHandle, u8 *returnChannelMap);
ble_sts_t 		bls_hci_le_readBufferSize_cmd(u8 *pData);

ble_sts_t		bls_hci_receiveACLData(u16 connHandle, u8 PB_Flag, u8 BC_Flag, u8 *pData );
ble_sts_t		bls_hci_sendACLData();
;





u8 blt_set_smp_busy(u8 paring_busy);
u8 blt_smp_pair_busy();
u8 blt_paring_pkt_recved( u8 pkt_recvd);
bond_initA_t* blt_get_bond_ptr();
void blt_get_adv_addr (u8* slave_address);
typedef int (*blt_LTK_req_callback_t)(u8* rand, u16 ediv);
void blt_registerLtkReqEvtCb(blt_LTK_req_callback_t* evtCbFunc);

ble_sts_t  ll_setLtk (u16 connHandle,  u8*ltk);
ble_sts_t  ll_getLtkVsConnHandleFail (u16 connHandle);
//////////////////For 8267 passive scan

u8 blt_register_passivescan_cb(blt_passive_scan_callback_t* cb);
u8 blt_passive_scan(int mask, u32 time_us);
u8 blt_stop_passive_scan(void);
/////////////////////////////////////

unsigned short crc16 (unsigned char *pD, int len);

void beacon_register_connreq_cb (void *p);  //Only used by beacon app



//////////////////////////////////
//  bc_hci_le_
//	bc_ll_
//	bc_l2cap_
//	bc_att_db_
//	bc_att_server_
//	bc_att_client_
//	bc_gap_
//	bc_smp_
//	bc_gap_adv_enable (int en);
//	bc_gap_adv_set_data(u8 *p, int n);
//	bc_gap_adv_set_para(u16 min, u16 max, );
//	bc_gap_disconnect ();
//	bs_gap_connect ();
//	bs_gap_connect_cancel ();
//	bs_gap_disconnect ();
//	bs_gap_auto_connection_start ();
//	bs_gap_auto_connection_stop ();	//0 stall all
//////////////////////////////////

void blc_l2cap_register_handler (void *p);
int blc_l2cap_packet_receive (u8 * p);
int blc_l2cap_send_data (u16 cid, u8 *p, int n);
int blc_send_acl_to_uart (u8 *p);
int blc_send_acl_to_usb (u8 *p);
int blm_send_acl_to_btusb (u16 conn, u8 *p);
int blc_acl_from_btusb ();

void blc_register_hci_handler (void *prx, void *ptx);
int blc_hci_rx_from_usb (void);
int blc_rx_from_uart (void);
int blc_hci_tx_to_usb (void);
int blc_hci_tx_to_uart (void);
int blc_hci_tx_to_btusb (void);

//------------- ATT client function -------------------------------

//------------- ATT service discovery function -------------------------------
int blm_att_discoveryService (u16 connHandle, att_db_uuid16_t *p16, int n16, att_db_uuid128_t *p128, int n128);
u16 blm_att_findHandleOfUuid16 (att_db_uuid16_t *p, u16 uuid, u16 ref);
u16 blm_att_findHandleOfUuid128 (att_db_uuid128_t *p, const u8 * uuid);

//------------	master function -----------------------------------
u8 blm_push_fifo (int h, u8 *p);

ble_sts_t blm_ll_connectWhiteList (int en);
ble_sts_t blm_ll_disconnect (u16 handle, u8 reason);
ble_sts_t blm_ll_readRemoteVersion (u16 handle);
ble_sts_t blm_ll_setScanEnable (u8 en, u8 duplicate);
ble_sts_t blm_ll_setScanParameter (u8 scan_type, u16 interval, u16 window, u8  adr_type, u8 policy);
ble_sts_t blm_ll_readRemoteFeature (u16 handle);
ble_sts_t blm_ll_readChannelMap (u16 handle, u8 * map);
ble_sts_t blm_ll_setHostChannel (u16 handle, u8 * map);

ble_sts_t blm_ll_createConnection (u16 scan_interval, u16 scan_window, u8 policy,
							  u8 adr_type, u8 *mac, u8 own_adr_type,
							  u16 conn_min, u16 conn_max, u16 conn_latency, u16 timeout,
							  u16 ce_min, u16 ce_max );

ble_sts_t blm_ll_createConnectionCancel ();

ble_sts_t blm_ll_updateConnection (u16 handle,
							  u16 conn_min, u16 conn_max, u16 conn_latency, u16 timeout,
							  u16 ce_min, u16 ce_max );
