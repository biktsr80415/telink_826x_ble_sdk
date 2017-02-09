/*
 * dongle.c
 *
 *  Created on: 2016-9-13
 *      Author: Administrator
 */
#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../vendor/common/myprintf.h"

#if(__PROJECT_8267_SPI_MASTER__)
#define			DEBUG_SPI				0
#define			EP_BO					5
#define			reg_host_if				REG_ADDR8(0x8008)
#define			reg_host_i2c_id			REG_ADDR8(0x8009)
#define			HOST_IF_SET_FLAG		BIT(7)
#define			HOST_IF_POLL_FLAG		BIT(6)
#define			HOST_IF_SPI				0
#define			HOST_IF_I2C				1
#define			HOST_IF_UART			2
#define			HOST_IF_USB				3


u8	host_if_polling;

u8	buff_command[64];
u8	buff_event[72];

const TBLCMDSET tbl_sys_id[] = {
	0x013b,		0x20,		TCMD_UNDER_BOTH | TCMD_WRITE,	//endpoint 8 buffer size: 32*8=256
	0x013c,		0x40,		TCMD_UNDER_BOTH | TCMD_WRITE,	//threshold for uart mode
	0x013d,		0x00,		TCMD_UNDER_BOTH | TCMD_WRITE,	//disable FIFO, no DMA

	0x074f,		0x01,		TCMD_UNDER_BOTH | TCMD_WRITE,	//enable system timer
	0x0620,		0x01,		TCMD_UNDER_BOTH | TCMD_WRITE,	//enable timer 0

	0x0074,		0x53,		TCMD_UNDER_BOTH | TCMD_WRITE,	//id enable
	0x007e,		0xbd,		TCMD_UNDER_BOTH | TCMD_WRITE,	//id low byte
	0x007f,		0x82,		TCMD_UNDER_BOTH | TCMD_WRITE,	//id high byte
	0x0074,		0x00,		TCMD_UNDER_BOTH | TCMD_WRITE,	//id enable
};

void  usb_host_init(void) {
	LoadTblCmdSet(tbl_sys_id, sizeof(tbl_sys_id) / sizeof(TBLCMDSET));

	usb_dp_pullup_en (1);
	while (!reg_usb_host_conn);
	reg_usb_ep_ctrl(EP_BO) = BIT(0);

#if (SLAVE_SPI_PM_ENABLE)
	gpio_set_func(MSPI_PM_WAKEUP_PIN,AS_GPIO);
	gpio_set_output_en (MSPI_PM_WAKEUP_PIN, 1);
	gpio_write (MSPI_PM_WAKEUP_PIN, 0);
#endif

#if DEBUG_SPI
	host_spi_init (127);			//spi 125K
	reg_spi_inv_clk = BIT (1); 		//trailing edge
#endif
}

///////////////	Host Interface Write	//////////////////////////////////////////////////
int send_status (u8 *p, int n)
{
	reg_usb_ep8_ptr = 0;
	for (int i=0; i<n; i++)
	{
		reg_usb_ep8_dat = *p ++;
	}
	reg_usb_ep8_ctrl = BIT(0);
	return n;
}

int host_usb_write (u8 *p, int n)
{
	send_status (p, n);
	return n;
}
u8 tst;
int host_write() {
	static u32 no_cmd;
	u16 cmdID;
	no_cmd++;
	int n = reg_usb_ep_ptr (EP_BO);
	reg_usb_ep_ptr(EP_BO) = 0;
	for (int i=0; i<n; i++)
	{
		buff_command[i] = reg_usb_ep_dat(EP_BO);
	}

	cmdID = *(u16 *)buff_command;

#if DEBUG_SPI
		host_spi_readwrite (buff_command, buff_event, n);
		send_status (buff_event, n);
		return n;
#endif

	if (reg_host_if == HOST_IF_USB)
	{
		return  host_usb_write (buff_command, n);
	}
	else if (reg_host_if == HOST_IF_SPI)
	{
		//spi_write_buff_8267(0xff80, buff_command, n); //same as host_spi_write function
		return  host_spi_write (buff_command, n);
	}

	return 0;
}

u32 tickI2CRx0 = 0;
void proc_host() {
	static u32	tick_bulk_out, tick_event;
	//////////// host interface change  //////////////////////////////////
	if (!DEBUG_SPI && HOST_IF_SET_FLAG & reg_host_if) {
		//set_host_if ();
		host_if_polling = reg_host_if & HOST_IF_POLL_FLAG;
		reg_host_if &= 15;
		if (reg_host_if == HOST_IF_SPI)
		{
			//host_spi_init (8);
		}
	}
	//////////// get command from bulk-out endpoint //////////////////////
	else if (reg_usb_irq & BIT(EP_BO)) {
		host_write ();
		reg_usb_irq = BIT(EP_BO);
		reg_usb_ep_ctrl(EP_BO) = BIT(0);
	}
	else if (reg_usb_ep_ctrl(EP_BO) & FLD_USB_EP_BUSY)
	{
		tick_bulk_out = clock_time ();
	}
	else if (clock_time_exceed (tick_bulk_out, 1000000))
	{
		reg_usb_ep_ctrl(EP_BO) = BIT(0);
	}

	//////////// get status from slave: interrupt or polling //////////////////////
#if (!DEBUG_SPI)
	if (host_if_polling || !gpio_read(MSPI_RX_NOTIFY_PIN)){
		int n = 0;

		if (reg_host_if == HOST_IF_SPI){
			n = host_spi_read((u8*)buff_event);
            //spi_read_buff_8267(0xffc0, buff_event, 64);// same as host_spi_read function
            //n = buff_event[1];

			if (n>2) {//only n > 2 .
				send_status (buff_event, n);
			}
		}

		if (n > 2 || !host_if_polling) {
			gpio_write (BLUE_LED, 1);
			tick_event = clock_time ();
		}
	}

#endif

	if (clock_time_exceed (tick_event, 200000))
	{
		gpio_write (BLUE_LED, 0);		//turn off led
	}
}


#endif

