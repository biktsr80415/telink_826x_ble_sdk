#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#if (__PROJECT_USB_HOST__)

extern void cpu_wakeup_init (void);
extern void proc_low_power (int);
extern void usb_cmd(unsigned int * buff_tx, int tx_count, unsigned int * buff_rx);

u32		m_loop;
unsigned int buff_tx[10];
unsigned int buff_rx[10];
unsigned int buff_ack[3];
u32		token_sof;


#define	USB_HS_NAK			0x5a
#define	USB_HS_ACK			0xd2

const unsigned char tbl_token_ack[] =	{0x80, 0xd2};
const unsigned char tbl_token_sof[] =	{0x80, 0xa5, 0x01, 0xe8};
const unsigned char tbl_token_in[] =	{0x80, 0x69, 0x00, 0x10};
const unsigned char tbl_token_setup[] = {0x80, 0x2d, 0x00, 0x10};
const unsigned char tbl_set_address[] = {0x80, 0x2d, 0x00, 0x10, 0x00, // setup-eop
		0x80, 0xc3, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xeb, 0x25};
const unsigned char tbl_set_configuration[] = {0x80, 0x2d, 0x00, 0x10, 0x00, // setup-eop
		0x80, 0xc3, 0x00, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x27, 0x25};
const unsigned char tbl_get_device_desc[] = {0x80, 0x2d, 0x00, 0x10, 0x00, // setup-eop
		0x80, 0xc3, 0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x80, 0x00, 0x8d, 0x94};
const unsigned char tbl_pc_read_reg[] = {0x80, 0x2d, 0x00, 0x10, 0x00, // setup-eop
		0x80, 0xc3, 0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x80, 0x00, 0x8d, 0x94};

const unsigned char tbl_out_dat1[] = {0x80, 0xe1, 0x00, 0x10, 0x00, // out-eop-data1
		0x80, 0x4b, 0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x80, 0x00, 0x8d, 0x94};

const unsigned char tbl_out_dat0[] = {0x80, 0xe1, 0x00, 0x10, 0x00, // out-eop-data0
		0x80, 0xc3, 0x80, 0x06, 0x00, 0x01, 0x00, 0x00, 0x80, 0x00, 0x8d, 0x94};

static inline void enable_ack_after_data (int en)
{
	reg_gpio_pd_ie	= en ? BIT(1) | BIT(0) : BIT(1);
}

int usb_get_tx_dat (unsigned char *ps, int n, unsigned int *pd)
{
	int i, j;
	int len = 0;
	int bit1s = 0;
	unsigned char dpdm = 0x02;
	unsigned char *po = (unsigned char *) pd;
	int pos = 0;
	*po = 0;
	for (i=0; i<=n; i++) {
		unsigned char s = ps[i];
		for (j=0x01; j<0x88; j=j<<1) {
			if (i==n && j==0x01) {
				j = 0x80;
				s = 0xff;
				dpdm = 0;		// insert SE0
			}
			if (s & j) {
				bit1s++;		// continuous 1
			}
			else {
				bit1s = 0;
				dpdm = dpdm ^ 0x03; 
			}
			*po |= dpdm << pos; 
			len ++;
			if (pos < 6)
				pos += 2;
			else {
				pos = 0;
				po++;
				*po = 0;
			}
			if (bit1s == 6) { //bit stuffing enable
				dpdm = dpdm ^ 0x03; 
				bit1s = 0;
				*po |= dpdm << pos; 
				len ++;
				if (pos < 6)
					pos += 2;
				else {
					pos = 0;
					po++;
					*po = 0;
				}
			}
		}
	}
	if (len > 40) {		//setup/out token + DATA
		po = (unsigned char *) pd ;
		po[8] = 0xa0;		//SE0_SE0_J_J
		po[9] = 0xaa;		//J_J_J_J
	}
	return len;
}

void usb_tx_rx (u32 * tx, int n) {
	for (int i=0; i<10; i++)
		buff_rx[i] = 0;
	usb_cmd(tx, n, buff_rx);
}

int usb_get_rx_dat ()
{
	int i, j;
	int len = 0;
	int bit = 0;
	int bit1s = 0;
	unsigned char *pi = (unsigned char *) buff_rx;
	unsigned char *po = (unsigned char *) buff_rx;
	int pos = 0;
	for (i=0; i<=40; i++) {
		unsigned char s = pi[i];
		for (j=0x01; j<0x88; j=j<<1) {
			int b = s & 1;
			s = s >> 1;
			if (b ^ bit) {		// 0	
				if (bit1s != 7) {
					*po &= ~(1<<pos);
					pos++;	
					len ++;
				}
				bit1s = 0;		
			}
			else {
				bit1s++ ;
				*po |= (1<<pos);
				pos++;	
				len ++;
			}
			if (bit1s > 7) {  // EOP
				len -= 11;
				return len;
			}
			bit = b;
			if (pos >= 8) {
				pos = 0;
				po++;
			}
		}
	}
	return 0;
}

u16 usb_add_crc5 (unsigned char *pD)
{
	int dat = pD[0] + pD[1]*256;
	int crc5 = 0x1f;
	int  i;
    for (i = 0; i < 11; i++) {
        if (((dat>>i) ^ (crc5>>4)) & 1 ) {
        	crc5 = (crc5<<1) ^ 0x5;
        }
        else {
        	crc5 = (crc5<<1);
        }
        //dat >>= 1;
     }
    crc5 = ~crc5;
    pD[1] |= (crc5 & BIT(4)) >> 1;
    pD[1] |= (crc5 & BIT(3)) << 1;
    pD[1] |= (crc5 & BIT(2)) << 3;
    pD[1] |= (crc5 & BIT(1)) << 5;
    pD[1] |= (crc5 & BIT(0)) << 7;
    //crc5 = {crc5[0], crc5[1], crc5[2], crc5[3], crc5[4]};
    return crc5;
}


u16 usb_add_crc16 (unsigned char *pD, int len)
{
	static unsigned short poly[2]={0, 0xa001};              //0x8005 <==> 0xa001
	unsigned short crc = 0xffff;
	int i,j;

	for(j=len; j>0; j--)
	{
		unsigned char ds = *pD++;
		for(i=0; i<8; i++)
		{
			crc = (crc >> 1) ^ poly[(crc ^ ds ) & 1];
			ds = ds >> 1;
		}
	}
	crc = ~crc;
	pD[0] = crc;
	pD[1] = crc >> 8;
	return crc;
}

void usb_send_sof () {
	int len = usb_get_tx_dat ((u8 *) & token_sof, 4, buff_tx);
	usb_tx_rx(buff_tx, len);
	token_sof += 0x10000;
	usb_add_crc5 (((u8 *) & token_sof) + 2);
}

void usb_uart (int n) {
	if (n < 8)		return;

	reg_usb_ep8_dat = n;
	u8 *p = (u8 *)buff_rx;
	//for (int l=0; l<14; l++) {
	for (int l=0; l<(n>>3); l++) {
		reg_usb_ep8_dat = p[l];
	}
	reg_usb_ep_ctrl(0) = FLD_USB_EP_EOF_ISO;
}

int	usb_ctrl_get (u8 * pcmd) {
	//setup phase
	int len = usb_get_tx_dat (pcmd, 17, buff_tx);
	usb_tx_rx(buff_tx, len);

	//data phase
	int len_tx = usb_get_tx_dat ((u8 *) tbl_token_in, 4, buff_tx);
	for (int j=0; j<32; j++) {
		usb_tx_rx(buff_tx, len_tx);
		len = usb_get_rx_dat ();
		if (len > 16) {
			break;
		}
	}

	// skip status phase
	return len;
}

int	usb_ctrl_get_tlk (u8 * pcmd) {
	//setup phase
	int len = usb_get_tx_dat (pcmd, 17, buff_tx);
	usb_tx_rx(buff_tx, len);

	//data phase
	int len_tx = usb_get_tx_dat ((u8 *) tbl_token_in, 4, buff_tx);
//	int rcvd = 0;
	int pkt = 0;
	for (int j=0; j<128; j++) {
		usb_tx_rx(buff_tx, len_tx);
		len = usb_get_rx_dat ();
		if (len > 16) {
			usb_uart (len);
			pkt++;
			// send ACK after DATA
			enable_ack_after_data (1);
			usb_tx_rx(buff_tx, len_tx);
			enable_ack_after_data (0);
			//usb_tx_rx(buff_ack, 17);
			if (pkt == 3)
				break;
		}

	}

	// skip status phase
	return len;
}

u8	buff_cmd[18] = {0x80, 0x2d, 0x00, 0x10, 0x00, // setup-eop
		0x80, 0xc3, 0x80, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0, 0x0};

int	usb_ctrl_read_regs (int adr, int n) {
	buff_cmd[1] = 0x2d;		//OUT
	buff_cmd[6] = 0xc3;		//data0
	buff_cmd[7] = 0xc1;
	buff_cmd[8] = 0x02;
	buff_cmd[9] = adr;		//80(sync) - c3(data0) - c1(class read) - 02 (read
	buff_cmd[10] = adr >> 8;		//80(sync) - c3(data0) - c1(class read) - 02 (read
	buff_cmd[11] = buff_cmd[12] = 0;
	buff_cmd[13] = n+8;
	buff_cmd[14] = 0;
	usb_add_crc16 (buff_cmd + 7, 8);

	return usb_ctrl_get (buff_cmd);
}

int	usb_ctrl_write_regs (int adr, int dat, int n) {
	if (n > 4)
		n = 4;
	buff_cmd[6] = 0xc3;		//data0
	buff_cmd[7] = 0x41;
	buff_cmd[8] = 0x02;
	buff_cmd[9] = adr;		//80(sync) - c3(data0) - c1(class read) - 02 (read
	buff_cmd[10] = adr >> 8;		//80(sync) - c3(data0) - c1(class read) - 02 (read)
	buff_cmd[11] = 0;
	buff_cmd[12] = 0;
	buff_cmd[13] = n+8;
	buff_cmd[14] = 0;
	usb_add_crc16 (buff_cmd + 7, 8);

	//setup phase
	int len = usb_get_tx_dat (buff_cmd, 17, buff_tx);
	usb_tx_rx(buff_tx, len);
	//len = usb_get_rx_dat ();
	//usb_uart (len);

	//data1 phase
	buff_cmd[1] = 0xe1;		//OUT
	buff_cmd[6] = 0x4b;		//data1
	buff_cmd[7] = 0x02;
	buff_cmd[8] = adr;
	buff_cmd[9] = adr>>8;
	buff_cmd[10] = buff_cmd[11] = buff_cmd[12] = buff_cmd[13] = 0x0;
	usb_add_crc16 (buff_cmd + 7, 8);
	len = usb_get_tx_dat (buff_cmd, 17, buff_tx);
	usb_tx_rx(buff_tx, len);
	//len = usb_get_rx_dat ();
	//usb_uart (len);
	//return len;

	//data0 phase
	buff_cmd[6] = 0xc3;		//data0
	buff_cmd[7] = dat;
	buff_cmd[8] = dat>>8;
	buff_cmd[9] = dat>>16;
	buff_cmd[10] = dat>>23;
	usb_add_crc16 (buff_cmd + 7, n);
	len = usb_get_tx_dat (buff_cmd, 9 + n, buff_tx);
	usb_tx_rx(buff_tx, len);
	len = usb_get_rx_dat ();
	//usb_uart (len);
	return len;
}

int	usb_ctrl_set (u8 * pcmd, u8 *pdat, int n) {
	//setup phase
	int len = usb_get_tx_dat (pcmd, 17, buff_tx);
	usb_tx_rx(buff_tx, len);

	//data phase
	len = usb_get_tx_dat ((u8 *)tbl_out_dat1, 9+n, buff_tx);
	usb_tx_rx(buff_tx, len);

	len = usb_get_rx_dat ();

	// skip status phase
	return len;
}

int	usb_set_configuration () {
	//setup phase
	int len = usb_get_tx_dat ((u8 *)tbl_set_configuration, 17, buff_tx);
	usb_tx_rx(buff_tx, len);

	//status phase
	int len_tx = usb_get_tx_dat ((u8 *) tbl_token_in, 4, buff_tx);
	enable_ack_after_data (1);
	usb_tx_rx(buff_tx, len_tx);
	enable_ack_after_data (0);
	return len;
}

int	usb_endpoint_in (int adr, int endpoint) {
	u32 token_in = 0x6980 | (adr<<16) | (endpoint<<23);
	usb_add_crc5 (((u8 *) & token_in) + 2);
	int len = usb_get_tx_dat ((u8 *)&token_in, 4, buff_tx);
	usb_tx_rx(buff_tx, len);
	len = usb_get_rx_dat ();
	return len;
}

void main_loop ()
{

}

void user_init ()
{
	u8 areg = analog_read (0x10) & 0x0f;
	//analog_write (0x10,  areg | 0xf0);		//bit[5:4] PD0 pulldown 100K; bit[7:6] PD1 pulldown 100k
	analog_write (0x10,  areg | 0xb0);		//bit[5:4] PD0 pulldown 100K; bit[7:6] PD1 pullup 10k

	reg_gpio_pd_out			= ~BIT(0);
	reg_gpio_pd_ie			= BIT(1);
	reg_gpio_pd_gpio		= 0xff;
	reg_gpio_pd_ds			= 0xff;
	reg_gpio_pd_irq_en		= BIT(1);		// PA1 (DP) interrupt source
	reg_gpio_wakeup_irq		= FLD_GPIO_INTERRUPT_EN;
	//reg_tmr_ctrl 			= FLD_TMR0_EN;

	token_sof = 0xe801a580;
	usb_get_tx_dat ((u8 *) & tbl_token_ack, 2, buff_ack);

	u32		tick = clock_time ();

	while (1) {
		if (clock_time_exceed (tick, 1000)) {
			//tick += 48000;
			tick = clock_time ();

			m_loop++;

			//send SOF
			usb_send_sof ();

			//reg_usb_ep8_dat = pe_i2c_read_byte(1);
			//reg_usb_ep8_dat = pe_i2c_read_byte_4mhz(1);

			int len;

			if ((m_loop & 0x7f) == 0xf) {
				usb_set_configuration ();
				//get device descriptor command
#if 1
				int len = usb_ctrl_get ((u8 *) tbl_get_device_desc);
				usb_uart (len);
#else
				int len = usb_ctrl_get_tlk ((u8 *) tbl_get_device_desc);
#endif
			}
			else if ((m_loop & 0x3) == 0x1) {
				// read data from end-point 1, device address 0
				len = usb_endpoint_in (0, 1);
				if (len > 8) {
					usb_uart (len);
					enable_ack_after_data (1);
					usb_endpoint_in (0, 1);
					enable_ack_after_data (0);
					//usb_tx_rx(buff_ack, 17);
				}
			}
			else if ((m_loop & 0x3) == 0x2) {
				// read data from end-point 2, device address 0
				len = usb_endpoint_in (0, 2);
				if (len > 8) {
					usb_uart (len);
					enable_ack_after_data (1);
					usb_endpoint_in (0, 2);
					enable_ack_after_data (0);
					//usb_tx_rx(buff_ack, 17);
				}
			}
			else if ((m_loop & 0x0) == 0x0e) {	//disabled
				// write registers (Telink usb device only)
				usb_ctrl_write_regs (0, 0xaa55, 2);
				// read registers (Telink usb device only)
				usb_ctrl_read_regs (0x00, 8);
			}
		}
	}
	return 1;
}


#endif
