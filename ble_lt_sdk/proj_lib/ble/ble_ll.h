
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
#define			BLE_LL_BUFF_SIZE		64
#define			BLE_LL_BUFF_NUM	        8

#define			BLT_TX_FIFO_NUM				16


#define			BLM_TX_FIFO_NUM				4
#define			BLM_TX_FIFO_SIZE			40

/////////////////////////////////////////////////////////////////////////////
#define		MAC_MATCH(m)	(blt_p_mac[0]==m[0] && blt_p_mac[1]==m[1] && blt_p_mac[2]==m[2])



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

#define					BLM_CONN_HANDLE					BIT(7)
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

#define					MASTER_LL_ENC_OFF				0
#define					MASTER_LL_ENC_REQ				1
#define					MASTER_LL_ENC_RSP_T				2
#define					MASTER_LL_ENC_START_REQ_T		3
#define					MASTER_LL_ENC_START_RSP			4
#define					MASTER_LL_ENC_START_RSP_T		5
#define					MASTER_LL_ENC_PAUSE_REQ			6
#define					MASTER_LL_ENC_PAUSE_RSP_T		7
#define					MASTER_LL_ENC_PAUSE_RSP			8
#define					MASTER_LL_REJECT_IND_T			9
#define					MASTER_LL_ENC_SMP_INFO_S		10
#define					MASTER_LL_ENC_SMP_INFO_E		11

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

//bls link layer state
#define			BLS_LINK_STATE_ADV			0
#define			BLS_LINK_STATE_CONN			1



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
	u8					mic_fail;
} ble_crypt_para_t;

typedef struct {
	u8		state;
	u8		time_update_st;
	u8		mcu_stall_en;
	u8		suspend_mask;

//	u8		rx_buff[160];
	u32		tx_fifo[BLM_TX_FIFO_NUM][BLM_TX_FIFO_SIZE>>2];
	u8		tx_wptr;
	u8		tx_rptr;
	u8		tx_num;
	u8		chn_idx;
	u8		chn_tbl[40];

	u8 		ll_localFeature;  //only 8 feature in core_4.2,  1 byte is enough
	u8 		ll_remoteFeature; //not only one for BLE master, use connHandle to identify
	u8		remoteFeatureReq;
	u8 		adv_filterPolicy;

	u8		macAddress_public[6];
	u8		macAddress_random[6];   //host may set this

	u8		rsvd;
	u8		peer_adr_type;
	u8		peer_adr[6];

	u32		conn_access_code;
	u8		conn_sn;
	u8		conn_snnesn;
	u8		conn_chn;
	u8		conn_update;

	u16		connHandle;
	u16		rsvd3;

	u32		conn_receive_packet;
	u32		conn_missing;
	u32		conn_timeout;
	u32		conn_tick;
	u32		conn_interval;
	u16		conn_inst;
	u16		conn_latency;

	u32		conn_winsize_next;
	u32		conn_timeout_next;
	u32		conn_offset_next;
	u32		conn_interval_next;
	u16		conn_inst_next;
	u16		conn_latency_next;

	u8		conn_chn_hop;
	u8		conn_chn_map[5];
	u8		conn_chn_map_next[5];
	u8		connParaUpReq_sent;

	u32		conn_Req_noAck_timeout;
	u8		conn_Req_waitAck_enable;
	u8		conn_terminate_reason;
	u8		conn_terminate_pending;   // terminate_pending = master_terminate || slave_terminate
	u8		remote_version;

	u32		slot_idx;
	u16		slot_latency;		//8 (40ms) typical; 2 (10ms) for high throughput connection
	u16		slot_num;

	u16		slot_latency_next;
	u16		enc_ediv;

	u32		enc_ivs;
	u8		enc_random[8];
	u8 		enc_skds[8];
	ble_crypt_para_t	crypt;
} st_ll_conn_master_t;

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


#define 		LOWPOWER_ADV			0x05   // ( SUSPEND_ADV  | DEEPSLEEP_ADV  )
#define 		LOWPOWER_CONN			0x0A   // ( SUSPEND_CONN | DEEPSLEEP_CONN )
#define			DEEPSLEEP_ENTER			0x0C   // ( DEEPSLEEP_ADV | DEEPSLEEP_CONN )


////////////////// Event Callback  ////////////////////////
typedef void (*blt_event_callback_t)(u8 e, u8 *p, int n);
#define			BLT_EV_FLAG_ADV						0
#define			BLT_EV_FLAG_ADV_DURATION_TIMEOUT	1
#define			BLT_EV_FLAG_SCAN_RSP				2
#define			BLT_EV_FLAG_CONNECT					3    //
#define			BLT_EV_FLAG_TERMINATE				4    //
#define			BLT_EV_FLAG_PAIRING_BEGIN			5
#define			BLT_EV_FLAG_PAIRING_FAIL			6
#define			BLT_EV_FLAG_ENCRYPTION_CONN_DONE    7
#define			BLT_EV_FLAG_USER_TIMER_WAKEUP		8
#define			BLT_EV_FLAG_GPIO_EARLY_WAKEUP		9
#define			BLT_EV_FLAG_CHN_MAP_REQ				10
#define			BLT_EV_FLAG_CONN_PARA_REQ			11
#define			BLT_EV_FLAG_CHN_MAP_UPDATE			12
#define			BLT_EV_FLAG_CONN_PARA_UPDATE		13
#define			BLT_EV_FLAG_SUSPEND_ENTER			14
#define			BLT_EV_FLAG_SUSPEND_EXIT			15


#define 		EVENT_MASK_ADV_DURATION_TIMEOUT		BIT(BLT_EV_FLAG_ADV_DURATION_TIMEOUT)
#define			EVENT_MASK_SCAN_RSP					BIT(BLT_EV_FLAG_SCAN_RSP)
#define			EVENT_MASK_CONNECT					BIT(BLT_EV_FLAG_CONNECT)
#define			EVENT_MASK_TERMINATE				BIT(BLT_EV_FLAG_TERMINATE)
#define			EVENT_MASK_CHN_MAP_REQ				BIT(BLT_EV_FLAG_CHN_MAP_REQ)
#define			EVENT_MASK_CONN_PARA_REQ			BIT(BLT_EV_FLAG_CONN_PARA_REQ)
#define			EVENT_MASK_CHN_MAP_UPDATE			BIT(BLT_EV_FLAG_CHN_MAP_UPDATE)
#define			EVENT_MASK_CONN_PARA_UPDATE			BIT(BLT_EV_FLAG_CONN_PARA_UPDATE)



typedef int (*hci_event_callback_t) (u32 h, u8 *para, int n);

blc_hci_handler_t			blc_master_handler;

extern my_fifo_t		hci_tx_fifo;
/******************************* User Interface  ************************************/

//--------------------  SLAVE ONLY   ---------------------------------------
// link layer
ble_sts_t 	bls_ll_setRandomAddr(u8 *randomAddr);
ble_sts_t	bls_ll_setAdvData(u8 *data, u8 len);
ble_sts_t 	bls_ll_setScanRspData(u8 *data, u8 len);
ble_sts_t   bls_ll_setAdvEnable(u8 en);
ble_sts_t   bls_ll_setAdvParam( u16 intervalMin, u16 intervalMax, u8 advType, 	   u8 ownAddrType,  \
							     u8 peerAddrType, u8 *peerAddr,   u8 adv_channelMap, u8 advFilterPolicy);

ble_sts_t   bls_ll_setAdvDuration (u32 duration_us, u8 duration_en);
ble_sts_t  	bls_ll_terminateConnection (u8 reason);

u8			bls_ll_getCurrentState(void);  //return  BLS_LINK_STATE_ADV/BLS_LINK_STATE_CONN
bool		bls_ll_isConnectState (void);

u16			bls_ll_getConnectionInterval(void);  // if return 0, means not in connection state
u16			bls_ll_getConnectionLatency(void);	 // if return 0, means not in connection state
u16			bls_ll_getConnectionTimeout(void);	 // if return 0, means not in connection state


u8			bls_ll_getTxFifoNumber (void);
void 		bls_ll_adjustScanRspTiming( int t_us );

u8 			bls_ll_getLatestAvgRSSI(void);

//l2cap
void		bls_l2cap_requestConnParamUpdate (u16 min_interval, u16 max_interval, u16 latency, u16 timeout);


// att
ble_sts_t	bls_att_pushNotifyData (u16 handle, u8 *p, int len);
ble_sts_t	bls_att_pushIndicateData (u16 handle, u8 *p, int len);
void		bls_att_setAttributeTable (u8 *p);



// power management
void		bls_pm_setSuspendMask (u8 mask);
u8 			bls_pm_getSuspendMask (void);
void 		bls_pm_setWakeupSource(u8 source);
u32 		bls_pm_getSystemWakeupTick(void);
bool 		bls_pm_isRxTimingAligned(void);

void 		bls_pm_setManualLatency(u16 latency); //manual set latency to save power
void 		bls_pm_setUserTimerWakeup(u32 tick, u8 enable); //user set timer wakeup
void 		bls_pm_enableAdvMcuStall(u8 en);



// application
void		bls_app_registerEventCallback (u8 e, blt_event_callback_t p);
void 		bls_register_event_data_callback (hci_event_callback_t  *event);






/******************************* Stack Interface  ************************************/
//link layer
void 			irq_blt_slave_handler(void);

void			bls_ll_init (u8 *public_adr);
ble_sts_t 		bls_ll_readBDAddr(u8 *addr);
bool			bls_ll_pushTxFifo (u8 *p);


// hci
ble_sts_t 		bls_hci_mod_setEventMask_cmd(u32 evtMask);
ble_sts_t 		bls_hci_le_setEventMask_cmd(u16 evtMask);
ble_sts_t 		bls_hci_le_getLocalSupportedFeatures(u8 *features);
ble_sts_t 		bls_hci_le_getRemoteSupportedFeatures(u16 connHandle);

ble_sts_t 		bls_hci_le_setAdvParam(adv_para_t *para);
ble_sts_t 		bls_hci_le_readChannelMap(u16 connHandle, u8 *returnChannelMap);
ble_sts_t 		bls_hci_le_readBufferSize_cmd(u8 *pData);

ble_sts_t		bls_hci_receiveACLData(u16 connHandle, u8 PB_Flag, u8 BC_Flag, u8 *pData );
ble_sts_t		bls_hci_sendACLData();

void blc_l2cap_register_handler (void *p);
int blc_l2cap_packet_receive (u8 * p);
int blc_l2cap_send_data (u16 cid, u8 *p, int n);
int blc_send_acl (u8 *p);
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
u16 blm_att_discoveryHandleOfUUID (u8 *l2cap_data, u8 *uuid128);

//------------	master function -----------------------------------
u8 blm_fifo_num (u16 h);
u8 blm_push_fifo (int h, u8 *p);

ble_sts_t blm_ll_connectWhiteList (int en);
ble_sts_t blm_ll_disconnect (u16 handle, u8 reason);
ble_sts_t blm_ll_readRemoteVersion (u16 handle);
ble_sts_t blm_ll_setScanEnable (u8 en, u8 duplicate);
ble_sts_t blm_ll_setScanParameter (u8 scan_type, u16 interval, u16 window, u8  adr_type, u8 policy);
ble_sts_t blm_ll_readRemoteFeature (u16 handle);
ble_sts_t blm_ll_readChannelMap (u16 handle, u8 * map);
ble_sts_t blm_ll_setHostChannel (u16 handle, u8 * map);

ble_sts_t blm_ll_connectDevice (u8 adr_type, u8 *mac, u16 conn_min, u16 conn_max, u16 conn_latency, u16 timeout);
ble_sts_t blm_ll_createConnection (u16 scan_interval, u16 scan_window, u8 policy,
							  u8 adr_type, u8 *mac, u8 own_adr_type,
							  u16 conn_min, u16 conn_max, u16 conn_latency, u16 timeout,
							  u16 ce_min, u16 ce_max );

ble_sts_t blm_ll_createConnectionCancel ();

ble_sts_t blm_ll_updateConnection (u16 handle,
							  u16 conn_min, u16 conn_max, u16 conn_latency, u16 timeout,
							  u16 ce_min, u16 ce_max );

st_ll_conn_master_t * blm_ll_getConnection (u16 h);

//------------	master security function -------------------
int  blm_ll_startEncryption (u8 connhandle ,u16 ediv, u8* random, u8* ltk);
void blm_ll_startDistributeKey (u8 connhandle );

//------------	slave security function -------------------
ble_sts_t bls_ll_getLtkVsConnHandleFail (u16 connHandle);
ble_sts_t  bls_ll_setLtk (u16 connHandle,  u8*ltk);
u8 blt_set_smp_busy(u8 paring_busy);
u8 blt_smp_pair_busy();
typedef int (*blt_LTK_req_callback_t)(u8* rand, u16 ediv);
void blt_registerLtkReqEvtCb(blt_LTK_req_callback_t* evtCbFunc);

int blm_l2cap_packet_receive (u16 conn, u8 * raw_pkt);
u8 * blm_l2cap_packet_pack (u16 conn, u8 * raw_pkt);
