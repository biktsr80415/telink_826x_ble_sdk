#ifndef LL__H_
#define LL__H_


#include <stack/ble/ble_common.h>
#include <stack/ble/blt_config.h>
#include <stack/ble/hci/hci_const.h>
#include <stack/ble/hci/hci_event.h>
#include <stack/ble/ll/ll_adv.h>
#include <stack/ble/ll/ll_encrypt.h>
#include <stack/ble/ll/ll_init.h>
#include <stack/ble/ll/ll_master.h>
#include <stack/ble/ll/ll_pm.h>
#include <stack/ble/ll/ll_scan.h>
#include <stack/ble/ll/ll_slave.h>
#include <stack/ble/ll/ll_whitelist.h>


#include "tl_common.h"
#include "drivers.h"


#define         VENDOR_ID                       0x0211
#define			BLUETOOTH_VER_4_0				6
#define			BLUETOOTH_VER_4_1				7
#define			BLUETOOTH_VER_4_2				8


#ifndef 		BLUETOOTH_VER
#define			BLUETOOTH_VER					BLUETOOTH_VER_4_2
#endif


#if (BLUETOOTH_VER == BLUETOOTH_VER_4_2)
	#define			BLUETOOTH_VER_SUBVER			0x22BB
#else
	#define			BLUETOOTH_VER_SUBVER			0x4103
#endif



/////////////////////////////////////////////////////////////////////////////
#define		CLOCK_SYS_CLOCK_1250US			(1250 * sys_tick_per_us)
#define		CLOCK_SYS_CLOCK_10MS			(10000 * sys_tick_per_us)
#define		FLG_RF_CONN_DONE	(FLD_RF_IRQ_CMD_DONE | FLD_RF_IRQ_FSM_TIMEOUT | FLD_RF_IRQ_FIRST_TIMEOUT | FLD_RF_IRQ_RX_TIMEOUT | FLD_RF_IRQ_RX_CRC_2)


/////////////////////////////////////////////////////////////////////////////
#define 				CONTROLLER_ONLY				0//1//

#define 				LL_ROLE_MASTER              0
#define 				LL_ROLE_SLAVE               1

#define					BLM_CONN_HANDLE				BIT(7)
#define					BLS_CONN_HANDLE				BIT(6)

#define					HANDLE_STK_FLAG				BIT(15)
/////////////////////////////////////////////////////////////////////////////
#define					LL_CONNECTION_UPDATE_REQ	0x00
#define					LL_CHANNEL_MAP_REQ			0x01
#define					LL_TERMINATE_IND			0x02

#define					LL_UNKNOWN_RSP				0x07
#define 				LL_FEATURE_REQ              0x08
#define 				LL_FEATURE_RSP              0x09

#define 				LL_VERSION_IND              0x0C
#define 				LL_REJECT_IND         		0x0D
#define 				LL_SLAVE_FEATURE_REQ        0x0E	//core_4.1
#define 				LL_CONNECTION_PARAM_REQ		0x0F	//core_4.1
#define 				LL_CONNECTION_PARAM_RSP		0x10	//core_4.1
#define					LL_REJECT_IND_EXT			0x11	//core_4.1
#define 				LL_PING_REQ					0x12    //core_4.1
#define					LL_PING_RSP					0x13    //core_4.1
#define 				LL_LENGTH_REQ				0x14    //core_4.2
#define					LL_LENGTH_RSP				0x15    //core_4.2

#define					LL_ENC_REQ					0x03
#define					LL_ENC_RSP					0x04
#define					LL_START_ENC_REQ			0x05
#define					LL_START_ENC_RSP			0x06

#define					LL_PAUSE_ENC_REQ			0x0A
#define					LL_PAUSE_ENC_RSP			0x0B




#if (BLUETOOTH_VER == BLUETOOTH_VER_4_0)
	#define LL_FEATURE_MASK_DEFAULT		LL_FEATURE_MASK_LL_ENCRYPTION
	#define LL_CMD_MAX					LL_REJECT_IND

#elif (BLUETOOTH_VER == BLUETOOTH_VER_4_1)
	#define LL_FEATURE_MASK_DEFAULT		(  LL_FEATURE_MASK_LL_ENCRYPTION                      |   \
									   	   LL_FEATURE_MASK_SLAVE_INITIATED_FEATURES_EXCHANGE  |   \
									   	   LL_FEATURE_MASK_LE_PING					)
	#define LL_CMD_MAX					LL_PING_RSP

#elif (BLUETOOTH_VER == BLUETOOTH_VER_4_2)

	#if (BLE_CORE42_DATA_LENGTH_EXTENSION_ENABLE)
		#define LL_FEATURE_MASK_DEFAULT		(  LL_FEATURE_MASK_LL_ENCRYPTION                      |   \
											   LL_FEATURE_MASK_SLAVE_INITIATED_FEATURES_EXCHANGE  |   \
											   LL_FEATURE_MASK_LE_PING							  |   \
											   LL_FEATURE_MASK_LE_DATA_PACKET_EXTENSION	)
	#else
		#define LL_FEATURE_MASK_DEFAULT		(  LL_FEATURE_MASK_LL_ENCRYPTION                      |   \
											   LL_FEATURE_MASK_SLAVE_INITIATED_FEATURES_EXCHANGE  |   \
											   LL_FEATURE_MASK_LE_PING					)
	#endif

	#define LL_CMD_MAX					LL_LENGTH_RSP

#else


#endif









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


#define			LL_ADV_TX_SETTLE				74
#define 		LL_SLAVE_TX_SETTLE				75   //TX settle time is short


//ble link layer state
#define			BLS_LINK_STATE_IDLE				0
#define			BLS_LINK_STATE_ADV				BIT(0)
#define			BLS_LINK_STATE_SCAN				BIT(1)
#define			BLS_LINK_STATE_INIT				BIT(2)
#define 		BLS_LINK_STATE_CONN				BIT(3)

//#define 		BLS_LINK_STATE_CONN_SLAVE		BIT(3)
//#define 		BLS_LINK_STATE_CONN_MASTER		BIT(4)





#define				BLE_STATE_BTX_S			4
#define				BLE_STATE_BTX_E			5
#define				BLE_STATE_BRX_S			6
#define				BLE_STATE_BRX_E			7




#define			MAX_OCTETS_DATA_LEN_27					27
#define			MAX_OCTETS_DATA_LEN_EXTENSION			251


#define			LL_PACKET_OCTET_TIME(n)				((n) * 8 + 112)

#define 		DATA_LENGTH_REQ_PENDING				1
#define			DATA_LENGTH_REQ_DONE				2


my_fifo_t			blt_rxfifo;
u8					blt_rxfifo_b[];

my_fifo_t			blt_txfifo;
u8					blt_txfifo_b[];
//////////////////////////////////////


typedef struct {
	u8		macAddress_public[6];
	u8		macAddress_random[6];   //host may set this
}ll_mac_t;

extern ll_mac_t  bltMac;

typedef struct {
	u8		adv_en;
	u8		adv_extension_mask;
	u8		adv_scanReq_connReq;
	u8 		phy_en;


	u8		ll_recentAvgRSSI;
	u8		ll_localFeature;
	u8		tx_irq_proc_en;


	u8		conn_rx_num;  //slave: rx number in a new interval
} st_ll_conn_t;

extern st_ll_conn_t  bltParam;


typedef struct {
	u16		connEffectiveMaxRxOctets;
	u16		connEffectiveMaxTxOctets;
	u16 	connMaxRxOctets;
	u16 	connMaxTxOctets;
	u16		connRemoteMaxRxOctets;
	u16 	connRemoteMaxTxOctets;
	u16		supportedMaxRxOctets;
	u16		supportedMaxTxOctets;

	u16 	connInitialMaxTxOctets;
	u8		connMaxTxRxOctets_req;
}ll_data_extension_t;

extern ll_data_extension_t  bltData;





////////////////// Telink defined Event Callback  ////////////////////////
typedef void (*blt_event_callback_t)(u8 e, u8 *p, int n);

#define 		BLT_EV_MAX_NUM						20

#define			BLT_EV_FLAG_ADV						0
#define			BLT_EV_FLAG_ADV_DURATION_TIMEOUT	1
#define			BLT_EV_FLAG_SCAN_RSP				2
#define			BLT_EV_FLAG_CONNECT					3    //
#define			BLT_EV_FLAG_TERMINATE				4    //
#define			BLT_EV_FLAG_PAIRING_BEGIN			5
#define			BLT_EV_FLAG_PAIRING_END				6	 //success or fail(with fail reason)
#define			BLT_EV_FLAG_ENCRYPTION_CONN_DONE    7
#define			BLT_EV_FLAG_DATA_LENGTH_EXCHANGE	8
#define			BLT_EV_FLAG_GPIO_EARLY_WAKEUP		9
#define			BLT_EV_FLAG_CHN_MAP_REQ				10
#define			BLT_EV_FLAG_CONN_PARA_REQ			11
#define			BLT_EV_FLAG_CHN_MAP_UPDATE			12
#define			BLT_EV_FLAG_CONN_PARA_UPDATE		13
#define			BLT_EV_FLAG_SUSPEND_ENTER			14
#define			BLT_EV_FLAG_SUSPEND_EXIT			15
#define			BLT_EV_FLAG_READ_P256_KEY			16
#define			BLT_EV_FLAG_GENERATE_DHKEY			17
#define			BLT_EV_FLAG_SMP_PINCODE_PROCESS	    18





#define 		EVENT_MASK_ADV_DURATION_TIMEOUT		BIT(BLT_EV_FLAG_ADV_DURATION_TIMEOUT)
#define			EVENT_MASK_SCAN_RSP					BIT(BLT_EV_FLAG_SCAN_RSP)
#define			EVENT_MASK_CONNECT					BIT(BLT_EV_FLAG_CONNECT)
#define			EVENT_MASK_TERMINATE				BIT(BLT_EV_FLAG_TERMINATE)
#define			EVENT_MASK_CHN_MAP_REQ				BIT(BLT_EV_FLAG_CHN_MAP_REQ)
#define			EVENT_MASK_CONN_PARA_REQ			BIT(BLT_EV_FLAG_CONN_PARA_REQ)
#define			EVENT_MASK_CHN_MAP_UPDATE			BIT(BLT_EV_FLAG_CHN_MAP_UPDATE)
#define			EVENT_MASK_CONN_PARA_UPDATE			BIT(BLT_EV_FLAG_CONN_PARA_UPDATE)
#define			EVENT_MASK_READ_P256_KEY			BIT(BLT_EV_FLAG_READ_P256_KEY)
#define			EVENT_MASK_GENERATE_DHKEY			BIT(BLT_EV_FLAG_GENERATE_DHKEY)



typedef void (*ll_irq_tx_callback_t)(void);

typedef int (*ll_irq_rx_data_callback_t)(u8 *, u32);
typedef int (*ll_irq_rx_post_callback_t)(void);

typedef void (*ll_irq_systemTick_conn_callback_t)(void);


typedef int (*blc_main_loop_data_callback_t)(u8 *);
typedef int (*blc_main_loop_post_callback_t)(void);


typedef int (*blc_main_loop_phyTest_callback_t)(void);





typedef int (*blt_LTK_req_callback_t)(u16 handle, u8* rand, u16 ediv);



extern my_fifo_t		hci_tx_fifo;

extern u8			 blt_state;
extern st_ll_scan_t  blts;

/******************************* User Interface  ************************************/
void		irq_blt_sdk_handler ();;
int 		blt_sdk_main_loop(void);

void 		blc_ll_initStandby_module (u8 *public_adr);

ble_sts_t 	blc_ll_setRandomAddr(u8 *randomAddr);


ble_sts_t 	blc_ll_readBDAddr(u8 *addr);

u8			blc_ll_getCurrentState(void);

u8 			blc_ll_getLatestAvgRSSI(void);

u16   		blc_ll_setInitTxDataLength (u16 maxTxOct);   //core4.2 long data packet


bool		blc_ll_isControllerEventPending(void);

u8  		blc_ll_getTxFifoNumber (void);

void 		blc_ll_initBasicMCU (void);

// application
void		bls_app_registerEventCallback (u8 e, blt_event_callback_t p);




void 		blc_ll_recoverDeepRetention(void);


bool 		blc_ll_isBrxBusy (void);

/************************* Stack Interface, user can not use!!! ***************************/
//encryption
ble_sts_t 		blc_hci_ltkRequestNegativeReply (u16 connHandle);
ble_sts_t  		blc_hci_ltkRequestReply (u16 connHandle,  u8*ltk);

void 			blc_ll_setEncryptionBusy(u8 enc_busy);
bool 			blc_ll_isEncryptionBusy(void);
void 			blc_ll_registerLtkReqEvtCb(blt_LTK_req_callback_t evtCbFunc);

void 			blc_ll_setIdleState(void);
ble_sts_t 		blc_hci_le_getLocalSupportedFeatures(u8 *features);

ble_sts_t 		blc_hci_le_readBufferSize_cmd(u8 *pData);


int 			blc_ll_encrypted_data(u8*key, u8*plaintextData, u8* encrypteTextData);

//core4.2 data extension
void 			blc_ll_initDataLengthExtension (void);
ble_sts_t   	blc_ll_exchangeDataLength (u8 opcode, u16 maxTxOct);
ble_sts_t 		blc_hci_setTxDataLength (u16 connHandle, u16 tx, u16 txtime);
ble_sts_t 		blc_hci_readSuggestedDefaultTxDataLength (u8 *tx, u8 *txtime);
ble_sts_t 		blc_hci_writeSuggestedDefaultTxDataLength (u16 tx, u16 txtime);




int blm_send_acl_to_btusb (u16 conn, u8 *p);





static inline void blc_ll_resetInfoRSSI(void)
{
	bltParam.ll_recentAvgRSSI = 0;
}

static inline void blc_ll_recordRSSI(u8 rssi)
{
	if(bltParam.ll_recentAvgRSSI == 0) {
		bltParam.ll_recentAvgRSSI = rssi;
	}
	else {
		bltParam.ll_recentAvgRSSI = (bltParam.ll_recentAvgRSSI + rssi) >> 1;
	}
}



/****************************** (ble1m,2m,500k,125k)RF RX/TX packet format ********************************************
RF RX packet format:
  b0          b3    b4         b5       b6   b(5+w) b(6+w) b(8+w) b(9+w) b(12+w)  b(13+w)    b(14+w)  b(15+w)                      b(16+w)
*---------------*---------*-----------*------------*------------*---------------*-------------------*----------*--------------------------------------------------*
|  DMA_len(4B)  | type(1B)| Rf_len(1B)| payload(wB)|   CRC(3B)  | time_stamp(4B)|  Fre_offset(2B)   | Rssi(1B) |           pkt status indicator(1B)               |
| (b0,b1 valid) |        Header       |   Payload  |            |               |                   | rssi-110 |[0]:crc err;[1]:sfd err;[2]:ll err;[4]:pwr err;   |
|               |<--           PDU              -->|            |               |                   |          |[4]:long range 125k;[6:5]:N/A;[7]:nordic NACK ind |
*---------------*----------------------------------*------------*---------------*-------------------*----------*--------------------------------------------------*
|<--- 4byte --->|<------ 2 byte ----->|<- Rf_len ->|<- 3 byte ->|<----------------------------------- 8 byte ---------------------------------------------------->|
note:       b4       ->  type(1B): llid(2bit) nesn(1bit) sn(1bit) md(1bit).
we can see: DMA_len     =   rx[0] = w(Rf_len)+13 = rx[5]+13.
            CRC_OK      =   DMA_buffer[rx[0]+3] == 0x00 ? True : False.

******
RF TX packet format:
 b0          b3      b4         b5       b6   b(5+w)
*---------------*----------*-----------*------------*
|  DMA_len(4B)  | type(1B) | Rf_len(1B)| payload(wB)|
| (b0,b1 valid) |         Header       |   Payload  |
|               |<--               PDU           -->|
*---------------*-----------------------------------*
note:       b4      ->  type(1B): llid(2bit) nesn(1bit) sn(1bit) md(1bit).Here type only means that llid, other bit is automatically populated when sent by hardware
we can see: DMA_len = rx[0]= w(Rf_len) + 2.
**********************************************************************************************************************/



/************************************** Link Layer pkt format *********************************************************
Link Layer pak format(BLE4.2 spec):
*-------------*-------------------*-------------------------------*-------------------*
| preamble(1B)| Access Address(4B)|          PDU(2~257B)          |      CRC(3B)      |
|             |                   |  Header(2B) | payload(0~255B) |                   |
*-------------*-------------------*-------------------------------*-------------------*
1.ADV Channel, payload:0~37bytes = 6bytes AdvAdd + [maximum 31bytes adv packet payload]
2.Data Channel, payload:0~255bytes = 0~251bytes + 4bytes MIC(may include MIC feild)[The payload in ble4.2 can reach 251 bytes].
  Protocol overhead: 10bytes(preamble\Access Address\Header\CRC) + L2CAP header 4bytes = 14bytes, all LL data contains 14 bytes of overhead,
  For att, opCode is also needed, 1bytes + handle 2bytes = 3bytes, 251-4-3=[final 247-3bytes available to users].
******
Link Layer pak format(BLE4.0\4.1 spec):
*-------------*-------------------*-------------------------------*-------------------*
| preamble(1B)| Access Address(4B)|          PDU(2~39B)           |      CRC(3B)      |
|             |                   |  Header(2B) | payload(0~37B)  |                   |
*-------------*-------------------*-------------------------------*-------------------*
1.ADV Channel, payload:0~37bytes = 6bytes AdvAdd + [maximum 31bytes adv packet payload]
2.Data Channel, payload:0~31bytes = 0~27bytes + 4bytes MIC(may include MIC feild)[The payload in ble4.0/4.1 is 27 bytes].
  Protocol overhead: 10bytes(preamble\Access Address\Header\CRC) + L2CAP header 4bytes = 14bytes,all LL data contains 14 bytes of overhead,
  For att, opCode is also needed, 1bytes + handle 2bytes = 3bytes, 27-4-3=[final 23-3bytes available to users]£¬This is why the default mtu size is 23 in the ble4.0 protocol.
**********************************************************************************************************************/


/*********************************** Advertising channel PDU : Header *************************************************
Header(2B):[Advertising channel PDU Header](BLE4.0\4.1 spec):
*--------------*----------*------------*-------------*-------------*----------*
|PDU Type(4bit)| RFU(2bit)| TxAdd(1bit)| RxAdd(1bit) |Length(6bits)| RFU(2bit)|
*--------------*----------*------------*-------------*-------------*----------*
public (TxAdd = 0) or random (TxAdd = 1).
**********************************************************************************************************************/


/******************************************* Data channel PDU : Header ************************************************
Header(2B):[Data channel PDU Header](BLE4.2 spec):(BLE4.0\4.1 spec):
*----------*-----------*---------*----------*----------*-------------*----------*
|LLID(2bit)| NESN(1bit)| SN(1bit)| MD(1bit) | RFU(3bit)|Length(5bits)| RFU(3bit)|
*----------*-----------*---------*----------*----------*-------------*----------*
******
Header(2B):[Data channel PDU Header](BLE4.2 spec):
*----------*-----------*---------*----------*----------*------------------------*
|LLID(2bit)| NESN(1bit)| SN(1bit)| MD(1bit) | RFU(3bit)|       Length(8bits)    |
*----------*-----------*---------*----------*----------*------------------------*
start    pkt:  llid 2 -> 0x02
continue pkt:  llid 1 -> 0x01
control  pkt:  llid 3 -> 0x03
***********************************************************************************************************************/


/*********************************** DATA channel PDU ******************************************************************
*------------------------------------- ll data pkt -------------------------------------------*
|             |llid nesn sn md |  pdu-len   | l2cap_len(2B)| chanId(2B)|  opCode(1B)|data(xB) |
| DMA_len(4B) |   type(1B)     | rf_len(1B) |       L2CAP header       |       value          |
|             |          data_headr         |                        payload                  |
*-------------*-----------------------------*-------------------------------------------------*
*--------------------------------- ll control pkt ----------------------------*
| DMA_len(4B) |llid nesn sn md |  pdu-len   | LL Opcode(1B) |  CtrData(0~22B) |
|             |   type(1B)     | rf_len(1B) |               |      value      |
|             |          data_headr         |            payload              |
*-------------*-----------------------------*---------------------------------*
***********************************************************************************************************************/


#define				STOP_RF_STATE_MACHINE	( REG_ADDR8(0xf00) = 0x80 )



#if (BLC_REGISTER_DBG_GPIO_IN_STACK)


void app_register_debug_gpio(GPIO_PinTypeDef io_pm,
							 GPIO_PinTypeDef io_brx,
							 GPIO_PinTypeDef io_rx,
							 GPIO_PinTypeDef io_rxOK,
							 GPIO_PinTypeDef io_snOK);


extern unsigned	int		gpio_dbg_pm_reg;
extern unsigned	int		gpio_dbg_brx_reg;
extern unsigned	int		gpio_dbg_rx_reg;
extern unsigned	int		gpio_dbg_rxOK_reg;
extern unsigned	int		gpio_dbg_snOK_reg;

extern unsigned char	gpio_dbg_pm_bit;
extern unsigned char	gpio_dbg_brx_bit;
extern unsigned char	gpio_dbg_rx_bit;
extern unsigned char	gpio_dbg_rxOK_bit;
extern unsigned char	gpio_dbg_snOK_bit;

#endif

#endif /* LL__H_ */
