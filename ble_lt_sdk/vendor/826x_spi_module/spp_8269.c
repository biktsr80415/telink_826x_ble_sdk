/*
 * spp_8269.c
 *
 *  Created on: 2016-9-1
 *      Author: yafei.tu
 */
#include "../tl_common.h"
#include "spp_8269.h"
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
u8 *spp_rx_buff = 0x80Bf80;
u8 *spp_tx_buff = 0x80Bfc0;

#else  //�����������ַ����ʱdebug���������ʽ

u8 spp_rx_buff[SPP_RX_BUFF_SIZE];
u8 spp_tx_buff[SPP_TX_BUFF_SIZE];

#endif

//status buffer used to buffer send back data packets to SPI master
u8 host_st[HOST_BUFF_NUM][SPP_TX_BUFF_SIZE];

u8 host_st_wptr = 0;
u8 host_st_rptr = 0;
u8 ble_module_flag;

u8 host_dat[HOST_BUFF_NUM][SPP_RX_BUFF_SIZE];//4buffers to buffer SPI packets
u8 host_dat_wptr = 0;
u8 host_dat_rptr = 0;

u8 p_module_write[64];//data packets received from BLE master

/**********************************************************************
 * LOCAL FUNCTIONS
 */
extern u8 notify_or_indicate_flg;
//handle:spp
extern u8  spp_handle_s2c;
extern u8  tbl_mac[];



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
				ble_module_flag |= FLAG_NOTIFY_STATE_CHANGE;
			}
				break;
			case BLT_EV_FLAG_TERMINATE:
			{
				extern u8 ui_ota_is_working;
				if(ui_ota_is_working){//when OTA is processing,the ble disconnect,ota flg should clear!
					ui_ota_is_working = 0;
				}
				gpio_write(RED_LED, 0);//LOW
				ble_module_flag |= FLAG_NOTIFY_STATE_CHANGE;
			}
				break;
			case BLT_EV_FLAG_SUSPEND_ENTER:
				break;
			default:
				break;
		}
	}
}

void spp_dataHandler(u8 *pData, u8 len){// notify/indicate the Master like mobile phone!
	if(notify_or_indicate_flg){
		bls_att_pushNotifyData(spp_handle_s2c, pData, len);
	}
	else{
		bls_att_pushIndicateData(spp_handle_s2c, pData, len);
	}
}


void	host_next_status (void)
{
	if (host_st_wptr == host_st_rptr)			//status buffer empty, clear GPIO flag
	{
		SPI_MODULE_DATE_FINISH;
		*(u32 *)spp_tx_buff = 0; //reset command buffer, if master read zero data, ignore the event
		ble_module_flag &= ~FLAG_HOST_STATUS_BUSY;
	}
	else
	{
		u8 *p = host_st[host_st_rptr++ & (HOST_BUFF_NUM - 1)];
		memcpy (spp_tx_buff, p, p[1] + 2);
	}
}


u8 tst2;// for test
u8	host_push_status (u16 st, int n, u8 *p)
{
	if( ((host_st_wptr - host_st_rptr) & (HOST_BUFF_NUM*2 - 1)) >= HOST_BUFF_NUM )
	{
		return 1;
	}

	u8 i = 0;
	/////////////////////////////////////////////////////////////////
	u8 *pw = host_st[host_st_wptr++ & (HOST_BUFF_NUM - 1)];
	if (n > 20)
	{
		n = 20;
	}

	st = ( st & 0x03FF) | 0x0400;//event ID

	pw[i++] = 0xFF;	//token

	pw[i++] = n+2;  //length

	pw[i++] = st;
	pw[i++] = st >> 8;

	if(st == 0x0731){//data receive, add handle
		pw[i++] = 0x01;
		pw[i++] = 0x00;
		pw[1] = n+4;
	}

	if (n)
	{
		memcpy (pw + i, p, n);//write data to staus buffer
	}

	if (!(ble_module_flag & FLAG_HOST_STATUS_BUSY))
	{
		//led_onoff(WHITE_LED, (++tst2) % 2 ? 1 : 0);
		host_next_status();
		ble_module_flag |= FLAG_HOST_STATUS_BUSY;//rise busy flag
		SPI_MODULE_DATE_READY;



	}
	return 0;
}

/********************************************************
*
*	@brief	Push the coarse PDU received from SPI master to BLE master device. Responsible for abstract the
*			data payload from the frame. The packets send from SPI master follow this form:
*			|0			1	|	1		2	| 3	.....	n |
*			 *Command ID*	  *param Len n*	  *PayLoad*
*
*	@param	*p:	address of the SPI mapping buffer
*
*	@return	0: send success
*			else: send error.
*/
u8	host_push_data (u8 *p)
{
	int num = (host_dat_wptr - host_dat_rptr) & 31;//11111
	if(bls_ll_getCurrentState()!=BLS_LINK_STATE_CONN){
		return DATA_SEND_FAILED_NOCONNECTION;
	}

	if (num >= HOST_BUFF_NUM)
	{
		return DATA_SEND_FAILED_BUSY;
	}
	if (p[2] > NOTIFY_MAXIMUM_DATA_LEN)
	{
		return DATA_SEND_FAILED_TXLEN;
	}
	u8 *pw = host_dat[host_dat_wptr++ & (HOST_BUFF_NUM - 1)];
	*pw = p[2];
	memcpy (pw+1, p + 4, p[2]);
	return 0;
}

//spp cmd internal process function

/*********************************************************************
 * @fn      spp_onModuleCmd
 *
 * @brief   Module command handler.
 *
 * @param   None
 *
 * @return  None
 */
void spp_onModuleCmd(void)
{

	u8 backCode = SPP_RESULT_SUCCESS;
	u8 read_buf_num[4];
	spp_cmd_t *sppData = (spp_cmd_t *)spp_rx_buff;

	switch (sppData->cmdID)
	{
		case HC_ACK:
			host_next_status ();
			break;
		// set advertising interval: 01 ff 02 00 50 00: 80 *0.625ms
		case HC_SET_ADV_INTERVAL:
		{
			u32 advInterval;
			advInterval = *(u16 *)sppData->data;
			if(advInterval<0x20 || advInterval>0x4000)//20ms ~ 10.24s
				backCode = SPP_RESULT_FAILED;
			else{
				//before set adv interval, make sure module in the adv state
				if(bls_ll_getCurrentState()!=BLS_LINK_STATE_ADV)
					bls_ll_setAdvInterval(advInterval, advInterval);
			}
			host_push_status(HC_SET_ADV_INTERVAL,1,&backCode);
		}
			break;

		case HC_NOTIFY_DATA: //data packet
			backCode = host_push_data(spp_rx_buff);
		    host_push_status(HC_NOTIFY_DATA,1,&backCode);

		    break;
		// enable/disable advertising: 0a ff 01 00  01
		case HC_ENABLE_DISABLE_ADV: //enable or disable ble module advertising function
			if(sppData->data[0] && bls_ll_getCurrentState() == BLS_LINK_STATE_IDLE){
				bls_ll_setAdvEnable(1);
				ble_module_flag |= FLAG_NOTIFY_STATE_CHANGE;
			}
			else if(!sppData->data[0] && bls_ll_getCurrentState() == BLS_LINK_STATE_ADV){
				bls_ll_setAdvEnable(0);
				ble_module_flag |= FLAG_NOTIFY_STATE_CHANGE;
			}
			else if(sppData->data[0] && bls_ll_getCurrentState() == BLS_LINK_STATE_ADV){
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
			backCode = bls_ll_getCurrentState();
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
			if(bls_ll_getCurrentState()!=BLS_LINK_STATE_CONN){
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
			if(bls_ll_getCurrentState()!=BLS_LINK_STATE_CONN){
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
			(bls_ll_getCurrentState() == BLS_LINK_STATE_CONN)? bls_ll_terminateConnection(HCI_ERR_REMOTE_USER_TERM_CONN):(backCode = 1);
			host_push_status(HC_TERMINATE_CONNECTION,1,&backCode);
			break;

		default:
			break;

	}
}

u8 tst1;
volatile u8 current_state;
void module_send_stateEvent(void ){

	ble_module_flag &= ~FLAG_NOTIFY_STATE_CHANGE;//clear state change send flag
	u8 sendBack[3]= {0,0,0};
	current_state = bls_ll_getCurrentState();
	u16 module_handle = (current_state == BLS_LINK_STATE_CONN)?0x0001:0xffff;
	sendBack[0] = module_handle;
	sendBack[1] = module_handle>>8;
	sendBack[2] = current_state;
	host_push_status(EV_STATE_CHANGE,3,sendBack);

}

u8 tst;
u8 spp_task_finished_flg = 0;//
void task_host(void){
	if(reg_irq_src & FLD_IRQ_GPIO_RISC2_EN ){//&& gpio_read(GPIO_PB4)){

        reg_irq_src = FLD_IRQ_GPIO_RISC2_EN;  //clear irq src

        if(*(u16*)(spp_rx_buff) != 0){//MSPI write irq
			//led_onoff(RED_LED, (++tst) % 2 ? 1 : 0);
			spp_onModuleCmd();
			spp_task_finished_flg = 1;//spp tsk start
			*(u16*)(spp_rx_buff) = 0;
		}
		else{//MSPI read irq
			if(*(u16*)(spp_tx_buff)){
				ble_module_flag &= ~FLAG_HOST_STATUS_BUSY;//clear busy flag
				host_next_status (); // ACK command, go to next status
				spp_task_finished_flg = 0;//spp tsk finished
			}
		}

	}
	////////////////////// data received from master ///////////////////////
	if (p_module_write[0])
	{
#if PRINT_DEBUG_INFO
		printf("data received from app, rec.data.len:0x%1x\n",(unsigned)(u8 )p_module_write[0]);
#endif
		host_push_status (EV_DATA_REC, p_module_write[0], p_module_write+1);
		p_module_write[0] = 0;
	}
	////////////////////// send data to master /////////////////////////////////
	if (bls_ll_getTxFifoNumber() < 3 && (host_dat_wptr != host_dat_rptr)) //data
	{
		u8 *ps = host_dat[host_dat_rptr++ & (HOST_BUFF_NUM - 1)];
		bls_att_pushNotifyData (spp_handle_s2c, ps + 1, ps[0]);
#if PRINT_DEBUG_INFO
	printf("packet send to phone\n");
#endif
	}
	////////////////////// module states change ////////////////////////////
	if(ble_module_flag & FLAG_NOTIFY_STATE_CHANGE){	//track module states
		module_send_stateEvent();
	}

}
