#include "tl_common.h"
#include "drivers.h"




extern void spi_master_test_init(void);
extern void spi_slave_test_init(void);
extern void spi_master_mainloop(void);




void app_spi_test_init(void)
{
	WaitMs(2000);  //leave enough time for SWS_reset when power on
	//SPI:CK/CN/DO/DI   A4/D6/A2/A3, D7/D2/B7/B6
	#if (SPI_MODE==SPI_MASTER_MODE)
	 spi_master_test_init();

	#else
	 spi_slave_test_init();
	#endif
}






void app_spi_test_start(void)
{
#if (SPI_MODE==SPI_MASTER_MODE)

	spi_master_mainloop();

#else
		WaitMs(50);
#endif
}


void app_spi_test_irq_proc(void)
{

}
