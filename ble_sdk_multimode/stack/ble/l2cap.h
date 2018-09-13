#pragma  once

//#include "../hci/hci_include.h"



// define the l2cap CID for BLE
#define L2CAP_CID_NULL                   0x0000
#define L2CAP_CID_ATTR_PROTOCOL          0x0004
#define L2CAP_CID_SIG_CHANNEL            0x0005
#define L2CAP_CID_SMP                    0x0006
#define L2CAP_CID_GENERIC                0x0007
#define L2CAP_CID_DYNAMIC                0x0040

#define L2CAP_CID_DYNAMIC                0x0040

#define L2CAP_HEADER_LENGTH              0x0004
#define L2CAP_MTU_SIZE                   23

#define L2CAP_CMD_REJECT                 0x01
#define L2CAP_CMD_DISC_CONN_REQ          0x06
#define L2CAP_CMD_DISC_CONN_RESP         0x07
#define L2CAP_CMD_CONN_UPD_PARA_REQ      0x12
#define L2CAP_CMD_CONN_UPD_PARA_RESP     0x13
#define L2CAP_CMD_CONN_REQ               0x14
#define L2CAP_CMD_CONN_RESP              0x15
#define L2CAP_CMD_FLOW_CTRL_CRED         0x16


#define L2CAP_SIGNAL_MSG_TYPE            0x01
#define L2CAP_DATA_MSG_TYPE              0x02

/** 
 * Command Reject: Reason Codes
 */
  // Command not understood
#define L2CAP_REJECT_CMD_NOT_UNDERSTOOD  0x0000

  // Signaling MTU exceeded
#define L2CAP_REJECT_SIGNAL_MTU_EXCEED   0x0001

  // Invalid CID in request
#define L2CAP_REJECT_INVALID_CID         0x0002

// Response Timeout expired
#define L2CAP_RTX_TIMEOUT_MS             2000

#define NEXT_SIG_ID()                    ( ++l2capId == 0 ? l2capId = 1 : l2capId )


#define L2CAP_PKT_HANDLER_SIZE           6
 
 
// l2cap handler type
#define L2CAP_CMD_PKT_HANDLER            0
#define L2CAP_USER_CB_HANDLER            1

// l2cap pb flag type
#define L2CAP_FRIST_PKT_H2C              0x00
#define L2CAP_CONTINUING_PKT             0x01
#define L2CAP_FIRST_PKT_C2H              0x02



#define L2CAP_CONNECTION_PARAMETER_ACCEPTED        0x0000
#define L2CAP_CONNECTION_PARAMETER_REJECTED        0x0001



//RF_LEN_MAX:255: MIC(4)+LL_MAX_LEN(251) => LL_MAX_LEN:l2cap_len(2)+cid(2)+ATT_MTU_MAX(247).
//l2cap buffer max: dma(4)+header(2)+l2cap_len(2)+cid(2)+ATT_MTU_MAX(247).
#define	L2CAP_RX_BUFF_LEN_MAX			  260//257 = 10+247,align 4, here we use 260
//dma(4)+header(2)+l2cap_len(2)+cid(2)+Attribute_data[ATT_MTU]
#define	ATT_RX_MTU_SIZE_MAX		  		  (L2CAP_RX_BUFF_LEN_MAX - 10)

#define L2CAP_RX_PDU_OFFSET				  12



typedef struct{
	u16 	connParaUpReq_minInterval;
	u16 	connParaUpReq_maxInterval;
	u16 	connParaUpReq_latency;
	u16 	connParaUpReq_timeout;

	u8		connParaUpReq_pending;
}para_up_req_t;

extern para_up_req_t	para_upReq;




typedef int (*l2cap_handler_t) (u16 conn, u8 * p);

typedef int (*l2cap_conn_update_rsp_callback_t) (u8 id, u16 result);

extern l2cap_handler_t	blc_l2cap_handler;


typedef enum{
	CONN_PARAM_UPDATE_ACCEPT = 0x0000,
	CONN_PARAM_UPDATE_REJECT = 0x0001,
}conn_para_up_rsp;




/******************************* User Interface  ************************************/
void		bls_l2cap_requestConnParamUpdate (u16 min_interval, u16 max_interval, u16 latency, u16 timeout);  //Slave

void        bls_l2cap_setMinimalUpdateReqSendingTime_after_connCreate(int time_ms);

void		blc_l2cap_register_handler (void *p);
int 		blc_l2cap_packet_receive (u16 connHandle, u8 * p);
int 		blc_l2cap_send_data (u16 cid, u8 *p, int n);

void 		blc_l2cap_reg_att_sig_hander(void *p);//signaling pkt proc



void  		blc_l2cap_SendConnParamUpdateResponse(u16 connHandle, u8 req_id, conn_para_up_rsp result);
void 		blc_l2cap_registerConnUpdateRspCb(l2cap_conn_update_rsp_callback_t cb);


//Master
