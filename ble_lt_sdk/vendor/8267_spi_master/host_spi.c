#include "../../proj/tl_common.h"
#include "../../proj/drivers/spi_8267.h"

#if(__PROJECT_8267_SPI_MASTER__)
#define			FLAG_TOKEN				0xff

void host_spi_init (int divider)
{
	spi_pin_init_8267(SPI_PIN_GPIOA);
	write_reg8(0x0a,read_reg8(0x0a)|divider);/////spi clock=system clock/((divider+1)*2)
	write_reg8(0x09,read_reg8(0x09)|0x02);////enable master mode
	write_reg8(0x0b,read_reg8(0x0b)|SPI_MODE0);////select SPI mode,surpport four modes
}

/////////////////////////////////////////////////////////////////////////
//		ff 80 00 - w0 w1 w2 w3 .....
/////////////////////////////////////////////////////////////////////////
int host_spi_write (u8 *p, int n)
{

	reg_spi_ctrl = FLD_SPI_MASTER_MODE_EN;		// SPI_CS = 0
#if (SLAVE_SPI_PM_ENABLE)
	gpio_write(MSPI_PM_WAKEUP_PIN,1);
	sleep_us(3000);//2500
#endif
	for (int i=0; i<n+3; i++) {
		// send "ff 80 00" to start write
		reg_spi_data =  i == 0 ? 0xfe :
						i == 1 ? 0x00 ://80
						i == 2 ? 0x00 :  	p[i-3];
		while (reg_spi_ctrl & FLD_SPI_BUSY);
	}
	reg_spi_ctrl = FLD_SPI_CS | FLD_SPI_MASTER_MODE_EN;	// SPI_CS = 1
#if (SLAVE_SPI_PM_ENABLE)
	gpio_write(MSPI_PM_WAKEUP_PIN,0);
#endif
	return n;
}

/////////////////////////////////////////////////////////////////////////
//		ff c0 80 - r0 r1 r2 r3 .....
/////////////////////////////////////////////////////////////////////////
int host_spi_readwrite (u8 *p, u8 *pr, int len)
{
	reg_spi_ctrl = FLD_SPI_MASTER_MODE_EN;		// SPI_CS = 0

	sleep_us (10);

	for (int i=0; i<len ; i++) {

		reg_spi_data =  p[i];

		while (reg_spi_ctrl & FLD_SPI_BUSY);

		*pr ++ = reg_spi_data;

	}

	sleep_us (10);

	reg_spi_ctrl = FLD_SPI_CS | FLD_SPI_MASTER_MODE_EN;	// SPI_CS = 1

	return 0;
}


int host_spi_read (u8 *p)
{
	reg_spi_ctrl = FLD_SPI_MASTER_MODE_EN;		// SPI_CS = 0

	int len = 64;
	for (int i=0; i<64 ; i++) {
		// send "ff c0 80" first to start read
		reg_spi_data =  i == 0 ? 0xfe :
						i == 1 ? 0x60 : 0x80;

		while (reg_spi_ctrl & FLD_SPI_BUSY);
		if (i>2)
		{
			*p ++ = reg_spi_data;
		}
		if (len == 0) {
			reg_spi_ctrl = FLD_SPI_CS | FLD_SPI_MASTER_MODE_EN;	// SPI_CS = 1
			return i - 2;
		}
		if (i==3 && reg_spi_data != FLAG_TOKEN) {
			len = 1;
		}
		else if (i==4 && reg_spi_data!=0 && reg_spi_data<62) {
			len = reg_spi_data;
		}
		len--;
	}

	reg_spi_ctrl = FLD_SPI_CS | FLD_SPI_MASTER_MODE_EN;	// SPI_CS = 1

	return 0;
}
#endif
