#include "../../proj/tl_common.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj_lib/ble/hci/hci.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/service/ble_ll_ota.h"
#include "../../proj/drivers/adc.h"
#include "../../proj_lib/ble/ble_smp.h"
#include "../../proj_lib/ble/ble_phy.h"
#include "../common/blt_soft_timer.h"

#include "../../proj/drivers/uart.h"


#if (__PROJECT_8261_DRIVER_TEST__ || __PROJECT_8266_DRIVER_TEST__ || __PROJECT_8267_DRIVER_TEST__ || __PROJECT_8269_DRIVER_TEST__)

extern void app_i2c_test_init(void);
extern void app_i2c_test_start(void);

extern void app_spi_test_init(void);
extern void app_spi_test_start(void);

extern void app_uart_test_init(void);
extern void app_uart_test_start(void);

extern void app_adc_test_init(void);
extern void app_adc_test_start(void);


void user_init()
{

#if (DRIVER_TEST_MODE == TEST_HW_TIMER)

#elif (DRIVER_TEST_MODE == TEST_GPIO_IRQ)
#elif (DRIVER_TEST_MODE == TEST_UART)

	app_uart_test_init();

#elif (DRIVER_TEST_MODE == TEST_IIC)

	app_i2c_test_init();

#elif (DRIVER_TEST_MODE == TEST_SPI)

	app_spi_test_init();

#elif (DRIVER_TEST_MODE == TEST_ADC)

	app_adc_test_init();

#endif

}


/////////////////////////////////////////////////////////////////////
// main loop flow
/////////////////////////////////////////////////////////////////////
u32 tick_wakeup;
void main_loop (void)
{
#if (DRIVER_TEST_MODE == TEST_UART)

	app_uart_test_start();

#elif (DRIVER_TEST_MODE == TEST_IIC)

	app_i2c_test_start();

#elif (DRIVER_TEST_MODE == TEST_SPI)

	app_spi_test_start();

#elif (DRIVER_TEST_MODE == TEST_ADC)

	app_adc_test_start();

#endif
}




#endif  // end of __PROJECT_826x_DRIVER_TEST__
