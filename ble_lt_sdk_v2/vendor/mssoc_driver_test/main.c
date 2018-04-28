#include "../../proj/tl_common.h"
#include "../../proj/mcu/watchdog_i.h"
#include "../../vendor/common/user_config.h"
#include "../../proj_lib/rf_drv.h"
#include "../../proj_lib/pm.h"
#include "../../proj_lib/ble/blt_config.h"
#include "../../proj_lib/ble/ll/ll.h"
#include "../../proj/drivers/uart.h"

#if (__PROJECT_MSSOC_DRIVER_TEST__)

extern my_fifo_t hci_rx_fifo;
extern void user_init();
extern void main_loop (void);

extern void app_i2c_test_irq_proc(void);
extern void app_uart_test_irq_proc(void);
extern void app_spi_test_irq_proc(void);
extern void app_timer_test_irq_proc(void);
extern void app_gpio_irq_test_proc(void);
extern void app_pwm_irq_test_proc(void);


_attribute_ram_code_ void irq_handler(void)
{

/***timer demo***/
#if (DRIVER_TEST_MODE == TEST_HW_TIMER)
	app_timer_test_irq_proc();

/***uart demo***/
#elif (DRIVER_TEST_MODE == TEST_UART)
	app_uart_test_irq_proc();

/***i2c demo***/
#elif (DRIVER_TEST_MODE == TEST_IIC)
	app_i2c_test_irq_proc();

/***spi demo ***/
#elif (DRIVER_TEST_MODE == TEST_SPI)
	app_spi_test_irq_proc();

#elif (DRIVER_TEST_MODE == TEST_GPIO_IRQ)

	app_gpio_irq_test_proc();

#elif (DRIVER_TEST_MODE == TEST_PWM)

	app_pwm_irq_test_proc();

#endif

}

int main (void) {

	cpu_wakeup_init();

	clock_init();

	rf_drv_init(CRYSTAL_TYPE);

	gpio_init();

	user_init();

	while (1) {
#if (MODULE_WATCHDOG_ENABLE)
		wd_clear(); //clear watch dog
#endif
		main_loop ();
	}
}




#endif   //end of __PROJECT_ MSSOC_DRIVER_TEST__
