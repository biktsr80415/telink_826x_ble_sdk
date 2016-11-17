#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"
#include "spp.h"


extern int	module_uart_data_flg;
extern u32 module_wakeup_module_tick;

extern my_fifo_t hci_tx_fifo;
extern void app_suspend_exit ();
///////////the code below is just for demonstration of the event callback only////////////
void event_handler(u32 h, u8 *para, int n)
{
	if((h&HCI_FLAG_EVENT_TLK_MODULE)!= 0)			//module event
	{
		switch((u8)(h&0xff))
		{
			case BLT_EV_FLAG_SCAN_RSP:
				break;
			case BLT_EV_FLAG_CONNECT:
			{
				u32 header;
				header = 0x0780 + BLT_EV_FLAG_CONNECT;		//state change event
				header |= HCI_FLAG_EVENT_TLK_MODULE;
				hci_send_data(header, NULL, 0);		//HCI_FLAG_EVENT_TLK_MODULE
				task_connect();
			}
				break;
			case BLT_EV_FLAG_TERMINATE:
			{
				u32 header;
				header = 0x0780 + BLT_EV_FLAG_TERMINATE;		//state change event
				header |= HCI_FLAG_EVENT_TLK_MODULE;
				hci_send_data(header, NULL, 0);		//HCI_FLAG_EVENT_TLK_MODULE
			}
				break;
			case BLT_EV_FLAG_PAIRING_BEGIN:
				break;
			case BLT_EV_FLAG_PAIRING_FAIL:
				break;
			case BLT_EV_FLAG_ENCRYPTION_CONN_DONE:
				break;
			case BLT_EV_FLAG_USER_TIMER_WAKEUP:
				break;
			case BLT_EV_FLAG_GPIO_EARLY_WAKEUP:
				break;
			case BLT_EV_FLAG_CHN_MAP_REQ:
				break;
			case BLT_EV_FLAG_CONN_PARA_REQ:
				break;
			case BLT_EV_FLAG_CHN_MAP_UPDATE:
			{
				u32 header;
				header = 0x0780 + BLT_EV_FLAG_CHN_MAP_UPDATE;		//state change event
				header |= HCI_FLAG_EVENT_TLK_MODULE;
				hci_send_data(header, NULL, 0);		//HCI_FLAG_EVENT_TLK_MODULE
			}
				break;
			case BLT_EV_FLAG_CONN_PARA_UPDATE:
			{
				u32 header;
				header = 0x0780 + BLT_EV_FLAG_CONN_PARA_UPDATE;		//state change event
				header |= HCI_FLAG_EVENT_TLK_MODULE;
				hci_send_data(header, NULL, 0);		//HCI_FLAG_EVENT_TLK_MODULE
			}
				break;
			case BLT_EV_FLAG_ADV_DURATION_TIMEOUT:
				break;
			case BLT_EV_FLAG_SUSPEND_EXIT:
				app_suspend_exit ();
				break;
			default:
				break;
		}
	}
}
/////////////////////////////////////blc_register_hci_handler for spp////////////////////////////
int rx_from_uart_cb (void)//UART data send to Master,we will handler the data as CMD or DATA
{
	if(rx_uart_w_index==rx_uart_r_index)  //rx buff empty
	{
        return 0;
	}

	u32 rx_len = T_rxdata_buf[rx_uart_r_index].len + 4 > sizeof(T_rxdata_user) ? sizeof(T_rxdata_user) : T_rxdata_buf[rx_uart_r_index].len + 4;
	memcpy(&T_rxdata_user, &T_rxdata_buf[rx_uart_r_index], rx_len);
	memset(&T_rxdata_buf[rx_uart_r_index],0,sizeof(uart_data_t));

	if (rx_len)
	{
		bls_uart_handler(T_rxdata_user.data, rx_len - 4);//todo:define your own handler for SPP CMD/DATA
	}
	rx_uart_r_index = (rx_uart_r_index + 1)&0x01;

	return 0;
}

///////////////////////////////////////////the default bls_uart_handler///////////////////////////////
int bls_uart_handler (u8 *p, int n)
{
	u8  status = BLE_SUCCESS;
	int  cmdLen;
	u8 *cmdPara;

	u16 cmd = p[0]|p[1]<<8;
	u32	header = 0;
	u8	para[16] = {0};
	u8 para_len = 1;
	cmdPara = p + 4;
	cmdLen = p[2] | p[3]<<8;
	header = ((p[0] + p[1] * 256) & 0x3ff) | 0x400;		//event complete
	header |= (3 << 16) | HCI_FLAG_EVENT_TLK_MODULE;
	// set advertising interval: 01 ff 02 00 50 00: 80 *0.625ms
	if (cmd == SPP_CMD_SET_ADV_INTV)
	{
		u8 interval = cmdPara[0] ;
		status = blt_set_advinterval(interval, interval);
	}
	// set advertising data: 02 ff 06 00 01 02 03 04 05 06
	else if (cmd == SPP_CMD_SET_ADV_DATA)
	{
		status = (u8)bls_ll_setAdvData(cmdPara, p[2]);
	}
	// enable/disable advertising: 0a ff 01 00  01
	else if (cmd == SPP_CMD_SET_ADV_ENABLE)
	{
		status = (u8)bls_ll_setAdvEnable(cmdPara[0]);
		para[0] = status;
	}
	// send data: 0b ff 05 00  01 02 03 04 05
	//change format to ob ff 07 handle(2bytes) 00 01 02 03 04 05
	else if (cmd == 0xFF0B)
	{

	}
	// get module available data buffer: 0c ff 00  00
	else if (cmd == SPP_CMD_GET_BUF_SIZE)
	{
		u8 r[4];
		para[0] = (u8)bls_hci_le_readBufferSize_cmd( (u8 *)(r) );
		para[1] = r[2];
		para_len = 2;
	}
	// set advertising type: 0d ff 01 00  00
	else if (cmd == SPP_CMD_SET_ADV_TYPE)
	{
		status = blt_set_advtype(cmdPara[0]);
	}
	// set advertising direct address: 0e ff 07 00  00(public; 1 for random) 01 02 03 04 05 06
	else if (cmd == SPP_CMD_SET_ADV_DIRECT_ADDR)
	{
		status = blt_set_adv_addrtype(cmdPara);
	}
	// add white list entry: 0f ff 07 00 01 02 03 04 05 06
	else if (cmd == SPP_CMD_ADD_WHITE_LST_ENTRY)
	{
		status = (u8)ll_whiteList_add(cmdPara[0], cmdPara + 1);
	}
	// delete white list entry: 10  ff 07 00 01 02 03 04 05 06
	else if (cmd == SPP_CMD_DEL_WHITE_LST_ENTRY)
	{
		status = (u8)ll_whiteList_delete(cmdPara[0], cmdPara + 1);
	}
	// reset white list entry: 11 ff 00 00
	else if (cmd == SPP_CMD_RST_WHITE_LST)
	{
		status = (u8)ll_whiteList_reset();
	}
	// set filter policy: 12 ff 10 00 00(bit0: scan WL enable; bit1: connect WL enable)
	else if (cmd == SPP_CMD_SET_FLT_POLICY)
	{
		status = ble_set_adv_filterpolicy(cmdPara[0]);
	}
	// set device name: 13 ff 0a 00  01 02 03 04 05 06 07 08 09 0a
	else if (cmd == SPP_CMD_SET_DEV_NAME)
	{
		status = ble_set_dev_name(cmdPara,p[2]);
	}
	// get connection parameter: 14 ff 00 00
	else if (cmd == SPP_CMD_GET_CONN_PARA)
	{
		blt_get_conn_para(cmdPara+1);
		para_len = 7;
	}
	// set connection parameter: 15 ff 08 00 a0 00 a2 00 00 00 2c 01 (min, max, latency, timeout)
	else if (cmd == SPP_CMD_SET_CONN_PARA)
	{
		bls_l2cap_requestConnParamUpdate(cmdPara[0]|cmdPara[1]<<8,cmdPara[2]|cmdPara[3]<<8,cmdPara[4]|cmdPara[5]<<8,cmdPara[6]|cmdPara[7]<<8);
	}
	// get module current work state: 16 ff 00 00
	else if (cmd == SPP_CMD_GET_CUR_STATE)
	{
		cmdPara[0] = bls_ll_getCurrentState();
	}
	// terminate connection: 17 ff 00 00
	else if (cmd == SPP_CMD_TERMINATE)
	{
		bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN);
	}
	// restart module: 18 ff 00 00
	else if (cmd == SPP_CMD_RESTART_MOD)
	{
		cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_TIMER, clock_time() + 10000 * sys_tick_per_us);
	}
	// enable/disable MAC binding function: 19 ff 01 00 00(disable, 01 enable)
	else if (cmd == 0x19)
	{

	}
	// add MAC address to binding table: 1a ff 06 00 01 02 03 04 05 06
	else if (cmd == 0x1a)
	{

	}
	// delete MAC address from binding table: 1b ff 06 00 01 02 03 04 05 06
	else if (cmd == 0x1b)
	{

	}
	//change format to 1c ff 07 handle(2bytes) 00 01 02 03 04 05
	else if (cmd == SPP_CMD_SEND_NOTIFY_DATA)
	{
		if (cmdLen > 22)
		{
			status = 2;			//data too long
		}
		else
		{
			status = bls_att_pushNotifyData( cmdPara[0] | (cmdPara[1]<<8), cmdPara + 2,  cmdLen - 2);
		}
	}
	para[0] = status;
	hci_send_data (header, para, para_len);
	return 0;
}

#define			HCI_BUFFER_TX_SIZE		72
#define			HCI_BUFFER_NUM			4
#if 0
u8				hci_buff[HCI_BUFFER_NUM][HCI_BUFFER_TX_SIZE] = {{0,},};
u8				hci_buff_wptr = 0;
u8				hci_buff_rptr = 0;
#endif
int hci_send_data (u32 h, u8 *para, int n)
{

	u8 *p = my_fifo_wptr (&hci_tx_fifo);
	if (!p || n >= hci_tx_fifo.size)
	{
		return -1;
	}

#if (BLE_MODULE_INDICATE_DATA_TO_MCU)
	if(!module_uart_data_flg){ //UART上空闲，新的数据发送
		GPIO_WAKEUP_MCU_HIGH;  //通知MCU有数据了
		module_wakeup_module_tick = clock_time() | 1;
		module_uart_data_flg = 1;
	}
#endif

	int nl = n + 4;
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
	my_fifo_next (&hci_tx_fifo);
	return 0;
}

int tx_to_uart_cb (void)
{
	u8 *p = my_fifo_get (&hci_tx_fifo);
	if (p && !uart_tx_is_busy ())
	{
		memcpy(&T_txdata_buf.data, p + 2, p[0]+p[1]*256);
		T_txdata_buf.len = p[0]+p[1]*256 ;


#if (BLE_MODULE_INDICATE_DATA_TO_MCU)
		//如果MCU端设计的有低功耗，而module有数据拉高GPIO_WAKEUP_MCU时只是将mcu唤醒，那么需要考虑
		//mcu从唤醒到能够稳定的接收uart数据是否需要一个回复时间T。如果需要回复时间T的话，这里
		//将下面的100us改为user实际需要的时间。
		if(module_wakeup_module_tick){
			while( !clock_time_exceed(module_wakeup_module_tick, 100) );
		}
#endif


#if __PROJECT_8266_MODULE__
		if (uart_Send_kma((u8 *)(&T_txdata_buf)))
#else
		if (uart_Send((u8 *)(&T_txdata_buf)))
#endif
		{
			my_fifo_pop (&hci_tx_fifo);
		}
	}
	return 0;
}
