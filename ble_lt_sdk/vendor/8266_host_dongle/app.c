#include "../../proj/tl_common.h"
#if 1
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/ll/ll_whitelist.h"
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

//////////////////////// UART variables ///////////////////////////
MYFIFO_INIT(uart_txfifo, 	128, 4);
MYFIFO_INIT(uart_rxfifo, 	128, 4);

#define MY_UART_DATA_LEN    128      // data max 252
typedef struct{
    unsigned int len;        // data max 252
    unsigned char data[MY_UART_DATA_LEN];
}myuart_data_t;


//////////////////////// USB variables ///////////////////////////
MYFIFO_INIT(usb_txfifo, 	128, 4);



u8		spp_test_en = 0;
u32		spp_num = 0;
u16		spp_conn = 0;
u16		spp_handle = 0;
u32		spp_err = 0;
//------------------- spp test ----------------------------------------------
void app_send_spp_status ()
{
	u8 dat[16];

	dat[0] = 0xff;
	dat[1] = 10;
	dat[2] = 0xe2;
	dat[3] = 0x07;
	memcpy (dat + 4, &spp_num, 4);
	memcpy (dat + 8, &spp_err, 4);
	my_fifo_push (&usb_txfifo, dat, 12);
}
//////////////////////////////////// SPP test code ///////////////////////////////
int	 spp_write = 0;
void spp_test_write ()
{
	if (!spp_write && spp_num && spp_handle && my_fifo_wptr (&uart_txfifo))
	{
		u8		dat[32];
		u8 *p = dat;
		*p++ = 0x1c;				//notify
		*p++ = 0xff;
		*p++ = 22;
		*p++ = 0;
		*p++ = spp_handle;
		*p++ = 0;
		memcpy (p, &spp_num, 4);
		for (int i=4; i<20; i++)
		{
			p[i] = p[0] + i;
		}

		if (my_fifo_push (&uart_txfifo, dat, 20 + (p - dat)) == 0)
		{
			spp_num--;
			spp_write = 1;
			if ((spp_num & 0xff) == 0)
			{
				app_send_spp_status ();
			}
		}
	}
}
void	spp_test_read (u8 *p, int n)
{
	static u32 spp_err_st;
	static u32 spp_read = 0;
	u32 seq;
	memcpy (&seq, p, 4);
	if (spp_read != seq)
	{
		spp_err++;
	}
	else
	{
		for (int i=4; i<n; i++)
		{
			if ((u8)(p[0] + i) != p[i])
			{
				spp_err++;
				spp_err_st += BIT(16);
				break;
			}
		}
	}
	spp_read = seq - 1;
	if ((spp_read & 0xff) == 0)
	{
		app_send_spp_status ();
	}
	if (0 && spp_err > 1)
	{
		gpio_set_input_en(GPIO_URX, 0);
		irq_disable ();
		while (1);
	}
}

//--------------------	USB -------------------------------------------------
/////////////////////////////////////////////////////
// write flash memory through swire
/////////////////////////////////////////////////////
#define			USB_ENDPOINT_BULK_IN			8
#define			USB_ENDPOINT_BULK_OUT			5
#define			USB_ENDPOINT_BULK_OUT_FLAG		(1 << (USB_ENDPOINT_BULK_OUT & 7))


static u32 myusb_endpoint_busy(u32 ep) {
	write_reg8 (0x80013d, 0);
	return read_reg8 (0x800120 + (ep&7)) & 1;
}

// 0: last packet; -1: busy
int myusb_bulk_in_packet (u8 *p, int n)
{
	static u32 offset = 0;

	if( myusb_endpoint_busy(USB_ENDPOINT_BULK_IN))
		return -1;


	if (n <= 64)
	{
		offset = 0;
	}
	else
	{
		n -= offset;
	}

	//reset pointer to 0
	write_reg8 (0x800110 + (USB_ENDPOINT_BULK_IN & 7), 0);
	for (int i=0; i<(n >= 64 ? 64 : n); i++)
	{
		write_reg8 (0x800118 + (USB_ENDPOINT_BULK_IN & 7), p[offset + i]);
	}
	write_reg8 (0x800120 + (USB_ENDPOINT_BULK_IN & 7), 1);		//ACK

	offset = n <= 64 ? 0 : offset + 64;

	return offset;
}

int myusb_bulk_out_get_data (u8 *p)
{
	if (read_reg8 (0x800139) & USB_ENDPOINT_BULK_OUT_FLAG)
	{
		//clear interrupt flag
		write_reg8 (0x800139, USB_ENDPOINT_BULK_OUT_FLAG);

		// read data
		int n = read_reg8 (0x800110 + (USB_ENDPOINT_BULK_OUT & 7));
		write_reg8 (0x800110 + (USB_ENDPOINT_BULK_OUT & 7), 0);
		for (int i=0; i<n; i++)
		{
			p[i] = read_reg8 (0x800118 + (USB_ENDPOINT_BULK_OUT & 7));
		}
		write_reg8 (0x800120 + (USB_ENDPOINT_BULK_OUT & 7), 1);		//ACK

		return n;
	}
	return 0;
}


int app_data_from_usb (void)
{
	u8 buff[128];
	int n = myusb_bulk_out_get_data (buff);
	if (n)
	{
		if (buff[0] == 0xe0 && buff[1] == 0xff)		//set mac address filter
		{
			//dev_rssi_th = buff[4];
			//memcpy (dev_mac, buff + 5, 12);
		}
		else if (buff[0] == 0xe1 && buff[1] == 0xff)		//spp test data from host to slave: write_cmd
		{
			memcpy (&spp_conn, buff + 4, 2);
			spp_test_en = spp_conn;
			spp_handle = spp_conn >> 8;
			memcpy (&spp_num, buff + 6, 4);
			spp_err = 0;

		}
		else if (buff[0] == 0xe2 && buff[1] == 0xff)		//spp test status: spp_num & spp_err
		{
			app_send_spp_status ();
		}
		else	// send to UART
		{
			my_fifo_push (&uart_txfifo, buff, n);
		}
	}
	return 0;
}

int app_data_to_usb ()
{
	u8 *p = my_fifo_get (&usb_txfifo);

	if(p && !myusb_bulk_in_packet (p + 2, p[0] + p[1] * 256)){
		my_fifo_pop (&usb_txfifo);
	}

	return 0;
}


//--------------------	UART -------------------------------------------------
int app_data_to_uart ()
{
	if (!uart_tx_is_busy ())
	{
		GPIO_WAKEUP_MODULE_RELEASE;	//release module wake-up signal when TX done
		u8 *p = my_fifo_get (&uart_txfifo);
		if (!p)
		{
			return 0;
		}

		myuart_data_t T_txdata_buf;
		T_txdata_buf.len = p[0]+p[1]*256 ;
		memcpy(&T_txdata_buf.data, p + 2, p[0]+p[1]*256);

		int module_st = gpio_read(GPIO_WAKEUP_MODULE);
		GPIO_WAKEUP_MODULE_HIGH;
		if (!module_st)
		{
			sleep_us (3000);
		}

#if (__PROJECT_8266_HOST_DONGLE__)
		if (uart_Send_kma((u8 *)(&T_txdata_buf)))
#else
		if (uart_Send((u8 *)(&T_txdata_buf)))
#endif
		{
			my_fifo_pop (&uart_txfifo);
		}
	}
	return 0;
}

int app_data_from_uart (void)
{
	u8 *p = my_fifo_get (&uart_rxfifo);
	if (p)
	{
		if (spp_test_en)
		{
			if (p[0] == 0xff && p[2] == 0xa0 && p[3] == 0x07)
			{
				spp_test_read (p + 7, p[1] - 5);
			}
			//0b ff 14 00
			else if (p[0] == 0x0b && p[1] == 0xff)
			{
				spp_test_read (p + 4, p[2]);
			}
			else if (p[0] == 0xff && p[2] == 0x1c && p[3] == 0x07)
			{
				if (p[4])		//data sent fail
				{
					spp_num ++;
				}
				spp_write = 0;
			}
		}

		if (!spp_test_en || !(p[0] == 0xff && p[2] == 0x1c && p[3] == 0x07 && spp_num != 1))
		{
			my_fifo_push (&usb_txfifo, &p[4], p[0] + p[1] * 256);
		}
		my_fifo_pop (&uart_rxfifo);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
void main_loop ()
{
	static u32 tick_loop;
	tick_loop ++;

	/// proc uart data ///
	app_data_to_uart();
	app_data_from_uart();

	/// proc usb data ///
	app_data_to_usb();
	app_data_from_usb();

	//spp_test_write ();

	/// bulk out endpoint buffer ready ///
	static u32 tick_bo;
	if (REG_ADDR8(0x120 + (USB_ENDPOINT_BULK_OUT & 7)) & BIT(0))
	{
		tick_bo = clock_time ();
	}
	else if (clock_time_exceed (tick_bo, 200000))
	{
		REG_ADDR8(0x120 + (USB_ENDPOINT_BULK_OUT & 7)) = BIT(0);
	}
}

void user_init()
{
	//set UAB ID
	usb_log_init ();
	REG_ADDR8(0x74) = 0x53;
	REG_ADDR16(0x7e) = 0x08d1;
	REG_ADDR8(0x74) = 0x00;
	reg_usb_ep_max_size = (256 >> 3);
	usb_dp_pullup_en (1);  //open USB enum

#if __PROJECT_8266_HOST_DONGLE__
	gpio_set_func(GPIO_UTX, AS_UART);
	gpio_set_func(GPIO_URX, AS_UART);
	gpio_set_input_en(GPIO_UTX, 1);
	gpio_set_input_en(GPIO_URX, 1);
	gpio_write (GPIO_UTX, 1);			//pull-high RX to avoid mis-trig by floating signal
	gpio_write (GPIO_URX, 1);			//pull-high RX to avoid mis-trig by floating signal
#else
	gpio_set_input_en(GPIO_PB2, 1);
	gpio_set_input_en(GPIO_PB3, 1);
	gpio_setup_up_down_resistor(GPIO_PB2, PM_PIN_PULLUP_1M);
	gpio_setup_up_down_resistor(GPIO_PB3, PM_PIN_PULLUP_1M);
	uart_io_init(UART_GPIO_8267_PB2_PB3);
#endif
	reg_dma_rx_rdy0 = FLD_DMA_UART_RX | FLD_DMA_UART_TX; //clear uart rx/tx status

	if(CLOCK_SYS_CLOCK_HZ == 16000000){
		CLK16M_UART115200;
	}
	else if(CLOCK_SYS_CLOCK_HZ == 32000000){
		CLK32M_UART115200;
	}

	uart_BuffInit(uart_rxfifo_b, uart_rxfifo.size, uart_txfifo_b);
}
#endif
