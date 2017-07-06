#include "../../proj/tl_common.h"


#if (1)


#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
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
#include "../../proj/drivers/uart.h"

#if (HCI_ACCESS==HCI_USE_UART || HCI_ACCESS==HCI_USE_USB)
	MYFIFO_INIT(hci_rx_fifo, 72, 4);
	MYFIFO_INIT(hci_tx_fifo, 80, 8);
#endif


MYFIFO_INIT(blt_rxfifo, 64, 8);

////////////////////////////////////////////////////////////////////
u16	current_connHandle = BLE_INVALID_CONNECTION_HANDLE;	 //	handle of  connection

u32 host_update_conn_param_req = 0;
u16 host_update_conn_min;
u16 host_update_conn_latency;
u16 host_update_conn_timeout;


u8 current_conn_adr_type;
u8 current_conn_address[6];
u8 mac_configured[8] = {0};
u8 adv_ibeacon_filter = 1;


typedef void (*main_service_t) (void);

main_service_t		main_service = 0;


extern u8 read_by_type_req_uuidLen;
extern u8 read_by_type_req_uuid[16];


int main_idle_loop (void);


#if (HOST_SIMPLE_SERVICE_DISCOVERY_ENABLE)
	extern void host_att_service_disccovery_clear(void);
	int host_att_client_handler (u16 connHandle, u8 *p);
	ble_sts_t  host_att_discoveryService (u16 handle, att_db_uuid16_t *p16, int n16, att_db_uuid128_t *p128, int n128);


	#define				ATT_DB_UUID16_NUM		20
	#define				ATT_DB_UUID128_NUM		16

	u8 	conn_char_handler[8] = {0};


	u8	serviceDiscovery_adr_type;
	u8	serviceDiscovery_address[6];

	volatile	u8 	att_serviceDiscovery_busy;

	extern const u8 my_OtaUUID[16];

	const u8 beacon_confServiceUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x80,0x20,0x0c,0xee};
	const u8 beacon_advTxPowerConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x86,0x20,0x0c,0xee};
	const u8 beacon_txPowerModeConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x88,0x20,0x0c,0xee};
	const u8 beacon_proxiUUIDConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x89,0x20,0x0c,0xee};
	const u8 beacon_txPM_BeaconPeriodConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x8A,0x20,0x0c,0xee};
	const u8 beacon_BeaconCharacterResetUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x83,0x20,0x0c,0xee};
	const u8 beacon_majorConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x8B,0x20,0x0c,0xee};
	const u8 beacon_minorConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x8C,0x20,0x0c,0xee};
	const u8 beacon_eddystoneIDConfUUID[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x8D,0x20,0x0c,0xee};
	const u8 beacon_workMode[16] = {0xd8,0x81,0xc9,0x1a,0xb9,0x99,0x96,0xab,0xba,0x40,0x86,0x87,0x8E,0x20,0x0c,0xee};

	void app_service_discovery ()
	{

		att_db_uuid16_t 	db16[ATT_DB_UUID16_NUM];
		att_db_uuid128_t 	db128[ATT_DB_UUID128_NUM];
		memset (db16, 0, ATT_DB_UUID16_NUM * sizeof (att_db_uuid16_t));
		memset (db128, 0, ATT_DB_UUID128_NUM * sizeof (att_db_uuid128_t));


		if (host_att_discoveryService (current_connHandle, db16, ATT_DB_UUID16_NUM, db128, ATT_DB_UUID128_NUM) == BLE_SUCCESS)	// service discovery OK
		{
			//int h = current_connHandle & 7;
			conn_char_handler[0] = blm_att_findHandleOfUuid128 (db128, beacon_proxiUUIDConfUUID);			//UUID
			conn_char_handler[1] = blm_att_findHandleOfUuid128 (db128, beacon_majorConfUUID);				//Major
			conn_char_handler[2] = blm_att_findHandleOfUuid128 (db128, beacon_minorConfUUID);				//Minor
			conn_char_handler[3] = blm_att_findHandleOfUuid128 (db128, beacon_txPowerModeConfUUID);			//Power
			conn_char_handler[4] = blm_att_findHandleOfUuid128 (db128, beacon_txPM_BeaconPeriodConfUUID);	//Interval
			conn_char_handler[5] = blm_att_findHandleOfUuid128 (db128, my_OtaUUID);							//OTA


			//characteristic handle event
			blc_hci_send_data (current_connHandle | BLM_CONN_HANDLE | HCI_FLAG_EVENT_TLK_MODULE | 0x80,
					conn_char_handler, 8);

			//save current service discovery conn address
			serviceDiscovery_adr_type = current_conn_adr_type;
			memcpy(serviceDiscovery_address, current_conn_address, 6);
			memcpy(mac_configured, current_conn_address, 6);
			mac_configured[6] = current_conn_adr_type;
			mac_configured[7] = 0;

		}

		host_att_service_disccovery_clear();
		att_serviceDiscovery_busy = 0; //clear service discovery busy flag

	}

	void app_register_service (void *p)
	{
		main_service = p;
	}


	#define			HID_HANDLE_CONSUME_REPORT			conn_char_handler[3]
	#define			HID_HANDLE_KEYBOARD_REPORT			conn_char_handler[4]
	#define			AUDIO_HANDLE_MIC					conn_char_handler[0]
#else  //no service discovery

	//need define att handle same with slave
	#define			HID_HANDLE_CONSUME_REPORT			25
	#define			HID_HANDLE_KEYBOARD_REPORT			29
	#define			AUDIO_HANDLE_MIC					47

#endif

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////


/**********************************************************************************
				// proc user  PAIR and UNPAIR
**********************************************************************************/
#if (HOST_PAIRING_MANAGEMENT_ENABLE)

#if(__PROJECT_8261_MASTER_KMA_DONGLE__)
	#define flash_adr_pairing   0x1e000
#else
	#define flash_adr_pairing   0x78000
#endif


/* define pair slave max num,
   if exceed this max num, two methods to process new slave pairing
   method 1: overwrite the oldest one(telink use this method)
   method 2: not allow paring unness unpair happend  */
#define	USER_PAIR_SLAVE_MAX_NUM       1  //telink use max 1


typedef struct {
	u8 bond_mark;
	u8 adr_type;
	u8 address[6];
} macAddr_t;


typedef struct {
	u32 bond_flash_idx[USER_PAIR_SLAVE_MAX_NUM];  //mark paired slave mac address in flash
	macAddr_t bond_device[USER_PAIR_SLAVE_MAX_NUM];  //macAddr_t alreay defined in ble stack
	u8 curNum;
} user_salveMac_t;



//current connect slave mac adr: when dongle establish conn with slave, it will record the device mac adr
//when unpair happens, you can select this addr to delete from  slave mac adr table.
extern u8 slaveMac_curConnect[6];   //already defined in ble stack

/* flash erase strategy:
   never erase flash when dongle is working, for flash sector erase takes too much time(20-100 ms)
   this will lead to timing err
   so we only erase flash at initiation,  and with mark 0x00 for no use symbol
 */

#define ADR_BOND_MARK 		0x5A
#define ADR_ERASE_MARK		0x00
/* flash stored mac address struct:
   every 8 bytes is a address area: first one is mark, second no use, third - eighth is 6 byte address
   	   0     1           2 - 7
   | mark |     |    mac_address     |
   mark = 0xff, current area is invalid, pair info end
   mark = 0x01, current area is valid, load the following mac_address,
   mark = 0x00, current area is invalid (previous valid address is erased)
 */

int		user_bond_slave_flash_cfg_idx;  //new mac address stored flash idx


user_salveMac_t user_tbl_slaveMac;  //slave mac bond table


void user_tbl_slave_mac_delete_by_index(int index)  //remove the oldest adr in slave mac table
{
	//erase the oldest with ERASE_MARK
	u8 delete_mark = ADR_ERASE_MARK;
	flash_write_page (flash_adr_pairing + user_tbl_slaveMac.bond_flash_idx[index], 1, &delete_mark);

	for(int i=index; i<user_tbl_slaveMac.curNum - 1; i++){ 	//move data
		user_tbl_slaveMac.bond_flash_idx[i] = user_tbl_slaveMac.bond_flash_idx[i+1];
		memcpy( (u8 *)&user_tbl_slaveMac.bond_device[i], (u8 *)&user_tbl_slaveMac.bond_device[i+1], 8 );
	}

	user_tbl_slaveMac.curNum --;
}


int user_tbl_slave_mac_add(u8 adr_type, u8 *adr)  //add new mac address to table
{
	u8 add_new = 0;
	if(user_tbl_slaveMac.curNum >= USER_PAIR_SLAVE_MAX_NUM){ //salve mac table is full
		//slave mac max, telink use  method 1: overwrite the oldest one
		user_tbl_slave_mac_delete_by_index(0);  //overwrite, delete index 0 (oldest) of table
		add_new = 1;  //add new
	}
	else{//slave mac table not full
		add_new = 1;
	}

	if(add_new){

		user_tbl_slaveMac.bond_device[user_tbl_slaveMac.curNum].bond_mark = ADR_BOND_MARK;
		user_tbl_slaveMac.bond_device[user_tbl_slaveMac.curNum].adr_type = adr_type;
		memcpy(user_tbl_slaveMac.bond_device[user_tbl_slaveMac.curNum].address, adr, 6);

		user_bond_slave_flash_cfg_idx += 8;  //inc flash idx to get the new 8 bytes area
		flash_write_page (flash_adr_pairing + user_bond_slave_flash_cfg_idx, 8, (u8 *)&user_tbl_slaveMac.bond_device[user_tbl_slaveMac.curNum] );

		user_tbl_slaveMac.bond_flash_idx[user_tbl_slaveMac.curNum] = user_bond_slave_flash_cfg_idx;  //mark flash idx
		user_tbl_slaveMac.curNum++;

		return 1;  //add OK
	}

	return 0;
}

/* search mac address in the bond slave mac table:
   when slave paired with dongle, add this addr to table
   re_poweron slave, dongle will search if this AdvA in slave adv pkt is in this table
   if in, it will connect slave directly
   this function must in ramcode
 */
int user_tbl_slave_mac_search(u8 adr_type, u8 * adr)
{
	for(int i=0; i< user_tbl_slaveMac.curNum; i++){
		if( user_tbl_slaveMac.bond_device[i].adr_type == adr_type && \
			!memcmp(user_tbl_slaveMac.bond_device[i].address ,adr, 6)){  //match

			return (i+1);  //return index+1( 1 - USER_PAIR_SLAVE_MAX_NUM)
		}
	}

	return 0;
}

//when rc trigger unpair, use this function to delete the slave mac
int user_tbl_slave_mac_delete_by_adr(u8 adr_type, u8 *adr)  //remove adr from slave mac table by adr
{
	for(int i=0;i<user_tbl_slaveMac.curNum;i++){
		if( user_tbl_slaveMac.bond_device[i].adr_type == adr_type && \
			!memcmp(user_tbl_slaveMac.bond_device[i].address ,adr, 6)){  //match

			//erase the match adr
			u8 delete_mark = ADR_ERASE_MARK;
			flash_write_page (flash_adr_pairing + user_tbl_slaveMac.bond_flash_idx[i], 1, &delete_mark);

			for(int j=i; j< user_tbl_slaveMac.curNum - 1;j++){ //move data
				user_tbl_slaveMac.bond_flash_idx[j] = user_tbl_slaveMac.bond_flash_idx[j+1];
				memcpy((u8 *)&user_tbl_slaveMac.bond_device[j], (u8 *)&user_tbl_slaveMac.bond_device[j+1], 8);
			}

			user_tbl_slaveMac.curNum --;
			return 1; //delete OK
		}
	}

	return 0;
}




void user_tbl_slave_mac_delete_all(void)  //delete all the  adr in slave mac table
{
	u8 delete_mark = ADR_ERASE_MARK;
	for(int i=0; i< user_tbl_slaveMac.curNum; i++){
		flash_write_page (flash_adr_pairing + user_tbl_slaveMac.bond_flash_idx[i], 1, &delete_mark);
		memset( (u8 *)&user_tbl_slaveMac.bond_device[i], 0, 8);
		//user_tbl_slaveMac.bond_flash_idx[i] = 0;  //do not  concern
	}

	user_tbl_slaveMac.curNum = 0;
}



/* unpair cmd proc
   when press unpair button on dongle or send unpair cmd from slave
   dongle will call this function to process current unpair cmd
 */
void user_tbl_salve_mac_unpair_proc(void)
{
	//telink will delete all adr when unpair happens, you can change to your own strategy
	//slaveMac_curConnect is for you to use
	user_tbl_slave_mac_delete_all();
}

u8 adbg_flash_clean;
#define DBG_FLASH_CLEAN   0
//when flash stored too many addr, it may exceed a sector max(4096), so we must clean this sector
// and rewrite the valid addr at the beginning of the sector(0x11000)
void	user_bond_slave_flash_clean (void)
{
#if	DBG_FLASH_CLEAN
	if (user_bond_slave_flash_cfg_idx < 8*8)  //debug, max 8 area, then clean flash
#else
	if (user_bond_slave_flash_cfg_idx < 2048)  //max 2048/8 = 256,rest available sector is big enough, no need clean
#endif
	{
		return;
	}

	adbg_flash_clean = 1;

	flash_erase_sector (flash_adr_pairing);

	user_bond_slave_flash_cfg_idx = -8;  //init value for no bond slave mac

	//rewrite bond table at the beginning of 0x11000
	for(int i=0; i< user_tbl_slaveMac.curNum; i++){
		//u8 add_mark = ADR_BOND_MARK;

		user_bond_slave_flash_cfg_idx += 8;  //inc flash idx to get the new 8 bytes area
		flash_write_page (flash_adr_pairing + user_bond_slave_flash_cfg_idx, 8, (u8*)&user_tbl_slaveMac.bond_device[i] );

		user_tbl_slaveMac.bond_flash_idx[i] = user_bond_slave_flash_cfg_idx;  //update flash idx
	}
}

void	user_master_host_pairing_flash_init(void)
{
	u8 *pf = (u8 *)flash_adr_pairing;
	for (user_bond_slave_flash_cfg_idx=0; user_bond_slave_flash_cfg_idx<4096; user_bond_slave_flash_cfg_idx+=8)
	{ //traversing 8 bytes area in sector 0x11000 to find all the valid slave mac adr
		if( pf[user_bond_slave_flash_cfg_idx] == ADR_BOND_MARK ){  //valid adr
			if(user_tbl_slaveMac.curNum < USER_PAIR_SLAVE_MAX_NUM){
				user_tbl_slaveMac.bond_flash_idx[user_tbl_slaveMac.curNum] = user_bond_slave_flash_cfg_idx;
				flash_read_page (flash_adr_pairing + user_bond_slave_flash_cfg_idx, 8, (u8 *)&user_tbl_slaveMac.bond_device[user_tbl_slaveMac.curNum] );
				user_tbl_slaveMac.curNum ++;
			}
			else{ //slave mac in flash more than max, we think it's code bug
				write_reg32(0x9000,0x12345678);  //for debug
				irq_disable();
				while(1);
			}
		}
		else if (pf[user_bond_slave_flash_cfg_idx] == 0xff)	//end
		{
			break;
		}
	}

	user_bond_slave_flash_cfg_idx -= 8; //back to the newest addr 8 bytes area flash ixd(if no valid addr, will be -8)

	user_bond_slave_flash_clean ();
}

void user_master_host_pairing_management_init(void)
{
	user_master_host_pairing_flash_init();
}



#endif  //end of HOST_PAIRING_MANAGEMENT_ENABLE








int			dongle_pairing_enable = 0;
int  		dongle_unpair_enable = 0;


int app_l2cap_handler (u16 conn, u8 *raw_pkt)
{

	//l2cap data packeted, make sure that user see complete l2cap data
	rf_packet_l2cap_t *ptrL2cap = blm_l2cap_packet_pack (conn, raw_pkt);
	if (!ptrL2cap)
	{
		return 0;
	}

	blc_hci_send_data (conn | BLM_CONN_HANDLE | HCI_FLAG_ACL_BT_STD, ptrL2cap, ptrL2cap->rf_len);	//can be removed, debug purpose

	//l2cap data channel id, 4 for att, 5 for signal, 6 for smp
	if(ptrL2cap->chanId == L2CAP_CID_ATTR_PROTOCOL)  //att data
	{

	#if (HOST_SIMPLE_SERVICE_DISCOVERY_ENABLE)
		if(att_serviceDiscovery_busy)
		{   //when service discovery function(host_att_discoveryService) is running, all the data acked from slave
			//will be processed by it,  user can only send your own att cmd after  service discovery is over
			host_att_client_handler (conn, (u8 *)ptrL2cap);
		}
		else
	#endif
		{
				rf_packet_att_t *pAtt = (rf_packet_att_t*)ptrL2cap;
				u16 attHandle = pAtt->handle0 | pAtt->handle1<<8;

				if(pAtt->opcode == ATT_OP_READ_BY_TYPE_RSP)  //slave ack ATT_OP_READ_BY_TYPE_REQ data
				{
					#if (KMA_DONGLE_OTA_ENABLE)
						//when use ATT_OP_READ_BY_TYPE_REQ to find ota atthandle, should get the result
						extern void host_find_slave_ota_attHandle(u8 *p);
						host_find_slave_ota_attHandle( (u8 *)pAtt );
					#endif
					//u16 slave_ota_handle;
				}
				else if(pAtt->opcode == ATT_OP_HANDLE_VALUE_NOTI)  //slave handle notify
				{
					if(attHandle == HID_HANDLE_CONSUME_REPORT)
					{
						static u32 app_key;
						app_key++;
						//att_keyboard_media (conn, pAtt);
					}
					else if(attHandle == HID_HANDLE_KEYBOARD_REPORT)
					{
						static u32 app_key;
						app_key++;
						//att_keyboard (conn, pAtt);

					}
					else if(attHandle == AUDIO_HANDLE_MIC)
					{
						static u32 app_mic;
						app_mic	++;
						//att_mic (conn, pAtt);
					}
					else
					{

					}
				}
				else if (pAtt->opcode == ATT_OP_HANDLE_VALUE_IND)
				{

				}
		}
	}
	else if(ptrL2cap->chanId == L2CAP_CID_SIG_CHANNEL)  //signal
	{
		if(ptrL2cap->opcode == L2CAP_CMD_CONN_UPD_PARA_REQ)  //slave send conn param update req on l2cap
		{
			rf_packet_l2cap_connParaUpReq_t  * req = (rf_packet_l2cap_connParaUpReq_t *)ptrL2cap;

			u32 interval_us = req->min_interval*1250;  //1.25ms unit
			u32 timeout_us = req->timeout*10000; //10ms unit
			u32 long_suspend_us = interval_us * (req->latency+1);

			//interval < 200ms
			//long suspend < 5S
			// interval * (latency +1)*2 <= timeout
			if( interval_us < 200000 && long_suspend_us < 5000000 && (long_suspend_us*2<=timeout_us) )
			{
				//when master host accept slave's conn param update req, should send a conn param update response on l2cap
				//with CONN_PARAM_UPDATE_ACCEPT; if not accpet,should send  CONN_PARAM_UPDATE_REJECT
				blc_l2cap_SendConnParamUpdateResponse(current_connHandle, CONN_PARAM_UPDATE_ACCEPT);  //send SIG Connection Param Update Response


				//if accept, master host should mark this, add will send  update conn param req on link layer later
				//set a flag here, then send update conn param req in mainloop
				host_update_conn_param_req = clock_time() | 1 ; //in case zero value
				host_update_conn_min = req->min_interval;  //backup update param
				host_update_conn_latency = req->latency;
				host_update_conn_timeout = req->timeout;
			}
		}


	}
	else if(ptrL2cap->chanId == L2CAP_CID_SMP) //smp
	{
		#if (HOST_SECURITY_ENABLE)
			u8* pr = l2cap_smp_initiator_handler (conn, (u8 *)ptrL2cap);
			if(pr)
			{
				blm_push_fifo(conn, pr);
			}
		#endif
	}
	else
	{

	}


	return 0;
}



int master_auto_connect = 0;
int user_manual_paring;

//////////////////////////////////////////////////////////
// event call back
//////////////////////////////////////////////////////////
int app_event_callback (u32 h, u8 *p, int n)
{


	static u32 event_cb_num, event_cb_conn;
	event_cb_num++;
	int send_to_hci = 1;

	if (h &HCI_FLAG_EVENT_BT_STD)		//ble controller hci event
	{
		u8 evtCode = h & 0xff;

		//------------ disconnect -------------------------------------
		if(evtCode == HCI_CMD_DISCONNECTION_COMPLETE)  //connection terminate
		{
			gpio_write (GPIO_LED_BLUE, 0);
			event_disconnection_t	*pd = (event_disconnection_t *)p;

			//terminate reason
			//connection timeout
			if(pd->reason == HCI_ERR_CONN_TIMEOUT){

			}
			//peer device(slave) send terminate cmd on link layer
			else if(pd->reason == HCI_ERR_REMOTE_USER_TERM_CONN){

			}
			//master host disconnect( blm_ll_disconnect(current_connHandle, HCI_ERR_REMOTE_USER_TERM_CONN) )
			else if(pd->reason == HCI_ERR_CONN_TERM_BY_LOCAL_HOST){

			}
			 //master create connection, send conn_req, but did not received acked packet in 6 connection event
			else if(pd->reason == HCI_ERR_CONN_FAILED_TO_ESTABLISH){
				//when controller is in initiating state, find the specified device, send connection request to slave,
				//but slave lost this rf packet, there will no ack packet from slave, after 6 connection events, master
				//controller send a disconnect event with reason HCI_ERR_CONN_FAILED_TO_ESTABLISH
				//if slave got the connection request packet and send ack within 6 connection events, controller
				//send connection establish event to host(telink defined event)


			}
			else{

			}

			//led show none connection state
			gpio_write(GPIO_LED_WHITE, LED_ON_LEVAL);   //white on
			gpio_write(GPIO_LED_RED, !LED_ON_LEVAL);    //red off


			current_connHandle = BLE_INVALID_CONNECTION_HANDLE;  //when disconnect, clear conn handle
			if(att_serviceDiscovery_busy){  //when disconnect, service discovery busy flag
				att_serviceDiscovery_busy = 0;
			}

			host_update_conn_param_req = 0; //when disconnect, clear update conn flag

			//should set scan mode again to scan slave adv packet
			blc_ll_setScanParameter(SCAN_TYPE_PASSIVE, SCAN_INTERVAL_100MS, SCAN_INTERVAL_100MS,
									  OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_ANY);
			blc_ll_setScanEnable (1, 0);


		}
		else if(evtCode == HCI_EVT_LE_META)
		{
			u8 subEvt_code = p[0];

			//------------le connection complete event-------------------------------------
			if (subEvt_code == HCI_SUB_EVT_LE_CONNECTION_COMPLETE)	// connection complete
			{
				event_cb_conn++;
				gpio_write (GPIO_LED_BLUE, 1);
				//after controller is set to initiating state by host (blc_ll_createConnection(...) )
				//it will scan the specified device(adr_type & mac), when find this adv packet, send a connection request packet to slave
				//and enter to connection state, send connection complete evnet. but notice that connection complete not
				//equals to connection establish. connection complete measn that master controller set all the ble timing
				//get ready, but has not received any slave packet, if slave rf lost the connection request packet, it will
				//not send any packet to master controller



			}
			else if(subEvt_code == HCI_SUB_EVT_LE_CONNECTION_ESTABLISH)  //connection establish(telink private event)
			{
				//notice that: this connection event is defined by telink, not a standard ble controller event
				//after master controller send connection request packet to slave, when slave received this packet
				//and enter to connection state, send a ack packet within 6 connection event, master will send
				//connection establish event to host(HCI_SUB_EVT_LE_CONNECTION_ESTABLISH)

				event_connection_complete_t *pc = (event_connection_complete_t *)p;
				if (pc->status == BLE_SUCCESS)	// status OK
				{
					//led show connection state
					gpio_write(GPIO_LED_RED, LED_ON_LEVAL);     //red on
					gpio_write(GPIO_LED_WHITE, !LED_ON_LEVAL);  //white off


					current_connHandle = pc->handle;   //mark conn handle, in fact this equals to BLM_CONN_HANDLE

					#if (HOST_SIMPLE_SERVICE_DISCOVERY_ENABLE)
						//new slave device, should do service discovery again
						if (1 || current_conn_adr_type != serviceDiscovery_adr_type || \
							memcmp(current_conn_address, serviceDiscovery_address, 6))
						{
							app_register_service(&app_service_discovery);
							att_serviceDiscovery_busy = 1;  //set service discovery busy flag
						}
					#endif


					#if (HOST_PAIRING_MANAGEMENT_ENABLE)
						// if this connection establish is a new device manual paring, should add this device to slave table
						if(user_manual_paring && !master_auto_connect){
							user_tbl_slave_mac_add(pc->peer_adr_type, pc->mac);
						}
					#endif
				}
			}
			//------------ le ADV report event ------------------------------------------
			else if (subEvt_code == HCI_SUB_EVT_LE_ADVERTISING_REPORT)	// ADV packet
			{
				//after controller is set to scan state, it will report all the adv packet it received by this event

				event_adv_report_t *pa = (event_adv_report_t *)p;
				s8 rssi = pa->data[pa->len];
				u8 rs = rssi;

				//vendor_apple: 0x004c; ibeacon: 0x1502
				if (!adv_ibeacon_filter)
				{

				}
				else if (pa->data[5] != 0x4c || pa->data[6] != 0 || pa->data[7] != 0x02 || pa->data[8] != 0x15
					|| rs < 0xc8 )
				//if (pa->data[5] != 0x4c || pa->data[6] != 0)
				{
					send_to_hci = 0;
				}
				else
				{
					//pa->adr_type = pa->data[pa->len];;
				}

				//user design manual paring methods, telink demo use button press and
				//rssi threshold(which means short distance) to trigger manual paring,
				//use can use special adv packet data to manual paring, compare with pa->len & pa->data
				user_manual_paring = dongle_pairing_enable && (rssi > -56);  //user design

				master_auto_connect = 0;
				#if (HOST_PAIRING_MANAGEMENT_ENABLE)
					//search in slave mac table to find whether this device is an old device which has already paired with master
					master_auto_connect = user_tbl_slave_mac_search(pa->adr_type, pa->mac);
				#endif

				//if(master_auto_connect || user_manual_paring || send_to_hci)
				//skip the device, if configured
				if (mac_configured[7] && memcmp (mac_configured, pa->mac, 6) == 0)
				{

				}
				else if (rs > 0xe0 && pa->event_type == 0 )
				{
					//send create connection cmd to controller, trigger it switch to initiating state, after this cmd,
					//controller will scan all the adv packets it received but not report to host, to find the specified
					//device(adr_type & mac), then send a connection request packet after 150us, enter to connection state
					// and send a connection complete event(HCI_SUB_EVT_LE_CONNECTION_COMPLETE)
					blc_ll_createConnection( SCAN_INTERVAL_100MS, SCAN_INTERVAL_100MS, INITIATE_FP_ADV_SPECIFY,  \
											 pa->adr_type, pa->mac, BLE_ADDR_PUBLIC, \
											 CONN_INTERVAL_30MS, CONN_INTERVAL_30MS, 0, CONN_TIMEOUT_4S, \
											 0, 0);

					//save current connect address type and address
					current_conn_adr_type = pa->adr_type;
					memcpy(current_conn_address, pa->mac, 6);
					event_cb_conn |= BIT(16);
				}

			}
			//------------le connection update complete event-------------------------------
			else if (subEvt_code == HCI_SUB_EVT_LE_CONNECTION_UPDATE_COMPLETE)	// connection update
			{
				//after master host send update conn param req cmd to controller( blm_ll_updateConnection(...) ),
				//when update take effect, controller send update complete event to host

				event_connection_update_t *pc = (event_connection_update_t *)p;

				#if (KMA_DONGLE_OTA_ENABLE)
					extern void host_ota_update_conn_complete(u16, u16, u16);
					host_ota_update_conn_complete( pc->interval, pc->latency, pc->timeout );
				#endif


			}
		}
	}

	if (send_to_hci)
	{
			blc_hci_send_data (h, p, n);
	}
	return 1;
}




#if (HCI_ACCESS == HCI_USE_UART)
	uart_data_t T_txdata_buf;

	int blc_rx_from_uart (void)
	{
		if(my_fifo_get(&hci_rx_fifo) == 0)
		{
			return 0;
		}

		u8* p = my_fifo_get(&hci_rx_fifo);
		u32 rx_len = p[0]; //usually <= 255 so 1 byte should be sufficient

		if (rx_len)
		{
			blm_hci_handler(&p[4], rx_len - 4);
			my_fifo_pop(&hci_rx_fifo);
		}


		return 0;
	}

	int blc_hci_tx_to_uart ()
	{
		static u32 uart_tx_tick = 0;

		u8 *p = my_fifo_get (&hci_tx_fifo);



	#if 1 //(ADD_DELAY_FOR_UART_DATA)
		if (p && !uart_tx_is_busy () && clock_time_exceed(uart_tx_tick, 30000))
	#else
		if (p && !uart_tx_is_busy ())
	#endif
		{
			memcpy(&T_txdata_buf.data, p + 2, p[0]+p[1]*256);
			T_txdata_buf.len = p[0]+p[1]*256 ;


			if (uart_Send((u8 *)(&T_txdata_buf)))
			{
				uart_tx_tick = clock_time();

				my_fifo_pop (&hci_tx_fifo);
			}
		}
		return 0;

	}
#endif

int app_hci_rx_from_usb (void)
{
	static u32 AACC = 0;
	u8 buff[72];
	static u32 hci_tx_ddd;

	extern int usb_bulk_out_get_data (u8 *p);
	int n = usb_bulk_out_get_data (buff);
	if (n)
	{
		hci_tx_ddd++;
		if (buff[0] == 0xd0 && buff[1] == 0xff)			//configuration done
		{
			mac_configured[7] = 1;
			blm_ll_disconnect(current_connHandle, HCI_ERR_REMOTE_USER_TERM_CONN);
		}
		else if (buff[0] == 0xd1 && buff[1] == 0xff)	//reset configuration status
		{
			mac_configured[7] = 0;
		}
		else if (buff[0] == 0xd2 && buff[1] == 0xff)	//read handle of char
		{
			blc_hci_send_data (current_connHandle | BLM_CONN_HANDLE | HCI_FLAG_EVENT_TLK_MODULE | 0x80,
								conn_char_handler, 8);
		}
		else if (buff[0] == 0xd3 && buff[1] == 0xff)	//set RX range: 0 ~ 9
		{
			u8 r = buff[4] & 0x0f;
			if (r > 9)
			{
				r = 9;
			}
			REG_ADDR8 (0x43e) = 0xc0 | r;	//
		}
		else if (buff[0] == 0xd4 && buff[1] == 0xff)	//disable ibeacon filter
		{
			adv_ibeacon_filter = buff[4];
		}
		else if (buff[0] == 0xfe && buff[1] == 0xff)	//connect to
		{
			blc_ll_createConnection( SCAN_INTERVAL_100MS, SCAN_INTERVAL_100MS, INITIATE_FP_ADV_SPECIFY,  \
										0, buff + 5, BLE_ADDR_PUBLIC, \
										CONN_INTERVAL_30MS, CONN_INTERVAL_30MS, 0, CONN_TIMEOUT_1S, \
										0, 0);
			current_conn_adr_type = buff[4];
			AACC++;
			memcpy(current_conn_address, buff + 5, 6);
		}
		else if (!blc_master_handler || !blc_master_handler (buff, n))
		{
			blc_hci_handler (buff, n);
		}
	}
	return 0;
}


///////////////////////////////////////////
void user_init()
{
	blc_app_loadCustomizedParameters();  //load customized freq_offset cap value and tp value

	//set USB ID
	usb_log_init ();
	REG_ADDR8(0x74) = 0x53;
	REG_ADDR16(0x7e) = 0x08d0;
	REG_ADDR8(0x74) = 0x00;

	usb_dp_pullup_en (1);  //open USB enum


///////////// BLE stack Initialization ////////////////
	u8  tbl_mac [6];
	if (*(u32 *) CFG_ADR_MAC == 0xffffffff){
		u16 * ps = (u16 *) tbl_mac;
		ps[0] = REG_ADDR16(0x448);  //random
		ps[1] = REG_ADDR16(0x448);
		ps[2] = REG_ADDR16(0x448);
		flash_write_page (CFG_ADR_MAC, 6, tbl_mac);  //store master address
	}
	else{
		memcpy (tbl_mac, (u8 *) CFG_ADR_MAC, 6);  //copy from flash
	}


	////// Controller Initialization  //////////
	blc_ll_initBasicMCU(tbl_mac);   //mandatory

	blc_ll_initScanning_module(tbl_mac); 	//scan module: 		 mandatory for BLE master,
	blc_ll_initInitiating_module();			//initiate module: 	 mandatory for BLE master,
	blc_ll_initMasterRoleSingleConn_module();			//master module: 	 mandatory for BLE master,


	//// controller hci event mask config ////
	//bluetooth event
	blc_hci_setEventMask_cmd (HCI_EVT_MASK_DISCONNECTION_COMPLETE);
	//bluetooth low energy(LE) event
	//HCI_SUB_EVT_LE_CONNECTION_COMPLETE
	blc_hci_le_setEventMask_cmd(		HCI_LE_EVT_MASK_CONNECTION_COMPLETE  \
									|	HCI_LE_EVT_MASK_ADVERTISING_REPORT \
									|   HCI_LE_EVT_MASK_CONNECTION_UPDATE_COMPLETE \
									|   HCI_LE_EVT_MASK_CONNECTION_ESTABLISH);



	////// Host Initialization  //////////
#if (HCI_ACCESS==HCI_USE_USB)
	usb_bulk_drv_init (0);
	blc_register_hci_handler (app_hci_rx_from_usb, blc_hci_tx_to_usb);
	REG_ADDR8 (0x43e) = 0xc2;			//max gain 2 (default 9: 42 db lower)
	//blc_ll_setAdvCustomedChannel (39, 39, 39);
#endif

	blc_enable_hci_master_handler ();
	blc_l2cap_register_handler (app_l2cap_handler);  //controller data to host(l2cap data) all processed in this func
	blc_hci_registerControllerEventHandler(app_event_callback); //controller hci event to host all processed in this func
	host_att_register_idle_func (main_idle_loop);


///////////// USER Initialization ////////////////
	rf_set_power_level_index (RF_POWER_8dBm);
	ll_whiteList_reset();  //clear whitelist



	user_master_host_pairing_management_init();


	//set scan paramter and scan enable
	blc_ll_setScanParameter(SCAN_TYPE_PASSIVE, SCAN_INTERVAL_100MS, SCAN_INTERVAL_100MS,
							  OWN_ADDRESS_PUBLIC, SCAN_FP_ALLOW_ADV_ANY);
	blc_ll_setScanEnable (1, 0);

#if(HOST_SECURITY_ENABLE)
	blm_smp_enableParing (SMP_PARING_PEER_TRRIGER);
#endif

}



/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
int main_idle_loop (void)
{
	static u32 tick_loop;
	tick_loop ++;


	////////////////////////////////////// BLE entry /////////////////////////////////
	blt_sdk_main_loop();


	/////////////////////////////////////// HCI ///////////////////////////////////////
	extern int blc_hci_proc ();
	blc_hci_proc ();


	////////////////////////////////////// UI entry /////////////////////////////////
	proc_button();  //button triggers pair & unpair  and OTA

	//terminate and unpair proc
	static int master_disconnect_flag;
	if(dongle_unpair_enable){
		if(!master_disconnect_flag && blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){
			if( blm_ll_disconnect(current_connHandle, HCI_ERR_REMOTE_USER_TERM_CONN) == BLE_SUCCESS){
				master_disconnect_flag = 1;

				#if (HOST_PAIRING_MANAGEMENT_ENABLE)
					user_tbl_slave_mac_delete_all();
				#endif
			}
		}
	}
	if(master_disconnect_flag && blc_ll_getCurrentState() != BLS_LINK_STATE_CONN){
		master_disconnect_flag = 0;
	}


#if(KMA_DONGLE_OTA_ENABLE)
	extern void proc_ota (void);
	proc_ota();
#endif


	//proc master update
	//50ms later and make sure service discovery is finished
	if( host_update_conn_param_req && clock_time_exceed(host_update_conn_param_req, 50000) && !att_serviceDiscovery_busy)
	{
		host_update_conn_param_req = 0;

		if(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){  //still in connection state
			blm_ll_updateConnection (current_connHandle,
					host_update_conn_min, host_update_conn_min, host_update_conn_latency,  host_update_conn_timeout,
											  0, 0 );
		}
	}


#if (KMA_DONGLE_OTA_ENABLE)
	extern u8 host_ota_update_pending;
	if(host_ota_update_pending == 1 && !host_update_conn_param_req && !att_serviceDiscovery_busy)
	{

		if(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN){  //still in connection state
			//10ms interval,  latency = 0   timeout = 2S
			blm_ll_updateConnection (current_connHandle,
										8, 8, 0,  200,
											  0, 0 );

			host_ota_update_pending = 2;

		}
	}
#endif



	return 0;
}




void main_loop (void)
{
//#if (HCI_ACCESS==HCI_USE_UART || HCI_ACCESS==HCI_USE_USB)
#if (0)
	blt_sdk_main_loop();
#else
	main_idle_loop ();

	if (main_service)
	{
		main_service ();
		main_service = 0;
	}
#endif
}



#endif  //end of __PROJECT_826X_MASTER_KMA_DONGLE__
