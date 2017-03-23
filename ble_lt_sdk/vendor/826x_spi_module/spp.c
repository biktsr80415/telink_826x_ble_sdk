/*
 * spp_8269.c
 *
 *  Created on: 2016-9-1
 *      Author: yafei.tu
 */
#include "../tl_common.h"
#include "spp.h"
#include "../../proj/drivers/spi.h"
#include "../../proj/mcu_spec/gpio_8267.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/blt_config.h"
/**********************************************************************
 * LOCAL VARIABLES
 */
///////////////////////////8269_spi.c (driver)//////////////////////////////////
void spi_pin_init_8267(enum SPI_PIN gpio_pin_x){
	if(gpio_pin_x == SPI_PIN_GPIOB){
		write_reg8(0x58e,read_reg8(0x58e)&0x0f);///disable GPIO:B<4~7>
		write_reg8(0x5b1,read_reg8(0x5b1)|0xf0);///enable SPI function:B<4~7>

		write_reg8(0x5b0,read_reg8(0x5b0)&0xC3);///disable SPI function:A<2~5>
		gpio_set_input_en(GPIO_PB4,1);
		gpio_set_input_en(GPIO_PB5,1);
		gpio_set_input_en(GPIO_PB6,1);
		gpio_set_input_en(GPIO_PB7,1);
	}
	if(gpio_pin_x == SPI_PIN_GPIOA){
		write_reg8(0x586,read_reg8(0x586)&0xC3);///disable GPIO:A<2~5>
		write_reg8(0x5b0,read_reg8(0x5b0)|0x3C);///enable SPI function:A<2~5>

		write_reg8(0x5b1,read_reg8(0x5b1)&0x0f);///disable SPI function:B<4~7>
		gpio_set_input_en(GPIO_PA2,1);
		gpio_set_input_en(GPIO_PA3,1);
		gpio_set_input_en(GPIO_PA4,1);
		gpio_set_input_en(GPIO_PA5,1);
	}
	write_reg8(0x0a,read_reg8(0x0a)|0x80);////enable spi
}

void spi_master_init_8267(unsigned char div_clock,enum SPI_MODE mode){
	write_reg8(0x0a,read_reg8(0x0a)|div_clock);/////spi clock=system clock/((div_clock+1)*2)
	write_reg8(0x09,read_reg8(0x09)|0x02);////enable master mode

	write_reg8(0x0b,read_reg8(0x0b)|mode);////select SPI mode,surpport four modes
}

void spi_slave_init_8267(unsigned char div_clock,enum SPI_MODE mode){
	write_reg8(0x0a,read_reg8(0x0a)|div_clock);/////spi clock=system clock/((div_clock+1)*2)
	write_reg8(0x09,read_reg8(0x09)&0xfd);////disable master mode

	write_reg8(0x0b,read_reg8(0x0b)|mode);////select SPI mode,surpport four modes
}

#if 1
void spi_write_buff_8267(unsigned short addr ,unsigned char* pbuff,unsigned int len){
	unsigned int i = 0;
	write_reg8(0x09,read_reg8(0x09)&0xfe);////CS level is low
	write_reg8(0x09,read_reg8(0x09)&0xfb);///enable output
	write_reg8(0x09,read_reg8(0x09)&0xf7);///enable write

	/***send addr***/
	write_reg8(0x08,(addr>>8)&0xff);/////high addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	write_reg8(0x08,addr&0xff);/////low addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status

	/***send write command:0x00***/
	write_reg8(0x08,0x00);/////0x80:read  0x00:write
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	/***send data***/
	for(i=0;i<len;i++){
		write_reg8(0x08,pbuff[i]);
		while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	}
	/***pull up CS***/
	write_reg8(0x09,read_reg8(0x09)|0x01);///CS level is high
}


void spi_read_buff_8267(unsigned short addr,unsigned char* pbuff,unsigned int len){
	unsigned int i = 0;
	unsigned char temp = 0;

	write_reg8(0x09,read_reg8(0x09)&0xfe);////CS level is low
	write_reg8(0x09,read_reg8(0x09)&0xfb);///enable output

	/***send addr***/
	write_reg8(0x08,(addr>>8)&0xff);/////high addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	write_reg8(0x08,addr&0xff);/////low addr
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status

	/***send read command:0x80***/
	write_reg8(0x08,0x80);/////0x80:read  0x00:write
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status

	/***when the read_bit was set 1,you can read 0x800008 to take eight clock cycle***/
	write_reg8(0x09,read_reg8(0x09)|0x08);////set read_bit to 1
	temp = read_reg8(0x08);///first byte isn't useful data,only take 8 clock cycle
	while(read_reg8(0x09)&0x40);/////bit<6>is busy status

	/***send one byte data to read data***/
	for(i=0;i<len;i++){
		pbuff[i] = read_reg8(0x08);///take 8 clock cycles
		while(read_reg8(0x09)&0x40);/////bit<6>is busy status
	}
	/////pull up CS
	write_reg8(0x09,read_reg8(0x09)|0x01);///CS level is high
}
#endif
/////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////spp based on spi//////////////////////////////////////////
#if 1
//the top of Stack - 128 Bytes.(tx 64 Bytes,rx 64 Bytes)
//stack_top = 0x80c000,reserve 72*2 byte for spi buffer
u8 *spi_rx_buff = 0x80BF80;  //fe00~fe48 64 rx buff
u8 *spi_tx_buff = 0x80BFC0;	 //fe60~fea8 64 tx buff
#else
u8 spi_rx_buff[SPP_RX_BUFF_SIZE];
u8 spi_rx_buff[SPP_TX_BUFF_SIZE];
#endif


/**********************************************************************
 * LOCAL FUNCTIONS
 */
extern u8 notify_or_indicate_flg;
//handle:spp
extern u8  spp_handle_s2c;
extern u8  tbl_mac[];
u8 ble_module_flag;


///////////the code below is just for demonstration of the event callback only////////////
int event_handler(u32 h, u8 *para, int n)
{
	if((h&HCI_FLAG_EVENT_TLK_MODULE)!= 0)			//module event
	{
		switch((u8)(h&0xff))
		{
			case BLT_EV_FLAG_CONNECT:
			{
                task_connect();//connection parameters request!!!
				gpio_write (RED_LED, 1);//HIGH
			}
				break;
			case BLT_EV_FLAG_TERMINATE:
			{
				extern u8 ui_ota_is_working;
				if(ui_ota_is_working){//when OTA is processing,the ble disconnect,ota flg should clear!
					ui_ota_is_working = 0;
				}
				gpio_write(RED_LED, 0);//LOW
			}
				break;
			case BLT_EV_FLAG_SUSPEND_ENTER:
				break;
			default:
				break;
		}
	}
}

u8	host_push_status (u16 st, int n, u8 *p)
{
	u8 *pw = my_fifo_wptr(&hci_tx_fifo);

	if (!pw || n >= hci_tx_fifo.size){
		return -1;
	}

	u8 i = 0;
	st = ( st & 0x03FF) | 0x0400;//event ID
	pw[i++] = 0xFF;	//token
	pw[i++] = n+2;  //length
	pw[i++] = st;
	pw[i++] = st >> 8;

	if (n){
		memcpy (pw + i, p, n);//write data to staus buffer
	}
	my_fifo_next (&hci_tx_fifo);
	return 0;
}


//spp cmd internal process function
void spp_onModuleCmd(u8* p, int n)
{
	u8 backCode = SPP_RESULT_SUCCESS;
	u8 read_buf_num[4];
	spp_cmd_t *sppData = (spp_cmd_t *)p;

	switch (sppData->cmdID)
	{
		// set advertising interval: 01 ff 02 00 50 00: 80 *0.625ms
		case HC_SET_ADV_INTERVAL:
		{
			u32 advInterval;
			advInterval = *(u16 *)sppData->data;
			if(advInterval<0x20 || advInterval>0x4000)//20ms ~ 10.24s
				backCode = SPP_RESULT_FAILED;
			else{
				//before set adv interval, make sure module in the adv state
				if(blc_ll_getCurrentState()!=BLS_LINK_STATE_ADV)
					bls_ll_setAdvInterval(advInterval, advInterval);
			}
			host_push_status(HC_SET_ADV_INTERVAL,1,&backCode);
		}
			break;
        // notify data packet
		case HC_NOTIFY_DATA:
			backCode = bls_att_pushNotifyData(spp_handle_s2c, sppData->data, sppData->paramLen);
			host_push_status(HC_NOTIFY_DATA,1,&backCode);
		    break;
		// enable/disable advertising: 0a ff 01 00  01
		case HC_ENABLE_DISABLE_ADV: //enable or disable ble module advertising function
			if(sppData->data[0] && blc_ll_getCurrentState() == BLS_LINK_STATE_IDLE){
				bls_ll_setAdvEnable(1);
			}
			else if(!sppData->data[0] && blc_ll_getCurrentState() == BLS_LINK_STATE_ADV){
				bls_ll_setAdvEnable(0);
			}
			else if(sppData->data[0] && blc_ll_getCurrentState() == BLS_LINK_STATE_ADV){
				//do nothing
			}
			else
				backCode = SPP_RESULT_FAILED;

			host_push_status(HC_ENABLE_DISABLE_ADV,1,&backCode);

			break;
		// set advertising type: 0d ff 01 00  00
		case HC_SET_ADV_TYPE:
//			backCode = blt_set_advtype(sppData->data[0]);//API can Not be used
//			host_push_status(HC_SET_ADV_TYPE,1,&backCode);
			break;
		// set advertising direct address: 0e ff 07 00  00(public; 1 for random) 01 02 03 04 05 06
		case HC_SET_DIRECT_ADDR: //set ble module ADV direct address
		{
			if(sppData->paramLen == 7 && sppData->data[0]<2){
				blt_set_adv_addrtype(sppData->data,7); //copy direct address to table
			}
			else{
				backCode = 1;
			}
			host_push_status(HC_SET_DIRECT_ADDR,1,&backCode);//cmd complete, syn ack
		}
			break;
		// set advertising data: 02 ff 06 00 01 02 03 04 05 06
		case HC_SET_ADV_DATA: //set advertise data
		{	if(!sppData->paramLen || sppData->paramLen > ADV_MAX_DATA_LEN)
				backCode = SPP_RESULT_FAILED;
			else{
				bls_ll_setAdvData(sppData->data,sppData->paramLen);
			}
			host_push_status(HC_SET_ADV_DATA,1,&backCode);
		}
			break;
		// restart module: 18 ff 00 00
		case HC_RESTART:
			write_reg8(0x0602, 0x88);//restart 8269 to run ram code
			break;

		case HC_RESET_MODULE_CONF://reset config para, now JUST restart the 8269 EVK
			write_reg8(0x0602, 0x88);//restart 8269 to run ram code
			break;

		case HC_GET_MODULE_MAC:
			host_push_status(HC_GET_MODULE_MAC,6,tbl_mac);
			break;
		// get module current work state: 16 ff 00 00
		case HC_GET_MODULE_STATE:
			backCode = blc_ll_getCurrentState();
			host_push_status(HC_GET_MODULE_STATE,1,&backCode);
			break;
		// set device name: 13 ff 0a 00  01 02 03 04 05 06 07 08 09 0a
		case HC_SET_DEVIVE_NAME:
		{
			if(!sppData->paramLen || sppData->paramLen > MAX_DEV_NAME_LEN)
				backCode = SPP_RESULT_FAILED;
			else{
				bls_att_setDeviceName(sppData->data,sppData->paramLen);
			}
			host_push_status(HC_SET_DEVIVE_NAME,1,&backCode);
		}
			break;
		// get connection parameter: 14 ff 00 00		,returrn interval(4) latency(2) timeout(4)
		case HC_GET_CONNECTION_PARAM:
		{
			if(blc_ll_getCurrentState()!=BLS_LINK_STATE_CONN){
				backCode = SPP_GET_OR_SETPARA_FAILED_NOCONNECTION;//0x03 connection not established
				host_push_status(HC_GET_CONNECTION_PARAM,1,&backCode);//busy
			}
			else{
				u8 rspData[10];
				blt_get_conn_para(rspData);
				host_push_status(HC_GET_CONNECTION_PARAM,10,(u8 *)rspData);//cmd complete, syn ack
			}
		}
			break;
		// set connection parameter: 15 ff 08 00 a0 00 a2 00 00 00 2c 01 (min, max, latency, timeout)
		case HC_SET_CONNECTION_PARAM:
			if(blc_ll_getCurrentState()!=BLS_LINK_STATE_CONN){
				backCode = SPP_GET_OR_SETPARA_FAILED_NOCONNECTION;
				host_push_status(HC_SET_CONNECTION_PARAM,1,&backCode);
			}
			else {
				bls_l2cap_requestConnParamUpdate(  sppData->data[0]|sppData->data[1]<<8, \
						                           sppData->data[2]|sppData->data[3]<<8, \
						                           sppData->data[4]|sppData->data[5]<<8, \
						                           sppData->data[6]|sppData->data[7]<<8);
				host_push_status(HC_SET_CONNECTION_PARAM,1,&backCode);
			}
			break;
	    // get module available data buffer: 0c ff 00  00
		case HC_GET_BUFFER_NUM:
			read_buf_num[0] = bls_hci_le_readBufferSize_cmd((u8 *)(read_buf_num));
			read_buf_num[1] = read_buf_num[2];
			host_push_status(HC_GET_BUFFER_NUM,2,read_buf_num);

			break;
		// terminate connection: 17 ff 00 00
		case HC_TERMINATE_CONNECTION:
			(blc_ll_getCurrentState() == BLS_LINK_STATE_CONN)? bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN):(backCode = 1);
			host_push_status(HC_TERMINATE_CONNECTION,1,&backCode);
			break;

		default:
			break;

	}
}

