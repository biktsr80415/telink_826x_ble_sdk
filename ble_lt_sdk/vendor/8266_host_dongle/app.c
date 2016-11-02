#include "../../proj/tl_common.h"
#if 1
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ble_ll.h"
#include "../../proj_lib/ble/ll_whitelist.h"
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

int		dongle_pairing_enable;
u8		spp_test_en = 0;
u32		spp_num = 0;
u16		spp_conn = 0;
u16		spp_handle = 0;
u32		spp_err = 0;

///////////////////////////////////////////
// TX FIFO
///////////////////////////////////////////
#define			HCI_BUFFER_TX_SIZE		72
#define			HCI_BUFFER_NUM			8
u8				app_hci_buff[HCI_BUFFER_NUM][HCI_BUFFER_TX_SIZE] = {{0,},};
u8				app_hci_buff_wptr = 0;
u8				app_hci_buff_rptr = 0;

typedef	struct {
	u8		size;
	u8		num;
	u8		wptr;
	u8		rptr;
	u8*		p;
}	my_fifo_t;

u8			uart_buff[HCI_BUFFER_NUM][HCI_BUFFER_TX_SIZE] = {{0},};
u8			usb_buff[HCI_BUFFER_NUM][HCI_BUFFER_TX_SIZE] = {{0},};
my_fifo_t	uart_fifo = {HCI_BUFFER_TX_SIZE, HCI_BUFFER_NUM, 0, 0, uart_buff[0]};
my_fifo_t	usb_fifo  = {HCI_BUFFER_TX_SIZE, HCI_BUFFER_NUM, 0, 0, usb_buff[0]};

void my_fifo_init (my_fifo_t *f, u8 s, u8 n, u8 w, u8 r, u8 *p)
{
	f->size = s;
	f->num = n;
	f->wptr = w;
	f->rptr = r;
	f->p = p;
}

u8* my_fifo_wptr (my_fifo_t *f)
{
	if (((f->wptr - f->rptr) & 255) < f->num)
	{
		return f->p + (f->wptr & (f->num-1)) * f->size;
	}
	return 0;
}

int my_fifo_push (my_fifo_t *f, u8 *p, u8 n)
{
	if (((f->wptr - f->rptr) & 255) >= f->num)
	{
		return -1;
	}

	if (n >= f->size)
	{
		return -1;
	}
	u8 *pd = f->p + (f->wptr++ & (f->num-1)) * f->size;
	*pd++ = n;
	*pd++ = n >> 8;
	memcpy (pd, p, n);
	return 0;
}

void my_fifo_pop (my_fifo_t *f)
{
	f->rptr++;
}

u8 * my_fifo_get (my_fifo_t *f)
{
	if (f->rptr != f->wptr)
	{
		u8 *p = f->p + (f->rptr & (f->num-1)) * f->size;
		return p;
	}
	return 0;
}

//------------------- spp test ----------------------------------------------
void app_send_spp_status ()
{
	u8 dat[16];

	dat[0] = 0xff;
	dat[1] = 11;
	dat[2] = 0;
	dat[3] = 0xe2;
	dat[4] = 0x07;
	dat[5] = 0;
	memcpy (dat + 6, &spp_num, 4);
	memcpy (dat + 10, &spp_err, 4);
	my_fifo_push (&usb_fifo, dat, 14);
}

//////////////////////////////////// SPP test code ///////////////////////////////
void spp_test_write ()
{
	if (spp_num && spp_handle && my_fifo_wptr (&uart_fifo))
	{
		u8		dat[32];
		u8 *p = dat;
		*p++ = 0x0b;
		*p++ = 0xff;
		*p++ = 20;
		*p++ = 0;
		memcpy (p, &spp_num, 4);
		for (int i=4; i<20; i++)
		{
			p[i] = p[0] + i;
		}

		if (my_fifo_push (&uart_fifo, dat, 20 + (p - dat)) == 0)
		{
			spp_num--;

			if ((spp_num & 0xff) == 0)
			{
				app_send_spp_status ();
			}
		}
	}
}

void	spp_test_read (u8 *p, int n)
{
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
				break;
			}
		}
	}
	spp_read = seq - 1;
	if ((spp_read & 0xff) == 0)
	{
		app_send_spp_status ();
	}
}

//--------------------	usb -------------------------------------------------
int app_packet_from_usb (void)
{
	u8 buff[72];
	extern int usb_bulk_out_get_data (u8 *p);
	int n = usb_bulk_out_get_data (buff);
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
			my_fifo_push (&uart_fifo, buff, n);
		}
	}
	return 0;
}

int app_packet_to_usb ()
{
	u8 *p = my_fifo_get (&usb_fifo);
	extern int usb_bulk_in_72byte (u8 *p, int n);
	if (p && usb_bulk_in_72byte (p + 2, p[0] + p[1] * 256) == 0)
	{
		my_fifo_pop (&usb_fifo);
	}
	return 0;
}


//--------------------	UART -------------------------------------------------
int app_packet_to_uart ()
{
	u8 *p = my_fifo_get (&uart_fifo);
	if (p && !uart_tx_is_busy ())
	{
		memcpy(&T_txdata_buf.data, p + 2, p[0]+p[1]*256);
		T_txdata_buf.len = p[0]+p[1]*256 ;
#if( __TL_LIB_8266__ || MCU_CORE_TYPE == MCU_CORE_8266)
		if (uart_Send_kma((u8 *)(&T_txdata_buf)))
#else
		if (uart_Send((u8 *)(&T_txdata_buf)))
#endif
		{
			my_fifo_pop (&uart_fifo);
		}
	}
	return 0;
}

int app_packet_from_uart (void)
{
	if(rx_uart_w_index==rx_uart_r_index)  //rx buff empty
		return 0;

	u32 rx_len = T_rxdata_buf[rx_uart_w_index].len + 4 > sizeof(T_rxdata_user) ? sizeof(T_rxdata_user) : T_rxdata_buf[rx_uart_w_index].len + 4;
	memcpy(&T_rxdata_user, &T_rxdata_buf[rx_uart_w_index], rx_len);
	//memset(&T_rxdata_buf[rx_uart_w_index],0,sizeof(uart_data_t));
	if (rx_len > 4)
	{
		u8 *p = T_rxdata_user.data;
		if (spp_test_en)
		{
			if (p[0] == 0xff && p[2] == 0x31 && p[3] == 0x07)
			{
				spp_test_read (p + 4, p[1] - 2);
			}
			//0b ff 14 00
			else if (p[0] == 0x0b && p[1] == 0xff)
			{
				spp_test_read (p + 4, p[2]);
			}
		}
		else
		{
			my_fifo_push (&usb_fifo, T_rxdata_user.data, rx_len - 4);		//spp test data check
		}
	}
	rx_uart_w_index = (rx_uart_w_index + 1)&0x01;
	return 0;
}

/////////////////////////// led management /////////////////////
enum{
	LED_POWER_ON = 0,
	LED_AUDIO_ON,	//1
	LED_AUDIO_OFF,	//2
	LED_SHINE_SLOW, //3
	LED_SHINE_FAST, //4
	LED_SHINE_OTA, //5
};

const led_cfg_t led_cfg[] = {
	    {1000,    0,      1,      0x00,	 },    //power-on, 1s on
	    {100,	  0 ,	  0xff,	  0x02,  },    //audio on, long on
	    {0,	      100 ,   0xff,	  0x02,  },    //audio off, long off
	    {500,	  500 ,   3,	  0x04,	 },    //1Hz for 3 seconds
	    {250,	  250 ,   6,	  0x04,  },    //2Hz for 3 seconds
	    {125,	  125 ,   200,	  0x08,  },    //4Hz for 50 seconds
};

void app_led_en (int id, int en)
{
	id &= 7;
	en = !(LED_ON_LEVAL ^ en);
	if (id == 0)
	{
		gpio_write(GPIO_LED_RED, en);
	}
	else if (id == 1)
	{
		gpio_write(GPIO_LED_GREEN, en);
	}
	else if (id == 2)
	{
		gpio_write(GPIO_LED_BLUE, en);
	}
	else if (id == 3)
	{
		gpio_write(GPIO_LED_WHITE, en);
	}
}
//////////////////////////////////////////
// Service Discovery
//////////////////////////////////////////


//////////////////////////////////////////////////////////
// event call back
//////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
int main_idle_loop ()
{
	static u32 tick_loop;
	tick_loop ++;

	///////////////////////////////////// proc usb cmd from host /////////////////////
	extern usb_handle_irq (void);
	//usb_handle_irq();

	///////////////////////////////////// proc usb cmd from host /////////////////////
	app_packet_from_uart ();
	app_packet_to_uart ();

	app_packet_from_usb ();
	app_packet_to_usb ();

	spp_test_write ();

	///////////////////////////////// bulk out endpoint buffer ready ///////////////////////////////////////
	static u32 tick_bo;
	if (REG_ADDR8(0x125) & BIT(0))
	{
		tick_bo = clock_time ();
	}
	else if (clock_time_exceed (tick_bo, 200000))
	{
		REG_ADDR8(0x125) = BIT(0);

	}

	////////////////////////////////////// UI entry /////////////////////////////////
	device_led_process();

	dongle_pairing_enable = !gpio_read (SW1_GPIO);

	static u32 gpio2 = 0;
	u8 gpio =	!gpio_read (SW2_GPIO);
	if (gpio & !gpio2)
	{
		//smp_param_reset ();
	}
	gpio2 = gpio;
	return 0;
}

void main_loop ()
{
	main_idle_loop ();
}

///////////////////////////////////////////
#define	CLK32M_UART115200		uart_Init(19,13,1,1,NOCONTROL)

void user_init()
{
	//rf_customized_param_load();  //load customized freq_offset cap value and tp value

	//set UAB ID
	usb_log_init ();
	//REG_ADDR8(0x74) = 0x53;
	//REG_ADDR16(0x7e) = 0x08ee;
	//REG_ADDR8(0x74) = 0x00;
	//////////////// config USB ISO IN/OUT interrupt /////////////////
	//reg_usb_mask = BIT(7);			//audio in interrupt enable
	//reg_irq_mask |= FLD_IRQ_IRQ4_EN;
	reg_usb_ep6_buf_addr = 0x80;
	reg_usb_ep7_buf_addr = 0x60;
	reg_usb_ep_max_size = (256 >> 3);

	usb_dp_pullup_en (1);  //open USB enum

	/////////////////// keyboard drive/scan line configuration /////////
	rx_uart_r_index = 0;
	rx_uart_w_index = 0;
	gpio_set_func(GPIO_UTX, AS_UART);
	gpio_set_func(GPIO_URX, AS_UART);
	gpio_set_input_en(GPIO_UTX, 1);
	gpio_set_input_en(GPIO_URX, 1);
	CLK32M_UART115200;			//todo:change to certain configuration according to lib
	uart_BuffInit((u8 *)(&T_rxdata_buf), sizeof(T_rxdata_buf), (u8 *)(&T_txdata_buf));
	REG_ADDR8(0x526) = BIT(0) | BIT(1);//CLR irq source
	//while (1);
}

#endif
