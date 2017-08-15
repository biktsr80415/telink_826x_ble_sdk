/*
 * uei_debug.c
 *
 *  Created on: 2017-7-17
 *      Author: Administrator
 */

#include "../../proj/tl_common.h"
#include "../../proj/drivers/uart.h"
#include "../../proj/mcu/gpio.h"
#include "../../proj/mcu/watchdog_i.h"

#define UEI_DEBUG_OPEN     (0)

#if UEI_DEBUG_OPEN

void uei_debug_init()
{
	u8 bwpc = 13;

	u8 div = CLOCK_SYS_CLOCK_1S / 115200 / (bwpc + 1);
	uart_Reset();
	uart_io_init(UART_GPIO_8267_PB2_PB3);
	gpio_set_input_en(GPIO_PB2, 1);
	gpio_set_input_en(GPIO_PB3, 1);
	gpio_setup_up_down_resistor(GPIO_PB2, PM_PIN_PULLUP_1M);
	gpio_setup_up_down_resistor(GPIO_PB3, PM_PIN_PULLUP_1M);
	uart_IRQSourceGet();
	uart_Init(div, bwpc, PARITY_NONE, STOP_BIT_ONE);
	uart_DmaModeInit(1, 1);
	sleep_us(1000);
}

void uei_debug_print(void *buf, size_t len)
{
	u32 dbg_buf[3] = {0x08, 0x00, 0x00};
	u8 max_len = dbg_buf[0];
	if (len > max_len)
		len = max_len;
	memcpy(dbg_buf + 1, buf, len);
	dbg_buf[0] = len;
	uart_Send_kma((u8 *)dbg_buf);
	wd_stop();
	sleep_us(len * 8 * 140);
	wd_start();
	dbg_buf[0] = 0x08;
	memset(dbg_buf + 1, 0, dbg_buf[0]);
}

void uei_debug_print_hex(u8 hex)
{
	uei_debug_print(&hex, sizeof(hex));
}

#endif
