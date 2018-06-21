
#include "tl_common.h"
#include "drivers.h"

void spi_slave_test_init(void)
{
	spi_slave_init((unsigned char)(CLOCK_SYS_CLOCK_HZ/(2*500000)-1),SPI_MODE0);           //slave mode init

    spi_slave_gpio_set(SPI_GPIO_GROUP_A2A3A4D6);      //slave mode £ºspi pin set
}
