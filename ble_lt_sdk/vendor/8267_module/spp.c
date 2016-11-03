#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"

////////////////////////////////////event callback////////////////////////////////
extern hci_event_callback_t	bls_event_callback;
u8 bls_event_cb_register(int* cb)
{
	if(cb!=0)
	{
		bls_event_callback = (hci_event_callback_t)(cb);
		return 0;
	}
	else return 1;
}

///////////the code below is just for demonstration of the event callback only////////////
void event_handler(u32 h, u8 *para, int n)
{
	static u8 terminate_flag;
	static u8 conn_flag;
	if((u8)(h&0xff) == 0x05)
		terminate_flag = 1;
	else if((u8)(h&0xff)== 0x3e)
	{
		hci_le_readRemoteFeaturesCompleteEvt_t* evt = (hci_le_readRemoteFeaturesCompleteEvt_t*)(para);
		if(evt->subEventCode == HCI_SUB_EVT_LE_CONNECTION_COMPLETE)
			conn_flag = 1;
	}
}
/////////////////////////////////////blc_register_hci_handler for spp////////////////////////////
int rx_from_uart_cb (void)//UART data send to Master,we will handler the data as CMD or DATA
{
	if(rx_uart_w_index==rx_uart_r_index)  //rx buff empty
	{
        return 0;
	}

	u32 rx_len = T_rxdata_buf[rx_uart_w_index].len + 4 > sizeof(T_rxdata_user) ? sizeof(T_rxdata_user) : T_rxdata_buf[rx_uart_w_index].len + 4;
	memcpy(&T_rxdata_user, &T_rxdata_buf[rx_uart_w_index], rx_len);
	memset(&T_rxdata_buf[rx_uart_w_index],0,sizeof(uart_data_t));

	if (rx_len)
	{
		bls_uart_handler(T_rxdata_user.data, rx_len - 4);//todo:define your own handler for SPP CMD/DATA
	}
	rx_uart_w_index = (rx_uart_w_index + 1)&0x01;

	return 0;
}


///////////////////////////////////////////the default bls_uart_handler///////////////////////////////
int bls_uart_handler (u8 *p, int n)
{
	u8  status = BLE_SUCCESS;
	int  cmdLen;
	u8 *cmdPara;

	u32	header = 0;
	u8	para[16] = {0};
	u8 para_len = 1;
	cmdPara = p + 4;
	cmdLen = p[2] | p[3]<<8;
	header = ((p[0] + p[1] * 256) & 0x3ff) | 0x400;		//event complete
	header |= (3 << 16) | HCI_FLAG_EVENT_TLK_MODULE;
	// set advertising interval: 01 ff 02 00 50 00: 80 *0.625ms
	if (p[0] == 0x01)
	{
		u8 interval = cmdPara[0] ;
		status = blt_set_advinterval(interval);
	}
	// set advertising data: 02 ff 06 00 01 02 03 04 05 06
	else if (p[0] == 0x02)
	{
		status = (u8)bls_ll_setAdvData(cmdPara, p[2]);
	}
	// enable/disable advertising: 0a ff 01 00  01
	else if (p[0] == 0x0a)
	{
		status = (u8)bls_ll_setAdvEnable(cmdPara[0]);
		para[0] = status;
	}
	// send data: 0b ff 05 00  01 02 03 04 05
	//change format to ob ff 07 handle(2bytes) 00 01 02 03 04 05
	else if (p[0] == 0x0b)
	{
	//			u8 len = p[2] - 2;
	//			u16 handle = cmdPara[0]|(cmdPara[1]<<8);
	//			if(bls_att_pushNotifyData(handle,cmdPara+2,(u32)(len)))
	//				status = BLE_SUCCESS;
	}
	// get module available data buffer: 0c ff 00  00
	else if (p[0] == 0x0c)
	{

	}
	// set advertising type: 0d ff 01 00  00
	else if (p[0] == 0x0d)
	{
		status = blt_set_advtype(cmdPara[0]);
	}
	// set advertising direct address: 0e ff 07 00  00(public; 1 for random) 01 02 03 04 05 06
	else if (p[0] == 0x0e)
	{
		status = blt_set_adv_addrtype(cmdPara);
	}
	// add white list entry: 0f ff 07 00 01 02 03 04 05 06
	else if (p[0] == 0x0f)
	{
		status = (u8)ll_whiteList_add(cmdPara[0], cmdPara + 1);
	}
	// delete white list entry: 10  ff 07 00 01 02 03 04 05 06
	else if (p[0] == 0x10)
	{
		status = (u8)ll_whiteList_delete(cmdPara[0], cmdPara + 1);
	}
	// reset white list entry: 11 ff 00 00
	else if (p[0] == 0x11)
	{
		status = (u8)ll_whiteList_reset();
	}
	// set filter policy: 12 ff 10 00 00(bit0: scan WL enable; bit1: connect WL enable)
	else if (p[0] == 0x12)
	{
		status = ble_set_adv_filterpolicy(cmdPara[0]);
	}
	// set device name: 13 ff 0a 00  01 02 03 04 05 06 07 08 09 0a
	else if (p[0] == 0x13)
	{
		status = ble_set_dev_name(cmdPara,p[2]);
	}
	// get connection parameter: 14 ff 00 00
	else if (p[0] == 0x14)
	{
		blt_get_conn_para(cmdPara+1);
		para_len = 7;
	}
	// set connection parameter: 15 ff 08 00 a0 00 a2 00 00 00 2c 01 (min, max, latency, timeout)
	else if (p[0] == 0x15)
	{
		bls_l2cap_requestConnParamUpdate((u16)cmdPara[0],(u16)cmdPara[2],(u16)cmdPara[4],(u16)cmdPara[6]);
	}
	// get module current work state: 16 ff 00 00
	else if (p[0] == 0x16)
	{
		cmdPara[0] = bls_ll_getCurrentState();
	}
	// terminate connection: 17 ff 00 00
	else if (p[0] == 0x17)
	{
		bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
	}
	// restart module: 18 ff 00 00
	else if (p[0] == 0x18)
	{
		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, clock_time() + 10000 * sys_tick_per_us);
	}
	// enable/disable MAC binding function: 19 ff 01 00 00(disable, 01 enable)
	else if (p[0] == 0x19)
	{

	}
	// add MAC address to binding table: 1a ff 06 00 01 02 03 04 05 06
	else if (p[0] == 0x1a)
	{

	}
	// delete MAC address from binding table: 1b ff 06 00 01 02 03 04 05 06
	else if (p[0] == 0x1b)
	{

	}
	//change format to 1c ff 07 handle(2bytes) 00 01 02 03 04 05
	else if (p[0] == 0x1c)
	{
		u8 len = p[2] - 2;
		u16 handle = cmdPara[0]|(cmdPara[1]<<8);
		if(bls_att_pushNotifyData(handle,cmdPara+2,(u32)(len)))
			status = BLE_SUCCESS;
	}
	para[0] = status;
	hci_send_data (header, para, para_len);
	return 0;
}

#define			HCI_BUFFER_TX_SIZE		72
#define			HCI_BUFFER_NUM			4
u8				hci_buff[HCI_BUFFER_NUM][HCI_BUFFER_TX_SIZE] = {{0,},};
u8				hci_buff_wptr = 0;
u8				hci_buff_rptr = 0;
int hci_send_data (u32 h, u8 *para, int n)
{
	if (((hci_buff_wptr - hci_buff_rptr) & 63) >= HCI_BUFFER_NUM)
	{
		return -1;
	}

	if (n >= HCI_BUFFER_TX_SIZE)
	{
		return -1;
	}

	int nl = n + 4;
	u8 *p = hci_buff[hci_buff_wptr++ & (HCI_BUFFER_NUM-1)];
	if (h & HCI_FLAG_EVENT_TLK_MODULE)
	{
		*p++ = nl;
		*p++ = nl >> 8;
		*p++ = 0xff;
		*p++ = n + 2;
		*p++ = h;
		*p++ = h>>8;
		memcpy (p, para, n);
		p += n;
	}
	return 0;
}

int tx_to_uart_cb ()
{
	if (hci_buff_rptr != hci_buff_wptr && !uart_tx_is_busy ())
	{
		u8 *p = hci_buff[hci_buff_rptr & (HCI_BUFFER_NUM-1)];
		memcpy(&T_txdata_buf.data, p + 2, p[0]+p[1]*256);
		T_txdata_buf.len = p[0]+p[1]*256 ;
		if (uart_Send_kma((u8 *)(&T_txdata_buf)))
		{
			hci_buff_rptr++;
		}
	}
	return 0;
}
